#pragma once
#include <SecureArray.h>
#include <Fortuna.h>

#include <array>

namespace RLWE
{
	class Polynomial
	{
	public:
		Polynomial();

		static const int scalarModulus = 12289;
		static const int polynomialModulus = 1024;							    // Number of coefficients
		static const unsigned int sharedKeyLength = (polynomialModulus / 4) / 8;// 4 coefficients per key bit, 8 bits per byte
		static const unsigned int reconciliationLength = sharedKeyLength * 8;	// 8 rec. bits per key bit, equivalent to polynomialModulus / 4

        static const unsigned int BINOMIAL_SAMPLE_COUNT = 16;                 // 2000 'virtual coin flips' to build binomial distribution

		static Polynomial RandomPolynomial(FortunaPRNG& fprng);
		static Polynomial ErrorPolynomial(FortunaPRNG& fprng);

		void Reconcile(
			const std::array<uint8_t, reconciliationLength>& reconciliation,
			SecureArray<sharedKeyLength>& keyOut,
			FortunaPRNG& fprng) const;
		void CreateSchemeAndReconcile(
			std::array<uint8_t, reconciliationLength>& reconciliationOut,
			SecureArray<sharedKeyLength>& keyOut,
			FortunaPRNG& fprng) const;
		uint16_t At(unsigned int exponent) const;
		void Print() const;

		Polynomial operator+(const Polynomial&) const;
	  	Polynomial operator-(const Polynomial&) const;
		Polynomial& operator+=(const Polynomial&);
		Polynomial operator*(const Polynomial&) const;
		Polynomial& operator*=(const Polynomial&);
		uint16_t& operator[](unsigned int exponent);
		bool operator==(const Polynomial& rhs) const;

	protected:
		SecureArray<polynomialModulus * sizeof(uint16_t)> coefficients;
	};

	Polynomial GeneratePublicKey(
		const Polynomial& privateKey,
		const Polynomial& basePolynomial,
		FortunaPRNG& fprng);

	void CreateSharedKey(
		const Polynomial& privateKey,
		const Polynomial& publicKey,
		const std::array<uint8_t, Polynomial::reconciliationLength>& reconciliation,
		SecureArray<Polynomial::sharedKeyLength>& sharedKeyOut,
		FortunaPRNG& fprng);

	void CreateSharedKeyAndReconciliation(
		const Polynomial& privateKey,
		const Polynomial& publicKey,
		std::array<uint8_t, Polynomial::reconciliationLength>& reconciliationOut,
		SecureArray<Polynomial::sharedKeyLength>& sharedKeyOut,
		FortunaPRNG& fprng);
}
