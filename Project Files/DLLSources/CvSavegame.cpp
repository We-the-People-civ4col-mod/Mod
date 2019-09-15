#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

//
// Classes to handle savegames
//
// The idea is to keep as much as possible as a byte array in the DLL and not read/write every single variable to the exe.
// The benefit of using a single array is that it allows compression.
//

// conversion table to include in savegames.
// generated at game startup and then reused each time a game is saved.
// this prevents the need to loop through all xml files when saving, hence faster saving.
std::vector<byte> save_conversion_table;

// conversion table
// generated on load. look up is [xml file][index]
// index is the value from the savegame while the returned data is the index of the same Type string with the current xml files
// This allows savegames to still work even if the order in xml is changed, including adding new stuff in the middle.
// Do note that -1 will be used if something is removed from xml
std::vector< std::vector<short> > conversion_table;

// stores the byte size of SavegameClassTypes for each class
// assigned automatically when saving. It's 1 byte if there are less than 257 variables (fits in unsigned byte)
// it automatically becomes two bytes if there are more variables.
// this is saved and on load the saved values are used instead of the current ones. This allows savegames to survive changing the byte size.
// This will in most cases save one byte for each variable saved, something which quickly adds up.
// It's easy to use since it auto configures to the enums. When a programmer adds "too many" enum values, the setting automatically change to match.
char enumByteSize[NUM_SAVEGAME_CLASS_TYPES];

// constructor
CvSavegameReader::CvSavegameReader(CvSavegameReaderBase& readerBase)
	: m_ReaderBase(readerBase)
{
	m_eClassType = NUM_SAVEGAME_CLASS_TYPES;
}

// copy constructor
CvSavegameReader::CvSavegameReader(const CvSavegameReader& reader)
	: m_ReaderBase(reader.m_ReaderBase)
{
	m_eClassType = NUM_SAVEGAME_CLASS_TYPES;
}

void CvSavegameReader::AssignClassType(SavegameClassTypes eType)
{
	m_eClassType = eType;
}

void CvSavegameReader::Read(SavegameVariableTypes& variable)
{
	FAssert(m_eClassType >= 0 && m_eClassType < NUM_SAVEGAME_CLASS_TYPES);
	int iSize = 2;

	if (m_eClassType >= 0 && m_eClassType < NUM_SAVEGAME_CLASS_TYPES)
	{
		iSize = enumByteSize[m_eClassType];
	}
	variable = (SavegameVariableTypes)ReadUnsignedBytes(iSize);
}

void CvSavegameReader::Read(int& variable)
{
	Read((byte*)&variable, sizeof(int));
}

void CvSavegameReader::Read(short& variable)
{
	Read((byte*)&variable, sizeof(short));
}

void CvSavegameReader::Read(char& variable)
{
	Read((byte*)&variable, sizeof(char));
}


void CvSavegameReader::Read(unsigned int& variable)
{
	Read((byte*)&variable, sizeof(unsigned int));
}

void CvSavegameReader::Read(unsigned short& variable)
{
	Read((byte*)&variable, sizeof(unsigned short));
}

void CvSavegameReader::Read(byte& variable)
{
	Read((byte*)&variable, sizeof(byte));
}

void CvSavegameReader::Read(bool& variable)
{
	// the 0/1 value is saved in 1 byte instead of 4
	char iBuffer;
	Read(iBuffer);
	variable = iBuffer;
}

void CvSavegameReader::Read(CvString& szString)
{
	szString.clear();

	while (true)
	{
		char iBuffer;
		Read(iBuffer);
		if (iBuffer == 0) return;
		szString += iBuffer;
	}
}

void CvSavegameReader::Read(CvWString& szString)
{
	szString.clear();

	while (true)
	{
		wchar iBuffer;
		Read(iBuffer);
		if (iBuffer == 0) return;
		szString += iBuffer;
	}
}

void CvSavegameReader::Read(BoolArray& baArray)
{
	baArray.Read(this);
}

void CvSavegameReader::Read(PlayerBoolArray& array)
{
	array.Read(*this);
}

void CvSavegameReader::Read(IDInfo& idInfo)
{
	idInfo.read(*this);
}

void CvSavegameReader::Read(byte* var, unsigned int iSize)
{
	m_ReaderBase.Read(var, iSize);
}

int CvSavegameReader::ReadBytes(int iNumBytes)
{
	switch (iNumBytes)
	{
	case 1:
	{
		char iBuffer;
		Read(iBuffer);
		return iBuffer;
	}
	case 2:
	{
		short iBuffer;
		Read(iBuffer);
		return iBuffer;
	}
	case 4:
	{
		int iBuffer;
		Read(iBuffer);
		return iBuffer;
	}
	}
	FAssert(false);
	return 0;
}

unsigned int CvSavegameReader::ReadUnsignedBytes(int iNumBytes)
{
	switch (iNumBytes)
	{
	case 1:
	{
		unsigned char iBuffer;
		Read(iBuffer);
		return iBuffer;
	}
	case 2:
	{
		unsigned short iBuffer;
		Read(iBuffer);
		return iBuffer;
	}
	case 4:
	{
		unsigned int iBuffer;
		Read(iBuffer);
		return iBuffer;
	}
	}
	FAssert(false);
	return 0;
}

void CvSavegameReader::ReadConversionTable()
{
	// prepare an empty table
	conversion_table.resize(NUM_JITarrayTypes);
	for (unsigned int i = 0; i < conversion_table.size(); ++i)
	{
		conversion_table[i].clear();
	}

	CvString szString;

	// read the table contents
	// reach iteration is one xml file
	while (true)
	{
		Read(szString);
		if (szString.length() == 0)
		{
			// done
			break;
		}

		JITarrayTypes eType = getJITArrayTypeFromString(szString.c_str());

		short iLength;
		Read(iLength);

		bool bIsValid = eType >= 0 && eType < (int)conversion_table.size();

		if (bIsValid)
		{
			// not strictly needed, but it improves performance to allocate in one go.
			// also by allocating "just enough" we can make sure it doesn't allocate more than needed.
			conversion_table[eType].reserve(iLength);
		}

		for (int i = 0; i < iLength; ++i)
		{
			Read(szString);
			if (bIsValid)
			{
				short iNewIndex = getIndexForTypeAddingPrefix(eType, szString.c_str());
				conversion_table[eType].push_back(iNewIndex);
			}
		}
	}

	// setup enum byte size

	// first default everything to 2
	for (SavegameClassTypes eClassType = FIRST_SAVEGAME_CLASS_TYPES; eClassType < NUM_SAVEGAME_CLASS_TYPES; ++eClassType)
	{
		enumByteSize[eClassType] = 2;
	}

	byte iLength;
	Read(iLength);
	for (SavegameClassTypes eClassType = FIRST_SAVEGAME_CLASS_TYPES; eClassType < iLength; ++eClassType)
	{
		Read(enumByteSize[eClassType]);
	}
}

int CvSavegameReader::ConvertIndex(JITarrayTypes eType, int iIndex) const
{
	JITarrayTypes eBaseType = GetBaseType(eType);

	if (iIndex >= 0 && eBaseType >= 0 && eBaseType < (int)conversion_table.size())
	{
		if ((int)conversion_table[eBaseType].size() > iIndex)
		{
			return conversion_table[eBaseType][iIndex];
		}
	}

	return iIndex;
}


///
///
///
///
///

// constructor
CvSavegameWriter::CvSavegameWriter(CvSavegameWriterBase& writerbase)
	: m_vector(writerbase.m_vector)
{
	m_eClassType = NUM_SAVEGAME_CLASS_TYPES;
}

// copy constructor
CvSavegameWriter::CvSavegameWriter(const CvSavegameWriter& writer)
	: m_vector(writer.m_vector)
{
	m_eClassType = NUM_SAVEGAME_CLASS_TYPES;
}

void CvSavegameWriter::AssignClassType(SavegameClassTypes eType)
{
	m_eClassType = eType;
}

void CvSavegameWriter::Write(int variable)
{
	Write((byte*)&variable, sizeof(int));
}

void CvSavegameWriter::Write(short variable)
{
	Write((byte*)&variable, sizeof(short));
}

void CvSavegameWriter::Write(char variable)
{
	Write((byte*)&variable, sizeof(char));
}

void CvSavegameWriter::Write(unsigned int variable)
{
	Write((byte*)&variable, sizeof(unsigned int));
}

void CvSavegameWriter::Write(unsigned short variable)
{
	Write((byte*)&variable, sizeof(unsigned short));
}

void CvSavegameWriter::Write(byte variable)
{
	Write((byte*)&variable, sizeof(byte));
}

void CvSavegameWriter::Write(bool variable)
{
	// save the 0/1 value in 1 byte instead of 4
	char iBuffer = variable ? 1 : 0;
	Write(iBuffer);
}

void CvSavegameWriter::Write(CvString& szString)
{
	int iLength = szString.length();

	const char* pStr = szString.c_str();

	for (int i = 0; i < iLength; ++i)
	{
		Write(pStr[i]);
	}
	char iBuffer = 0;
	Write(iBuffer);
}

void CvSavegameWriter::Write(CvWString& szString)
{
	int iLength = szString.length();

	const wchar* pStr = szString.c_str();

	for (int i = 0; i < iLength; ++i)
	{
		Write(pStr[i]);
	}
	wchar iBuffer = 0;
	Write(iBuffer);
}

void CvSavegameWriter::Write(BoolArray& baArray)
{
	baArray.Write(this);
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, CvString& szString)
{
	if (szString.length() > 0)
	{
		Write(eType);
		Write(szString);
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, CvWString& szString)
{
	if (szString.length() > 0)
	{
		Write(eType);
		Write(szString);
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, BoolArray& baArray)
{
	if (baArray.hasContent())
	{
		Write(eType);
		Write(baArray);
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, PlayerBoolArray& array)
{
	if (array.hasContent())
	{
		Write(eType);
		array.Write(*this);
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, IDInfo& idInfo)
{
	// get hold of the default values
	IDInfo temp;
	temp.reset();

	// save if one or more values differs from the default
	if (temp.eOwner != idInfo.eOwner || temp.iID != idInfo.iID)
	{
		Write(eType);
		idInfo.write(*this);
	}
}

void CvSavegameWriter::Write(byte* var, unsigned int iSize)
{
	for (unsigned int i = 0; i < iSize; ++i)
	{
		m_vector.push_back(*var);
		++var;
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType)
{
	FAssert(m_eClassType >= 0 && m_eClassType < NUM_SAVEGAME_CLASS_TYPES);
	int iSize = 2;

	if (m_eClassType >= 0 && m_eClassType < NUM_SAVEGAME_CLASS_TYPES)
	{
		iSize = enumByteSize[m_eClassType];
	}

	// Just writing the bytes works because it's little endian and the variable in question is 4 bytes.
	// The idea is that it skips writing 1 or 3 bytes, but only in cases where we know they will always be 0.
	Write((byte*)&eType, iSize);
}

void CvSavegameWriter::GenerateTranslationTable()
{
	FAssertMsg(save_conversion_table.size() == 0, "Conversion table only needs to be set once");

	CvString szString;

	for (JITarrayTypes eType = FIRST_JIT_ARRAY; eType < NUM_JITarrayTypes; ++eType)
	{
		if (isConversionArray(eType))
		{
			short iLength = getArrayLength(eType);
			if (iLength == 0)
			{
				// no need to save empty arrays
				continue;
			}

			szString = getArrayName(eType);
			Write(szString);
			Write(iLength);

			const char* szPrefix = getArrayPrefix(eType);
			int iLengthPrefix = strlen(szPrefix);

			for (int i = 0; i < iLength; ++i)
			{
				const char* szType = getArrayType(eType, i);
				szString = szType + iLengthPrefix;
				Write(szString);
			}
		}
	}

	// end of table. Marked with an empty string.
	szString.clear();
	Write(szString);

	// copy the temp saved vector into the permanent vector for the conversion table
	save_conversion_table = m_vector;
}

int getNumSavedEnumValuesArea();
int getNumSavedEnumValuesMap();
int getNumSavedEnumValuesPlot();
int getNumSavedEnumValuesUnit();
int getNumSavedEnumValuesUnitAI();

void CvSavegameWriter::WriteTranslationTable()
{
	this->m_vector = save_conversion_table;

	// save the byte size of SavegameVariableTypes for each class

	byte iLength = NUM_SAVEGAME_CLASS_TYPES;
	Write(iLength);

	for (SavegameClassTypes eClassType = FIRST_SAVEGAME_CLASS_TYPES; eClassType < NUM_SAVEGAME_CLASS_TYPES; ++eClassType)
	{
		int iCount = 2000; // default to 2 bytes. Should be overwritten unless there is a missing switch-case

		// use switch case to ensure all cases are covered
		// it catches the case where a new class is added, but this function isn't updated
		// we want an assert asap if that happens, like every time a game is saved, even auto save when starting a new game
		switch (eClassType)
		{
		case SAVEGAME_CLASS_AREA:        iCount = getNumSavedEnumValuesArea       (); break;
		case SAVEGAME_CLASS_MAP:         iCount = getNumSavedEnumValuesMap        (); break;
		case SAVEGAME_CLASS_PLOT:        iCount = getNumSavedEnumValuesPlot       (); break;
		case SAVEGAME_CLASS_UNIT:        iCount = getNumSavedEnumValuesUnit       (); break;
		case SAVEGAME_CLASS_UNIT_AI:     iCount = getNumSavedEnumValuesUnitAI     (); break;
		default:
			FAssertMsg(false, "missing case");
		}

		enumByteSize[eClassType] = iCount <= 0x100 ? 1 : 2;
		Write(enumByteSize[eClassType]);
	}
}

///
///
///  base classes
///
///

CvSavegameReaderBase::CvSavegameReaderBase(FDataStreamBase* pStream)
	: m_pStream(pStream)
{
	pStream->Read(&m_iSize);
	m_MemoryAllocation = NULL;
	if (m_iSize > 0)
	{
		m_MemoryAllocation = new byte[m_iSize];
		pStream->Read(m_iSize, m_MemoryAllocation);
	}
	m_Memory = m_MemoryAllocation;
	m_MemoryEnd = m_Memory + m_iSize;
}

CvSavegameReaderBase::~CvSavegameReaderBase()
{
	// first check if the entire savegame have been read
	int iBytesLeft = m_MemoryAllocation + m_iSize - m_Memory;

	if (iBytesLeft != 0)
	{
		// there are leftover bytes. Display an error message because the savegame code is broken and should be fixed ASAP.

		FAssertMsg(false, CvString::format("Savegame read error (%d unread bytes)", iBytesLeft).c_str());
		char szMessage[1024];

		sprintf(szMessage, "There are unread leftover bytes in the savegame");
		gDLL->MessageBox(szMessage, "Savegame read Error");
	}

	SAFE_DELETE_ARRAY(m_MemoryAllocation);
}

void CvSavegameReaderBase::Read(byte* var, unsigned int iSize)
{
	for (unsigned int i = 0; i < iSize; ++i)
	{
		if (m_Memory == m_MemoryEnd)
		{
			FAssertMsg(false, "Savegame read error (not enough bytes)");
			char szMessage[1024];

			sprintf(szMessage, "Tried to read more bytes than there are saved in the savegame");
			gDLL->MessageBox(szMessage, "Savegame read Error");

			return;
		}
		var[i] = *m_Memory;
		++m_Memory;
	}
}



CvSavegameWriterBase::CvSavegameWriterBase(FDataStreamBase* pStream)
	: m_pStream(pStream)
{
}

void CvSavegameWriterBase::WriteFile()
{
	unsigned int iSize = m_vector.size();
	m_pStream->Write(iSize);

	byte* array = &m_vector.front();
	if (iSize > 0)
	{
		m_pStream->Write(iSize, array);
	}
}
