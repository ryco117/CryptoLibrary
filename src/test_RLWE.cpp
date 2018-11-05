#include "RLWE.h"
#include "Base64.h"

#include <iostream>
#include <memory>
#include <assert.h>
#include <string.h>

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

	const unsigned int TRIALS_COUNT = 100000;
	float faults = 0.0;

	// Generate initial polynomials
	Polynomial basePolynomial = Polynomial::RandomPolynomial(fprng);
	// cout << "Base Polynomial: ";
	// basePolynomial.Print();

    for(unsigned int i = 0; i < TRIALS_COUNT; i++)
    {
	    Polynomial privateKey1 = Polynomial::ErrorPolynomial(fprng);
	    // cout << "=======================================================================\n\nPrivate Key (1): ";
	    // privateKey1.Print();

	    Polynomial privateKey2 = Polynomial::ErrorPolynomial(fprng);
	    // cout << "=======================================================================\n\nPrivate Key (2): ";
	    // privateKey2.Print();

	    // Generate public keys
	    Polynomial publicKey1 = GeneratePublicKey(privateKey1, basePolynomial, fprng);
	    Polynomial publicKey2 = GeneratePublicKey(privateKey2, basePolynomial, fprng);

        // cout << "=======================================================================\n\nPublic Key (1): ";
	    // publicKey1.Print();
	    // cout << "=======================================================================\n\nPublic Key (2): ";
	    // publicKey2.Print();

	    // Generate shared keys
	    std::array<uint8_t, Polynomial::reconciliationLength> reconciliationScheme;
	    SecureArray<32> sharedKey1;
	    SecureArray<32> sharedKey2;
	    CreateSharedKeyAndReconciliation(privateKey1, publicKey2, reconciliationScheme, sharedKey1, fprng);
	    CreateSharedKey(privateKey2, publicKey1, reconciliationScheme, sharedKey2, fprng);

        // cout << "=======================================================================\n";
	    // cout << "Resulting Shared Key (1): " << Base64::Encode(sharedKey1.Get(), 32) <<
		//     "\nResulting Shared Key (2): " << Base64::Encode(sharedKey2.Get(), 32) << endl;

	    // assert(memcmp(sharedKey1.Get(), sharedKey2.Get(), 32) == 0);
	    faults += (memcmp(sharedKey1.Get(), sharedKey2.Get(), 32) != 0) ? 1.0 : 0.0;
	}
	cout << "Calculated fault percentage after " << TRIALS_COUNT << " trials: " << 100.0*faults/float(TRIALS_COUNT) << "%\n";
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
