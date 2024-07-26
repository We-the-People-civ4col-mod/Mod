#include "../CvGameCoreDLL.h"


ActionTypes::types ActionTypes::m_num = ActionTypes::NONE;
const ActionTypes::types& ActionTypes::NUM = ActionTypes::m_num;

// implementations in EnumClassInfo.cpp
#if 0
void ActionTypes::initLength()
const CvActionInfo& ActionTypes::info() const
#endif

ActionTypes::ActionTypes()
	: data(NONE)
{

}ActionTypes::ActionTypes(types type)
	: data(type)
{
}

ActionTypes::types ActionTypes::value() const
{
	return data;
}

bool ActionTypes::isValid() const
{
	return data >= 0 && data < NUM;
}

bool ActionTypes::next()
{
	data = static_cast<types>(data + 1);
	return data < NUM;
}

ActionTypes ActionTypes::createFromInt(int iIndex)
{
	ActionTypes eTemp;
	eTemp.data = static_cast<types>(iIndex);
	return eTemp;
}
