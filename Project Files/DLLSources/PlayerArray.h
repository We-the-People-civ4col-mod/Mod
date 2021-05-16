#ifndef PLAYER_ARRAY_H
#define PLAYER_ARRAY_H
#pragma once

/*
 * PlayerArray
 * 
 * Stores an array of length MAX_PLAYERS
 * Use get and set while ignoring memory allocation. It happens automatically. Also it releases the memory in deconstructor.
 * Default value is 0, except for enums where it's -1 (NO_PLAYER etc)
 * Automatically sets the data size to use minimal memory for the task (int->int, but PlayerTypes stores in char)
 * Fully integrated with CvSavegame to save as few bytes as possible while also supporting xml enum conversion.
 */

class CvSavegameReader;
class CvSavegameWriter;

// don't allocate directly. Use PlayerArray or TeamArray
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

	// get functions for certain internal data
	// usually constants, but different depending on which T is in use
	T getDefault() const;
	int getByteSize() const;

private:
	void allocate();

	// T specific get functions. They are specific to the array size used (controlled by T)
	int getIntertal(int iIndex) const;
	void setIntertal(int iIndex, int iValue);

	// assign default value
	// leave empty is bytesize == 1 or default is 0
	void init();

	// Assign the default value to arrays using more than 1 byte
	void init2();
	void init4();

	// pointer to the array itself
	// declared as a union because it simplifies the macros
	// this also allows picking a size at runtime, like based on xml size
	union
	{
		T     * m_Array;
		char  * m_charArray;
		short * m_shortArray;
		int   * m_intArray;
	};
};

// strictly speaking not a bug, but rather a design goal
BOOST_STATIC_ASSERT(sizeof(PlayerArrayBase<int>) == 4);

//
// Inline functions because the idea is to flatten the memory access for performance reasons
//

template<class T>
__forceinline T PlayerArrayBase<T>::get(int iIndex) const
{
	FAssert(iIndex >= 0 && iIndex < MAX_PLAYERS);
	if (!m_Array) return getDefault();
	return (T)getIntertal(iIndex);
}

template<class T>
__forceinline void PlayerArrayBase<T>::set(int iIndex, T iValue)
{
	FAssert(iIndex >= 0 && iIndex < MAX_PLAYERS);
	if (m_Array == NULL)
	{
		if (iValue == getDefault()) return;
		allocate();
	}
	setIntertal(iIndex, iValue);
}

template<class T>
__forceinline void PlayerArrayBase<T>::allocate()
{
	FAssert(m_Array == NULL);
	const int iNumBytes = getByteSize()*MAX_PLAYERS;
	m_charArray = new char[iNumBytes];
	memset(m_charArray, getDefault(), iNumBytes);
	init();
}

template<class T>
__forceinline void PlayerArrayBase<T>::init2()
{
	for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
	{
		m_shortArray[eTeam] = -1;
	}
}

template<class T>
__forceinline void PlayerArrayBase<T>::init4()
{
	for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
	{
		m_intArray[eTeam] = -1;
	}
}

//
// Create the two classes PlayerArray and TeamArray
// They function the same, except they use PlayerTypes and TeamTypes respectively for index.
// This means the baseclass can be used for both teams and players, but one instance can't switch.
// This will prevent the case where an instance is sometimes used with teams and sometimes with players.
//

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
	void add(PlayerTypes ePlayer, T iValue)
	{
		set(ePlayer, get(ePlayer) + iValue);
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
	void add(TeamTypes eTeam, T iValue)
	{
		set(eTeam, get(eTeam) + iValue);
	}
};

//
// declare the type specific functions
// This will allow each type to have a different default value and control how many bytes each variable use in the array
//

// standard primitives like int and short
#define PLAYER_ARRAY_DECLARATION_STANDARD( VAR_TYPE ) \
template<> __forceinline VAR_TYPE PlayerArrayBase<VAR_TYPE>::getDefault() const { return 0; } \
template<> __forceinline int PlayerArrayBase<VAR_TYPE>::getByteSize() const { return sizeof(VAR_TYPE); } \
template<> __forceinline int PlayerArrayBase<VAR_TYPE>::getIntertal(int iIndex) const { return m_Array[iIndex]; } \
template<> __forceinline void PlayerArrayBase<VAR_TYPE>::setIntertal(int iIndex, int iValue) { m_Array[iIndex] = iValue; } \
template<> __forceinline void PlayerArrayBase<VAR_TYPE>::init() { }

// enum types, which can be stored in a single byte and defaults to -1
// example: PlayerTypes. 6 bit is enough storage and it defaults to NO_PLAYER
#define PLAYER_ARRAY_DECLARATION_BYTE( VAR_TYPE, DEFAULT ) \
template<> __forceinline VAR_TYPE PlayerArrayBase<VAR_TYPE>::getDefault() const { return DEFAULT; } \
template<> __forceinline int PlayerArrayBase<VAR_TYPE>::getByteSize() const { return 1; } \
template<> __forceinline int PlayerArrayBase<VAR_TYPE>::getIntertal(int iIndex) const { return m_charArray[iIndex]; } \
template<> __forceinline void PlayerArrayBase<VAR_TYPE>::setIntertal(int iIndex, int iValue) { m_charArray[iIndex] = iValue; } \
template<> __forceinline void PlayerArrayBase<VAR_TYPE>::init() {}

// enum types, which might use iether 1 or 2 bytes
// this is typically enum values linked to xml files
// default is -1 (NO_)
// If the max is known at compile time (like using HARDCODE_XML_VALUES), the compiler should be able
//   to detect that the if statements will always be the same and optimize out the check and the wrong case.
#define PLAYER_ARRAY_DECLARATION_ENUM( VAR_TYPE, NUM_TYPES )                                           \
                                                                                                       \
template<> __forceinline VAR_TYPE PlayerArrayBase<VAR_TYPE>::getDefault() const                               \
{                                                                                                      \
	return (VAR_TYPE)-1;                                                                               \
}                                                                                                      \
                                                                                                       \
template<> __forceinline int PlayerArrayBase<VAR_TYPE>::getByteSize() const                                   \
{                                                                                                      \
	return NUM_TYPES > MAX_CHAR ? 2 : 1;                                                               \
}                                                                                                      \
                                                                                                       \
template<> __forceinline int PlayerArrayBase<VAR_TYPE>::getIntertal(int iIndex) const                         \
{                                                                                                      \
	return NUM_TYPES > MAX_CHAR ? m_shortArray[iIndex] : m_charArray[iIndex];                          \
}                                                                                                      \
                                                                                                       \
template<> __forceinline void PlayerArrayBase<VAR_TYPE>::setIntertal(int iIndex, int iValue)                  \
{                                                                                                      \
    const VAR_TYPE eMax = NUM_TYPES; /* prevents mixing VAR_TYPE and NUM_TYPES from different enums */ \
	if (NUM_TYPES > MAX_CHAR)                                                                          \
	{                                                                                                  \
		m_shortArray[iIndex] = iValue;                                                                 \
	}                                                                                                  \
	else                                                                                               \
	{                                                                                                  \
		m_charArray[iIndex] = iValue;                                                                  \
	}                                                                                                  \
}                                                                                                      \
                                                                                                       \
template<> __forceinline void PlayerArrayBase<VAR_TYPE>::init()                                               \
{                                                                                                      \
    if (NUM_TYPES > MAX_CHAR) init2();                                                                 \
}

// Note: this reserves room for NUM_TYPES in the byte, which is usually not needed.
// It's kept this way because then it won't cause a severe bug should the number be needed to be included at some point.
// Also it makes the code more readable if it can just rely on MAX_CHAR.
// This only affects xml files with precisely 128 entries, which is such an edge case that it might never happen.

//
// declarations of all the variables we can for the template
//

// standard variables (int family)
PLAYER_ARRAY_DECLARATION_STANDARD(int)
PLAYER_ARRAY_DECLARATION_STANDARD(short)
PLAYER_ARRAY_DECLARATION_STANDARD(char)
PLAYER_ARRAY_DECLARATION_STANDARD(unsigned int)
PLAYER_ARRAY_DECLARATION_STANDARD(unsigned short)
PLAYER_ARRAY_DECLARATION_STANDARD(unsigned char)

// xml enums
// PLAYER_ARRAY_DECLARATION_ENUM (variable type, NUM_*_TYPES)
// The last argument is used to determine if the array should use char or short, hence it needs awareness of the max value


// single byte
// PLAYER_ARRAY_DECLARATION_BYTE (variable type, default value)
PLAYER_ARRAY_DECLARATION_BYTE(PlayerTypes, NO_PLAYER)


template<class T> inline T PlayerArrayBase<T>::getDefault() const
{
	// intentionally left blank
	// If you get an error, then it's because the code use a template variable, which isn't in the declarations
	// add the declaration rather than fixing this function

	// The goal of this function is to cause an error at compile time with a reference to where the error is in the code
	// If it's not here, then the compiler will be fine, but the linker will fail without telling which file the fix should be in.

	// Remember to also add the type to the end of PlayerArray.cpp
	// Failure to update the cpp file will cause a linker error, but not compiler error, hence no hint to where to fix the problem.
}

#endif
