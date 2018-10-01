#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvInfos.h
//
//  PURPOSE: All Civ4 info classes and the base class for them
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef CV_INFO_H
#define CV_INFO_H
#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class
#pragma warning( disable: 4127 )
class CvXMLLoadUtility;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInfoBase
//
//  DESC:   The base class for all info classes to inherit from.  This gives us
//			the base description and type strings
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInfoBase
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	//constructor
	DllExport CvInfoBase();
	//destructor
	DllExport virtual ~CvInfoBase();
	DllExport virtual void reset();
	DllExport bool isGraphicalOnly() const;

	DllExport const char* getType() const;
	DllExport virtual const char* getButton() const;
	// for python wide string handling
	std::wstring pyGetTextKey() { return getTextKeyWide(); }
	std::wstring pyGetDescription() { return getDescription(0); }
	std::wstring pyGetDescriptionForm(uint uiForm) { return getDescription(uiForm); }
	std::wstring pyGetText() { return getText(); }
	std::wstring pyGetCivilopedia() { return getCivilopedia(); }
	std::wstring pyGetHelp() { return getHelp(); }
	std::wstring pyGetStrategy() { return getStrategy(); }

	DllExport const wchar* getTextKeyWide() const;
	DllExport const wchar* getDescription(uint uiForm = 0) const;
	DllExport const wchar* getText() const;
	DllExport const wchar* getCivilopedia() const;
	DllExport const wchar* getHelp() const;
	DllExport const wchar* getStrategy() const;

	DllExport bool isMatchForLink(std::wstring szLink, bool bKeysOnly) const;
	DllExport virtual void read(FDataStreamBase* pStream);
	DllExport virtual void write(FDataStreamBase* pStream);
	DllExport virtual bool read(CvXMLLoadUtility* pXML);
	DllExport virtual bool readPass2(CvXMLLoadUtility* pXML) { return false; }
	DllExport virtual bool readPass3() { FAssertMsg(false, "Override this"); return false; }

protected:
	bool doneReadingXML(CvXMLLoadUtility* pXML);
	bool m_bGraphicalOnly;
	CvString m_szType;
	CvString m_szButton;				// Used for Infos that don't require an ArtAssetInfo
	CvWString m_szTextKey;
	CvWString m_szCivilopediaKey;
	CvWString m_szHelpKey;
	CvWString m_szStrategyKey;

	std::vector<CvString> m_aszExtraXMLforPass3;

	// translated text
	mutable CvWString m_szCachedText;
	mutable CvWString m_szCachedCivilopedia;
	mutable CvWString m_szCachedHelp;
	mutable CvWString m_szCachedStrategy;
	mutable std::vector<CvWString> m_aCachedDescriptions;
};

//
// holds the scale for scalable objects
//
class CvScalableInfo
{
public:
	DllExport CvScalableInfo() : m_fScale(1.0f), m_fInterfaceScale(1.0f) { }
	DllExport float getScale() const;
	DllExport void setScale(float fScale);
	DllExport float getInterfaceScale() const;
	DllExport void setInterfaceScale(float fInterfaceScale);
	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	float m_fScale;
	float m_fInterfaceScale;	//!< the scale of the unit appearing in the interface screens
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvHotkeyInfo
//!  \brief			holds the hotkey info for an info class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHotkeyInfo : public CvInfoBase
{
public:
	//constructor
	DllExport CvHotkeyInfo();
	//destructor
	DllExport virtual ~CvHotkeyInfo();
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport virtual void read(FDataStreamBase* pStream);
	DllExport virtual void write(FDataStreamBase* pStream);
	DllExport int getActionInfoIndex() const;
	DllExport void setActionInfoIndex(int i);
	DllExport int getHotKeyVal() const;
	DllExport void setHotKeyVal(int i);
	DllExport int getHotKeyPriority() const;
	DllExport void setHotKeyPriority(int i);
	DllExport int getHotKeyValAlt() const;
	DllExport void setHotKeyValAlt(int i);
	DllExport int getHotKeyPriorityAlt() const;
	DllExport void setHotKeyPriorityAlt(int i);
	DllExport int getOrderPriority() const;
	DllExport void setOrderPriority(int i);
	DllExport bool isAltDown() const;
	DllExport void setAltDown(bool b);
	DllExport bool isShiftDown() const;
	DllExport void setShiftDown(bool b);
	DllExport bool isCtrlDown() const;
	DllExport void setCtrlDown(bool b);
	DllExport bool isAltDownAlt() const;
	DllExport void setAltDownAlt(bool b);
	DllExport bool isShiftDownAlt() const;
	DllExport void setShiftDownAlt(bool b);
	DllExport bool isCtrlDownAlt() const;
	DllExport void setCtrlDownAlt(bool b);
	DllExport const char* getHotKey() const;
	DllExport void setHotKey(const char* szVal);
	DllExport std::wstring getHotKeyDescription() const;
	DllExport void setHotKeyDescription(const wchar* szHotKeyDescKey, const wchar* szHotKeyAltDescKey, const wchar* szHotKeyString);

protected:
	int m_iActionInfoIndex;
	int m_iHotKeyVal;
	int m_iHotKeyPriority;
	int m_iHotKeyValAlt;
	int m_iHotKeyPriorityAlt;
	int m_iOrderPriority;
	bool m_bAltDown;
	bool m_bShiftDown;
	bool m_bCtrlDown;
	bool m_bAltDownAlt;
	bool m_bShiftDownAlt;
	bool m_bCtrlDownAlt;
	CvString m_szHotKey;
	CvWString m_szHotKeyDescriptionKey;
	CvWString m_szHotKeyAltDescriptionKey;
	CvWString m_szHotKeyString;
};

class CvDiplomacyResponse
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvDiplomacyResponse();
	DllExport virtual ~CvDiplomacyResponse();
	DllExport int getNumDiplomacyText();
	DllExport void setNumDiplomacyText(int i);
	DllExport bool getCivilizationTypes(int i);
	DllExport bool* getCivilizationTypes() const;
	DllExport void setCivilizationTypes(int i, bool bVal);
	DllExport bool getLeaderHeadTypes(int i);
	DllExport bool* getLeaderHeadTypes() const;
	DllExport void setLeaderHeadTypes(int i, bool bVal);
	DllExport bool getAttitudeTypes(int i) const;
	DllExport bool* getAttitudeTypes() const;
	DllExport void setAttitudeTypes(int i, bool bVal);
	DllExport bool getDiplomacyPowerTypes(int i);
	DllExport bool* getDiplomacyPowerTypes() const;
	DllExport void setDiplomacyPowerTypes(int i, bool bVal);
	DllExport const char* getDiplomacyText(int i) const;
	DllExport const CvString* getDiplomacyText() const;
	DllExport void setDiplomacyText(int i, CvString szText);
	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iNumDiplomacyText;
	bool* m_abCivilizationTypes;
	bool* m_abLeaderHeadTypes;
	bool* m_abAttitudeTypes;
	bool* m_abDiplomacyPowerTypes;
	CvString* m_paszDiplomacyText;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPromotionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPromotionInfo :	public CvHotkeyInfo
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvPromotionInfo();
	DllExport virtual ~CvPromotionInfo();
	DllExport int getPrereqPromotion() const;
	DllExport void setPrereqPromotion(int i);
	DllExport int getPrereqOrPromotion1() const;
	DllExport void setPrereqOrPromotion1(int i);
	DllExport int getPrereqOrPromotion2() const;
	DllExport void setPrereqOrPromotion2(int i);

	DllExport int getVisibilityChange() const;
	DllExport int getMovesChange() const;
	DllExport int getMoveDiscountChange() const;
	DllExport int getWithdrawalChange() const;
	DllExport int getCargoChange() const;
	DllExport int getBombardRateChange() const;
	DllExport int getEnemyHealChange() const;
	DllExport int getNeutralHealChange() const;
	DllExport int getFriendlyHealChange() const;
	DllExport int getSameTileHealChange() const;
	DllExport int getAdjacentTileHealChange() const;
	DllExport int getCombatPercent() const;
	DllExport int getCityAttackPercent() const;
	DllExport int getCityDefensePercent() const;
	DllExport int getHillsAttackPercent() const;
	DllExport int getHillsDefensePercent() const;
	DllExport int getDomesticBonusPercent() const;	
	DllExport int getCommandType() const;
	DllExport void setCommandType(int iNewType);
	DllExport int getPillageChange() const;
	DllExport int getUpgradeDiscount() const;
	DllExport int getExperiencePercent() const;

	DllExport bool isLeader() const;
	DllExport bool isBlitz() const;
	DllExport bool isAmphib() const;
	DllExport bool isRiver() const;
	DllExport bool isEnemyRoute() const;
	DllExport bool isAlwaysHeal() const;
	DllExport bool isHillsDoubleMove() const;

	DllExport const char* getSound() const;
	DllExport void setSound(const char* szVal);
	// Arrays
	DllExport int getTerrainAttackPercent(int i) const;
	DllExport int getTerrainDefensePercent(int i) const;
	DllExport int getFeatureAttackPercent(int i) const;
	DllExport int getFeatureDefensePercent(int i) const;
	DllExport int getUnitClassAttackModifier(int i) const;
	DllExport int getUnitClassDefenseModifier(int i) const;
	DllExport int getUnitCombatModifierPercent(int i) const;
	DllExport int getDomainModifierPercent(int i) const;

	DllExport bool getTerrainDoubleMove(int i) const;
	DllExport bool getFeatureDoubleMove(int i) const;
	DllExport bool getUnitCombat(int i) const;

	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport bool readPass2(CvXMLLoadUtility* pXML);

protected:

	int m_iPrereqPromotion;
	int m_iPrereqOrPromotion1;
	int m_iPrereqOrPromotion2;
	int m_iVisibilityChange;
	int m_iMovesChange;
	int m_iMoveDiscountChange;
	int m_iWithdrawalChange;
	int m_iCargoChange;
	int m_iBombardRateChange;
	int m_iEnemyHealChange;
	int m_iNeutralHealChange;
	int m_iFriendlyHealChange;
	int m_iSameTileHealChange;
	int m_iAdjacentTileHealChange;
	int m_iCombatPercent;
	int m_iCityAttackPercent;
	int m_iCityDefensePercent;
	int m_iHillsAttackPercent;
	int m_iHillsDefensePercent;
	int m_iDomesticBonusPercent;	
	int m_iCommandType;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	bool m_bLeader;
	bool m_bBlitz;
	bool m_bAmphib;
	bool m_bRiver;
	bool m_bEnemyRoute;
	bool m_bAlwaysHeal;
	bool m_bHillsDoubleMove;

	CvString m_szSound;
	// Arrays
	int* m_aiTerrainAttackPercent;
	int* m_aiTerrainDefensePercent;
	int* m_aiFeatureAttackPercent;
	int* m_aiFeatureDefensePercent;
	int* m_aiUnitClassAttackModifier;
	int* m_aiUnitClassDefenseModifier;
	int* m_aiUnitCombatModifierPercent;
	int* m_aiDomainModifierPercent;
	bool* m_abTerrainDoubleMove;
	bool* m_abFeatureDoubleMove;
	bool* m_abUnitCombat;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvProfessionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProfessionInfo :	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvProfessionInfo();
	DllExport virtual ~CvProfessionInfo();
	DllExport int getUnitCombatType() const;
	// TAC - LbD - Ray - START
	DllExport bool LbD_isUsed() const;
	DllExport int LbD_getExpert() const;
	DllExport int LbD_getLearnLevel() const;
	// TAC - LbD - Ray - END
	DllExport int getDefaultUnitAIType() const;
	// R&R, ray , MYCP partially based on code of Aymerick - START
	// DllExport int getYieldProduced() const;
	// DllExport int getYieldConsumed() const;
	// R&R, ray , MYCP partially based on code of Aymerick - END
	DllExport int getSpecialBuilding() const;
	DllExport int getCombatChange() const;
	DllExport int getMovesChange() const;
	DllExport int getWorkRate() const;
	DllExport int getMissionaryRate() const;
	DllExport int getPowerValue() const;
	DllExport int getAssetValue() const;
	DllExport bool isWorkPlot() const;
	DllExport bool isCitizen() const;
	DllExport bool isWater() const;
	DllExport bool isScout() const;
	DllExport bool isCityDefender() const;
	DllExport bool canFound() const;
	DllExport bool isUnarmed() const;
	DllExport bool isNoDefensiveBonus() const;
	DllExport int getYieldEquipmentAmount(int iYield) const;
	DllExport bool isFreePromotion(int i) const;
	// R&R, ray , MYCP partially based on code of Aymerick - START
	DllExport int getYieldsProduced(int i) const;
	DllExport int getNumYieldsProduced() const;
	DllExport int getYieldsConsumed(int i) const;
	DllExport int getNumYieldsConsumed() const;
	// R&R, ray , MYCP partially based on code of Aymerick - END

	/// Move Into Peak - start - Nightinggale
	bool allowsMoveIntoPeak() const {return m_bMoveIntoPeak;}
	/// Move Into Peak - end - Nightinggale

	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport bool readPass3();


	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iUnitCombatType;
	int m_iDefaultUnitAIType;
	// R&R, ray , MYCP partially based on code of Aymerick - START
	// int m_iYieldProduced;
	// int m_iYieldConsumed;
	// R&R, ray , MYCP partially based on code of Aymerick - END
	int m_iSpecialBuilding;
	int m_iCombatChange;
	int m_iMovesChange;
	int m_iWorkRate;
	int m_iMissionaryRate;
	int m_iPowerValue;
	int m_iAssetValue;
	bool m_bWorkPlot;
	bool m_bCitizen;
	bool m_bWater;
	bool m_bScout;
	bool m_bCityDefender;
	bool m_bCanFound;
	bool m_bUnarmed;
	bool m_bNoDefensiveBonus;

	/// Move Into Peak - start - Nightinggale
	bool m_bMoveIntoPeak;
	/// Move Into Peak - end - Nightinggale

	// TAC - LbD - Ray - START
	bool m_LbDused;
	int m_expert;
	int m_learnLevel;
	// TAC - LbD - Ray - END

	struct YieldEquipment
	{
		int iYieldType;
		int iYieldAmount;
	};

	std::vector<YieldEquipment> m_aYieldEquipments;
	bool* m_abFreePromotions;
	// R&R, ray , MYCP partially based on code of Aymerick - START
	std::vector<int> m_aiYieldsProduced;
	std::vector<int> m_aiYieldsConsumed;
	// R&R, ray , MYCP partially based on code of Aymerick - END
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMissionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMissionInfo : public CvHotkeyInfo
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvMissionInfo();
	DllExport virtual ~CvMissionInfo();
	DllExport int getTime() const;

	DllExport bool isSound() const;
	DllExport bool isTarget() const;
	DllExport bool isBuild() const;
	DllExport bool getVisible() const;
	DllExport EntityEventTypes getEntityEvent() const;
	DllExport const char* getWaypoint() const;		// effect type, Exposed to Python
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iTime;
	bool m_bSound;
	bool m_bTarget;
	bool m_bBuild;
	bool m_bVisible;
	EntityEventTypes m_eEntityEvent;
	CvString m_szWaypoint;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvControlInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvControlInfo : public CvHotkeyInfo
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvControlInfo();
	DllExport virtual ~CvControlInfo();
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCommandInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCommandInfo : public CvHotkeyInfo
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvCommandInfo();
	DllExport virtual ~CvCommandInfo();
	DllExport bool getConfirmCommand() const;
	DllExport bool getVisible() const;
	DllExport bool getAll() const;
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	bool m_bConfirmCommand;
	bool m_bVisible;
	bool m_bAll;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAutomateInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAutomateInfo : public CvHotkeyInfo
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvAutomateInfo();
	DllExport virtual ~CvAutomateInfo();
	DllExport int getCommand() const;
	DllExport void setCommand(int i);
	DllExport int getAutomate() const;
	DllExport void setAutomate(int i);
	DllExport bool getConfirmCommand() const;
	DllExport void setConfirmCommand(bool bVal);
	DllExport bool getVisible() const;
	DllExport void setVisible(bool bVal);
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iCommand;
	int m_iAutomate;
	bool m_bConfirmCommand;
	bool m_bVisible;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvActionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvActionInfo
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvActionInfo();
	DllExport virtual ~CvActionInfo();
	DllExport int getMissionData() const;
	DllExport int getCommandData() const;

	DllExport int getAutomateType() const;
	DllExport int getInterfaceModeType() const;
	DllExport int getMissionType() const;
	DllExport int getCommandType() const;
	DllExport int getControlType() const;
	DllExport int getOriginalIndex() const;
	DllExport void setOriginalIndex(int i);
	DllExport bool isConfirmCommand() const;
	DllExport bool isVisible() const;
	DllExport ActionSubTypes getSubType() const;
	DllExport void setSubType(ActionSubTypes eSubType);
	// functions to replace the CvInfoBase calls
	DllExport const char* getType() const;
	DllExport const wchar* getDescription() const;
	DllExport const wchar* getCivilopedia() const;
	DllExport const wchar* getHelp() const;
	DllExport const wchar* getStrategy() const;
	DllExport virtual const char* getButton() const;
	DllExport const wchar* getTextKeyWide() const;
	// functions to replace the CvHotkey calls
	DllExport int getActionInfoIndex() const;
	DllExport int getHotKeyVal() const;
	DllExport int getHotKeyPriority() const;
	DllExport int getHotKeyValAlt() const;
	DllExport int getHotKeyPriorityAlt() const;
	DllExport int getOrderPriority() const;
	DllExport bool isAltDown() const;
	DllExport bool isShiftDown() const;
	DllExport bool isCtrlDown() const;
	DllExport bool isAltDownAlt() const;
	DllExport bool isShiftDownAlt() const;
	DllExport bool isCtrlDownAlt() const;
	DllExport const char* getHotKey() const;

	DllExport std::wstring getHotKeyDescription() const;

//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iOriginalIndex;
	ActionSubTypes m_eSubType;
private:
	CvHotkeyInfo* getHotkeyInfo() const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitMeshGroup
{
public:
	CvUnitMeshGroup();
	CvUnitMeshGroup(int iNumRequired, const CvString &szArtDefineTag);
	~CvUnitMeshGroup();
	int getNumRequired() const;
	const CvString& getArtDefineTag() const;
	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
private:
	int m_iNumRequired;
	CvString m_szArtDefineTag;
};

class CvUnitMeshGroups
{
public:
	CvUnitMeshGroups();
	CvUnitMeshGroups(ProfessionTypes eProfession, int iMeleeWaveSize, int iRangedWaveSize, float fMaxSpeed, float fPadTime);
	~CvUnitMeshGroups();
	void addMeshGroup(const CvUnitMeshGroup& kMeshGroup);
	ProfessionTypes getProfession() const;
	int getMeleeWaveSize() const;
	int getRangedWaveSize() const;
	float getMaxSpeed() const;
	float getPadTime() const;
	int getNumMeshGroups() const;
	const CvUnitMeshGroup& getMeshGroup(int index) const;
	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
private:
	ProfessionTypes m_eProfession;
	int m_iMeleeWaveSize;
	int m_iRangedWaveSize;
	float m_fMaxSpeed;
	float m_fPadTime;
	std::vector<CvUnitMeshGroup> m_aMeshGroups;
};

class CvArtInfoUnit;
class CvUnitInfo : public CvHotkeyInfo
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvUnitInfo();
	DllExport virtual ~CvUnitInfo();

	// PatchMod: Berth size START
protected:
	int m_iBerthSize;
public:
	DllExport int getBerthSize() const;
	// PatchMod: Berth size END

/** NBMOD EDU **/
    DllExport int NBMOD_GetTeachLevel() const;
    float NBMOD_GetStrengthWeight() const;
/** NBMOD EDU **/
	DllExport int getAIWeight() const;
	DllExport int getHurryCostModifier() const;
	DllExport int getEuropeCost() const;
	DllExport int getEuropeCostIncrease() const;
	/*** TRIANGLETRADE 10/24/08 by DPII ***/
	DllExport int getAfricaCost() const;
	DllExport int getAfricaCostIncrease() const;
	/**************************************/
	// R&R, ray, Port Royal
	DllExport int getPortRoyalCost() const;
	DllExport int getPortRoyalCostIncrease() const;
	// R&R, ray, Port Royal - END
	DllExport int getImmigrationWeight() const;
	DllExport int getImmigrationWeightDecay() const;
	DllExport int getAdvancedStartCost() const;
	DllExport int getAdvancedStartCostIncrease() const;
	DllExport int getMinAreaSize() const;
	DllExport int getMoves() const;
	DllExport int getWorkRate() const;
	DllExport int getWorkRateModifier() const;
	DllExport int getMissionaryRateModifier() const;
	DllExport int getCombat() const;
	DllExport void setCombat(int iNum);
	DllExport int getXPValueAttack() const;
	DllExport int getXPValueDefense() const;
	DllExport int getWithdrawalProbability() const;
	DllExport int getCityAttackModifier() const;
	DllExport int getCityDefenseModifier() const;
	DllExport int getHillsAttackModifier() const;
	DllExport int getHillsDefenseModifier() const;
	DllExport int getDomesticBonusModifier() const;	
	DllExport int getBombardRate() const;
	DllExport int getSpecialCargo() const;
	DllExport int getDomainCargo() const;

	DllExport int getCargoSpace() const;
	DllExport int getRequiredTransportSize() const;
	DllExport int getAssetValue() const;
	DllExport int getPowerValue() const;
	DllExport int getUnitClassType() const;
	DllExport int getSpecialUnitType() const;
	DllExport int getUnitCaptureClassType() const;
	DllExport int getUnitCombatType() const;
	DllExport int getDomainType() const;
	DllExport int getDefaultProfession() const;
	DllExport int getDefaultUnitAIType() const;
	DllExport int getInvisibleType() const;
	DllExport int getSeeInvisibleType(int i) const;
	DllExport int getNumSeeInvisibleTypes() const;
	DllExport int getPrereqBuilding() const;
	DllExport int getGroupSize(int iProfession) const;									 // the initial number of individuals in the unit group
	DllExport int getGroupDefinitions(int iProfession) const;					 // the number of UnitMeshGroups for this unit
	DllExport int getMeleeWaveSize(int iProfession) const;
	DllExport int getRangedWaveSize(int iProfession) const;
	DllExport int getNumUnitNames() const;
	DllExport int getCommandType() const;
	DllExport void setCommandType(int iNewType);
	DllExport int getLearnTime() const;
	DllExport int getStudentWeight() const;
	DllExport int getTeacherWeight() const;
	// < JAnimals Mod Start >
	DllExport int getAnimalPatrolWeight() const;
	DllExport int getAnimalAttackWeight() const;
	// < JAnimals Mod End >
	DllExport bool isNoBadGoodies() const;
	DllExport bool isOnlyDefensive() const;
	DllExport bool isNoCapture() const;
	DllExport bool isQuickCombat() const;
	DllExport bool isRivalTerritory() const;
	DllExport bool isMilitaryProduction() const;
	DllExport bool isFound() const;
	DllExport bool isInvisible() const;
	DllExport void setInvisible(bool bEnable) ;
	DllExport bool isNoDefensiveBonus() const;
	DllExport bool isCanMoveImpassable() const;
	DllExport bool isCanMoveAllTerrain() const;
	DllExport bool isFlatMovementCost() const;
	DllExport bool isIgnoreTerrainCost() const;
	DllExport bool isMechUnit() const;
	DllExport bool isLineOfSight() const;
	DllExport bool isHiddenNationality() const;
	DllExport bool isAlwaysHostile() const;
	DllExport bool isTreasure() const;
	// R&R, ray, Changes for Treasures, START
	DllExport bool isNoRevealMap() const;
	// R&R, ray, Changes for Treasures, END

	// TAC - LbD - Ray - START
	DllExport bool LbD_canBecomeExpert() const;
	DllExport bool LbD_canGetFree() const;
	DllExport bool LbD_canEscape() const;
	// TAC - LbD - Ray - END

	DllExport bool isCapturesCargo() const;
	// TAC Capturing Ships - ray
	DllExport bool isCapturesShips() const;
	// TAC Capturing Ships - ray -END
	DllExport bool isLandYieldChanges() const;
	DllExport bool isWaterYieldChanges() const;

	//TAC Whaling, ray
	DllExport bool isGatherBoat() const;
	//End TAC Whaling, ray
	// < JAnimals Mod Start >
	DllExport bool isAnimal() const;
	// < JAnimals Mod End >
	DllExport float getUnitMaxSpeed(int iProfession) const;
	DllExport float getUnitPadTime(int iProfession) const;

	/// Move Into Peak - start - Nightinggale
	bool allowsMoveIntoPeak() const {return m_bMoveIntoPeak;}
	/// Move Into Peak - end - Nightinggale

	// Arrays
	DllExport int getProductionTraits(int i) const;
	DllExport int getTerrainAttackModifier(int i) const;
	DllExport int getTerrainDefenseModifier(int i) const;
	DllExport int getFeatureAttackModifier(int i) const;
	DllExport int getFeatureDefenseModifier(int i) const;
	DllExport int getUnitClassAttackModifier(int i) const;
	DllExport int getUnitClassDefenseModifier(int i) const;
	DllExport int getUnitCombatModifier(int i) const;
	DllExport int getDomainModifier(int i) const;
	DllExport int getYieldModifier(int i) const;
	// R&R, Androrc, Domestic Market
	DllExport int getYieldDemand(int i) const;
	//Androrc End
	DllExport int getBonusYieldChange(int i) const;
	DllExport int getYieldChange(int i) const;
	DllExport int getYieldCost(int i) const;
	DllExport int getUnitGroupRequired(int i, int iProfession) const;

	DllExport bool getUpgradeUnitClass(int i) const;
	DllExport bool getUnitAIType(int i) const;
	DllExport bool getNotUnitAIType(int i) const;
	DllExport bool getBuilds(int i) const;
	DllExport bool getTerrainImpassable(int i) const;
	DllExport bool getFeatureImpassable(int i) const;
	// < JAnimals Mod Start >
	DllExport bool getTerrainNative(int iIndex) const;
	// < JAnimals Mod End >
	DllExport bool isEvasionBuilding(int i) const;
	//TAC Whaling, ray
	DllExport int getNumYieldsGatherable() const;
	DllExport bool canGatherYield(int index) const;
	//End TAC Whaling, ray
	DllExport bool getFreePromotions(int i) const;
	DllExport bool getProfessionsNotAllowed(int i) const;  	///TK Viscos Mod
	DllExport bool isPrereqOrBuilding(int i) const;
	DllExport int getLeaderPromotion() const;
	DllExport int getLeaderExperience() const;

	//Androrc UnitArtStyles
	//DllExport const char* getArtDefineTag(int i, int iProfession) const;
	DllExport const char* getArtDefineTag(int i, int iProfession, int iStyle = -1) const;
	//Androrc End
	DllExport const char* getUnitNames(int i) const;
	DllExport const char* getFormationType() const;
	DllExport const char* getButton() const;
	DllExport void updateArtDefineButton();
	DllExport const CvArtInfoUnit* getArtInfo(int i, int iProfession) const;
	//Androrc UnitArtStyles
	DllExport const CvArtInfoUnit* getUnitArtStylesArtInfo(int i, int iProfession, int iStyle = -1) const;
	//Androrc End
	DllExport const CvUnitMeshGroups& getProfessionMeshGroup(int iProfession) const;
	DllExport void read(FDataStreamBase* );
	DllExport void write(FDataStreamBase* );
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

    /** NBMOD EDU **/
    int m_iTeachLevel;
    float m_fNBMOD_REF_StrengthWeight;
    /** NBMOD EDU **/

	int m_iAIWeight;
	int m_iHurryCostModifier;
	int m_iEuropeCost;
	int m_iEuropeCostIncrease;
	/*** TRIANGLETRADE 10/24/08 by DPII ***/
	int m_iAfricaCost;
	int m_iAfricaCostIncrease;
	/**************************************/
	// R&R, ray, Port Royal
	int m_iPortRoyalCost;
	int m_iPortRoyalCostIncrease;
	// R&R, ray, Port Royal - END
	int m_iImmigrationWeight;
	int m_iImmigrationWeightDecay;
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
	int m_iMinAreaSize;
	int m_iMoves;
	int m_iWorkRate;
	int m_iWorkRateModifier;
	int m_iMissionaryRateModifier;
	int m_iCombat;
	int m_iXPValueAttack;
	int m_iXPValueDefense;
	int m_iWithdrawalProbability;
	int m_iCityAttackModifier;
	int m_iCityDefenseModifier;
	int m_iHillsAttackModifier;
	int m_iHillsDefenseModifier;
	int m_iDomesticBonusModifier;	
	int m_iBombardRate;
	int m_iSpecialCargo;
	int m_iDomainCargo;
	int m_iCargoSpace;
	int m_iRequiredTransportSize;
	int m_iAssetValue;
	int m_iPowerValue;
	int m_iUnitClassType;
	int m_iSpecialUnitType;
	int m_iUnitCaptureClassType;
	int m_iUnitCombatType;
	int m_iDomainType;
	int m_iDefaultProfession;
	int m_iDefaultUnitAIType;
	int m_iInvisibleType;
	int m_iPrereqBuilding;
	int m_iNumUnitNames;
	int m_iCommandType;
	int m_iLeaderExperience;
	int m_iLearnTime;
	int m_iStudentWeight;
	int m_iTeacherWeight;
	// < JAnimals Mod Start >
	int m_iAnimalPatrolWeight;
	int m_iAnimalAttackWeight;
	// < JAnimals Mod End >
	bool m_bNoBadGoodies;
	bool m_bOnlyDefensive;
	bool m_bNoCapture;
	bool m_bQuickCombat;
	bool m_bRivalTerritory;
	bool m_bMilitaryProduction;
	bool m_bPillage;
	bool m_bFound;
	bool m_bInvisible;
	bool m_bNoDefensiveBonus;
	bool m_bCanMoveImpassable;
	bool m_bCanMoveAllTerrain;
	bool m_bFlatMovementCost;
	bool m_bIgnoreTerrainCost;
	bool m_bMechanized;
	bool m_bLineOfSight;
	bool m_bHiddenNationality;
	bool m_bAlwaysHostile;
	bool m_bTreasure;
	// R&R, ray, Changes for Treasures, START
	bool m_bNoRevealMap;
	// R&R, ray, Changes for Treasures, END
	
	// TAC - LbD - Ray - START
	bool m_canBecomeExpert;
	bool m_canGetFree;
	bool m_canEscape;
	// TAC - LbD - Ray - END

	bool m_bCapturesCargo;
	// TAC Capturing Ships - ray
	bool m_bCapturesShips;
	// TAC Capturing Ships - ray - END
	bool m_bLandYieldChanges;
	bool m_bWaterYieldChanges;
	//TAC Whaling, ray
	bool m_bGatherBoat;
	//End TAC Whaling, ray
	// < JAnimals Mod Start >
	bool m_bAnimal;
	// < JAnimals Mod End >
	int m_iLeaderPromotion;

	/// Move Into Peak - start - Nightinggale
	bool m_bMoveIntoPeak;
	/// Move Into Peak - end - Nightinggale

	// Arrays
	int* m_aiProductionTraits;
	int* m_aiTerrainAttackModifier;
	int* m_aiTerrainDefenseModifier;
	int* m_aiFeatureAttackModifier;
	int* m_aiFeatureDefenseModifier;
	int* m_aiUnitClassAttackModifier;
	int* m_aiUnitClassDefenseModifier;
	int* m_aiUnitCombatModifier;
	int* m_aiDomainModifier;
	int* m_aiYieldModifier;
	// R&R, Androrc, Domestic Market
	int* m_aiYieldDemand;
	//Androrc End
	int* m_aiBonusYieldChange;
	int* m_aiYieldChange;
	int* m_aiYieldCost;
	bool* m_abUpgradeUnitClass;
	bool* m_abUnitAIType;
	bool* m_abNotUnitAIType;
	bool* m_abBuilds;
	bool* m_abTerrainImpassable;
	bool* m_abFeatureImpassable;
	// < JAnimals Mod Start >
	bool* m_abTerrainNative;
	// < JAnimals Mod End >
	bool* m_abEvasionBuilding;
	//TAC Whaling, ray
	bool* m_abYieldGatherable;
	//End TAC Whaling, ray
	bool* m_abFreePromotions;
	///TK Viscos Mod
	bool* m_abProfessionsNotAllowed;
	///TK end
	bool* m_abPrereqOrBuilding;
	CvString* m_paszUnitNames;
	CvString m_szFormationType;
	CvString m_szArtDefineButton;
	std::vector<CvUnitMeshGroups> m_aProfessionGroups;
	std::vector<int> m_aiSeeInvisibleTypes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// class	: CvUnitFormationInfo
//
// \brief	: Holds information relating to the formation of sub-units within a unit
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitEntry
{
public:
	CvUnitEntry()
	{
	}
	CvUnitEntry(const NiPoint2 &position, float radius, float facingDirection, float facingVariance) :
	m_position(position),
	m_fRadius(radius),
	m_fFacingDirection(facingDirection),
	m_fFacingVariance(facingVariance)
	{
	}
	NiPoint2 m_position;
	float m_fRadius;
	float m_fFacingDirection;
	float m_fFacingVariance;
};

class CvUnitFormationInfo : public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvUnitFormationInfo();
	DllExport virtual ~CvUnitFormationInfo();
	DllExport const char* getFormationType() const;
	DllExport const std::vector<EntityEventTypes> & getEventTypes() const;
	DllExport int getNumUnitEntries() const;
	DllExport const CvUnitEntry &getUnitEntry(int index) const;
	DllExport void addUnitEntry(const CvUnitEntry &unitEntry);
	DllExport int getNumGreatUnitEntries() const;
	DllExport const CvUnitEntry &getGreatUnitEntry(int index) const;
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
protected:
	CvString m_szFormationType;
	std::vector<EntityEventTypes>	m_vctEventTypes;		//!< The list of EntityEventTypes that this formation is intended for
	std::vector<CvUnitEntry> m_vctUnitEntries;
	std::vector<CvUnitEntry> m_vctGreatUnitEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpecialUnitInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialUnitInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvSpecialUnitInfo();
	DllExport virtual ~CvSpecialUnitInfo();
	DllExport bool isValid() const;
	DllExport bool isCityLoad() const;
	// Arrays
	DllExport bool isCarrierUnitAIType(int i) const;
	DllExport int getProductionTraits(int i) const;

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	bool m_bValid;
	bool m_bCityLoad;
	// Arrays
	bool* m_abCarrierUnitAITypes;
	int* m_aiProductionTraits;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCivicInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCivicInfo :
	public CvInfoBase
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvCivicInfo();
	DllExport virtual ~CvCivicInfo();
	DllExport int getCivicOptionType() const;
	DllExport int getAIWeight() const;
	DllExport int getGreatGeneralRateModifier() const;
	DllExport int getDomesticGreatGeneralRateModifier() const;
	DllExport int getFreeExperience() const;
	DllExport int getWorkerSpeedModifier() const;
	DllExport int getImprovementUpgradeRateModifier() const;
	DllExport int getMilitaryProductionModifier() const;
	DllExport int getExpInBorderModifier() const;
	DllExport int getImmigrationConversion() const;
	DllExport int getNativeAttitudeChange() const;
	DllExport int getNativeCombatModifier() const;
	DllExport int getFatherPointModifier() const;
	DllExport bool isDominateNativeBorders() const;
	DllExport bool isRevolutionEuropeTrade() const;

	// Arrays
	DllExport int getYieldModifier(int i) const;
	int* getYieldModifierArray() const;
	DllExport int getCapitalYieldModifier(int i) const;
	int* getCapitalYieldModifierArray() const;
	DllExport int getProfessionCombatChange(int iProfession) const;
	DllExport bool isHurry(int i) const;
	DllExport bool isSpecialBuildingNotRequired(int i) const;
	DllExport int getImprovementYieldChanges(int i, int j) const;

	DllExport int getNumFreeUnitClasses() const;
	DllExport int getFreeUnitClass(int i) const;

	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);
//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iCivicOptionType;
	int m_iAIWeight;
	int m_iGreatGeneralRateModifier;
	int m_iDomesticGreatGeneralRateModifier;
	int m_iFreeExperience;
	int m_iWorkerSpeedModifier;
	int m_iImprovementUpgradeRateModifier;
	int m_iMilitaryProductionModifier;
	int m_iBaseFreeMilitaryUnits;
	int m_iFreeMilitaryUnitsPopulationPercent;
	int m_iGoldPerUnit;
	int m_iGoldPerMilitaryUnit;
	int m_iExpInBorderModifier;
	int m_iImmigrationConversion;
	int m_iNativeAttitudeChange;
	int m_iNativeCombatModifier;
	int m_iFatherPointModifier;
	bool m_bDominateNativeBorders;
	bool m_bRevolutionEuropeTrade;

	// Arrays
	int* m_aiYieldModifier;
	int* m_aiCapitalYieldModifier;
	int* m_aiProfessionCombatChange;
	bool* m_pabHurry;
	bool* m_pabSpecialBuildingNotRequired;
	std::vector<int*> m_aaiImprovementYieldChanges;

	std::vector<int> m_aFreeUnitClasses;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvDiplomacyInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDiplomacyInfo :
	public CvInfoBase
{
	friend class CvXMLLoadUtility;		// so it can access private vars to initialize the class
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvDiplomacyInfo();
	DllExport virtual ~CvDiplomacyInfo();
	DllExport void uninit();

	DllExport const CvDiplomacyResponse& getResponse(int iNum) const;
	DllExport int getNumResponses() const;

	DllExport bool getCivilizationTypes(int i, int j) const;
	DllExport bool getLeaderHeadTypes(int i, int j) const;
	DllExport bool getAttitudeTypes(int i, int j) const;
	DllExport bool getDiplomacyPowerTypes(int i, int j) const;

	DllExport int getNumDiplomacyText(int i) const;

	DllExport const char* getDiplomacyText(int i, int j) const;

	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);
private:
	std::vector<CvDiplomacyResponse*> m_pResponses;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitClassInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitClassInfo :
	public CvInfoBase
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvUnitClassInfo();
	DllExport virtual ~CvUnitClassInfo();
	DllExport int getDefaultUnitIndex() const;
	DllExport void setDefaultUnitIndex(int i);
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport bool readPass3();
//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iDefaultUnitIndex;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBuildingInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoBuilding;
class CvArtInfoMovie;
class CvBuildingInfo : public CvHotkeyInfo
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:

    /** NBMOD EDU **/
    int NBMOD_GetTeachLevel() const;
    /** NBMOD EDU **/
	// TAC - AI Buildings - koma13 - START
	int getAICitySize() const;
	// TAC - AI Buildings - koma13 - END

	DllExport CvBuildingInfo();
	DllExport virtual ~CvBuildingInfo();
	DllExport int getBuildingClassType() const;
	DllExport int getVictoryPrereq() const;
	DllExport int getFreeStartEra() const;
	DllExport int getMaxStartEra() const;
	DllExport int getFreePromotion() const;
	DllExport int getAIWeight() const;
	DllExport int getHurryCostModifier() const;
	DllExport int getAdvancedStartCost() const;
	DllExport int getAdvancedStartCostIncrease() const;
	DllExport int getProfessionOutput() const;
	DllExport int getMaxWorkers() const;
	DllExport int getMinAreaSize() const;
	DllExport int getNumCitiesPrereq() const;
	DllExport int getNumTeamsPrereq() const;
	DllExport int getUnitLevelPrereq() const;
	DllExport int getMinLatitude() const;
	DllExport int getMaxLatitude() const;
	DllExport int getFreeExperience() const;
	DllExport int getFoodKept() const;
	DllExport int getMilitaryProductionModifier() const;
	DllExport int getAssetValue() const;
	DllExport int getPowerValue() const;
	DllExport int getYieldStorage() const;
	DllExport int getSpecialBuildingType() const;
	DllExport int getNextSpecialBuilding() const;
	DllExport int getConquestProbability() const;
	DllExport int getHealRateChange() const;
	DllExport int getDefenseModifier() const;
	DllExport int getBombardDefenseModifier() const;
	DllExport int getMissionType() const;
	DllExport void setMissionType(int iNewType);
	DllExport int getOverflowSellPercent() const;
	DllExport int getSpecialBuildingPriority() const;
	DllExport float getVisibilityPriority() const;
	DllExport bool isWorksWater() const;
	DllExport bool isWater() const;
	DllExport bool isRiver() const;
	DllExport bool isCapital() const;
	DllExport bool isNationalWonder() const; // R&R, ray, National Wonders
	DllExport bool isNeverCapture() const;
	DllExport bool isCenterInCity() const;

	DllExport const char* getConstructSound() const;
	DllExport void setConstructSound(const char* szVal);
	DllExport const char* getArtDefineTag() const;
	DllExport void setArtDefineTag(const char* szVal);
	DllExport const char* getMovieDefineTag() const;
	DllExport void setMovieDefineTag(const char* szVal);
	// Arrays
	DllExport int getYieldChange(int i) const;
	int* getYieldChangeArray() const;
	DllExport int getYieldModifier(int i) const;;
	int* getYieldModifierArray() const;
	DllExport int getLandPlotYieldChange(int i) const; // R&R, ray, Landplot Yields
	int* getLandPlotYieldChangeArray() const; // R&R, ray, Landplot Yields
	DllExport int getSeaPlotYieldChange(int i) const;
	int* getSeaPlotYieldChangeArray() const;
	DllExport int getRiverPlotYieldChange(int i) const;
	int* getRiverPlotYieldChangeArray() const;
	DllExport int getUnitCombatFreeExperience(int i) const;
	DllExport int getDomainFreeExperience(int i) const;
	DllExport int getDomainProductionModifier(int i) const;
	DllExport int getProductionTraits(int i) const;
	DllExport int getPrereqNumOfBuildingClass(int i) const;
	DllExport int getYieldCost(int i) const;

	DllExport bool isBuildingClassNeededInCity(int i) const;

	// Other
	DllExport const CvArtInfoBuilding* getArtInfo() const;
	DllExport const CvArtInfoMovie* getMovieInfo() const;
	DllExport const char* getButton() const;
	DllExport const char* getMovie() const;
	// serialization
	DllExport void read(FDataStreamBase*);
	DllExport void write(FDataStreamBase*);
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport bool readPass2(CvXMLLoadUtility* pXML);
	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
protected:

    /** NBMOD EDU **/
    int m_iTeachLevel;
    /** NBMOD EDU **/
	// TAC - AI Buildings - koma13 - START
	int m_iAICitySize;
	// TAC - AI Buildings - koma13 - END

	int m_iBuildingClassType;
	int m_iVictoryPrereq;
	int m_iFreeStartEra;
	int m_iMaxStartEra;
	int m_iFreePromotion;
	int m_iAIWeight;
	int m_iHurryCostModifier;
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
	int m_iProfessionOutput;
	int m_iMaxWorkers;
	int m_iMinAreaSize;
	int m_iNumCitiesPrereq;
	int m_iNumTeamsPrereq;
	int m_iUnitLevelPrereq;
	int m_iMinLatitude;
	int m_iMaxLatitude;
	int m_iFreeExperience;
	int m_iFoodKept;
	int m_iMilitaryProductionModifier;
	int m_iAssetValue;
	int m_iPowerValue;
	int m_iYieldStorage;
	int m_iSpecialBuildingType;
	int m_iNextSpecialBuilding;
	int m_iConquestProbability;
	int m_iHealRateChange;
	int m_iDefenseModifier;
	int m_iBombardDefenseModifier;
	int m_iMissionType;
	int m_iOverflowSellPercent;
	int m_iSpecialBuildingPriority;
	float m_fVisibilityPriority;
	bool m_bWorksWater;
	bool m_bWater;
	bool m_bRiver;
	bool m_bCapital;
	bool m_bNationalWonder; // R&R, ray, National Wonders
	bool m_bNeverCapture;
	bool m_bCenterInCity;
	CvString m_szConstructSound;
	CvString m_szArtDefineTag;
	CvString m_szMovieDefineTag;
	// Arrays
	int* m_aiProductionTraits;
	int* m_aiLandPlotYieldChange; // R&R, ray, Landplot Yields
	int* m_aiSeaPlotYieldChange;
	int* m_aiRiverPlotYieldChange;
	int* m_aiYieldChange;
	int* m_aiYieldModifier;
	int* m_aiUnitCombatFreeExperience;
	int* m_aiDomainFreeExperience;
	int* m_aiDomainProductionModifier;
	int* m_aiPrereqNumOfBuildingClass;
	int* m_aiYieldCost;
	bool* m_abBuildingClassNeededInCity;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpecialBuildingInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialBuildingInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvSpecialBuildingInfo();
	DllExport virtual ~CvSpecialBuildingInfo();
	DllExport bool isValid() const;
	DllExport int getChar() const;
	DllExport void setChar(int i);
	DllExport int getFontButtonIndex() const;

	// Arrays
	DllExport int getProductionTraits(int i) const;

	DllExport const char* getNatureObject() const;	// TAC - Nature Objects - koma13

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	bool m_bValid;
	int m_iChar;
	int m_iFontButtonIndex;
	// Arrays
	int* m_aiProductionTraits;
	CvString m_szNatureObject;	// TAC - Nature Objects - koma13
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBuildingClassInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildingClassInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvBuildingClassInfo();
	DllExport virtual ~CvBuildingClassInfo();
	DllExport int getDefaultBuildingIndex() const;
	DllExport void setDefaultBuildingIndex(int i);

	// Arrays
	DllExport int getVictoryThreshold(int i) const;

	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport bool readPass3();
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iDefaultBuildingIndex;
	// Arrays
	int* m_aiVictoryThreshold;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRiverModelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRiverModelInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvRiverModelInfo();
	DllExport virtual ~CvRiverModelInfo();
	DllExport const char* getModelFile() const;
	DllExport void setModelFile(const char* szVal);				// The model filename
	DllExport const char* getBorderFile() const;
	DllExport void setBorderFile(const char* szVal);				// The model filename
	DllExport int getTextureIndex() const;
	DllExport const char* getDeltaString() const;
	DllExport const char* getConnectString() const;
	DllExport const char* getRotateString() const;

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	CvString m_szModelFile;					// The model filename
	CvString m_szBorderFile;				// The border filename
	char		m_szDeltaString[32];		// The delta type
	char		m_szConnectString[32];		// The connections this cell makes ( N S E W NE NW SE SW )
	char		m_szRotateString[32];		// The possible rotations for this cell ( 0 90 180 270 )
	int			m_iTextureIndex;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRouteModelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRouteModelInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvRouteModelInfo();
	DllExport virtual ~CvRouteModelInfo();
	DllExport RouteTypes getRouteType() const;				// The route type
	DllExport const char* getModelFile() const;
	DllExport void setModelFile(const char* szVal);				// The model filename
	DllExport const char* getConnectString() const;
	DllExport const char* getModelConnectString() const;
	DllExport const char* getRotateString() const;

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	RouteTypes	m_eRouteType;			// The route type
	CvString m_szModelFile;				// The model filename
	char		m_szConnectString[32];	// The connections this cell makes ( N S E W NE NW SE SW )
	char		m_szModelConnectString[32];	// The connections this model makes ( N S E W NE NW SE SW )
	char		m_szRotateString[32];	// The possible rotations for this cell ( 0 90 180 270 )
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCivilizationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoCivilization;
class CvCivilizationInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvCivilizationInfo();
	DllExport virtual ~CvCivilizationInfo();
	DllExport virtual void reset();
	DllExport int getDefaultPlayerColor() const;
	DllExport int getArtStyleType() const;
	//Androrc UnitArtStyles
	DllExport int getUnitArtStyleType() const;         // Expose to Python
	//Androrc End
	DllExport int getNumCityNames() const;
	// TAC - Great General Names - Ray - START
	DllExport int getNumGeneralNames() const;
	// TAC - Great General Names - Ray - END
	// R&R, ray, Great Admirals - START
	DllExport int getNumAdmiralNames() const;
	// R&R, ray, Great Admirals - END
	// TAC - Ship Names - Ray - START
	DllExport int getNumShipNames() const;
	// TAC - Ship Names - Ray - END
	DllExport int getNumLeaders() const;				 // the number of leaders the Civ has, this is needed so that random leaders can be generated easily
	DllExport int getSelectionSoundScriptId() const;
	DllExport int getActionSoundScriptId() const;
	DllExport int getAdvancedStartPoints() const;
	DllExport int getAreaMultiplier() const;
	DllExport int getDensityMultiplier() const;
	DllExport int getTreasure() const;
	DllExport int getFavoredTerrain() const;
	DllExport int getCapturedCityUnitClass() const;
	DllExport int getDefaultProfession() const;
	DllExport int getMissionaryChar() const;
	DllExport void setMissionaryChar(int iChar);

	DllExport bool isAIPlayable() const;
	DllExport bool isPlayable() const;
	DllExport bool isWaterStart() const;
	DllExport bool isOpenBorders() const;
	DllExport bool isWaterWorks() const;
	DllExport bool isEurope() const;
	DllExport bool isNative() const;

	// R&R, ray, Correct Geographical Placement of Natives - START
	DllExport bool isNorthAmericanNative() const;
	DllExport bool isSouthAmericanNative() const;
	DllExport bool isCentralAmericanNative() const;
	// R&R, ray, Correct Geographical Placement of Natives - END

	std::wstring pyGetShortDescription(uint uiForm) { return getShortDescription(uiForm); }
	DllExport const wchar* getShortDescription(uint uiForm = 0);
	DllExport const wchar* getShortDescriptionKey() const;
	std::wstring pyGetShortDescriptionKey() { return getShortDescriptionKey(); }

	std::wstring pyGetAdjective(uint uiForm) { return getAdjective(uiForm);  }
	DllExport const wchar* getAdjective(uint uiForm = 0);
	DllExport const wchar* getAdjectiveKey() const;
	std::wstring pyGetAdjectiveKey() { return getAdjectiveKey(); }

	DllExport const char* getFlagTexture() const;
	DllExport const char* getArtDefineTag() const;
	DllExport void setArtDefineTag(const char* szVal);
	// Arrays
	DllExport int getCivilizationBuildings(int i) const;
	DllExport int getCivilizationUnits(int i) const;
	DllExport int getCivilizationProfessions(int i) const;
	DllExport int getNumCivilizationFreeUnits() const;
	DllExport int getCivilizationFreeUnitsClass(int index) const;
	DllExport int getCivilizationFreeUnitsProfession(int index) const;
	DllExport int getCivilizationInitialCivics(int i) const;
	DllExport int getFreeYields(int i) const;
	DllExport int getTeachUnitClassWeight(int i) const;

	DllExport bool isLeaders(int i) const;
	DllExport bool isCivilizationFreeBuildingClass(int i) const;
	DllExport bool isValidProfession(int i) const;
	DllExport bool hasTrait(int i) const;

	DllExport std::string getCityNames(int i) const;

	// TAC - Great General Names - Ray - START
	DllExport std::string getGeneralNames(int i) const;
	// TAC - Great General Names - Ray - END

	// R&R, ray, Great Admirals - START
	DllExport std::string getAdmiralNames(int i) const;
	// R&R, ray, Great Admirals - END

	// TAC - Ship Names - Ray - START
	DllExport std::string getShipNames(int i) const;
	// TAC - Ship Names - Ray - END

	DllExport const CvArtInfoCivilization* getArtInfo() const;
	DllExport const char* getButton() const;
	DllExport int getDerivativeCiv() const;
	void setDerivativeCiv(int iCiv);
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport bool readPass2(CvXMLLoadUtility* pXML);
	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iDefaultPlayerColor;
	int m_iArtStyleType;

	//Androrc UnitArtStyles
	int m_iUnitArtStyleType;  // FlavorUnits by Impaler[WrG]
	//Androrc End

	// TAC - Great General Names - Ray - START
	int m_iNumGeneralNames;
	CvString* m_paszGeneralNames;
	// TAC - Great General Names - Ray - END

	// R&R, ray, Great Admirals - START
	int m_iNumAdmiralNames;
	CvString* m_paszAdmiralNames;
	// R&R, ray, Great Admirals - END

	// TAC - Ship Names - Ray - Start
	int m_iNumShipNames;
	CvString* m_paszShipNames;
	// TAC - Ship Names - Ray - END

	int m_iNumCityNames;
	int m_iNumLeaders;				 // the number of leaders the Civ has, this is needed so that random leaders can be generated easily
	int m_iSelectionSoundScriptId;
	int m_iActionSoundScriptId;
	int m_iDerivativeCiv;
	int m_iAdvancedStartPoints;
	int m_iAreaMultiplier;
	int m_iDensityMultiplier;
	int m_iTreasure;
	int m_iFavoredTerrain;
	int m_iCapturedCityUnitClass;
	int m_iDefaultProfession;
	int m_iMissionaryChar;

	bool m_bAIPlayable;
	bool m_bPlayable;
	bool m_bWaterStart;
	bool m_bOpenBorders;
	bool m_bWaterWorks;
	bool m_bEurope;
	bool m_bNative;

	// R&R, ray, Correct Geographical Placement of Natives - START
	bool m_bNorthAmericanNative;
	bool m_bSouthAmericanNative;
	bool m_bCentralAmericanNative;
	// R&R, ray, Correct Geographical Placement of Natives - END

	CvString m_szArtDefineTag;
	CvWString m_szShortDescriptionKey;
	CvWString m_szAdjectiveKey;

	// Arrays
	int* m_aiCivilizationBuildings;
	int* m_aiCivilizationUnits;
	int* m_aiTeachUnitClassWeights;
	std::vector< std::pair<UnitClassTypes, ProfessionTypes> > m_aCivilizationFreeUnits;
	int* m_aiCivilizationInitialCivics;
	int* m_aiFreeYields;
	bool* m_abLeaders;
	bool* m_abCivilizationFreeBuildingClass;
	bool* m_abValidProfessions;
	bool* m_abTraits;
	CvString* m_paszCityNames;

	mutable std::vector<CvWString> m_szCachedShortDescription;
	mutable std::vector<CvWString> m_szCachedAdjective;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvVictoryInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVictoryInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvVictoryInfo();
	DllExport virtual ~CvVictoryInfo();

	// PatchMod: Victorys START
protected:
	int m_iTotalProductionRate;
public:
	DllExport int getTotalProductionRate() const;
	DllExport void setTotalProductionRate(int iValue);
	// PatchMod: Victorys END

	DllExport int getPopulationPercentLead() const;
	DllExport int getLandPercent() const;
	DllExport int getMinLandPercent() const;
	DllExport int getCityCulture() const;
	DllExport int getNumCultureCities() const;
	DllExport int getTotalCultureRatio() const;

	DllExport bool getDefault() const;
	DllExport bool isTargetScore() const;
	DllExport bool isEndEurope() const;
	DllExport bool isEndScore() const;
	DllExport bool isConquest() const;
	DllExport bool isPermanent() const;
	DllExport bool isRevolution() const;

	DllExport const char* getMovie() const;
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iPopulationPercentLead;
	int m_iLandPercent;
	int m_iMinLandPercent;
	int m_iCityCulture;
	int m_iNumCultureCities;
	int m_iTotalCultureRatio;
	bool m_bDefault;
	bool m_bTargetScore;
	bool m_bEndEurope;
	bool m_bEndScore;
	bool m_bConquest;
	bool m_bPermanent;
	bool m_bRevolution;
	CvString m_szMovie;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvHurryInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHurryInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
	public:
		DllExport CvHurryInfo();
		DllExport virtual ~CvHurryInfo();
		DllExport int getGoldPerProduction() const;
		DllExport int getProductionPerPopulation() const;
		DllExport int getGoldPerCross() const;
		DllExport int getYieldCostEuropePercent() const;
		DllExport int getProductionYieldConsumed() const;
		DllExport int getProductionYieldPercent() const;
		DllExport int getFlatGold() const;
		DllExport bool isStarting() const;
		DllExport bool isCity() const;

		DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
	protected:
		int m_iGoldPerProduction;
		int m_iProductionPerPopulation;
		int m_iGoldPerCross;
		int m_iYieldCostEuropePercent;
		int m_iProductionYieldConsumed;
		int m_iProductionYieldPercent;
		int m_iFlatGold;
		bool m_bStarting;
		bool m_bCity;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvHandicapInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHandicapInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:

    /** NBMOD TAX **/

    int NBMOD_GetInitMaxTaxRate() const;
    int NBMOD_GetMaxGoldAskPercent() const;
    int NBMOD_GetMinGoldAskPercent() const;

    /** NBMOD TAX **/

    /** NBMOD REF **/

    int NBMOD_GetREFWeight() const;

    /** NBMOD REF **/

	DllExport CvHandicapInfo();
	DllExport virtual ~CvHandicapInfo();

	// TAC - AI More Immigrants - koma13 - START
protected:
	int m_iAIImmigration;
	int m_iAIOverflowSell;
	int m_iAIMaxTaxrate;
public:
	DllExport int getAIImmigration() const;
	DllExport int getAIOverflowSell() const;
	DllExport int getAIMaxTaxrate() const;
	// TAC - AI More Immigrants - koma13 - END
	
	// TAC: ray Starting Location Start
	DllExport int getStartingLocationPercent() const;
	// TAC: ray Starting Location End

	DllExport int getAdvancedStartPointsMod() const;
	DllExport int getStartingGold() const;
	DllExport int getFatherPercent() const;
	DllExport int getAttitudeChange() const;

	DllExport int getStartingDefenseUnits() const;
	DllExport int getStartingWorkerUnits() const;
	DllExport int getStartingExploreUnits() const;
	DllExport int getAIStartingUnitMultiplier() const;
	DllExport int getAIStartingDefenseUnits() const;
	DllExport int getAIStartingWorkerUnits() const;
	DllExport int getAIStartingExploreUnits() const;
	DllExport int getAIDeclareWarProb() const;
	DllExport int getAIWorkRateModifier() const;
	DllExport int getAINativeCombatModifier() const;
	DllExport int getAIKingCombatModifier() const;
	DllExport int getAIRebelModifier() const;
	DllExport int getAIGrowthPercent() const;
	DllExport int getAITrainPercent() const;
	DllExport int getAIConstructPercent() const;
	DllExport int getAIUnitUpgradePercent() const;
	DllExport int getAIHurryPercent() const;
	DllExport int getAIExtraTradePercent() const;
	DllExport int getAIPerEraModifier() const;
	DllExport int getAIAdvancedStartPercent() const;
	DllExport int getAIKingUnitThresholdPercent() const;
	// < JAnimals Mod Start >
	DllExport int getAIAnimalLandMaxPercent() const;
	DllExport int getAIAnimalSeaMaxPercent() const;
	DllExport int getAIAnimalLandNumTurnsNoSpawn() const;
	DllExport int getAIAnimalSeaNumTurnsNoSpawn() const;
	// < JAnimals Mod End >
	DllExport int getNumGoodies() const;
	DllExport int getEuropePriceThresholdMultiplier() const;
	DllExport int getNativePacifismPercent() const;
	DllExport int getMissionFailureThresholdPercent() const;
	DllExport int getKingNumUnitMultiplier() const;
	DllExport int getKingGoldThresholdPercent() const;

	// TAC - AI Revolution - koma13 - START
	DllExport int getWaveTurns() const;
	DllExport int getNumWaves();
	DllExport int getWaves(int index);
	// TAC - AI Revolution - koma13 - END
	
	// Arrays
	DllExport int getGoodies(int i) const;

	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	// TAC: ray Starting Location Start
	int m_iStartingLocationPercent;
	// TAC: ray Starting Location End

    /** NBMOD TAX **/

    int m_iInitMaxTaxRate;
    int m_iMaxGoldAskPercent;
    int m_iMinGoldAskPercent;

    /** NBMOD TAX **/

    /** NBMOD REF **/
    int m_iNBMOD_REF_Weight;
    /** NBMOD REF **/
	int m_iAdvancedStartPointsMod;
	int m_iStartingGold;
	int m_iFatherPercent;
	int m_iAttitudeChange;
	int m_iStartingDefenseUnits;
	int m_iStartingWorkerUnits;
	int m_iStartingExploreUnits;
	int m_iAIStartingUnitMultiplier;
	int m_iAIStartingDefenseUnits;
	int m_iAIStartingWorkerUnits;
	int m_iAIStartingExploreUnits;
	int m_iAIDeclareWarProb;
	int m_iAIWorkRateModifier;
	int m_iAINativeCombatModifier;
	int m_iAIKingCombatModifier;
	int m_iAIRebelModifier;
	int m_iAIGrowthPercent;
	int m_iAITrainPercent;
	int m_iAIConstructPercent;
	int m_iAIUnitUpgradePercent;
	int m_iAIHurryPercent;
	int m_iAIExtraTradePercent;
	int m_iAIPerEraModifier;
	int m_iAIAdvancedStartPercent;
	int m_iAIKingUnitThresholdPercent;
	// < JAnimals Mod Start >
	int m_iAIAnimalLandMaxPercent;
	int m_iAIAnimalSeaMaxPercent;
	int m_iAIAnimalLandNumTurnsNoSpawn;
	int m_iAIAnimalSeaNumTurnsNoSpawn;
	// < JAnimals Mod End >
	int m_iNumGoodies;
	int m_iEuropePriceThresholdMultiplier;
	int m_iNativePacifismPercent;
	int m_iMissionFailureThresholdPercent;
	int m_iKingNumUnitMultiplier;
	int m_iKingGoldThresholdPercent;
	CvString m_szHandicapName;
	
	// TAC - AI Revolution - koma13 - START
	int m_iWaveTurns;
	std::vector<int> m_aWaves;
	// TAC - AI Revolution - koma13 - END

	// Arrays
	int* m_aiGoodies;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameSpeedInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameSpeedInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvGameSpeedInfo();
	DllExport virtual ~CvGameSpeedInfo();
	DllExport int getGrowthPercent() const;
	DllExport int getStoragePercent() const;
	DllExport int getTrainPercent() const;
	DllExport int getConstructPercent() const;
	DllExport int getFatherPercent() const;
	DllExport int getGreatGeneralPercent() const;
	DllExport int getRevolutionTurns() const;
	DllExport int getNumTurnIncrements() const;

	DllExport GameTurnInfo& getGameTurnInfo(int iIndex) const;
	DllExport void allocateGameTurnInfos(const int iSize);
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iGrowthPercent;
	int m_iStoragePercent;
	int m_iTrainPercent;
	int m_iConstructPercent;
	int m_iFatherPercent;
	int m_iGreatGeneralPercent;
	int m_iRevolutionTurns;
	int m_iNumTurnIncrements;
	CvString m_szGameSpeedName;
	GameTurnInfo* m_pGameTurnInfo;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTurnTimerInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTurnTimerInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvTurnTimerInfo();
	DllExport virtual ~CvTurnTimerInfo();
	DllExport int getBaseTime() const;
	DllExport int getCityBonus() const;
	DllExport int getUnitBonus() const;
	DllExport int getFirstTurnMultiplier() const;

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iBaseTime;
	int m_iCityBonus;
	int m_iUnitBonus;
	int m_iFirstTurnMultiplier;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBuildInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildInfo : public CvHotkeyInfo
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvBuildInfo();
	DllExport virtual ~CvBuildInfo();
	DllExport int getTime() const;
	DllExport int getCost() const;
	DllExport int getImprovement() const;
	DllExport int getPrereqTerrain() const; // R&R, ray, Terraforming Features
	DllExport int getResultTerrain() const; // R&R, ray, Terraforming Features 
	DllExport int getResultFeature() const; // R&R, ray, Terraforming Features
	DllExport int getRoute() const;
	DllExport int getEntityEvent() const;
	DllExport int getMissionType() const;
	DllExport void setMissionType(int iNewType);
	DllExport bool isKill() const;

	// Arrays
	DllExport int getFeatureTime(int i) const;
	DllExport int getFeatureYield(int iFeature, int iYield) const;

	DllExport bool isFeatureRemove(int i) const;

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iTime;
	int m_iCost;
	int m_iImprovement;
	int m_iPrereqTerrain; // R&R, ray, Terraforming Features
	int m_iResultTerrain; // R&R, ray, Terraforming Features
	int m_iResultFeature; // R&R, ray, Terraforming Features
	int m_iRoute;
	int m_iEntityEvent;
	int m_iMissionType;
	bool m_bKill;
	// Arrays
	int* m_paiFeatureTime;
	bool* m_pabFeatureRemove;
	std::vector< std::vector<int> > m_aaiFeatureYield;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGoodyInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGoodyInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvGoodyInfo();
	DllExport virtual ~CvGoodyInfo();
	DllExport int getGold() const;
	DllExport int getGoldRand1() const;
	DllExport int getGoldRand2() const;
	DllExport int getImmigration() const; // R&R, ray, Goody Enhancement
	DllExport int getMapOffset() const;
	DllExport int getMapRange() const;
	DllExport int getMapProb() const;
	DllExport int getExperience() const;
	DllExport int getHealing() const;
	DllExport int getDamagePrereq() const;
	DllExport int getCityGoodyWeight() const;
	DllExport int getUnitClassType() const;
	DllExport int getTeachUnitClassType() const;

	DllExport bool isBad() const;
	DllExport bool isWar() const;
	DllExport bool isWaterGoody() const; // R&R, ray, Goodies on Water
	DllExport bool isUnique() const; // R&R, ray, Goody Enhancement

	DllExport int getGoodyWeight(int iGoody) const;

	DllExport const char* getSound() const;
	DllExport void setSound(const char* szVal);
	DllExport const char* getAnnounceTextKey() const;
	DllExport const char* getChiefTextKey() const;
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport bool readPass2(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iGold;
	int m_iGoldRand1;
	int m_iGoldRand2;
	int m_iImmigration; // R&R, ray, Goody Enhancement
	int m_iMapOffset;
	int m_iMapRange;
	int m_iMapProb;
	int m_iExperience;
	int m_iHealing;
	int m_iDamagePrereq;
	int m_iCityGoodyWeight;
	int m_iUnitClassType;
	int m_iTeachUnitClassType;

	bool m_bBad;
	bool m_bWar;
	bool m_bWaterGoody; // R&R, ray, Goodies on Water
	bool m_bUnique; // R&R, ray, Goody Enhancement

	int* m_aGoodyWeights;

	CvString m_szSound;
	CvString m_szAnnounceTextKey;
	CvString m_szChiefTextKey;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRouteInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRouteInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvRouteInfo();
	DllExport virtual ~CvRouteInfo();
	DllExport int getAdvancedStartCost() const;
	DllExport int getAdvancedStartCostIncrease() const;
	DllExport int getValue() const;
	DllExport int getMovementCost() const;
	DllExport int getFlatMovementCost() const;

	// Arrays
	DllExport int getYieldChange(int i) const;

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
	int m_iValue;
	int m_iMovementCost;
	int m_iFlatMovementCost;
	// Arrays
	int* m_aiYieldChange;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvImprovementBonusInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvImprovementBonusInfo :
	public CvInfoBase
{
friend class CvImprovementInfo;
friend class CvXMLLoadUtility;
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvImprovementBonusInfo();
	DllExport virtual ~CvImprovementBonusInfo();
	DllExport int getDiscoverRand() const;

	DllExport bool isBonusMakesValid() const;

	DllExport int getYieldChange(int i) const;

	// Serialize
	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iDiscoverRand;
	bool m_bBonusMakesValid;
	// Arrays
	int* m_aiYieldChange;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvImprovementInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoImprovement;
class CvImprovementInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvImprovementInfo();
	DllExport virtual ~CvImprovementInfo();
	DllExport int getAdvancedStartCost() const;
	DllExport int getAdvancedStartCostIncrease() const;
	DllExport int getTilesPerGoody() const;
	DllExport int getGoodyUniqueRange() const;
	DllExport int getFeatureGrowthProbability() const;
	DllExport int getUpgradeTime() const;
	DllExport int getDefenseModifier() const;
	DllExport int getPillageGold() const;
	DllExport int getImprovementPillage() const;
	DllExport void setImprovementPillage(int i);
	DllExport int getImprovementUpgrade() const;
	DllExport void setImprovementUpgrade(int i);
	// Super Forts begin *XML*
	DllExport int getCulture() const;
	DllExport int getCultureRange() const;
	DllExport int getVisibilityChange() const;
	DllExport int getSeeFrom() const;
	DllExport int getUniqueRange() const;
	DllExport bool isBombardable() const;
	DllExport bool isUpgradeRequiresFortify() const;
	// Super Forts end
	DllExport bool isActsAsCity() const;
	DllExport bool isFort() const; // R&R, ray, Monasteries and Forts
	DllExport bool isMonastery() const; // R&R, ray, Monasteries and Forts
	DllExport bool isHillsMakesValid() const;
	DllExport bool isRiverSideMakesValid() const;
	DllExport bool isRequiresFlatlands() const;
	DllExport bool isRequiresRiverSide() const;
	DllExport bool isRequiresFeature() const;
	DllExport bool isWater() const;
	DllExport bool isGoody() const;
	DllExport bool isPermanent() const;
	DllExport bool useLSystem() const;
	DllExport bool isOutsideBorders() const;

	DllExport const char* getArtDefineTag() const;
	DllExport void setArtDefineTag(const char* szVal);
	DllExport int getWorldSoundscapeScriptId() const;
	// Arrays
	DllExport int getPrereqNatureYield(int i) const;
	int* getPrereqNatureYieldArray();
	DllExport int getYieldIncrease(int i) const;
	int* getYieldIncreaseArray();
	DllExport int getRiverSideYieldChange(int i) const;
	int* getRiverSideYieldChangeArray();
	DllExport int getHillsYieldChange(int i) const;
	int* getHillsYieldChangeArray();
	DllExport bool getTerrainMakesValid(int i) const;
	DllExport bool getFeatureMakesValid(int i) const;

	DllExport int getRouteYieldChanges(int i, int j) const;
	DllExport int* getRouteYieldChangesArray(int i);				// For Moose - CvWidgetData XXX
	DllExport int getImprovementBonusYield(int i, int j) const;
	DllExport bool isImprovementBonusMakesValid(int i) const;
	DllExport int getImprovementBonusDiscoverRand(int i) const;

	// Other
	DllExport const char* getButton() const;
	DllExport const CvArtInfoImprovement* getArtInfo() const;
	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport bool readPass2(CvXMLLoadUtility* pXML);


	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
	int m_iTilesPerGoody;
	int m_iGoodyUniqueRange;
	int m_iFeatureGrowthProbability;
	int m_iUpgradeTime;
	int m_iDefenseModifier;
	int m_iPillageGold;
	int m_iImprovementPillage;
	int m_iImprovementUpgrade;
	// Super Forts begin *XML*
	int m_iCulture;
	int m_iCultureRange;
	int m_iVisibilityChange;
	int m_iSeeFrom;
	int m_iUniqueRange;
	bool m_bBombardable;
	bool m_bUpgradeRequiresFortify;
	// Super Forts end
	bool m_bActsAsCity;
	bool m_bFort; // R&R, ray, Monasteries and Forts
	bool m_bMonastery; // R&R, ray, Monasteries and Forts
	bool m_bHillsMakesValid;
	bool m_bRiverSideMakesValid;
	bool m_bRequiresFlatlands;
	bool m_bRequiresRiverSide;
	bool m_bRequiresFeature;
	bool m_bWater;
	bool m_bGoody;
	bool m_bPermanent;
	bool m_bUseLSystem;
	bool m_bOutsideBorders;
	CvString m_szArtDefineTag;

	int m_iWorldSoundscapeScriptId;
	// Arrays
	int* m_aiPrereqNatureYield;
	int* m_aiYieldIncrease;
	int* m_aiRiverSideYieldChange;
	int* m_aiHillsYieldChange;
	bool* m_abTerrainMakesValid;
	bool* m_abFeatureMakesValid;
	std::vector<int*> m_aaiRouteYieldChanges;
	CvImprovementBonusInfo* m_paImprovementBonus;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBonusInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoBonus;
class CvBonusInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvBonusInfo();
	DllExport virtual ~CvBonusInfo();
	DllExport int getChar() const;
	DllExport void setChar(int i);
	DllExport int getAIObjective() const;
	DllExport int getMinAreaSize() const;
	DllExport int getMinLatitude() const;
	DllExport int getMaxLatitude() const;
	DllExport int getPlacementOrder() const;
	DllExport int getConstAppearance() const;
	DllExport int getRandAppearance1() const;
	DllExport int getRandAppearance2() const;
	DllExport int getRandAppearance3() const;
	DllExport int getRandAppearance4() const;
	DllExport int getPercentPerPlayer() const;
	DllExport int getTilesPer() const;
	DllExport int getMinLandPercent() const;
	DllExport int getUniqueRange() const;
	DllExport int getGroupRange() const;
	DllExport int getGroupRand() const;

	DllExport bool isOneArea() const;
	// R&R, ray, Bonus on Peaks - START
	DllExport bool isPeaks() const;
	// R&R, ray, Bonus on Peaks - END
	DllExport bool isHills() const;
	//TAC Whaling, ray
	DllExport bool isOcean() const;
	DllExport bool isWhalingboatWorkable() const;
	//End TAC Whaling, ray
	DllExport bool isFishingboatWorkable() const; // R&R, ray, High Sea Fishing
	DllExport bool isFlatlands() const;
	DllExport bool isNoRiverSide() const;
	DllExport bool useLSystem() const;
	DllExport const char* getArtDefineTag() const;
	DllExport void setArtDefineTag(const char* szVal);
	//TAC Whaling, ray
	DllExport int getNumYieldChanges() const;
	std::vector<int> getYieldChangesArray();
	//End TAC Whaling, ray
	// Arrays
	DllExport int getYieldChange(int i) const;
	int* getYieldChangeArray();
	DllExport int getImprovementChange(int i) const;
	DllExport bool isTerrain(int i) const;
	DllExport bool isFeature(int i) const;
	DllExport bool isFeatureTerrain(int i) const;

	// Other
	DllExport const char* getButton() const;
	DllExport const CvArtInfoBonus* getArtInfo() const;
	DllExport int getBuilding() const;

	DllExport void read(FDataStreamBase* stream);
	DllExport void write(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
protected:
	int m_iChar;
	int m_iAIObjective;
	int m_iMinAreaSize;
	int m_iMinLatitude;
	int m_iMaxLatitude;
	int m_iPlacementOrder;
	int m_iConstAppearance;
	int m_iRandAppearance1;
	int m_iRandAppearance2;
	int m_iRandAppearance3;
	int m_iRandAppearance4;
	int m_iPercentPerPlayer;
	int m_iTilesPer;
	int m_iMinLandPercent;
	int m_iUniqueRange;
	int m_iGroupRange;
	int m_iGroupRand;
	int m_iBuilding;
	bool m_bOneArea;
	bool m_bPeaks; // R&R, ray, Bonus on Peaks - START
	bool m_bHills;
	bool m_bOcean; //TAC Whaling, ray
	bool m_bFlatlands;
	bool m_bNoRiverSide;
	bool m_bUseLSystem;
	bool m_bWhalingboatWorkable; //TAC Whaling, ray
	bool m_bFishingboatWorkable; // R&R, ray, High Sea Fishing
	CvString m_szArtDefineTag;
	// Arrays
	int* m_aiYieldChange;
	int* m_aiImprovementChange;
	bool* m_abTerrain;
	bool* m_abFeature;
	bool* m_abFeatureTerrain;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvFeatureInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoFeature;
class CvFeatureInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvFeatureInfo();
	DllExport virtual ~CvFeatureInfo();
	DllExport int getMovementCost() const;
	DllExport int getSeeThroughChange() const;
	DllExport int getAppearanceProbability() const;
	DllExport int getDisappearanceProbability() const;
	DllExport int getGrowthProbability() const;
	DllExport int getDefenseModifier() const;
	DllExport int getAdvancedStartRemoveCost() const;
	// R&R, Robert Surcouf, Damage on Storm plots, Start
	DllExport int getTurnDamage() const;
	DllExport bool isGeneratedEveryRound() const;
	// R&R, Robert Surcouf, Damage on Storm plots, End
	DllExport bool isNoCoast() const;
	DllExport bool isNoRiver() const;
	DllExport bool isNoAdjacent() const;
	DllExport bool isRequiresFlatlands() const;
	DllExport bool isRequiresRiver() const;
	DllExport bool isImpassable() const;
	DllExport bool isNoCity() const;
	DllExport bool isNoImprovement() const;
	DllExport bool isVisibleAlways() const;
	DllExport const char* getOnUnitChangeTo() const;
	DllExport const char* getArtDefineTag() const;
	DllExport void setArtDefineTag(const char* szTag);

	DllExport int getWorldSoundscapeScriptId() const;
	DllExport const char* getEffectType() const;
	DllExport int getEffectProbability() const;
	// Arrays
	DllExport int getYieldChange(int i) const;
	DllExport int getRiverYieldIncrease(int i) const;
	DllExport int get3DAudioScriptFootstepIndex(int i) const;
	DllExport bool isTerrain(int i) const;
	DllExport int getNumVarieties() const;
	// Other
	DllExport const CvArtInfoFeature* getArtInfo() const;
	DllExport const char* getButton() const;
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iMovementCost;
	int m_iSeeThroughChange;
	int m_iAppearanceProbability;
	int m_iDisappearanceProbability;
	int m_iGrowthProbability;
	int m_iDefenseModifier;
	int m_iAdvancedStartRemoveCost;
	// R&R, Robert Surcouf, Damage on Storm plots, Start
	int m_iTurnDamage;
	bool m_bGeneratedEveryRound;
	// R&R, Robert Surcouf, Damage on Storm plots, End
	bool m_bNoCoast;
	bool m_bNoRiver;
	bool m_bNoAdjacent;
	bool m_bRequiresFlatlands;
	bool m_bRequiresRiver;
	bool m_bImpassable;
	bool m_bNoCity;
	bool m_bNoImprovement;
	bool m_bVisibleAlways;
	CvString m_szOnUnitChangeTo;
	int m_iWorldSoundscapeScriptId;
	CvString m_szEffectType;
	int m_iEffectProbability;
	// Arrays
	int* m_aiYieldChange;
	int* m_aiRiverYieldIncrease;
	int* m_ai3DAudioScriptFootstepIndex;
	bool* m_abTerrain;
private:
	CvString m_szArtDefineTag;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvYieldInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvYieldInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvYieldInfo();
	DllExport virtual ~CvYieldInfo();
	DllExport int getChar() const;
	DllExport void setChar(int i);
	DllExport const char* getIcon() const;
// KJ Jansson addon for Multiple Professions per Building modcomp by Androrc the Orc START
	DllExport const char* getCombiIcon() const;
// KJ Jansson addon for Multiple Professions per Building modcomp by Androrc the Orc END
	DllExport const char* getHighlightIcon() const;
	DllExport int getBuyPriceLow() const;
	DllExport int getBuyPriceHigh() const;
	// TAC - Price Limits - Ray - START
	DllExport int getMinimumBuyPrice() const;
	// TAC - Price Limits - Ray - END
	DllExport int getSellPriceDifference() const;
	DllExport int getPriceChangeThreshold() const;
	DllExport int getPriceCorrectionPercent() const;
	// R&R, Androrc Price Recovery
	DllExport int getEuropeVolumeAttrition() const;
	//Androrc End
	// R&R, ray, Africa
	DllExport int getAfricaBuyPriceHigh() const;
	DllExport int getAfricaBuyPriceLow() const;
	DllExport int getAfricaSellPriceDifference() const;
	// R&R, ray, Africa - END
	// R&R, ray, Port Royal
	DllExport int getPortRoyalBuyPriceHigh() const;
	DllExport int getPortRoyalBuyPriceLow() const;
	DllExport int getPortRoyalSellPriceDifference() const;
	// R&R, ray, Port Royal - END
	DllExport int getNativeBuyPrice() const;
	DllExport int getNativeSellPrice() const;
	DllExport int getNativeConsumptionPercent() const;
	DllExport int getNativeHappy() const;
	DllExport int getHillsChange() const;
	DllExport int getPeakChange() const;
	DllExport int getLakeChange() const;
	DllExport int getCityChange() const;
	DllExport int getMinCity() const;
	DllExport int getAIWeightPercent() const;
	DllExport int getAIBaseValue() const;
	DllExport int getNativeBaseValue() const;
	DllExport int getColorType() const;
	DllExport int getUnitClass() const;
	DllExport int getTextureIndex() const;
	DllExport int getWaterTextureIndex() const;
	DllExport int getPowerValue() const;
	DllExport int getAssetValue() const;

	DllExport bool isCargo() const;

	bool isExportYield() const {return m_bIsExportYield;} // auto traderoute - Nightinggale

	// R&R, Androrc, Livestock Breeding
	DllExport bool isLivestock() const;
	// R&R, Androrc, Livestock Breeding, END

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iChar;
	CvString m_szIcon;
// KJ Jansson addon for Multiple Professions per Building modcomp by Androrc the Orc START
	CvString m_szCombiIcon;
// KJ Jansson addon for Multiple Professions per Building modcomp by Androrc the Orc END
	CvString m_szHightlightIcon;
	int m_iBuyPriceLow;
	int m_iBuyPriceHigh;
	// TAC - Price Limits - Ray - START
	int m_iBuyPriceMin;
	// TAC - Price Limits - Ray - END
	int m_iSellPriceDifference;
	int m_iPriceChangeThreshold;
	int m_iPriceCorrectionPercent;
	// R&R, Androrc Price Recovery
	int m_iEuropeVolumeAttrition;
	//Androrc End
	// R&R, ray, Africa
	int m_iAfricaBuyPriceLow;
	int m_iAfricaBuyPriceHigh;
	int m_iAfricaSellPriceDifference;
	// R&R, ray, Africa - END
	// R&R, ray, Port Royal
	int m_iPortRoyalBuyPriceLow;
	int m_iPortRoyalBuyPriceHigh;
	int m_iPortRoyalSellPriceDifference;
	// R&R, ray, Port Royal - END
	int m_iNativeBuyPrice;
	int m_iNativeSellPrice;
	int m_iNativeConsumptionPercent;
	int m_iNativeHappy;
	int m_iHillsChange;
	int m_iPeakChange;
	int m_iLakeChange;
	int m_iCityChange;
	int m_iMinCity;
	int m_iAIWeightPercent;
	int m_iAIBaseValue;
	int m_iNativeBaseValue;
	int m_iColorType;
	int m_iUnitClass;
	int m_iTextureIndex;
	int m_iWaterTextureIndex;
	int m_iPowerValue;
	int m_iAssetValue;

	bool m_bCargo;
	bool m_bIsExportYield; // auto traderoute - Nightinggale

	// R&R, Androrc, Livestock Breeding
	bool m_bLivestock;
	// R&R, Androrc, Livestock Breeding, END
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTerrainInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoTerrain;
class CvTerrainInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvTerrainInfo();
	DllExport virtual ~CvTerrainInfo();
	DllExport int getMovementCost() const;
	DllExport int getSeeFromLevel() const;
	DllExport int getSeeThroughLevel() const;
	DllExport int getBuildModifier() const;
	DllExport int getDefenseModifier() const;

	DllExport bool isWater() const;
	DllExport bool isImpassable() const;
	DllExport bool isFound() const;
	DllExport bool isFoundCoast() const;

	DllExport const char* getArtDefineTag() const;
	DllExport void setArtDefineTag(const char* szTag);

	DllExport int getWorldSoundscapeScriptId() const;
	// Arrays
	DllExport int getYield(int i) const;
	DllExport int getRiverYieldIncrease(int i) const;
	DllExport int get3DAudioScriptFootstepIndex(int i) const;
	// Other
	DllExport const CvArtInfoTerrain* getArtInfo() const;
	DllExport const char* getButton() const;
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iMovementCost;
	int m_iSeeFromLevel;
	int m_iSeeThroughLevel;
	int m_iBuildModifier;
	int m_iDefenseModifier;

	bool m_bWater;
	bool m_bImpassable;
	bool m_bFound;
	bool m_bFoundCoast;
	int m_iWorldSoundscapeScriptId;
	// Arrays
	int* m_aiYields;
	int* m_aiRiverYieldIncrease;
	int* m_ai3DAudioScriptFootstepIndex;
private:
	CvString m_szArtDefineTag;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInterfaceModeInfo (ADD to Python)
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInterfaceModeInfo :
	public CvHotkeyInfo
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvInterfaceModeInfo();
	DllExport virtual ~CvInterfaceModeInfo();
	DllExport int getCursorIndex() const;
	DllExport int getMissionType() const;
	DllExport bool getVisible() const;
	DllExport bool getGotoPlot() const;
	DllExport bool getHighlightPlot() const;
	DllExport bool getSelectType() const;
	DllExport bool getSelectAll() const;
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iCursorIndex;
	int m_iMissionType;
	bool m_bVisible;
	bool m_bGotoPlot;
	bool m_bHighlightPlot;
	bool m_bSelectType;
	bool m_bSelectAll;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvLeaderHeadInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoLeaderhead;
class CvLeaderHeadInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvLeaderHeadInfo();
	DllExport virtual ~CvLeaderHeadInfo();
	DllExport int getAlarmType() const;
	DllExport int getBaseAttitude() const;
	DllExport int getNativeAttitude() const;
	//TAC Native Mercs
	DllExport int getMinAttitudeGiveMercs() const;
	DllExport int getBaseChanceGiveMercs() const;
	//END TAC Native Mercs
	// R&R, ray, Native Slave, START
	DllExport int getMinAttitudeGiveNativeSlaves() const;
	DllExport int getBaseChanceGiveNativeSlaves() const;
	// R&R, ray, Native Slave, END
	DllExport int getRefuseToTalkWarThreshold() const;
	DllExport int getMaxGoldTradePercent() const;
	DllExport int getMaxWarRand() const;
	DllExport int getMaxWarNearbyPowerRatio() const;
	DllExport int getMaxWarDistantPowerRatio() const;
	DllExport int getMaxWarMinAdjacentLandPercent() const;
	DllExport int getLimitedWarRand() const;
	DllExport int getLimitedWarPowerRatio() const;
	DllExport int getDogpileWarRand() const;
	DllExport int getMakePeaceRand() const;
	DllExport int getDeclareWarTradeRand() const;
	DllExport int getDemandRebukedSneakProb() const;
	DllExport int getDemandRebukedWarProb() const;
	DllExport int getRazeCityProb() const;
	DllExport int getBaseAttackOddsChange() const;
	DllExport int getAttackOddsChangeRand() const;
	DllExport int getCloseBordersAttitudeChange() const;
	DllExport int getAlarmAttitudeChange() const;
	DllExport int getLostWarAttitudeChange() const;
	DllExport int getRebelAttitudeDivisor() const;
	DllExport int getAtWarAttitudeDivisor() const;
	DllExport int getAtWarAttitudeChangeLimit() const;
	DllExport int getAtPeaceAttitudeDivisor() const;
	DllExport int getAtPeaceAttitudeChangeLimit() const;
	DllExport int getOpenBordersAttitudeDivisor() const;
	DllExport int getOpenBordersAttitudeChangeLimit() const;
	DllExport int getDefensivePactAttitudeDivisor() const;
	DllExport int getDefensivePactAttitudeChangeLimit() const;
	DllExport int getShareWarAttitudeChange() const;
	DllExport int getShareWarAttitudeDivisor() const;
	DllExport int getShareWarAttitudeChangeLimit() const;
	DllExport int getDemandTributeAttitudeThreshold() const;
	DllExport int getNoGiveHelpAttitudeThreshold() const;
	DllExport int getMapRefuseAttitudeThreshold() const;
	DllExport int getDeclareWarRefuseAttitudeThreshold() const;
	DllExport int getDeclareWarThemRefuseAttitudeThreshold() const;
	DllExport int getStopTradingRefuseAttitudeThreshold() const;
	DllExport int getStopTradingThemRefuseAttitudeThreshold() const;
	DllExport int getOpenBordersRefuseAttitudeThreshold() const;
	DllExport int getDefensivePactRefuseAttitudeThreshold() const;
	DllExport int getPermanentAllianceRefuseAttitudeThreshold() const;
	// R&R, ray, Natives raiding party - START
	DllExport int getNativeRaidAttitudeThreshold() const;
	// R&R, ray, Natives raiding party - END

	DllExport const char* getArtDefineTag() const;
	DllExport void setArtDefineTag(const char* szVal);
	// Arrays
	DllExport bool hasTrait(int i) const;

	DllExport int getContactRand(int i) const;
	DllExport int getContactDelay(int i) const;
	DllExport int getMemoryDecayRand(int i) const;
	DllExport int getMemoryAttitudePercent(int i) const;
	DllExport int getNoWarAttitudeProb(int i) const;
	DllExport int getUnitAIWeightModifier(int i) const;
	DllExport int getImprovementWeightModifier(int i) const;
	DllExport int getDiploPeaceMusicScriptIds(int i) const;
	DllExport int getDiploWarMusicScriptIds(int i) const;
	// Other
	DllExport const CvArtInfoLeaderhead* getArtInfo() const;
	DllExport const char* getLeaderHead() const;
	DllExport const char* getButton() const;
	DllExport void write(FDataStreamBase* stream);
	DllExport void read(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iAlarmType;
	int m_iBaseAttitude;
	int m_iNativeAttitude;
	//TAC Native Mercs
	int m_iMinAttitudeGiveMercs;
	int m_iBaseChanceGiveMercs;
	//End TAC Native Mercs
	// R&R, ray, Native Slave, START
	int m_iMinAttitudeGiveNativeSlaves;
	int m_iBaseChanceGiveNativeSlaves;
	// R&R, ray, Native Slave, END
	int m_iRefuseToTalkWarThreshold;
	int m_iMaxGoldTradePercent;
	int m_iMaxWarRand;
	int m_iMaxWarNearbyPowerRatio;
	int m_iMaxWarDistantPowerRatio;
	int m_iMaxWarMinAdjacentLandPercent;
	int m_iLimitedWarRand;
	int m_iLimitedWarPowerRatio;
	int m_iDogpileWarRand;
	int m_iMakePeaceRand;
	int m_iDeclareWarTradeRand;
	int m_iDemandRebukedSneakProb;
	int m_iDemandRebukedWarProb;
	int m_iRazeCityProb;
	int m_iBaseAttackOddsChange;
	int m_iAttackOddsChangeRand;
	int m_iCloseBordersAttitudeChange;
	int m_iAlarmAttitudeChange;
	int m_iLostWarAttitudeChange;
	int m_iRebelAttitudeDivisor;
	int m_iAtWarAttitudeDivisor;
	int m_iAtWarAttitudeChangeLimit;
	int m_iAtPeaceAttitudeDivisor;
	int m_iAtPeaceAttitudeChangeLimit;
	int m_iOpenBordersAttitudeDivisor;
	int m_iOpenBordersAttitudeChangeLimit;
	int m_iDefensivePactAttitudeDivisor;
	int m_iDefensivePactAttitudeChangeLimit;
	int m_iShareWarAttitudeChange;
	int m_iShareWarAttitudeDivisor;
	int m_iShareWarAttitudeChangeLimit;
	int m_iDemandTributeAttitudeThreshold;
	int m_iNoGiveHelpAttitudeThreshold;
	int m_iMapRefuseAttitudeThreshold;
	int m_iDeclareWarRefuseAttitudeThreshold;
	int m_iDeclareWarThemRefuseAttitudeThreshold;
	int m_iStopTradingRefuseAttitudeThreshold;
	int m_iStopTradingThemRefuseAttitudeThreshold;
	int m_iOpenBordersRefuseAttitudeThreshold;
	int m_iDefensivePactRefuseAttitudeThreshold;
	int m_iPermanentAllianceRefuseAttitudeThreshold;
	// R&R, ray, Natives raiding party - START
	int m_iNativeRaidAttitudeThreshold;
	// R&R, ray, Natives raiding party - END
	CvString m_szArtDefineTag;
	// Arrays
	bool* m_abTraits;
	int* m_aiContactRand;
	int* m_aiContactDelay;
	int* m_aiMemoryDecayRand;
	int* m_aiMemoryAttitudePercent;
	int* m_aiNoWarAttitudeProb;
	int* m_aiUnitAIWeightModifier;
	int* m_aiImprovementWeightModifier;
	int* m_aiDiploPeaceMusicScriptIds;
	int* m_aiDiploWarMusicScriptIds;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvWorldInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWorldInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvWorldInfo();
	DllExport virtual ~CvWorldInfo();
	DllExport int getDefaultPlayers() const;
	DllExport int getDefaultNativePlayers() const;
	DllExport int getUnitNameModifier() const;
	DllExport int getTargetNumCities() const;
	DllExport int getBuildingClassPrereqModifier() const;
	DllExport int getGridWidth() const;
	DllExport int getGridHeight() const;
	DllExport int getTerrainGrainChange() const;
	DllExport int getFeatureGrainChange() const;
	DllExport int getFatherPercent() const;
	DllExport int getAdvancedStartPointsMod() const;
	DllExport int getAIImmigrationModifier() const;	// TAC - AI More Immigrants - koma13

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iDefaultPlayers;
	int m_iDefaultNativePlayers;
	int m_iUnitNameModifier;
	int m_iTargetNumCities;
	int m_iBuildingClassPrereqModifier;
	int m_iGridWidth;
	int m_iGridHeight;
	int m_iTerrainGrainChange;
	int m_iFeatureGrainChange;
	int m_iFatherPercent;
	int m_iAdvancedStartPointsMod;
	int m_iAIImmigrationModifier;	// TAC - AI More Immigrants - koma13
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvClimateInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvClimateInfo :	public CvInfoBase
{
public:
	DllExport CvClimateInfo();
	DllExport virtual ~CvClimateInfo();
	DllExport int getDesertPercentChange() const;
	DllExport int getJungleLatitude() const;
	DllExport int getHillRange() const;
	DllExport int getPeakPercent() const;

	DllExport float getSnowLatitudeChange() const;
	DllExport float getTundraLatitudeChange() const;
	DllExport float getGrassLatitudeChange() const;
	DllExport float getDesertBottomLatitudeChange() const;
	DllExport float getDesertTopLatitudeChange() const;
	DllExport float getRandIceLatitude() const;

	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	int m_iDesertPercentChange;
	int m_iJungleLatitude;
	int m_iHillRange;
	int m_iPeakPercent;
	float m_fSnowLatitudeChange;
	float m_fTundraLatitudeChange;
	float m_fGrassLatitudeChange;
	float m_fDesertBottomLatitudeChange;
	float m_fDesertTopLatitudeChange;
	float m_fRandIceLatitude;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSeaLevelInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSeaLevelInfo :	public CvInfoBase
{
public:
	DllExport CvSeaLevelInfo();
	DllExport virtual ~CvSeaLevelInfo();
	DllExport int getSeaLevelChange() const;

	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	int m_iSeaLevelChange;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvEuropeInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEuropeInfo : public CvInfoBase
{
public:
	DllExport CvEuropeInfo();
	DllExport virtual ~CvEuropeInfo();

	DllExport bool isStart() const;
	DllExport int getCardinalDirection() const;
	DllExport int getTripLength() const;
	DllExport int getMinLandDistance() const;
	DllExport int getWidthPercent() const;

	DllExport bool read(CvXMLLoadUtility* pXML);

protected:
	bool m_bStart;
	int m_iCardinalDirection;
	int m_iTripLength;
	int m_iMinLandDistance;
	int m_iWidthPercent;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTraitInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTraitInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvTraitInfo();
	DllExport virtual ~CvTraitInfo();

	inline CivEffectTypes getCivEffect() const { return m_eCivEffect; }

	DllExport int getLevelExperienceModifier() const;
	DllExport int getGreatGeneralRateModifier() const;
	DllExport int getDomesticGreatGeneralRateModifier() const;
	DllExport int getNativeAngerModifier() const;
	DllExport int getLearnTimeModifier() const;
	DllExport int getNativeCombatModifier() const;
	DllExport int getMissionaryModifier() const;
	DllExport int getNativeTradeModifier() const; // R&R, ray, new Attribute in Traits
	DllExport int getRebelCombatModifier() const;
	DllExport int getTaxRateThresholdModifier() const;
	DllExport int getMaxTaxRateThresholdDecrease() const; // R&R, ray, new Attribute in Traits
	DllExport int getMercantileFactor() const;
	DllExport int getTreasureModifier() const;
	DllExport int getChiefGoldModifier() const;
	DllExport int getNativeAttitudeChange() const;
	DllExport int getEuropeanAttitudeChange() const; // R&R, ray, new Attribute in Traits 
	DllExport int getKingAttitudeChange() const; // R&R, ray, new Attribute in Traits 
	DllExport int getCityDefense() const;
	DllExport int getLandPriceDiscount() const;
	DllExport int getRecruitPriceDiscount() const;
	DllExport int getEuropeTravelTimeModifier() const;
	DllExport int getImmigrationThresholdModifier() const;
	DllExport int getPopGrowthThresholdModifier() const;		// Schmiddie, 7 new variables for traits for Europeans, START
	DllExport int getCultureLevelModifier() const;
	DllExport int getPioneerSpeedModifier() const;
	DllExport int getImprovementPriceModifier() const;
	DllExport int getLearningByDoingModifier() const;
	DllExport int getSpecialistPriceModifier() const;
	DllExport int getStorageCapacityModifier() const;		// Schmiddie, 7 new variables for traits for Europeans, END

	DllExport const char* getShortDescription() const;
	void setShortDescription(const char* szVal);

	// Arrays
	DllExport int getCityExtraYield(int i) const;
	DllExport int getExtraYieldThreshold(int i) const;
	DllExport int getProfessionEquipmentModifier(int i) const;
	DllExport int isFreePromotion(int i) const;
	DllExport int isFreePromotionUnitCombat(int i) const;
	DllExport int getYieldModifier(int iYield) const;
	DllExport int getGoodyFactor(int iGoody) const;
	DllExport int getBuildingProductionModifier(int iBuildingClass) const;
	DllExport int getBuildingRequiredYieldModifier(int iYield) const;
	DllExport const int* getBuildingRequiredYieldModifierArray() const;
	DllExport int getUnitMoveChange(int iUnitClass) const;
	DllExport int getUnitStrengthModifier(int iUnitClass) const;
	DllExport int getProfessionMoveChange(int iProfession) const;
	DllExport bool isTaxYieldModifier(int i) const;
	DllExport bool isFreeBuildingClass(int i) const;
	DllExport int getBuildingYieldChange(int iBuildingClass, int iYieldType) const;

	DllExport void read(FDataStreamBase* );
	DllExport void write(FDataStreamBase* );
	DllExport bool read(CvXMLLoadUtility* pXML);

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	CivEffectTypes m_eCivEffect;

	int m_iLevelExperienceModifier;
	int m_iGreatGeneralRateModifier;
	int m_iDomesticGreatGeneralRateModifier;
	int m_iNativeAngerModifier;
	int m_iLearnTimeModifier;
	int m_iNativeCombatModifier;
	int m_iMissionaryModifier;
	int m_iNativeTradeModifier; // R&R, ray, new Attribute in Traits
	int m_iRebelCombatModifier;
	int m_iTaxRateThresholdModifier;
	int m_iMaxTaxRateThresholdDecrease; // R&R, ray, new Attribute in Traits
	int m_iMercantileFactor;
	int m_iTreasureModifier;
	int m_iChiefGoldModifier;
	int m_iNativeAttitudeChange;
	int m_iEuropeanAttitudeChange; // R&R, ray, new Attribute in Traits
	int m_iKingAttitudeChange; // R&R, ray, new Attribute in Traits
	int m_iCityDefense;
	int m_iLandPriceDiscount;
	int m_iRecruitPriceDiscount;
	int m_iEuropeTravelTimeModifier;
	int m_iImmigrationThresholdModifier;
	int m_iPopGrowthThresholdModifier;		// Schmiddie, 7 new variables for traits for Europeans, START
	int m_iCultureLevelModifier;
	int m_iPioneerSpeedModifier;
	int m_iImprovementPriceModifier;
	int m_iLearningByDoingModifier;
	int m_iSpecialistPriceModifier;
	int m_iStorageCapacityModifier;		// Schmiddie, 7 new variables for traits for Europeans, END

	CvString m_szShortDescription;
	// Arrays
	int* m_aiCityExtraYields;
	int* m_aiExtraYieldThreshold;
	int* m_aiProfessionEquipmentModifier;
	int* m_aiYieldModifier;
	int* m_aiGoodyFactor;
	int* m_aiBuildingProductionModifier;
	int* m_aiBuildingRequiredYieldModifier;
	int* m_aiUnitMoveChange;
	int* m_aiUnitStrengthModifier;
	int* m_aiProfessionMoveChange;
	bool* m_abTaxYieldModifier;
	bool* m_abFreePromotion;
	bool* m_abFreePromotionUnitCombat;
	bool* m_abFreeBuildingClass;

	std::vector<int*> m_aaiBuildingYieldChanges;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCursorInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCursorInfo : public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvCursorInfo();
	DllExport virtual ~CvCursorInfo();
	DllExport const char* getPath();
	DllExport void setPath(const char* szVal);

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	CvString m_szPath;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSlideShowInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSlideShowInfo : public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvSlideShowInfo();
	DllExport virtual ~CvSlideShowInfo();
	DllExport const char* getPath();
	DllExport void setPath(const char* szVal);
	DllExport const char* getTransitionType();
	DllExport void setTransitionType(const char* szVal);
	DllExport float getStartTime();
	DllExport void setStartTime(float fVal);

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	float m_fStartTime;
	CvString m_szPath;
	CvString m_szTransitionType;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSlideShowRandomInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSlideShowRandomInfo : public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvSlideShowRandomInfo();
	DllExport virtual ~CvSlideShowRandomInfo();
	DllExport const char* getPath();
	DllExport void setPath(const char* szVal);
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	CvString m_szPath;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvWorldPickerInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWorldPickerInfo : public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvWorldPickerInfo();
	DllExport virtual ~CvWorldPickerInfo();
	DllExport const char* getMapName();
	DllExport void setMapName(const char* szVal);
	DllExport const char* getModelFile();
	DllExport void setModelFile(const char* szVal);
	DllExport int getNumSizes();
	DllExport float getSize(int index);
	DllExport int getNumClimates();
	DllExport const char* getClimatePath(int index);
	DllExport int getNumWaterLevelDecals();
	DllExport const char* getWaterLevelDecalPath(int index);
	DllExport int getNumWaterLevelGloss();
	DllExport const char* getWaterLevelGlossPath(int index);
	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	CvString m_szMapName;
	CvString m_szModelFile;
	std::vector<float> m_aSizes;
	std::vector<CvString> m_aClimates;
	std::vector<CvString> m_aWaterLevelDecals;
	std::vector<CvString> m_aWaterLevelGloss;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAnimationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef std::vector<std::pair<int,float> > CvAnimationPathDefinition;
typedef std::pair<int,int >			CvAnimationCategoryDefinition;
class CvAnimationPathInfo : public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
	public:
		DllExport CvAnimationPathInfo();
		DllExport virtual ~CvAnimationPathInfo();
		DllExport int getPathCategory( int i );
		DllExport float getPathParameter( int i );
		DllExport int getNumPathDefinitions();
		DllExport CvAnimationPathDefinition * getPath( );
		DllExport bool isMissionPath() const;
		DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
	private:
		CvAnimationPathDefinition 	m_vctPathDefinition;	//!< Animation path definitions, pair(category,param).
		bool						m_bMissionPath;			//!< True if this animation is used in missions
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAnimationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAnimationCategoryInfo : public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
	public:
		DllExport CvAnimationCategoryInfo();
		DllExport virtual ~CvAnimationCategoryInfo();
		DllExport int getCategoryBaseID( );
		DllExport int getCategoryDefaultTo( );
		DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
	private:
		CvAnimationCategoryDefinition	m_kCategory;		//!< The pair(base IDs, default categories) defining the animation categories
		CvString						m_szDefaultTo;		//!< Holds the default to parameter, until all categories are read
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEntityEventInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEntityEventInfo : public CvInfoBase
{
		//---------------------------------------PUBLIC INTERFACE---------------------------------
	public:
		DllExport CvEntityEventInfo();
		DllExport virtual ~CvEntityEventInfo();
		DllExport bool read(CvXMLLoadUtility* pXML);
		DllExport AnimationPathTypes getAnimationPathType(int iIndex = 0) const;
		DllExport EffectTypes getEffectType(int iIndex = 0) const;
		DllExport int getAnimationPathCount() const;
		DllExport int getEffectTypeCount() const;
		DllExport bool getUpdateFormation() const;
		//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
	private:
		std::vector<AnimationPathTypes>	m_vctAnimationPathType;
		std::vector<EffectTypes>		m_vctEffectTypes;
		bool							m_bUpdateFormation;
};

// The below classes are for the ArtFile Management
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  classes : CvArtInfos
//
// This is also an abstract BASE class
//
//  DESC:  Used to store data from Art\Civ4ArtDefines.xml
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAssetInfoBase : public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvAssetInfoBase()  {}
	DllExport virtual ~CvAssetInfoBase() {}
	DllExport const char* getTag() const;
	DllExport void setTag(const char* szDesc);

	DllExport const char* getPath() const;
	DllExport void setPath(const char* szDesc);

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	CvString m_szPath;
};

class CvArtInfoAsset : 	public CvAssetInfoBase
{
public:
	DllExport CvArtInfoAsset() {}
	DllExport virtual ~CvArtInfoAsset() {}
	DllExport const char* getNIF() const;
	DllExport const char* getKFM() const;

	DllExport void setNIF(const char* szDesc);
	DllExport void setKFM(const char* szDesc);

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	CvString m_szKFM;
	CvString m_szNIF;
};

//
//////////////////////////////////////////////////////////////////////////
// Another base class
//////////////////////////////////////////////////////////////////////////
class CvArtInfoScalableAsset :
	public CvArtInfoAsset,
	public CvScalableInfo
{
public:
	DllExport bool read(CvXMLLoadUtility* pXML);
};

// todoJS: Remove empty classes if additional items are not added
class CvArtInfoInterface : 	public CvArtInfoAsset
{
public:
	DllExport CvArtInfoInterface() {}
	DllExport virtual ~CvArtInfoInterface() {}
};

class CvArtInfoMisc : 	public CvArtInfoScalableAsset
{
public:
	DllExport CvArtInfoMisc() {}
	DllExport virtual ~CvArtInfoMisc() {}
};

class CvArtInfoMovie : 	public CvArtInfoAsset
{
public:
	DllExport CvArtInfoMovie() {}
	DllExport virtual ~CvArtInfoMovie() {}
};

class CvArtInfoUnit : public CvArtInfoScalableAsset
{
public:
	DllExport CvArtInfoUnit();
	DllExport virtual ~CvArtInfoUnit();
	DllExport const char* getFullLengthIcon() const;
	DllExport bool getActAsRanged() const;
	DllExport int getDamageStates() const;
	DllExport const char* getTrailTexture() const;
	DllExport float getTrailWidth() const;
	DllExport float getTrailLength() const;
	DllExport float getTrailTaper() const;
	DllExport float getTrailFadeStarTime() const;
	DllExport float getTrailFadeFalloff() const;
	DllExport float getBattleDistance() const;
	DllExport float getRangedDeathTime() const;
	DllExport float getExchangeAngle() const;
	DllExport bool getCombatExempt() const;
	DllExport bool getSmoothMove() const;
	DllExport float getAngleInterpRate() const;
	DllExport float getBankRate() const;
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport const char* getTrainSound() const;
	DllExport void setTrainSound(const char* szVal);
	DllExport int getRunLoopSoundTag() const;
	DllExport int getRunEndSoundTag() const;
	DllExport int getSelectionSoundScriptId() const;
	DllExport int getActionSoundScriptId() const;
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	CvString m_szFullLengthIcon;//!< The full body Icon for city view and drag and drop
	int m_iDamageStates;		//!< The maximum number of damage states this unit type supports
	bool m_bActAsRanged;		//!< true if the unit acts as a ranged unit in combat (but may or may not be actually a ranged unit)
	bool m_bCombatExempt;		//!< true if the unit is 'exempt' from combat - ie. it just flees instead of dying
	bool m_bSmoothMove;			//!< true if the unit should do non-linear interpolation for moves
	CvString m_szTrailTexture;	//!< The trail texture of the unit
	float m_fTrailWidth;		//!< The width of the trail
	float m_fTrailLength;		//!< The length of the trail
	float m_fTrailTaper;		//!< Tapering of the trail
	float m_fTrailFadeStartTime;//!< Time after which the trail starts to fade
	float m_fTrailFadeFalloff;	//!< Speed at which the fade happens
	float m_fBattleDistance;	//!< The preferred attack distance of this unit (1.0 == plot size)
	float m_fRangedDeathTime;	//!< The offset from firing in which an opponent should die
	float m_fExchangeAngle;		//!< The angle at which the unit does combat.
	float m_fAngleInterRate;	//!< The rate at which the units' angle interpolates
	float m_fBankRate;
	CvString m_szTrainSound;
	int m_iRunLoopSoundTag;
	int m_iRunEndSoundTag;
	int m_iSelectionSoundScriptId;
	int m_iActionSoundScriptId;
};

class CvArtInfoBuilding : public CvArtInfoScalableAsset
{
public:
	DllExport CvArtInfoBuilding();
	DllExport virtual ~CvArtInfoBuilding();
	DllExport bool isAnimated() const;
	DllExport const char* getCityTexture() const;
	DllExport const char* getCitySelectedTexture() const;
	DllExport const char* getLSystemName() const;
	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	bool m_bAnimated;
	CvString m_cityTexture;
	CvString m_citySelectedTexture;
	CvString m_szLSystemName;
};

class CvArtInfoCivilization : public CvArtInfoAsset
{
public:
	DllExport CvArtInfoCivilization();
	DllExport virtual ~CvArtInfoCivilization();
	DllExport int getFontButtonIndex() const;
	DllExport bool isWhiteFlag() const;
	DllExport bool isInvertFlag() const;

	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	int m_iFontButtonIndex;
	bool m_bWhiteFlag;
	bool m_bInvertFlag;
};

class CvArtInfoLeaderhead : public CvArtInfoAsset
{
public:
	DllExport CvArtInfoLeaderhead() {}
	DllExport virtual ~CvArtInfoLeaderhead() {}
	DllExport const char* getBackgroundKFM() const;
	DllExport void setBackgroundKFM( const char* szKFM);
	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	CvString m_szBackgroundKFM;
};

class CvArtInfoBonus : public CvArtInfoScalableAsset
{
public:
	DllExport CvArtInfoBonus();
	DllExport virtual ~CvArtInfoBonus() {}
	DllExport int getFontButtonIndex() const;
	DllExport bool isShadowCastor() const;
	DllExport bool isRefractionCastor() const;
	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	int m_iFontButtonIndex;
	bool m_bShadowCastor;
	bool m_bRefractionCastor;
};

class CvArtInfoImprovement : public CvArtInfoScalableAsset
{
public:
	DllExport CvArtInfoImprovement();
	DllExport virtual ~CvArtInfoImprovement();
	DllExport bool isExtraAnimations() const;

	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	bool m_bExtraAnimations;
};

typedef std::vector<std::pair<int, int> > CvTextureBlendSlotList;
class CvArtInfoTerrain : public CvArtInfoAsset
{
public:
	DllExport CvArtInfoTerrain();
	DllExport virtual ~CvArtInfoTerrain();
	DllExport const char* getBaseTexture();
	DllExport void setBaseTexture(const char* szTmp );
	DllExport const char* getGridTexture();
	DllExport void setGridTexture(const char* szTmp );
	DllExport const char* getDetailTexture();
	DllExport void setDetailTexture(const char* szTmp);
	DllExport int getLayerOrder() const;
	DllExport TerrainGroupTypes getTerrainGroup() const;
	DllExport CvTextureBlendSlotList &getBlendList(int blendMask);
	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	CvString m_szDetailTexture;				//!< Detail texture associated with the Terrain base texture
	CvString m_szGridTexture;
	int m_iLayerOrder;									//!< Layering order of texture
	TerrainGroupTypes m_eTerrainGroup;
	int m_numTextureBlends;						//!< number to blend textures.
	CvTextureBlendSlotList  **m_pTextureSlots;	//!< Array of Textureslots per blend tile
};

class CvArtInfoFeature : public CvArtInfoScalableAsset
{
public:
	DllExport CvArtInfoFeature();
	DllExport virtual ~CvArtInfoFeature();
	DllExport bool isAnimated() const;
	DllExport TileArtTypes getTileArtType() const;
	DllExport bool read(CvXMLLoadUtility* pXML);
	class FeatureArtModel
	{
	public:
		FeatureArtModel(const CvString &modelFile)
		{
			m_szModelFile = modelFile;
		}
		const CvString &getModelFile() const
		{
			return m_szModelFile;
		}
	private:
		CvString m_szModelFile;
	};

	class FeatureArtPiece
	{
	public:
		FeatureArtPiece(int connectionMask)
		{
			m_iConnectionMask = connectionMask;
		}
		int getConnectionMask() const
		{
			return m_iConnectionMask;
		}
		int getNumArtModels() const
		{
			return m_aArtModels.size();
		}
		const FeatureArtModel &getArtModel(int index) const
		{
			FAssertMsg((index >= 0) && (index < (int) m_aArtModels.size()), "[Jason] Invalid feature model file index.");
			return m_aArtModels[index];
		}
	private:
		std::vector<FeatureArtModel> m_aArtModels;
		int m_iConnectionMask;
		friend CvArtInfoFeature;
	};

	class FeatureVariety
	{
	public:
		FeatureVariety()
		{
			m_iModelCopies = 0;
		}
		const CvString &getVarietyButton() const
		{
			return m_szVarietyButton;
		}
		const FeatureArtPiece &getFeatureArtPiece(int index) const
		{
			FAssertMsg((index >= 0) && (index < (int) m_aFeatureArtPieces.size()), "[Jason] Invalid feature art index.");
			return m_aFeatureArtPieces[index];
		}
		int getModelCopies() const
		{
			return m_iModelCopies;
		}
		const FeatureArtPiece &getFeatureArtPieceFromConnectionMask(int connectionMask) const
		{
			for(int i=0;i<(int)m_aFeatureArtPieces.size();i++)
				if(m_aFeatureArtPieces[i].getConnectionMask() == connectionMask)
					return m_aFeatureArtPieces[i];
			FAssertMsg(false, "[Jason] Failed to find feature art piece with valid connection mask.");
			return m_aFeatureArtPieces[0];
		}
		FeatureArtPiece &createFeatureArtPieceFromConnectionMask(int connectionMask)
		{
			for(int i=0;i<(int)m_aFeatureArtPieces.size();i++)
				if(m_aFeatureArtPieces[i].getConnectionMask() == connectionMask)
					return m_aFeatureArtPieces[i];
			m_aFeatureArtPieces.push_back(FeatureArtPiece(connectionMask));
			return m_aFeatureArtPieces.back();
		}
	private:
		std::vector<FeatureArtPiece> m_aFeatureArtPieces;
		int m_iModelCopies;
		CvString m_szVarietyButton;
		friend CvArtInfoFeature;
	};

	DllExport const FeatureVariety &getVariety(int index) const;
	DllExport int getNumVarieties() const;
protected:
	int getConnectionMaskFromString(const CvString &connectionString);
	bool m_bAnimated;
	TileArtTypes m_eTileArtType;
	std::vector<FeatureVariety> m_aFeatureVarieties;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEmphasizeInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEmphasizeInfo :
	public CvInfoBase
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvEmphasizeInfo();
	DllExport virtual ~CvEmphasizeInfo();
	DllExport bool isAvoidGrowth() const;

	// Arrays
	DllExport int getYieldChange(int i) const;

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	bool m_bAvoidGrowth;
	// Arrays
	int* m_aiYieldModifiers;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCultureLevelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCultureLevelInfo :
	public CvInfoBase
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	DllExport CvCultureLevelInfo();
	DllExport virtual ~CvCultureLevelInfo();
	DllExport int getThreshold() const;

	DllExport bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iThreshold;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEraInfo
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEraInfo :
	public CvInfoBase
{
public:
	DllExport CvEraInfo();
	DllExport virtual ~CvEraInfo();
	DllExport int getGameTurn() const;
	DllExport int getStartingUnitMultiplier() const;
	DllExport int getStartingDefenseUnits() const;
	DllExport int getStartingWorkerUnits() const;
	DllExport int getStartingExploreUnits() const;
	DllExport int getAdvancedStartPoints() const;
	DllExport int getStartingGold() const;
	DllExport int getFreePopulation() const;
	DllExport int getStartPercent() const;
	DllExport int getGrowthPercent() const;
	DllExport int getTrainPercent() const;
	DllExport int getConstructPercent() const;
	DllExport int getFatherPercent() const;
	DllExport int getGreatGeneralPercent() const;
	DllExport int getEventChancePerTurn() const;
	DllExport int getSoundtrackSpace() const;
	DllExport int getNumSoundtracks() const;
	DllExport const char* getAudioUnitVictoryScript() const;
	DllExport const char* getAudioUnitDefeatScript() const;

	DllExport bool isRevolution() const;
	DllExport bool isNoGoodies() const;
	DllExport bool isFirstSoundtrackFirst() const;

	// Arrays
	DllExport int getSoundtracks(int i) const;
	DllExport int getCitySoundscapeSciptId(int i) const;

	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	int m_iGameTurn;
	int m_iStartingUnitMultiplier;
	int m_iStartingDefenseUnits;
	int m_iStartingWorkerUnits;
	int m_iStartingExploreUnits;
	int m_iAdvancedStartPoints;
	int m_iStartingGold;
	int m_iFreePopulation;
	int m_iStartPercent;
	int m_iGrowthPercent;
	int m_iTrainPercent;
	int m_iConstructPercent;
	int m_iFatherPercent;
	int m_iGreatGeneralPercent;
	int m_iEventChancePerTurn;
	int m_iSoundtrackSpace;
	int m_iNumSoundtracks;
	CvString m_szAudioUnitVictoryScript;
	CvString m_szAudioUnitDefeatScript;
	bool m_bRevolution;
	bool m_bNoGoodies;
	bool m_bFirstSoundtrackFirst;
	// Arrays
	int* m_paiSoundtracks;
	int* m_paiCitySoundscapeSciptIds;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvColorInfo
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvColorInfo :
	public CvInfoBase
{
public:
	DllExport CvColorInfo();
	DllExport virtual ~CvColorInfo();
	DllExport const NiColorA& getColor() const;

	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	NiColorA m_Color;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPlayerColorInfo (ADD to Python)
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerColorInfo :
	public CvInfoBase
{
public:
	DllExport CvPlayerColorInfo();
	DllExport virtual ~CvPlayerColorInfo();
	DllExport int getColorTypePrimary() const;
	DllExport int getColorTypeSecondary() const;
	DllExport int getTextColorType() const;

	DllExport bool read(CvXMLLoadUtility* pXML);
protected:
	int m_iColorTypePrimary;
	int m_iColorTypeSecondary;
	int m_iTextColorType;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvLandscapeInfo
//
//  Purpose:	This info acts as the Civ4Terrain.ini and is initialize in CvXmlLoadUtility with the infos in
//					XML/Terrain/TerrainSettings.xml
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLandscapeInfo :
	public CvInfoBase
{
	public:
		DllExport CvLandscapeInfo();
		DllExport virtual ~CvLandscapeInfo() {}
		DllExport int getPlotVertsWide() const;
		DllExport int getPlotVertsHigh() const;
		DllExport int getPlotsPerCellX() const;
		DllExport int getPlotsPerCellY() const;
		DllExport int getCellVertsWide() const;
		DllExport int getCellVertsHigh() const;
		DllExport int getWaterHeight() const;
		DllExport float getTextureScaleX() const;
		DllExport float getTextureScaleY() const;
		DllExport float getZScale() const;

		DllExport float getPeakScale() const;
		DllExport float getHillScale() const;
		DllExport const char* getEnvironmentTexture();

		DllExport bool read(CvXMLLoadUtility* pXML);

	protected:
		int m_iPlotVertsWide;
		int m_iPlotVertsHigh;
		int m_iPlotsPerCellX;
		int m_iPlotsPerCellY;
		int m_iCellVertsWide;
		int m_iCellVertsHigh;
		int m_iWaterHeight;
		float m_fTextureScaleX;
		float m_fTextureScaleY;
		float m_fZScale;
		float m_fPeakScale;
		float m_fHillScale;
		CvString m_szEnvironmentTexture;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameText
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameText : public CvInfoBase
{
public:
	DllExport CvGameText();
	const wchar* getText() const;
	void setText(const wchar* szText);
	// for Python
	std::wstring pyGetText() const { return getText(); }
	void setGender(const wchar* szGender) { m_szGender = szGender;	}
	const wchar* getGender() const { return m_szGender; }
	void setPlural(const wchar* szPlural) { m_szPlural = szPlural; }
	const wchar* getPlural() const { return m_szPlural; }
	DllExport int getNumLanguages() const; // not static for Python access
	DllExport void setNumLanguages(int iNum); // not static for Python access
	bool read(CvXMLLoadUtility* pXML);

protected:
	CvWString m_szText;
	CvWString m_szGender;
	CvWString m_szPlural;
	static int NUM_LANGUAGES;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvDiplomacyTextInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDiplomacyTextInfo :	public CvInfoBase
{
	friend class CvXMLLoadUtility;		// so it can access private vars to initialize the class
public:
	struct Response
	{
		DllExport Response() :
			m_iNumDiplomacyText(0),
			m_abCivilizationTypes(NULL),
			m_abLeaderHeadTypes(NULL),
			m_abAttitudeTypes(NULL),
			m_abDiplomacyPowerTypes(NULL),
			m_paszDiplomacyText(NULL)
		{
		}
		DllExport virtual ~Response ()
		{
			SAFE_DELETE_ARRAY(m_abCivilizationTypes);
			SAFE_DELETE_ARRAY(m_abLeaderHeadTypes);
			SAFE_DELETE_ARRAY(m_abAttitudeTypes);
			SAFE_DELETE_ARRAY(m_abDiplomacyPowerTypes);
			SAFE_DELETE_ARRAY(m_paszDiplomacyText);
		}
		void read(FDataStreamBase* stream);
		void write(FDataStreamBase* stream);
		int m_iNumDiplomacyText;
		bool* m_abCivilizationTypes;
		bool* m_abLeaderHeadTypes;
		bool* m_abAttitudeTypes;
		bool* m_abDiplomacyPowerTypes;
		CvString* m_paszDiplomacyText;	// needs to be public for xml load assignment
	};

	DllExport CvDiplomacyTextInfo();
	DllExport virtual ~CvDiplomacyTextInfo() { uninit(); }	// free memory - MT
	// note - Response member vars allocated by CvXmlLoadUtility
	DllExport void init(int iNum);
	DllExport void uninit();

	DllExport const Response& getResponse(int iNum) const { return m_pResponses[iNum]; }
	DllExport int getNumResponses() const;

	DllExport bool getCivilizationTypes(int i, int j) const;
	DllExport bool getLeaderHeadTypes(int i, int j) const;
	DllExport bool getAttitudeTypes(int i, int j) const;
	DllExport bool getDiplomacyPowerTypes(int i, int j) const;

	DllExport int getNumDiplomacyText(int i) const;

	DllExport const char* getDiplomacyText(int i, int j) const;

	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	DllExport bool read(CvXMLLoadUtility* pXML);

private:
	int m_iNumResponses;			// set by init
	Response* m_pResponses;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEffectInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEffectInfo :
	public CvInfoBase,
	public CvScalableInfo
{
public:
	DllExport CvEffectInfo();
	DllExport virtual ~CvEffectInfo();
	DllExport const char* getPath() const { return m_szPath; }
	DllExport void setPath(const char* szVal) { m_szPath = szVal; }
	DllExport float getUpdateRate( ) const { return m_fUpdateRate; };

	DllExport void setUpdateRate( float fUpdateRate ) { m_fUpdateRate = fUpdateRate; }
	DllExport bool isProjectile() const { return m_bProjectile; };

	DllExport float getProjectileSpeed() const { return m_fProjectileSpeed; };

	DllExport float getProjectileArc() const { return m_fProjectileArc; };

	DllExport bool isSticky() const { return m_bSticky; };

	DllExport bool read(CvXMLLoadUtility* pXML);

private:
	CvString m_szPath;
	float m_fUpdateRate;
	bool m_bProjectile;
	bool m_bSticky;
	float m_fProjectileSpeed;
	float m_fProjectileArc;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAttachableInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAttachableInfo :
	public CvInfoBase,
	public CvScalableInfo
{
public:
	DllExport CvAttachableInfo();
	DllExport virtual ~CvAttachableInfo();
	DllExport const char* getPath() const { return m_szPath; }
	DllExport void setPath(const char* szVal) { m_szPath = szVal; }
	DllExport bool read(CvXMLLoadUtility* pXML);

private:
	CvString m_szPath;
	float m_fUpdateRate;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameOptionInfo :
	public CvInfoBase
{
public:
	DllExport CvGameOptionInfo();
	DllExport virtual ~CvGameOptionInfo();
	DllExport bool getDefault() const;
	DllExport bool getVisible() const;
	DllExport bool read(CvXMLLoadUtility* pXML);

private:
	bool m_bDefault;
	bool m_bVisible;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMPOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMPOptionInfo :
	public CvInfoBase
{
public:
	DllExport CvMPOptionInfo();
	DllExport virtual ~CvMPOptionInfo();
	DllExport bool getDefault() const;
	DllExport bool read(CvXMLLoadUtility* pXML);

private:
	bool m_bDefault;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvForceControlInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvForceControlInfo :
	public CvInfoBase
{
public:
	DllExport CvForceControlInfo();
	DllExport virtual ~CvForceControlInfo();
	DllExport bool getDefault() const;
	DllExport bool read(CvXMLLoadUtility* pXML);

private:
	bool m_bDefault;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPlayerOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerOptionInfo :
	public CvInfoBase
{
public:
	DllExport CvPlayerOptionInfo();
	DllExport virtual ~CvPlayerOptionInfo();
	DllExport bool getDefault() const;
	DllExport bool read(CvXMLLoadUtility* pXML);

private:
	bool m_bDefault;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGraphicOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGraphicOptionInfo :
	public CvInfoBase
{
public:
	DllExport CvGraphicOptionInfo();
	DllExport virtual ~CvGraphicOptionInfo();
	DllExport bool getDefault() const;
	DllExport bool read(CvXMLLoadUtility* pXML);

private:
	bool m_bDefault;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEventTriggerInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEventTriggerInfo : public CvInfoBase
{
	friend class CvXMLLoadUtility;
public:
	DllExport CvEventTriggerInfo();
	DllExport virtual ~CvEventTriggerInfo();
	DllExport int getPercentGamesActive() const;
	DllExport int getProbability() const;
	DllExport int getNumUnits() const;
	DllExport int getNumBuildings() const;
	DllExport int getNumUnitsGlobal() const;
	DllExport int getNumBuildingsGlobal() const;
	DllExport int getNumPlotsRequired() const;
	DllExport int getPlotType() const;
	DllExport int getOtherPlayerShareBorders() const;
	DllExport int getCityFoodWeight() const;
	DllExport int getCivic() const;
	DllExport int getMinPopulation() const;
	DllExport int getMaxPopulation() const;
	DllExport int getMinMapLandmass() const;
	DllExport int getMinOurLandmass() const;
	DllExport int getMaxOurLandmass() const;
	DllExport int getMinDifficulty() const;
	DllExport int getUnitDamagedWeight() const;
	DllExport int getUnitDistanceWeight() const;
	DllExport int getUnitExperienceWeight() const;
	DllExport int getMinTreasury() const;

	DllExport int getBuildingRequired(int i) const;
	DllExport int getNumBuildingsRequired() const;
	DllExport int getUnitRequired(int i) const;
	DllExport int getNumUnitsRequired() const;
	DllExport int getEvent(int i) const;
	DllExport int getNumEvents() const;
	DllExport int getPrereqEvent(int i) const;
	DllExport int getNumPrereqEvents() const;
	DllExport int getFeatureRequired(int i) const;
	DllExport int getNumFeaturesRequired() const;
	DllExport int getTerrainRequired(int i) const;
	DllExport int getNumTerrainsRequired() const;
	DllExport int getImprovementRequired(int i) const;
	DllExport int getNumImprovementsRequired() const;
	DllExport int getRouteRequired(int i) const;
	DllExport int getNumRoutesRequired() const;

	DllExport const CvWString& getText(int i) const;
	DllExport int getTextEra(int i) const;
	DllExport int getNumTexts() const;
	DllExport const CvWString& getWorldNews(int i) const;
	DllExport int getNumWorldNews() const;

	// Start EmperorFool: Events with Images
	DllExport const TCHAR* getEventArt() const;				// Exposed to Python
	// End EmperorFool: Events with Images
	DllExport bool isTutorial() const;
	DllExport bool isSinglePlayer() const;
	DllExport bool isTeam() const;
	DllExport bool isRecurring() const;
	DllExport bool isGlobal() const;
	DllExport bool isPickPlayer() const;
	DllExport bool isOtherPlayerWar() const;
	DllExport bool isOtherPlayerAI() const;
	DllExport bool isOtherPlayerNative() const;
	DllExport bool isOtherPlayerPlayable() const;
	DllExport bool isPickCity() const;
	DllExport bool isPickOtherPlayerCity() const;
	DllExport bool isShowPlot() const;
	DllExport bool isUnitsOnPlot() const;
	DllExport bool isOwnPlot() const;
	DllExport bool isProbabilityUnitMultiply() const;
	DllExport bool isProbabilityBuildingMultiply() const;
	DllExport bool isPrereqEventCity() const;
	DllExport bool isFrontPopup() const;

	DllExport const char* getPythonCallback() const;
	DllExport const char* getPythonCanDo() const;
	DllExport const char* getPythonCanDoCity() const;
	DllExport const char* getPythonCanDoUnit() const;
	DllExport void read(FDataStreamBase* );
	DllExport void write(FDataStreamBase* );
	DllExport bool read(CvXMLLoadUtility* pXML);
private:
	int m_iPercentGamesActive;
	int m_iProbability;
	int m_iNumUnits;
	int m_iNumBuildings;
	int m_iNumUnitsGlobal;
	int m_iNumBuildingsGlobal;
	int m_iNumPlotsRequired;
	int m_iPlotType;
	int m_iOtherPlayerShareBorders;
	int m_iCityFoodWeight;
	int m_iCivic;
	int m_iMinPopulation;
	int m_iMaxPopulation;
	int m_iMinMapLandmass;
	int m_iMinOurLandmass;
	int m_iMaxOurLandmass;
	int m_iMinDifficulty;
	int m_iUnitDamagedWeight;
	int m_iUnitDistanceWeight;
	int m_iUnitExperienceWeight;
	int m_iMinTreasury;
	std::vector<int> m_aiUnitsRequired;
	std::vector<int> m_aiBuildingsRequired;
	std::vector<int> m_aiEvents;
	std::vector<int> m_aiPrereqEvents;
	std::vector<int> m_aiFeaturesRequired;
	std::vector<int> m_aiTerrainsRequired;
	std::vector<int> m_aiImprovementsRequired;
	std::vector<int> m_aiRoutesRequired;
	std::vector<int> m_aiTextEra;
	std::vector<CvWString> m_aszText;
	std::vector<CvWString> m_aszWorldNews;
	// Start EmperorFool: Events with Images
	CvString m_szEventArt;
	// End EmperorFool: Events with Images
	bool m_bTutorial;
	bool m_bSinglePlayer;
	bool m_bTeam;
	bool m_bRecurring;
	bool m_bGlobal;
	bool m_bPickPlayer;
	bool m_bOtherPlayerWar;
	bool m_bOtherPlayerAI;
	bool m_bOtherPlayerNative;
	bool m_bOtherPlayerPlayable;
	bool m_bPickCity;
	bool m_bPickOtherPlayerCity;
	bool m_bShowPlot;
	bool m_bUnitsOnPlot;
	bool m_bOwnPlot;
	bool m_bProbabilityUnitMultiply;
	bool m_bProbabilityBuildingMultiply;
	bool m_bPrereqEventCity;
	bool m_bFrontPopup;
	CvString m_szPythonCallback;
	CvString m_szPythonCanDo;
	CvString m_szPythonCanDoCity;
	CvString m_szPythonCanDoUnit;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEventInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEventInfo : public CvInfoBase
{
	friend class CvXMLLoadUtility;
public:
	DllExport CvEventInfo();
	DllExport virtual ~CvEventInfo();
	DllExport bool isQuest() const;
	DllExport bool isGlobal() const;
	DllExport bool isTeam() const;
	DllExport bool isCityEffect() const;
	DllExport bool isOtherPlayerCityEffect() const;
	DllExport bool isGoldToPlayer() const;
	DllExport bool isDeclareWar() const;
	DllExport bool isDisbandUnit() const;

	DllExport int getGold() const;
	DllExport int getRandomGold() const;
	DllExport int getCulture() const;
	DllExport int getHealth() const; // R&R, ray, change for Health in Events
	DllExport int getUnitClass() const;
	DllExport int getNumUnits() const;
	DllExport int getBuildingClass() const;
	DllExport int getBuildingChange() const;
	DllExport int getFood() const;
	DllExport int getFoodPercent() const;
	DllExport int getFeature() const;
	DllExport int getFeatureChange() const;
	DllExport int getImprovement() const;
	DllExport int getImprovementChange() const;
	DllExport int getRoute() const;
	DllExport int getRouteChange() const;
	DllExport int getBonusRevealed() const;
	DllExport int getUnitExperience() const;
	DllExport int getUnitImmobileTurns() const;
	DllExport int getOurAttitudeModifier() const;
	DllExport int getAttitudeModifier() const;
	DllExport int getTheirEnemyAttitudeModifier() const;
	DllExport int getPopulationChange() const;
	DllExport int getRevoltTurns() const;
	DllExport int getMinPillage() const;
	DllExport int getMaxPillage() const;
	DllExport int getUnitPromotion() const;
	DllExport int getAIValue() const;
	// TAC - Generic Parameters for Events - Ray - START
	DllExport int getGenericParameter(int x) const;
	// TAC - Generic Parameters for Events - Ray - END
	DllExport int getAdditionalEventChance(int i) const;
	DllExport int getAdditionalEventTime(int i) const;
	DllExport int getClearEventChance(int i) const;
	DllExport int getPlotExtraYield(int i) const;
	DllExport int getUnitCombatPromotion(int i) const;
	DllExport int getUnitClassPromotion(int i) const;
	DllExport const CvWString& getWorldNews(int i) const;
	DllExport int getNumWorldNews() const;
	DllExport int getBuildingYieldChange(int iBuildingClass, int iYield) const;
	DllExport int getNumBuildingYieldChanges() const;
	DllExport const char* getPythonCallback() const;
	DllExport const char* getPythonExpireCheck() const;
	DllExport const char* getPythonCanDo() const;
	DllExport const char* getPythonHelp() const;
	DllExport const wchar* getUnitNameKey() const;
	DllExport const wchar* getQuestFailTextKey() const;
	DllExport const wchar* getOtherPlayerPopup() const;
	DllExport const wchar* getLocalInfoTextKey() const;
	DllExport void read(FDataStreamBase* );
	DllExport void write(FDataStreamBase* );
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport bool readPass2(CvXMLLoadUtility* pXML);

private:
	bool m_bQuest;
	bool m_bGlobal;
	bool m_bTeam;
	bool m_bCityEffect;
	bool m_bOtherPlayerCityEffect;
	bool m_bGoldToPlayer;
	bool m_bDeclareWar;
	bool m_bDisbandUnit;
	int m_iGold;
	int m_iRandomGold;
	int m_iCulture;
	int m_iHealth; // R&R, ray, change for Health in Events
	int m_iUnitClass;
	int m_iNumUnits;
	int m_iBuildingClass;
	int m_iBuildingChange;
	int m_iFood;
	int m_iFoodPercent;
	int m_iFeature;
	int m_iFeatureChange;
	int m_iImprovement;
	int m_iImprovementChange;
	int m_iRoute;
	int m_iRouteChange;
	int m_iBonusRevealed;
	int m_iUnitExperience;
	int m_iUnitImmobileTurns;
	int m_iOurAttitudeModifier;
	int m_iAttitudeModifier;
	int m_iTheirEnemyAttitudeModifier;
	int m_iPopulationChange;
	int m_iRevoltTurns;
	int m_iMinPillage;
	int m_iMaxPillage;
	int m_iUnitPromotion;
	int m_iAIValue;
	// TAC - Generic Parameters for Events - Ray - START
	int m_iGenericParameter1;
	int m_iGenericParameter2;
	int m_iGenericParameter3;
	int m_iGenericParameter4;
	// TAC - Generic Parameters for Events - Ray - END
	int* m_aiPlotExtraYields;
	int* m_aiAdditionalEventChance;
	int* m_aiAdditionalEventTime;
	int* m_aiClearEventChance;
	int* m_aiUnitCombatPromotions;
	int* m_aiUnitClassPromotions;
	std::vector<BuildingYieldChange> m_aBuildingYieldChanges;
	CvString m_szPythonCallback;
	CvString m_szPythonExpireCheck;
	CvString m_szPythonCanDo;
	CvString m_szPythonHelp;
	CvWString m_szUnitName;
	CvWString m_szOtherPlayerPopup;
	CvWString m_szQuestFailText;
	CvWString m_szLocalInfoText;
	std::vector<CvWString> m_aszWorldNews;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMainMenuInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMainMenuInfo : public CvInfoBase
{
public:
	DllExport CvMainMenuInfo();
	DllExport virtual ~CvMainMenuInfo();
	DllExport std::string getScene() const;
	DllExport std::string getSoundtrack() const;
	DllExport std::string getLoading() const;
	DllExport std::string getLoadingSlideshow() const;
	DllExport bool read(CvXMLLoadUtility* pXML);

protected:
	std::string m_szScene;
	std::string m_szSoundtrack;
	std::string m_szLoading;
	std::string m_szLoadingSlideshow;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvFatherInfo
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvFatherInfo : public CvInfoBase
{
public:
	DllExport CvFatherInfo();
	DllExport virtual ~CvFatherInfo();
	DllExport int getFatherCategory() const;
	DllExport int getTrait() const;
	DllExport int getFreeUnits(int iUnitClass) const;
	DllExport int getPointCost(int iFatherPointType) const;
	DllExport bool isRevealImprovement(int iImprovement) const;
	DllExport const char* getPortrait() const;

	std::wstring pyGetQuoteKey() { return getQuoteKey(); }
	DllExport const wchar* getQuoteKey();
	DllExport const char* getSound() const;
	DllExport const char* getSoundMP() const;
	DllExport void read(FDataStreamBase* );
	DllExport void write(FDataStreamBase* );
	DllExport bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iFatherCategory;
	int m_iTrait;
	int* m_aiFreeUnits;
	int* m_aiPointCost;
	bool* m_abRevealImprovement;

	CvString m_szPortrait;
	CvWString m_szQuoteKey;
	CvString m_szSound;
	CvString m_szSoundMP;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvFatherPointInfo
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvFatherPointInfo : public CvInfoBase
{
public:
	DllExport CvFatherPointInfo();
	DllExport virtual ~CvFatherPointInfo();
	DllExport int getChar() const;
	DllExport void setChar(int i);
	DllExport int getFontButtonIndex() const;
	DllExport int getLandTilePoints() const;
	DllExport int getWaterTilePoints() const;
	DllExport int getMeetNativePoints() const;
	DllExport int getScoutVillagePoints() const;
	DllExport int getGoodyPoints() const;
	DllExport int getExperiencePoints() const;
	DllExport int getConquerCityPoints() const;
	DllExport int getRazeCityPoints() const;
	DllExport int getMissionaryPoints() const;
	DllExport int getEuropeTradeGoldPointPercent() const;
	DllExport int getNativeTradeGoldPointPercent() const;
	DllExport int getProductionConversionPoints() const;
	DllExport int getBuildingPoints(int iBuildingClass) const;
	DllExport int getYieldPoints(int iYield) const;
	DllExport bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iChar;
	int m_iFontButtonIndex;
	int m_iLandTilePoints;
	int m_iWaterTilePoints;
	int m_iMeetNativePoints;
	int m_iScoutVillagePoints;
	int m_iGoodyPoints;
	int m_iExperiencePoints;
	int m_iConquerCityPoints;
	int m_iRazeCityPoints;
	int m_iMissionaryPoints;
	int m_iProductionConversionPoints;
	int m_iEuropeTradeGoldPointPercent;
	int m_iNativeTradeGoldPointPercent;
	int* m_aiBuildingPoints;
	int* m_aiYieldPoints;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAlarmInfo
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAlarmInfo : public CvInfoBase
{
public:
	DllExport CvAlarmInfo();
	DllExport virtual ~CvAlarmInfo();

	DllExport int getRange() const;
	DllExport int getColony() const;
	DllExport int getNumColonies() const;
	DllExport int getPopulation() const;
	DllExport int getUnit() const;
	DllExport int getMissionary() const;
	DllExport int getAttitudeDivisor() const;

	DllExport bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iRange;
	int m_iColony;
	int m_iNumColonies;
	int m_iPopulation;
	int m_iUnit;
	int m_iMissionary;
	int m_iAttitudeDivisor;
};

//Androrc UnitArtStyles
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitArtStyleTypeInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitArtStyleTypeInfo : public CvInfoBase
{
public:

	DllExport CvUnitArtStyleTypeInfo();
	DllExport virtual ~CvUnitArtStyleTypeInfo();

    DllExport const char* getArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, int /*ProfessionType*/ m) const;
	DllExport void setArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, int /*ProfessionType*/ m, const char* szVal);

	DllExport bool read(CvXMLLoadUtility* pXML);

protected:

	struct ArtDefneTag
	{
		int iMeshIndex;
		int iUnitType;
		int iProfessionType;
		CvString szTag;
	};
	typedef std::vector<ArtDefneTag> ArtDefineArray;
    ArtDefineArray m_azArtDefineTags;
};
//Androrc End

// PatchMod: Achievements START
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAchieveInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAchieveInfo : public CvInfoBase
{
public:
	DllExport CvAchieveInfo();
	DllExport virtual ~CvAchieveInfo();
	DllExport bool read(CvXMLLoadUtility* pXML);
	DllExport bool isActive();
	DllExport bool isLandDiscovered();
	DllExport bool isAfterMove();
	DllExport bool isDiscoverEast();
	DllExport bool isDiscoverWest();
	DllExport int getLowTurnNumber();
	DllExport int getHighTurnNumber();
	DllExport int getPictureX();
	DllExport int getPictureY();
	DllExport int getNumColonies();
	DllExport int getNumEuroContacts();
	DllExport int getNumNativeContacts();
	DllExport int getNumGoodsTraded();
	DllExport int getNumCombatsWonNeeded();
	DllExport int getNumSeaCombatsWonNeeded(); // R&R, ray, Achievments for Sea Combats
	DllExport int getNumBuildings(int iBuilding) const;
	DllExport int getNumUnits(int iUnit) const;
	DllExport int getNumProfessions(int iProfession) const;
	DllExport int getYieldProduced(int iYield) const;
	DllExport const wchar* getDescription() const;
	DllExport const wchar* getHeaderTextKey() const;
	DllExport const wchar* getBodyTextKey() const;
	DllExport const char* getPictureFileName() const;
protected:
	bool m_bActive;
	bool m_bLandDiscovered;
	bool m_bAfterMove;
	bool m_bDiscoverEast;
	bool m_bDiscoverWest;
	int m_iLowTurnNumber;
	int m_iHighTurnNumber;
	int m_iPictureX;
	int m_iPictureY;
	int m_iNumColonies;
	int m_iNumEuroContacts;
	int m_iNumNativeContacts;
	int m_iNumGoodsTraded;
	int m_iNumCombatsWonNeeded;
	int m_iNumSeaCombatsWonNeeded; // R&R, ray, Achievments for Sea Combats
	int* m_aiNumBuildings;
	int* m_aiNumUnits;
	int* m_aiNumProfessions;
	int* m_aiYieldProduced;
	CvWString m_szDescriptionKey;
	CvWString m_szHeaderTextKey;
	CvWString m_szBodyTextKey;
	CvString m_szPictureFileName;
};
// PatchMod: Achievements END

class CivEffectInfo :
	public CvInfoBase
{
public:
	CivEffectInfo();
	~CivEffectInfo();

	inline const InfoArray* getAllowedBuildingClasses()       const { return &m_info_AllowBuildings   ; }
	inline const InfoArray* getAllowedUnitClasses()           const { return &m_info_AllowUnits       ; }

	bool read(CvXMLLoadUtility* pXML);

protected:
	InfoArrayMod m_info_AllowBuildings;
	InfoArrayMod m_info_AllowUnits;
};


#endif
