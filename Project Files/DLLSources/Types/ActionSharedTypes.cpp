#include "../CvGameCoreDLL.h"
#include "../GlobalsInfoContainer.h"

//
// This file contains the definition of select functions from EnumClass.h
// The idea here is that there is direct access to the info storage, hence this file should only handle info related functions, including lengths
//
// TODO: figure out a clean implementation with const access
//

extern GlobalsInfoContainer InfoContainer;

void ActionTypes::initLength()
{
	m_num = static_cast<types>(InfoContainer.m_actions.size());
}

const CvActionInfo& ActionTypes::info() const
{
	FAssert(isValid());
	return InfoContainer.m_actions[data];
}
