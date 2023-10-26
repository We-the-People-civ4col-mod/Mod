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
#ifdef COMPILE_STATIC_TEST
	: private boost::noncopyable
#endif
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	//constructor
	CvInfoBase();
	//destructor
	DllExport virtual ~CvInfoBase();
	virtual void reset();
	DllExport bool isGraphicalOnly() const;
	bool hideFromPedia() const;

	DllExport const char* getType() const;
	virtual const char* getButton() const;
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
	const wchar* getCivilopedia() const;
	DllExport const wchar* getHelp() const;
	const wchar* getStrategy() const;

	bool isMatchForLink(std::wstring szLink, bool bKeysOnly) const;
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);
	virtual bool read(CvXMLLoadUtility* pXML);
	virtual bool readPass2(CvXMLLoadUtility* pXML) { return false; }
	virtual bool readPass3() { FAssertMsg(false, "Override this"); return false; }

	void cleanStrings();
	bool postLoadSetup();

protected:
	void checkStringContents(CvWString& szStr, const wchar* szExtension);

	template<typename T0, typename T1, typename T2, typename T3>
	void readXML(InfoArray<T0, T1, T2, T3>& kInfo, const char* szTag);

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

private:
	static CvXMLLoadUtility* m_pXML;
};

//
// holds the scale for scalable objects
//
class CvScalableInfo
{
public:
	CvScalableInfo() : m_fScale(1.0f), m_fInterfaceScale(1.0f) { }
	DllExport float getScale() const;
	void setScale(float fScale);
	DllExport float getInterfaceScale() const;
	void setInterfaceScale(float fInterfaceScale);
	bool read(CvXMLLoadUtility* pXML);
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
	CvHotkeyInfo();
	//destructor
	virtual ~CvHotkeyInfo();
	bool read(CvXMLLoadUtility* pXML);
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);
	int getActionInfoIndex() const;
	void setActionInfoIndex(int i);
	int getHotKeyVal() const;
	void setHotKeyVal(int i);
	int getHotKeyPriority() const;
	void setHotKeyPriority(int i);
	int getHotKeyValAlt() const;
	void setHotKeyValAlt(int i);
	int getHotKeyPriorityAlt() const;
	void setHotKeyPriorityAlt(int i);
	int getOrderPriority() const;
	void setOrderPriority(int i);
	bool isAltDown() const;
	void setAltDown(bool b);
	bool isShiftDown() const;
	void setShiftDown(bool b);
	bool isCtrlDown() const;
	void setCtrlDown(bool b);
	bool isAltDownAlt() const;
	void setAltDownAlt(bool b);
	bool isShiftDownAlt() const;
	void setShiftDownAlt(bool b);
	bool isCtrlDownAlt() const;
	void setCtrlDownAlt(bool b);
	const char* getHotKey() const;
	void setHotKey(const char* szVal);
	std::wstring getHotKeyDescription() const;
	void setHotKeyDescription(const wchar* szHotKeyDescKey, const wchar* szHotKeyAltDescKey, const wchar* szHotKeyString);

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
	CvDiplomacyResponse();
	virtual ~CvDiplomacyResponse();
	int getNumDiplomacyText();
	void setNumDiplomacyText(int i);
	bool getCivilizationTypes(int i);
	bool* getCivilizationTypes() const;
	void setCivilizationTypes(int i, bool bVal);
	bool getLeaderHeadTypes(int i);
	bool* getLeaderHeadTypes() const;
	void setLeaderHeadTypes(int i, bool bVal);
	bool getAttitudeTypes(int i) const;
	bool* getAttitudeTypes() const;
	void setAttitudeTypes(int i, bool bVal);
	bool getDiplomacyPowerTypes(int i);
	bool* getDiplomacyPowerTypes() const;
	void setDiplomacyPowerTypes(int i, bool bVal);
	const char* getDiplomacyText(int i) const;
	const CvString* getDiplomacyText() const;
	void setDiplomacyText(int i, CvString szText);
	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	bool read(CvXMLLoadUtility* pXML);

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
	virtual ~CvPromotionInfo();
	PromotionTypes getIndex() const { return m_eIndex; }
	bool hideFromPedia() const;
	bool isNotEarnedByXP() const; //WTP, ray, Promotions not earned by XP
	int getPrereqPromotion() const;
	void setPrereqPromotion(int i);
	int getPrereqOrPromotion1() const;
	void setPrereqOrPromotion1(int i);
	int getPrereqOrPromotion2() const;
	void setPrereqOrPromotion2(int i);

	int getVisibilityChange() const;
	int getMovesChange() const;
	int getMoveDiscountChange() const;
	int getWithdrawalChange() const;
	int getCargoChange() const;
	int getBombardRateChange() const;
	int getEnemyHealChange() const;
	int getNeutralHealChange() const;
	int getFriendlyHealChange() const;
	int getSameTileHealChange() const;
	int getAdjacentTileHealChange() const;
	int getCombatPercent() const;
	int getCityAttackPercent() const;
	int getCityDefensePercent() const;
	int getHillsAttackPercent() const;
	int getHillsDefensePercent() const;
	int getDomesticBonusPercent() const;
	int getCommandType() const;
	void setCommandType(int iNewType);
	int getPillageChange() const;
	int getAnimalGoldChange() const; //WTP, ray, Animal Promotions increase gold from Animals
	int getSlaveRevoltReductionBonus() const; //WTP, ray, Slave Hunter and Slave Master
	int getSlaveWorkerProductionBonus() const; //WTP, ray, Slave Hunter and Slave Master
	int getAdditionalLawToCity() const; // WTP, ray, Lawkeeper Promotion - START
	int getUpgradeDiscount() const;
	int getExperiencePercent() const;

	bool isLeader() const;
	bool isNegativePromotion() const; //WTP, ray Negative Promotions - START
	bool isBlitz() const;
	bool isAmphib() const;
	bool isRiver() const;
	bool isEnemyRoute() const;
	bool isAlwaysHeal() const;
	bool isHillsDoubleMove() const;
	bool isAvailableForDefensiveUnit() const; //WTP, jbu - calculated from attack characteristics or rather lack thereof
	bool isNotAvailableForDefensiveUnit() const; //WTP, jbu

	const char* getSound() const;
	void setSound(const char* szVal);
	// Arrays
	int getTerrainAttackPercent(int i) const;
	int getTerrainDefensePercent(int i) const;
	int getFeatureAttackPercent(int i) const;
	int getFeatureDefensePercent(int i) const;
	int getUnitClassAttackModifier(int i) const;
	int getUnitClassDefenseModifier(int i) const;
	int getUnitCombatModifierPercent(int i) const;
	int getDomainModifierPercent(int i) const;

	bool getTerrainDoubleMove(int i) const;
	bool getFeatureDoubleMove(int i) const;
	bool getUnitCombat(int i) const;

	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	bool postLoadSetup();

protected:

	PromotionTypes m_eIndex;
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
	int m_iAnimalGoldChange; //WTP, ray, Animal Promotions increase gold from Animals
	int m_iSlaveRevoltReductionBonus; //WTP, ray, Slave Hunter and Slave Master
	int m_iSlaveWorkerProductionBonus; //WTP, ray, Slave Hunter and Slave Master
	int m_iAdditionalLawToCity; // WTP, ray, Lawkeeper Promotion - START
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	bool m_bHideFromPedia;
	bool m_bNotEarnedByXP; //WTP, ray, Promotions not earned by XP
	bool m_bLeader;
	bool m_bNegativePromotion; //WTP, ray Negative Promotions - START
	bool m_bBlitz;
	bool m_bAmphib;
	bool m_bRiver;
	bool m_bEnemyRoute;
	bool m_bAlwaysHeal;
	bool m_bHillsDoubleMove;
	bool m_bAvailableForDefensiveUnit;
	void calculateAvailableForDefensiveUnit();
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
	virtual ~CvProfessionInfo();
	ProfessionTypes getIndex() const { return m_eIndex; }
	int getUnitCombatType() const;
	// TAC - LbD - Ray - START
	bool LbD_isUsed() const;
	int LbD_getExpert() const;
	int LbD_getLearnLevel() const;
	// TAC - LbD - Ray - END
	int getDefaultUnitAIType() const;
	// R&R, ray , MYCP partially based on code of Aymerick - START
	// int getYieldProduced() const;
	// int getYieldConsumed() const;
	// R&R, ray , MYCP partially based on code of Aymerick - END
	int getSpecialBuilding() const;
	int getCombatChange() const;
	int getBombardRateChangeProfession() const; // WTP, ray, Cannons to Professions - START
	int getBarracksSpaceNeededChange() const;	// WTP, ray, new Barracks System - START
	int getMovesChange() const;
	int getWorkRate() const;
	int getMissionaryRate() const;
	int getNativeTradeRate() const; // WTP, ray, Native Trade Posts - START
	int getPowerValue() const;
	int getAssetValue() const;
	bool isWorkPlot() const;
	bool isCitizen() const;
	bool isWater() const;
	bool isScout() const;
	bool isCanCrossLargeRivers() const; //WTP, ray, Large Rivers
	bool isCanEnterPeaks() const; //WTP, ray, Large Rivers
	bool isCityDefender() const;
	bool canFound() const;
	bool isUnarmed() const;
	bool isNoDefensiveBonus() const;
	int getYieldEquipmentAmount(int iYield) const;
	bool isFreePromotion(int i) const;
	// R&R, ray , MYCP partially based on code of Aymerick - START
	int getYieldsProduced(int i) const;
	int getNumYieldsProduced() const;
	int getYieldsConsumed(int i) const;
	int getNumYieldsConsumed() const;
	// R&R, ray , MYCP partially based on code of Aymerick - END

	bool isWorkSlot() const;

	/// Move Into Peak - start - Nightinggale
	bool allowsMoveIntoPeak() const {return m_bMoveIntoPeak;}
	/// Move Into Peak - end - Nightinggale

	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	bool read(CvXMLLoadUtility* pXML);
	bool readPass3();


	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	ProfessionTypes m_eIndex;
	int m_iUnitCombatType;
	int m_iDefaultUnitAIType;
	// R&R, ray , MYCP partially based on code of Aymerick - START
	// int m_iYieldProduced;
	// int m_iYieldConsumed;
	// R&R, ray , MYCP partially based on code of Aymerick - END
	int m_iSpecialBuilding;
	int m_iCombatChange;
	int m_iBombardRateChangeProfession;// WTP, ray, Cannons to Professions - START
	int m_iBarracksSpaceNeededChange; // WTP, ray, new Barracks System - START
	int m_iMovesChange;
	int m_iWorkRate;
	int m_iMissionaryRate;
	int m_iNativeTradeRate; // WTP, ray, Native Trade Posts - START
	int m_iPowerValue;
	int m_iAssetValue;
	bool m_bWorkPlot;
	bool m_bCitizen;
	bool m_bWater;
	bool m_bScout;
	bool m_bCanCrossLargeRivers; //WTP, ray, Large Rivers
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
	CvMissionInfo();
	virtual ~CvMissionInfo();
	DllExport int getTime() const;

	bool isSound() const;
	DllExport bool isTarget() const;
	bool isBuild() const;
	bool getVisible() const;
	DllExport EntityEventTypes getEntityEvent() const;
	const char* getWaypoint() const;		// effect type, Exposed to Python
	bool read(CvXMLLoadUtility* pXML);
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
	CvControlInfo();
	virtual ~CvControlInfo();
	bool read(CvXMLLoadUtility* pXML);
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
	CvCommandInfo();
	virtual ~CvCommandInfo();
	bool getConfirmCommand() const;
	bool getVisible() const;
	DllExport bool getAll() const;
	bool read(CvXMLLoadUtility* pXML);
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
	CvAutomateInfo();
	virtual ~CvAutomateInfo();
	int getCommand() const;
	void setCommand(int i);
	int getAutomate() const;
	void setAutomate(int i);
	bool getConfirmCommand() const;
	void setConfirmCommand(bool bVal);
	bool getVisible() const;
	void setVisible(bool bVal);
	bool read(CvXMLLoadUtility* pXML);
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
	CvActionInfo();
	virtual ~CvActionInfo();
	int getMissionData() const;
	int getCommandData() const;

	int getAutomateType() const;
	int getInterfaceModeType() const;
	int getMissionType() const;
	int getCommandType() const;
	int getControlType() const;
	int getOriginalIndex() const;
	void setOriginalIndex(int i);
	bool isConfirmCommand() const;
	DllExport bool isVisible() const;
	DllExport ActionSubTypes getSubType() const;
	void setSubType(ActionSubTypes eSubType);
	// functions to replace the CvInfoBase calls
	const char* getType() const;
	const wchar* getDescription() const;
	const wchar* getCivilopedia() const;
	const wchar* getHelp() const;
	const wchar* getStrategy() const;
	virtual const char* getButton() const;
	const wchar* getTextKeyWide() const;
	// functions to replace the CvHotkey calls
	int getActionInfoIndex() const;
	DllExport int getHotKeyVal() const;
	DllExport int getHotKeyPriority() const;
	DllExport int getHotKeyValAlt() const;
	DllExport int getHotKeyPriorityAlt() const;
	int getOrderPriority() const;
	DllExport bool isAltDown() const;
	DllExport bool isShiftDown() const;
	DllExport bool isCtrlDown() const;
	DllExport bool isAltDownAlt() const;
	DllExport bool isShiftDownAlt() const;
	DllExport bool isCtrlDownAlt() const;
	const char* getHotKey() const;

	std::wstring getHotKeyDescription() const;

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
	virtual ~CvUnitInfo();

	int getBerthSize() const; // PatchMod: Berth size START
	int getHarbourSpaceNeeded() const; // WTP, ray, new Harbour System - START
	int getBarracksSpaceNeeded() const; // WTP, ray, new Barracks System - START

/** NBMOD EDU **/
    int NBMOD_GetTeachLevel() const;
    float NBMOD_GetStrengthWeight() const;
/** NBMOD EDU **/
	int getAIWeight() const;
	int getHurryCostModifier() const;
	int getProductionWhenUsed() const; // WTP, ray, Construction Supplies - START
	int getEuropeCost() const;
	int getEuropeCostIncrease() const;
	/*** TRIANGLETRADE 10/24/08 by DPII ***/
	int getAfricaCost() const;
	int getAfricaCostIncrease() const;
	/**************************************/
	// R&R, ray, Port Royal
	int getPortRoyalCost() const;
	int getPortRoyalCostIncrease() const;
	// R&R, ray, Port Royal - END
	int getImmigrationWeight() const;
	int getImmigrationWeightDecay() const;
	int getAdvancedStartCost() const;
	int getAdvancedStartCostIncrease() const;
	int getMinAreaSize() const;
	int getMoves() const;
	int getWorkRate() const;
	int getWorkRateModifier() const;
	int getGoldFromGoodiesAndChiefsModifier() const; // WTP, ray, Scout Gold Modifier for Goodies and Chiefs at Unit - START
	int getMissionaryRateModifier() const;
	int getNativeTradeRateModifier() const; // WTP, ray, Native Trade Posts - START
	int getCombat() const;
	void setCombat(int iNum);
	int getXPValueAttack() const;
	int getXPValueDefense() const;
	int getWithdrawalProbability() const;
	int getCityAttackModifier() const;
	int getCityDefenseModifier() const;
	int getHillsAttackModifier() const;
	int getHillsDefenseModifier() const;
	int getDomesticBonusModifier() const;
	int getBombardRate() const;
	int getSpecialCargo() const;
	int getDomainCargo() const;

	int getCargoSpace() const;
	int getRequiredTransportSize() const;
	int getAssetValue() const;
	int getPowerValue() const;
	UnitClassTypes getUnitClassType() const;
	int getSpecialUnitType() const;
	int getUnitCaptureClassType() const;
	int getUnitCombatType() const;
	int getDomainType() const;
	ProfessionTypes getDefaultProfession() const;
	int getDefaultUnitAIType() const;
	int getInvisibleType() const;
	int getSeeInvisibleType(int i) const;
	int getNumSeeInvisibleTypes() const;
	int getPrereqBuilding() const;
	DllExport int getGroupSize(int iProfession) const;									 // the initial number of individuals in the unit group
	DllExport int getGroupDefinitions(int iProfession) const;					 // the number of UnitMeshGroups for this unit
	int getMeleeWaveSize(int iProfession) const;
	int getRangedWaveSize(int iProfession) const;
	int getNumUnitNames() const;
	int getCommandType() const;
	void setCommandType(int iNewType);
	int getLearnTime() const;
	int getStudentWeight() const;
	int getTeacherWeight() const;
	// < JAnimals Mod Start >
	int getAnimalPatrolWeight() const;
	int getAnimalAttackWeight() const;
	// < JAnimals Mod End >
	bool isNoBadGoodies() const;
	bool isOnlyDefensive() const;
	bool isNoCapture() const;
	bool isQuickCombat() const;
	bool isRivalTerritory() const;
	bool isMilitaryProduction() const;
	bool isFound() const;
	bool isInvisible() const;
	void setInvisible(bool bEnable) ;
	bool isNoDefensiveBonus() const;
	bool isCanMoveImpassable() const;
	bool isCanMoveAllTerrain() const;
	bool isFlatMovementCost() const;
	bool isIgnoreTerrainCost() const;
	DllExport bool isMechUnit() const;
	bool isLineOfSight() const;
	bool isHiddenNationality() const;
	bool isSlaveShip() const; // WTP, ray Slave Ship
	bool isTreasureShip() const;// WTP, ray Treasure Ship
	bool isTroopShip() const;// WTP, ray Troop Ship
	bool isAlwaysHostile() const;
	bool isTreasure() const;
	EthnicityTypes getEthnicity() const; // WTP, ray, Ethnically correct Population Growth - START
	CitizenStatusTypes getCitizenStatus() const; // WTP, ray, Ethnically correct Population Growth - START
	// R&R, ray, Changes for Treasures, START
	bool isNoRevealMap() const;
	// R&R, ray, Changes for Treasures, END

	// TAC - LbD - Ray - START
	bool LbD_canBecomeExpert() const;
	bool LbD_canGetFree() const;
	bool LbD_canEscape() const;
	// TAC - LbD - Ray - END

	// WTP, ray, LbD Slaves Revolt and Free - START
	bool LbD_canRevolt() const;
	// WTP, ray, LbD Slaves Revolt and Free - END

	bool isCapturesCargo() const;
	int getCaptureShipsChanceIncrease() const;
	// TAC Capturing Ships - ray
	bool isCapturesShips() const;
	// TAC Capturing Ships - ray -END
	bool isLandYieldChanges() const;
	bool isWaterYieldChanges() const;

	//TAC Whaling, ray
	bool isGatherBoat() const;
	//End TAC Whaling, ray
	// < JAnimals Mod Start >
	bool isAnimal() const;
	// < JAnimals Mod End >
	DllExport float getUnitMaxSpeed(int iProfession) const;
	DllExport float getUnitPadTime(int iProfession) const;

	/// Move Into Peak - start - Nightinggale
	bool allowsMoveIntoPeak() const {return m_bMoveIntoPeak;}
	/// Move Into Peak - end - Nightinggale

	// Arrays
	int getProductionTraits(int i) const;
	int getTerrainAttackModifier(int i) const;
	int getTerrainDefenseModifier(int i) const;
	int getFeatureAttackModifier(int i) const;
	int getFeatureDefenseModifier(int i) const;
	int getUnitClassAttackModifier(int i) const;
	int getUnitClassDefenseModifier(int i) const;
	int getUnitCombatModifier(int i) const;
	int getDomainModifier(int i) const;
	int getYieldModifier(int i) const;
	// R&R, Androrc, Domestic Market -- modified by Nightinggale - start
	const InfoArray<YieldTypes, int>& getYieldDemands() const;
	// R&R, Androrc, Domestic Market -- modified by Nightinggale - end
	int getBonusYieldChange(int i) const;
	int getYieldChange(int i) const;
	int getYieldCost(YieldTypes eYield) const;
	DllExport int getUnitGroupRequired(int i, int iProfession) const;

	bool getUpgradeUnitClass(int i) const;
	bool getUnitAIType(int i) const;
	bool getNotUnitAIType(int i) const;
	bool getBuilds(int i) const;
	bool getTerrainImpassable(int i) const;
	bool getFeatureImpassable(int i) const;
	// < JAnimals Mod Start >
	bool getTerrainNative(int iIndex) const;
	// < JAnimals Mod End >
	bool isEvasionBuilding(int i) const;
	//TAC Whaling, ray
	int getNumYieldsGatherable() const;
	bool canGatherYield(int index) const;
	//End TAC Whaling, ray
	bool getFreePromotions(int i) const;
	bool getProfessionsNotAllowed(int i) const;  	///TK Viscos Mod
	bool isPrereqOrBuilding(int i) const;
	int getLeaderPromotion() const;
	int getLeaderExperience() const;

	//Androrc UnitArtStyles
	//const char* getArtDefineTag(int i, int iProfession) const;
	const char* getArtDefineTag(int i, int iProfession, int iStyle = -1) const;
	//Androrc End
	const char* getUnitNames(int i) const;
	DllExport const char* getFormationType() const;
	const char* getButton() const;
	void updateArtDefineButton();
	const CvArtInfoUnit* getArtInfo(int i, int iProfession) const;
	//Androrc UnitArtStyles
	const CvArtInfoUnit* getUnitArtStylesArtInfo(int i, int iProfession, int iStyle = -1) const;
	//Androrc End
	const CvUnitMeshGroups& getProfessionMeshGroup(int iProfession) const;
	void read(FDataStreamBase* );
	void write(FDataStreamBase* );
	bool read(CvXMLLoadUtility* pXML);

	int PYgetYieldCost(int i) const;

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:

	int m_iBerthSize; // PatchMod: Berth size START
	int m_iHarbourSpaceNeeded; // WTP, ray, new Harbour System - START
	int m_iBarracksSpaceNeeded; // WTP, ray, new Barracks System - START

    /** NBMOD EDU **/
    int m_iTeachLevel;
    float m_fNBMOD_REF_StrengthWeight;
    /** NBMOD EDU **/

	int m_iAIWeight;
	int m_iHurryCostModifier;
	int m_iProductionWhenUsed; // WTP, ray, Construction Supplies - START
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
	int m_iGoldFromGoodiesAndChiefsModifier; // WTP, ray, Scout Gold Modifier for Goodies and Chiefs at Unit - START
	int m_iMissionaryRateModifier;
	int m_iNativeTradeRateModifier; // WTP, ray, Native Trade Posts - START
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
	UnitClassTypes m_eUnitClassType;
	int m_iSpecialUnitType;
	int m_iUnitCaptureClassType;
	int m_iUnitCombatType;
	int m_iDomainType;
	ProfessionTypes m_eDefaultProfession;
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
	bool m_bSlaveShip; // WTP, ray Slave Ship
	bool m_bTreasureShip; // WTP, ray Treasure Ship
	bool m_bTroopShip; // WTP, ray Treasure Ship
	bool m_bAlwaysHostile;
	bool m_bTreasure;
	EthnicityTypes m_eEthnicity; // WTP, ray, Ethnically correct Population Growth - START
	CitizenStatusTypes m_eCitizenStatus; // WTP, ray, Ethnically correct Population Growth - START
	// R&R, ray, Changes for Treasures, START
	bool m_bNoRevealMap;
	// R&R, ray, Changes for Treasures, END

	// TAC - LbD - Ray - START
	bool m_canBecomeExpert;
	bool m_canGetFree;
	bool m_canEscape;
	// TAC - LbD - Ray - END
	// WTP, ray, LbD Slaves Revolt and Free - START
	bool m_canRevolt;
	// WTP, ray, LbD Slaves Revolt and Free - END
	bool m_bCapturesCargo;
	// TAC Capturing Ships - ray
	bool m_bCapturesShips;
	int m_iCaptureShipsChanceIncrease; // WTP, ray, Capture Ship chance increase - START
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
	// R&R, Androrc, Domestic Market -- modified by Nightinggale - start
	InfoArray<YieldTypes, int> m_info_YieldDemands;
	// R&R, Androrc, Domestic Market -- modified by Nightinggale - end
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
	virtual ~CvUnitFormationInfo();
	DllExport const char* getFormationType() const;
	DllExport const std::vector<EntityEventTypes> & getEventTypes() const;
	DllExport int getNumUnitEntries() const;
	DllExport const CvUnitEntry &getUnitEntry(int index) const;
	DllExport void addUnitEntry(const CvUnitEntry &unitEntry);
	int getNumGreatUnitEntries() const;
	DllExport const CvUnitEntry &getGreatUnitEntry(int index) const;
	bool read(CvXMLLoadUtility* pXML);
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
	CvSpecialUnitInfo();
	virtual ~CvSpecialUnitInfo();
	bool isValid() const;
	bool isCityLoad() const;
	// Arrays
	bool isCarrierUnitAIType(int i) const;
	int getProductionTraits(int i) const;

	bool read(CvXMLLoadUtility* pXML);
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
	virtual ~CvCivicInfo();
	int getCivicOptionType() const;
	int getAIWeight() const;
	inline CivEffectTypes getCivEffect() const { return m_eCivEffect; }
	int getGreatGeneralRateModifier() const;
	int getDomesticGreatGeneralRateModifier() const;
	int getFreeExperience() const;
	int getWorkerSpeedModifier() const;
	int getImprovementUpgradeRateModifier() const;
	int getMilitaryProductionModifier() const;
	int getExpInBorderModifier() const;
	int getImmigrationConversion() const;
	int getNativeAttitudeChange() const;
	int getNativeCombatModifier() const;
	int getFatherPointModifier() const;
	bool isDominateNativeBorders() const;
	bool isRevolutionEuropeTrade() const;

	// Arrays
	int getYieldModifier(int i) const;
	int* getYieldModifierArray() const;
	int getCapitalYieldModifier(int i) const;
	int* getCapitalYieldModifierArray() const;
	int getProfessionCombatChange(int iProfession) const;
	bool isHurry(int i) const;
	bool isSpecialBuildingNotRequired(int i) const;
	int getImprovementYieldChanges(int i, int j) const;

	int getNumFreeUnitClasses() const;
	int getFreeUnitClass(int i) const;

	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	bool read(CvXMLLoadUtility* pXML);
//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	CivEffectTypes m_eCivEffect;
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
	virtual ~CvDiplomacyInfo();
	void uninit();

	const CvDiplomacyResponse& getResponse(int iNum) const;
	int getNumResponses() const;

	bool getCivilizationTypes(int i, int j) const;
	bool getLeaderHeadTypes(int i, int j) const;
	bool getAttitudeTypes(int i, int j) const;
	bool getDiplomacyPowerTypes(int i, int j) const;

	int getNumDiplomacyText(int i) const;

	const char* getDiplomacyText(int i, int j) const;

	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	bool read(CvXMLLoadUtility* pXML);
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
	CvUnitClassInfo();
	virtual ~CvUnitClassInfo();
	int getDefaultUnitIndex() const;
	void setDefaultUnitIndex(int i);
	bool read(CvXMLLoadUtility* pXML);
	bool readPass3();
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
	virtual ~CvBuildingInfo();
	DllExport int getBuildingClassType() const;
	int getVictoryPrereq() const;
	DllExport int getFreeStartEra() const;
	int getMaxStartEra() const;
	int getFreePromotion() const;
	int getRouteTypeCreated() const; //ray, removing hardcoded Roads for Buildings
	int getAIWeight() const;
	int getHurryCostModifier() const;
	int getAdvancedStartCost() const;
	int getAdvancedStartCostIncrease() const;
	int getProfessionOutput() const;
	int getMaxWorkers() const;
	int getMinAreaSize() const;
	int getNumCitiesPrereq() const;
	int getNumTeamsPrereq() const;
	int getUnitLevelPrereq() const;
	int getMinLatitude() const;
	int getMaxLatitude() const;
	int getFreeExperience() const;
	int getFoodKept() const;
	int getMilitaryProductionModifier() const;
	int getAssetValue() const;
	int getPowerValue() const;
	int getYieldStorage() const;
	int getMaxHarbourSpaceProvided() const; // WTP, ray, new Harbour System - START
	int getMaxBarracksSpaceProvided() const; // WTP, ray, new Barracks System - START
	int getSpecialBuildingType() const;
	inline BuildingTypes getIndexOf_NextBuildingType_In_SpecialBuilding() const		{ return m_eIndexOf_NextBuildingType_In_SpecialBuilding; }
	int getConquestProbability() const;
	int getHealRateChange() const;
	int getDefenseModifier() const;
	int getBombardDefenseModifier() const;
	int getMissionType() const;
	void setMissionType(int iNewType);
	inline int getStorageLossSellPercentage() const							{ return m_iStorageLossSellPercentage; }
	inline bool getIgnoresBoycott() const									{ return m_bIgnoresBoycott; }
	inline bool getUnlocksStorageLossTradeSettings() const					{ return m_bUnlocksStorageLossTradeSettings; }
	int getSpecialBuildingPriority() const;
	DllExport float getVisibilityPriority() const;
	bool isWorksWater() const;
	DllExport bool isWater() const;
	bool isRiver() const;
	bool isCapital() const;
	bool isNationalWonder() const; // R&R, ray, National Wonders
	bool isNeverCapture() const;
	bool isCenterInCity() const;
	int getDomesticMarketModifier() const { return m_iDomesticMarketModifier; }
	int getEntertainmentGoldModifier() const { return m_iEntertainmentGoldModifier; } // ray, Balancing of Entertainment Buildings in XML
	// R&R, Androrc, Domestic Market -- modified by Nightinggale - start
	const InfoArray<YieldTypes, int>& getYieldDemands() const;
	// R&R, Androrc, Domestic Market -- modified by Nightinggale - end
	const char* getConstructSound() const;
	void setConstructSound(const char* szVal);
	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szVal);
	const char* getMovieDefineTag() const;
	void setMovieDefineTag(const char* szVal);
	// Arrays
	int getYieldChange(int i) const;
	int* getYieldChangeArray() const;
	int getYieldModifier(int i) const;;
	int* getYieldModifierArray() const;
	int getLandPlotYieldChange(int i) const; // R&R, ray, Landplot Yields
	int* getLandPlotYieldChangeArray() const; // R&R, ray, Landplot Yields
	int getSeaPlotYieldChange(int i) const;
	int* getSeaPlotYieldChangeArray() const;
	int getRiverPlotYieldChange(int i) const;
	int* getRiverPlotYieldChangeArray() const;
	int getUnitCombatFreeExperience(int i) const;
	int getDomainFreeExperience(int i) const;
	int getDomainProductionModifier(int i) const;
	int getProductionTraits(int i) const;
	int getPrereqNumOfBuildingClass(int i) const;
	//WTP, Nightinggale - Terrain locator - start
	const InfoArray<TerrainTypes>& getRequiredAdjacentTerrains() const;
	const InfoArray<TerrainTypes>& getRequiredCatchmentAreaTerrains() const;
	const InfoArray<FeatureTypes>& getRequiredCatchmentAreaFeatures() const;
	const InfoArray<TerrainTypes>& getAIRequiredCatchmentAreaTerrains() const;
	const InfoArray<FeatureTypes>& getAIRequiredCatchmentAreaFeatures() const;
	const InfoArray<PlotTypes   >& AI_getRequiredCatchmentAreaPlotTypes() const;
	const InfoArray<UnitClassTypes, int>& AI_getUnitClassWeight() const;
	//WTP, Nightinggale - Terrain locator - end
	int getYieldCost(int i) const;

	bool isBuildingClassNeededInCity(int i) const;

	// Other
	DllExport const CvArtInfoBuilding* getArtInfo() const;
	const CvArtInfoMovie* getMovieInfo() const;
	const char* getButton() const;
	const char* getMovie() const;
	// serialization
	void read(FDataStreamBase*);
	void write(FDataStreamBase*);
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
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
	int m_iRouteTypeCreated; //ray, removing hardcoded Roads for Buildings
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
	int m_iMaxHarbourSpaceProvided; // WTP, ray, new Harbour System - START
	int m_iMaxBarracksSpaceProvided; // WTP, ray, new Barracks System - START
	int m_iSpecialBuildingType;
	BuildingTypes m_eIndexOf_NextBuildingType_In_SpecialBuilding;
	int m_iConquestProbability;
	int m_iHealRateChange;
	int m_iDefenseModifier;
	int m_iBombardDefenseModifier;
	int m_iMissionType;
	int m_iStorageLossSellPercentage;
	bool m_bIgnoresBoycott;
	bool m_bUnlocksStorageLossTradeSettings;
	int m_iSpecialBuildingPriority;
	float m_fVisibilityPriority;
	bool m_bWorksWater;
	bool m_bWater;
	bool m_bRiver;
	bool m_bCapital;
	bool m_bNationalWonder; // R&R, ray, National Wonders
	bool m_bNeverCapture;
	bool m_bCenterInCity;
	int m_iDomesticMarketModifier;
	int m_iEntertainmentGoldModifier; // ray, Balancing of Entertainment Buildings in XML
	// R&R, Androrc, Domestic Market -- modified by Nightinggale - start
	InfoArray<YieldTypes, int> m_info_YieldDemands;
	// R&R, Androrc, Domestic Market -- modified by Nightinggale - end
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
	//WTP, Nightinggale - Terrain locator - start
	InfoArray<TerrainTypes> m_info_RequiredAdjacentTerrains;
	InfoArray<TerrainTypes> m_info_RequiredCatchmentAreaTerrains;
	InfoArray<FeatureTypes> m_info_RequiredCatchmentAreaFeatures;
	InfoArray<TerrainTypes> m_info_AIRequiredCatchmentAreaTerrains;
	InfoArray<FeatureTypes> m_info_AIRequiredCatchmentAreaFeatures;
	InfoArray<PlotTypes>    m_info_AIRequiredCatchmentAreaPlotTypes;
	InfoArray<UnitClassTypes, int> m_info_AIUnitClassWeight;
	//WTP, Nightinggale - Terrain locator - end
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
	friend class CvSpecialBuildingInfo;

	CvSpecialBuildingInfo();
	virtual ~CvSpecialBuildingInfo();
	bool isValid() const;
	int getChar() const;
	DllExport void setChar(int i);
	DllExport int getFontButtonIndex() const;

	// Arrays
	const InfoArray<BuildingTypes, int>& getBuildings() const;

	int getProductionTraits(int i) const;

	const char* getNatureObject() const;	// TAC - Nature Objects - koma13

	bool read(CvXMLLoadUtility* pXML);

	static void postXmlReadSetup();
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	bool m_bValid;
	int m_iChar;
	int m_iFontButtonIndex;
	// Arrays
	InfoArray<BuildingTypes, int> m_buildings;
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
	CvBuildingClassInfo();
	virtual ~CvBuildingClassInfo();
	int getDefaultBuildingIndex() const;
	void setDefaultBuildingIndex(int i);

	// Arrays
	int getVictoryThreshold(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	bool readPass3();
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
	CvRiverModelInfo();
	virtual ~CvRiverModelInfo();
	DllExport const char* getModelFile() const;
	void setModelFile(const char* szVal);				// The model filename
	DllExport const char* getBorderFile() const;
	void setBorderFile(const char* szVal);				// The model filename
	DllExport int getTextureIndex() const;
	DllExport const char* getDeltaString() const;
	DllExport const char* getConnectString() const;
	DllExport const char* getRotateString() const;

	bool read(CvXMLLoadUtility* pXML);
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
	CvRouteModelInfo();
	virtual ~CvRouteModelInfo();
	DllExport RouteTypes getRouteType() const;				// The route type
	DllExport const char* getModelFile() const;
	void setModelFile(const char* szVal);				// The model filename
	DllExport const char* getConnectString() const;
	DllExport const char* getModelConnectString() const;
	DllExport const char* getRotateString() const;

	bool read(CvXMLLoadUtility* pXML);
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
	virtual ~CvCivilizationInfo();
	virtual void reset();
	DllExport int getDefaultPlayerColor() const;
	int getArtStyleType() const;
	//Androrc UnitArtStyles
	int getUnitArtStyleType() const;         // Expose to Python
	//Androrc End
	int getNumCityNames() const;
	// TAC - Great General Names - Ray - START
	int getNumGeneralNames() const;
	// TAC - Great General Names - Ray - END
	// R&R, ray, Great Admirals - START
	int getNumAdmiralNames() const;
	// R&R, ray, Great Admirals - END
	// TAC - Ship Names - Ray - START
	int getNumShipNames() const;
	// TAC - Ship Names - Ray - END
	int getNumLeaders() const;				 // the number of leaders the Civ has, this is needed so that random leaders can be generated easily
	DllExport int getSelectionSoundScriptId() const;
	int getActionSoundScriptId() const;
	int getAdvancedStartPoints() const;
	int getAreaMultiplier() const;
	int getDensityMultiplier() const;
	int getTreasure() const;
	int getFavoredTerrain() const;
	int getCapturedCityUnitClass() const;
	ProfessionTypes getDefaultProfession() const;
	DllExport int getMissionaryChar() const;
	DllExport void setMissionaryChar(int iChar);
	int getChar() const;
	int getTradingPostChar() const;
	void setChar(int iChar);

	DllExport bool isAIPlayable() const;
	DllExport bool isPlayable() const;
	bool isWaterStart() const;
	DllExport bool isOpenBorders() const;
	bool isWaterWorks() const;
	DllExport bool isEurope() const;
	DllExport bool isNative() const;

	// R&R, ray, Correct Geographical Placement of Natives - START
	bool isNorthAmericanNative() const;
	bool isSouthAmericanNative() const;
	bool isCentralAmericanNative() const;
	// R&R, ray, Correct Geographical Placement of Natives - END

	CivCategoryTypes getCivCategoryTypes() const;

	inline CivEffectTypes getCivEffect() const { return m_eCivEffect; }

	std::wstring pyGetShortDescription(uint uiForm) { return getShortDescription(uiForm); }
	DllExport const wchar* getShortDescription(uint uiForm = 0);
	const wchar* getShortDescriptionKey() const;
	std::wstring pyGetShortDescriptionKey() { return getShortDescriptionKey(); }

	std::wstring pyGetAdjective(uint uiForm) { return getAdjective(uiForm);  }
	DllExport const wchar* getAdjective(uint uiForm = 0);
	const wchar* getAdjectiveKey() const;
	std::wstring pyGetAdjectiveKey() { return getAdjectiveKey(); }

	DllExport const char* getFlagTexture() const;
	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szVal);
	// Arrays
	DllExport int getCivilizationBuildings(int i) const;
	DllExport int getCivilizationUnits(int i) const;
	int getCivilizationProfessions(int i) const;
	int getNumCivilizationFreeUnits() const;
	int getCivilizationFreeUnitsClass(int index) const;
	int getCivilizationFreeUnitsProfession(int index) const;
	int getCivilizationInitialCivics(int i) const;
	int getFreeYields(int i) const;
	int getTeachUnitClassWeight(int i) const;

	template<typename Ta, typename Tb> Ta getCivSpecificForClass(Tb eVar) const;

	DllExport bool isLeaders(int i) const;
	bool isCivilizationFreeBuildingClass(int i) const;
	bool isValidProfession(int i) const;
	bool hasTrait(int i) const;

	CvWString getCityNames(int i) const;

	// TAC - Great General Names - Ray - START
	CvWString getGeneralNames(int i) const;
	// TAC - Great General Names - Ray - END

	// R&R, ray, Great Admirals - START
	CvWString getAdmiralNames(int i) const;
	// R&R, ray, Great Admirals - END

	// TAC - Ship Names - Ray - START
	CvWString getShipNames(int i) const;
	// TAC - Ship Names - Ray - END

	DllExport const CvArtInfoCivilization* getArtInfo() const;
	const char* getButton() const;
	DllExport int getDerivativeCiv() const;
	void setDerivativeCiv(int iCiv);
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);

	bool postLoadSetup();

	// EXE/python access functions
	int PY_getDefaultProfession() const;

	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iDefaultPlayerColor;
	int m_iArtStyleType;

	//Androrc UnitArtStyles
	int m_iUnitArtStyleType;  // FlavorUnits by Impaler[WrG]
	//Androrc End

	// TAC - Great General Names - Ray - START
	int m_iNumGeneralNames;
	// TAC - Great General Names - Ray - END

	// R&R, ray, Great Admirals - START
	int m_iNumAdmiralNames;
	// R&R, ray, Great Admirals - END

	// TAC - Ship Names - Ray - Start
	int m_iNumShipNames;
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
	ProfessionTypes m_eDefaultProfession;
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

	CivCategoryTypes m_eCivCategory;

	CivEffectTypes m_eCivEffect;

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
	CvVictoryInfo();
	virtual ~CvVictoryInfo();

	// PatchMod: Victorys START
protected:
	int m_iTotalProductionRate;
public:
	int getTotalProductionRate() const;
	void setTotalProductionRate(int iValue);
	// PatchMod: Victorys END

	int getPopulationPercentLead() const;
	int getLandPercent() const;
	int getMinLandPercent() const;
	int getCityCulture() const;
	int getNumCultureCities() const;
	int getTotalCultureRatio() const;

	DllExport bool getDefault() const;
	bool isTargetScore() const;
	bool isEndEurope() const;
	bool isEndScore() const;
	bool isConquest() const;
	DllExport bool isPermanent() const;
	bool isRevolution() const;

	const char* getMovie() const;
	bool read(CvXMLLoadUtility* pXML);
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

template<typename Ta, typename Tb>
inline Ta CvCivilizationInfo::getCivSpecificForClass(Tb eVar) const
{
	// WARNING
	// do not access member data from this call
	// InfoArray calls this while the this pointer is NULL
	// As such treat it as static even though it isn't due to the specialized functions
	BOOST_STATIC_ASSERT((boost::is_same<Ta, Tb>::value));
	return eVar;
}

template<>
inline BuildingTypes CvCivilizationInfo::getCivSpecificForClass(BuildingClassTypes eVar) const
{
	FAssertMsg(this != NULL, "InfoArray: BuildingClass->Unit conversion done on a NULL civ pointer");
	return (BuildingTypes)getCivilizationBuildings(eVar);
}

template<>
inline UnitTypes CvCivilizationInfo::getCivSpecificForClass(UnitClassTypes eVar) const
{
	FAssertMsg(this != NULL, "InfoArray: UnitClass->Unit conversion done on a NULL civ pointer");
	return (UnitTypes)getCivilizationUnits(eVar);
}

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
		CvHurryInfo();
		virtual ~CvHurryInfo();
		int getGoldPerProduction() const;
		int getProductionPerPopulation() const;
		int getGoldPerCross() const;
		int getYieldCostEuropePercent() const;
		YieldTypes getProductionYieldConsumed() const;
		int getProductionYieldPercent() const;
		int getFlatGold() const;
		bool isStarting() const;
		bool isCity() const;

		bool read(CvXMLLoadUtility* pXML);

		int PYgetProductionYieldConsumed() const;
	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
	protected:
		int m_iGoldPerProduction;
		int m_iProductionPerPopulation;
		int m_iGoldPerCross;
		int m_iYieldCostEuropePercent;
		YieldTypes m_eProductionYieldConsumed;
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
	virtual ~CvHandicapInfo();

	// TAC - AI More Immigrants - koma13 - START
protected:
	int m_iAIImmigration;
	int m_iAIMaxTaxrate;
public:
	int getAIImmigration() const;
	int getAIMaxTaxrate() const;
	// TAC - AI More Immigrants - koma13 - END

	// TAC: ray Starting Location Start
	int getStartingLocationPercent() const;
	// TAC: ray Starting Location End

	int getAdvancedStartPointsMod() const;
	int getStartingGold() const;
	int getFatherPercent() const;
	int getAttitudeChange() const;

	int getStartingDefenseUnits() const;
	int getStartingWorkerUnits() const;
	int getStartingExploreUnits() const;
	int getAIStartingUnitMultiplier() const;
	int getAIStartingDefenseUnits() const;
	int getAIStartingWorkerUnits() const;
	int getAIStartingExploreUnits() const;
	int getAIDeclareWarProb() const;
	int getAIWorkRateModifier() const;
	int getAINativeCombatModifier() const;
	int getAIKingCombatModifier() const;
	int getAIRebelModifier() const;
	int getAIGrowthPercent() const;
	int getAITrainPercent() const;
	int getAIConstructPercent() const;
	int getAIUnitUpgradePercent() const;
	int getAIHurryPercent() const;
	int getAIExtraTradePercent() const;
	int getAIPerEraModifier() const;
	int getAIAdvancedStartPercent() const;
	int getAIKingUnitThresholdPercent() const;
	// < JAnimals Mod Start >
	int getAIAnimalLandMaxPercent() const;
	int getAIAnimalSeaMaxPercent() const;
	int getAIAnimalLandNumTurnsNoSpawn() const;
	int getAIAnimalSeaNumTurnsNoSpawn() const;
	// < JAnimals Mod End >
	int getNumGoodies() const;
	int getEuropePriceThresholdMultiplier() const;
	int getNativePacifismPercent() const;
	int getMissionFailureThresholdPercent() const;
	int getKingNumUnitMultiplier() const;
	int getKingGoldThresholdPercent() const;

	// TAC - AI Revolution - koma13 - START
	int getWaveTurns() const;
	int getNumWaves();
	int getWaves(int index);
	// TAC - AI Revolution - koma13 - END

	int getOppressometerGrowthHandicap() const;

	// Arrays
	int getGoodies(int i) const;

	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	bool read(CvXMLLoadUtility* pXML);
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

	int m_iOppressometerGrowthHandicap;

	CvString m_szHandicapName;

	// TAC - AI Revolution - koma13 - START
	int m_iWaveTurns;
	std::vector<int> m_aWaves;
	// TAC - AI Revolution - koma13 - END

	// Arrays
	int* m_aiGoodies;

	//Storage loss trading related values
protected:
	int m_iAIMinimumStorageLossSellPercentage;
public:
	inline int getAIMinimumStorageLossSellPercentage() const					{ return m_iAIMinimumStorageLossSellPercentage; }

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
	CvGameSpeedInfo();
	virtual ~CvGameSpeedInfo();
	int getGrowthPercent() const;
	int getStoragePercent() const;
	int getTrainPercent() const;
	int getConstructPercent() const;
	int getFatherPercent() const;
	int getGreatGeneralPercent() const;
	int getRevolutionTurns() const;
	int getNumTurnIncrements() const;

	GameTurnInfo& getGameTurnInfo(int iIndex) const;
	void allocateGameTurnInfos(const int iSize);
	bool read(CvXMLLoadUtility* pXML);
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
	CvTurnTimerInfo();
	virtual ~CvTurnTimerInfo();
	int getBaseTime() const;
	int getCityBonus() const;
	int getUnitBonus() const;
	int getFirstTurnMultiplier() const;

	bool read(CvXMLLoadUtility* pXML);
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
	CvBuildInfo();
	virtual ~CvBuildInfo();
	int getTime() const;
	int getCost() const;
	int getImprovement() const;
	int getPrereqTerrain() const; // R&R, ray, Terraforming Features
	int getResultTerrain() const; // R&R, ray, Terraforming Features
	int getResultFeature() const; // R&R, ray, Terraforming Features
	int getRoute() const;
	DllExport int getEntityEvent() const;
	DllExport int getMissionType() const;
	void setMissionType(int iNewType);
	bool isKill() const;
	bool isRoute() const;

	// Arrays
	int getFeatureTime(int i) const;
	int getFeatureYield(int iFeature, int iYield) const;

	bool isFeatureRemove(int i) const;

	bool read(CvXMLLoadUtility* pXML);
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
	CvGoodyInfo();
	virtual ~CvGoodyInfo();
	int getGold() const;
	int getGoldRand1() const;
	int getGoldRand2() const;
	int getImmigration() const; // R&R, ray, Goody Enhancement
	int getMapOffset() const;
	int getMapRange() const;
	int getMapProb() const;
	int getExperience() const;
	int getHealing() const;
	int getDamagePrereq() const;
	int getCityGoodyWeight() const;
	int getUnitClassType() const;
	int getTeachUnitClassType() const;

	bool isBad() const;
	bool isWar() const;
	bool isWaterGoody() const; // R&R, ray, Goodies on Water
	bool isUnique() const; // R&R, ray, Goody Enhancement
	// WTP, ray, Unit spawning Goodies and Goody Huts - START
	int getMinTurnValid() const;
	int getRandNumHostilesSpawned() const;
	bool isSpawnHostileUnitsAsXML() const;
	bool isSpawnHostileAnimals() const;
	bool isSpawnHostileNatives() const;
	bool isSpawnHostileCriminals() const;
	// WTP, ray, Unit spawning Goodies and Goody Huts - END
	int getGoodyWeight(int iGoody) const;

	const char* getSound() const;
	void setSound(const char* szVal);
	const char* getAnnounceTextKey() const;
	const char* getChiefTextKey() const;
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
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
	// WTP, ray, Unit spawning Goodies and Goody Huts - START
	int m_iMinTurnValid;
	int m_iRandNumHostilesSpawned;
	bool m_bSpawnHostileUnitsAsXML;
	bool m_bSpawnHostileAnimals;
	bool m_bSpawnHostileNatives;
	bool m_bSpawnHostileCriminals;
	// WTP, ray, Unit spawning Goodies and Goody Huts - END
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
	CvRouteInfo();
	virtual ~CvRouteInfo();
	int getAdvancedStartCost() const;
	int getAdvancedStartCostIncrease() const;
	int getValue() const;
	int getMovementCost() const;
	int getFlatMovementCost() const;

	// Arrays
	int getYieldChange(int i) const;

	bool read(CvXMLLoadUtility* pXML);
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
	CvImprovementBonusInfo();
	virtual ~CvImprovementBonusInfo();
	int getDiscoverRand() const;

	bool isBonusMakesValid() const;

	int getYieldChange(int i) const;

	// Serialize
	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
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
	virtual ~CvImprovementInfo();
	int getAdvancedStartCost() const;
	int getAdvancedStartCostIncrease() const;
	int getTilesPerGoody() const;
	int getGoodyUniqueRange() const;
	int getFeatureGrowthProbability() const;
	int getUpgradeTime() const;
	int getDefenseModifier() const;
	int getFoodModifierForCity() const; // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	int getHammersModifierForCity() const; // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	int getToolsModifierForCity() const; // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	int getStorageModifierForCity() const; // WTP, ray, Improvements give Bonus to their City - PART 3 - START
	int getPillageGold() const;
	int getHealModifier() const;	// WTP, ray, Plot Heal Modifier for Improvements - START
	int getImprovementPillage() const;
	void setImprovementPillage(int i);
	int getImprovementUpgrade() const;
	void setImprovementUpgrade(int i);
	// Super Forts begin *XML*
	int getCulture() const;
	int getCultureRange() const;
	int getPlotVisibilityChange() const;
	int getUnitVisibilityChange() const;
	int getSeeFrom() const;
	int getUniqueRange() const;
	bool isBombardable() const;
	bool isUpgradeRequiresFortify() const;
	// Super Forts end
	bool isActsAsCity() const;
	bool isFort() const; // R&R, ray, Monasteries and Forts
	bool isMonastery() const; // R&R, ray, Monasteries and Forts
	bool isCanal() const; // WTP, ray, Canal
	bool isNotAllowedNextToSameAsItself() const; // WTP, ray, Not allowed next to itself - START
	bool isHillsMakesValid() const;
	bool isRiverSideMakesValid() const;
	bool isRequiresFlatlands() const;
	DllExport bool isRequiresRiverSide() const;
	bool isRequiresFeature() const;
	DllExport bool isWater() const;
	bool isGoody() const;
	bool isGoodyForSpawningUnits() const; //WTP, Unit only Goodies
	bool isGoodyForSpawningHostileAnimals() const; //WTP, Unit only Goodies
	bool isGoodyForSpawningHostileNatives() const; //WTP, Unit only Goodies
	bool isGoodyForSpawningHostileCriminals() const; //WTP, Unit only Goodies
	bool isPermanent() const;
	DllExport bool useLSystem() const;
	bool isOutsideBorders() const;

	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szVal);
	DllExport int getWorldSoundscapeScriptId() const;
	// Arrays
	int getPrereqNatureYield(int i) const;
	int* getPrereqNatureYieldArray();
	int getYieldIncrease(int i) const;
	int* getYieldIncreaseArray();
	int getRiverSideYieldChange(int i) const;
	int* getRiverSideYieldChangeArray();
	int getHillsYieldChange(int i) const;
	int* getHillsYieldChangeArray();
	bool getTerrainMakesValid(int i) const;
	bool getFeatureMakesValid(int i) const;

	int getRouteYieldChanges(int i, int j) const;
	int* getRouteYieldChangesArray(int i);				// For Moose - CvWidgetData XXX
	int getImprovementBonusYield(int i, int j) const;
	DllExport bool isImprovementBonusMakesValid(int i) const;
	int getImprovementBonusDiscoverRand(int i) const;

	// Other
	const char* getButton() const;
	DllExport const CvArtInfoImprovement* getArtInfo() const;
	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);


	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
	int m_iTilesPerGoody;
	int m_iGoodyUniqueRange;
	int m_iFeatureGrowthProbability;
	int m_iUpgradeTime;
	int m_iDefenseModifier;
	int m_iFoodModifierForCity;  // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	int m_iHammersModifierForCity;  // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	int m_iToolsModifierForCity;  // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	int m_iStorageModifierForCity;  // WTP, ray, Improvements give Bonus to their City - PART 3 - START
	int m_iPillageGold;
	int m_iHealModifier; // WTP, ray, Plot Heal Modifier for Improvements - START
	int m_iImprovementPillage;
	int m_iImprovementUpgrade;
	// Super Forts begin *XML*
	int m_iCulture;
	int m_iCultureRange;
	int m_iPlotVisibilityChange;
	int m_iUnitVisibilityChange;
	int m_iSeeFrom;
	int m_iUniqueRange;
	bool m_bBombardable;
	bool m_bUpgradeRequiresFortify;
	// Super Forts end
	bool m_bActsAsCity;
	bool m_bFort; // R&R, ray, Monasteries and Forts
	bool m_bMonastery; // R&R, ray, Monasteries and Forts
	bool m_bCanal; // WTP, ray, Canal - START
	bool m_bNotAllowedNextToSameAsItself; // WTP, ray, Not allowed next to itself - START
	bool m_bHillsMakesValid;
	bool m_bRiverSideMakesValid;
	bool m_bRequiresFlatlands;
	bool m_bRequiresRiverSide;
	bool m_bRequiresFeature;
	bool m_bWater;
	bool m_bGoody;
	bool m_bGoodyForSpawningUnits; //WTP, Unit only Goodies
	bool m_bGoodyForSpawningHostileAnimals; //WTP, Unit only Goodies
	bool m_bGoodyForSpawningHostileNatives; //WTP, Unit only Goodies
	bool m_bGoodyForSpawningHostileCriminals; //WTP, Unit only Goodies
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
	virtual ~CvBonusInfo();
	DllExport int getChar() const;
	DllExport void setChar(int i);
	int getHealthEffectFromRessource() const; // WTP, ray, Health from specific Bonus Ressources
	int getAIObjective() const;
	int getMinAreaSize() const;
	int getMinLatitude() const;
	int getMaxLatitude() const;
	int getPlacementOrder() const;
	int getConstAppearance() const;
	int getRandAppearance1() const;
	int getRandAppearance2() const;
	int getRandAppearance3() const;
	int getRandAppearance4() const;
	int getPercentPerPlayer() const;
	int getTilesPer() const;
	int getMinLandPercent() const;
	int getUniqueRange() const;
	int getGroupRange() const;
	int getGroupRand() const;

	bool isOneArea() const;
	// R&R, ray, Bonus on Peaks - START
	bool isPeaks() const;
	// R&R, ray, Bonus on Peaks - END
	bool isHills() const;
	//TAC Whaling, ray
	bool isOcean() const;
	bool isWhalingboatWorkable() const;
	//End TAC Whaling, ray
	bool isFishingboatWorkable() const; // R&R, ray, High Sea Fishing
	bool isFlatlands() const;
	bool isNoRiverSide() const;
	bool isRiverSideOnly() const; // Ray, adding 2 more XML tags to control bonus placement
	bool isCoastalLandOnly() const; // Ray, adding 2 more XML tags to control bonus placement
	bool isOnlySouthernHemisphere() const;
	bool isOnlyNorthernHemisphere() const;
	DllExport bool useLSystem() const;
	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szVal);
	//TAC Whaling, ray
	int getNumYieldChanges() const;
	std::vector<int> getYieldChangesArray();
	//End TAC Whaling, ray
	// Arrays
	int getYieldChange(int i) const;
	int* getYieldChangeArray();
	int getImprovementChange(int i) const;
	DllExport bool isTerrain(int i) const;
	DllExport bool isFeature(int i) const;
	bool isFeatureTerrain(int i) const;

	// Other
	const char* getButton() const;
	DllExport const CvArtInfoBonus* getArtInfo() const;
	int getBuilding() const;

	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
protected:
	int m_iChar;
	int m_iHealthEffectFromRessource; // WTP, ray, Health from specific Bonus Ressources
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
	bool m_bRiverSideOnly; // Ray, adding 2 more XML tags to control bonus placement
	bool m_bCoastalLandOnly; // Ray, adding 2 more XML tags to control bonus placement
	bool m_bOnlySouthernHemisphere; //ray, Norther and Southern Hemisphere, using hint of f1rpo
	bool m_bOnlyNorthernHemisphere; //ray, Norther and Southern Hemisphere, using hint of f1rpo
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
	CvFeatureInfo();
	virtual ~CvFeatureInfo();
	int getMovementCost() const;
	int getSeeThroughChange() const;
	int getAppearanceProbability() const;
	int getDisappearanceProbability() const;
	int getGrowthProbability() const;
	int getDefenseModifier() const;
	int getAdvancedStartRemoveCost() const;
	// R&R, Robert Surcouf, Damage on Storm plots, Start
	int getTurnDamage() const;
	bool isGeneratedEveryRound() const;
	// R&R, Robert Surcouf, Damage on Storm plots, End
	bool isNoCoast() const;
	bool isOnlyCoastalLand() const; //WTP, Feature settings enhancements
	bool isNoRiver() const;
	bool isNoAdjacent() const;
	bool isRequiresFlatlands() const;
	bool isRequiresHills() const; //WTP, Feature settings enhancements
	bool isRequiresPeaks() const; //WTP, Feature settings enhancements
	bool isRequiresRiver() const;
	bool isImpassable() const;

	// ray, Streams Feature - START
	bool isNorthMovementBonus() const;
	bool isSouthMovementBonus() const;
	bool isEastMovementBonus() const;
	bool isWestMovementBonus() const;
	bool isNorthEastMovementBonus() const;
	bool isNorthWestMovementBonus() const;
	bool isSouthEastMovementBonus() const;
	bool isSouthWestMovementBonus() const;
	// ray, Streams Feature - END

	bool isNoCity() const;
	bool isNoImprovement() const;
	bool isVisibleAlways() const;
	const char* getOnUnitChangeTo() const;
	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szTag);

	DllExport int getWorldSoundscapeScriptId() const;
	EffectTypes getEffectType() const;
	int getEffectProbability() const;
	// Arrays
	int getYieldChange(int i) const;
	int getRiverYieldIncrease(int i) const;
	DllExport int get3DAudioScriptFootstepIndex(int i) const;
	bool isTerrain(int i) const;
	int getNumVarieties() const;
	// Other
	DllExport const CvArtInfoFeature* getArtInfo() const;
	const char* getButton() const;
	bool read(CvXMLLoadUtility* pXML);
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
	bool m_bOnlyCoastalLand; //WTP, Feature settings enhancements
	bool m_bNoRiver;
	bool m_bNoAdjacent;
	bool m_bRequiresFlatlands;
	bool m_bRequiresHills; //WTP, Feature settings enhancements
	bool m_bRequiresPeaks; //WTP, Feature settings enhancements
	bool m_bRequiresRiver;
	bool m_bImpassable;

	// ray, Streams Feature - START
	bool m_bNorthMovementBonus;
	bool m_bSouthMovementBonus;
	bool m_bEastMovementBonus;
	bool m_bWestMovementBonus;
	bool m_bNorthEastMovementBonus;
	bool m_bNorthWestMovementBonus;
	bool m_bSouthEastMovementBonus;
	bool m_bSouthWestMovementBonus;
	// ray, Streams Feature - END

	bool m_bNoCity;
	bool m_bNoImprovement;
	bool m_bVisibleAlways;
	CvString m_szOnUnitChangeTo;
	int m_iWorldSoundscapeScriptId;
	EffectTypes m_eEffectType;
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
	CvYieldInfo();
	virtual ~CvYieldInfo();
	YieldTypes getIndex() const { return m_eIndex; }
	DllExport int getChar() const;
	DllExport void setChar(int i);
	const char* getIcon() const;
	WidgetTypes getWikiWidget() const;
	YieldTypes getID() const;
// KJ Jansson addon for Multiple Professions per Building modcomp by Androrc the Orc START
	const char* getCombiIcon() const;
// KJ Jansson addon for Multiple Professions per Building modcomp by Androrc the Orc END
	const char* getHighlightIcon() const;
	int getBuyPriceLow() const;
	int getBuyPriceHigh() const;
	// TAC - Price Limits - Ray - START
	int getMinimumBuyPrice() const;
	// TAC - Price Limits - Ray - END
	int getSellPriceDifference() const;
	int getPriceChangeThreshold() const;
	int getPriceCorrectionPercent() const;
	// R&R, Androrc Price Recovery
	int getEuropeVolumeAttrition() const;
	//Androrc End
	// R&R, ray, Africa
	int getAfricaBuyPriceHigh() const;
	int getAfricaBuyPriceLow() const;
	int getAfricaSellPriceDifference() const;
	// R&R, ray, Africa - END
	// R&R, ray, Port Royal
	int getPortRoyalBuyPriceHigh() const;
	int getPortRoyalBuyPriceLow() const;
	int getPortRoyalSellPriceDifference() const;
	// R&R, ray, Port Royal - END
	int getNativeBuyPrice() const;
	int getNativeSellPrice() const;
	int getNativeConsumptionPercent() const;
	int getNativeHappy() const;
	int getHillsChange() const;
	int getPeakChange() const;
	int getLakeChange() const;
	int getCityChange() const;
	int getMinCity() const;
	int getAIWeightPercent() const;
	int getAIBaseValue() const;
	int getNativeBaseValue() const;
	DllExport int getColorType() const;
	int getUnitClass() const;
	DllExport int getTextureIndex() const;
	DllExport int getWaterTextureIndex() const;
	int getPowerValue() const;
	int getAssetValue() const;

	bool isCargo() const;
	bool isIgnoredForStorageCapacity() const; // ray, making special storage capacity rules for Yields XML configurable

	bool isExportYield() const {return m_bIsExportYield;} // auto traderoute - Nightinggale

	// R&R, Androrc, Livestock Breeding
	bool isLivestock() const;
	// R&R, Androrc, Livestock Breeding, END

	bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	YieldTypes m_eIndex;
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
	bool m_bIgnoredForStorageCapacity; // ray, making special storage capacity rules for Yields XML configurable
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
	CvTerrainInfo();
	virtual ~CvTerrainInfo();
	int getMovementCost() const;
	int getSeeFromLevel() const;
	int getSeeThroughLevel() const;
	int getBuildModifier() const;
	int getDefenseModifier() const;

	DllExport bool isWater() const;
	bool isImpassable() const;
	bool isFound() const;
	bool isFoundCoast() const;
	bool isBadCityLocation() const; // WTP, ray, Health Overhaul

	DllExport const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szTag);

	DllExport int getWorldSoundscapeScriptId() const;
	// Arrays
	int getYield(int i) const;
	int getRiverYieldIncrease(int i) const;
	DllExport int get3DAudioScriptFootstepIndex(int i) const;
	// Other
	DllExport const CvArtInfoTerrain* getArtInfo() const;
	const char* getButton() const;

	bool canHavePlotType(PlotTypes ePlotType) const;

	bool read(CvXMLLoadUtility* pXML);
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
	bool m_bBadCityLocation; // WTP, ray, Health Overhaul
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
	CvInterfaceModeInfo();
	virtual ~CvInterfaceModeInfo();
	DllExport int getCursorIndex() const;
	DllExport int getMissionType() const;
	bool getVisible() const;
	DllExport bool getGotoPlot() const;
	DllExport bool getHighlightPlot() const;
	bool getSelectType() const;
	bool getSelectAll() const;
	bool read(CvXMLLoadUtility* pXML);
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
	virtual ~CvLeaderHeadInfo();
	int getAlarmType() const;
	int getBaseAttitude() const;
	int getNativeAttitude() const;
	//TAC Native Mercs
	int getMinAttitudeGiveMercs() const;
	int getBaseChanceGiveMercs() const;
	//END TAC Native Mercs
	// R&R, ray, Native Slave, START
	int getMinAttitudeGiveNativeSlaves() const;
	int getBaseChanceGiveNativeSlaves() const;
	// R&R, ray, Native Slave, END
	int getRefuseToTalkWarThreshold() const;
	int getMaxGoldTradePercent() const;
	int getMaxWarRand() const;
	int getMaxWarNearbyPowerRatio() const;
	int getMaxWarDistantPowerRatio() const;
	int getMaxWarMinAdjacentLandPercent() const;
	int getLimitedWarRand() const;
	int getLimitedWarPowerRatio() const;
	int getDogpileWarRand() const;
	int getMakePeaceRand() const;
	int getDeclareWarTradeRand() const;
	int getDemandRebukedSneakProb() const;
	int getDemandRebukedWarProb() const;
	int getRazeCityProb() const;
	int getBaseAttackOddsChange() const;
	int getAttackOddsChangeRand() const;
	int getCloseBordersAttitudeChange() const;
	int getAlarmAttitudeChange() const;
	int getLostWarAttitudeChange() const;
	int getRebelAttitudeDivisor() const;
	int getAtWarAttitudeDivisor() const;
	int getAtWarAttitudeChangeLimit() const;
	int getAtPeaceAttitudeDivisor() const;
	int getAtPeaceAttitudeChangeLimit() const;
	int getOpenBordersAttitudeDivisor() const;
	int getOpenBordersAttitudeChangeLimit() const;
	int getDefensivePactAttitudeDivisor() const;
	int getDefensivePactAttitudeChangeLimit() const;
	int getShareWarAttitudeChange() const;
	int getShareWarAttitudeDivisor() const;
	int getShareWarAttitudeChangeLimit() const;
	int getDemandTributeAttitudeThreshold() const;
	int getNoGiveHelpAttitudeThreshold() const;
	int getMapRefuseAttitudeThreshold() const;
	int getDeclareWarRefuseAttitudeThreshold() const;
	int getDeclareWarThemRefuseAttitudeThreshold() const;
	int getStopTradingRefuseAttitudeThreshold() const;
	int getStopTradingThemRefuseAttitudeThreshold() const;
	int getOpenBordersRefuseAttitudeThreshold() const;
	int getDefensivePactRefuseAttitudeThreshold() const;
	int getPermanentAllianceRefuseAttitudeThreshold() const;
	// R&R, ray, Natives raiding party - START
	int getNativeRaidAttitudeThreshold() const;
	// R&R, ray, Natives raiding party - END

	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szVal);
	// Arrays
	bool hasTrait(int i) const;

	int getContactRand(int i) const;
	int getContactDelay(int i) const;
	int getMemoryDecayRand(int i) const;
	int getMemoryAttitudePercent(int i) const;
	int getNoWarAttitudeProb(int i) const;
	int getUnitAIWeightModifier(int i) const;
	int getImprovementWeightModifier(int i) const;
	DllExport int getDiploPeaceMusicScriptIds(int i) const;
	DllExport int getDiploWarMusicScriptIds(int i) const;
	// Other
	DllExport const CvArtInfoLeaderhead* getArtInfo() const;
	const char* getLeaderHead() const;
	const char* getButton() const;
	void write(FDataStreamBase* stream);
	void read(FDataStreamBase* stream);
	bool read(CvXMLLoadUtility* pXML);
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
	CvWorldInfo();
	virtual ~CvWorldInfo();
	DllExport int getDefaultPlayers() const;
	DllExport int getDefaultNativePlayers() const;
	int getUnitNameModifier() const;
	int getTargetNumCities() const;
	int getBuildingClassPrereqModifier() const;
	int getGridWidth() const;
	int getGridHeight() const;
	int getTerrainGrainChange() const;
	int getFeatureGrainChange() const;
	int getFatherPercent() const;
	int getAdvancedStartPointsMod() const;
	int getAIImmigrationModifier() const;	// TAC - AI More Immigrants - koma13

	bool read(CvXMLLoadUtility* pXML);
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
	CvClimateInfo();
	virtual ~CvClimateInfo();
	int getDesertPercentChange() const;
	int getJungleLatitude() const;
	int getHillRange() const;
	int getPeakPercent() const;

	float getSnowLatitudeChange() const;
	float getTundraLatitudeChange() const;
	float getGrassLatitudeChange() const;
	float getDesertBottomLatitudeChange() const;
	float getDesertTopLatitudeChange() const;
	float getRandIceLatitude() const;

	bool read(CvXMLLoadUtility* pXML);
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
	CvSeaLevelInfo();
	virtual ~CvSeaLevelInfo();
	int getSeaLevelChange() const;

	bool read(CvXMLLoadUtility* pXML);
protected:
	int m_iSeaLevelChange;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvEuropeInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEuropeInfo : public CvInfoBase
{
public:
	CvEuropeInfo();
	virtual ~CvEuropeInfo();

	bool isStart() const;
	int getCardinalDirection() const;
	int getTripLength() const;
	int getMinLandDistance() const;
	int getWidthPercent() const;

	bool read(CvXMLLoadUtility* pXML);

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
	virtual ~CvTraitInfo();

	inline CivEffectTypes getCivEffect() const { return m_eCivEffect; }

	int getLevelExperienceModifier() const;
	int getGreatGeneralRateModifier() const;
	int getDomesticGreatGeneralRateModifier() const;
	int getNativeAngerModifier() const;
	int getLearnTimeModifier() const;
	int getNativeCombatModifier() const;
	int getMissionaryModifier() const;
	int getNativeTradeModifier() const; // R&R, ray, new Attribute in Traits
	int getRebelCombatModifier() const;
	int getTaxRateThresholdModifier() const;
	int getMaxTaxRateThresholdDecrease() const; // R&R, ray, new Attribute in Traits
	int getMercantileFactor() const;
	int getAfricaSellProfitModifierInPercent() const; // WTP, Africa and Port Royal Profit Modifiers - START
	int getPortRoyalSellProfitModifierInPercent() const; // WTP, Africa and Port Royal Profit Modifiers - START
	int getDomesticMarketProfitModifierInPercent() const; // WTP, ray, Domestic Market Profit Modifier
	int getTreasureModifier() const;
	int getGoodUniqueGoodyChanceModifierLand() const; // WTP, ray, Unique Goody Chance Modifiers - START
	int getGoodUniqueGoodyChanceModifierWater() const; // WTP, ray, Unique Goody Chance Modifiers - START
	int getUnhappinessFromSlavesModifier() const; // WTP, ray, Happiness - START
	int getChiefGoldModifier() const;
	int getNativeAttitudeChange() const;
	int getEuropeanAttitudeChange() const; // R&R, ray, new Attribute in Traits
	int getKingAttitudeChange() const; // R&R, ray, new Attribute in Traits
	int getCityDefense() const;
	int getLandPriceDiscount() const;
	int getRecruitPriceDiscount() const;
	int getRecruitPriceDiscountAfrica() const; // WTP, ray, Recruit Price Discounts Africa and Port Royal
	int getRecruitPriceDiscountPortRoyal() const; // WTP, ray, Recruit Price Discounts Africa and Port Royal
	int getEuropeTravelTimeModifier() const;
	int getImmigrationThresholdModifier() const;
	int getPopGrowthThresholdModifier() const;		// Schmiddie, 7 new variables for traits for Europeans, START
	int getCultureLevelModifier() const;
	int getPioneerSpeedModifier() const;
	int getImprovementPriceModifier() const;
	int getImprovementGrowthTimeModifier() const; // WTP, ray, Improvement Growth Modifier
	int getLearningByDoingFreeModifier() const; // WTP, ray, adding modifiers for other LBD features - START
	int getLearningByDoingRunawayModifier() const; // WTP, ray, adding modifiers for other LBD features - START
	int getLearningByDoingRevoltModifier() const; // WTP, ray, adding modifiers for other LBD features - START
	int getSpecialistPriceModifier() const;
	int getStorageCapacityModifier() const;		// Schmiddie, 7 new variables for traits for Europeans, END

	const char* getShortDescription() const;
	void setShortDescription(const char* szVal);

	// Arrays
	int getCityExtraYield(int i) const;
	int getExtraYieldThreshold(int i) const;
	int getProfessionEquipmentModifier(int i) const;
	int isFreePromotion(int i) const;
	int isFreePromotionUnitCombat(int i) const;
	int getYieldModifier(int iYield) const;
	int getGoodyFactor(int iGoody) const;
	int getBuildingProductionModifier(int iBuildingClass) const;
	int getBuildingRequiredYieldModifier(int iYield) const;
	const int* getBuildingRequiredYieldModifierArray() const;
	int getUnitMoveChange(int iUnitClass) const;
	int getUnitStrengthModifier(int iUnitClass) const;
	int getProfessionMoveChange(int iProfession) const;
	bool isTaxYieldModifier(int i) const;
	bool isFreeBuildingClass(int i) const;
	int getBuildingYieldChange(int iBuildingClass, int iYieldType) const;

	void read(FDataStreamBase* );
	void write(FDataStreamBase* );
	bool read(CvXMLLoadUtility* pXML);

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
	int m_iAfricaSellProfitModifierInPercent; // WTP, Africa and Port Royal Profit Modifiers - START
	int m_iPortRoyalSellProfitModifierInPercent; // WTP, Africa and Port Royal Profit Modifiers - START
	int m_iDomesticMarketProfitModifierInPercent; // WTP, ray, Domestic Market Profit Modifier
	int m_iTreasureModifier;
	int m_iGoodUniqueGoodyChanceModifierLand; // WTP, ray, Unique Goody Chance Modifiers - START
	int m_iGoodUniqueGoodyChanceModifierWater; // WTP, ray, Unique Goody Chance Modifiers - START
	int m_iUnhappinessFromSlavesModifier; // WTP, ray, Happiness - START
	int m_iChiefGoldModifier;
	int m_iNativeAttitudeChange;
	int m_iEuropeanAttitudeChange; // R&R, ray, new Attribute in Traits
	int m_iKingAttitudeChange; // R&R, ray, new Attribute in Traits
	int m_iCityDefense;
	int m_iLandPriceDiscount;
	int m_iRecruitPriceDiscount;
	int m_iRecruitPriceDiscountAfrica; // WTP, ray, Recruit Price Discounts Africa and Port Royal
	int m_iRecruitPriceDiscountPortRoyal; // WTP, ray, Recruit Price Discounts Africa and Port Royal
	int m_iEuropeTravelTimeModifier;
	int m_iImmigrationThresholdModifier;
	int m_iPopGrowthThresholdModifier;		// Schmiddie, 7 new variables for traits for Europeans, START
	int m_iCultureLevelModifier;
	int m_iPioneerSpeedModifier;
	int m_iImprovementPriceModifier;
	int m_iImprovementGrowthTimeModifier;  // WTP, ray, Improvement Growth Modifier
	int m_iLearningByDoingFreeModifier; // WTP, ray, adding modifiers for other LBD features - START
	int m_iLearningByDoingRunawayModifier; // WTP, ray, adding modifiers for other LBD features - START
	int m_iLearningByDoingRevoltModifier; // WTP, ray, adding modifiers for other LBD features - START
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
	CvCursorInfo();
	virtual ~CvCursorInfo();
	DllExport const char* getPath();
	void setPath(const char* szVal);

	bool read(CvXMLLoadUtility* pXML);
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
	CvSlideShowInfo();
	virtual ~CvSlideShowInfo();
	DllExport const char* getPath();
	void setPath(const char* szVal);
	DllExport const char* getTransitionType();
	void setTransitionType(const char* szVal);
	DllExport float getStartTime();
	void setStartTime(float fVal);

	bool read(CvXMLLoadUtility* pXML);
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
	CvSlideShowRandomInfo();
	virtual ~CvSlideShowRandomInfo();
	DllExport const char* getPath();
	void setPath(const char* szVal);
	bool read(CvXMLLoadUtility* pXML);
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
	CvWorldPickerInfo();
	virtual ~CvWorldPickerInfo();
	DllExport const char* getMapName();
	void setMapName(const char* szVal);
	DllExport const char* getModelFile();
	void setModelFile(const char* szVal);
	DllExport int getNumSizes();
	DllExport float getSize(int index);
	DllExport int getNumClimates();
	DllExport const char* getClimatePath(int index);
	DllExport int getNumWaterLevelDecals();
	DllExport const char* getWaterLevelDecalPath(int index);
	DllExport int getNumWaterLevelGloss();
	DllExport const char* getWaterLevelGlossPath(int index);
	bool read(CvXMLLoadUtility* pXML);
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
		CvAnimationPathInfo();
		virtual ~CvAnimationPathInfo();
		DllExport int getPathCategory( int i );
		float getPathParameter( int i );
		DllExport int getNumPathDefinitions();
		DllExport CvAnimationPathDefinition * getPath( );
		DllExport bool isMissionPath() const;
		bool read(CvXMLLoadUtility* pXML);
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
		CvAnimationCategoryInfo();
		virtual ~CvAnimationCategoryInfo();
		DllExport int getCategoryBaseID( );
		DllExport int getCategoryDefaultTo( );
		bool read(CvXMLLoadUtility* pXML);
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
		CvEntityEventInfo();
		virtual ~CvEntityEventInfo();
		bool read(CvXMLLoadUtility* pXML);
		DllExport AnimationPathTypes getAnimationPathType(int iIndex = 0) const;
		DllExport EffectTypes getEffectType(int iIndex = 0) const;
		int getAnimationPathCount() const;
		int getEffectTypeCount() const;
		bool getUpdateFormation() const;
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
	CvAssetInfoBase()  {}
	virtual ~CvAssetInfoBase() {}
	const char* getTag() const;
	void setTag(const char* szDesc);

	DllExport const char* getPath() const;
	void setPath(const char* szDesc);

	bool read(CvXMLLoadUtility* pXML);
	//---------------------------------------PROTECTED MEMBER VARIABLES---------------------------------
protected:
	CvString m_szPath;
};

class CvArtInfoAsset : 	public CvAssetInfoBase
{
public:
	CvArtInfoAsset() {}
	virtual ~CvArtInfoAsset() {}
	const char* getNIF() const;
	const char* getKFM() const;

	void setNIF(const char* szDesc);
	void setKFM(const char* szDesc);

	bool read(CvXMLLoadUtility* pXML);
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
	bool read(CvXMLLoadUtility* pXML);
};

// todoJS: Remove empty classes if additional items are not added
class CvArtInfoInterface : 	public CvArtInfoAsset
{
public:
	CvArtInfoInterface() {}
	virtual ~CvArtInfoInterface() {}
};

class CvArtInfoMisc : 	public CvArtInfoScalableAsset
{
public:
	CvArtInfoMisc() {}
	virtual ~CvArtInfoMisc() {}
};

class CvArtInfoMovie : 	public CvArtInfoAsset
{
public:
	CvArtInfoMovie() {}
	virtual ~CvArtInfoMovie() {}
};

class CvArtInfoUnit : public CvArtInfoScalableAsset
{
public:
	CvArtInfoUnit();
	virtual ~CvArtInfoUnit();
	const char* getFullLengthIcon() const;
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
	bool getSmoothMove() const;
	float getAngleInterpRate() const;
	DllExport float getBankRate() const;
	bool read(CvXMLLoadUtility* pXML);
	const char* getTrainSound() const;
	void setTrainSound(const char* szVal);
	DllExport int getRunLoopSoundTag() const;
	DllExport int getRunEndSoundTag() const;
	DllExport int getSelectionSoundScriptId() const;
	int getActionSoundScriptId() const;
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
	CvArtInfoBuilding();
	virtual ~CvArtInfoBuilding();
	bool isAnimated() const;
	const char* getCityTexture() const;
	const char* getCitySelectedTexture() const;
	DllExport const char* getLSystemName() const;
	bool read(CvXMLLoadUtility* pXML);
protected:
	bool m_bAnimated;
	CvString m_cityTexture;
	CvString m_citySelectedTexture;
	CvString m_szLSystemName;
};

class CvArtInfoCivilization : public CvArtInfoAsset
{
public:
	CvArtInfoCivilization();
	virtual ~CvArtInfoCivilization();
	DllExport int getFontButtonIndex() const;
	bool isWhiteFlag() const;
	bool isInvertFlag() const;

	bool read(CvXMLLoadUtility* pXML);
protected:
	int m_iFontButtonIndex;
	bool m_bWhiteFlag;
	bool m_bInvertFlag;
};

class CvArtInfoLeaderhead : public CvArtInfoAsset
{
public:
	CvArtInfoLeaderhead() {}
	virtual ~CvArtInfoLeaderhead() {}
	DllExport const char* getBackgroundKFM() const;
	void setBackgroundKFM( const char* szKFM);
	bool read(CvXMLLoadUtility* pXML);
protected:
	CvString m_szBackgroundKFM;
};

class CvArtInfoBonus : public CvArtInfoScalableAsset
{
public:
	CvArtInfoBonus();
	virtual ~CvArtInfoBonus() {}
	DllExport int getFontButtonIndex() const;
	DllExport bool isShadowCastor() const;
	DllExport bool isRefractionCastor() const;
	bool read(CvXMLLoadUtility* pXML);
protected:
	int m_iFontButtonIndex;
	bool m_bShadowCastor;
	bool m_bRefractionCastor;
};

class CvArtInfoImprovement : public CvArtInfoScalableAsset
{
public:
	CvArtInfoImprovement();
	virtual ~CvArtInfoImprovement();
	bool isExtraAnimations() const;

	bool read(CvXMLLoadUtility* pXML);
protected:
	bool m_bExtraAnimations;
};

typedef std::vector<std::pair<int, int> > CvTextureBlendSlotList;
class CvArtInfoTerrain : public CvArtInfoAsset
{
public:
	CvArtInfoTerrain();
	virtual ~CvArtInfoTerrain();
	DllExport const char* getBaseTexture();
	void setBaseTexture(const char* szTmp );
	DllExport const char* getGridTexture();
	void setGridTexture(const char* szTmp );
	DllExport const char* getDetailTexture();
	void setDetailTexture(const char* szTmp);
	DllExport int getLayerOrder() const;
	DllExport TerrainGroupTypes getTerrainGroup() const;
	DllExport CvTextureBlendSlotList &getBlendList(int blendMask);
	bool read(CvXMLLoadUtility* pXML);
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
	CvArtInfoFeature();
	virtual ~CvArtInfoFeature();
	DllExport bool isAnimated() const;
	DllExport TileArtTypes getTileArtType() const;
	bool read(CvXMLLoadUtility* pXML);
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
	CvEmphasizeInfo();
	virtual ~CvEmphasizeInfo();
	bool isAvoidGrowth() const;

	// Arrays
	int getYieldChange(int i) const;

	bool read(CvXMLLoadUtility* pXML);
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
	CvCultureLevelInfo();
	virtual ~CvCultureLevelInfo();
	int getThreshold() const;

	bool read(CvXMLLoadUtility* pXML);
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
	CvEraInfo();
	virtual ~CvEraInfo();
	int getGameTurn() const;
	int getStartingUnitMultiplier() const;
	int getStartingDefenseUnits() const;
	int getStartingWorkerUnits() const;
	int getStartingExploreUnits() const;
	int getAdvancedStartPoints() const;
	int getStartingGold() const;
	int getFreePopulation() const;
	int getStartPercent() const;
	int getGrowthPercent() const;
	int getTrainPercent() const;
	int getConstructPercent() const;
	int getFatherPercent() const;
	int getGreatGeneralPercent() const;
	int getEventChancePerTurn() const;
	DllExport int getSoundtrackSpace() const;
	DllExport int getNumSoundtracks() const;
	const char* getAudioUnitVictoryScript() const;
	const char* getAudioUnitDefeatScript() const;

	inline CivEffectTypes getCivEffect() const { return m_eCivEffect; }

	bool isRevolution() const;
	bool isNoGoodies() const;
	DllExport bool isFirstSoundtrackFirst() const;

	// Arrays
	DllExport int getSoundtracks(int i) const;
	DllExport int getCitySoundscapeSciptId(int i) const;

	bool read(CvXMLLoadUtility* pXML);
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
	CivEffectTypes m_eCivEffect;
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
	CvColorInfo();
	virtual ~CvColorInfo();
	DllExport const NiColorA& getColor() const;

	bool read(CvXMLLoadUtility* pXML);
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
	CvPlayerColorInfo();
	virtual ~CvPlayerColorInfo();
	DllExport int getColorTypePrimary() const;
	DllExport int getColorTypeSecondary() const;
	int getTextColorType() const;

	bool read(CvXMLLoadUtility* pXML);
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
		CvLandscapeInfo();
		virtual ~CvLandscapeInfo() {}
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

		bool read(CvXMLLoadUtility* pXML);

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
	bool read(CvXMLLoadUtility* pXML, bool bUTF8, const char *szFileName, const TCHAR* szLanguage);

	static int getNumLanguagesStatic();
	static const TCHAR* getLanguageName(int iLanguageID);
	static int getLanguageAtIndex(int iIndex);
	static bool readLanguages(CvXMLLoadUtility* pXML);
	static void setChangeLanguage();

	static int getCodePage();
	static int getLanguageID(const char* szLanguageName);


	static bool readString(CvXMLLoadUtility* pXML, CvWString &szString, const char* szTagName, bool bUTF8, const char *szFileName, bool bLanguageFound, const char* szType);
	static CvWString convertFromUTF8(const CvString sourceString, bool bFallback, const char *szFileName, const char* szType);

protected:

	CvWString m_szText;
	CvWString m_szGender;
	CvWString m_szPlural;
	static int STATIC_iNumLanguages;
	static int *STATIC_pLanguageArray;
	static bool STATIC_bChangeLanguage;
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
		Response() :
			m_iNumDiplomacyText(0),
			m_abCivilizationTypes(NULL),
			m_abLeaderHeadTypes(NULL),
			m_abAttitudeTypes(NULL),
			m_abDiplomacyPowerTypes(NULL),
			m_paszDiplomacyText(NULL)
		{
		}
		virtual ~Response ()
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

	CvDiplomacyTextInfo();
	virtual ~CvDiplomacyTextInfo() { uninit(); }	// free memory - MT
	// note - Response member vars allocated by CvXmlLoadUtility
	void init(int iNum);
	void uninit();

	const Response& getResponse(int iNum) const { return m_pResponses[iNum]; }
	int getNumResponses() const;

	bool getCivilizationTypes(int i, int j) const;
	bool getLeaderHeadTypes(int i, int j) const;
	bool getAttitudeTypes(int i, int j) const;
	bool getDiplomacyPowerTypes(int i, int j) const;

	int getNumDiplomacyText(int i) const;

	const char* getDiplomacyText(int i, int j) const;

	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	bool read(CvXMLLoadUtility* pXML);

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
	CvEffectInfo();
	virtual ~CvEffectInfo();
	DllExport const char* getPath() const { return m_szPath; }
	void setPath(const char* szVal) { m_szPath = szVal; }
	float getUpdateRate( ) const { return m_fUpdateRate; };

	void setUpdateRate( float fUpdateRate ) { m_fUpdateRate = fUpdateRate; }
	bool isProjectile() const { return m_bProjectile; };

	float getProjectileSpeed() const { return m_fProjectileSpeed; };

	float getProjectileArc() const { return m_fProjectileArc; };

	bool isSticky() const { return m_bSticky; };

	bool read(CvXMLLoadUtility* pXML);

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
	CvAttachableInfo();
	virtual ~CvAttachableInfo();
	DllExport const char* getPath() const { return m_szPath; }
	void setPath(const char* szVal) { m_szPath = szVal; }
	bool read(CvXMLLoadUtility* pXML);

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
	CvGameOptionInfo();
	virtual ~CvGameOptionInfo();
	DllExport bool getDefault() const;
	DllExport bool getVisible() const;
	bool read(CvXMLLoadUtility* pXML);

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
	CvMPOptionInfo();
	virtual ~CvMPOptionInfo();
	bool getDefault() const;
	bool read(CvXMLLoadUtility* pXML);

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
	CvForceControlInfo();
	virtual ~CvForceControlInfo();
	bool getDefault() const;
	bool read(CvXMLLoadUtility* pXML);

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
	CvPlayerOptionInfo();
	virtual ~CvPlayerOptionInfo();
	DllExport bool getDefault() const;
	bool read(CvXMLLoadUtility* pXML);

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
	CvGraphicOptionInfo();
	virtual ~CvGraphicOptionInfo();
	DllExport bool getDefault() const;
	bool read(CvXMLLoadUtility* pXML);

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
	virtual ~CvEventTriggerInfo();
	int getPercentGamesActive() const;
	int getProbability() const;
	int getNumUnits() const;
	int getNumBuildings() const;
	int getNumUnitsGlobal() const;
	int getNumBuildingsGlobal() const;
	int getNumPlotsRequired() const;
	PlotTypes getPlotType() const;
	int getOtherPlayerShareBorders() const;
	int getCityFoodWeight() const;
	CivicTypes getCivic() const;
	int getMinPopulation() const;
	int getMaxPopulation() const;
	int getMinMapLandmass() const;
	int getMinOurLandmass() const;
	int getMaxOurLandmass() const;
	HandicapTypes getMinDifficulty() const;
	int getUnitDamagedWeight() const;
	int getUnitDistanceWeight() const;
	int getUnitExperienceWeight() const;
	int getMinTreasury() const;

	bool canTriggerOnCivCategory(CivCategoryTypes eCategory) const;
	const InfoArray<UnitClassTypes>& getUnitsRequired() const;
	const InfoArray<BuildingClassTypes>& getBuildingsRequired() const;
	const InfoArray<EventTypes>& getEvents() const;
	const InfoArray<EventTypes>& getPrereqEvents() const;
	const InfoArray<FeatureTypes>& getFeaturesRequired() const;
	const InfoArray<TerrainTypes>& getTerrainsRequired() const;
	const InfoArray<ImprovementTypes>& getImprovementsRequired() const;
	const InfoArray<RouteTypes>& getRoutesRequired() const;

	const CvWString& getText(int i) const;
	int getTextEra(int i) const;
	int getNumTexts() const;
	const CvWString& getWorldNews(int i) const;
	int getNumWorldNews() const;

	// Start EmperorFool: Events with Images
	const TCHAR* getEventArt() const;				// Exposed to Python
	// End EmperorFool: Events with Images
	bool isTutorial() const;
	bool isSinglePlayer() const;
	bool isTeam() const;
	bool isRecurring() const;
	bool isGlobal() const;
	bool isPickPlayer() const;
	bool isOtherPlayerWar() const;
	bool isOtherPlayerAI() const;
	bool isOtherPlayerNative() const;
	bool isOtherPlayerPlayable() const;
	bool isPickCity() const;
	bool isPickOtherPlayerCity() const;
	bool isShowPlot() const;
	bool isUnitsOnPlot() const;
	bool isOwnPlot() const;
	bool isProbabilityUnitMultiply() const;
	bool isProbabilityBuildingMultiply() const;
	bool isPrereqEventCity() const;
	bool isFrontPopup() const;

	const char* getPythonCallback() const;
	const char* getPythonCanDo() const;
	const char* getPythonCanDoCity() const;
	const char* getPythonCanDoUnit() const;
	void read(FDataStreamBase* );
	void write(FDataStreamBase* );
	bool read(CvXMLLoadUtility* pXML);

	void verifyTriggerSettings() const;

private:
	template<typename T>
	void verifyTriggerSettings(const InfoArray<T>&) const;
	const char* verifyTriggerSettings(FeatureTypes) const;
	const char* verifyTriggerSettings(TerrainTypes) const;
	const char* verifyTriggerSettings(ImprovementTypes) const;
	const char* verifyTriggerSettings(RouteTypes) const;

	int m_iPercentGamesActive;
	int m_iProbability;
	int m_iNumUnits;
	int m_iNumBuildings;
	int m_iNumUnitsGlobal;
	int m_iNumBuildingsGlobal;
	int m_iNumPlotsRequired;
	PlotTypes m_ePlotType;
	int m_iOtherPlayerShareBorders;
	int m_iCityFoodWeight;
	CivicTypes m_eCivic;
	int m_iMinPopulation;
	int m_iMaxPopulation;
	int m_iMinMapLandmass;
	int m_iMinOurLandmass;
	int m_iMaxOurLandmass;
	HandicapTypes m_eMinDifficulty;
	int m_iUnitDamagedWeight;
	int m_iUnitDistanceWeight;
	int m_iUnitExperienceWeight;
	int m_iMinTreasury;
	EnumMap<CivCategoryTypes, bool> m_emAllowedCivCategories;
	InfoArray<UnitClassTypes> m_info_UnitsRequired;
	InfoArray<BuildingClassTypes> m_info_BuildingsRequired;
	InfoArray<EventTypes> m_info_Events;
	InfoArray<EventTypes> m_info_PrereqEvents;
	InfoArray<FeatureTypes> m_info_FeaturesRequired;
	InfoArray<TerrainTypes> m_info_TerrainsRequired;
	InfoArray<ImprovementTypes> m_info_ImprovementsRequired;
	InfoArray<RouteTypes> m_info_RoutesRequired;
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

public:
	// functions purely for the python interface
	int PY_getPlotType() const;
	int PY_getCivic() const;
	int PY_getMinDifficulty() const;
	int PY_getBuildingRequired(int i) const;
	int PY_getNumBuildingsRequired() const;
	int PY_getUnitRequired(int i) const;
	int PY_getNumUnitsRequired() const;
	int PY_getEvent(int i) const;
	int PY_getNumEvents() const;
	int PY_getPrereqEvent(int i) const;
	int PY_getNumPrereqEvents() const;
	int PY_getFeatureRequired(int i) const;
	int PY_getNumFeaturesRequired() const;
	int PY_getTerrainRequired(int i) const;
	int PY_getNumTerrainsRequired() const;
	int PY_getImprovementRequired(int i) const;
	int PY_getNumImprovementsRequired() const;
	int PY_getRouteRequired(int i) const;
	int PY_getNumRoutesRequired() const;
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
	virtual ~CvEventInfo();
	bool isQuest() const;
	bool isGlobal() const;
	bool isTeam() const;
	bool isCityEffect() const;
	bool isOtherPlayerCityEffect() const;
	bool isGoldToPlayer() const;
	bool isDeclareWar() const;
	bool isDisbandUnit() const;

	int getGold() const;
	int getRandomGold() const;
	int getCulture() const;
	int getHealth() const; // R&R, ray, change for Health in Events
	int getUnitClass() const;
	int getNumUnits() const;
	int getBuildingClass() const;
	int getBuildingChange() const;
	int getFood() const;
	int getFoodPercent() const;
	int getFeature() const;
	int getFeatureChange() const;
	int getImprovement() const;
	int getImprovementChange() const;
	int getRoute() const;
	int getRouteChange() const;
	int getBonusRevealed() const;
	int getUnitExperience() const;
	int getUnitImmobileTurns() const;
	int getOurAttitudeModifier() const;
	int getAttitudeModifier() const;
	int getTheirEnemyAttitudeModifier() const;
	int getPopulationChange() const;
	int getRevoltTurns() const;
	int getMinPillage() const;
	int getMaxPillage() const;
	int getUnitPromotion() const;
	int getAIValue() const;
	// TAC - Generic Parameters for Events - Ray - START
	int getGenericParameter(int x) const;
	// TAC - Generic Parameters for Events - Ray - END
	int getAdditionalEventChance(int i) const;
	int getAdditionalEventTime(int i) const;
	int getClearEventChance(int i) const;
	int getPlotExtraYield(int i) const;
	int getUnitCombatPromotion(int i) const;
	int getUnitClassPromotion(int i) const;
	const CvWString& getWorldNews(int i) const;
	int getNumWorldNews() const;
	int getBuildingYieldChange(int iBuildingClass, int iYield) const;
	int getNumBuildingYieldChanges() const;
	const char* getPythonCallback() const;
	const char* getPythonExpireCheck() const;
	const char* getPythonCanDo() const;
	const char* getPythonHelp() const;
	const wchar* getUnitNameKey() const;
	const wchar* getQuestFailTextKey() const;
	const wchar* getOtherPlayerPopup() const;
	const wchar* getLocalInfoTextKey() const;

	// functions to calculate indirect data
	// useful for using the same code in canApply() and apply()
	int getFoodChange(const CvCity* pCity) const;

	void read(FDataStreamBase* );
	void write(FDataStreamBase* );
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

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
	CvMainMenuInfo();
	virtual ~CvMainMenuInfo();
	DllExport std::string getScene() const;
	DllExport std::string getSoundtrack() const;
	DllExport std::string getLoading() const;
	DllExport std::string getLoadingSlideshow() const;
	bool read(CvXMLLoadUtility* pXML);

protected:
	std::string m_szScene;
	std::string m_szSoundtrack;
	std::vector<std::string> m_a_szLoading;
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
	virtual ~CvFatherInfo();
	int getFatherCategory() const;
	TraitTypes getTrait() const;
	int getFreeUnits(int iUnitClass) const;
	int getPointCost(int iFatherPointType) const;
	bool isRevealImprovement(int iImprovement) const;
	const char* getPortrait() const;

	inline CivEffectTypes getCivEffect() const { return m_eCivEffect; }

	std::wstring pyGetQuoteKey() { return getQuoteKey(); }
	const wchar* getQuoteKey();
	const char* getSound() const;
	const char* getSoundMP() const;
	void read(FDataStreamBase* );
	void write(FDataStreamBase* );
	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iFatherCategory;
	TraitTypes m_eTrait;
	CivEffectTypes m_eCivEffect;
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
	CvFatherPointInfo();
	virtual ~CvFatherPointInfo();
	DllExport int getChar() const;
	DllExport void setChar(int i);
	DllExport int getFontButtonIndex() const;
	int getLandTilePoints() const;
	int getWaterTilePoints() const;
	int getMeetNativePoints() const;
	int getScoutVillagePoints() const;
	int getGoodyPoints() const;
	int getExperiencePoints() const;
	int getConquerCityPoints() const;
	int getRazeCityPoints() const;
	int getMissionaryPoints() const;
	int getEuropeTradeGoldPointPercent() const;
	int getNativeTradeGoldPointPercent() const;
	int getProductionConversionPoints() const;
	int getBuildingPoints(int iBuildingClass) const;
	int getYieldPoints(int iYield) const;
	bool read(CvXMLLoadUtility* pXML);

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
	CvAlarmInfo();
	virtual ~CvAlarmInfo();

	int getRange() const;
	int getColony() const;
	int getNumColonies() const;
	int getPopulation() const;
	int getUnit() const;
	int getMissionary() const;
	int getAttitudeDivisor() const;

	bool read(CvXMLLoadUtility* pXML);

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

	CvUnitArtStyleTypeInfo();
	virtual ~CvUnitArtStyleTypeInfo();

    const char* getArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, int /*ProfessionType*/ m) const;
	void setArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, int /*ProfessionType*/ m, const char* szVal);

	bool read(CvXMLLoadUtility* pXML);

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
	CvAchieveInfo();
	virtual ~CvAchieveInfo();
	bool read(CvXMLLoadUtility* pXML);
	bool isActive();
	bool isLandDiscovered();
	bool isAfterMove();
	bool isDiscoverEast();
	bool isDiscoverWest();
	int getLowTurnNumber();
	int getHighTurnNumber();
	int getPictureX();
	int getPictureY();
	int getNumColonies();
	int getNumEuroContacts();
	int getNumNativeContacts();
	int getNumGoodsTraded();
	int getNumCombatsWonNeeded();
	int getNumSeaCombatsWonNeeded(); // R&R, ray, Achievments for Sea Combats
	int getNumBuildings(int iBuilding) const;
	int getNumUnits(int iUnit) const;
	int getNumProfessions(int iProfession) const;
	int getYieldProduced(int iYield) const;
	const wchar* getDescription() const;
	const wchar* getHeaderTextKey() const;
	const wchar* getBodyTextKey() const;
	const char* getPictureFileName() const;
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

// trade screen type - start - Nightinggale
class CvTradeScreenInfo :
	public CvInfoBase
{
public:
	CvTradeScreenInfo();
	~CvTradeScreenInfo();

	bool read(CvXMLLoadUtility* pXML);

protected:
};
// trade screen type - end - Nightinggale

class CivEffectInfo :
	public CvInfoBase
{
public:
	CivEffectInfo(bool bAutogenerateAllow = false);
	~CivEffectInfo();

	// allow
	inline const InfoArray<BonusTypes        , int>& getAllowedBonuses              () const { return m_info_AllowBonuses      ; }
	inline const InfoArray<BuildTypes        , int>& getAllowedBuilds               () const { return m_info_AllowBuilds       ; }
	inline const InfoArray<BuildingClassTypes, int>& getAllowedBuildingClasses      () const { return m_info_AllowBuildings    ; }
	inline const InfoArray<CivicTypes        , int>& getAllowedCivics               () const { return m_info_AllowCivics       ; }
	inline const InfoArray<CivCategoryTypes  , int>& getAllowConqueringCity         () const { return m_info_AllowConqueringCity;}
	inline const InfoArray<UnitClassTypes    , int>& getAllowedImmigrants           () const { return m_info_AllowImmigrants   ; }
	inline const InfoArray<ImprovementTypes  , int>& getAllowedImprovements         () const { return m_info_AllowImprovements ; }
	inline const InfoArray<ProfessionTypes   , int>& getAllowedProfessions          () const { return m_info_AllowProfessions  ; }
	inline const InfoArray<PromotionTypes    , int>& getAllowedPromotions           () const { return m_info_AllowPromotions   ; }
	inline const InfoArray<RouteTypes        , int>& getAllowedRoutes               () const { return m_info_AllowRoutes       ; }
	inline const InfoArray<UnitClassTypes    , int>& getAllowedUnitClasses          () const { return m_info_AllowUnits        ; }
	inline const InfoArray<YieldTypes        , int>& getAllowedYields               () const { return m_info_AllowYields       ; }

	inline const int getAllowFoundCity                     () const { return m_iAllowFoundCity        ; }

	// city
	inline int getCanUseDomesticMarket                     () const { return m_iCanUseDomesticMarket  ; }

	// growth
	int getLearningByDoingModifier                         () const;
	inline int getNumUnitsOnDockChange                     () const { return m_iNumUnitsOnDockChange  ; }

	// unit
	inline const InfoArray<PromotionTypes, int>                 & getFreePromotions              () const { return m_info_FreePromotions              ; }
	inline const InfoArray<ProfessionTypes, PromotionTypes, int>& getFreePromotionsForProfessions() const { return m_info_FreePromotionsForProfessions; }
	inline const InfoArray<UnitCombatTypes, PromotionTypes, int>& getFreePromotionsForUnitCombats() const { return m_info_FreePromotionsForUnitCombats; }

	bool read(CvXMLLoadUtility* pXML);

protected:
	// allow
	InfoArray<BonusTypes        , int> m_info_AllowBonuses;
	InfoArray<BuildTypes        , int> m_info_AllowBuilds;
	InfoArray<BuildingClassTypes, int> m_info_AllowBuildings;
	InfoArray<CivicTypes        , int> m_info_AllowCivics;
	InfoArray<CivCategoryTypes  , int> m_info_AllowConqueringCity;
	InfoArray<UnitClassTypes    , int> m_info_AllowImmigrants;
	InfoArray<ImprovementTypes  , int> m_info_AllowImprovements;
	InfoArray<ProfessionTypes   , int> m_info_AllowProfessions;
	InfoArray<PromotionTypes    , int> m_info_AllowPromotions;
	InfoArray<RouteTypes        , int> m_info_AllowRoutes;
	InfoArray<UnitClassTypes    , int> m_info_AllowUnits;
	InfoArray<YieldTypes        , int> m_info_AllowYields;

	int m_iAllowFoundCity;

	// city
	int m_iCanUseDomesticMarket;

	// growth
	int m_iLearningByDoingModifier;
	int m_iNumUnitsOnDockChange;

	// unit
	InfoArray<                 PromotionTypes, int> m_info_FreePromotions;
	InfoArray<ProfessionTypes, PromotionTypes, int> m_info_FreePromotionsForProfessions;
	InfoArray<UnitCombatTypes, PromotionTypes, int> m_info_FreePromotionsForUnitCombats;
};


#endif
