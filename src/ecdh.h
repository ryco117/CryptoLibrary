/*#pragma once
extern "C"
{
	#include "curve25519-donna.h"
}
#include "Fortuna.h"
#include "SecureArray.h"

#include <array>

namespace Curve25519
{
	const std::array<uint8_t, 32> Base = {
		9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	void CreateKeyPair(std::array<uint8_t, 32>& publicKey, SecureArray<32>& privateKey, FortunaPRNG& fprng);
	void CreateSharedKey();
}*/
