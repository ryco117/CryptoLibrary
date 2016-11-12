#pragma once
#include "SecureString.h"
#include "SecureArray.h"

#include <array>
#include <string>

namespace AES
{
	unsigned int PaddedSize(unsigned int size);
	void Encrypt(const uint8_t* plaintext, const unsigned int length, const std::array<uint8_t, 16>& iv, const SecureArray<32>& key, uint8_t* ciphertextOut, bool usePKCS7Padding = true);
	unsigned int Decrypt(const uint8_t* ciphertext, const unsigned int length, const std::array<uint8_t, 16>& iv, const SecureArray<32>& key, uint8_t* plainTextOut, bool expectPKCS7Padding = true);
	bool AES_NI();
};
