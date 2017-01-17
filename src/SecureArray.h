#pragma once
#include "SecureFixedLengthArray.h"

template<unsigned int arraySize>
class SecureArray
{
public:
	SecureArray() : data(arraySize) {}

	void Zero()
	{
		data.Zero();
	}

	unsigned int Size() const
	{
		return arraySize;
	}

	uint8_t& operator[](unsigned int i)
	{
		return data[i];
	}

	uint8_t* Get()
	{
		return data.Get();
	}
	const uint8_t* GetConst() const
	{
		return data.GetConst();
	}

private:
	SecureFixedLengthArray data;
};
