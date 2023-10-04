/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

#include "FireSource.h"
#include "../FireConfig.h"

#include <AK/AkWwiseSDKVersion.h>

AK::IAkPlugin* CreateFireSource(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, FireSource());
}

AK::IAkPluginParam* CreateFireSourceParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, FireSourceParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(FireSource, AkPluginTypeSource, FireConfig::CompanyID, FireConfig::PluginID)

FireSource::FireSource()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

FireSource::~FireSource()
{
}

AKRESULT FireSource::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkSourcePluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (FireSourceParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;
    
    FireFilter::SampleRate                  = static_cast<AkReal32>(in_rFormat.uSampleRate);

    LowpassFilter.CutoffFrequence           = m_pParams->RTPC.fHissLP;
    LowpassFilter.Q                         = 0.707f;

    HighpassFilter.CutoffFrequence          = m_pParams->RTPC.fHissHP;
    HighpassFilter.Q                        = 0.707f;

    Cracking_BandpassFilter.CutoffFrequence = 15000.0f;
    Cracking_BandpassFilter.Q               = 1.0f;

    CrackingCount                           = static_cast<AkInt32>(m_pParams->RTPC.ICrackLN);

    LappingBandpassFilter.CutoffFrequence   = m_pParams->RTPC.fLappBP;
    LappingBandpassFilter.Q                 = 100.0f;

    RemoveDCOffset.CutoffFrequence          = 25.0f;
    RemoveDCOffset.Q                        = 0.707f;


    m_durationHandler.Setup(m_pParams->RTPC.fDuration, in_pContext->GetNumLoops(), in_rFormat.uSampleRate);

    return AK_Success;
}

AKRESULT FireSource::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT FireSource::Reset()
{
    return AK_Success;
}

AKRESULT FireSource::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeSource;
    out_rPluginInfo.bIsInPlace = true;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void FireSource::Execute(AkAudioBuffer* out_pBuffer)
{
    m_durationHandler.SetDuration(m_pParams->RTPC.fDuration);
    m_durationHandler.ProduceBuffer(out_pBuffer);

    const AkUInt32 uNumChannels = out_pBuffer->NumChannels();

    AkUInt16 uFramesProduced;
    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)out_pBuffer->GetChannel(i);

        uFramesProduced = 0;
        while (uFramesProduced < out_pBuffer->uValidFrames)
        {
            AkReal32 NoiseValue      = Noise.floatValue();
            // Generate output here
            //////////////////////////////FIRE___HISS//////////////////////////////
            LowpassFilter.CutoffFrequence = m_pParams->RTPC.fHissLP;
            AkReal32 LowSample      = LowpassFilter.LowpassFilter(NoiseValue);
            LowSample               = powf(LowSample, 2.0f) * m_pParams->RTPC.fHissNS;

            HighpassFilter.CutoffFrequence = m_pParams->RTPC.fHissHP;
            AkReal32 HighSample     = HighpassFilter.HighpassFilter(NoiseValue);
            AkReal32 HissOutSample  = (HighSample * LowSample) * dbToLinear.dBToLinear(m_pParams->RTPC.fHissDB);

            //////////////////////////////FIRE___HISS////////////////////////////
            //                                                                 ||
            //                                                                 ||
            //                                                                 ||
            //////////////////////////////FIRE_CRACKING//////////////////////////
            AkReal32 CrackingSample = 0.0f;
            if (!IsCrackingPlay)
            {
                if ((NoiseValue > 0.5f) && (NoiseValue < (0.5f + m_pParams->RTPC.fCrackFQ * 0.001f)))
                {
                    IsCrackingPlay = true;
                    Cracking_BandpassFilter.CutoffFrequence = abs(Noise.floatValue()) * 15000.0f + 1500.0f;
                }
                    
            }
            else
            {
                CrackingSample = NoiseValue * (AkReal32)CrackingCount / m_pParams->RTPC.ICrackLN;
                CrackingSample = Cracking_BandpassFilter.BandPassFilter(CrackingSample) * dbToLinear.dBToLinear(m_pParams->RTPC.fCrackDB);
                --CrackingCount;
                if (CrackingCount == 0)
                {
                    IsCrackingPlay = false;
                    CrackingCount = (AkInt32)m_pParams->RTPC.ICrackLN;
                }
            }

            //////////////////////////////FIRE_CRACKING//////////////////////////
            //                                                                 ||
            //                                                                 ||
            //                                                                 ||
            //////////////////////////////FIRE_Lapping///////////////////////////

            LappingBandpassFilter.CutoffFrequence = m_pParams->RTPC.fLappBP;
            LappingBandpassFilter.Q = m_pParams->RTPC.fLappBQ;
                                                                                                                                                          
            AkReal32 LappingSample = LappingBandpassFilter.BandPassFilter(NoiseValue) * m_pParams->RTPC.fLappNS;
            LappingSample = std::clamp(RemoveDCOffset.HighpassFilter(LappingSample), -0.9f, 0.9f);
            LappingSample = RemoveDCOffset.HighpassFilter(LappingSample) * dbToLinear.dBToLinear(m_pParams->RTPC.fLappDB);;

            //////////////////////////////FIRE_Lapping///////////////////////////
            *pBuf++ = LappingSample + CrackingSample + HissOutSample;
            ++uFramesProduced;
        }
    }
}

AkReal32 FireSource::GetDuration() const
{
    return m_durationHandler.GetDuration() * 1000.0f;
}
