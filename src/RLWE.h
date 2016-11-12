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

		static const unsigned int scalarModulus = 12289;
		static const unsigned int polynomialModulus = 1024;
		static const unsigned int reconciliationLength = polynomialModulus / 8;

		static Polynomial RandomPolynomial(FortunaPRNG& fprng);
		static Polynomial ErrorPolynomial(FortunaPRNG& fprng);

		void Reconcile(
			const std::array<uint8_t, reconciliationLength>& scheme,
			SecureArray<reconciliationLength>& reconciliationOut,
			FortunaPRNG& fprng) const;
		void CreateSchemeAndReconcile(
			std::array<uint8_t, reconciliationLength>& schemeOut,
			SecureArray<reconciliationLength>& reconciliationOut,
			FortunaPRNG& fprng) const;
		uint16_t At(unsigned int exponent) const;
		void Print() const;
		uint16_t AverageDistance() const;

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
		const std::array<uint8_t, 128>& scheme,
		SecureArray<Polynomial::reconciliationLength>& reconciliationOut,
		FortunaPRNG& fprng);

	void CreateSharedKeyAndReconciliation(
		const Polynomial& privateKey,
		const Polynomial& publicKey,
		std::array<uint8_t, 128>& schemeOut,
		SecureArray<Polynomial::reconciliationLength>& reconciliationOut,
		FortunaPRNG& fprng);
}
