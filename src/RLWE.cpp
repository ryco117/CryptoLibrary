#include "RLWE.h"
#include "Fortuna.h"

#include <string.h>
#include <math.h>
#include <iostream>
#include <assert.h>

namespace
{
	// Double coefficient, add 1 50% of the time, cast to floatW
	float randDbl(uint16_t x, FortunaPRNG& fprng)
	{
		return float(2 * x + fprng.Decision());
	}

	// Take four coefficients in range [0, scalarModulus-1] and map to [0, 1)^4
	void Map2HyperCube(uint16_t a, uint16_t b, uint16_t c, uint16_t d,
		float& x, float& y, float& z, float& w, FortunaPRNG& fprng)
	{
		const float max = 2.0 * float(RLWE::Polynomial::scalarModulus);

		x = randDbl(a, fprng);
		x = x / max;

		y = randDbl(b, fprng);
		y = y / max;

		z = randDbl(c, fprng);
		z = z / max;

		w = randDbl(d, fprng);
		w = w / max;
	}

	float GetHyperCubeManhattanDist(const float& x, const float& y, const float& z, const float& w)
	{
		return fabs(x) + fabs(y) + fabs(z) + fabs(w);
	}

    // Determine if located within central (0,0,0,0) 24-cell on 4-D lattice
	bool InShaded24Cell(const float& x, const float& y, const float& z, const float& w)
	{
	    const float x0 = round(x);
	    const float y0 = round(y);
	    const float z0 = round(z);
	    const float w0 = round(w);

	    return (GetHyperCubeManhattanDist(x - x0, y - y0, z - z0, w - w0) > 1.0);
	}
	bool InShaded24CellAndMapIfNot(float& x, float& y, float& z, float& w)
	{
	    const float x0 = round(x);
	    const float y0 = round(y);
	    const float z0 = round(z);
	    const float w0 = round(w);

	    const float x1 = round(x - 0.5);
	    const float y1 = round(y - 0.5);
	    const float z1 = round(z - 0.5);
	    const float w1 = round(w - 0.5);

	    float u, v, s, t;
	    float k = (GetHyperCubeManhattanDist(x - x0, y - y0, z - z0, w - w0) < 1.0) ? 0.0 : 1.0;
	    if(k < 1.0)
	    {
            u = x0;
            v = y0;
            s = z0;
            t = w0;
	    }
	    else
	    {
	        u = x1;
            v = y1;
            s = z1;
            t = w1;
	    }
	    u -= t;
	    v -= t;
	    s -= t;
        t += t + k;

        const float floatTOver2 = float(t)/2.0;
        float xP = u + floatTOver2;
        float yP = v + floatTOver2;
        float zP = s + floatTOver2;
        float wP = floatTOver2;

        // std::cout << "Point: <" << x << "," << y << "," << z << "," << w <<
		//     "> Closest lattice: <" << xP << "," << yP << "," << zP << "," << wP << "> ";
		// std::cout << "Distance: " << sqrt(pow(x-xP, 2.0) + pow(y-yP, 2.0) + pow(z-zP, 2.0) + pow(w-wP, 2.0)) << " -- ";

		bool shaded = k > 0.0;
		if(!shaded)
		{
		    x += 0.5 - xP;
		    y += 0.5 - yP;
		    z += 0.5 - zP;
		    w += 0.5 - wP;
		}

		return shaded;
	}

	// Take point in central (shaded) voronoi cell and map to 8 bit
	// reconciliation vector (2 coord bits per dimension in uvst axis)
	void ReconcileShadedCell(float x, float y, float z, float w,
		uint8_t& rec, FortunaPRNG& fprng)
	{
	    // std::cout << "Point: <" << x << "," << y << "," << z << "," << w << "> ";
	    const float v0 = 4.0*(x - 0.5);
	    const float v1 = 4.0*(y - 0.5);
	    const float v2 = 4.0*(z - 0.5);
	    const float v3 = 4.0*(w - 0.5);
        float u0 = (v0 - v1)/sqrt(2.0);
		float u1 = (v0 + v1)/sqrt(2.0);
		float u2 = (v2 - v3)/sqrt(2.0);
		float u3 = (v2 + v3)/sqrt(2.0);
		// std::cout << "Rotate&ScaleTo: <" << u0 << "," << u1 << "," << u2 << "," << u3 << "> ";
		const float f0 = floor(u0);
	    const float f1 = floor(u1);
	    const float f2 = floor(u2);
	    const float f3 = floor(u3);
	    u0 -= f0; u1 -= f1; u2 -= f2; u3 -= f3;
	    // std::cout << "FloorTo: <" << u0 << "," << u1 << "," << u2 << "," << u3 << "> ";

		const float x0 = round(u0);
	    const float y0 = round(u1);
	    const float z0 = round(u2);
	    const float w0 = round(u3);

	    const float x1 = round(u0 - 0.5);
	    const float y1 = round(u1 - 0.5);
	    const float z1 = round(u2 - 0.5);
	    const float w1 = round(u3 - 0.5);

	    int8_t u, v, s, t;
	    float k = (GetHyperCubeManhattanDist(u0 - x0, u1 - y0, u2 - z0, u3 - w0) < 1.0) ? 0.0 : 1.0;
	    if(k < 1.0)
	    {
            u = x0;
            v = y0;
            s = z0;
            t = w0;
	    }
	    else
	    {
	        u = x1;
            v = y1;
            s = z1;
            t = w1;
	    }
	    u -= t;
	    v -= t;
	    s -= t;
        t += t + k;

        const float floatTOver2 = float(t)/2.0;
        int8_t u0P = u + floatTOver2;
        int8_t u1P = v + floatTOver2;
        int8_t u2P = s + floatTOver2;
        int8_t u3P = floatTOver2;
        // std::cout << "uvst: <" << (int)u0P << "," << (int)u1P << "," << (int)u2P << "," << (int)u3P << "> ";
	    u0P += f0 + 2; u1P += f1 + 2; u2P += f2 + 2; u3P += f3 + 2;
	    const int8_t THREE = 3; u0P = std::min(THREE, u0P); u1P = std::min(THREE, u1P); u2P = std::min(THREE, u2P); u3P = std::min(THREE, u3P);
	    const int8_t ZERO = 0; u0P = std::max(ZERO, u0P); u1P = std::max(ZERO, u1P); u2P = std::max(ZERO, u2P); u3P = std::max(ZERO, u3P);

        // std::cout << "AddFloor-uvst: <" << (int)u0P << "," << (int)u1P << "," << (int)u2P << "," << (int)u3P << "> ";

        float xP = float(u0P+u1P - 4)/(sqrt(2.0)*4.0) + 0.5;
		float yP = float(u1P-u0P)/(sqrt(2.0)*4.0) + 0.5;
		float zP = float(u2P+u3P - 4)/(sqrt(2.0)*4.0) + 0.5;
		float wP = float(u3P-u2P)/(sqrt(2.0)*4.0) + 0.5;

		// std::cout << "Recovered Original: <" << xP << "," << yP << "," << zP << "," << wP << "> ";
		// std::cout << "uvst Distance: " << sqrt(pow(x-xP, 2.0) + pow(y-yP, 2.0) + pow(z-zP, 2.0) + pow(w-wP, 2.0)) << "\n";

		rec = u0P & 3;
		rec <<= 2;
		rec += u1P & 3;
		rec <<= 2;
		rec += u2P & 3;
		rec <<= 2;
		rec += u3P & 3;
	}

	// Transform 8 bit reconciliation vector to corresponding xyzw axis vector
	// and subtract it from input vector
	void SubtractReconcileVector(uint8_t rec, float& x, float& y, float& z, float& w)
	{
		float t = float(rec & 3);
		rec >>= 2;
		float s = float(rec & 3);
		rec >>= 2;
		float v = float(rec & 3);
		rec >>= 2;
		float u = float(rec & 3);

 		float rX = (u+v - 4.0)/(sqrt(2.0)*4.0);
		float rY = (v-u)/(sqrt(2.0)*4.0);
		float rZ = (s+t - 4.0)/(sqrt(2.0)*4.0);
		float rW = (t-s)/(sqrt(2.0)*4.0);

        // std::cout << "Original Point: <" << x << "," << y << "," << z << "," << w << "> ";
		// std::cout << "Reconcile Vector: <" << rX << "," << rY << "," << rZ << "," << rW << "> ";
		x -= rX; y -= rY; z -= rZ; w -= rW;
		// std::cout << "Shifted Point: <" << x << "," << y << "," << z << "," << w << ">\n";
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
			for(unsigned int j = 0; j < BINOMIAL_SAMPLE_COUNT; j++)
			{
			    t += (int)fprng.Decision() - fprng.Decision();
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

		for(unsigned int i = 0; i < Polynomial::reconciliationLength; i++)
		{
			float x,y,z,w;
			unsigned int coeffIndex = i << 2;
			Map2HyperCube(At(coeffIndex), At(coeffIndex+1), At(coeffIndex+2), At(coeffIndex+3),
				x, y, z, w, fprng);

			SubtractReconcileVector(reconciliation[i], x, y, z, w);
			bool shaded = InShaded24Cell(x, y, z, w);
			keyOut[i >> 3] |= static_cast<uint8_t>(shaded) << (i & 7);
		}
	}

	void Polynomial::CreateSchemeAndReconcile(
		std::array<uint8_t, Polynomial::reconciliationLength>& reconciliationOut,
		SecureArray<Polynomial::sharedKeyLength>& keyOut,
		FortunaPRNG& fprng) const
	{
		reconciliationOut.fill(0);
		keyOut.Zero();

		for(unsigned int i = 0; i < Polynomial::reconciliationLength; i++)
		{
			float x,y,z,w;
			unsigned int coeffIndex = i << 2;
			Map2HyperCube(At(coeffIndex), At(coeffIndex+1), At(coeffIndex+2), At(coeffIndex+3),
				x, y, z, w, fprng);

			bool shaded = InShaded24CellAndMapIfNot(x, y, z, w);
			keyOut[i >> 3] |= static_cast<uint8_t>(shaded) << (i & 7);

			ReconcileShadedCell(x, y, z, w,
				reconciliationOut[i], fprng);
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
			uint16_t val = At(i);
			if(val != 0)
			{
				if(first)
				{
				    first = false;
				    if(val > 1)
					{
					    std::cout << val;
					}
				}
				else
				{
				    std::cout << " + ";
				    if(val > 1)
				    {
					    std::cout << val;
					}
				}

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
				unsigned int exp = lhExp + rhExp;
				if(exp < polynomialModulus)
				{
				    poly[exp] += (static_cast<uint32_t>(At(lhExp)) * static_cast<uint32_t>(rhPoly.At(rhExp))) % static_cast<uint32_t>(scalarModulus);
				}
				else
				{
				    exp -= polynomialModulus;
				    poly[exp] += scalarModulus - ((static_cast<uint32_t>(At(lhExp)) * static_cast<uint32_t>(rhPoly.At(rhExp))) % static_cast<uint32_t>(scalarModulus));
				}
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
				unsigned int exp = lhExp + rhExp;
				if(exp < polynomialModulus)
				{
				    poly[exp] += (static_cast<uint32_t>(At(lhExp)) * static_cast<uint32_t>(rhPoly.At(rhExp))) % static_cast<uint32_t>(scalarModulus);
				}
				else
				{
				    exp -= polynomialModulus;
				    poly[exp] += scalarModulus - ((static_cast<uint32_t>(At(lhExp)) * static_cast<uint32_t>(rhPoly.At(rhExp))) % static_cast<uint32_t>(scalarModulus));
				}
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
		const std::array<uint8_t, Polynomial::reconciliationLength>& reconciliation,
		SecureArray<Polynomial::sharedKeyLength>& sharedKeyOut,
		FortunaPRNG& fprng)
	{
		Polynomial product = (publicKey * privateKey) + Polynomial::ErrorPolynomial(fprng);
		product.Reconcile(reconciliation, sharedKeyOut, fprng);
	}

	void CreateSharedKeyAndReconciliation(
		const Polynomial& privateKey,
		const Polynomial& publicKey,
		std::array<uint8_t, Polynomial::reconciliationLength>& reconciliationOut,
		SecureArray<Polynomial::sharedKeyLength>& sharedKeyOut,
		FortunaPRNG& fprng)
	{
		Polynomial product = (publicKey * privateKey) + Polynomial::ErrorPolynomial(fprng);
		product.CreateSchemeAndReconcile(reconciliationOut, sharedKeyOut, fprng);
	}
}
