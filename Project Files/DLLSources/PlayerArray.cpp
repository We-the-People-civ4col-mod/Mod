#include "CvGameCoreDLL.h"
#include "CvSavegame.h"
#include "PlayerArray.h"

BOOST_STATIC_ASSERT(MAX_PLAYERS == MAX_TEAMS);

template<class T>
PlayerArrayBase<T>::PlayerArrayBase()
	: m_Array(NULL)
{
}

template<class T>
void PlayerArrayBase<T>::reset()
{
	SAFE_DELETE_ARRAY(m_Array);
}

template<class T>
bool PlayerArrayBase<T>::hasContent()
{
	const PlayerArrayBase<T>* pConstThis = this;
	bool bContents = pConstThis->hasContent();

	if (!bContents && m_Array != NULL)
	{
		reset();
	}

	return bContents;
}

template<class T>
bool PlayerArrayBase<T>::hasContent() const
{
	if (m_Array != NULL)
	{
		for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
		{
			if (get(eTeam) != getDefault())
			{
				return true;
			}
		}
	}
	return false;
}

// savegame format:
// the savegame contains one or more of the following tokens:
// 1st byte:
//  bit 0-5 first entry
//  bit 6 multi byte index
//  bit 7 last token
// 2nd byte last entry in the token (skipped if multi byte is off)
//
// this is followed by the actual array ranging from first entry to last entry (entries in array indexes)
// if last token isn't set, then what follows is another token, also for this PlayerArrayBase
// The multi token for one array approach allows saving indexes 0-3 and then 48 while skipping saving all the 0 values in 4-47.

enum
{
	SAVE_INDEX = SETBITS(6, 0),
	SAVE_MULTI_BYTE = SETBIT(6),
	SAVE_LAST_TOKEN = SETBIT(7),
	SAVE_EMPTY_ARRAY = 0xFF,
};

template<class T>
void PlayerArrayBase<T>::Read(CvSavegameReader& reader)
{
	byte iBuffer;

	reader.Read(iBuffer);

	if (iBuffer == SAVE_EMPTY_ARRAY)
	{
		// empty array
		return;
	}

	// the array has contents. Make sure there is memory to save it in.
	this->allocate();

	while (true)
	{
		byte iFirst = iBuffer & SAVE_INDEX;
		bool bMultiByte = iBuffer & SAVE_MULTI_BYTE;
		bool bLast = iBuffer & SAVE_LAST_TOKEN;

		if (bMultiByte)
		{
			byte iLast;
			reader.Read(iLast);
			for (int i = iFirst; i <= iLast; ++i)
			{
				// use a buffer because then it doesn't matter if T, array size and save size differs.
				T iBuffer;
				reader.Read(iBuffer);
				set(i, iBuffer);
			}
		}
		else
		{
			// single variable token
			T iBuffer;
			reader.Read(iBuffer);
			set(iFirst, iBuffer);
		}

		if (bLast)
		{
			return;
		}
		reader.Read(iBuffer);
	}
}

class interval
{
public:
	byte first;
	byte last;

	interval()
	{
		first = 0;
		last = 0;
	}
};

template<class T>
void PlayerArrayBase<T>::Write(CvSavegameWriter& writer) const
{
	// first support writing an empty array
	// this is likely never needed, but support it for completeness and stability
	if (!hasContent())
	{
		byte iBuffer = SAVE_EMPTY_ARRAY;
		writer.Write(iBuffer);
		return;
	}

	std::vector<interval> tokens(0);
	interval* pInterval = NULL;

	// generate tokens
	// pInterval points to the token currently being created
	// this means it both acts as a pointer to where to store iLast, but also storing the state
	for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
	{
		if (get(eTeam) != getDefault())
		{
			if (pInterval == NULL)
			{
				interval eTemp;
				tokens.push_back(eTemp);
				pInterval = &tokens[tokens.size() - 1];
				pInterval->first = eTeam;
			}
			pInterval->last = eTeam;
		}
		else
		{
			pInterval = NULL;
		}
	}

	int iEnd = tokens.size() - 1;

	// save the tokens
	for (int i = 0; i <= iEnd; ++i)
	{
		byte iFirst = tokens[i].first;
		byte iLast  = tokens[i].last;

		byte iBuffer = iFirst & SAVE_INDEX;

		if (i == iEnd)
		{
			iBuffer |= SAVE_LAST_TOKEN;
		}

		if (iFirst == iLast)
		{
			writer.Write(iBuffer);
			writer.Write(get(iFirst));
		}
		else
		{
			iBuffer |= SAVE_MULTI_BYTE;
			writer.Write(iBuffer);
			writer.Write(iLast);
			for (int i = iFirst; i <= iLast; ++i)
			{
				writer.Write(get(i));
			}
		}
	}
}

// All types used with PlayerArray should be declared here.
// Failure to do so will result in a linking error.

template class PlayerArrayBase <int>;
template class PlayerArrayBase <short>;
template class PlayerArrayBase <char>;

template class PlayerArrayBase <PlayerTypes>;
