#include "CvGameCoreDLL.h"
#include "CvDiploParameters.h"

CvDiploParameters::CvDiploParameters(PlayerTypes ePlayer) :
	m_eWhoTalkingTo(ePlayer),
	m_eCommentType(NO_DIPLOCOMMENT),
	m_bRenegotiate(false),
	m_bAIContact(false),
	m_bPendingDelete(false),
	m_iData(-1),
	m_bHumanDiplo(false),
	m_bOurOffering(false),
	m_bTheirOffering(false)
{
	m_ourOffer.clear();
	m_theirOffer.clear();
}

CvDiploParameters::~CvDiploParameters()
{
	m_ourOffer.clear();
	m_theirOffer.clear();
}

void CvDiploParameters::setWhoTalkingTo(PlayerTypes eWhoTalkingTo)
{
	m_eWhoTalkingTo = eWhoTalkingTo;
}

PlayerTypes CvDiploParameters::getWhoTalkingTo() const
{
	return m_eWhoTalkingTo;
}

void CvDiploParameters::setDiploComment(DiploCommentTypes eCommentType)
{
	m_eCommentType = eCommentType;
}

void CvDiploParameters::addDiploCommentVariable(const wchar *szArg)
{
	if (szArg != NULL)
	{
		FVariable var;
		var.m_eType = FVARTYPE_WSTRING;
		var.m_wszValue = new wchar[wcslen(szArg)+1];
		wcscpy(var.m_wszValue, szArg);
		addDiploCommentVariable(var);
	}
}

void CvDiploParameters::addDiploCommentVariable(int iArg)
{
	if (iArg != MAX_INT)
	{
		FVariable var;
		var.m_eType = FVARTYPE_INT;
		var.m_iValue = iArg;
		addDiploCommentVariable(var);
	}
}

void CvDiploParameters::addDiploCommentVariable(const FVariable& var)
{
	m_diploCommentArgs.push_back(var);
}

DiploCommentTypes CvDiploParameters::getDiploComment() const
{
	return m_eCommentType;
}

void CvDiploParameters::setOurOfferList(const CLinkList<TradeData>& ourOffer)
{
	CLLNode<TradeData> *pNode;

	m_ourOffer.clear();

	for (pNode = ourOffer.head(); pNode; pNode = ourOffer.next(pNode))
	{
		m_ourOffer.insertAtEnd(pNode->m_data);
	}
}

const CLinkList<TradeData>& CvDiploParameters::getOurOfferList() const
{
	return m_ourOffer;
}

void CvDiploParameters::setTheirOfferList(const CLinkList<TradeData>& theirOffer)
{
	CLLNode<TradeData> *pNode;

	m_theirOffer.clear();

	for (pNode = theirOffer.head(); pNode; pNode = theirOffer.next(pNode))
	{
		m_theirOffer.insertAtEnd(pNode->m_data);
	}
}

const CLinkList<TradeData>& CvDiploParameters::getTheirOfferList() const
{
	return m_theirOffer;
}

void CvDiploParameters::setRenegotiate(bool bValue)
{
	m_bRenegotiate = bValue;
}

bool CvDiploParameters::getRenegotiate() const
{
	return m_bRenegotiate;
}

void CvDiploParameters::setAIContact(bool bValue)
{
	m_bAIContact = bValue;
}

bool CvDiploParameters::getAIContact() const
{
	return m_bAIContact;
}


void CvDiploParameters::setPendingDelete(bool bPending)
{
	m_bPendingDelete = bPending;
}

bool CvDiploParameters::getPendingDelete() const
{
	return m_bPendingDelete;
}


void CvDiploParameters::setData(int iData)
{
	m_iData = iData;
}

int CvDiploParameters::getData() const
{
	return m_iData;
}

void CvDiploParameters::setHumanDiplo(bool bValue)
{
	m_bHumanDiplo = bValue;
}

bool CvDiploParameters::getHumanDiplo() const
{
	return m_bHumanDiplo;
}

void CvDiploParameters::setOurOffering(bool bValue)
{
	m_bOurOffering = bValue;
}

bool CvDiploParameters::getOurOffering() const
{
	return m_bOurOffering;
}

void CvDiploParameters::setTheirOffering(bool bValue)
{
	m_bTheirOffering = bValue;
}

bool CvDiploParameters::getTheirOffering() const
{
	return m_bTheirOffering;
}

void CvDiploParameters::setChatText(const wchar* szText)
{
	m_szChatText = szText;
}

const wchar* CvDiploParameters::getChatText() const
{
	return m_szChatText;
}

const IDInfo& CvDiploParameters::getTransport() const
{
	return m_kTransport;
}

void CvDiploParameters::setTransport(const IDInfo& kTransport)
{
	m_kTransport = kTransport;
}

const IDInfo& CvDiploParameters::getCity() const
{
	return m_kCity;
}

void CvDiploParameters::setCity(const IDInfo& kCity)
{
	m_kCity = kCity;
}

void CvDiploParameters::read(FDataStreamBase& stream)
{
	int iType;
	uint uiFlag=0;
	stream.Read(&uiFlag);	// flags for expansion

	stream.Read(&iType);
	m_eWhoTalkingTo = (PlayerTypes)iType;
	stream.Read(&iType);
	m_eCommentType = (DiploCommentTypes)iType;
	m_ourOffer.Read(&stream);
	m_theirOffer.Read(&stream);
	stream.Read(&m_bRenegotiate);
	stream.Read(&m_bAIContact);
	stream.Read(&m_iData);
	stream.Read(&m_bHumanDiplo);
	stream.Read(&m_bOurOffering);
	stream.Read(&m_bTheirOffering);
	stream.ReadString(m_szChatText);

	// read diplo args vec
	int i, iSize;
	stream.Read(&iSize);
	m_diploCommentArgs.resize(iSize);
	for(i=0;i<iSize;i++)
		m_diploCommentArgs[i].Read(&stream);

	m_kTransport.read(&stream);
	m_kCity.read(&stream);
}

void CvDiploParameters::write(FDataStreamBase& stream) const
{
	uint uiFlag=0;
	stream.Write(uiFlag);		// flag for expansion

	stream.Write(m_eWhoTalkingTo);
	stream.Write(m_eCommentType);
	m_ourOffer.Write(&stream);
	m_theirOffer.Write(&stream);
	stream.Write(m_bRenegotiate);
	stream.Write(m_bAIContact);
	stream.Write(m_iData);
	stream.Write(m_bHumanDiplo);
	stream.Write(m_bOurOffering);
	stream.Write(m_bTheirOffering);
	stream.WriteString(m_szChatText);

	// write diplo args vec
	int i, iSize = m_diploCommentArgs.size();
	stream.Write(iSize);
	for(i=0;i<iSize;i++)
		m_diploCommentArgs[i].Write(&stream);

	m_kTransport.write(&stream);
	m_kCity.write(&stream);
}
