#ifndef BOOL_ARRAY_H
#define BOOL_ARRAY_H
#pragma once
// BoolArray.h

/*
 * BoolArray is used more or less like a just-in-time array.
 * Functions are named the same and to the outside world they behave the same. 
 */

class CvXMLLoadUtility;
class CvSavegameReader;
class CvSavegameWriter;

class BoolArray
{
private:
	unsigned int* m_iArray;
	const unsigned short m_iLength;
	const unsigned char m_iType;
	const bool m_bDefault : 1;

public:
	BoolArray(JITarrayTypes eType, bool bDefault = false);

	~BoolArray();

	// reset content of an array if it is allocated
	void reset();

	// non-allocated arrays contains only default values
	// this is a really fast content check without even looking at array content
	// note that it is possible to have allocated arrays with only default content
	inline bool isAllocated() const
	{
		return m_iArray != NULL;
	}
	
	inline int length() const
	{
		return m_iLength;
	}

	// get stored value
	bool get(int iIndex) const;

	// assign argument to storage
	void set(bool bValue, int iIndex);

	// set with bound checks. Does nothing if out of bound indexes are used
	// Slower, use only if out of bound indexes can happen and if such arguments are ok to ignore
	// Mostly used to avoid issues with index -1
	void safeSet(bool bValue, int iIndex);

	JITarrayTypes getType() const
	{
		return static_cast<JITarrayTypes>(m_iType);
	}

	bool hasContent(bool bRelease = true);
	inline bool isEmpty(bool bRelease = true)
	{
		return !hasContent(bRelease);
	}

	int getNumUsedElements() const;

	int getNumTrueElements() const;

	// binary add a 1D InfoArray into the array
	// returns true if something changed
	bool add(const InfoArrayBase& kIarray);

	// bEnable can be used like "uiFlag > x" to make oneline conditional loads
	void read (FDataStreamBase* pStream, bool bEnable);
	void write(FDataStreamBase* pStream, bool bEnable);
	void Read (FDataStreamBase* pStream);
	void Write(FDataStreamBase* pStream);
	void Read(CvSavegameReader& reader);
	void Write(CvSavegameWriter& writer);
	void read(CvXMLLoadUtility* pXML, const char* sTag);

	BoolArray& operator=(const BoolArray &rhs);

private:
	// should never be called. Protects against silent bugs from swapping arguments
	// marked private to throw the error at the caller rather than this header file (if possible)
	// Functions intentionally lacks an implementation.
	void set(int bValue, int iIndex);
	void safeSet(int bValue, int iIndex);
};
#endif
