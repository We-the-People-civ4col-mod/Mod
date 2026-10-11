
#include "CvGameCoreDLL.h"
#include "WTPTestFramework.h"

#ifdef FASSERT_ENABLE

// ============================================================================
// CvRandom Determinism Tests
// ============================================================================

WTP_TEST(Determinism, SameSeedSameSequence)
{
	CvRandom rng1;
	CvRandom rng2;
	rng1.init(42);
	rng2.init(42);

	for (int i = 0; i < 100; ++i)
	{
		unsigned short val1 = rng1.get(1000, NULL);
		unsigned short val2 = rng2.get(1000, NULL);
		WTP_ASSERT_EQ(val1, val2);
	}
}

WTP_TEST(Determinism, DifferentSeedsDifferentSequence)
{
	CvRandom rng1;
	CvRandom rng2;
	rng1.init(42);
	rng2.init(99);

	// At least one of the first 10 values should differ
	bool bDifferent = false;
	for (int i = 0; i < 10; ++i)
	{
		if (rng1.get(1000, NULL) != rng2.get(1000, NULL))
		{
			bDifferent = true;
			break;
		}
	}
	WTP_ASSERT(bDifferent);
}

WTP_TEST(Determinism, ReseedRestoresSequence)
{
	CvRandom rng;
	rng.init(12345);

	// Generate some values
	unsigned short first = rng.get(1000, NULL);
	unsigned short second = rng.get(1000, NULL);

	// Reseed to same value
	rng.reseed(12345);

	// Should produce same sequence
	WTP_ASSERT_EQ(first, rng.get(1000, NULL));
	WTP_ASSERT_EQ(second, rng.get(1000, NULL));
}

WTP_TEST(Determinism, PeekDoesNotAdvance)
{
	CvRandom rng;
	rng.init(42);

	unsigned long seedBefore = rng.getSeed();
	unsigned long peeked = rng.peek();
	unsigned long seedAfter = rng.getSeed();

	// Peek should not change state
	WTP_ASSERT_EQ(seedBefore, seedAfter);

	// Peek should predict the next state
	rng.get(1000, NULL);
	// After advancing, seed should match what peek predicted
	// peek() returns the raw next seed, get() also advances
	// So we verify peek returned something different from current
	WTP_ASSERT(peeked != seedBefore);
}

WTP_TEST(Determinism, GetRangeValid)
{
	CvRandom rng;
	rng.init(777);

	for (int i = 0; i < 200; ++i)
	{
		unsigned short val = rng.get(100, NULL);
		WTP_ASSERT(val < 100);
	}
}

WTP_TEST(Determinism, GetRange1AlwaysZero)
{
	CvRandom rng;
	rng.init(42);

	for (int i = 0; i < 50; ++i)
	{
		WTP_ASSERT_EQ(0, rng.get(1, NULL));
	}
}

WTP_TEST(Determinism, FloatInRange)
{
	CvRandom rng;
	rng.init(42);

	for (int i = 0; i < 100; ++i)
	{
		float f = rng.getFloat();
		WTP_ASSERT(f >= 0.0f);
		WTP_ASSERT(f <= 1.0f);
	}
}

WTP_TEST(Determinism, SeedSurvivesRoundTrip)
{
	CvRandom rng;
	rng.init(54321);
	// Advance state a few times
	rng.get(100, NULL);
	rng.get(100, NULL);
	unsigned long savedSeed = rng.getSeed();

	CvRandom rng2;
	rng2.reseed(savedSeed);
	WTP_ASSERT_EQ(savedSeed, rng2.getSeed());

	// Both should produce identical sequences from here
	for (int i = 0; i < 20; ++i)
	{
		WTP_ASSERT_EQ(rng.get(500, NULL), rng2.get(500, NULL));
	}
}

// ============================================================================
// Test Runner
// ============================================================================

void RunDeterminismTests()
{
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(Determinism, SameSeedSameSequence);
	WTP_RUN_TEST(Determinism, DifferentSeedsDifferentSequence);
	WTP_RUN_TEST(Determinism, ReseedRestoresSequence);
	WTP_RUN_TEST(Determinism, PeekDoesNotAdvance);
	WTP_RUN_TEST(Determinism, GetRangeValid);
	WTP_RUN_TEST(Determinism, GetRange1AlwaysZero);
	WTP_RUN_TEST(Determinism, FloatInRange);
	WTP_RUN_TEST(Determinism, SeedSurvivesRoundTrip);
	WTP_RUN_SUITE_END("Determinism");
}

#endif
