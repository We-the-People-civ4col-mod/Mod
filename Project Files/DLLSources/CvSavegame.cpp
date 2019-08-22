#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

//
// Classes to handle savegames
//
// The idea is to keep as much as possible as a byte array in the DLL and not read/write every single variable to the exe.
// The benefit of using a single array is that it allows compression.
//


enum SavegameVariableTypes
{
	Save_END,
};



CvSavegameReader::CvSavegameReader(FDataStreamBase* pStream)
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
}

CvSavegameReader::~CvSavegameReader()
{
	SAFE_DELETE_ARRAY(m_MemoryAllocation);
}


void CvSavegameReader::Read(byte* var, unsigned int iSize)
{
	for (unsigned int i = 0; i < iSize; ++i)
	{
		FAssertMsg(m_Memory < (m_MemoryAllocation + m_iSize), "Savegame malfunction");
		var[i] = *m_Memory;
		++m_Memory;
	}
}

SavegameVariableTypes CvSavegameReader::ReadSwitch()
{
	short iTemp;
	Read(iTemp);
	return static_cast<SavegameVariableTypes>(iTemp);
}



///
///
///
///
///


CvSavegameWriter::CvSavegameWriter(FDataStreamBase* pStream)
	: m_pStream(pStream)
{
}

void CvSavegameWriter::WriteFile()
{
	unsigned int iSize = m_vector.size();
	m_pStream->Write(iSize);

	byte* array = &m_vector.front();
	if (iSize > 0)
	{
		m_pStream->Write(iSize, array);
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
	short iBuffer = eType;
	Write(iBuffer);
}
