#ifndef FORTUNA_CPP
#define FORTUNA_CPP
#include "Fortuna.h"

#include <string.h>
#include <stdlib.h>
#include <stdexcept>
extern "C"
{
	#include <libscrypt.h>
}
#include "AES.h"

FortunaPRNG::FortunaPRNG()
{
	memset(key.Get(), 0, key.Size());
	memset(counter.data(), 0, counter.size());
	memset(zeroBlk.data(), 0, zeroBlk.size());
}

void FortunaPRNG::Seed(const unsigned  char* seed, unsigned int len)
{
	libscrypt_scrypt(key.Get(), key.Size(), seed, len, 16384, 8, 1, key.Get(), key.Size());
	CountInc();
	GenerateBlocks(decisions.Get(), 1);
	decisionIndex = 0;
	return;
}

void FortunaPRNG::GenerateBlocks(unsigned char* out, unsigned int n)
{
	if(memcmp(counter.data(), zeroBlk.data(), 16) == 0)
		throw std::runtime_error("Generator Not Initialized!");
	if(n == 0)
		return;
	if(n > 65536)
		throw "Size is too large!";

	for(unsigned int i = 0; i < n; i++)
	{
		AES::Encrypt(counter.data(), counter.size(), zeroBlk, key, &out[i*16], false);
		CountInc();
	}

	//Generate a new key
	uint8_t newKey[32];
	AES::Encrypt(counter.data(), counter.size(), zeroBlk, key, newKey, true);
	memcpy(key.Get(), newKey, 32);
	CountInc();
	memset(newKey, 0, 32);
	return;
}

bool FortunaPRNG::Decision()
{
	const bool r = (decisions[decisionIndex / 8] >> (decisionIndex % 8)) % 2;
	decisionIndex++;
	if(decisionIndex == 128)
	{
		GenerateBlocks(decisions.Get(), 1);
		decisionIndex = 0;
	}

	return r;
}

void FortunaPRNG::CountInc()
{
	unsigned char i = 0;
	while(true)
	{
		counter[i] += 1;
		if(counter[i] == 0 && ++i < counter.size())	//Continue loop if carry and wasn't last byte
			continue;

		break;
	}
	return;
}
#endif
