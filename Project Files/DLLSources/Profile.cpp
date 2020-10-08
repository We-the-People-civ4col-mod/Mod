
#include "CvGameCoreDLL.h"
#include "Profile.h"

static __declspec(naked)unsigned __int64 __cdecl RDTSC(void)
{
	__asm
	{
		rdtsc
		ret; // return value at EDX : EAX
	}
}

Profiler::Profiler(const char* szName)
	: m_szName(szName)
	, m_iStartTime(RDTSC())
{
}

Profiler::~Profiler()
{
	unsigned __int64 iTime = RDTSC();
	iTime -= m_iStartTime;
	GC.getProfiler().addSample(iTime, m_szName);
}

void ProfilerManager::addSample(unsigned __int64 iTime, const char* szName)
{
	SampleStorageTypes::iterator pIterator;

	// TODO: make thread safe without slowing down single threaded performance

	pIterator = this->m_Samples.find(szName);
	if (pIterator == m_Samples.end())
	{
		m_Samples.insert(std::pair <std::string, storage>( szName, storage(1, iTime)));
	}
	else
	{
		pIterator->second.first++;
		pIterator->second.second += iTime;
	}
}

void ProfilerManager::writeFile() const
{
	if (m_Samples.empty())
	{
		return;
	}

	CvString filename = gDLL->getModName();
	filename.append("Profile output.txt");

	// Using C style file writing
	// The reason is that C++ style apparently has a known bug in our compiler and it won't compile in some projects (like ours).
	// The precise cause is unknown, but the recommendation is to use C style file writing.
	FILE *f = fopen(filename.GetCString(), "w");
	if (f != NULL)
	{
		fprintf(f, "Name\tAverage clocks\tNumber of calls\tTotal clocks\n");

		for (SampleStorageTypes::const_iterator iterator = m_Samples.begin(); iterator != m_Samples.end(); ++iterator)
		{
			const char* name = iterator->first.c_str();
			unsigned __int64 iCount = iterator->second.first;
			unsigned __int64 iTime = iterator->second.second;
			unsigned __int64 iAvg = iTime / iCount;

			fprintf(f, "%s", name);
			fprintf(f, "\t%I64u", iAvg);
			fprintf(f, "\t%I64u", iCount);
			fprintf(f, "\t%I64u", iTime);
			fprintf(f, "\n");
		}
		fclose(f);
	}
}
