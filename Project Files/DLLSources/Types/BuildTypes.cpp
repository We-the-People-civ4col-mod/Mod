#include "../CvGameCoreDLL.h"
#include "BuildTypes.h"



LoopBuildTypes::LoopBuildTypes()
	: data(NO_BUILD)
{

}LoopBuildTypes::LoopBuildTypes(BuildTypes type)
	: data(type)
{
}

bool LoopBuildTypes::isValid() const
{
	return data >= FIRST_BUILD && data < NUM_BUILD_TYPES;
}

bool LoopBuildTypes::next()
{
	data = static_cast<BuildTypes>(data + 1);
	return data < NUM_BUILD_TYPES;
}

const CvBuildInfo& LoopBuildTypes::info() const
{
	return INFO.getInfo(data);
}

LoopBuildTypes::operator BuildTypes() const
{
	return data;
}

LoopBuildTypes LoopBuildTypes::createFromInt(int iIndex)
{
	FAssert(iIndex >= -1 && iIndex < NUM_BUILD_TYPES);
	return LoopBuildTypes(static_cast<BuildTypes>(iIndex));
}

const CvBuildInfo& LoopBuildTypes::info(BuildTypes eBuild)
{
	return INFO.getInfo(eBuild);
}

//
// Route Build Type
//

RouteBuildTypes::RouteBuildTypes() : LoopBuildTypes()
{
}

RouteBuildTypes::RouteBuildTypes(BuildTypes type) : LoopBuildTypes(type)
{
}

bool RouteBuildTypes::isValid() const
{
	return data >= FIRST_BUILD && data < NUM_BUILD_TYPES;
}

bool RouteBuildTypes::next()
{
	data = static_cast<BuildTypes>(data + 1);
	return data < NUM_BUILD_TYPES;
}

LoopBuildTypes RouteBuildTypes::createFromInt(int iIndex)
{
	FAssert(iIndex >= -1 && iIndex < NUM_BUILD_TYPES);
	return LoopBuildTypes(static_cast<BuildTypes>(iIndex));
}
