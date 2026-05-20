#pragma once
#ifndef CVDIPLOPARAMETERS_H
#define CVDIPLOPARAMETERS_H

#include "LinkedList.h"
//#include "CvStructs.h"
#include "FVariableSystem.h"

class CvDiploParameters
{
public:
	DllExport CvDiploParameters(PlayerTypes ePlayer = NO_PLAYER);
	DllExport virtual ~CvDiploParameters();

	DllExport void setWhoTalkingTo(PlayerTypes eWhoTalkingTo);
	DllExport PlayerTypes getWhoTalkingTo() const;
	DllExport void setDiploComment(DiploCommentTypes eCommentType);
	DllExport void addDiploCommentVariable(const wchar *szArg);
	DllExport void addDiploCommentVariable(int iArg);
	DllExport void addDiploCommentVariable(const FVariable& var);

	DllExport DiploCommentTypes getDiploComment() const;
	DllExport void setOurOfferList(const CLinkList<TradeData>& ourOffer);
	DllExport const CLinkList<TradeData>& getOurOfferList() const;
	DllExport void setTheirOfferList(const CLinkList<TradeData>& theirOffer);
	DllExport const CLinkList<TradeData>& getTheirOfferList() const;
	DllExport void setRenegotiate(bool bValue);
	DllExport bool getRenegotiate() const;
	DllExport void setAIContact(bool bValue);
	DllExport bool getAIContact() const;
	DllExport void setPendingDelete(bool bPending);
	DllExport bool getPendingDelete() const;
	DllExport void setData(int iData);
	DllExport int getData() const;
	DllExport void setHumanDiplo(bool bValue);
	DllExport bool getHumanDiplo() const;
	DllExport void setOurOffering(bool bValue);
	DllExport bool getOurOffering() const;
	DllExport void setTheirOffering(bool bValue);
	DllExport bool getTheirOffering() const;
	DllExport void setChatText(const wchar* szText);
	DllExport const wchar* getChatText() const;
	DllExport const std::vector<FVariable>& getDiploCommentArgs() const { return m_DiploCommentArgs; }
	DllExport const IDInfo& getTransport() const;
	DllExport void setTransport(const IDInfo& kTransport);
	DllExport const IDInfo& getCity() const;
	DllExport void setCity(const IDInfo& kCity);

	DllExport void read(FDataStreamBase& stream);
	DllExport void write(FDataStreamBase& stream) const;

	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer) const;

	void resetSavedData();

private:
	PlayerTypes m_eWhoTalkingTo;
	DiploCommentTypes m_eCommentType;
	CLinkList<TradeData> m_OurOffer;
	CLinkList<TradeData> m_TheirOffer;
	bool m_bRenegotiate;
	bool m_bAIContact;
	bool m_bPendingDelete;
	int m_iData;
	bool m_bHumanDiplo;
	bool m_bOurOffering;
	bool m_bTheirOffering;
	CvWString m_szChatText;
	std::vector<FVariable> m_DiploCommentArgs;
	IDInfo m_kTransport;
	IDInfo m_kCity;
};

#endif
