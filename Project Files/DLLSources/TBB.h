#pragma once
#ifndef TBB_COLO_H
#define TBB_COLO_H

#include <deque>
#include "ThreadOverview.h"

// MULTICORE can be defined or undefined in ThreadOverview.h

#ifdef MULTICORE

#pragma push_macro("free")
#pragma push_macro("new")
#undef free
#undef new
#include "lib/tbb/atomic.h"
#include "lib/tbb/blocked_range.h"
#include "lib/tbb/cache_aligned_allocator.h"
#include "lib/tbb/concurrent_queue.h"
#include "lib/tbb/parallel_for.h"
#include "lib/tbb/parallel_reduce.h"
#include "lib/tbb/mutex.h"
#include "lib/tbb/partitioner.h"
#include "lib/tbb/task_group.h"
#include "lib/tbb/task_scheduler_init.h"
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

		class scoped_lock
		{
		public:
			scoped_lock() : m_pMutex(NULL) {}

			explicit scoped_lock(const mutex& m) : m_pMutex(NULL)
			{
				acquire(m);
			}

			~scoped_lock()
			{
				release();
			}

			void acquire(const mutex& m)
			{
				// no-op lock, but keep state for symmetry
				m_pMutex = &m;
				// const_cast is fine here because lock() is const anyway
				const_cast<mutex*>(m_pMutex)->lock();
			}

			bool try_acquire(const mutex& m)
			{
				acquire(m);
				return true;
			}

			void release()
			{
				if (m_pMutex != NULL)
				{
					const_cast<mutex*>(m_pMutex)->unlock();
					m_pMutex = NULL;
				}
			}

		private:
			const mutex* m_pMutex;
		};
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
	struct simple_partitioner {};
	struct split {};

	template<typename T>
	struct atomic {
	public:
		// Default constructor initializes with default value of T.
		atomic() : m_value() {}

		// Construct with an initial value.
		atomic(const T& initial) : m_value(initial) {}

		// Implicit conversion to T.
		operator T() const { return m_value; }

		atomic<T>& operator=(const T& new_value) {
			m_value = new_value;
			return *this;
		}

	private:
		T m_value;
	};

	template<typename T>
	class concurrent_queue
	{
	public:
		void push(const T& v) { m_q.push_back(v); }

		bool try_pop(T& out)
		{
			if (m_q.empty())
				return false;
			out = m_q.front();
			m_q.pop_front();
			return true;
		}

	private:
		std::deque<T> m_q;
	};
}
#endif

struct Threads
{
	template<typename Range, typename Body, typename Partitioner>
	static void parallel_reduce(const Range& range, Body& body, const Partitioner& partitioner)
	{
#ifdef MULTICORE
		tbb::parallel_reduce(range, body, partitioner);
#else
		body(range);
#endif
}
	template<typename Range, typename Body, typename Partitioner>
	static void parallel_for(const Range& range, Body& body, const Partitioner& partitioner)
	{
#ifdef MULTICORE
		tbb::parallel_for(range, body, partitioner);
#else
		body(range);
#endif
	}
};

#endif
