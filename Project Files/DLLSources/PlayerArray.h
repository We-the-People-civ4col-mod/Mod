#ifndef PLAYER_ARRAY_H
#define PLAYER_ARRAY_H
#pragma once

/*
 * PlayerBoolArray
 * As the name indicates, it's an array of bools with one bool for each player.
 * The goal is to flatten the memory access and reduce each bool to a single bit in order to fit more data in the CPU cache.
 *
 * a "normal" bool array takes up 4 bytes for the pointer and then 4 bytes for each bool somewhere else.
 * On top of that there are two memory I/O delays, one for the pointer and one for the array itself.
 * This class takes up 8 bytes and only has one I/O delay making it much more likely to be in the cache already.
 */

class CvSavegameReader;
class CvSavegameWriter;

// don't allocate directly. Use PlayerBoolArray or TeamBoolArray
// they use the same code, but with PlayerTypes and TeamTypes arguments to prevent bugs when teamID != playerID
template<class T>
class PlayerArrayBase
{
protected:
	PlayerArrayBase();

	T get(int iIndex) const;
	void set(int iIndex, T iValue);

public:
	// non-const version will free unused memory
	bool hasContent();
	bool hasContent() const;

	void reset();

	void Read(CvSavegameReader& reader);
	void Write(CvSavegameWriter& writer) const;

private:
	void allocate();

	T* m_array;
};

//
// Inline functions because the idea is to flatten the memory access for performance reasons
//


template<class T>
inline T PlayerArrayBase<T>::get(int iIndex) const
{
	FAssert(iIndex >= 0 && iIndex < MAX_PLAYERS);
	return m_array ? m_array[iIndex] : 0;
}


template<class T>
inline void PlayerArrayBase<T>::set(int iIndex, T iValue)
{
	FAssert(iIndex >= 0 && iIndex < MAX_PLAYERS);
	if (m_array == NULL)
	{
		if (iValue == 0)
		{
			// no need to allocate to assign the default value
			return;
		}
		allocate();
	}
	m_array[iIndex] = iValue;
}

template<class T>
inline void PlayerArrayBase<T>::allocate()
{
	FAssert(m_array == NULL);
	m_array = new T[MAX_PLAYERS];
	memset(m_array, 0, sizeof(T)*MAX_PLAYERS);
}

template<class T>
class PlayerArray : public PlayerArrayBase<T>
{
public:
	T get(PlayerTypes ePlayer) const
	{
		FAssert(ePlayer < MAX_PLAYERS);
		return PlayerArrayBase<T>::get(ePlayer);
	}
	void set(PlayerTypes ePlayer, T iValue)
	{
		FAssert(ePlayer < MAX_PLAYERS);
		PlayerArrayBase<T>::set(ePlayer, iValue);
	}
};

template<class T>
class TeamArray : public PlayerArrayBase<T>
{
public:
	T get(TeamTypes eTeam) const
	{
		FAssert(eTeam < MAX_TEAMS);
		return PlayerArrayBase<T>::get(eTeam);
	}
	void set(TeamTypes eTeam, T iValue)
	{
		FAssert(eTeam < MAX_TEAMS);
		PlayerArrayBase<T>::set(eTeam, iValue);
	}
};

#endif
