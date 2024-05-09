#pragma once

// random.h

#ifndef CIV4_RANDOM_H
#define CIV4_RANDOM_H

#if GLOBAL_DEFINE_USE_OOS_LOGGING
#define OOS_LOG( szLog, iValue ) CvRandom::writeLog( szLog , iValue )
#define OOS_LOG_3( szLog, szText, iValue ) CvRandom::writeLog( szLog , szText, iValue )
#else
#define OOS_LOG( szLog, iValue )
#define OOS_LOG_3( szLog, szText, iValue )
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
	static void writeLog(const char* szLog, const char* szExtraText);
	static void writeLog(const char* szLog, int iData1, int iData2);
	static void writeLog(const char* szLog, const char* szExtraText, int iData);
	static void writeLog(const char* szLog, const char* szExtraText, const char* szText3);

	DllExport void init(unsigned long ulSeed);
	void uninit();
	void reset(unsigned long ulSeed = 0);

	DllExport unsigned short get(unsigned short usNum, char const* pszLog = NULL);  //  Returns value from 0 to num-1 inclusive.
	DllExport float getFloat();
	DllExport float getGaussian(float fMean, float fStandardDeviation);
	int pickValue(AssertCallerData assertData, std::vector<int>& aWeights, char const* pszLog);
	void shuffleArray(std::vector<int>& aNumbers, char const* pszLog);
	DllExport void shuffleSequence(std::vector<int>& aNumbers, char const* pszLog);

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
