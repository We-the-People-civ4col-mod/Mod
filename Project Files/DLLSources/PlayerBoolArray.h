#ifndef PLAYER_BOOL_ARRAY_H
#define PLAYER_BOOL_ARRAY_H
#pragma once

/*
 * PlayerBoolArray
 * As the name indicates, it's an array of bools with one bool for each player.
 * The goal is to flatten the memory access and reduce each bool to a single bit in order to fit more data in the CPU cache.
 *
 * a "normal" bool array takes up 4 bytes for the pointer and then 4 bytes for each bool somewhere else.
 * On top of that there are two memory I/O delays, one for the pointer and one for the array itself.
 * This class takes up 8 bytes and only has one I/O delay and it's much more likely to be in the cache already.
 */

class CvSavegameReader;
class CvSavegameWriter;

class PlayerBoolArray
{
public:
	PlayerBoolArray();

	bool get(int iIndex) const;
	void set(int iIndex, bool bValue);

	bool hasContent() const;

	void reset();

	void Read(CvSavegameReader& reader);
	void Write(CvSavegameWriter& writer) const;

private:
	unsigned int m_aiBits[2];
};

//
// Inline functions because the idea is to flatten the memory access for performance reasons
//

inline bool PlayerBoolArray::get(int iIndex) const
{
	FAssert(iIndex >= 0 && iIndex < 64);
	int iBlock = iIndex < 32 ? 0 : 1;
	int iIndexInBlock = iIndex & 0x1F;
	return HasBit(m_aiBits[iBlock], iIndexInBlock);
}

inline void PlayerBoolArray::set(int iIndex, bool bValue)
{
	FAssert(iIndex >= 0 && iIndex < 64);
	int iBlock = iIndex < 32 ? 0 : 1;
	int iIndexInBlock = iIndex & 0x1F;
	SetBit(m_aiBits[iBlock], iIndexInBlock, bValue);
}

inline bool PlayerBoolArray::hasContent() const
{
	return m_aiBits[0] != 0 || m_aiBits[1] != 0;
}

#endif
