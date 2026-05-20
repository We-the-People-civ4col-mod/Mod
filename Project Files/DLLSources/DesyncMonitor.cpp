#include "DesyncMonitor.h"
#include "FAssert.h"

static int COUNT_ASYNC = 0;
static int COUNT_SYNC  = 0;
static int COUNT_NETWORK_RECEIVE = 0;

CxDesyncMonitor::CxDesyncMonitor(TYPE eType)
	: m_iAsync(1)
	, m_iSync(0)
	, m_iNetworkReceive(0)
{
	switch (eType)
	{
	case TYPE_ASYNC:
		break;
	case TYPE_BOTH:
		m_iAsync = 0;
		m_iSync = 1;
		break;
	case TYPE_RESET:
		m_iAsync = -COUNT_ASYNC;
		m_iSync = -COUNT_SYNC;
		break;
	case TYPE_NETWORK_RECEIVE:
		FAssert(COUNT_NETWORK_RECEIVE == 0);
		m_iAsync = 0;
		m_iNetworkReceive = 1;
		break;
	}

	COUNT_ASYNC += m_iAsync;
	COUNT_SYNC += m_iSync;
	COUNT_NETWORK_RECEIVE += m_iNetworkReceive;
	FAssert(COUNT_ASYNC >= 0);
	FAssert(COUNT_SYNC >= 0);
}

CxDesyncMonitor::~CxDesyncMonitor()
{
	COUNT_ASYNC -= m_iAsync;
	COUNT_SYNC -= m_iSync;
	COUNT_NETWORK_RECEIVE -= m_iNetworkReceive;
	FAssert(COUNT_ASYNC >= 0);
	FAssert(COUNT_SYNC >= 0);
	FAssert(COUNT_NETWORK_RECEIVE >= 0);
}

bool CxDesyncMonitor::isCurrentlySync()
{
	return COUNT_ASYNC == 0;
}

bool CxDesyncMonitor::isAlwaysSync()
{
	return COUNT_ASYNC == 0 && COUNT_SYNC == 0;
}

bool CxDesyncMonitor::isNeverSync()
{
	return COUNT_ASYNC > 0 && COUNT_SYNC == 0;
}

bool CxDesyncMonitor::isNetworkReceive()
{
	return COUNT_NETWORK_RECEIVE > 0;
}
