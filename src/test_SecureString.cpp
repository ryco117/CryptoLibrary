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

	cout << (str == SecureString("Hello! W0RLD!!\tD:") ? "Equivalence: Pass" : "Equivalence: Fail") << endl;
//	cout << SecureString(0).GetLength() << endl;
	cout << (SecureString() == SecureString("") ? "Empty Equivalence: Pass" : "Empty Equivalence: Fail") << endl;
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

	cout << str.GetStr() << endl << str.GetLength() << " / " << str.GetBufferLength() << endl;
	return 0;
}
