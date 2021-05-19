#pragma once

#ifndef SMALL_CLASSES_H
#define SMALL_CLASSES_H

class CvPlayer;

class CvTradeMessage
{
	friend class CvTradeMessageList;
public:
	CvTradeMessage(TradeScreenTypes, TradeMessageTypes, YieldTypes, int, int, int);
	TradeScreenTypes getScreenType() const;
	TradeMessageTypes getMessageType() const;
	YieldTypes getYield() const;
	int getAmount() const;
	int getPrice() const;
	int getCommission() const;
	CvWString getMessage() const;

protected:
	PlayerTypes m_ePlayer;
	TradeScreenTypes m_eTradeScreen;
	TradeMessageTypes m_eMessageType;
	YieldTypes m_eYield;
	int m_iAmount;
	int m_iPrice;
	int m_iCommission;

};

class CvTradeMessageList
{
public:
	CvTradeMessageList(CvPlayer* pPlayer);

	void add(CvTradeMessage kNewEntry);
	const CvTradeMessage& get(int iIndex) const;
	int getLength() const;
	void clear();
protected:
	CvPlayer* m_pPlayer;
	std::vector<CvTradeMessage> m_aTradeMessages;
};

#endif	// SMALL_CLASSES_H
