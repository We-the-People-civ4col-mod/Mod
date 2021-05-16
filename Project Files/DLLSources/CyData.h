#pragma once

#ifndef CyData_h
#define CyData_h

#include "InfoArray.h"

class CyInfoArray
{
public:
	CyInfoArray();
	CyInfoArray(const InfoArrayMod& parent);
	CyInfoArray(const BoolArray& BArray);

	int get(int iIndex) const;
	int get1(int iIndex) const;
	int get2(int iIndex) const;
	int get3(int iIndex) const;
	int getSub(int iIndex, int iSubIndex = 0) const;
	int getLength() const;
	int getDimentions() const;
	JITarrayTypes getType(int i) const;

protected:
	// Use an instance rather than a pointer
	// This makes it a lot easier to avoid memory leaks at the cost of copying an entire array a few times
	// It's a read only array anyway. Doesn't matter if python only accesses a copy or read the original directly
	InfoArrayMod m_Array;
};

#endif	// CyMapGenerator_h
