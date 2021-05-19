#include "CvGameCoreDLL.h"

//
// CvTradeMessage
//
CvTradeMessage::CvTradeMessage(TradeScreenTypes eTradeScreen, TradeMessageTypes eMessageType, YieldTypes eYield, int iAmount, int iPrice, int iCommission)
	: m_ePlayer(NO_PLAYER)
	, m_eTradeScreen(eTradeScreen)
	, m_eMessageType(eMessageType)
	, m_eYield(eYield)
	, m_iAmount(iAmount)
	, m_iPrice(iPrice)
	, m_iCommission(iCommission)
{
}

TradeScreenTypes CvTradeMessage::getScreenType() const
{
	return m_eTradeScreen; 
}

TradeMessageTypes CvTradeMessage::getMessageType() const
{
	return m_eMessageType;
}

YieldTypes CvTradeMessage::getYield() const
{ 
	return m_eYield; 
}

int CvTradeMessage::getAmount() const
{ 
	return m_iAmount; 
}

int CvTradeMessage::getPrice() const
{ 
	return m_iPrice; 
}

int CvTradeMessage::getCommission() const
{ 
	return m_iCommission; 
}

CvWString CvTradeMessage::getMessage() const
{
	// no need to store strings when they are generated from the stored data anyway
	switch (m_eMessageType)
	{
	case TRADE_MESSAGE_LACK_FUNDS: return gDLL->getText("EUROPE_SCREEN_BUY_UNIT_LACK_FUNDS", GC.getYieldInfo(m_eYield).getTextKeyWide(), m_iPrice);
	default: return L"";
	}
}

//
// CvTradeMessageList
//
CvTradeMessageList::CvTradeMessageList(CvPlayer* pPlayer)
	: m_pPlayer(pPlayer)
{
}

void CvTradeMessageList::add(CvTradeMessage kNewEntry)
{
	kNewEntry.m_ePlayer = m_pPlayer->getID();
	m_aTradeMessages.push_back(kNewEntry);
}

const CvTradeMessage& CvTradeMessageList::get(int iIndex) const
{
	FAssert(iIndex >= 0 && iIndex < getLength());
	return m_aTradeMessages[iIndex];
}

int CvTradeMessageList::getLength() const
{
	return (int)m_aTradeMessages.size();
}

void CvTradeMessageList::clear()
{
	m_aTradeMessages.clear();
}
