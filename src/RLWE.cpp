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

	void round(double& x0, double& x1, double& x2, double& x3)
	{
		x0 = std::round(x0);
		x1 = std::round(x1);
		x2 = std::round(x2);
		x3 = std::round(x3);
	}

	bool Decode(const double& x0, const double& x1, const double& x2, const double& x3)
	{
		double r0 = x0;
		double r1 = x1;
		double r2 = x2;
		double r3 = x3;
		round(r0, r1, r2, r3);

		return std::abs(x0 - r0) + std::abs(x1 - r1) + std::abs(x2 - r2) + std::abs(x3 - r3) > 1;
	}

	bool CVP(const double& x0, const double& x1, const double& x2, const double& x3, uint8_t& recByteOut)
        {
                double r0 = x0;
                double r1 = x1;
                double r2 = x2;
                double r3 = x3;
                round(r0, r1, r2, r3);

		double s0 = x0 - 0.5;
                double s1 = x1 - 0.5;
                double s2 = x2 - 0.5;
                double s3 = x3 - 0.5;
                round(s0, s1, s2, s3);

                bool k = std::abs(x0 - r0) + std::abs(x1 - r1) + std::abs(x2 - r2) + std::abs(x3 - r3) > 1;
		if(k)
		{
			uint8_t a = s0 - s3;
			uint8_t b = s1 - s3;
			uint8_t c = s2 - s3;
			uint8_t d = (uint8_t)k + 2*s3;
			
		}
        }
}

namespace RLWE
{
	Polynomial Polynomial::RandomPolynomial(FortunaPRNG& fprng)
	{
		Polynomial poly;
		fprng.GenerateBlocks(poly.coefficients.Get(), (polynomialModulus*sizeof(uint16_t))/16);

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
			int t = 0;
			for(unsigned int j = 0; j < 16; j++)
			{
				t += fprng.Decision() - fprng.Decision();
			}
			error[i] = (t + scalarModulus) % scalarModulus;
		}
		return std::move(error);
	}

	void Polynomial::Reconcile(
		const std::array<uint8_t, Polynomial::reconciliationLength>& reconciliation,
		SecureArray<Polynomial::sharedKeyLength>& keyOut,
		FortunaPRNG& fprng) const
	{
		keyOut.Zero();
		for(unsigned int i = 0; i < sharedKeyLength; i++)
		{
			unsigned int j = i * 32;
			double x0 = At(j++)/scalarModulus;
			double x1 = At(j++)/scalarModulus;
			double x2 = At(j++)/scalarModulus;
			double x3 = At(j++)/scalarModulus;

			x0 = At(j+0)/scalarModulus;
                        double x1 = At(j+1)/scalarModulus;
                        double x2 = At(j+2)/scalarModulus;
                        double x3 = At(j+3)/scalarModulus;
		}
	}

	void Polynomial::CreateSchemeAndReconcile(
		std::array<uint8_t, Polynomial::reconciliationLength>& reconciliationOut,
		SecureArray<Polynomial::sharedKeyLength>& keyOut,
		FortunaPRNG& fprng) const
	{
		reconciliationOut.fill(0);
		keyOut.Zero();

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
