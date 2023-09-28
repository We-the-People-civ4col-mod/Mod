#include "CvGameCoreDLL.h"
#include "CvInitCore.h"
#include "CvDiploParameters.h"

#include "CvSavegame.h"

// set the default values
const PlayerTypes defaultWhoTalkingTo = NO_PLAYER;
const DiploCommentTypes defaultCommentType= NO_DIPLOCOMMENT;
const bool defaultRenegotiate = false;
const bool defaultAIContact = false;
const int defaultData = -1;
const bool defaultHumanDiplo = false;
const bool defaultOurOffering = false;
const bool defaultTheirOffering = false;

// 
enum SavegameVariableTypes
{
	DiploParametersSave_END,
	DiploParametersSave_WhoTalkingTo,
	DiploParametersSave_CommentType,
	DiploParametersSave_OurOffer,
	DiploParametersSave_TheirOffer,
	DiploParametersSave_Renegotiate,
	DiploParametersSave_AIContact,
	DiploParametersSave_Data,
	DiploParametersSave_HumanDiplo,
	DiploParametersSave_OurOffering,
	DiploParametersSave_TheirOffering,
	DiploParametersSave_ChatText,
	DiploParametersSave_DiploCommentArgs,
	DiploParametersSave_Transport,
	DiploParametersSave_City,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameDiploParameters(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case DiploParametersSave_END: return "DiploParametersSave_END";
	case DiploParametersSave_WhoTalkingTo: return "DiploParametersSave_WhoTalkingTo";
	case DiploParametersSave_CommentType: return "DiploParametersSave_CommentType";
	case DiploParametersSave_OurOffer: return "DiploParametersSave_OurOffer";
	case DiploParametersSave_TheirOffer: return "DiploParametersSave_TheirOffer";
	case DiploParametersSave_Renegotiate: return "DiploParametersSave_Renegotiate";
	case DiploParametersSave_AIContact: return "DiploParametersSave_AIContact";
	case DiploParametersSave_Data: return "DiploParametersSave_Data";
	case DiploParametersSave_HumanDiplo: return "DiploParametersSave_HumanDiplo";
	case DiploParametersSave_OurOffering: return "DiploParametersSave_OurOffering";
	case DiploParametersSave_TheirOffering: return "DiploParametersSave_TheirOffering";
	case DiploParametersSave_ChatText: return "DiploParametersSave_ChatText";
	case DiploParametersSave_DiploCommentArgs: return "DiploParametersSave_DiploCommentArgs";
	case DiploParametersSave_Transport: return "DiploParametersSave_Transport";
	case DiploParametersSave_City: return "DiploParametersSave_City";
	}
	FAssertMsg(0, "Missing case");
	return "";
}

int getNumSavedEnumValuesDiploParameters()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvDiploParameters::resetSavedData()
{
	m_eWhoTalkingTo = defaultWhoTalkingTo;
	m_eCommentType = defaultCommentType;
	m_OurOffer.clear();
	m_TheirOffer.clear();
	m_bRenegotiate = defaultRenegotiate;
	m_bAIContact = defaultAIContact;
	m_iData = defaultData;
	m_bHumanDiplo = defaultHumanDiplo;
	m_bOurOffering = defaultOurOffering;
	m_bTheirOffering = defaultTheirOffering;
	m_szChatText.clear();
	m_DiploCommentArgs.clear();
	m_kTransport.reset();
	m_kCity.reset();
}

void CvDiploParameters::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_DIPLOPARAMETERS);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	resetSavedData();

	// loop read all the variables
	// As long as each variable has a UnitSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case DiploParametersSave_END: bContinue = false; break;
		case DiploParametersSave_WhoTalkingTo: reader.Read(m_eWhoTalkingTo); break;
		case DiploParametersSave_CommentType: 
			DiplomacyTypes eTmp; // it is saved as DiplomacyTypes
			reader.Read(eTmp); m_eCommentType = (DiploCommentTypes)eTmp; break;
		case DiploParametersSave_OurOffer: reader.Read(m_OurOffer); break;
		case DiploParametersSave_TheirOffer: reader.Read(m_TheirOffer); break;
		case DiploParametersSave_Renegotiate: reader.Read(m_bRenegotiate); break;
		case DiploParametersSave_AIContact: reader.Read(m_bAIContact); break;
		case DiploParametersSave_Data: reader.Read(m_iData); break;
		case DiploParametersSave_HumanDiplo: reader.Read(m_bHumanDiplo); break;
		case DiploParametersSave_OurOffering: reader.Read(m_bOurOffering); break;
		case DiploParametersSave_TheirOffering: reader.Read(m_bTheirOffering); break;
		case DiploParametersSave_ChatText: reader.Read(m_szChatText); break;
		case DiploParametersSave_DiploCommentArgs: reader.Read(m_DiploCommentArgs);	break;
		case DiploParametersSave_Transport: reader.Read(m_kTransport); break;
		case DiploParametersSave_City: reader.Read(m_kCity); break;
		
		}
	}
	
}

void CvDiploParameters::write(CvSavegameWriter writer) const
{
	LogIntentHelper helper(writer, "CvDiploParameters");

	writer.AssignClassType(SAVEGAME_CLASS_DIPLOPARAMETERS);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(DiploParametersSave_WhoTalkingTo, m_eWhoTalkingTo, defaultWhoTalkingTo);
	writer.Write(DiploParametersSave_CommentType, (DiplomacyTypes)m_eCommentType, (DiplomacyTypes)defaultCommentType);
	writer.Write(DiploParametersSave_OurOffer, m_OurOffer);
	writer.Write(DiploParametersSave_TheirOffer, m_TheirOffer);
	writer.Write(DiploParametersSave_Renegotiate, m_bRenegotiate, defaultRenegotiate);
	writer.Write(DiploParametersSave_AIContact, m_bAIContact, defaultAIContact);
	writer.Write(DiploParametersSave_Data, m_iData, defaultData);
	writer.Write(DiploParametersSave_HumanDiplo, m_bHumanDiplo, defaultHumanDiplo);
	writer.Write(DiploParametersSave_OurOffering, m_bOurOffering, defaultOurOffering);
	writer.Write(DiploParametersSave_TheirOffering, m_bTheirOffering, defaultTheirOffering);
	writer.Write(DiploParametersSave_ChatText, m_szChatText);
	writer.Write(DiploParametersSave_DiploCommentArgs, m_DiploCommentArgs);
	writer.Write(DiploParametersSave_Transport, m_kTransport);
	writer.Write(DiploParametersSave_City, m_kCity);
	
	writer.Write(DiploParametersSave_END);
}
