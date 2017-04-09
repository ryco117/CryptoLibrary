#pragma once
#include "SecureString.h"
#include "SecureArray.h"

#include <array>
#include <string>

namespace AES
{
	// Returns the lowest multiple of 16 that is greather than 'size'
	// (ie the space required to store an encrypted and padded plaintext of size)
	unsigned int PaddedSize(unsigned int size);

	// Encrypt/Decrypt data at plaintext/ciphertext buffer of size 'length' (using 128bit iv and 256bit key),
	// and write result to out buffer (can be same as src buffer, since it writes 1 block at a time starting from lowest address)
	// Last arg controls whether to pad extra bytes in last plaintext 16byte block with zeros (false) or with the value 'PaddedSize(length)-length', a number in [1, 16] (true)
	// 		If decrypting, padding controls whether to check for valid decryption (~1/256 failrate) through padding mechanism, and will return the same size used to encrypt the plaintext on success (true)
	//		or performs no validation of correct decryption and returns the input 'length' (false)
	//
	// Decrypt throws std::exception on padding check fail
	void Encrypt(const uint8_t* plaintext, const unsigned int length, const std::array<uint8_t, 16>& iv, const SecureArray<32>& key, uint8_t* ciphertextOut, bool usePKCS7Padding = true);
	unsigned int Decrypt(const uint8_t* ciphertext, const unsigned int length, const std::array<uint8_t, 16>& iv, const SecureArray<32>& key, uint8_t* plainTextOut, bool expectPKCS7Padding = true);

	// Uses CPUID instruction to determine if CPU cupports AES-NI instruction set
	bool AES_NI();
};
