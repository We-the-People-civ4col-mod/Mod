#include "../CvGameCoreDLL.h"
#include "YieldTypes.h"



LoopYieldTypes::LoopYieldTypes()
	: data(NO_YIELD)
{
}

LoopYieldTypes::LoopYieldTypes(YieldTypes type)
	: data(type)
{
}

bool LoopYieldTypes::isValid() const
{
	return data >= FIRST_YIELD && data < NUM_YIELD_TYPES;
}

bool LoopYieldTypes::next()
{
	data = static_cast<YieldTypes>(data + 1);
	return data < NUM_YIELD_TYPES;
}

const CvYieldInfo& LoopYieldTypes::info() const
{
	return GC.getYieldInfo(data);
}

LoopYieldTypes::operator YieldTypes() const
{
	return data;
}

LoopYieldTypes LoopYieldTypes::createFromInt(int iIndex)
{
	FAssert(iIndex >= -1 && iIndex < NUM_YIELD_TYPES);
	return LoopYieldTypes(static_cast<YieldTypes>(iIndex));
}

const CvYieldInfo& LoopYieldTypes::info(YieldTypes eYield)
{
	return GC.getYieldInfo(eYield);
}

//
// Cargo Yield Types
//
CargoYieldTypes::CargoYieldTypes() : LoopYieldTypes()
{
}

CargoYieldTypes::CargoYieldTypes(YieldTypes type) : LoopYieldTypes(type)
{
}

bool CargoYieldTypes::isValid() const
{
	return data >= FIRST_YIELD && data < NUM_CARGO_YIELD_TYPES;
}

bool CargoYieldTypes::next()
{
	data = static_cast<YieldTypes>(data + 1);
	return data < NUM_CARGO_YIELD_TYPES;
}

CargoYieldTypes CargoYieldTypes::createFromInt(int iIndex)
{
	FAssert(iIndex >= -1 && iIndex < NUM_YIELD_TYPES);
	return CargoYieldTypes(static_cast<YieldTypes>(iIndex));
}

//
// Luxury Yield Types
//
LuxuryYieldTypes::LuxuryYieldTypes() : LoopYieldTypes()
{
}

LuxuryYieldTypes::LuxuryYieldTypes(YieldTypes type) : LoopYieldTypes(type)
{
}

bool LuxuryYieldTypes::isValid() const
{
	return data >= FIRST_YIELD && data < NUM_CARGO_YIELD_TYPES;
}

bool LuxuryYieldTypes::next()
{
	data = static_cast<YieldTypes>(data + 1);
	return data < NUM_CARGO_YIELD_TYPES;
}

LuxuryYieldTypes LuxuryYieldTypes::createFromInt(int iIndex)
{
	FAssert(iIndex >= -1 && iIndex < NUM_CARGO_YIELD_TYPES);
	return LuxuryYieldTypes(static_cast<YieldTypes>(iIndex));
}

//
// Plot Yield Types
//
PlotYieldTypes::PlotYieldTypes() : LoopYieldTypes()
{
}

PlotYieldTypes::PlotYieldTypes(YieldTypes type) : LoopYieldTypes(type)
{
}

bool PlotYieldTypes::isValid() const
{
	return data >= FIRST_YIELD && data < NUM_CARGO_YIELD_TYPES;
}

bool PlotYieldTypes::next()
{
	data = static_cast<YieldTypes>(data + 1);
	return data < NUM_CARGO_YIELD_TYPES;
}

PlotYieldTypes PlotYieldTypes::createFromInt(int iIndex)
{
	FAssert(iIndex >= -1 && iIndex < NUM_CARGO_YIELD_TYPES);
	return PlotYieldTypes(static_cast<YieldTypes>(iIndex));
}
