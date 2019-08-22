#ifndef SAVEGAME_H
#define SAVEGAME_H

class FDataStreamBase;
enum SavegameVariableTypes;

class CvSavegameReader
{
public:
	CvSavegameReader(FDataStreamBase* pStream);

	~CvSavegameReader();

	template<class T>
	void Read(T& variable)	{ Read((byte*)&variable, sizeof(T));}

	SavegameVariableTypes ReadSwitch();

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

	template<class T>
	void Write(T variable)	{ Write((byte*)&variable, sizeof(T)); }

	void Write(SavegameVariableTypes eType);

	template<class T>
	void Write(SavegameVariableTypes eType, T eVariable, T eDefault)
	{
		if (eVariable != eDefault)
		{
			Write(eType);
			Write(eVariable);
		}
	}
	
	void WriteFile();

private:

	void Write(byte*var, unsigned int iSize);

	FDataStreamBase* m_pStream;
	std::vector<byte> m_vector;
};

#endif
