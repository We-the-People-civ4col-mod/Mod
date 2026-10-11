## WTP Test Suite - Python Tests (Tier 3)
## Callable from the in-game debug console or via CvEventManager hook.
## Tests exercise the Python-DLL bridge and validate XML data integrity.
##
## Usage: from the debug console or Python event hook, call:
##   import WTPTests
##   WTPTests.runAllTests()

from CvPythonExtensions import *
import CvUtil

gc = CyGlobalContext()

# ============================================================================
# Test Infrastructure
# ============================================================================

class WTPTestRunner:
	def __init__(self):
		self.tests_run = 0
		self.tests_passed = 0
		self.tests_failed = 0
		self.failures = []

	def reset(self):
		self.tests_run = 0
		self.tests_passed = 0
		self.tests_failed = 0
		self.failures = []

	def runTest(self, name, testFunc):
		self.tests_run = self.tests_run + 1
		errors = testFunc()
		if len(errors) == 0:
			self.tests_passed = self.tests_passed + 1
		else:
			self.tests_failed = self.tests_failed + 1
			for e in errors:
				self.failures.append("%s: %s" % (name, e))

	def report(self):
		if self.tests_failed == 0:
			msg = "[PASS] WTPTests.py: %d/%d tests passed" % (self.tests_passed, self.tests_run)
			CvUtil.pyPrint(msg)
			CyInterface().addImmediateMessage(msg, "")
		else:
			msg = "[FAIL] WTPTests.py: %d/%d tests failed" % (self.tests_failed, self.tests_run)
			CvUtil.pyPrint(msg)
			CyInterface().addImmediateMessage(msg, "")
			for f in self.failures:
				CvUtil.pyPrint("  " + f)
				CyInterface().addImmediateMessage(f, "")

g_runner = WTPTestRunner()

# ============================================================================
# Test: Profession Yields Valid
# ============================================================================

def testProfessionYieldsValid():
	errors = []
	numYields = gc.getNumYieldInfos()
	for i in range(gc.getNumProfessionInfos()):
		info = gc.getProfessionInfo(i)
		for j in range(info.getNumYieldsProduced()):
			eYield = info.getYieldsProduced(j)
			if eYield != -1 and (eYield < 0 or eYield >= numYields):
				errors.append("Profession %d has invalid produced yield %d" % (i, eYield))
		for j in range(info.getNumYieldsConsumed()):
			eYield = info.getYieldsConsumed(j)
			if eYield != -1 and (eYield < 0 or eYield >= numYields):
				errors.append("Profession %d has invalid consumed yield %d" % (i, eYield))
	return errors

# ============================================================================
# Test: Building Classes Valid
# ============================================================================

def testBuildingClassesValid():
	errors = []
	numClasses = gc.getNumBuildingClassInfos()
	for i in range(gc.getNumBuildingInfos()):
		info = gc.getBuildingInfo(i)
		iClass = info.getBuildingClassType()
		if iClass < 0 or iClass >= numClasses:
			errors.append("Building %d has invalid class %d" % (i, iClass))
	return errors

# ============================================================================
# Test: Unit Default Professions Valid
# ============================================================================

def testUnitDefaultProfessions():
	errors = []
	numProfessions = gc.getNumProfessionInfos()
	for i in range(gc.getNumUnitInfos()):
		info = gc.getUnitInfo(i)
		eProfession = info.getDefaultProfession()
		if eProfession != -1 and (eProfession < 0 or eProfession >= numProfessions):
			errors.append("Unit %d has invalid default profession %d" % (i, eProfession))
	return errors

# ============================================================================
# Test: Father Categories Valid
# ============================================================================

def testFatherCategoriesValid():
	errors = []
	numCategories = gc.getNumFatherCategoryInfos()
	for i in range(gc.getNumFatherInfos()):
		info = gc.getFatherInfo(i)
		eCategory = info.getFatherCategory()
		if eCategory < 0 or eCategory >= numCategories:
			errors.append("Father %d has invalid category %d" % (i, eCategory))
	return errors

# ============================================================================
# Test: Yield Infos Cargo Flag Consistency
# ============================================================================

def testYieldCargoConsistency():
	errors = []
	for i in range(gc.getNumYieldInfos()):
		info = gc.getYieldInfo(i)
		# Every yield should have a valid type string
		typeName = info.getType()
		if typeName is None or len(typeName) == 0:
			errors.append("Yield %d has empty type string" % i)
	return errors

# ============================================================================
# Test: Terrain Yield Access
# ============================================================================

def testTerrainYieldAccess():
	errors = []
	numYields = gc.getNumYieldInfos()
	for i in range(gc.getNumTerrainInfos()):
		info = gc.getTerrainInfo(i)
		for j in range(numYields):
			# Should not crash
			val = info.getYield(j)
	return errors

# ============================================================================
# Test: Civilization Info Consistency
# ============================================================================

def testCivilizationInfoConsistency():
	errors = []
	for i in range(gc.getNumCivilizationInfos()):
		info = gc.getCivilizationInfo(i)
		typeName = info.getType()
		if typeName is None or len(typeName) == 0:
			errors.append("Civilization %d has empty type string" % i)
	return errors

# ============================================================================
# Test Runner
# ============================================================================

def runAllTests():
	g_runner.reset()
	g_runner.runTest("ProfessionYieldsValid", testProfessionYieldsValid)
	g_runner.runTest("BuildingClassesValid", testBuildingClassesValid)
	g_runner.runTest("UnitDefaultProfessions", testUnitDefaultProfessions)
	g_runner.runTest("FatherCategoriesValid", testFatherCategoriesValid)
	g_runner.runTest("YieldCargoConsistency", testYieldCargoConsistency)
	g_runner.runTest("TerrainYieldAccess", testTerrainYieldAccess)
	g_runner.runTest("CivilizationInfoConsistency", testCivilizationInfoConsistency)
	g_runner.report()
	return g_runner.failures
