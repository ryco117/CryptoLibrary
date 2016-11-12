#pragma once
#include "SecureFixedLengthArray.h"

#include <memory>


class SecureString
{
public:
	SecureString();				// Initialize (buffer)length to 0, array at nullptr
  	SecureString(const char* cstr);
 	SecureString(const void* ptr, unsigned int len);
	SecureString(const SecureString& str);
	virtual ~SecureString();

	void Append(const SecureString& str);
	void Append(char& c);		// Append c and zero character
	void ReplaceAt(unsigned int index, SecureString str);
	void PullFrom(SecureString& str);
	SecureString SubStr(unsigned int index, unsigned int len) const;
	uint8_t AtIndex(unsigned int index) const;
	void Clear();
	void ResizeToFit(unsigned int newLength);
	void SetLength(unsigned int newLength);

	bool Empty() const;
  	unsigned int GetLength() const;
  	unsigned int GetBufferLength() const;
	const uint8_t* GetStr() const;
	uint8_t* GetStr();		// Returns nullptr iff GetBufferLength==0

	SecureString& operator<< (SecureString& str);
	uint8_t& operator[] (unsigned int index);
	SecureString& operator= (SecureString) = delete;

protected:
	static unsigned int CalcAllocateSize(unsigned int n);
	void SwitchWith(SecureString& str);
	std::unique_ptr<SecureFixedLengthArray> arrayPtr;

private:
	void SafeAllocate(unsigned int newBufferLength);
	unsigned int length;
};
