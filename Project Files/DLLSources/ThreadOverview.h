#pragma once

#define MULTICORE


class ThreadOverviewClass
{
	friend struct Threads;
public:
	ThreadOverviewClass() : m_bMultithreaded(false) {}

	inline bool isMultiThreaded() const
	{
#ifdef MULTICORE
		return m_bMultithreaded;
#else
		return false;
#endif
	}

private:
	bool m_bMultithreaded;
};


extern ThreadOverviewClass ThreadOverview;
