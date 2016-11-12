#include <iostream>

#include "SecureString.h"
#include "SecureArray.h"

using namespace std;

int main()
{
	SecureString str("Hello!", 6);
	SecureString cstr(" W0RLD");

	str.Append(cstr);
	char exc = str.AtIndex(5);
	str.Append(SecureString(&exc, 1));
	str.Append(exc);
	str.Append("\t:D");

	str.ReplaceAt(15, "D:");
	str.ReplaceAt(str.GetLength(),
	"\n()()()()()*\
	\n()()()()*()\
	\n()()()*()()\
	\n()()*()()()\
	\n()*()()()()");
	SecureString test("\n*()()()()()");
	SecureString pullFrom;
	pullFrom << test;
	str.Append(pullFrom);

	std::cout << str.GetStr() << std::endl;

	SecureArray<32> key;
	std::cout << key.Size() << std::endl;

	return 0;
}
