/*  <advc.127> Cut and pasted from CvDLLInterfaceIFaceBase.h; this function
	is getting too large for an inline definition. */

#include "CvGameCoreDLL.h"
#include "CvDLLInterfaceIFaceBase.h" // </advc.127>
#include "CvGame.h"
#include "CvPlayer.h"
#include "CvPlot.h"

// Uncomment this to silence all message output
//#define NO_ADDMESSAGE

void CvDLLInterfaceIFaceBase::addPlayerMessage(PlayerTypes ePlayer, bool bForce,
	int iLength, CvWString szString, LPCTSTR pszSound,
	InterfaceMessageTypes eType, LPCSTR pszIcon, ColorTypes eFlashColor,
	int iFlashX, int iFlashY, bool bShowOffScreenArrows,
	bool bShowOnScreenArrows)
{
#ifndef NO_ADDMESSAGE
	addMessage(ePlayer, bForce, iLength, szString,
		pszSound, eType, pszIcon, eFlashColor, iFlashX, iFlashY,
		bShowOffScreenArrows, bShowOnScreenArrows);
#endif
}

void CvDLLInterfaceIFaceBase::addPlayerMessage(PlayerTypes ePlayer, bool bForce,
	int iLength, CvWString szString, CvPlot const& kPlot,
	LPCTSTR pszSound, InterfaceMessageTypes eType, LPCSTR pszIcon,
	ColorTypes eFlashColor, bool bShowOffScreenArrows, bool bShowOnScreenArrows)
{
	addPlayerMessage(ePlayer, bForce, iLength, szString, pszSound, eType, pszIcon,
			eFlashColor, kPlot.getX(), kPlot.getY(),
			bShowOffScreenArrows, bShowOnScreenArrows);
}
