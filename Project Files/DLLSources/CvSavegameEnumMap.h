#ifndef SAVEGAME_ENUMMAP_H
#define SAVEGAME_ENUMMAP_H

#include "CvSavegame.h"
#include "CvSavegameEnumMapHelper.h"

enum
{
	ENUMMAP_BITMASK_8_BIT = 7,
};

template<>
template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameReader::ReadEnumMap<VARIABLE_TYPE_BOOL>::Read(CvSavegameReader& reader, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING>& em)
{
	const bool bValid = boost::is_same<bool, T>::value;
	BOOST_STATIC_ASSERT(bValid);
	em.reset();

	const int iLength = reader.GetXmlSize(VARINFO<LengthType>::JIT) > 0 ? reader.GetXmlSize(VARINFO<LengthType>::JIT) : VARINFO<LengthType>::length();

	LengthType eStart;
	LengthType eEnd;

	if (iLength <= 0x100)
	{
		byte iBuffer;
		reader.Read(iBuffer);
		eStart = (LengthType)iBuffer;
		reader.Read(iBuffer);
		eEnd = (LengthType)iBuffer;
	}
	else
	{
		unsigned short iBuffer;
		reader.Read(iBuffer);
		eStart = (LengthType)iBuffer;
		reader.Read(iBuffer);
		eEnd = (LengthType)iBuffer;
	}

	byte iBuffer = 0;

	for (LengthType eLoop = eStart; eLoop <= eEnd; ++eLoop)
	{
		const byte iBlockIndex = (eLoop - eStart) & ENUMMAP_BITMASK_8_BIT;
		if (iBlockIndex == 0)
		{
			// read next block
			reader.Read(iBuffer);
		}
		em.set((IndexType)reader.ConvertIndex(VARINFO<LengthType>::JIT, eLoop), HasBit(iBuffer, iBlockIndex));
	}
}

template<>
template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameWriter::WriteEnumMap<VARIABLE_TYPE_BOOL>::Write(CvSavegameWriter& kWriter, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING>& em)
{
	const bool bValid = boost::is_same<bool, T>::value;
	BOOST_STATIC_ASSERT(bValid);

	const int iLength = em.getLength();


	LengthType eStart = (LengthType)MAX_INT;
	LengthType eEnd = (LengthType)MIN_INT;

	for (LengthType eLoop = (LengthType)em.First(); eLoop < em.getLength(); ++eLoop)
	{
		if (em.get(eLoop))
		{
			if (eStart > eLoop)
			{
				eStart = eLoop;
			}
			if (eEnd < eLoop)
			{
				eEnd = eLoop;
			}
		}
	}

	// write empty array if needed

	if (eStart == (LengthType)MAX_INT)
	{
		// saving 1 to 0 will cause the reader to not read any data, hence no need to save blocks
		FAssert(eEnd == (LengthType)MIN_INT);
		eStart = (LengthType)1;
		eEnd = (LengthType)0;
	}

	if (iLength <= 0x100)
	{
		byte iBuffer = eStart;
		kWriter.Write(iBuffer);
		iBuffer = eEnd;
		kWriter.Write(iBuffer);
	}
	else
	{
		unsigned short iBuffer = eStart;
		kWriter.Write(iBuffer);
		iBuffer = eEnd;
		kWriter.Write(iBuffer);
	}

	byte iBuffer = 0;

	for (LengthType eLoop = eStart; eLoop <= eEnd; ++eLoop)
	{
		const byte iBlockIndex = (eLoop - eStart) & ENUMMAP_BITMASK_8_BIT;
		if (em.get(eLoop)) {
			SetBit(iBuffer, iBlockIndex);
		}
		if (iBlockIndex == ENUMMAP_BITMASK_8_BIT)
		{
			//now 8 bits are set, -> write to file
			kWriter.Write(iBuffer);
			iBuffer = 0;
		}
	}
	// there might be an unfinished byte, -> write to file
	const byte iBlockIndex = (eEnd - eStart) & ENUMMAP_BITMASK_8_BIT;
	if (iBlockIndex != ENUMMAP_BITMASK_8_BIT)
	{
		kWriter.Write(iBuffer);
	}
}

template<int TYPE2>
template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameReader::ReadEnumMap<TYPE2>::Read(CvSavegameReader& reader, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING>& em)
{
	const bool bValid1 = !boost::is_same<bool, T>::value;
	BOOST_STATIC_ASSERT(bValid1);
	const bool bValid2 = TYPE == TYPE2;
	BOOST_STATIC_ASSERT(bValid2);
	const bool bValid3 = TYPE != VARIABLE_TYPE_CLASS;
	BOOST_STATIC_ASSERT(bValid3);
	em.reset();

	while (true)
	{
		SavegameEnumMapTokenRead<IndexType, LengthType> Token(reader);

		if (Token.isEmpty())
		{
			return;
		}

		for (IndexType i = Token.getFirstIndex(); i <= Token.getLastIndex(); ++i)
		{
			T tBuffer;
			reader.Read(tBuffer);
			IndexType iIndex = (IndexType)reader.ConvertIndex(VARINFO<LengthType>::JIT, static_cast<int>(i));
			if (iIndex != (IndexType)-1)
			{
				// -1 means xml entry was removed. Discard the data in question and hope the savegame will still work
				em.set(iIndex, tBuffer);
			}
		}

		if (Token.isLast())
		{
			return;
		}
	}
}

template<int TYPE2>
template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameWriter::WriteEnumMap<TYPE2>::Write(CvSavegameWriter& kWriter, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING>& em)
{
	const bool bValid = TYPE == TYPE2 && !boost::is_same<bool, T>::value;
	BOOST_STATIC_ASSERT(bValid);

	SavegameEnumMapTokenWrite<IndexType, LengthType>* Token = NULL;
	std::list<SavegameEnumMapTokenWrite<IndexType, LengthType>* > tokens;

	const IndexType eEnd = (IndexType)VARINFO<LengthType>::end();
	for (IndexType i = (IndexType)VARINFO<LengthType>::start(); i < eEnd; ++i)
	{
		if (em.getDefault() != em.get(i))
		{
			if (Token == NULL)
			{
				Token = new SavegameEnumMapTokenWrite<IndexType, LengthType>();
				Token->setStart(i);
				tokens.push_back(Token);
			}
			Token->setEnd(i);
		}
		else if (Token != NULL)
		{
			Token = NULL;
		}
	}

	if (tokens.size() == 0)
	{
		SavegameEnumMapTokenWrite<IndexType, LengthType> EmptyToken;
		EmptyToken.writeEmpty(kWriter);
		return;
	}

	while (tokens.size() != 0)
	{
		Token = tokens.front();
		tokens.pop_front();
		if (tokens.size() == 0)
		{
			Token->setLast();
		}

		Token->write(kWriter);
		for (IndexType eIndex = Token->getFirstIndex(); eIndex <= Token->getLastIndex(); ++eIndex)
		{
			kWriter.Write(em.get(eIndex));
		}
		SAFE_DELETE(Token);
	}
}


template<>
template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameReader::ReadEnumMap<VARIABLE_TYPE_CLASS>::Read(CvSavegameReader& reader, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING>& em)
{
	em.reset();

	while (true)
	{
		SavegameEnumMapTokenRead<IndexType, LengthType> Token(reader);

		if (Token.isEmpty())
		{
			return;
		}

		for (IndexType i = Token.getFirstIndex(); i <= Token.getLastIndex(); ++i)
		{
			IndexType iIndex = (IndexType)reader.ConvertIndex(VARINFO<LengthType>::JIT, static_cast<int>(i));
			if (iIndex != (IndexType)-1)
			{
				reader.Read(em[iIndex]);
			}
			else
			{
				// -1 means xml entry was removed. Discard the data in question and hope the savegame will still work
				EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING> tmp;
				reader.Read(tmp);
			}
		}

		if (Token.isLast())
		{
			return;
		}
	}
}


template<>
template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameWriter::WriteEnumMap<VARIABLE_TYPE_CLASS>::Write(CvSavegameWriter& kWriter, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING>& em)
{
	SavegameEnumMapTokenWrite<IndexType, LengthType>* Token = NULL;
	std::list<SavegameEnumMapTokenWrite<IndexType, LengthType>* > tokens;

	const IndexType eEnd = (IndexType)VARINFO<LengthType>::end();
	for (IndexType i = (IndexType)VARINFO<LengthType>::start(); i < eEnd; ++i)
	{
		if (em[i].hasContent())
		{
			if (Token == NULL)
			{
				Token = new SavegameEnumMapTokenWrite<IndexType, LengthType>();
				Token->setStart(i);
				tokens.push_back(Token);
			}
			Token->setEnd(i);
		}
		else if (Token != NULL)
		{
			Token = NULL;
		}
	}

	if (tokens.size() == 0)
	{
		SavegameEnumMapTokenWrite<IndexType, LengthType> EmptyToken;
		EmptyToken.writeEmpty(kWriter);
		return;
	}

	while (tokens.size() != 0)
	{
		Token = tokens.front();
		tokens.pop_front();
		if (tokens.size() == 0)
		{
			Token->setLast();
		}

		Token->write(kWriter);
		for (IndexType eIndex = Token->getFirstIndex(); eIndex <= Token->getLastIndex(); ++eIndex)
		{
			kWriter.Write(em[eIndex]);
		}
		SAFE_DELETE(Token);
	}
}

#endif
