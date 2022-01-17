#pragma once

#ifndef CyData_h
#define CyData_h

#include "InfoArray.h"

class CyInfoArray
{
public:
	int get(int iIndex) const;
	int get1(int iIndex) const;
	int get2(int iIndex) const;
	int get3(int iIndex) const;
	int getSub(int iIndex, int iSubIndex = 0) const;
	int getLength() const;
	int getDimentions() const;
	JITarrayTypes getType(int i) const;

	// constructors using EnumMaps as argument
	// has to specify each possible number of arguments to get the templates/InfoArray constructor working
	CyInfoArray();
	CyInfoArray(const InfoArrayBase& parent);
	template<typename T>
	CyInfoArray(const InfoArray1<T>& parent)
		: m_Array(parent.getType(0), parent.getType(1), parent.getType(2), parent.getType(3))
	{
		m_Array = parent;
	}
	template<typename Ta, int DEFAULT>
	CyInfoArray(const EnumMap<Ta, bool, DEFAULT>& em)
	: m_Array(VARINFO<Ta>::JIT, JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE)
	{
		InfoArray<Ta> buffer;
		buffer.assignFrom(em);
		m_Array = buffer;
	}
	template<typename Ta, typename Tb,  int DEFAULT>
	CyInfoArray(const EnumMap<Ta, Tb, DEFAULT>& em)
		: m_Array(VARINFO<Ta>::JIT, VARINFO<Tb>::JIT, JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE)
	{
		InfoArray<Ta, Tb> buffer;
		buffer.assignFrom(em);
		m_Array = buffer;
	}
	template<typename Ta, typename Tb, int DEFAULT>
	CyInfoArray(const EnumMap2D<Ta, Tb, bool, DEFAULT>& em)
		: m_Array(VARINFO<Ta>::JIT, VARINFO<Tb>::JIT, JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE)
	{
		InfoArray2D<Ta, Tb, bool> buffer;
		buffer.assignFrom(em);
		m_Array = buffer;
	}
	template<typename Ta, typename Tb, typename Tc, int DEFAULT>
	CyInfoArray(const EnumMap2D<Ta, Tb, Tc, DEFAULT>& em)
		: m_Array(VARINFO<Ta>::JIT, VARINFO<Tb>::JIT, VARINFO<Tc>::JIT, JIT_ARRAY_NO_TYPE)
	{
		InfoArray2D<Ta, Tb, Tc> buffer;
		buffer.assignFrom(em);
		m_Array = buffer;
	}

protected:
	// Use an instance rather than a pointer
	// This makes it a lot easier to avoid memory leaks at the cost of copying an entire array a few times
	// It's a read only array anyway. Doesn't matter if python only accesses a copy or read the original directly
	InfoArrayBase m_Array;
};

#endif	// CyMapGenerator_h
