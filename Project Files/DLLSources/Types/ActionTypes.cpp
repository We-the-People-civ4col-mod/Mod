#include "../CvGameCoreDLL.h"


ActionTypes::types ActionTypes::m_num = ActionTypes::NONE;
const ActionTypes::types& ActionTypes::NUM = ActionTypes::m_num;

// implementations in EnumClassInfo.cpp
#if 0
void ActionTypes::initLength()
const CvActionInfo& ActionTypes::info() const
#endif

ActionTypes::ActionTypes()
	: m_var(NONE)
{

}ActionTypes::ActionTypes(types type)
	: m_var(type)
{
}

bool ActionTypes::isValid() const
{
	return m_var >= 0 && m_var < NUM;
}

int ActionTypes::getInt() const
{
	return m_var;
}

bool ActionTypes::next()
{
	m_var = static_cast<types>(m_var + 1);
	return isValid();
}

ActionTypes ActionTypes::createFromInt(int iIndex)
{
	ActionTypes eTemp;
	eTemp.m_var = static_cast<types>(iIndex);
	return eTemp;
}
