#pragma once

// random.h

#ifndef CIV4_RANDOM_H
#define CIV4_RANDOM_H

#if GLOBAL_DEFINE_USE_OOS_LOGGING
#define OOS_LOG( szLog, iValue ) CvRandom::writeLog( szLog , iValue )
#else
#define OOS_LOG( szLog, iValue )
#endif

class CvRandom
{

public:

	DllExport CvRandom();
	DllExport virtual ~CvRandom();

	bool isSorenRand() const;
	void setSorenRand();
	static void writeLog(const CvString& szLog);
	static void writeLog(const char* szLog, int iData);

	DllExport void init(unsigned long ulSeed);
	void uninit();
	void reset(unsigned long ulSeed = 0);

	DllExport unsigned short get(unsigned short usNum, const TCHAR* pszLog = NULL);  //  Returns value from 0 to num-1 inclusive.
	DllExport float getFloat();
	DllExport float getGaussian(float fMean, float fStandardDeviation);
	int pickValue(std::vector<int>& aWeights, const TCHAR* pszLog);
	void shuffleArray(std::vector<int>& aNumbers, const TCHAR* pszLog);
	DllExport void shuffleSequence(std::vector<int>& aNumbers, const TCHAR* pszLog);

	void reseed(unsigned long ulNewValue);
	unsigned long getSeed();

	unsigned long peek() const;

	void setSyncedStatus(bool bNewSetting);

	// for serialization
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);

protected:

	unsigned long m_ulRandomSeed;
	bool m_bSynced;
#ifdef WITH_RANDOM_LOGGING
	bool m_bIsSorenRand;
#endif
};

#endif
