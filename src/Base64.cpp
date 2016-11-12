#include "Base64.h"
#include <map>

namespace
{
	const char BaseTable[] =
	{
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/'
	};

	const std::map<char, unsigned int> DecodeMap =
	{
		{'A', 0}, {'B', 1}, {'C', 2}, {'D', 3}, {'E', 4}, {'F', 5}, {'G', 6}, {'H', 7},
		{'I', 8}, {'J', 9}, {'K', 10}, {'L', 11}, {'M', 12}, {'N', 13}, {'O', 14}, {'P', 15},
		{'Q', 16}, {'R', 17}, {'S', 18}, {'T', 19}, {'U', 20}, {'V', 21}, {'W', 22}, {'X', 23},
		{'Y', 24}, {'Z', 25}, {'a', 26}, {'b', 27}, {'c', 28}, {'d', 29}, {'e', 30}, {'f', 31},
		{'g', 32}, {'h', 33}, {'i', 34}, {'j', 35}, {'k', 36}, {'l', 37}, {'m', 38}, {'n', 39},
		{'o', 40}, {'p', 41}, {'q', 42}, {'r', 43}, {'s', 44}, {'t', 45}, {'u', 46}, {'v', 47},
		{'w', 48}, {'x', 49}, {'y', 50}, {'z', 51}, {'0', 52}, {'1', 53}, {'2', 54}, {'3', 55},
		{'4', 56}, {'5', 57}, {'6', 58}, {'7', 59}, {'8', 60}, {'9', 61}, {'+', 62}, {'/', 63}
	};
}

std::string Base64::Encode(const std::string& data)
{
	std::string paddedData = data;
	std::string encoded;
	while(paddedData.size() % 3 != 0)
	{
		paddedData.push_back(0x00);
	}
	const unsigned int equalsCount = paddedData.size() - data.size();
	const unsigned int newLen = (paddedData.size() / 3) * 4;
	encoded.resize(newLen);

	for(unsigned int i = 0; i < newLen / 4; i++)
	{
		unsigned int k = static_cast<unsigned int>(paddedData[i * 3]) << 16;
		k += static_cast<unsigned int>(paddedData[i * 3 + 1]) << 8;
		k += static_cast<unsigned int>(paddedData[i * 3 + 2]);

		encoded[i * 4] = BaseTable[(k & 0xFC0000) >> 18];
		encoded[i * 4 + 1] = BaseTable[(k & 0x3F000) >> 12];
		encoded[i * 4 + 2] = BaseTable[(k & 0xFC0) >> 6];
		encoded[i * 4 + 3] = BaseTable[k & 0x3F];
	}

	switch(equalsCount)
	{
	case 2:
		encoded[newLen - 2] = '=';
	case 1:
		encoded[newLen - 1] = '=';
		break;
	}

	return encoded;
}

std::string Base64::Encode(const uint8_t* data, const unsigned int dataLength)
{
	return Encode(std::string(reinterpret_cast<const char*>(data), dataLength));
}

std::string Base64::Decode(const std::string& encodedData)
{
	std::string data;
	unsigned int equalsCount;
	for(auto it = encodedData.rbegin(); it != encodedData.rend(); it++)
	{
		if(*it == '=')
			equalsCount++;
		else
			break;
	}
	const unsigned int newLenPadded = (encodedData.length() / 4) * 3;
	data.resize(newLenPadded);

	for(unsigned int i = 0; i < encodedData.length() / 4; i++)
	{
		unsigned int k = DecodeMap.at(encodedData[i * 4]) << 18;
		k += DecodeMap.at(encodedData[i * 4 + 1]) << 12;
		k += DecodeMap.at(encodedData[i * 4 + 2]) << 6;
		k += DecodeMap.at(encodedData[i * 4 + 3]);

		data[i * 3] = static_cast<char>((k & 0xFF0000) >> 16);
		data[i * 3 + 1] = static_cast<char>((k & 0xFF00) >> 8);
		data[i * 3 + 2] = static_cast<char>(k & 0xFF);
	}

	switch(equalsCount)
	{
	case 2:
		data.pop_back();
	case 1:
		data.pop_back();
		break;
	}

	return data;
}
