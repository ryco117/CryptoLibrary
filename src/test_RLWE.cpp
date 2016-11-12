#include "RLWE.h"
#include "Base64.h"

#include <iostream>
#include <memory>

using namespace std;
using namespace RLWE;

std::unique_ptr<uint8_t[]> GetSeed(unsigned int n);

int main()
{
	// Randomness setup
	const unsigned int seedLength = 20;
	unique_ptr<uint8_t[]> seed = GetSeed(seedLength);
	FortunaPRNG fprng;
	fprng.Seed(seed.get(), seedLength);

	// Generate initial polynomials
	Polynomial basePolynomial = Polynomial::RandomPolynomial(fprng);
	Polynomial privateKey1 = Polynomial::RandomPolynomial(fprng);
	Polynomial privateKey2 = Polynomial::RandomPolynomial(fprng);

	// Generate public keys
	Polynomial publicKey1 = GeneratePublicKey(privateKey1, basePolynomial, fprng);
	Polynomial publicKey2 = GeneratePublicKey(privateKey2, basePolynomial, fprng);

	//publicKey1.Print();
	//cout << "=======================================================================\n";

	/*cout << "PublicKey1 * PrivateKey2 equals PublicKey2 * PrivateKey1? " <<
		((publicKey1 * privateKey2) == (publicKey2 * privateKey1)) << endl;*/

	/*cout << "PublicKey1 * PrivateKey2 equals PrivateKey2 * PublicKey1? " <<
		((publicKey1 * privateKey2) == (privateKey2 * publicKey1)) << endl;*/		//true

	//(publicKey1 * privateKey2).Print();
	//cout << "=======================================================================\n";
	//(publicKey2 * privateKey1).Print();

	// Generate shared keys
	std::array<uint8_t, Polynomial::reconciliationLength> reconciliationScheme;
	SecureArray<128> sharedKey1;
	SecureArray<128> sharedKey2;
	CreateSharedKeyAndReconciliation(privateKey1, publicKey2, reconciliationScheme, sharedKey1, fprng);
	cout << "=======================================================================\n";
	CreateSharedKey(privateKey2, publicKey1, reconciliationScheme, sharedKey2, fprng);

	cout << Base64::Encode(sharedKey1.Get(), 128) << "\n" <<
		Base64::Encode(sharedKey2.Get(), 128) << "\n";

	//Polynomial random = Polynomial::RandomPolynomial(fprng);
	//Polynomial poly; poly[0] = 1234; poly[1] = 8324;
	//Polynomial poly = random;
	//Polynomial error = Polynomial::ErrorPolynomial(fprng);
	//Polynomial error; error.Print();
	//Polynomial result = (poly * poly) - (poly * (poly + error)) + (poly * error);
	//result.Print();
	//cout << "Average: " << result.AverageDistance() << endl;
}


std::unique_ptr<uint8_t[]> GetSeed(unsigned int n)
{
	//Properly Seed
	std::unique_ptr<uint8_t[]> seed(new uint8_t[n]);
	#ifdef WINDOWS
		RtlGenRandom(seed, n);
	#else
		FILE* random;
		random = fopen ("/dev/urandom", "r");		//Unix provides it, why not use it
		if(random == NULL)
		{
			fprintf(stderr, "Cannot open /dev/urandom!\n");
			return nullptr;
		}
		fread(seed.get(), 1, n, random);
		fclose(random);
	#endif

	return seed;
}
