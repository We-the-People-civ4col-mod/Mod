#include "CvGameCoreDLL.h"
#include "CvSavegame.h"
#include "PlayerBoolArray.h"

PlayerBoolArray::PlayerBoolArray()
{
	reset();
}

void PlayerBoolArray::reset()
{
	m_aiBits[0] = 0;
	m_aiBits[1] = 0;
}

void PlayerBoolArray::Read(CvSavegameReader& reader)
{
	reader.Read(m_aiBits[0]);
	reader.Read(m_aiBits[1]);
}

void PlayerBoolArray::Write(CvSavegameWriter& writer) const
{
	writer.Write(m_aiBits[0]);
	writer.Write(m_aiBits[1]);
}

