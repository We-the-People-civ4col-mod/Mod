#include "CvGameCoreDLL.h"

#include "CvSavegame.h"
#include "FVariableSystem.h"
#include "CvPopupInfo.h"
#include "CvDiploParameters.h"

//
// Classes to handle savegames
//
// The idea is to keep as much as possible as a byte array in the DLL and not read/write every single variable to the exe.
// The benefit of using a single array is that it allows compression.
//

// conversion table
// generated on load. look up is [xml file][index]
// index is the value from the savegame while the returned data is the index of the same Type string with the current xml files
// This allows savegames to still work even if the order in xml is changed, including adding new stuff in the middle.
// Do note that -1 will be used if something is removed from xml
std::vector< short > conversion_table[NUM_JITarrayTypes];

// stores the byte size of SavegameClassTypes for each class
// assigned automatically when saving. It's 1 byte if there are less than 257 variables (fits in unsigned byte)
// it automatically becomes two bytes if there are more variables.
// this is saved and on load the saved values are used instead of the current ones. This allows savegames to survive changing the byte size.
// This will in most cases save one byte for each variable saved, something which quickly adds up.
// It's easy to use since it auto configures to the enums. When a programmer adds "too many" enum values, the setting automatically change to match.
char enumByteSize[NUM_SAVEGAME_CLASS_TYPES];

// savegame string indicator for the bytesize array
const char* szEnumByteSize = "enumByteSize";

// same as enumByteSize, but for xml entries.
// it's however set once at startup and then used as ROM.
// It's only used for saving as loading will rely on data from conversion_table instead.
char xmlEnumByteSize[NUM_JITarrayTypes];

// we need the ability to save both positive and negative xml entries because negative are used for no, invalid etc
// however saving a signed byte limits the max length to 128 entries before switching to 2 byte saving
// in order to use 1 byte as much as possible, adding an offset and then saving unsigned will allow saving those -1 etc.
// the constant here sets how many negative values we reserve room for. Make it higher than 2 in order to reserve for future expansion.
// this can be changed later, but with backward compatibility it's better to reserve a few extra early on.
const int XML_ENUM_OFFSET = 6;

// name says it all. It's a shortcut for cleaner code rather than calculating with the offset each time the number is needed.
const int XML_ENUM_MAX_LENGTH_ONE_BYTE = 0x100 - XML_ENUM_OFFSET;

const char* getSavedEnumNameMap(SavegameVariableTypes eType);
const char* getSavedEnumNamePlot(SavegameVariableTypes eType);
const char* getSavedEnumNameUnit(SavegameVariableTypes eType);
const char* getSavedEnumNameUnitAi(SavegameVariableTypes eType);
const char* getSavedEnumNameCity(SavegameVariableTypes eType);
const char* getSavedEnumNameCityAi(SavegameVariableTypes eType);
const char* getSavedEnumNamePlayer(SavegameVariableTypes eType);
const char* getSavedEnumNamePlayerAi(SavegameVariableTypes eType);
const char* getSavedEnumNamePopupInfo(SavegameVariableTypes eType);
const char* getSavedEnumNameDiploParameters(SavegameVariableTypes eType);
const char* getSavedEnumNameTalkingHeadMessage(SavegameVariableTypes eType);
const char* getSavedEnumNameTradeRoute(SavegameVariableTypes eType);
const char* getSavedEnumNameTradeRouteGroup(SavegameVariableTypes eType);
const char* getSavedEnumNameSelectionGroup(SavegameVariableTypes eType);
const char* getSavedEnumNameSelectionGroupAi(SavegameVariableTypes eType);
const char* getSavedEnumNameGame(SavegameVariableTypes eType);
const char* getSavedEnumNameGameAi(SavegameVariableTypes eType);
const char* getSavedEnumNameDeal(SavegameVariableTypes eType);
const char* getSavedEnumNameReplayMessage(SavegameVariableTypes eType);
const char* getSavedEnumNameTeam(SavegameVariableTypes eType);
const char* getSavedEnumNameTeamAI(SavegameVariableTypes eType);

const char* getSavedEnumName(SavegameClassTypes eClass, SavegameVariableTypes eType)
{
	switch (eClass)
	{
	case SAVEGAME_CLASS_MAP: return getSavedEnumNameMap(eType);
	case SAVEGAME_CLASS_PLOT: return getSavedEnumNamePlot(eType);
	case SAVEGAME_CLASS_UNIT: return getSavedEnumNameUnit(eType);
	case SAVEGAME_CLASS_UNIT_AI: return getSavedEnumNameUnitAi(eType);
	case SAVEGAME_CLASS_CITY: return getSavedEnumNameCity(eType);
	case SAVEGAME_CLASS_CITY_AI: return getSavedEnumNameCityAi(eType);
	case SAVEGAME_CLASS_PLAYER: return getSavedEnumNamePlayer(eType);
	case SAVEGAME_CLASS_PLAYER_AI: return getSavedEnumNamePlayerAi(eType);
	case SAVEGAME_CLASS_POPUPINFO: return getSavedEnumNamePopupInfo(eType);
	case SAVEGAME_CLASS_DIPLOPARAMETERS: return getSavedEnumNameDiploParameters(eType);
	case SAVEGAME_CLASS_TALKINGHEADMESSAGE: return getSavedEnumNameTalkingHeadMessage(eType);
	case SAVEGAME_CLASS_TRADEROUTE: return getSavedEnumNameTradeRoute(eType);
	case SAVEGAME_CLASS_TRADEROUTEGROUP: return getSavedEnumNameTradeRouteGroup(eType);
	case SAVEGAME_CLASS_SELECTIONGROUP: return getSavedEnumNameSelectionGroup(eType);
	case SAVEGAME_CLASS_SELECTIONGROUP_AI: return getSavedEnumNameSelectionGroupAi(eType);
	case SAVEGAME_CLASS_GAME: return getSavedEnumNameGame(eType);
	case SAVEGAME_CLASS_GAME_AI: return getSavedEnumNameGameAi(eType);
	case SAVEGAME_CLASS_DEAL: return getSavedEnumNameDeal(eType);
	case SAVEGAME_CLASS_REPLAYMESSAGE: return getSavedEnumNameReplayMessage(eType);
	case SAVEGAME_CLASS_TEAM: return getSavedEnumNameTeam(eType);
	case SAVEGAME_CLASS_TEAM_AI: return getSavedEnumNameTeamAI(eType);

	}

	return "";
}

int getByteSizeForXML(JITarrayTypes eType)
{
	if (eType >= 0 && eType < NUM_JITarrayTypes)
	{
		unsigned int iLength = conversion_table[eType].size();
		return iLength <= XML_ENUM_MAX_LENGTH_ONE_BYTE ? 1 : 2;
	}
	// TODO figure out how to set the size of fixed sized variables like JIT_ARRAY_PLAYER
	FAssert(false);
	return 2;
}

// constructor
CvSavegameReader::CvSavegameReader(CvSavegameReaderBase& readerBase)
	: m_ReaderBase(readerBase)
{
	m_eClassType = NUM_SAVEGAME_CLASS_TYPES;

	// read the conversion table

	CvString szString;

	while (true)
	{
		Read(szString);

		if (szString.length() == 0)
		{
			// end of table
			return;
		}
		if (szString == szEnumByteSize)
		{
			// start of savegame. Init data.
			for (JITarrayTypes eArray = FIRST_JIT_ARRAY; eArray < NUM_JITarrayTypes; ++eArray)
			{
				conversion_table[eArray].clear();
			}

			// read the class enum sizes
			byte iSize;
			Read(iSize);
			for (int i = 0; i < iSize; ++i)
			{
				Read(enumByteSize[i]);
			}
		}
		else
		{
			JITarrayTypes eType = getJITArrayTypeFromString(szString.c_str());

			short iLength;
			Read(iLength);

			bool bIsValid = eType >= 0 && eType < NUM_JITarrayTypes;

			if (bIsValid)
			{
				// not strictly needed, but it improves performance to allocate in one go.
				// also by allocating "just enough" we can make sure it doesn't allocate more than needed.
				conversion_table[eType].reserve(iLength);

				FAssert(conversion_table[eType].size() == 0);
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
	}
}

// copy constructor
CvSavegameReader::CvSavegameReader(const CvSavegameReader& reader)
	: m_ReaderBase(reader.m_ReaderBase)
{
	m_eClassType = NUM_SAVEGAME_CLASS_TYPES;
}

bool CvSavegameReader::isDebug() const
{
	return m_ReaderBase.isDebug();
}

void CvSavegameReader::AssignClassType(SavegameClassTypes eType)
{
	m_eClassType = eType;
}

void CvSavegameReader::Read(SavegameVariableTypes& variable)
{
	FAssert(m_eClassType >= FIRST_SAVEGAME_CLASS_TYPES && m_eClassType < NUM_SAVEGAME_CLASS_TYPES);
	int iSize = 2;

	unsigned int padding_prefix = 0;

	if (isDebug())
	{
		Read(padding_prefix);
	}

	if (m_eClassType >= FIRST_SAVEGAME_CLASS_TYPES && m_eClassType < NUM_SAVEGAME_CLASS_TYPES)
	{
		iSize = enumByteSize[m_eClassType];
	}
	variable = (SavegameVariableTypes)ReadUnsignedBytes(iSize);

	if (isDebug())
	{
		unsigned int padding_postfix;
		Read(padding_postfix);
		m_szPreviousType = m_szType;
		Read(m_szType);

		FAssert(m_szType == getSavedEnumName(m_eClassType, variable));
		FAssert(padding_prefix == MAX_UNSIGNED_INT);
		FAssert(padding_postfix == MAX_UNSIGNED_INT);
	}
}

void CvSavegameReader::Read(unsigned long& variable)
{
	Read((byte*)& variable, sizeof(unsigned long));
}

void CvSavegameReader::Read(double& variable)
{
	Read((byte*)& variable, sizeof(double));
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

void CvSavegameReader::Read(char* szString)
{
	CvString szBuffer;
	Read(szBuffer);
	SAFE_DELETE_ARRAY(szString);
	szString = new char[szBuffer.length() + 1];
	strcpy(szString, szBuffer.c_str());
	szString[szBuffer.length()] = 0;
}

void CvSavegameReader::Read(wchar* szString)
{
	CvWString szBuffer;
	Read(szBuffer);
	SAFE_DELETE_ARRAY(szString);
	szString = new wchar[szBuffer.length() + 1];
	int iLength = szBuffer.length();
	szString[iLength] = 0;
	const wchar* szFrom = szBuffer.c_str();
	for (int i = 0; i < iLength; ++i)
	{
		szString[i] = szFrom[i];
	}
}

void CvSavegameReader::Read(BoolArray& baArray)
{
	baArray.Read(*this);
}

void CvSavegameReader::Read(PlayerBoolArrayBase& array)
{
	array.Read(*this);
}

void CvSavegameReader::Read(CvTurnScoreMap& Map)
{
	int iSize;
	Read(iSize);
	for (int iI = 0; iI < iSize; iI++){
		int iTurn;
		int iValue;
		Read(iTurn);
		Read(iValue);
		Map[iTurn]=iValue;
	}
}

void CvSavegameReader::Read(CvEventMap& Map)
{
	int iSize;
	Read(iSize);
	for (int iI = 0; iI < iSize; iI++){
		EventTypes eEvent;
		EventTriggeredData kData;
		Read(eEvent);
		Read(kData); 
		Map[eEvent]=kData;
	}
}

void CvSavegameReader::ReadXmlEnum(int& iVariable, JITarrayTypes eType)
{
	FAssert(eType >= 0 && eType < NUM_JITarrayTypes);

	unsigned int iLength = conversion_table[eType].size();

	if (getByteSizeForXML(eType) == 2)
	{
		iVariable = ReadBytes(2);
	}
	else
	{
		iVariable = ReadUnsignedBytes(1);
		iVariable -= XML_ENUM_OFFSET;
	}
	iVariable = ConvertIndex(eType, iVariable);
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

int CvSavegameReader::ConvertIndex(JITarrayTypes eType, int iIndex) const
{
	JITarrayTypes eBaseType = GetBaseType(eType);

	if (iIndex >= 0 && eBaseType >= 0 && eBaseType < NUM_JITarrayTypes)
	{
		if ((int)conversion_table[eBaseType].size() > iIndex)
		{
			return conversion_table[eBaseType][iIndex];
		}
	}

	return iIndex;
}

int CvSavegameReader::GetXmlSize(JITarrayTypes eType) const
{
	JITarrayTypes eBaseType = GetBaseType(eType);

	if (eBaseType >= 0 && eBaseType < NUM_JITarrayTypes)
	{
		return conversion_table[eBaseType].size();
	}
	return -1;
}

void CvSavegameReader::Read(FVariable             & variable) { variable.read(*this); }
void CvSavegameReader::Read(CvDiploParameters     & variable) { variable.read(*this); }
void CvSavegameReader::Read(CvPopupButtonPython   & variable) { variable.read(*this); }
void CvSavegameReader::Read(CvPopupInfo           & variable) { variable.read(*this); }
void CvSavegameReader::Read(CvTalkingHeadMessage  & variable) { variable.read(*this); }
void CvSavegameReader::Read(CvTradeRouteGroup     & variable) { variable.read(*this); }


///
///
///
///
///

// constructor
CvSavegameWriter::CvSavegameWriter(CvSavegameWriterBase& writerbase)
	: m_vector(writerbase.m_vector)
	, m_writerbase(writerbase)
{
	m_eClassType = NUM_SAVEGAME_CLASS_TYPES;
}

// copy constructor
CvSavegameWriter::CvSavegameWriter(const CvSavegameWriter& writer)
	: m_vector(writer.m_vector)
	, m_writerbase(writer.m_writerbase)
{
	m_eClassType = NUM_SAVEGAME_CLASS_TYPES;
}

bool CvSavegameWriter::isDebug() const
{
	return m_writerbase.isDebug();
}

void CvSavegameWriter::AssignClassType(SavegameClassTypes eType)
{
	m_eClassType = eType;
}

void CvSavegameWriter::Write(unsigned long variable)
{
	Write((byte*)& variable, sizeof(unsigned long));
}

void CvSavegameWriter::Write(double variable)
{
	Write((byte*)& variable, sizeof(double));
}

void CvSavegameWriter::Write(int variable)
{
	Write((byte*)& variable, sizeof(int));
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

void CvSavegameWriter::Write(const CvString& szString)
{
	Write(szString.c_str());
}

void CvSavegameWriter::Write(const CvWString& szString)
{
	Write(szString.c_str());
}

void CvSavegameWriter::Write(const char* szString)
{
	int iLength = strlen(szString);
	for (int i = 0; i <= iLength; ++i)
	{
		Write(szString[i]);
	}
}

void CvSavegameWriter::Write(const wchar* szString)
{
	int iLength = wcslen(szString);
	for (int i = 0; i <= iLength; ++i)
	{
		Write(szString[i]);
	}
}

void CvSavegameWriter::Write(BoolArray& baArray)
{
	baArray.Write(*this);
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, const CvString& szString)
{
	if (szString.length() > 0)
	{
		Write(eType);
		Write(szString);
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, const CvWString& szString)
{
	if (szString.length() > 0)
	{
		Write(eType);
		Write(szString);
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, const char* szString)
{
	if (szString != NULL)
	{
		Write(eType);
		Write(szString);
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, const wchar* szString)
{
	if (szString != NULL)
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

void CvSavegameWriter::Write(SavegameVariableTypes eType, const PlayerBoolArrayBase& array)
{
	if (array.hasContent())
	{
		Write(eType);
		array.Write(*this);
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, const IDInfo& idInfo)
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
void CvSavegameWriter::Write(SavegameVariableTypes eType, const CvTurnScoreMap& Map)
{
	int iSize=Map.size();
	if(iSize>0){
		Write(eType);
		Write(iSize);
		for (CvTurnScoreMap::const_iterator it = Map.begin(); it != Map.end(); ++it)
		{
			Write(it->first);
			Write(it->second);
		}
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, CvEventMap& Map)
{
	int iSize=Map.size();
	if(iSize>0){
		Write(eType);
		Write(iSize);
		for (CvEventMap::iterator it = Map.begin(); it != Map.end(); ++it)
		{
			Write(it->first);
			Write(it->second);
		}
	}
}

void CvSavegameWriter::Write(SavegameVariableTypes eType, CvRandom& rand)
{
	Write(eType);
	Write(rand);
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
	FAssert(m_eClassType >= FIRST_SAVEGAME_CLASS_TYPES && m_eClassType < NUM_SAVEGAME_CLASS_TYPES);
	int iSize = 2;

	if (m_eClassType >= FIRST_SAVEGAME_CLASS_TYPES && m_eClassType < NUM_SAVEGAME_CLASS_TYPES)
	{
		iSize = enumByteSize[m_eClassType];
	}

	if (isDebug())
	{
		// first add FF padding to catch off by one or a few bytes
		unsigned int iBuffer = MAX_UNSIGNED_INT;
		Write(iBuffer);
	}

	// Just writing the bytes works because it's little endian and the variable in question is 4 bytes.
	// The idea is that it skips writing 1 or 3 bytes, but only in cases where we know they will always be 0.
	Write((byte*)&eType, iSize);

	if (isDebug())
	{
		// more padding
		unsigned int iBuffer = MAX_UNSIGNED_INT;
		Write(iBuffer);

		// add the name of the variable
		Write(getSavedEnumName(m_eClassType, eType));
	}
}

int CvSavegameWriter::GetXmlByteSize(JITarrayTypes eType)
{
	return m_writerbase.GetXmlByteSize(eType);
}

int CvSavegameWriter::GetXmlSize(JITarrayTypes eType)
{
	JITarrayTypes eBaseType = GetBaseType(eType);

	if (eBaseType >= 0 && eBaseType < NUM_JITarrayTypes)
	{
		int iLength = conversion_table[eBaseType].size();
		if (iLength == 0)
		{
			// add to conversion table
			GetXmlByteSize(eType);
			iLength = conversion_table[eBaseType].size();
		}
		return iLength;
	}
	return -1;
}

void CvSavegameWriter::WriteXmlEnum(int iVariable, JITarrayTypes eType)
{
	FAssert(eType >= 0 && eType < NUM_JITarrayTypes);

	int iByteSize = GetXmlByteSize(eType);

	switch (iByteSize)
	{
	default:
		FAssert(false); // what happened???
		// fallthrough
	case 2:
	{
		short iBuffer = iVariable;
		Write(iBuffer);
	} break; //so we dont write 3 bytes 
	case 1:
	{
		int iTemp = iVariable + XML_ENUM_OFFSET;
		FAssert(iTemp >= 0 && iTemp < 0x100);
		byte iBuffer = iTemp;
		Write(iBuffer);
	}
	}
}

void CvSavegameWriter::Write(FVariable            &variable) { variable.write(*this); }
void CvSavegameWriter::Write(CvDiploParameters    &variable) { variable.write(*this); }
void CvSavegameWriter::Write(CvPopupButtonPython  &variable) { variable.write(*this); }
void CvSavegameWriter::Write(CvPopupInfo          &variable) { variable.write(*this); }
void CvSavegameWriter::Write(CvTalkingHeadMessage &variable) { variable.write(*this); }
void CvSavegameWriter::Write(CvTradeRouteGroup    &variable) { variable.write(*this); }


///
///
///  base classes
///
///

enum Savegame_baseclass_flags
{
	Savegame_baseclass_flags_debug,
	Savegame_baseclass_flags_compressed //uses minz.h to compress the data
};

CvSavegameBase::CvSavegameBase()
{
	this->m_iFlag = 0;
}

bool CvSavegameBase::isDebug() const
{
	return HasBit(m_iFlag, Savegame_baseclass_flags_debug);
}
bool CvSavegameBase::isCompressed() const
{
	return HasBit(m_iFlag, Savegame_baseclass_flags_compressed);
}

CvSavegameReaderBase::CvSavegameReaderBase(FDataStreamBase* pStream)
	: m_pStream(pStream),
	m_MemoryAllocation(NULL),
	m_iRead(0)
{
	m_pStream->Read(&m_iFlag);
	m_pStream->Read(&m_iSize);
	if (m_iSize > 0)
	{
		m_MemoryAllocation = new byte[std::min((unsigned int)CHUNK_SIZE,m_iSize)];
		FAssert(m_MemoryAllocation);
		int iReturnVal = ReadChunk();
		FAssert(iReturnVal);
	}
}

int CvSavegameReaderBase::ReadChunk()
{
	int iCurrRead = std::min((unsigned int)CHUNK_SIZE, m_iSize - m_iRead);
	if (!isCompressed())
	{
		m_pStream->Read(iCurrRead, m_MemoryAllocation);
	}
	else
	{
		// TODO compression
	}
	m_iRead+=iCurrRead;
	m_Memory = m_MemoryAllocation;
	m_MemoryEnd = m_MemoryAllocation+iCurrRead;
	return iCurrRead;
}

CvSavegameReaderBase::~CvSavegameReaderBase()
{
	// first check if the entire savegame have been read
	int iBytesLeft = (m_iSize - m_iRead) + (m_MemoryEnd - m_Memory);

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
			if(ReadChunk()==0){
			FAssertMsg(false, "Savegame read error (not enough bytes)");
			char szMessage[1024];

			sprintf(szMessage, "Tried to read more bytes than there are saved in the savegame");
			gDLL->MessageBox(szMessage, "Savegame read Error");

			return;
			}
		}
		var[i] = *m_Memory;
		++m_Memory;
	}
}



CvSavegameWriterBase::CvSavegameWriterBase(FDataStreamBase* pStream)
	: m_pStream(pStream)
{
	// set the debug flag
	// TODO: don't hardcode all savegames to include debug info
	SetBit(m_iFlag, Savegame_baseclass_flags_debug);
	// TODO compression and don't hardcore this setting at compile time
	//SetBit(m_iFlag, Savegame_baseclass_flags_compressed);

	// m_table needs to end with an empty string, which is effectively just a 0 byte.
	// Add this as the first byte in m_vector here in the constructor because then it's easily done.
	// Once both vectors are completed, they are merged meaning it doesn't matter which one the termination byte is added to.
	m_vector.push_back(0);
}

void CvSavegameWriterBase::WriteFile()
{
	m_pStream->Write(m_iFlag);

	unsigned int iSize = m_vector.size();
	iSize += m_table.size();
	m_pStream->Write(iSize);
	if(!isCompressed())
	{
		if (m_table.size() > 0)
		{
			m_pStream->Write(m_table.size(), &m_table.front());
		}

		if (m_vector.size() > 0)
		{
			m_pStream->Write(m_vector.size(), &m_vector.front());
		}
	}
	else
	{
		// TODO compression
	}
}

int getNumSavedEnumValuesArea();
int getNumSavedEnumValuesMap();
int getNumSavedEnumValuesPlot();
int getNumSavedEnumValuesUnit();
int getNumSavedEnumValuesUnitAI();
int getNumSavedEnumValuesCity();
int getNumSavedEnumValuesCityAI();
int getNumSavedEnumValuesPlayer();
int getNumSavedEnumValuesPlayerAI();
int getNumSavedEnumValuesPopupInfo();
int getNumSavedEnumValuesDiploParameters();
int getNumSavedEnumValuesTalkingHeadMessage();
int getNumSavedEnumValuesTradeRoute();
int getNumSavedEnumValuesTradeRouteGroup();
int getNumSavedEnumValuesSelectionGroup();
int getNumSavedEnumValuesSelectionGroupAi();
int getNumSavedEnumValuesGame();
int getNumSavedEnumValuesGameAI();
int getNumSavedEnumValuesDeal();
int getNumSavedEnumValuesReplayMessage();
int getNumSavedEnumValuesTeam();
int getNumSavedEnumValuesTeamAI();

void CvSavegameWriterBase::InitSavegame()
{
	for (JITarrayTypes eArray = FIRST_JIT_ARRAY; eArray < NUM_JITarrayTypes; ++eArray)
	{
		conversion_table[eArray].clear();
	}

	WriteTableString(szEnumByteSize);
	byte iSize = NUM_SAVEGAME_CLASS_TYPES;
	m_table.push_back(iSize);

	for (SavegameClassTypes eClassType = FIRST_SAVEGAME_CLASS_TYPES; eClassType < NUM_SAVEGAME_CLASS_TYPES; ++eClassType)
	{
		int iCount = 2000; // default to 2 bytes. Should be overwritten unless there is a missing switch-case

		// use switch case to ensure all cases are covered
		// it catches the case where a new class is added, but this function isn't updated
		// we want an assert asap if that happens, like every time a game is saved, even auto save when starting a new game
		switch (eClassType)
		{
		case SAVEGAME_CLASS_AREA:        iCount = getNumSavedEnumValuesArea();      break;
		case SAVEGAME_CLASS_MAP:         iCount = getNumSavedEnumValuesMap();       break;
		case SAVEGAME_CLASS_PLOT:        iCount = getNumSavedEnumValuesPlot();      break;
		case SAVEGAME_CLASS_UNIT:        iCount = getNumSavedEnumValuesUnit();      break;
		case SAVEGAME_CLASS_UNIT_AI:     iCount = getNumSavedEnumValuesUnitAI();    break;
		case SAVEGAME_CLASS_CITY:        iCount = getNumSavedEnumValuesCity();      break;
		case SAVEGAME_CLASS_CITY_AI:     iCount = getNumSavedEnumValuesCityAI();    break;
		case SAVEGAME_CLASS_PLAYER:      iCount = getNumSavedEnumValuesPlayer();    break;
		case SAVEGAME_CLASS_PLAYER_AI:   iCount = getNumSavedEnumValuesPlayerAI();  break;
		case SAVEGAME_CLASS_POPUPINFO:   iCount = getNumSavedEnumValuesPopupInfo(); break;
		case SAVEGAME_CLASS_DIPLOPARAMETERS:   iCount = getNumSavedEnumValuesDiploParameters(); break;
		case SAVEGAME_CLASS_TALKINGHEADMESSAGE:   iCount = getNumSavedEnumValuesTalkingHeadMessage(); break;
		case SAVEGAME_CLASS_TRADEROUTE:  iCount = getNumSavedEnumValuesTradeRoute(); break;
		case SAVEGAME_CLASS_TRADEROUTEGROUP:  iCount = getNumSavedEnumValuesTradeRouteGroup(); break;
		case SAVEGAME_CLASS_SELECTIONGROUP:  iCount = getNumSavedEnumValuesSelectionGroup(); break;
		case SAVEGAME_CLASS_SELECTIONGROUP_AI:  iCount = getNumSavedEnumValuesSelectionGroupAi(); break;
		case SAVEGAME_CLASS_GAME:  iCount = getNumSavedEnumValuesGame(); break;
		case SAVEGAME_CLASS_GAME_AI:  iCount = getNumSavedEnumValuesGameAI(); break;
		case SAVEGAME_CLASS_DEAL:  iCount = getNumSavedEnumValuesDeal(); break;
		case SAVEGAME_CLASS_REPLAYMESSAGE:  iCount = getNumSavedEnumValuesReplayMessage(); break;
		case SAVEGAME_CLASS_TEAM:  iCount = getNumSavedEnumValuesTeam(); break;
		case SAVEGAME_CLASS_TEAM_AI:  iCount = getNumSavedEnumValuesTeamAI(); break;

		default:
			FAssertMsg(false, "missing case");
		}

		enumByteSize[eClassType] = iCount <= 0x100 ? 1 : 2;
		m_table.push_back(enumByteSize[eClassType]);
	}
}

int CvSavegameWriterBase::GetXmlByteSize(JITarrayTypes eType)
{
	if (eType >= 0 && eType < NUM_JITarrayTypes)
	{
		int iLength = conversion_table[eType].size();

		if (iLength == 0)
		{
			iLength = getArrayLength(eType);
			if (iLength > 0)
			{
				// first set the vector size
				// this is used as the size() is the count used to set byte size
				// yes it cost memory, but that memory is used on load anyway
				// allocating it on save doesn't matter because it's just kept forever once allocated
				conversion_table[eType].reserve(iLength);
				conversion_table[eType].resize(iLength);

				// write the xml file name
				WriteTableString(getArrayName(eType));

				// write the length. It's a little dirty since the Write(short) function isn't available in the base class
				const byte* pChar = (byte*)&iLength;
				m_table.push_back(pChar[0]);
				m_table.push_back(pChar[1]);

				// write all the types in the file in question
				for (int i = 0; i < iLength; ++i)
				{
					WriteTableString(getArrayTypeWithoutPrefix(eType, i));
				}
			}
		}
	}
	return getByteSizeForXML(eType);
}

void CvSavegameWriterBase::WriteTableString(const char *szString)
{
	while (*szString != 0)
	{
		m_table.push_back(*szString);
		++szString;
	}
	// null termination
	m_table.push_back(0);
}


inline void FVariable::read(CvSavegameReader reader)
{
	reader.Read(m_eType);
	if (m_eType==FVARTYPE_STRING)
		reader.Read(m_szValue);
	else
	if (m_eType==FVARTYPE_WSTRING)
		reader.Read(m_wszValue);
	else
		reader.Read(m_dValue);		// read the maximum size of the union
}

inline void FVariable::write(CvSavegameWriter writer)
{
	writer.Write(m_eType);
	if (m_eType==FVARTYPE_STRING)
		writer.Write(m_szValue);
	else
	if (m_eType==FVARTYPE_WSTRING)
		writer.Write(m_wszValue);
	else
		writer.Write(m_dValue);
}