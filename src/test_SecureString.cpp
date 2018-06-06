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

	cout << "Equivalence: " << (str == SecureString("Hello! W0RLD!!\tD:") ? "Pass" : "Fail") << endl;
	cout << "Empty Equivalence: " << (SecureString() == SecureString("") ? "Pass" : "Fail") << endl;
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

	cout << endl << "String: " << str.GetStr() << endl << "String-length to allocated buffer: " << str.GetLength() << " / " << str.GetBufferLength() << endl;
	return 0;
}
