#include "SecureString.h"

#include <cstdlib>
#include <string.h>
#include <stdexcept>

SecureString::SecureString() :
	length(0), arrayPtr(new SecureFixedLengthArray(16)) {}

SecureString::SecureString(const char* cstr) :
	SecureString(cstr, strlen(cstr)) {}

SecureString::SecureString(const void* ptr, unsigned int len) :
	SecureString()
{
	if(ptr != nullptr && len != 0)
	{
		ResizeToFit(len);
		uint8_t* array = GetStr();
		memcpy(array, ptr, len);
		array[len] = 0;
		length = len;
	}
}

SecureString::SecureString(const SecureString& str) :
	SecureString(str.GetStr(), str.GetLength()) {}

SecureString::~SecureString()
{
	Clear();
}

void SecureString::Append(const SecureString& str)
{
	ResizeToFit(length + str.GetLength());
	uint8_t* array = GetStr();
	memcpy(&array[length], str.GetStr(), str.GetLength());
	length += str.GetLength();
	array[length] = 0;
}

void SecureString::Append(char& c)
{
	ResizeToFit(length + 1);
	GetStr()[length++] = c;
	GetStr()[length] = 0;
	c = 0;
}

void SecureString::ReplaceAt(unsigned int index, SecureString str)
{
	if(index > length)
		throw std::runtime_error("Index out of bounds.");

	unsigned int newLength = std::max(length, index + str.GetLength());
	ResizeToFit(newLength);
	memcpy(&GetStr()[index], str.GetStr(), str.GetLength());
	length = newLength;
	GetStr()[length] = 0;
}

void SecureString::PullFrom(SecureString& str)
{
	Clear();
	SwitchWith(str);
}

SecureString SecureString::SubStr(unsigned int index, unsigned int len) const
{
	if(index > length)
		throw std::runtime_error("Index out of bounds.");

	if(index + len > length)
		len = length - index;

	return SecureString(&(*arrayPtr).Get()[index], len);
}

bool SecureString::Equals(const SecureString& str)
{
	if(GetStr() == str.GetStr())
		return true;

	if(str.GetLength() == GetLength() &&
		memcmp(str.GetStr(), GetStr(), GetLength()) == 0)
		return true;
	else
		return false;
}

uint8_t SecureString::AtIndex(unsigned int index) const
{
	if(index >= GetBufferLength())
		throw std::runtime_error("Index out of bounds.");

	return GetStr()[index];
}

void SecureString::Clear()
{
	SafeAllocate(0);
}

bool SecureString::Empty() const
{
	return (length == 0);
}

unsigned int SecureString::GetLength() const
{
	return length;
}

unsigned int SecureString::GetBufferLength() const
{
	if(arrayPtr == nullptr)
		return 0;
	else
		return arrayPtr->Size();
}

const uint8_t* SecureString::GetStr() const
{
	if(arrayPtr == nullptr)
		return nullptr;
	else
		return arrayPtr->Get();
}

uint8_t* SecureString::GetStr()
{
	if(arrayPtr == nullptr)
		return nullptr;
	else
		return arrayPtr->Get();
}

SecureString& SecureString::operator<< (SecureString& str)
{
	PullFrom(str);
	return *this;
}

uint8_t& SecureString::operator[] (unsigned int index)
{
	if(index >= GetBufferLength())
		throw std::runtime_error("Index out of bounds.");

	return GetStr()[index];
}

bool SecureString::operator== (const SecureString& str)
{
	return Equals(str);
}

bool SecureString::operator!= (const SecureString& str)
{
	return !Equals(str);
}

unsigned int SecureString::CalcAllocateSize(unsigned int n)
{
	if(n < 32)
	{
		if(n)
			return 32;
		else
			return 0;
	}

	unsigned int i = 1;
	while(n > 0)
	{
		n >>= 1;
		i <<= 1;
	}

	if(i == 0)
	{
		const unsigned int max = static_cast<unsigned int>(-1);
		if(n == max)
			throw std::runtime_error("Cannot allocate string of length " + std::to_string(max));
		else
			return max;
	}
	else
		return i;
}

void SecureString::SwitchWith(SecureString& str)
{
	unsigned int oldLength = length;
	unsigned int oldStrLength = str.GetLength();
	arrayPtr.swap(str.arrayPtr);
	this->SetLength(oldStrLength);
	str.SetLength(oldLength);
}

void SecureString::SafeAllocate(unsigned int newBufferLength)
{
	if(newBufferLength == GetBufferLength())
		return;

	SecureFixedLengthArray* newArray = new SecureFixedLengthArray(newBufferLength);
	if(newBufferLength != 0)
	{
		length = std::min(length, newBufferLength - 1);
		memcpy((*newArray).Get(), GetStr(), length);
		(*newArray).Get()[length] = 0;
	}
	else
	{
		length = 0;
		arrayPtr.reset(nullptr);
	}
	arrayPtr.reset(newArray);
}

void SecureString::ResizeToFit(unsigned int newLength)
{
	if(newLength < GetBufferLength() && newLength >= GetBufferLength()/3)
		return;

	unsigned int newBufferLength = CalcAllocateSize(newLength);
	SafeAllocate(newBufferLength);
}

void SecureString::SetLength(unsigned int newLength)
{
	if(GetBufferLength() == 0)
	{
		if(newLength != 0)
			throw std::runtime_error("Empty buffer can only have length zero");
		else
			return;
	}

	if(newLength >= GetBufferLength())
		throw std::runtime_error("New length does not allow for null terminator within buffer");

	length = newLength;
	GetStr()[length] = 0;
}
