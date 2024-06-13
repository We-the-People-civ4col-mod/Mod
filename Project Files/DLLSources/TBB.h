#pragma once
#ifndef TBB_COLO_H
#define TBB_COLO_H

#include "ThreadOverview.h"

// MULTICORE can be defined or undefined in ThreadOverview.h

#ifdef MULTICORE

#pragma push_macro("free")
#pragma push_macro("new")
#undef free
#undef new
#include "tbb/parallel_for.h"
#include "tbb/parallel_reduce.h"
#include "tbb/blocked_range.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/cache_aligned_allocator.h"
#include "tbb/mutex.h"
#pragma pop_macro("new")
#pragma pop_macro("free")

#else
// singlecore wrappers to allow TBB syntax

namespace tbb
{

	struct mutex
	{
		inline void lock() const {}
		inline void unlock() const {}
	};

	template<typename T>
	class blocked_range
	{
	public:
		blocked_range(T begin, T end, unsigned int grainsize = 1)
			: my_begin(begin)
			, my_end(end)
			, my_grainsize(grainsize)
		{}

		T begin() const { return my_begin; }
		T end() const { return my_end; }

	private:
		T my_begin;
		T my_end;
		unsigned int my_grainsize;
	};

	struct auto_partitioner {};
	struct split {};

}
#endif

struct Threads
{
	template<typename Range, typename Body, typename auto_partitioner>
	static void parallel_reduce(const Range& range, Body& body, const auto_partitioner& partitioner)
	{
#ifdef MULTICORE
		ThreadOverview.m_bMultithreaded = true;
		tbb::parallel_reduce(range, body, partitioner);
		ThreadOverview.m_bMultithreaded = false;
#else
		body(range);
#endif
}

	template<typename Range, typename Body, typename auto_partitioner>
	static void parallel_for(const Range& range, Body& body, const auto_partitioner& partitioner)
	{
#ifdef MULTICORE
		ThreadOverview.m_bMultithreaded = true;
		tbb::parallel_for(range, body, partitioner);
		ThreadOverview.m_bMultithreaded = false;
#else
		body(range);
#endif
	}
};

#endif
