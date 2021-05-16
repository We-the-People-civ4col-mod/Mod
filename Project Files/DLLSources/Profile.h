#ifndef PROFILE_H
#define PROFILE_H

#ifdef PROFILING_ENABLED
#define MOD_PROFILE( x ) Profiler profileSession( x );
#else
#define MOD_PROFILE( x )
#endif

class Profiler
{
public:
	Profiler(const char* szName);
	~Profiler();

private:
	const char* m_szName;
	unsigned __int64 m_iStartTime;
};

class ProfilerManager
{
public:
	void addSample(unsigned __int64 iTime, const char* szName);

	void writeFile() const;

private:
	typedef std::pair<unsigned __int64, unsigned __int64> storage;
	typedef stdext::hash_map< std::string, storage > SampleStorageTypes;

	SampleStorageTypes m_Samples;
};

#endif
