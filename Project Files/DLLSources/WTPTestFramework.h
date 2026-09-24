#pragma once

// WTP Test Framework -- MinUnit-derived, C++03 compatible
//
// Usage:
//   WTP_TEST(TestSuiteName, TestName)
//   {
//       WTP_ASSERT(1 + 1 == 2);
//       WTP_ASSERT_MSG(value > 0, "value must be positive");
//       WTP_ASSERT_EQ(expected, actual);
//   }
//
//   In RunAllWTPTests():
//     WTP_RUN_SUITE_BEGIN();
//     WTP_RUN_TEST(SuiteName, TestName);
//     WTP_RUN_SUITE_END("SuiteName");

#ifdef FASSERT_ENABLE

struct WTPTestResult
{
	int tests_run;
	int tests_passed;
	int tests_failed;
	const char* first_failure_file;
	int first_failure_line;
	const char* first_failure_msg;

	WTPTestResult() : tests_run(0), tests_passed(0), tests_failed(0),
		first_failure_file(NULL), first_failure_line(0),
		first_failure_msg(NULL) {}
};

// Global result accumulator (reset per suite)
extern WTPTestResult g_wtpTestResult;

#define WTP_TEST(suite, name) \
	static void suite##_##name(WTPTestResult& _result)

#define WTP_ASSERT(expr) \
	do { \
		_result.tests_run++; \
		if (!(expr)) { \
			_result.tests_failed++; \
			if (_result.first_failure_file == NULL) { \
				_result.first_failure_file = __FILE__; \
				_result.first_failure_line = __LINE__; \
				_result.first_failure_msg = #expr; \
			} \
		} else { \
			_result.tests_passed++; \
		} \
	} while(0)

#define WTP_ASSERT_MSG(expr, msg) \
	do { \
		_result.tests_run++; \
		if (!(expr)) { \
			_result.tests_failed++; \
			if (_result.first_failure_file == NULL) { \
				_result.first_failure_file = __FILE__; \
				_result.first_failure_line = __LINE__; \
				_result.first_failure_msg = msg; \
			} \
		} else { \
			_result.tests_passed++; \
		} \
	} while(0)

#define WTP_ASSERT_EQ(expected, actual) \
	WTP_ASSERT_MSG((expected) == (actual), #expected " != " #actual)

#define WTP_RUN_TEST(suite, name) \
	suite##_##name(g_wtpTestResult)

#define WTP_RUN_SUITE_BEGIN() \
	g_wtpTestResult = WTPTestResult()

#define WTP_RUN_SUITE_END(suiteName) \
	WTPTestReport(suiteName, g_wtpTestResult)

// Report function (implemented in WTPTestFramework.cpp)
void WTPTestReport(const char* suiteName, const WTPTestResult& result);

// Master test runner (called from CvGlobalsEnumSetup.cpp)
void RunAllWTPTests();

#else

// Compiled out in Release/FinalRelease
inline void RunAllWTPTests() {}

#endif
