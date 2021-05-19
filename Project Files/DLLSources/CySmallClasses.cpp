#include "CvGameCoreDLL.h"
#include "CySmallClasses.h"


CyTradeMessage::CyTradeMessage()
	:m_pMessage(NULL)
{
}

CyTradeMessage::CyTradeMessage(const CvTradeMessage* pMessage)
	: m_pMessage(pMessage)
{
}

CyTradeMessage::CyTradeMessage(const CvTradeMessage& kMessage)
	: m_pMessage(&kMessage)
{
}

TradeScreenTypes CyTradeMessage::getScreenType() const
{
	return m_pMessage ? m_pMessage->getScreenType() : NO_TRADESCREEN;
}

TradeMessageTypes CyTradeMessage::getMessageType() const
{
	return m_pMessage ? m_pMessage->getMessageType() : NO_TRADE_MESSAGE;
}

int CyTradeMessage::getYield() const
{
	return m_pMessage ? m_pMessage->getYield() : NO_YIELD;
}

int CyTradeMessage::getAmount() const
{
	return m_pMessage ? m_pMessage->getAmount() : 0;
}

int CyTradeMessage::getPrice() const
{
	return m_pMessage ? m_pMessage->getPrice() : 0;
}

int CyTradeMessage::getCommission() const
{
	return m_pMessage ? m_pMessage->getCommission() : 0;
}

std::wstring CyTradeMessage::getMessage() const
{
	return m_pMessage ? m_pMessage->getMessage() : L"";
}
