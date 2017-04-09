#pragma once
#include "SecureArray.h"
#include <array>

class FortunaPRNG
{
public:
	FortunaPRNG();

	// Seed generator with bytes from seed buffer of size 'len'
	void Seed(const unsigned char* seed, unsigned int len);

	// Generate 'n' 16byte blocks and store the result contiguously at 'out'
	void GenerateBlocks(unsigned char* out, unsigned int n);

	// Return new bit from random pool
	bool Decision();

private:
	// Increment the internal counter (plaintext of AES random output)
	void CountInc();

	// AES key used to generate 16byte blocks (different per call to GenerateBlocks)
	SecureArray<32> key;

	// AES input with very long cycle period (never same per instantiation)
	std::array<uint8_t, 16> counter;

	// 16byte (128bit) block used one bit at a time from a randomly generated block (newly generated every 128 Decision() calls)
	SecureArray<16> decisions;
	uint8_t decisionIndex;

	// lazy check if counter is zero (uninitialized)
	std::array<uint8_t, 16> zeroBlk;
};
