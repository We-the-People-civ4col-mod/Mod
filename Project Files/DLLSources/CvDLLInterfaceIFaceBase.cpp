/*  <advc.127> Cut and pasted from CvDLLInterfaceIFaceBase.h; this function
	is getting too large for an inline definition. */

#include "CvGameCoreDLL.h"
#include "CvDLLInterfaceIFaceBase.h" // </advc.127>
#include "CvGame.h"
#include "CvPlayer.h"
#include "CvPlot.h"

// Uncomment this to silence all message output
//#define NO_ADDMESSAGE

void CvDLLInterfaceIFaceBase::addPlayerMessage(PlayerTypes ePlayer, bool bForce, int iLength, CvWString szString,
		Coordinates coord, LPCTSTR pszSound, InterfaceMessageTypes eType,
		LPCSTR pszIcon, ColorTypes eFlashColor,
		bool bShowOffScreenArrows, bool bShowOnScreenArrows)
{
#ifndef NO_ADDMESSAGE
	addMessage(ePlayer, bForce, iLength, szString,
		coord, pszSound, eType, pszIcon, eFlashColor,
		bShowOffScreenArrows, bShowOnScreenArrows);
#endif
}

void CvDLLInterfaceIFaceBase::addAllPlayersMessage(bool bForce, int iLength, CvWString szString,
		Coordinates coord, LPCTSTR pszSound, InterfaceMessageTypes eType,
		LPCSTR pszIcon, ColorTypes eFlashColor,
		bool bShowOffScreenArrows, bool bShowOnScreenArrows)
{
	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER(ePlayer);
		if (kLoopPlayer.isAlive())
		{
			if (coord.plot()->isRevealed(kLoopPlayer.getTeam(), false))
			{
				addPlayerMessage(kLoopPlayer.getID(), bForce, iLength, szString, coord, pszSound, eType,
																		pszIcon, eFlashColor, bShowOffScreenArrows, bShowOnScreenArrows);
			}
		}
	}
}

void CvDLLInterfaceIFaceBase::addOtherPlayersMessage(PlayerTypes eExcludePlayer, bool bForce, int iLength, CvWString szString,
		Coordinates coord, LPCTSTR pszSound, InterfaceMessageTypes eType,
		LPCSTR pszIcon, ColorTypes eFlashColor,
		bool bShowOffScreenArrows, bool bShowOnScreenArrows)
{
	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER(ePlayer);
		if (ePlayer != eExcludePlayer && kLoopPlayer.isAlive())
		{
			if (coord.plot()->isRevealed(kLoopPlayer.getTeam(), false))
			{
				addPlayerMessage(kLoopPlayer.getID(), bForce, iLength, szString, coord, pszSound, eType,
																		pszIcon, eFlashColor, bShowOffScreenArrows, bShowOnScreenArrows);
			}
		}
	}
}
