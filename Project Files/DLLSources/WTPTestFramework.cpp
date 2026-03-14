
#include "CvGameCoreDLL.h"
#include "WTPTestFramework.h"

#ifdef FASSERT_ENABLE

WTPTestResult g_wtpTestResult;

static const char* LOG_FILE = "Logs\\WTPTests.log";

void WTPTestReport(const char* suiteName, const WTPTestResult& result)
{
	char szBuf[512];

	if (result.tests_failed == 0)
	{
		snprintf(szBuf, sizeof(szBuf), "[PASS] %s: %d/%d tests passed",
			suiteName, result.tests_passed, result.tests_run);
		gDLL->logMsg(LOG_FILE, szBuf);
	}
	else
	{
		snprintf(szBuf, sizeof(szBuf), "[FAIL] %s: %d/%d tests failed (first failure: %s at %s:%d)",
			suiteName, result.tests_failed, result.tests_run,
			result.first_failure_msg ? result.first_failure_msg : "unknown",
			result.first_failure_file ? result.first_failure_file : "unknown",
			result.first_failure_line);
		gDLL->logMsg(LOG_FILE, szBuf);

		// Also fire FAssert so developers see the failure in Debug builds
		FAssertMsg(false, szBuf);
	}
}

// forward declarations for test suites
void RunDataStructureTests();
void RunXMLIntegrityTests();
void RunSavegameTests();

void RunAllWTPTests()
{
	gDLL->logMsg(LOG_FILE, "=== WTP Test Suite Starting ===");

	RunDataStructureTests();
	RunXMLIntegrityTests();
	RunSavegameTests();

	gDLL->logMsg(LOG_FILE, "=== WTP Test Suite Complete ===");
}

#endif
