#include "RLWE.h"
#include "Fortuna.h"

#include <string.h>
#include <iostream>

namespace
{
	uint16_t randDbl(uint16_t x, FortunaPRNG& fprng)
	{
		return 2 * x + fprng.Decision();
	}

	// round: Z_2q -> Z_2
	uint8_t round(uint16_t x)
	{
		const int q = RLWE::Polynomial::scalarModulus;
		return static_cast<uint8_t>((2 * x + q / 2) / q) % 2;
	}

	// round_2: Z_q -> Z_2
	uint8_t round_2(uint16_t x, FortunaPRNG& fprng)
	{
		return round(randDbl(x, fprng));
	}

	// crossround: Z_2q -> Z_2
	uint8_t crossround(uint16_t x)
	{
		const int q = RLWE::Polynomial::scalarModulus;
		return static_cast<uint8_t>(4 * x / q) % 2;
	}

	// crossround_2: Z_q -> Z_2
	uint8_t crossround_2(uint16_t x, FortunaPRNG& fprng)
	{
		return crossround(randDbl(x, fprng));
	}
}

namespace RLWE
{
	Polynomial Polynomial::RandomPolynomial(FortunaPRNG& fprng)
	{
		Polynomial poly;
		fprng.GenerateBlocks(poly.coefficients.Get(), polynomialModulus/8);

		for(unsigned int i = 0; i < polynomialModulus; i++)
		{
			poly[i] %= scalarModulus;
		}
		return std::move(poly);
	}

	Polynomial Polynomial::ErrorPolynomial(FortunaPRNG& fprng)
	{
		Polynomial error;
		for(unsigned int i = 0; i < polynomialModulus; i++)
		{
			uint16_t j = 0;
			while(fprng.Decision() && j < 12){j++;}

			if(j != 0)
				error[i] = fprng.Decision() ? j : scalarModulus - j;
		}
		return std::move(error);
	}

	void Polynomial::Reconcile(
		const std::array<uint8_t, Polynomial::reconciliationLength>& scheme,
		SecureArray<Polynomial::reconciliationLength>& reconciliationOut,
		FortunaPRNG& fprng) const
	{
		reconciliationOut.Zero();
		for(unsigned int c = 0; c < polynomialModulus; c++)
		{
			uint16_t coefficient = At(c);
			if((scheme[c / 8] >> (c % 8)) % 2)
			{
				std::cout << coefficient << " - Scheme 1 - ";
				if(round((coefficient + scalarModulus/8) % scalarModulus))
				{
					std::cout << "1\n";
					reconciliationOut[c / 8] |= 1 << (c % 8);
				}
				else
				{
					std::cout << "0\n";
				}
			}
			else if(round((coefficient + scalarModulus - scalarModulus/8) % scalarModulus))
			{
				std::cout << coefficient << " - Scheme 0 - 1\n";
				reconciliationOut[c / 8] |= 1 << (c % 8);
			}
			else
			{
				std::cout << coefficient << " - Scheme 0 - 0\n";
			}
		}
	}

	void Polynomial::CreateSchemeAndReconcile(
		std::array<uint8_t, Polynomial::reconciliationLength>& schemeOut,
		SecureArray<Polynomial::reconciliationLength>& reconciliationOut,
		FortunaPRNG& fprng) const
	{
		reconciliationOut.Zero();
		schemeOut.fill(0);

		for(unsigned int c = 0; c < polynomialModulus; c++)
		{
			uint16_t coefficient = At(c);
			if(crossround(coefficient))
			{
				std::cout << coefficient << " - Scheme 1 - ";
				schemeOut[c / 8] |= 1 << (c % 8);

				if(round((coefficient + scalarModulus - 3*scalarModulus/8) % scalarModulus))
				{
					std::cout << "1\n";
					reconciliationOut[c / 8] |= 1 << (c % 8);
				}
				else
				{
					std::cout << "0\n";
				}
			}
			else if(round((coefficient + scalarModulus - scalarModulus/8) % scalarModulus))
			{
				std::cout << coefficient << " - Scheme 0 - 1\n";
				reconciliationOut[c / 8] |= 1 << (c % 8);
			}
			else
			{
				std::cout << coefficient << " - Scheme 0 - 0\n";
			}
		}
	}

	uint16_t Polynomial::At(unsigned int exponent) const
	{
		const uint16_t* coefficientPtr = reinterpret_cast<const uint16_t*>(coefficients.GetConst());
		return coefficientPtr[exponent % polynomialModulus];
	}

	void Polynomial::Print() const
	{
		bool first = true;
		for(unsigned int i = 0; i < polynomialModulus; i++)
		{
			//uint16_t val = std::min(At(i), static_cast<uint16_t>(scalarModulus - At(i)));
			uint16_t val = At(i);
			if(val != 0)
			{
				if(first)
				{
					std::cout << val;
					first = false;
				}
				else
					std::cout << " + " << val;

				switch(i)
				{
				case 0:
					break;
				case 1:
					std::cout << "x";
					break;
				default:
					std::cout << "x^" << i;
					break;
				}
			}
		}

		std::cout << std::endl;
	}

	uint16_t Polynomial::AverageDistance() const
	{
		uint32_t total;
		for(unsigned int i = 0; i < polynomialModulus; i++)
		{
			total += std::min(At(i), static_cast<uint16_t>(scalarModulus - At(i)));
		}
		return static_cast<uint16_t>(total/polynomialModulus);
	}

	Polynomial Polynomial::operator+(const Polynomial& rhPoly) const
	{
		Polynomial poly;
		for(unsigned int exp = 0; exp < polynomialModulus; exp++)
		{
			poly[exp] = (At(exp) + rhPoly.At(exp)) % scalarModulus;
		}

		return std::move(poly);
	}

	Polynomial Polynomial::operator-(const Polynomial& rhPoly) const
	{
		Polynomial poly;
		for(unsigned int exp = 0; exp < polynomialModulus; exp++)
		{
			poly[exp] = (At(exp) + scalarModulus - rhPoly.At(exp)) % scalarModulus;
		}

		return std::move(poly);
	}

	Polynomial& Polynomial::operator+=(const Polynomial& rhPoly)
	{
		for(unsigned int exp = 0; exp < polynomialModulus; exp++)
		{
			operator[](exp) += rhPoly.At(exp);
			operator[](exp)	%= scalarModulus;
		}

		return *this;
	}

	Polynomial Polynomial::operator*(const Polynomial& rhPoly) const
	{
		Polynomial poly;
		for(unsigned int lhExp = 0; lhExp < polynomialModulus; lhExp++)
		{
			for(unsigned int rhExp = 0; rhExp < polynomialModulus; rhExp++)
			{
				unsigned int exp = (lhExp + rhExp);
				if(exp >= polynomialModulus)
				{
					exp -= polynomialModulus;
					poly[exp] += scalarModulus - ((static_cast<uint32_t>(At(lhExp)) * static_cast<uint32_t>(rhPoly.At(rhExp))) % static_cast<uint32_t>(scalarModulus));
				}
				else
					poly[exp] += (static_cast<uint32_t>(At(lhExp)) * static_cast<uint32_t>(rhPoly.At(rhExp))) % static_cast<uint32_t>(scalarModulus);

				poly[exp] %= scalarModulus;
			}
		}

		return std::move(poly);
	}

	Polynomial& Polynomial::operator*=(const Polynomial& rhPoly)
	{
		Polynomial poly;
		for(unsigned int lhExp = 0; lhExp < polynomialModulus; lhExp++)
		{
			for(unsigned int rhExp = 0; rhExp < polynomialModulus; rhExp++)
			{
				unsigned int exp = (lhExp + rhExp);
				if(exp >= polynomialModulus)
				{
					exp -= polynomialModulus;
					poly[exp] += scalarModulus - ((static_cast<uint32_t>(At(lhExp)) * static_cast<uint32_t>(rhPoly.At(rhExp))) % static_cast<uint32_t>(scalarModulus));
				}
				else
					poly[exp] += (static_cast<uint32_t>(At(lhExp)) * static_cast<uint32_t>(rhPoly.At(rhExp))) % static_cast<uint32_t>(scalarModulus);

				poly[exp] %= scalarModulus;
			}
		}

		memcpy(coefficients.Get(), poly.coefficients.GetConst(), coefficients.Size());
		return *this;
	}

	uint16_t& Polynomial::operator[](unsigned int exponent)
	{
		uint16_t* coefficientPtr = reinterpret_cast<uint16_t*>(coefficients.Get());
		return coefficientPtr[exponent % polynomialModulus];
	}

	bool Polynomial::operator==(const Polynomial& rhs) const
	{
		return memcmp(coefficients.GetConst(), rhs.coefficients.GetConst(), coefficients.Size()) == 0;
	}

	Polynomial::Polynomial()
	{
		coefficients.Zero();
	}

	Polynomial GeneratePublicKey(
		const Polynomial& privateKey,
		const Polynomial& basePolynomial,
		FortunaPRNG& fprng)
	{
		return (basePolynomial * privateKey) + Polynomial::ErrorPolynomial(fprng);
	}

	void CreateSharedKey(
		const Polynomial& privateKey,
		const Polynomial& publicKey,
		const std::array<uint8_t, 128>& scheme,
		SecureArray<Polynomial::reconciliationLength>& reconciliationOut,
		FortunaPRNG& fprng)
	{
		Polynomial product = (publicKey * privateKey);
		product.Reconcile(scheme, reconciliationOut, fprng);
	}

	void CreateSharedKeyAndReconciliation(
		const Polynomial& privateKey,
		const Polynomial& publicKey,
		std::array<uint8_t, 128>& schemeOut,
		SecureArray<Polynomial::reconciliationLength>& reconciliationOut,
		FortunaPRNG& fprng)
	{
		Polynomial product = (publicKey * privateKey);
		product.CreateSchemeAndReconcile(schemeOut, reconciliationOut, fprng);
	}
}
