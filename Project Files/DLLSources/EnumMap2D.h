#ifndef ENUM_MAP_2D_H
#define ENUM_MAP_2D_H
#pragma once

#include "EnumMap.h"


template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
class EnumMap2DDefault
{
public:
	EnumMap2DDefault();
	~EnumMap2DDefault();

	OuterArrayType First() const;
	OuterArrayType Length() const;

	T get(OuterArrayType eArray, InnerArrayType eIndex) const;
	void set(OuterArrayType eArray, InnerArrayType eIndex, T eValue);
	void add(OuterArrayType eArray, InnerArrayType eIndex, T eValue);

	bool hasContent() const;

	void reset();

	void Read(CvSavegameReader& reader);
	void Write(CvSavegameWriter& writer) const;

private:
	void allocate();

	EnumMapDefault<InnerArrayType, T, DEFAULT> * m_pOuterArray;

	enum
	{
		SAVE_ARRAY_MULTI_BYTE,
		SAVE_ARRAY_LAST_TOKEN,
		SAVE_ARRAY_INDEX_OFFSET,
		SAVE_ARRAY_EMPTY_BYTE = 0xFF,
		SAVE_ARRAY_EMPTY_SHORT = 0xFFFF,
	};

	class interval
	{
	public:
		OuterArrayType first;
		OuterArrayType last;

		interval()
		{
			first = (OuterArrayType)0;
			last = (OuterArrayType)0;
		}
	};
};

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::EnumMap2DDefault()
	: m_pOuterArray(NULL)
{
}

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::~EnumMap2DDefault()
{
	reset();
}

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
__forceinline OuterArrayType EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::First() const
{
	return (OuterArrayType)0;
}

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
__forceinline OuterArrayType EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::Length() const
{
	return ArrayLength((OuterArrayType)0);
}

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
inline T EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::get(OuterArrayType eArray, InnerArrayType eIndex) const
{
	FAssert(eArray >= 0 && eArray < Length());
	return m_pOuterArray ? m_pOuterArray[eArray].get(eIndex) : (T)DEFAULT;
}

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
inline void EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::set(OuterArrayType eArray, InnerArrayType eIndex, T eValue)
{
	FAssert(eArray >= 0 && eArray < Length());
	if (m_pOuterArray == NULL)
	{
		if (eValue == DEFAULT) return;
		allocate();
	}
	m_pOuterArray[eArray].set(eIndex, eValue);
}

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
inline void EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::add(OuterArrayType eArray, InnerArrayType eIndex, T eValue)
{
	FAssert(eArray >= 0 && eArray < Length());
	if (eValue == 0)
	{
		return;
	}
	
	if (m_pOuterArray == NULL)
	{
		allocate();
	}
	m_pOuterArray[eArray].add(eIndex, eValue);
}

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
inline bool EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::hasContent() const
{
	if (m_pOuterArray)
	{
		for (OuterArrayType eArray = First(); eArray < Length(); ++eArray)
		{
			if (m_pOuterArray[eArray].hasContent())
			{
				return true;
			}
		}
		// now we cheat and alter data despite being const.
		// We just detected all data to be of the default value, meaning the array is not used.
		// Release the data to save memory. It won't change how the outside world view the EnumMap.
		(const_cast <EnumMap2DDefault*> (this))->reset();
	}

	return false;
}

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
inline void EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::reset()
{
	SAFE_DELETE_ARRAY(m_pOuterArray);
}

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
inline void EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::allocate()
{
	FAssert(m_pOuterArray == NULL);
	m_pOuterArray = new EnumMapDefault<InnerArrayType, T, DEFAULT>[Length()];
}

template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
inline void EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::Read(CvSavegameReader& reader)
{
	// savegame format is as follows:
	// first index
	// last index
	// loop writing contents from first to last, both inclusive.
	// An array can be split into multiple subarrays, called tokens.
	// Any data not saved will be set to the default value on load.
	// This will greatly reduce savegame size if just a few entries have non-default data.
	//
	// To reduce overhead, the size of the indexes are shortened to be as short as possible, but still contain the max value.
	// There are two bools in each token. One states it's the last token and the other says first == last.
	// Those two are saved together with the first index, meaning only 14 bits are available for saved array length.
	// If the length of the array can fit in 6 bits, then first index is saved in just a single byte.
	// CvSavegameReader takes care of saving the last index, meaning it's a single byte if the length is max 250.
	// Last index is not saved if the bool for first == last is set.
	// As a result, the overhead for each token can be 1-4 bytes.

	const int iLength = (JITarrayTypes)InnerArrayType((OuterArrayType)0) == NO_JIT_ARRAY_TYPE ? ArrayLength((OuterArrayType)0) : reader.GetXmlSize(ArrayType((OuterArrayType)0));
	const bool bUseTwoByteStart = iLength > 64;

	int iBuffer;

	reset();

	if (bUseTwoByteStart)
	{
		unsigned short iReadBuffer;
		reader.Read(iReadBuffer);
		iBuffer = iReadBuffer;
		if (iBuffer == SAVE_ARRAY_EMPTY_BYTE)
		{
			// empty array
			return;
		}
	}
	else
	{
		byte iReadBuffer;
		reader.Read(iReadBuffer);
		iBuffer = iReadBuffer;
		if (iBuffer == SAVE_ARRAY_EMPTY_SHORT)
		{
			// empty array
			return;
		}
	}

	allocate();
	

	while (true)
	{
		OuterArrayType iFirst = (OuterArrayType)(iBuffer >> SAVE_ARRAY_INDEX_OFFSET);
		bool bMultiByte = HasBit(iBuffer, SAVE_ARRAY_MULTI_BYTE);
		bool bLast = HasBit(iBuffer, SAVE_ARRAY_LAST_TOKEN);

		if (bMultiByte)
		{
			int iLast;
			if (iLength > 256)
			{
				unsigned short iReadBuffer;
				reader.Read(iReadBuffer);
				iLast = iReadBuffer;
			}
			else
			{
				byte iReadBuffer;
				reader.Read(iReadBuffer);
				iLast = iReadBuffer;
			}

			for (OuterArrayType i = iFirst; i <= iLast; ++i)
			{
				int iIndex = reader.ConvertIndex(ArrayType((OuterArrayType)0), i);

				if (iIndex >= First() && iIndex < Length())
				{
					m_pOuterArray[iIndex].Read(reader);
				}
				else
				{
					// entry removed. Read data and discard.
					EnumMapDefault<InnerArrayType, T, DEFAULT> temp;
					temp.Read(reader);
				}
			}
		}
		else
		{
			// single variable token
			int iIndex = reader.ConvertIndex(ArrayType((OuterArrayType)0), iFirst);

			if (iIndex >= First() && iIndex < Length())
			{
				m_pOuterArray[iFirst].Read(reader);
			}
			else
			{
				// entry removed. Read data and discard.
				EnumMapDefault<InnerArrayType, T, DEFAULT> temp;
				temp.Read(reader);
			}
		}

		if (bLast)
		{
			return;
		}
		if (bUseTwoByteStart)
		{
			unsigned short iReadBuffer;
			reader.Read(iReadBuffer);
			iBuffer = iReadBuffer;
		}
		else
		{
			byte iReadBuffer;
			reader.Read(iReadBuffer);
			iBuffer = iReadBuffer;
		}
	}
}


template<class OuterArrayType, class InnerArrayType, class T, int DEFAULT>
inline void EnumMap2DDefault<OuterArrayType, InnerArrayType, T, DEFAULT>::Write(CvSavegameWriter& writer) const
{
	const int iLength = (JITarrayTypes)InnerArrayType((OuterArrayType)0) == NO_JIT_ARRAY_TYPE ? ArrayLength((OuterArrayType)0) : writer.GetXmlSize(ArrayType((OuterArrayType)0));
	const bool bUseTwoByteStart = iLength > 64;

	// first support writing an empty array
	// this is likely never needed, but support it for completeness and stability
	if (!hasContent())
	{
		if (bUseTwoByteStart)
		{
			unsigned short iBuffer = SAVE_ARRAY_EMPTY_SHORT;
			writer.Write(iBuffer);
		}
		else
		{
			byte iBuffer = SAVE_ARRAY_EMPTY_BYTE;
			writer.Write(iBuffer);
		}
		return;
	}

	std::vector<interval> tokens(0);
	interval* pInterval = NULL;

	// generate tokens
	// pInterval points to the token currently being created
	// this means it both acts as a pointer to where to store iLast, but also storing the state
	for (OuterArrayType eIndex = First(); eIndex < Length(); ++eIndex)
	{
		if (m_pOuterArray[eIndex].hasContent())
		{
			if (pInterval == NULL)
			{
				interval eTemp;
				tokens.push_back(eTemp);
				pInterval = &tokens[tokens.size() - 1];
				pInterval->first = eIndex;
			}
			pInterval->last = eIndex;
		}
		else
		{
			pInterval = NULL;
		}
	}

	int iEnd = tokens.size() - 1;

	// save the tokens
	for (int i = 0; i <= iEnd; ++i)
	{
		OuterArrayType iFirst = tokens[i].first;
		OuterArrayType iLast = tokens[i].last;

		int iBuffer = iFirst << SAVE_ARRAY_INDEX_OFFSET;

		if (i == iEnd)
		{
			SetBit(iBuffer, SAVE_ARRAY_LAST_TOKEN);
		}

		if (iFirst == iLast)
		{
			if (bUseTwoByteStart)
			{
				writer.Write((unsigned short)iBuffer);
			}
			else
			{
				writer.Write((byte)iBuffer);
			}
			writer.Write(m_pOuterArray[iFirst]);
		}
		else
		{
			SetBit(iBuffer, SAVE_ARRAY_MULTI_BYTE);
			if (bUseTwoByteStart)
			{
				writer.Write((unsigned short)iBuffer);
			}
			else
			{
				writer.Write((byte)iBuffer);
			}
			if (iLength > 256)
			{
				writer.Write((unsigned short)iLast);
			}
			else
			{
				writer.Write((byte)iLast);
			}

			for (int i = iFirst; i <= iLast; ++i)
			{
				writer.Write(m_pOuterArray[i]);
			}
		}
	}
}


template<class outerArrayType, class InnerArrayType, class T>
class EnumMap2D : public EnumMap2DDefault < outerArrayType, InnerArrayType, T, EnumMapGetDefault<T>::DEFAULT_VALUE > {};

#endif
