#pragma once

#ifndef BITFUNCTIONS_H
#define BITFUNCTIONS_H

// variableless versions assuming the argument to be 0
// useful for enums
#define SETBIT(x) (1 << (x))
#define SETBITS(numBits, firstBit) (((1 << (numBits)) - 1) << (firstBit))

#define GETBIT(x, y) (((x) >> (y)) & 1)
#define GETBITS(x, y, z) (((x) >> (y)) & ((1 << (z)) - 1))

template <typename T>
static inline bool HasBit(const T x, const int y)
{
	return (x & ((T)1U << y)) != 0;
}

template <typename T>
static inline T SetBit(T &x, const int y)
{
	return x = (T)(x | ((T)1U << y));
}

template <typename T>
static inline T ClrBit(T &x, const int y)
{
	return x = (T)(x & ~((T)1U << y));
}

// use one of the previous functions instead of hardcoding bValue
// this function is only for cases where a bit can be turned both on and off
template <typename T>
static inline T SetBit(T &x, const int y, const bool bValue)
{
	if (bValue)
	{
		return x = (T)(x | ((T)1U << y));
	}
	else {
		return x = (T)(x & ~((T)1U << y));
	}
}

// use both has and get prefix as both fits and remembering both appears to be a pain
template <typename T>
static inline T GetBits(T &x, const int iIndex, const int iNumBits)
{
	return (x >> iIndex) & (((T)1u << iNumBits) - 1);
}

template <typename T>
static inline T HasBits(T &x, const int iIndex, const int iNumBits)
{
	return GetBits(x, iIndex, iNumBits);
}

template <typename T>
static inline T SetBits(T &x, const int iIndex, const int iNumBits, const T iValue)
{
	x &= ~(((1 << iNumBits) - 1) << iIndex);
	x |= (iValue & ((1 << iNumBits) - 1)) << iIndex;
	return x;
}

// advc.enum (not from WtP): Macro for (somewhat) type-safe bitmasks
#define OVERLOAD_BITWISE_OPERATORS(EnumType) \
	inline EnumType operator|(EnumType eLeft, EnumType eRight) \
	{ \
		int iLeft = eLeft, iRight = eRight; \
		return static_cast<EnumType>(iLeft | iRight); \
	} \
	inline EnumType operator|=(EnumType& eLeft, EnumType eRight) \
	{ \
		return eLeft = (eLeft | eRight); \
	} \
	inline EnumType operator&(EnumType eLeft, EnumType eRight) \
	{ \
		int iLeft = eLeft, iRight = eRight; \
		return static_cast<EnumType>(iLeft & iRight); \
	} \
	inline EnumType operator&=(EnumType& eLeft, EnumType eRight) \
	{ \
		return eLeft = (eLeft & eRight); \
	} \
	inline EnumType operator^(EnumType eLeft, EnumType eRight) \
	{ \
		int iLeft = eLeft, iRight = eRight; \
		return static_cast<EnumType>(iLeft ^ iRight); \
	} \
	inline EnumType operator^=(EnumType& eLeft, EnumType eRight) \
	{ \
		return eLeft = (eLeft ^ eRight); \
	} \
	inline EnumType operator~(EnumType e) \
	{ \
		int i = e; \
		return static_cast<EnumType>(~i); \
	}

#endif
