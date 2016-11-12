#pragma once
#include <string>

namespace Base64
{
	std::string Encode(const std::string& data);
	std::string Encode(const uint8_t* data, const unsigned int length);
	std::string Decode(const std::string& encodedData);
}
