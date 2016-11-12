#pragma once
#include "SecureFixedLengthArray.h"

template<unsigned int arraySize>
class SecureArray : public SecureFixedLengthArray
{
public:
	SecureArray() : SecureFixedLengthArray(arraySize) {};
};
