# WTP Test Suite

## Why we built this

While implementing trade route automation for Africa and Port Royal, we ran into a chain of bugs that were invisible to normal playtesting: ships forgetting their routes during ocean crossings, automation silently clearing itself when selection groups split, AI code that only handled one of three off-map destinations. These were the kind of issues that only surface 50 turns into a game, or worse, cause a multiplayer desync that nobody can reproduce.

WTP has historically suffered from three categories of hard-to-catch defects:

1. **Savegame corruption** — a new member variable without a serialization entry, or a field order change that breaks old saves
2. **Multiplayer desync (OOS)** — non-deterministic code paths, unsynchronized state, stray `rand()` calls
3. **Crash-to-desktop** — null pointer dereferences on objects that "should always exist," XML cross-references that point to nothing

None of these announce themselves at compile time. By the time a player hits one, the root cause is hours of gameplay in the past.

So we built a test suite that runs automatically on every game launch (in Assert builds), validates the entire XML dataset, exercises core data structures, verifies savegame round-trips, and checks RNG determinism — all before you see the main menu. If something is wrong, you know immediately, with an exact file and line number.

## Architecture

### Test tiers

Tests run at four different points in the development cycle:

```
Tier 0  Build-time      Perl scripts during compilation     <1 second
Tier 1  Startup-time    C++ tests on game load              ~0.5 seconds
Tier 2  Runtime         FAssert guards during gameplay       continuous
Tier 3  On-demand       Python tests from debug console      manual
```

**Tier 0** catches problems before you even finish compiling. Perl scripts scan the source code for known anti-patterns: calls to `rand()` or `srand()` outside CvRandom, text keys referenced in XML but missing from the text files.

**Tier 1** is the core of the suite. C++ tests run inside the DLL during game initialization, after all XML data is loaded. They have full access to `GC` (the global context) and can test real game data structures with real XML-loaded values. This is where the bulk of the 48,944 tests live.

**Tier 2** is the existing FAssert system. Not a separate test file, but systematic guards at function entry points in hot code paths. When `FAssert` fires during gameplay, it points directly to the violated invariant.

**Tier 3** is Python-side validation, runnable from the in-game debug console. These tests exercise the DLL-Python bridge and validate XML data from the Python side of the interface.

### Zero overhead in release builds

The entire C++ test framework compiles under `#ifdef FASSERT_ENABLE`. In Release and FinalRelease builds, `RunAllWTPTests()` is an empty inline function. No code, no data, no runtime cost.

```cpp
#ifdef FASSERT_ENABLE
// ... full framework ...
void RunAllWTPTests();
#else
inline void RunAllWTPTests() {}
#endif
```

### Test output

All results go to `Logs/WTPTests.log`:

```
=== WTP Test Suite Starting ===
[PASS] EnumMap_Int: 22/22 tests passed
[PASS] EnumMap_Bool: 16/16 tests passed
[PASS] XMLIntegrity: 48770/48770 tests passed
[PASS] Savegame_Primitives: 14/14 tests passed
[PASS] Determinism: 576/576 tests passed
=== WTP Test Suite Complete ===
```

If any suite fails, the log shows the first failure location and an `FAssert` dialog pops up so you can't miss it:

```
[FAIL] XMLIntegrity: 3/48770 tests failed (first failure: Profession has invalid produced yield at TestXMLIntegrity.cpp:24)
```

## The C++ test framework

### Why custom, not a library

The toolchain is VC++ 2003 (C++03). No exceptions, no C++11, no package manager. Every established C++ test framework either requires exceptions (Catch, doctest), C++11 (lest), a static library link (UnitTest++), or carries license concerns for mod distribution (CxxTest is LGPL). We needed something that compiles on any C++03 compiler, has zero dependencies, and lives entirely in a single header.

The result is a MinUnit-derived macro framework: ~80 lines of header code in `WTPTestFramework.h`, a small reporting function in `WTPTestFramework.cpp`.

### The three macros

Every test in the suite is built from three assertion macros:

```cpp
WTP_ASSERT(expr)                 // fails if expr is false
WTP_ASSERT_MSG(expr, msg)        // fails with a custom message
WTP_ASSERT_EQ(expected, actual)  // fails if expected != actual
```

Each assertion increments a counter. There are no exceptions, no longjmp, no early return — a failing assertion records the failure and execution continues. This means a single test function can report multiple failures, which is essential for loop-based tests that check thousands of XML entries.

### Writing a test

A test is a static function with a standardized signature. The `WTP_TEST` macro generates it:

```cpp
WTP_TEST(SuiteName, TestName)
{
    // ... assertions ...
}
```

This expands to:

```cpp
static void SuiteName_TestName(WTPTestResult& _result)
{
    // ... assertions use _result to track pass/fail counts ...
}
```

### Running tests

Tests are grouped into suites. Each suite resets counters, runs its tests, and reports:

```cpp
void RunMySuiteTests()
{
    WTP_RUN_SUITE_BEGIN();                    // reset counters
    WTP_RUN_TEST(SuiteName, TestName);        // call each test
    WTP_RUN_TEST(SuiteName, AnotherTest);
    WTP_RUN_SUITE_END("SuiteName");           // log results, FAssert on failure
}
```

Suite runner functions are forward-declared and called from `RunAllWTPTests()` in `WTPTestFramework.cpp`.

### Adding a new test suite

1. Create `TestMyFeature.cpp` in `DLLSources/`
2. Add it to `RaR.vcxproj`
3. Write tests and a runner function
4. Forward-declare and call the runner in `WTPTestFramework.cpp`

Full example — say you want to test a new bonus system:

**TestBonusSystem.cpp:**
```cpp
#include "CvGameCoreDLL.h"
#include "WTPTestFramework.h"

#ifdef FASSERT_ENABLE

WTP_TEST(BonusSystem, AllBonusesHaveValidTerrain)
{
    for (BonusTypes eBonus = FIRST_BONUS; eBonus < NUM_BONUS_TYPES; ++eBonus)
    {
        const CvBonusInfo& kBonus = GC.getBonusInfo(eBonus);
        // Every bonus that can appear on terrain should reference valid terrain
        // ... your validation logic ...
        WTP_ASSERT_MSG(/* condition */, "Bonus has invalid terrain reference");
    }
}

WTP_TEST(BonusSystem, NoDuplicateBonusClasses)
{
    // ... your test ...
}

void RunBonusSystemTests()
{
    WTP_RUN_SUITE_BEGIN();
    WTP_RUN_TEST(BonusSystem, AllBonusesHaveValidTerrain);
    WTP_RUN_TEST(BonusSystem, NoDuplicateBonusClasses);
    WTP_RUN_SUITE_END("BonusSystem");
}

#endif
```

**WTPTestFramework.cpp** — add two lines:
```cpp
void RunBonusSystemTests();   // forward declaration

void RunAllWTPTests()
{
    // ... existing suites ...
    RunBonusSystemTests();    // new
}
```

## What we test and why

### Data structure tests (`TestDataStructures.cpp`)

**Principle:** The backbone data structures — EnumMap, JustInTimeArray, CvIdVector — are used thousands of times across the codebase. A subtle bug in any of them (wrong default value, broken reset, off-by-one in iteration) would cascade into dozens of seemingly unrelated symptoms. We test the contract of each structure exhaustively so that when something goes wrong elsewhere, we can rule these out.

**EnumMap** is the most heavily used container in the DLL. It's a compile-time-optimized map from an enum key to a value, with specializations for `bool` (bitfield), `int`, and enum value types. We test each specialization independently.

```cpp
// Int specialization: verify default, set/get, reset, arithmetic
WTP_TEST(EnumMap, IntDefaultZero)
{
    EnumMap<RouteTypes, int> em;
    RouteTypes eKey = static_cast<RouteTypes>(0);

    WTP_ASSERT(!em.isAllocated());   // lazy allocation — not allocated until needed
    WTP_ASSERT(!em.hasContent());    // no non-default values
    WTP_ASSERT_EQ(0, em.get(eKey));  // default is zero
    WTP_ASSERT_EQ(0, em.getTotal()); // sum of all values is zero
}

// Bool specialization: backed by bitfield, different code path
WTP_TEST(EnumMap, BoolSetGetReset)
{
    EnumMap<RouteTypes, bool> em;
    RouteTypes eKey = static_cast<RouteTypes>(0);

    em.set(eKey, true);
    WTP_ASSERT(em.get(eKey));
    WTP_ASSERT(em.hasContent());
    WTP_ASSERT_EQ(1, em.getTotal());

    em.set(eKey, false);
    WTP_ASSERT(!em.get(eKey));
    WTP_ASSERT(!em.hasContent());
}

// Enum value type: default is the NO_* sentinel, not zero
WTP_TEST(EnumMap, EnumValueType)
{
    EnumMap<RouteTypes, PlayerTypes> em;
    RouteTypes eKey = static_cast<RouteTypes>(0);

    WTP_ASSERT_EQ(NO_PLAYER, em.get(eKey));  // default is NO_PLAYER, not 0
    WTP_ASSERT(!em.hasContent());

    em.set(eKey, FIRST_PLAYER);
    WTP_ASSERT_EQ(FIRST_PLAYER, em.get(eKey));
    WTP_ASSERT(em.hasContent());
}
```

**JustInTimeArray** uses lazy allocation — it doesn't allocate memory until a non-default value is written. This saves significant memory in a 32-bit address space where every byte counts. We verify the lazy allocation contract:

```cpp
WTP_TEST(JustInTimeArray, DefaultUnallocated)
{
    JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);
    WTP_ASSERT(!arr.isAllocated());   // no memory allocated yet
    WTP_ASSERT_EQ(0, arr.get(0));     // but reads still return default
}

WTP_TEST(JustInTimeArray, SetAllocates)
{
    JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);
    arr.set(5, 0);
    WTP_ASSERT(arr.isAllocated());    // now allocated
    WTP_ASSERT_EQ(5, arr.get(0));
}

WTP_TEST(JustInTimeArray, ResetDeallocates)
{
    JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);
    arr.set(42, 0);
    WTP_ASSERT(arr.isAllocated());
    arr.reset();
    WTP_ASSERT(!arr.isAllocated());   // memory freed
    WTP_ASSERT_EQ(0, arr.get(0));     // back to default
}
```

**CvIdVector** manages collections of game objects with stable IDs (trade routes, units in a group). The critical invariant: IDs are never reused after removal, because stale ID references elsewhere in the code would silently point to the wrong object.

```cpp
WTP_TEST(CvIdVector, IdsNotReusedAfterRemove)
{
    CvIdVector<TestIdItem> vec;
    vec.addNew(); // ID 0
    vec.addNew(); // ID 1
    vec.removeById(0);

    TestIdItem* p2 = vec.addNew(); // must be ID 2, not recycled 0
    WTP_ASSERT_EQ(2, p2->getID());
}
```

**Current coverage:** 122 data structure tests across 7 suites (EnumMap_Int, EnumMap_Bool, EnumMap_Enum, JustInTimeArray, Coordinates, TradeRoute, CvIdVector).

### XML integrity tests (`TestXMLIntegrity.cpp`)

**Principle:** The game loads hundreds of XML files at startup. These files cross-reference each other — a profession references yield types, a building references a building class, a unit references a default profession. If any reference is invalid (typo, removed entry, reordered enum), the game will crash or silently produce wrong results at some unpredictable later point. We validate every cross-reference at startup, turning a runtime crash into an immediate, specific error message.

The Perl autogeneration system catches enum mismatches, but it cannot catch invalid references *within* the XML data itself. That's what these tests do.

```cpp
// Every profession's produced/consumed yields must be valid YieldTypes (or NO_YIELD)
WTP_TEST(XMLIntegrity, ProfessionYieldsValid)
{
    for (ProfessionTypes eProfession = FIRST_PROFESSION; eProfession < NUM_PROFESSION_TYPES; ++eProfession)
    {
        const CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

        for (int i = 0; i < kProfession.getNumYieldsProduced(); ++i)
        {
            int eYield = kProfession.getYieldsProduced(i);
            // NO_YIELD (-1) is valid — game code checks and skips it
            WTP_ASSERT_MSG(eYield == NO_YIELD || (eYield >= 0 && eYield < NUM_YIELD_TYPES),
                "Profession has invalid produced yield");
        }
    }
}

// Every building must belong to a valid building class
WTP_TEST(XMLIntegrity, BuildingClassValid)
{
    for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
    {
        const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
        int iBuildingClass = kBuilding.getBuildingClassType();
        WTP_ASSERT_MSG(iBuildingClass >= 0 && iBuildingClass < GC.getNumBuildingClassInfos(),
            "Building has invalid BuildingClassType");
    }
}
```

The pattern is always the same: iterate every instance of a type, check that its references point to valid entries. This is where the assertion-counting approach pays off — `ProfessionYieldsValid` alone runs thousands of individual checks (every yield slot of every profession), and a single `WTP_ASSERT_MSG` in a loop handles them all.

**Current coverage:** 48,770 checks across 10 test functions covering professions, buildings, units, founding fathers, equipment yields, terrain, and trade route sentinels.

### Savegame round-trip tests (`TestSavegame.cpp`)

**Principle:** The savegame system writes variables by name/tag, not by position. Missing variables get defaults on load; conversion tables handle XML reordering. This flexibility is powerful but fragile — it's easy to introduce a serialization bug that only manifests when loading a save from a previous version, or when a field's type doesn't match what was written.

We test the foundation: does every primitive type survive a write-then-read cycle through the same stream interface the real savegame system uses?

**MemoryStream** is a minimal in-memory implementation of `FDataStreamBase` (the engine's stream interface). It lets us test serialization without touching the filesystem or needing a running game:

```cpp
class MemoryStream : public FDataStreamBase
{
    std::vector<byte> m_data;
    unsigned int m_iPos;
public:
    // Implements Read, Write, GetPosition, SetPosition, GetEOF, etc.
    // We never pass this to the EXE — it only flows through our own
    // CvSavegameReader/Writer code, so ABI compatibility is not a concern.
};
```

With MemoryStream, round-trip tests are straightforward:

```cpp
// Integers survive serialization
WTP_TEST(Savegame, IntRoundTrip)
{
    MemoryStream stream;
    stream.Write(42);
    stream.Rewind();
    int iRead = 0;
    stream.Read(&iRead);
    WTP_ASSERT_EQ(42, iRead);
}

// Trade route sentinel IDs survive serialization
WTP_TEST(Savegame, IDInfoTradeRouteSentinelRoundTrip)
{
    MemoryStream stream;

    IDInfo europeInfo(FIRST_PLAYER, CvTradeRoute::EUROPE_CITY_ID);
    IDInfo anywhereInfo(FIRST_PLAYER, CvTradeRoute::ANYWHERE_CITY_ID);

    europeInfo.write(&stream);
    anywhereInfo.write(&stream);

    stream.Rewind();

    IDInfo loadedEurope, loadedAnywhere;
    loadedEurope.read(&stream);
    loadedAnywhere.read(&stream);

    WTP_ASSERT(loadedEurope == europeInfo);
    WTP_ASSERT(loadedAnywhere == anywhereInfo);

    // Verify sentinel values survived round-trip
    WTP_ASSERT_EQ(CvTradeRoute::EUROPE_CITY_ID, loadedEurope.iID);
    WTP_ASSERT_EQ(CvTradeRoute::ANYWHERE_CITY_ID, loadedAnywhere.iID);
}
```

We also test stream mechanics (position tracking, seeking, truncation) and edge cases (MAX_INT, empty strings, zero-length arrays, JustInTimeArray's behavior of allocating even on empty reads).

**Current coverage:** 56 tests across 7 suites (Primitives, StreamState, Strings, IDInfo, JustInTimeArray, Enums, EdgeCases).

### Determinism tests (`TestDeterminism.cpp`)

**Principle:** Multiplayer synchronization requires that every player's game state evolves identically given the same inputs. The most common source of desync is non-deterministic random number generation. CvRandom is the only RNG that should be used in game state code — we verify that it behaves deterministically and that its output stays within documented bounds.

```cpp
// The core invariant: same seed, same sequence, always
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

// peek() must not advance state (used for logging without side effects)
WTP_TEST(Determinism, PeekDoesNotAdvance)
{
    CvRandom rng;
    rng.init(42);

    unsigned long seedBefore = rng.getSeed();
    unsigned long peeked = rng.peek();
    unsigned long seedAfter = rng.getSeed();

    WTP_ASSERT_EQ(seedBefore, seedAfter);  // state unchanged
    WTP_ASSERT(peeked != seedBefore);       // but peek returned the next value
}

// get(N) must always return [0, N)
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

// Saving and restoring a seed must produce identical sequences
WTP_TEST(Determinism, SeedSurvivesRoundTrip)
{
    CvRandom rng;
    rng.init(54321);
    rng.get(100, NULL);
    rng.get(100, NULL);
    unsigned long savedSeed = rng.getSeed();

    CvRandom rng2;
    rng2.reseed(savedSeed);
    WTP_ASSERT_EQ(savedSeed, rng2.getSeed());

    for (int i = 0; i < 20; ++i)
    {
        WTP_ASSERT_EQ(rng.get(500, NULL), rng2.get(500, NULL));
    }
}
```

**Current coverage:** 576 assertions across 8 tests covering seed reproducibility, range bounds, float output, peek side-effects, edge cases (range=1), and state round-trips.

### Build-time validation (Perl scripts in `bin/`)

**Principle:** Some bugs can be caught by static analysis of the source code, without compiling or running anything. Perl scripts run during the `source_list` build phase and fail the build immediately if they find problems.

**`test_determinism.pl`** — Scans C++ sources for calls to `rand()`, `srand()`, `time()`, and `GetTickCount()` outside of explicitly whitelisted files. Any of these in game state code would cause multiplayer desync.

**`test_text_keys.pl`** — Verifies that every `TXT_KEY_*` referenced in XML data files exists in the text XML files under `Assets/XML/Text/`. A missing text key shows as a raw key string in the UI instead of the translated text.

### Python tests (`Assets/Python/_DebugTools/WTPTests.py`)

**Principle:** The Python-DLL bridge exposes game data to the UI layer. If the bridge is broken — a method returns wrong data, a new enum isn't exposed, an index is off — the UI silently shows wrong information. Python tests validate the bridge from the consumer side.

These run on demand from the debug console (requires `CheatCode = chipotle` in `CivilizationIV.ini`):

```
~ (tilde key to open console)
>>> import WTPTests
>>> WTPTests.runAllTests()
```

The Python test runner is simple — each test function returns a list of error strings (empty = pass):

```python
def testBuildingClassesValid():
    errors = []
    numClasses = gc.getNumBuildingClassInfos()
    for i in range(gc.getNumBuildingInfos()):
        info = gc.getBuildingInfo(i)
        iClass = info.getBuildingClassType()
        if iClass < 0 or iClass >= numClasses:
            errors.append("Building %d has invalid class %d" % (i, iClass))
    return errors
```

Results appear as in-game messages and in `PythonDbg.log`.

**Current coverage:** 7 tests covering profession yields, building classes, unit professions, father categories, yield info integrity, terrain yield access, and civilization info consistency.

## File layout

```
Project Files/DLLSources/
    WTPTestFramework.h          Test macros and WTPTestResult struct
    WTPTestFramework.cpp        Report function, RunAllWTPTests() dispatcher
    TestDataStructures.cpp      EnumMap, JustInTimeArray, Coordinates, TradeRoute, CvIdVector
    TestXMLIntegrity.cpp        XML cross-reference validation
    TestSavegame.cpp            MemoryStream + round-trip serialization tests
    TestDeterminism.cpp         CvRandom determinism and bounds tests

Project Files/bin/
    test_determinism.pl         Scan for non-deterministic stdlib calls
    test_text_keys.pl           Verify TXT_KEY coverage

Assets/Python/_DebugTools/
    WTPTests.py                 Python test suite (debug console)
```

## Lessons learned during implementation

**NO_YIELD is a valid value.** Our first XML integrity run flagged 109 "invalid yields" because profession yield lists use NO_YIELD (-1) as a placeholder. The game code explicitly checks for and skips NO_YIELD. The fix: `eYield == NO_YIELD || (eYield >= 0 && eYield < NUM_YIELD_TYPES)`. Lesson: understand the domain's sentinel values before writing assertions.

**JustInTimeArray::Read() always allocates.** We initially asserted that reading an empty array should leave it unallocated. But `Read()` unconditionally calls `allocate()` — even for an array of all zeros. The implementation is correct (it simplifies the read path); our assumption was wrong. Lesson: test the actual behavior, not what you think the behavior should be.

**Enum casts are mandatory in C++03.** VC++ 2003 will not implicitly convert between `int` and enum types in many contexts. Savegame round-trip tests for enums must read into an `int` and then `static_cast`:

```cpp
int iRead = 0;
stream.Read(&iRead);
YieldTypes eRead = static_cast<YieldTypes>(iRead);
```

**gDLL->logMsg() prepends "Logs/".** We initially set `LOG_FILE = "Logs\\WTPTests.log"`, which created the file at `Logs/Logs/WTPTests.log`. The engine's logging function already writes to the Logs directory.
