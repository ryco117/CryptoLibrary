#pragma once
#include "SecureArray.h"
#include <array>

class FortunaPRNG
{
public:
	FortunaPRNG();
	void Seed(const unsigned char* seed, unsigned int len);
	void GenerateBlocks(unsigned char* out, unsigned int n);
	bool Decision();
	void CountInc();
private:
	SecureArray<32> key;
	std::array<uint8_t, 16> counter;
	SecureArray<16> decisions;
	uint8_t decisionIndex;
	std::array<uint8_t, 16> zeroBlk;
};
