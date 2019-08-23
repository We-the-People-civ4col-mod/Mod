#ifndef SAVEGAME_H
#define SAVEGAME_H

class FDataStreamBase;
enum SavegameVariableTypes;

#include "JustInTimeArray.h"
#include "BoolArray.h"

class CvSavegameReader
{
public:
	CvSavegameReader(FDataStreamBase* pStream);

	~CvSavegameReader();

	void Read(int& variable);
	void Read(short& variable);
	void Read(char& variable);

	void Read(unsigned int& variable);
	void Read(unsigned short& variable);
	void Read(byte& variable);
	
	template<class T>
	void Read(JustInTimeArray<T>& jitArray);

	void Read(BoolArray& baArray);

	template<typename T>
	void ReadEnum(T& variable);

	SavegameVariableTypes ReadSwitch();

	// Add all enums used in savegames
	void Read(DirectionTypes        & variable) { ReadEnum(variable); }
	void Read(UnitTypes             & variable) { ReadEnum(variable); }

private:

	void Read(byte* var, unsigned int iSize);

	FDataStreamBase* m_pStream;
	byte* m_Memory;
	byte* m_MemoryAllocation;
	unsigned int m_iSize;
};

class CvSavegameWriter
{
public:
	CvSavegameWriter(FDataStreamBase* pStream);

	template<enum T>
	void Write(T variable);

	void Write(int iVar);
	void Write(short iVar);
	void Write(char iVar);
	void Write(unsigned int iVar);
	void Write(unsigned short iVar);
	void Write(byte iVar);

	void Write(BoolArray& baArray);
	
	template<class T>
	void Write(JustInTimeArray<T>& jitArray);

	void Write(SavegameVariableTypes eType);

	void Write(SavegameVariableTypes eType, BoolArray& baArray);

	template<class T>
	void Write(SavegameVariableTypes eType, T eVariable, T eDefault);

	template<class T>
	void Write(SavegameVariableTypes eType, JustInTimeArray<T>& jitArray);
	
	void WriteFile();

private:

	void Write(byte*var, unsigned int iSize);

	FDataStreamBase* m_pStream;
	std::vector<byte> m_vector;
};

//
// Inline functions to allow easy handling of templates
//

//
// Reader
//

template<typename T>
void CvSavegameReader::ReadEnum(T& variable)
{
	Read((byte*)&variable, sizeof(T));
}

template<class T>
inline void CvSavegameReader::Read(JustInTimeArray<T>& jitArray)
{
	jitArray.Read(this);
}



//
// Writer
//

template<enum T>
inline void CvSavegameWriter::Write(T variable)
{
	Write((byte*)&variable, sizeof(T));
}

template<class T>
inline void CvSavegameWriter::Write(JustInTimeArray<T>& jitArray)
{
	jitArray.Write(this);
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, T eVariable, T eDefault)
{
	if (eVariable != eDefault)
	{
		Write(eType);
		Write(eVariable);
	}
}

template<class T>
inline void Write(SavegameVariableTypes eType, JustInTimeArray<T>& jitArray)
{
	if (jitArray->hasContent())
	{
		Write(eType);
		Write(jitArray);
	}
}


#endif
