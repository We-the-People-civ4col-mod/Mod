
#include "CvGameCoreDLL.h"

#ifdef _DEBUG

void TestEnumMap()
{
	{
		EnumMap<RouteTypes, int> test;
		RouteTypes var = static_cast<RouteTypes>(1);

		FAssert(test.get(var) == 0);
		FAssert(!test.hasContent());
		FAssert(!test.isAllocated());
		FAssert(test.getTotal() == 0);
		test.set(var, 1);
		FAssert(test.getTotal() == 1);
		FAssert(test.isAllocated());
		FAssert(test.get(var) == 1);
		FAssert(test.hasContent());
		test.set(var, 0);
		FAssert(test.getTotal() == 0);
		FAssert(test.get(var) == 0);
		FAssert(test.isAllocated());
		FAssert(!test.hasContent());
		test.set(var, 1);
		test.reset();
		FAssert(test.get(var) == 0);
		FAssert(!test.isAllocated());
		FAssert(!test.hasContent());
	}

	{
		EnumMap<RouteTypes, bool> test;
		RouteTypes var = static_cast<RouteTypes>(1);

		FAssert(!test.get(var));
		FAssert(!test.hasContent());
		FAssert(test.getTotal() == 0);
		test.set(var, true);
		FAssert(test.getTotal() == 1);
		FAssert(test.get(var));
		FAssert(test.hasContent());
		test.set(var, false);
		FAssert(test.getTotal() == 0);
		FAssert(!test.get(var));
		FAssert(!test.hasContent());
		test.set(var, true);
		test.reset();
		FAssert(!test.get(var));
		FAssert(!test.hasContent());
	}

	{
		EnumMap<RouteTypes, PlayerTypes> test;
		RouteTypes var = static_cast<RouteTypes>(1);

		FAssert(test.get(var) == NO_PLAYER);
		FAssert(!test.hasContent());
		test.set(var, FIRST_PLAYER);
		FAssert(test.get(var) == FIRST_PLAYER);
		FAssert(test.hasContent());
		test.set(var, NO_PLAYER);
		FAssert(test.get(var) == NO_PLAYER);
		FAssert(!test.hasContent());
		test.set(var, FIRST_PLAYER);
		test.reset();
		FAssert(test.get(var) == NO_PLAYER);
		FAssert(!test.hasContent());
	}

	{
		EnumMap<RouteTypes, int, 10> test;
		RouteTypes var = static_cast<RouteTypes>(1);

		FAssert(test.getTotal() == NUM_ROUTE_TYPES*10);
		test.set(var, 0);
		FAssert(test.getTotal() == ((NUM_ROUTE_TYPES * 10) - 10));
		test.addAll(1);
		FAssert(test.getTotal() == ((NUM_ROUTE_TYPES * 11) - 10));
	}

	{
		EnumMap<UnitTypes, int, 10> test;
		UnitTypes var = static_cast<UnitTypes>(1);
		
		FAssert(test.getTotal() == NUM_UNIT_TYPES * 10);
		test.set(var, 0);
		FAssert(test.getTotal() == ((NUM_UNIT_TYPES * 10) - 10));
		test.addAll(1);
		FAssert(test.getTotal() == ((NUM_UNIT_TYPES * 11) - 10));
	}
}

#endif
