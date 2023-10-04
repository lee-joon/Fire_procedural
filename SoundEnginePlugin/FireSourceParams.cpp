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

#include "FireSourceParams.h"

#include <AK/Tools/Common/AkBankReadHelpers.h>

FireSourceParams::FireSourceParams()
{
}

FireSourceParams::~FireSourceParams()
{
}

FireSourceParams::FireSourceParams(const FireSourceParams& in_rParams)
{
    RTPC = in_rParams.RTPC;
    NonRTPC = in_rParams.NonRTPC;
    m_paramChangeHandler.SetAllParamChanges();
}

AK::IAkPluginParam* FireSourceParams::Clone(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, FireSourceParams(*this));
}

AKRESULT FireSourceParams::Init(AK::IAkPluginMemAlloc* in_pAllocator, const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    if (in_ulBlockSize == 0)
    {
        // Initialize default parameters here
        RTPC.fDuration  = 0.0f;

        RTPC.fHissLP = 10.0f;
        RTPC.fHissHP = 10000.0f;
        RTPC.fHissDB = 0.0f;
        RTPC.fHissNS = 100.0f;

        RTPC.fCrackFQ = 0.01f;
        RTPC.ICrackLN = 500.0f;
        RTPC.fCrackDB = 0.0f;

        RTPC.fLappBP = 30.0f;
        RTPC.fLappBQ = 50.0f;
        RTPC.fLappDB = 0.0f;
        RTPC.fLappNS = 10.0f;

        m_paramChangeHandler.SetAllParamChanges();
        return AK_Success;
    }

    return SetParamsBlock(in_pParamsBlock, in_ulBlockSize);
}

AKRESULT FireSourceParams::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT FireSourceParams::SetParamsBlock(const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    AKRESULT eResult = AK_Success;
    AkUInt8* pParamsBlock = (AkUInt8*)in_pParamsBlock;

    // Read bank data here
    RTPC.fDuration = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);

    RTPC.fHissLP = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fHissHP = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fHissDB = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fHissNS = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
                
    RTPC.fCrackFQ = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.ICrackLN = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fCrackDB = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
                
    RTPC.fLappBP = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fLappBQ = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fLappDB = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fLappNS = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    CHECKBANKDATASIZE(in_ulBlockSize, eResult);
    m_paramChangeHandler.SetAllParamChanges();

    return eResult;
}

AKRESULT FireSourceParams::SetParam(AkPluginParamID in_paramID, const void* in_pValue, AkUInt32 in_ulParamSize)
{
    AKRESULT eResult = AK_Success;

    // Handle parameter change here
    switch (in_paramID)
    {
    case PARAM_DURATION_ID:
        RTPC.fDuration = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_DURATION_ID);
        break;
    case PARAM_HissLP_ID:
        RTPC.fHissLP = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_HissLP_ID);
        break;
    case PARAM_HissHP_ID:
        RTPC.fHissHP = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_HissHP_ID);
        break;
    case PARAM_HissNS_ID:
        RTPC.fHissNS = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_HissNS_ID);
        break;
    case PARAM_HissDB_ID:
        RTPC.fHissDB = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_HissDB_ID);
        break;
    case PARAM_CrackFQ_ID:
        RTPC.fCrackFQ = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_CrackFQ_ID);
        break;
    case PARAM_CrackLN_ID:
        RTPC.ICrackLN = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_CrackLN_ID);
        break;
    case PARAM_CrackDB_ID:
        RTPC.fCrackDB = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_CrackDB_ID);
        break;
    case PARAM_LappBP_ID:
        RTPC.fLappBP = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_LappBP_ID);
        break;
    case PARAM_LappBQ_ID:
        RTPC.fLappBQ = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_LappBQ_ID);
        break;
    case PARAM_LappNS_ID:
        RTPC.fLappNS = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_LappNS_ID);
        break;
    case PARAM_LappDB_ID:
        RTPC.fLappDB = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_LappDB_ID);
        break;
    default:
        eResult = AK_InvalidParameter;
        break;
    }

    return eResult;
}
