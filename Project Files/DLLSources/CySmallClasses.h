#ifndef CY_SMALL_CLASSES_H
#define CY_SMALL_CLASSES_H



class CyTradeMessage
{
public:
	CyTradeMessage();
	CyTradeMessage(const CvTradeMessage*);
	CyTradeMessage(const CvTradeMessage&);

	TradeScreenTypes getScreenType() const;
	TradeMessageTypes getMessageType() const;
	int getYield() const;
	int getAmount() const;
	int getPrice() const;
	int getCommission() const;
	std::wstring getMessage() const;

protected:
	const CvTradeMessage* m_pMessage;
};

#endif