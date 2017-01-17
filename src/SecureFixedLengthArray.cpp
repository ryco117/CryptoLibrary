#include "SecureFixedLengthArray.h"

#include <stdexcept>
#include <assert.h>

//POSIX
#include <sys/mman.h>
#include <string.h>


SecureFixedLengthArray::SecureFixedLengthArray(const unsigned int argSize) :
	size(argSize), ptr(nullptr)
{
	if(size > 0)
	{
		uint8_t* cptr = new uint8_t[size];
		if(int err = mlock(cptr, size) != 0)
		{
			delete[] cptr;
			throw std::runtime_error("mlock failed with return value: " +
				std::to_string(err));
		}

		ptr.reset(cptr);
		cptr = nullptr;
		Zero();
	}
}

SecureFixedLengthArray::SecureFixedLengthArray(const SecureFixedLengthArray& array) :
	SecureFixedLengthArray(array.Size())
{
	if(size > 0)
	{
		memcpy(ptr.get(), array.GetConst(), size);
	}
}

SecureFixedLengthArray::~SecureFixedLengthArray()
{
	Zero();
	if(int err = munlock(ptr.get(), size) != 0)
	{
		throw std::runtime_error("munlock failed with return value: " +
			std::to_string(err));
	}
}

SecureFixedLengthArray& SecureFixedLengthArray::operator= (SecureFixedLengthArray& rhs)
{
	if(size != rhs.Size())
		throw std::runtime_error("SecureFixedLengthArray: Cannot use assignment operator for arrays of different lengths");

	memcpy(ptr.get(), rhs.Get(), size);
	return *this;
}

uint8_t& SecureFixedLengthArray::operator[] (unsigned int index)
{
	return ptr[index];
}

uint8_t* SecureFixedLengthArray::Get()
{
	return ptr.get();
}

const uint8_t* SecureFixedLengthArray::GetConst() const
{
	return ptr.get();
}

const unsigned int SecureFixedLengthArray::Size() const
{
	return size;
}

void SecureFixedLengthArray::Zero()
{
	if(ptr != nullptr)
		memset(ptr.get(), 0, size);
}
