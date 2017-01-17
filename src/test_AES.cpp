#include <AES.h>
#include <Base64.h>
#include <iostream>
#include <string>
#include <string.h>

using namespace AES;

int main()
{
	std::array<uint8_t, 16> IV = {0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0};
	uint8_t keyBytes[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
	SecureArray<32> key;
	memcpy(key.Get(), keyBytes, 32);
	std::string Message = "Hello there world! This tests arbitrarily sized plaintext! Did it work?!?aaaaa";
	uint8_t buff[128] = {0};

	if(!AES_NI())
	{
		std::cout << "AES-NI is not supported on this CPU :(\n";
		return -1;
	}

	const bool pad = true;
	const unsigned int cipherLength = pad ? PaddedSize(Message.size()) :
		(PaddedSize(Message.size()) - Message.size() == 16 ? Message.size() : PaddedSize(Message.size()) );
	std::cout << "Plaintext: " << Message << std::endl;
	Encrypt(reinterpret_cast<const uint8_t*>(Message.c_str()), Message.size(), IV, key, buff, pad);
	std::cout << "Encrypted: " << Base64::Encode(buff, cipherLength) << std::endl;

	int len = Decrypt(buff, cipherLength, IV, key, buff, pad);
	std::cout << "Decrypted: " << buff << std::endl;
	std::cout << "String Length: " << Message.size() << ", " << len << std::endl;
	return 0;
}
