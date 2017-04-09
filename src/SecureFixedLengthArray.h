#pragma once
#include <memory>
#include <stdint.h>

class SecureFixedLengthArray
{
public:
	// Create a managed buffer of constant size 'size' that cannot be swapped to disk
	SecureFixedLengthArray(const unsigned int size);

	// Ceate a deep copy of input array
	SecureFixedLengthArray(const SecureFixedLengthArray& array);
	SecureFixedLengthArray() = delete;

	// If size, free disk-swap lock on memory and zero
	~SecureFixedLengthArray();

	// If same size copy contents from input, otherwise throw exception
	SecureFixedLengthArray& operator= (SecureFixedLengthArray&);

	// 'ptr' element access
	uint8_t& operator[] (unsigned int index);

	// 'ptr->get()'
	uint8_t* Get();
	const uint8_t* GetConst() const;
	const unsigned int Size() const;

	// Zero all bytes in buffer (size remains static)
	void Zero();

private:
	const unsigned int size;
	std::unique_ptr<uint8_t[]> ptr;
};
