#pragma once
#include "fpm/fixed.hpp"
#include "fpm/math.hpp"

using fixed = fpm::fixed_16_16;

inline fixed operator "" _fx(unsigned long long int i)
{
	return fixed{ i };
}

struct FVec
{
	fixed X;
	fixed Y;
};

inline FVec operator-(FVec V)
{
	return FVec{-V.X, -V.Y};
}

inline FVec operator+(FVec A, FVec B)
{
	return FVec{ A.X + B.X, A.Y + B.Y };
}

inline FVec operator*(FVec V, fixed C)
{
	return FVec{ V.X * C, V.Y * C };
}

inline FVec operator*(fixed C, FVec V)
{
	return FVec{ V.X * C, V.Y * C };
}

inline FVec operator/(FVec V, fixed C)
{
	return FVec{ V.X / C, V.Y / C };
}

inline FVec operator-(FVec A, FVec B)
{
	return FVec{ A.X - B.X, A.Y - B.Y };
}

inline fixed operator|(FVec A, FVec B)
{
	return A.X * B.X + A.Y * B.Y;
}

inline fixed phi(FVec V)
{
	return fpm::atan2(V.Y, V.X);
}

inline FVec phiToDir(fixed phi)
{
	return FVec{ fpm::cos(phi), fpm::sin(phi) };
}

inline fixed length(FVec V)
{
	fixed Phi = fpm::atan2(V.Y, V.X);
	if (fpm::abs(V.X) > fpm::abs(V.Y))
	{
		return V.X / fpm::cos(Phi);
	}
	else
	{
		return V.Y / fpm::sin(Phi);
	}
}

inline FVec normalize(FVec V)
{
	return V / length(V);
}

inline FVec RotatePlus90(FVec V)
{
	return FVec{ -V.Y, V.X };
}

inline FVec RotateMinus90(FVec V)
{
	return FVec{ V.X, -V.Y };
}