#include "CvGameCoreDLL.h"
#include "CvSavegame.h"
#include "PlayerBoolArray.h"

BOOST_STATIC_ASSERT(MAX_PLAYERS <= 64);
BOOST_STATIC_ASSERT(MAX_TEAMS <= 64);

PlayerBoolArrayBase::PlayerBoolArrayBase()
{
	reset();
}

void PlayerBoolArrayBase::reset()
{
	m_aiBits[0] = 0;
	m_aiBits[1] = 0;
}

void PlayerBoolArrayBase::Read(CvSavegameReader& reader)
{
	reader.Read(m_aiBits[0]);
	reader.Read(m_aiBits[1]);
}

void PlayerBoolArrayBase::Write(CvSavegameWriter& writer) const
{
	writer.Write(m_aiBits[0]);
	writer.Write(m_aiBits[1]);
}

