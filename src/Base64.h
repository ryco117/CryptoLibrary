#pragma once
#include <string>

namespace Base64
{
	// Return string containing base64 encoding of data using its length/size, not null terminators
	std::string Encode(const std::string& data);
	std::string Encode(const uint8_t* data, const unsigned int length);

	// Return unformatted string from base64 formatted input (throws exception if not valid)
	std::string Decode(const std::string& encodedData);
}
