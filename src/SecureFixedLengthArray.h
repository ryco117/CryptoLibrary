#pragma once
#include <memory>
#include <stdint.h>


class SecureFixedLengthArray
{
public:
	SecureFixedLengthArray() = delete;
	SecureFixedLengthArray(const unsigned int size);
	SecureFixedLengthArray(const SecureFixedLengthArray& array);
	~SecureFixedLengthArray();
	SecureFixedLengthArray& operator= (SecureFixedLengthArray&);
	uint8_t& operator[] (unsigned int index);

	uint8_t* Get();
	const uint8_t* Get() const;
	const unsigned int Size() const;
	void Zero();
private:
	const unsigned int size;
	std::unique_ptr<uint8_t[]> ptr;
};
