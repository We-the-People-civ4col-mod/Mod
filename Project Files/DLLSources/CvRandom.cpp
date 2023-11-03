// random.cpp

#include "CvGameCoreDLL.h"
#include "CvRandom.h"
#include "CvGlobals.h"
#include "CyArgsList.h"
#include "CvGameAI.h"
#include <numeric>
#include <limits>

#include "CvSavegame.h"

#define RANDOM_A      (1103515245)
#define RANDOM_C      (12345)
#define RANDOM_SHIFT  (16)

CvRandom::CvRandom()
	: m_bSynced(false)
{
#ifdef WITH_RANDOM_LOGGING
	m_bIsSorenRand = false;
#endif
	reset();
}


CvRandom::~CvRandom()
{
	uninit();
}

bool CvRandom::isSorenRand() const
{
	// avoid crashing during init
	// also don't log random stuff, which won't cause desyncs
	if (!GC.IsGraphicsInitialized() || gDLL->isGameInitializing() || !m_bSynced)
	{
		return false;
	}

	// always log random stuff in network games
	// an OOS is much easier to track down if this log is available
	if (GC.getGameINLINE().isNetworkMultiPlayer())
	{
		return true;
	}
#ifdef WITH_RANDOM_LOGGING
	return m_bIsSorenRand;
#else
	return false;
#endif
}

void CvRandom::setSorenRand()
{
#ifdef WITH_RANDOM_LOGGING
	m_bIsSorenRand = true;
#endif
}

void CvRandom::writeLog(const CvString& szLog)
{
#if GLOBAL_DEFINE_USE_OOS_LOGGING
	const CvGame& kGame = GC.getGameINLINE();
	if (!kGame.isNetworkMultiPlayer() || !kGame.isFinalInitialized() || !GC.getRandLogging())
	{
		return;
	}
	CvString filename;
	filename.append(CvString::format("Random Player %d.log", GC.getGameINLINE().getActivePlayer()));
	gDLL->logMsg(filename, szLog);
#endif
}

void CvRandom::writeLog(const char* szLog, int iData)
{
#if GLOBAL_DEFINE_USE_OOS_LOGGING
	const CvGame& kGame = GC.getGameINLINE();
	if (!kGame.isNetworkMultiPlayer() || !kGame.isFinalInitialized() || !GC.getRandLogging())
	{
		return;
	}
	CvString filename;
	filename.append(CvString::format("Random Player %d.log", GC.getGameINLINE().getActivePlayer()));
	CvString szText = szLog;
	szText.append(CvString::format(" %d", iData));
	gDLL->logMsg(filename, szText.c_str());
#endif
}

void CvRandom::writeLog(const char* szLog, const char* szExtraText)
{
#if GLOBAL_DEFINE_USE_OOS_LOGGING
	const CvGame& kGame = GC.getGameINLINE();
	if (!kGame.isNetworkMultiPlayer() || !kGame.isFinalInitialized() || !GC.getRandLogging())
	{
		return;
	}
	CvString filename;
	filename.append(CvString::format("Random Player %d.log", GC.getGameINLINE().getActivePlayer()));
	CvString szText = szLog;
	szText.append(CvString::format(" %s", szExtraText));
	gDLL->logMsg(filename, szText.c_str());
#endif
}

void CvRandom::writeLog(const char* szLog, int iData1, int iData2)
{
#if GLOBAL_DEFINE_USE_OOS_LOGGING
	const CvGame& kGame = GC.getGameINLINE();
	if (!kGame.isNetworkMultiPlayer() || !kGame.isFinalInitialized() || !GC.getRandLogging())
	{
		return;
	}
	CvString filename;
	filename.append(CvString::format("Random Player %d.log", GC.getGameINLINE().getActivePlayer()));
	CvString szText = szLog;
	szText.append(CvString::format(" %d %d", iData1, iData2));
	gDLL->logMsg(filename, szText.c_str());
#endif
}

void CvRandom::writeLog(const char* szLog, const char* szExtraText, int iData)
{
#if GLOBAL_DEFINE_USE_OOS_LOGGING
	const CvGame& kGame = GC.getGameINLINE();
	if (!kGame.isNetworkMultiPlayer() || !kGame.isFinalInitialized() || !GC.getRandLogging())
	{
		return;
	}
	CvString filename;
	filename.append(CvString::format("Random Player %d.log", GC.getGameINLINE().getActivePlayer()));
	CvString szText = szLog;
	szText.append(CvString::format(" %s %d", szExtraText, iData));
	gDLL->logMsg(filename, szText.c_str());
#endif
}

void CvRandom::writeLog(const char* szLog, const char* szExtraText, const char* szText3)
{
#if GLOBAL_DEFINE_USE_OOS_LOGGING
	const CvGame& kGame = GC.getGameINLINE();
	if (!kGame.isNetworkMultiPlayer() || !kGame.isFinalInitialized() || !GC.getRandLogging())
	{
		return;
	}
	CvString filename;
	filename.append(CvString::format("Random Player %d.log", GC.getGameINLINE().getActivePlayer()));
	CvString szText = szLog;
	szText.append(CvString::format(" %s %s", szExtraText, szText3));
	gDLL->logMsg(filename, szText.c_str());
#endif
}

void CvRandom::init(unsigned long ulSeed)
{
	//--------------------------------
	// Init saved data
	reset(ulSeed);

	//--------------------------------
	// Init non-saved data
}


void CvRandom::uninit()
{
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvRandom::reset(unsigned long ulSeed)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_ulRandomSeed = ulSeed;
	m_bSynced = false;

	/// random network fix - start - Nightinggale
	std::srand(m_ulRandomSeed);
	/// random network fix - end - Nightinggale

	if (isSorenRand())
	{
		CvString szLog;
		szLog.Format("Reset %llu", getSeed());
		writeLog(szLog);
	}
}


unsigned short CvRandom::get(unsigned short usNum, char const* pszLog)
{
	FAssertMsg(!m_bSynced || GC.isMainThread(), "Random called outside main thread");

	if (pszLog != NULL)
	{
		if (GC.getLogging() && GC.getRandLogging())
		{
			if (GC.getGameINLINE().getTurnSlice() > 0)
			{
				char szOut[1024];
				sprintf(szOut, "Rand = %d on %d (%s)\n", getSeed(), GC.getGameINLINE().getTurnSlice(), pszLog);
				gDLL->messageControlLog(szOut);
			}
		}
	}

	if (pszLog && isSorenRand())
	{
		CvString szLog;
		szLog.Format("%s, %llu, %d", pszLog, getSeed(), usNum);
		writeLog(szLog);
	}

	m_ulRandomSeed = ((RANDOM_A * m_ulRandomSeed) + RANDOM_C);

	/// random network fix - start - Nightinggale
	std::srand(m_ulRandomSeed);
	/// random network fix - end - Nightinggale

	unsigned short us = ((unsigned short)((((m_ulRandomSeed >> RANDOM_SHIFT) & MAX_UNSIGNED_SHORT) * ((unsigned long)usNum)) / (MAX_UNSIGNED_SHORT + 1)));

	if (pszLog && isSorenRand())
	{
		CvString szLog;
		szLog.Format("%llu, %d", getSeed(), us);
		writeLog(szLog);
	}

	return us;
}


float CvRandom::getFloat()
{
	return (((float)(get(MAX_UNSIGNED_SHORT))) / ((float)MAX_UNSIGNED_SHORT));
}

/* boxmuller.c           Implements the Polar form of the Box-Muller
Transformation

(c) Copyright 1994, Everett F. Carter Jr.
Permission is granted by the author to use
this software for any application provided this
copyright notice is preserved.

*/

float CvRandom::getGaussian(float fMean, float fStandardDeviation)
{
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do
		{
			x1 = 2.0f * getFloat() - 1.0f;
			x2 = 2.0f * getFloat() - 1.0f;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0f );

		w = sqrt( (-2.0f * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( fMean + y1 * fStandardDeviation );
}

int CvRandom::pickValue(std::vector<int>& aWeights, char const* pszLog)
{
	int iTotalWeights = std::accumulate(aWeights.begin(), aWeights.end(), 0);
	FAssert(iTotalWeights >= 0);
	FAssert(iTotalWeights <= std::numeric_limits<unsigned short>::max());

	int iValue = get(iTotalWeights, pszLog);
	int iSum = 0;
	for (int i = 0; i < (int)aWeights.size(); ++i)
	{
		iSum += aWeights[i];
		if (iValue < iSum)
		{
			return i;
		}
	}

	FAssert(false);
	return 0;
}

void CvRandom::shuffleArray(std::vector<int>& aNumbers, char const* pszLog)
{
	for (uint iI = 0; iI < aNumbers.size(); iI++)
	{
		int iJ = (get(aNumbers.size() - iI, pszLog) + iI);

		if (iI != iJ)
		{
			int iTemp = aNumbers[iI];
			aNumbers[iI] = aNumbers[iJ];
			aNumbers[iJ] = iTemp;
		}
	}
}

void CvRandom::shuffleSequence(std::vector<int>& aNumbers, char const* pszLog)
{
	for (uint i = 0; i < aNumbers.size(); ++i)
	{
		aNumbers[i] = i;
	}
	shuffleArray(aNumbers, pszLog);
}


void CvRandom::reseed(unsigned long ulNewValue)
{
	m_ulRandomSeed = ulNewValue;
	m_bSynced = false;
	/// random network fix - start - Nightinggale
	std::srand(m_ulRandomSeed);
	/// random network fix - end - Nightinggale

	if (isSorenRand())
	{
		CvString szLog;
		szLog.Format("Reseed %llu", getSeed());
		writeLog(szLog);
	}
}


unsigned long CvRandom::getSeed()
{
	return m_ulRandomSeed;
}


void CvRandom::read(FDataStreamBase* pStream)
{
	reset();

	pStream->Read(&m_ulRandomSeed);
	/// random network fix - start - Nightinggale
	std::srand(m_ulRandomSeed);
	/// random network fix - end - Nightinggale

	if (isSorenRand())
	{
		CvString szLog;
		szLog.Format("Load old %llu", getSeed());
		writeLog(szLog);
	}
}


void CvRandom::write(FDataStreamBase* pStream)
{
	pStream->Write(m_ulRandomSeed);
}

//this is good as is
void CvRandom::read(CvSavegameReader reader)
{
	reset();

	reader.Read(m_ulRandomSeed);
	/// random network fix - start - Nightinggale
	std::srand(m_ulRandomSeed);
	/// random network fix - end - Nightinggale

	if (isSorenRand())
	{
		CvString szLog;
		szLog.Format("Load new %llu", getSeed());
		writeLog(szLog);
	}
}


void CvRandom::write(CvSavegameWriter writer)
{
	writer.Write(m_ulRandomSeed);
}

// Returns the next random seed. Useful for inspecting\comparing the state of the RNG without
// being forced to actually generate a new random number.
unsigned long CvRandom::peek() const
{
	return ((RANDOM_A * m_ulRandomSeed) + RANDOM_C);
}

void CvRandom::setSyncedStatus(bool bNewSetting)
{
	m_bSynced = bNewSetting;
}
