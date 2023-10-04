#pragma once
#include <AK/SoundEngine/Common/IAkPlugin.h>
class NoiseGenerator
{
public:
	NoiseGenerator()
	{
		AkReal32 fRandom = (rand() / ((AkReal32)RAND_MAX));
		SetSeed(*(AkReal32*)&fRandom);
	}

	void SetSeed(AkUInt32 Seed)
	{
		noiseSeed = Seed;
	}

	AkReal32 floatValue()
	{
		// Generate the next integer pseudorandom number.
		noiseSeed = noiseSeed * 196314165 + 907633515;

		// Convert the integer to a float, to get a number between 2 and 4.
		AkUInt32 r = (noiseSeed & 0x7FFFFF) + 0x40000000;
		AkReal32 noise = *(AkReal32*)&r;

		// Subtract 3 to get the float into the range [-1, 1].
		return noise - 3.0f;
	}
private:
	AkUInt32 noiseSeed = 0;
};