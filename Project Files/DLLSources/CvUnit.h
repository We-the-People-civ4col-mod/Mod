#pragma once

// unit.h

#ifndef CIV4_UNIT_H
#define CIV4_UNIT_H

#include "CvDLLEntity.h"
//#include "CvEnums.h"
//#include "CvStructs.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvPlot;
class CvArea;
class CvUnitInfo;
class CvSelectionGroup;
class FAStarNode;
class CvArtInfoUnit;
class KmodPathFinder;


class CvSavegameReader;
class CvSavegameWriter;

struct CombatDetails
{
	int iExtraCombatPercent;
	int iNativeCombatModifierTB;
	int iNativeCombatModifierAB;
	int iPlotDefenseModifier;
	int iFortifyModifier;
	int iCityDefenseModifier;
	int iHillsAttackModifier;
	int iHillsDefenseModifier;
	int iDomesticBonusModifier;
	int iFeatureAttackModifier;
	int iFeatureDefenseModifier;
	int iTerrainAttackModifier;
	int iTerrainDefenseModifier;
	int iCityAttackModifier;
	int iDomainDefenseModifier;
	int iClassDefenseModifier;
	int iClassAttackModifier;
	int iCombatModifierT;
	int iCombatModifierA;
	int iDomainModifierA;
	int iDomainModifierT;
	int iRiverAttackModifier;
	int iAmphibAttackModifier;
	int iRebelPercentModifier;
	int iModifierTotal;
	int iBaseCombatStr;
	int iCombat;
	int iMaxCombatStr;
	int iCurrHitPoints;
	int iMaxHitPoints;
	int iCurrCombatStr;
	PlayerTypes eOwner;
	PlayerTypes eVisualOwner;
	std::wstring sUnitName;
};

class CvUnitTemporaryStrengthModifier
{
public:
	CvUnitTemporaryStrengthModifier(CvUnit* pUnit, ProfessionTypes eProfession);
	~CvUnitTemporaryStrengthModifier();

private:
	CvUnit* m_pUnit;
	ProfessionTypes m_eProfession;
};

enum Port
{
	EUROPE,
	AFRICA
};

class CvUnit : public CvDLLEntity
{

public:

    /** NBMOD REF **/

    float NBMOD_GetShipStrength() const;

    /** NBMOD REF **/

	CvUnit();
	virtual ~CvUnit();

	void reloadEntity();
	void init(int iID, UnitTypes eUnit, ProfessionTypes eProfession, UnitAITypes eUnitAI, PlayerTypes eOwner, Coordinates initCoord, DirectionTypes eFacingDirection, int iYieldStored);
	void uninit();
	void reset(int iID = 0, UnitTypes eUnit = NO_UNIT, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall = false);
	void setupGraphical();
	void convert(CvUnit* pUnit, bool bKill);
	void kill(bool bDelay, CvUnit* pAttacker = NULL);
	void removeFromMap();
	void addToMap(Coordinates targetCoord);
	void addToMap(CvPlot *targetPlot);
	void updateOwnerCache(int iChange);

	DllExport void NotifyEntity(MissionTypes eMission);

	void doTurn();

	void updateCombat(bool bQuick = false);

	bool isActionRecommended(int iAction);
	bool isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker, bool bBreakTies) const;

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bTestBusy = true);
	DllExport void doCommand(CommandTypes eCommand, int iData1, int iData2);

	//FAStarNode* getPathLastNode() const; // disabled by K-Mod
	CvPlot* getPathEndTurnPlot() const;
	int getPathCost() const;
	// TAC - AI Improved Naval AI - koma13 - START
	//bool generatePath(const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL) const;
	//bool generatePath(const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL, bool bIgnoreDanger = true) const;
	// TAC - AI Improved Naval AI - koma13 - END
	bool generatePath(const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false,							// Exposed to Python
		int* piPathTurns = NULL,
		int iMaxPath = -1, // K-Mod
		bool bUseTempFinder = false) const; // advc.128
	KmodPathFinder& getPathFinder() const; // K-Mod

	bool canEnterTerritory(PlayerTypes ePlayer, bool bIgnoreRightOfPassage = false) const;
	bool canEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage = false) const;
	TeamTypes getDeclareWarUnitMove(const CvPlot* pPlot) const;
	bool canMoveInto(CvPlot const& kPlot, bool bAttack = false, bool bDeclareWar = false,					// Exposed to Python
		bool bIgnoreLoad = false,
		bool bAssumeVisible = true, // K-Mod
		bool bDangerCheck = false) const; // advc.001k
	bool canMoveOrAttackInto(const CvPlot* pPlot, bool bDeclareWar = false) const;
	bool canMoveThrough(const CvPlot* pPlot) const;
	void attack(CvPlot* pPlot, bool bQuick);
	void move(CvPlot* pPlot, bool bShow);
	bool jumpToNearestValidPlot();
	bool isValidPlot(const CvPlot* pPlot) const;

	bool canAutomate(AutomateTypes eAutomate) const;
	void automate(AutomateTypes eAutomate);
	bool canScrap() const;
	void scrap();
	bool canGift(bool bTestVisible = false, bool bTestTransport = true);
	void gift(bool bTestTransport = true);
	bool canLoadUnit(const CvUnit* pTransport, const CvPlot* pPlot, bool bCheckCity) const;
	void loadUnit(CvUnit* pTransport);
	bool canLoad(const CvPlot* pPlot, bool bCheckCity) const;
	bool load(bool bCheckCity);
	bool shouldLoadOnMove(const CvPlot* pPlot) const;

	int getLoadedYieldAmount(YieldTypes eYield) const;
	int getLoadYieldAmount(YieldTypes eYield) const;
	bool canLoadYields(const CvPlot* pPlot, bool bTrade) const;
	bool canLoadYield(const CvPlot* pPlot, YieldTypes eYield, bool bTrade) const;
	//R&R mod, vetiarvind, max yield import limit - start
	//void loadYield(YieldTypes eYield, bool bTrade);
	//void loadYieldAmount(YieldTypes eYield, int iAmount, bool bTrade);
	int loadYield(YieldTypes eYield, bool bTrade);
	int loadYieldAmount(YieldTypes eYield, int iAmount, bool bTrade);
	//R&R mod, vetiarvind, max yield import limit - end
	int getMaxLoadYieldAmount(YieldTypes eYield) const;

	bool canTradeYield(const CvPlot* pPlot) const;
	void tradeYield();

	bool canClearSpecialty() const;
	void clearSpecialty();

	bool canAutoCrossOcean(const CvPlot* pPlot) const;
	bool canCrossOcean(const CvPlot* pPlot, UnitTravelStates eNewState) const;
	void crossOcean(UnitTravelStates eNewState);
	bool canUnload() const;
	void unload();
	void unloadStoredAmount(int iAmount);
	bool canUnloadAll() const;
	void unloadAll();

	bool canLearn() const;
	void learn();
	void doLiveAmongNatives();
	void doLearn();
	UnitTypes getLearnUnitType(const CvPlot* pPlot) const;
	int getLearnTime() const;

	bool canKingTransport() const;
	void kingTransport(bool bSkipPopup);
	void doKingTransport();

	bool canEstablishMission() const;
	void establishMission();
	int getMissionarySuccessPercent() const;

	// Ramstormp, Disillusioned Missionary - START
	int getFailedMissionarySurvivalPercent() const;
	// Ramstormp - END

	// WTP, ray, Failed Trader - START
	int getFailedTraderSurvivalPercent() const;
	// WTP, ray, Failed Trader - END

	// WTP, ray, Native Trade Posts - START
	bool canEstablishTradePost() const;
	void establishTradePost();
	int getNativeTradePostSuccessPercent() const;
	// WTP, ray, Native Trade Posts - END

	// R&R, ray , Stirring Up Natives - START
	bool canStirUp() const;
	void stirUpNatives();
	int getStirUpSuccessPercent() const;
	// R&R, ray , Stirring Up Natives - END

	// WTP, merge Treasures, of Raubwuerger - START
	bool canMergeTreasures() const;
	void mergeTreasures();
	void createTreasures(int overallAmount, int maxTreasureGold);
	// WTP, merge Treasures, of Raubwuerger - END

	// WTP, ray, Construction Supplies - START
	bool canUseProductionSupplies() const;
	void useProductionSupplies();
	// WTP, ray, Construction Supplies - END

	bool canSpeakWithChief(CvPlot* pPlot) const;
	void speakWithChief();
	bool canHold(const CvPlot* pPlot) const;
	DllExport bool canSleep(const CvPlot* pPlot) const;
	DllExport bool canFortify(const CvPlot* pPlot) const;

	bool canHeal(const CvPlot* pPlot) const;
	bool canSentry(const CvPlot* pPlot) const;

	int healRate(const CvPlot* pPlot) const;
	int healTurns(const CvPlot* pPlot) const;
	void doHeal();
	CvCity* bombardTarget(const CvPlot* pPlot) const;
	// Super Forts begin *bombard*
	CvPlot* bombardImprovementTarget(const CvPlot* pPlot) const;
	// Super Forts end
	bool canBombard(const CvPlot* pPlot) const;
	bool bombard();
	bool canPillage(const CvPlot* pPlot) const;
	bool pillage();
	bool canSack(const CvPlot* pPlot);
	bool sack(CvPlot* pPlot);
	bool canFound(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool found();
	bool doFound(bool bBuyLand);
	bool doFoundCheckNatives();
	//ray18
	bool doAcquireCheckNatives();
	void buyLandAfterAcquire();
	//Ende ray18
	// TAC - Clear Specialty Fix - koma13 - START
	//bool canJoinCity(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool canJoinCity(const CvPlot* pPlot, bool bTestVisible = false, bool bIgnoreFood = false) const;
	// TAC - Clear Specialty Fix - koma13 - END
	bool canJoinStarvingCity(const CvCity& kCity) const;
	bool joinCity();
	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible = false) const;
	bool build(BuildTypes eBuild);
	bool canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const;
	void promote(PromotionTypes ePromotion, int iLeaderUnitId);

	int canLead(const CvPlot* pPlot, int iUnitId) const;
	bool lead(int iUnitId);
	int canGiveExperience(const CvPlot* pPlot) const;
	bool giveExperience();
	int getStackExperienceToGive(int iNumUnits) const;
	int upgradePrice(UnitTypes eUnit) const;
	bool upgradeAvailable(UnitTypes eFromUnit, UnitClassTypes eToUnitClass, int iCount = 0) const;
	bool canUpgrade(UnitTypes eUnit, bool bTestVisible = false) const;
	bool isReadyForUpgrade() const;
	bool hasUpgrade(bool bSearch = false) const;
	bool hasUpgrade(UnitTypes eUnit, bool bSearch = false) const;
	CvCity* getUpgradeCity(bool bSearch = false) const;
	CvCity* getUpgradeCity(UnitTypes eUnit, bool bSearch = false, int* iSearchValue = NULL) const;
	void upgrade(UnitTypes eUnit);
	HandicapTypes getHandicapType() const;
	DllExport CivilizationTypes getCivilizationType() const;
	const wchar* getVisualCivAdjective(TeamTypes eForTeam) const;
	SpecialUnitTypes getSpecialUnitType() const;
	UnitTypes getCaptureUnitType(CivilizationTypes eCivilization) const;
	UnitCombatTypes getUnitCombatType() const;
	DllExport DomainTypes getDomainType() const;
	InvisibleTypes getInvisibleType() const;
	int getNumSeeInvisibleTypes() const;
	InvisibleTypes getSeeInvisibleType(int i) const;

	bool isHuman() const;
	bool isNative() const;

	int visibilityRange() const;
	int visibilityRangeUncached() const;

	int baseMoves() const;
	int maxMoves() const;
	int movesLeft() const;
	DllExport bool canMove() const;
	DllExport bool hasMoved() const;

	bool canBuildRoute(RouteTypes ePreferredRoute = NO_ROUTE) const;
	DllExport BuildTypes getBuildType() const;
	int workRate(bool bMax) const;
	void changeExtraWorkRate(int iChange);
	int getExtraWorkRate() const;
	bool isNoBadGoodies() const;
	bool isOnlyDefensive() const;
	bool isNoUnitCapture() const;
	bool isNoCityCapture() const;
	bool isRivalTerritory() const;
	bool canCoexistWithEnemyUnit(TeamTypes eTeam) const;

	DllExport bool isFighting() const;
	DllExport bool isAttacking() const;
	DllExport bool isDefending() const;
	bool isCombat() const;

	DllExport int maxHitPoints() const;
	int currHitPoints() const;
	bool isHurt(int iThresholdPercent = 0) const;
	DllExport bool isDead() const;

	void setBaseCombatStr(int iCombat);
	int baseCombatStr() const;
	void updateBestLandCombat();
	int maxCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL) const;
	int currCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL) const;
	int currFirepower(const CvPlot* pPlot, const CvUnit* pAttacker) const;
	int currEffectiveStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL) const;
	DllExport float maxCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const;
	DllExport float currCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const;
	bool isUnarmed() const;
	int getPower() const;
	int getAsset() const;

	DllExport bool canFight() const;
	bool canAttack() const;
	bool canDefend(const CvPlot* pPlot = NULL) const;
	bool canSiege(TeamTypes eTeam) const;

	bool isAutomated() const;
	DllExport bool isWaiting() const;
	DllExport bool isFortifyable() const;
	int fortifyModifier() const;

	int experienceNeeded() const;
	int attackXPValue() const;
	int defenseXPValue() const;
	int maxXPValue() const;

	DllExport bool isRanged() const;

	bool alwaysInvisible() const;
	bool noDefensiveBonus() const;
	bool canMoveImpassable() const;
	bool flatMovementCost() const;
	bool ignoreTerrainCost() const;
	bool isNeverInvisible() const;
	DllExport bool isInvisible(TeamTypes eTeam, bool bDebug, bool bCheckCargo = true) const;

	int withdrawalProbability() const;
	int getEvasionProbability(const CvUnit& kAttacker) const;
	CvCity* getEvasionCity() const;

	int cityAttackModifier() const;
	int cityDefenseModifier() const;
	int hillsAttackModifier() const;
	int hillsDefenseModifier() const;
	int DomesticBonusModifier() const;
	int terrainAttackModifier(TerrainTypes eTerrain) const;
	int terrainDefenseModifier(TerrainTypes eTerrain) const;
	int featureAttackModifier(FeatureTypes eFeature) const;
	int featureDefenseModifier(FeatureTypes eFeature) const;
	int unitClassAttackModifier(UnitClassTypes eUnitClass) const;
	int unitClassDefenseModifier(UnitClassTypes eUnitClass) const;
	int unitCombatModifier(UnitCombatTypes eUnitCombat) const;
	int domainModifier(DomainTypes eDomain) const;
	int rebelModifier(PlayerTypes eOtherPlayer) const;

	int bombardRate() const;
	SpecialUnitTypes specialCargo() const;
	DomainTypes domainCargo() const;
	int cargoSpace() const;
	void changeCargoSpace(int iChange);
	bool isFull() const;
	int cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const;
	bool hasCargo() const;
	bool canCargoAllMove() const;
	bool canCargoEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage) const;
	int getUnitAICargo(UnitAITypes eUnitAI) const;
	bool canAssignTradeRoute(int iRouteId, bool bReusePath = false) const;

	DllExport int getID() const;
	int getIndex() const;
	DllExport IDInfo getIDInfo() const;
	void setID(int iID);
	int getGroupID() const;
	bool isInGroup() const;
	DllExport bool isGroupHead() const;
	DllExport CvSelectionGroup* getGroup() const;
	bool canJoinGroup(const CvPlot* pPlot, CvSelectionGroup* pSelectionGroup) const;
	DllExport void joinGroup(CvSelectionGroup* pSelectionGroup, bool bRemoveSelected = false, bool bRejoin = true);
	DllExport int getHotKeyNumber();
	void setHotKeyNumber(int iNewValue);
	//TAC Whaling, ray
	bool isFullToBrim() const;
	bool canGatherResource(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool isGatheringResource() const;
	void setGatheringResource(bool bNewValue);
	void convertFishingBoat();
	bool gatherResource();
	bool isWhalingBoat() const;
	//End TAC Whaling, ray
	bool isFishingBoat() const; // R&R, ray, High Sea Fishing
	// R&R, ray, Natives Trading - START
	void setYieldForNativeTrade(YieldTypes nativeTradeYield);
	void setAmountForNativeTrade(int nativeTradeAmount);
	YieldTypes getYieldForNativeTrade() const;
	int getAmountForNativeTrade() const;
	// R&R, ray, Natives Trading - END
	DllExport int getX() const;
	inline int getX_INLINE() const
	{
		return m_coord.x();
	}
	DllExport int getY() const;
	inline int getY_INLINE() const
	{
		return m_coord.y();
	}
	inline const Coordinates& coord() const
	{
		return m_coord;
	}
	void setXY(int iX, int iY, bool bGroup = false, bool bUpdate = true, bool bShow = false, bool bCheckPlotVisible = false);
	void jumpTo(Coordinates toCoord, bool bGroup = false, bool bUpdate = true, bool bShow = false, bool bCheckPlotVisible = false);
	void jumpTo(CvPlot *plot, bool bGroup = false, bool bUpdate = true, bool bShow = false, bool bCheckPlotVisible = false);
	bool at(int iX, int iY) const;
	bool at(Coordinates testCoord) const;
	DllExport bool atPlot(const CvPlot* pPlot) const;
	DllExport CvPlot* plot() const;
	CvCity* getCity() const;
	int getArea() const;
	int getLandArea() const;
	CvArea* area() const;
	int getLastMoveTurn() const;
	void setLastMoveTurn(int iNewValue);
	int getGameTurnCreated() const;
	void setGameTurnCreated(int iNewValue);
	DllExport int getDamage() const;
	void setDamage(int iNewValue, CvUnit* pAttacker = NULL, bool bNotifyEntity = true);
	void changeDamage(int iChange, CvUnit* pAttacker = NULL);
	void addDamageRandom(int iMinDamage, int iMaxDamage, int iMinHealthPercentageRemaining = 0);

	int getMoves() const;
	void setMoves(int iNewValue);
	void changeMoves(int iChange);
	void finishMoves();

	int getExperience() const;
	void setExperience(int iNewValue, int iMax = -1);
	void changeExperience(int iChange, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false);

	int getLevel() const;
	void setLevel(int iNewValue);
	void changeLevel(int iChange);
	int getCargo() const;
	void changeCargo(int iChange);

	CvPlot* getAttackPlot() const;
	void setAttackPlot(const CvPlot* pNewValue);

	DllExport int getCombatTimer() const;
	void setCombatTimer(int iNewValue);
	void changeCombatTimer(int iChange);

	int getCombatDamage() const;
	void setCombatDamage(int iNewValue);

	int getFortifyTurns() const;
	void setFortifyTurns(int iNewValue);
	void changeFortifyTurns(int iChange);
	int getBlitzCount() const;
	bool isBlitz() const;
	void changeBlitzCount(int iChange);

	int getAmphibCount() const;
	bool isAmphib() const;
	void changeAmphibCount(int iChange);

	int getRiverCount() const;
	bool isRiver() const;
	void changeRiverCount(int iChange);

	int getEnemyRouteCount() const;
	bool isEnemyRoute() const;
	void changeEnemyRouteCount(int iChange);

	int getAlwaysHealCount() const;
	bool isAlwaysHeal() const;
	void changeAlwaysHealCount(int iChange);

	int getHillsDoubleMoveCount() const;
	bool isHillsDoubleMove() const;
	void changeHillsDoubleMoveCount(int iChange);

	int getExtraVisibilityRange() const;
	void changeVisibilityRange(int iChange);
	int getExtraMoves() const;
	void changeExtraMoves(int iChange);
	int getExtraMoveDiscount() const;
	void changeExtraMoveDiscount(int iChange);
	int getExtraWithdrawal() const;
	void changeExtraWithdrawal(int iChange);
	int getExtraBombardRate() const;
	void changeExtraBombardRate(int iChange);
	int getExtraEnemyHeal() const;
	void changeExtraEnemyHeal(int iChange);

	int getExtraNeutralHeal() const;
	void changeExtraNeutralHeal(int iChange);

	int getExtraFriendlyHeal() const;
	void changeExtraFriendlyHeal(int iChange);

	int getSameTileHeal() const;
	void changeSameTileHeal(int iChange);

	int getAdjacentTileHeal() const;
	void changeAdjacentTileHeal(int iChange);

	int getExtraCombatPercent() const;
	void changeExtraCombatPercent(int iChange);
	int getExtraCityAttackPercent() const;
	void changeExtraCityAttackPercent(int iChange);
	int getExtraCityDefensePercent() const;
	void changeExtraCityDefensePercent(int iChange);
	int getExtraHillsAttackPercent() const;
	void changeExtraHillsAttackPercent(int iChange);
	int getExtraHillsDefensePercent() const;
	void changeExtraHillsDefensePercent(int iChange);
	int getExtraDomesticBonusPercent() const;
	void changeExtraDomesticBonusPercent(int iChange);
	int getPillageChange() const;
	void changePillageChange(int iChange);
	int getAnimalGoldChange() const; //WTP, ray, Animal Promotions increase gold from Animals
	void changeAnimalGoldChange(int iChange); //WTP, ray, Animal Promotions increase gold from Animals
	int getSlaveRevoltReductionBonus() const; //WTP, ray, Slave Hunter and Slave Master
	void changeSlaveRevoltReductionBonus(int iChange); //WTP, ray, Slave Hunter and Slave Master
	int getSlaveWorkerProductionBonus() const; //WTP, ray, Slave Hunter and Slave Master
	void changeSlaveWorkerProductionBonus(int iChange); //WTP, ray, Slave Hunter and Slave Master
	int getAdditionalLawToCityFromUnit() const; // WTP, ray, Lawkeeper Promotion - START
	void changeAdditionalLawToCityFromUnit(int iChange); // WTP, ray, Lawkeeper Promotion - START
	int getUpgradeDiscount() const;
	void changeUpgradeDiscount(int iChange);
	int getExperiencePercent() const;
	void changeExperiencePercent(int iChange);
	DllExport DirectionTypes getFacingDirection(bool checkLineOfSightProperty) const;
	void setFacingDirection(DirectionTypes facingDirection);
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();
	DllExport ProfessionTypes getProfession() const;

	// TAC - LbD - Ray - START
	int getLbDrounds() const;
	void setLbDrounds(int newRounds);
	ProfessionTypes getLastLbDProfession() const;
	void setLastLbDProfession(ProfessionTypes eProfession);
	// TAC - LbD - Ray - END


	// WTP, ray, saving 1 more Profession for Fisher Issue - START
	int getLbDroundsBefore() const;
	void setLbDroundsBefore(int newRounds);
	ProfessionTypes getLastLbDProfessionBefore() const;
	void setLastLbDProfessionBefore(ProfessionTypes eProfession);
	// WTP, ray, saving 1 more Profession for Fisher Issue - END

	bool canLeaveCity() const;
	bool setProfession(ProfessionTypes eProfession, bool bForce = false, bool bRemoveYieldsFromCity = true);
	bool canHaveProfession(ProfessionTypes eProfession, bool bBumpOther,  const CvPlot* pPlot = NULL, bool bForceCheck = false) const;
	void processProfession(ProfessionTypes eProfession, int iChange, bool bUpdateCity, bool bRemoveYieldsFromCity = true);
	void processProfessionStats(ProfessionTypes eProfession, int iChange);
private:
	void processProfessionStatsUnsaved(const CvProfessionInfo& kProfession, int iChange);
public:
	int getProfessionChangeYieldRequired(ProfessionTypes eProfession, YieldTypes eYield) const;
	int getEuropeProfessionChangeCost(ProfessionTypes eProfession) const;

	bool isMadeAttack() const;
	void setMadeAttack(bool bNewValue);

	DllExport bool isPromotionReady() const;
	DllExport void setPromotionReady(bool bNewValue);
	void testPromotionReady();

	bool isDelayedDeath() const;
	void startDelayedDeath();
	bool doDelayedDeath();

	bool isCombatFocus() const;

	DllExport bool isInfoBarDirty() const;
	DllExport void setInfoBarDirty(bool bNewValue);

	DllExport PlayerTypes getOwner() const;
	inline PlayerTypes getOwnerINLINE() const
	{
		return m_eOwner;
	}
	CvPlayer &getOwnerR() const;
	DllExport PlayerTypes getVisualOwner(TeamTypes eForTeam = NO_TEAM) const;
	PlayerTypes getCombatOwner(TeamTypes eForTeam, const CvPlot* pPlot) const;
	DllExport TeamTypes getTeam() const;
	DllExport PlayerColorTypes getPlayerColor(TeamTypes eForTeam = NO_TEAM) const;
	DllExport CivilizationTypes getVisualCiv(TeamTypes eForTeam = NO_TEAM) const;
	TeamTypes getCombatTeam(TeamTypes eForTeam, const CvPlot* pPlot) const;

	PlayerTypes getCapturingPlayer() const;
	void setCapturingPlayer(PlayerTypes eNewValue);
	bool isCapturableLandUnit() const;
	DllExport UnitTypes getUnitType() const;
	DllExport CvUnitInfo &getUnitInfo() const;
	UnitClassTypes getUnitClassType() const;

	DllExport UnitTypes getLeaderUnitType() const;
	void setLeaderUnitType(UnitTypes leaderUnitType);

	DllExport CvUnit* getCombatUnit() const;
	void setCombatUnit(CvUnit* pUnit, bool bAttacking = false);
	DllExport CvPlot* getPostCombatPlot() const;
	void setPostCombatPlot(Coordinates coord);
	DllExport CvUnit* getTransportUnit() const;
	bool isCargo() const;
	bool setTransportUnit(CvUnit* pTransportUnit, bool bUnload = true);
	int getExtraDomainModifier(DomainTypes eIndex) const;
	void changeExtraDomainModifier(DomainTypes eIndex, int iChange);
	DllExport const CvWString getName(uint uiForm = 0) const;
	const wchar* getNameKey() const;
	const CvWString getNameNoDesc() const;
	void setName(const CvWString szNewValue);
	const CvWString getNameAndProfession() const;
	const wchar* getNameOrProfessionKey() const;

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	int getTerrainDoubleMoveCount(TerrainTypes eIndex) const;
	bool isTerrainDoubleMove(TerrainTypes eIndex) const;
	void changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange);

	int getFeatureDoubleMoveCount(FeatureTypes eIndex) const;
	bool isFeatureDoubleMove(FeatureTypes eIndex) const;
	void changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange);
	int getExtraTerrainAttackPercent(TerrainTypes eIndex) const;
	void changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange);
	int getExtraTerrainDefensePercent(TerrainTypes eIndex) const;
	void changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange);
	int getExtraFeatureAttackPercent(FeatureTypes eIndex) const;
	void changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange);
	int getExtraFeatureDefensePercent(FeatureTypes eIndex) const;
	void changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange);
	int getExtraUnitClassAttackModifier(UnitClassTypes eIndex) const;
	void changeExtraUnitClassAttackModifier(UnitClassTypes eIndex, int iChange);
	int getExtraUnitClassDefenseModifier(UnitClassTypes eIndex) const;
	void changeExtraUnitClassDefenseModifier(UnitClassTypes eIndex, int iChange);
	int getExtraUnitCombatModifier(UnitCombatTypes eIndex) const;
	void changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange);
	bool canAcquirePromotion(PromotionTypes ePromotion) const;
	bool canAcquireNegativePromotion(PromotionTypes ePromotion) const; //WTP, ray Negative Promotions - START
	void acquireAnyNegativePromotion(); //WTP, ray Negative Promotions - START
	void cleanseAllNegativePromotions(); //WTP, ray Negative Promotions - START
	bool hasNegativePromotion() const; //WTP, ray Negative Promotions - START
	bool testWillGetNegativePromotion() const;
	bool canAcquirePromotionAny() const;
	bool isPromotionValid(PromotionTypes ePromotion) const;
	bool isHasPromotion(PromotionTypes eIndex) const;
	bool isHasRealPromotion(PromotionTypes eIndex) const;
	void setHasRealPromotion(PromotionTypes eIndex, bool bValue);
	void changeFreePromotionCount(PromotionTypes eIndex, int iChange);
	void setFreePromotionCount(PromotionTypes eIndex, int iValue);
	void resetPromotions();
	void setPromotions(PromotionTypes ePromotion = NO_PROMOTION);

	int getFreePromotionCount(PromotionTypes eIndex) const;

	int getSubUnitCount() const;
	DllExport int getSubUnitsAlive() const;
	int getSubUnitsAlive(int iDamage) const;

	DllExport bool isEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL) const;
	bool isPotentialEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL) const;

	int getTriggerValue(EventTriggerTypes eTrigger, const CvPlot* pPlot, bool bCheckPlot) const;
	bool canApplyEvent(EventTypes eEvent) const;
	void applyEvent(EventTypes eEvent);
	int getImmobileTimer() const;
	void setImmobileTimer(int iNewValue);
	void changeImmobileTimer(int iChange);

	bool potentialWarAction(const CvPlot* pPlot) const;

	bool isAlwaysHostile(const CvPlot* pPlot) const;

	bool verifyStackValid();

	void setYieldStored(int iYieldAmount);
	int getYieldStored() const;
	YieldTypes getYield() const;
	bool isGoods() const;
	bool hasAnyUnitInCargo() const;
	bool isYield() const;

	void changeBadCityDefenderCount(int iChange);
	int getBadCityDefenderCount() const;
	bool isCityDefender() const;
	void changeUnarmedCount(int iChange);
	int getUnarmedCount() const;

	int getUnitTravelTimer() const;
	void setUnitTravelTimer(int iValue);
	UnitTravelStates getUnitTravelState() const;
	void setUnitTravelState(UnitTravelStates eState, bool bShowEuropeScreen);

	bool setSailEurope(EuropeTypes eEurope);
	bool canSailEurope(EuropeTypes eEurope) const;

	void setHomeCity(CvCity* pNewValue);
	CvCity* getHomeCity() const;

	DllExport bool isOnMap() const;
	const CvArtInfoUnit* getArtInfo(int i) const;
	DllExport char const* getButton() const;
	char const* getFullLengthIcon() const;

	bool isColonistLocked();
	void setColonistLocked(bool bNewValue);

	// < JAnimals Mod Start >
	bool isBarbarian() const;
	void setBarbarian(bool bNewValue);
	// < JAnimals Mod End >
	// TAC - Trade Routes Advisor - koma13 - START
	bool isIgnoreDanger() const;
	void setIgnoreDanger(bool bNewValue);
	// TAC - Trade Routes Advisor - koma13 - END

	bool raidWeapons(CvCity* pCity);
	bool raidWeapons(CvUnit* pUnit);
	bool raidGoods(CvCity* pCity);


	// R&R, ray, Natives raiding party - START
	bool raidTreasury(CvCity* pCity);
	bool raidBuilding(CvCity* pCity);
	bool raidProduction(CvCity* pCity);
	bool raidScalp(CvCity* pCity);
	bool raidHarbor(CvCity* pCity);

	bool raidCity(CvCity* pCity);
	// R&R, ray, Natives raiding party - END

	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);
	void resetSavedData(int iID, UnitTypes eUnit, PlayerTypes eOwner, bool bConstructorCall);
	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);

	virtual void AI_init() = 0;
	virtual void AI_uninit() = 0;
	virtual void AI_reset() = 0;
	virtual bool AI_update() = 0;
	virtual bool AI_portUpdate() = 0;
	virtual bool AI_follow() = 0;
	virtual void AI_upgrade() = 0;
	virtual void AI_promote() = 0;
	virtual int AI_groupFirstVal() = 0;
	virtual int AI_groupSecondVal() = 0;
	virtual int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy) const = 0;
	virtual bool AI_bestCityBuild(CvCity* pCity, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, CvPlot* pIgnorePlot = NULL, CvUnit* pUnit = NULL) = 0;
	virtual bool AI_isCityAIType() const = 0;
	virtual UnitAITypes AI_getUnitAIType() const = 0;
	virtual void AI_setUnitAIType(UnitAITypes eNewValue) = 0;
	virtual UnitAIStates AI_getUnitAIState() const = 0;
	virtual void AI_setUnitAIState(UnitAIStates eNewValue) = 0;
	virtual bool AI_hasAIChanged(int iNumTurns) const = 0;
	virtual int AI_sacrificeValue(const CvPlot* pPlot) const = 0;
	virtual CvPlot* AI_determineDestination(CvPlot** ppMissionPlot, MissionTypes* peMission, MissionAITypes* peMissionAI) = 0;
	virtual bool AI_moveFromTransport(CvPlot* pHintPlot) = 0;
	virtual bool AI_attackFromTransport(CvPlot* pHintPlot, int iLowOddsThreshold, int iHighOddsThreshold) = 0;
	virtual int AI_getMovePriority() const = 0;
	virtual void AI_doInitialMovePriority() = 0;
	virtual void AI_setMovePriority(int iNewValue) = 0;
	virtual void AI_doFound() = 0;
	virtual ProfessionTypes AI_getOldProfession() const = 0;
	virtual void AI_setOldProfession(ProfessionTypes eProfession) = 0;
	virtual ProfessionTypes AI_getIdealProfession() const = 0;

	/*** TRIANGLETRADE 10/15/08 by DPII ***/
	bool canSailToAfrica(const CvPlot* pPlot, UnitTravelStates eNewState = NO_UNIT_TRAVEL_STATE) const;
	void sailToAfrica(UnitTravelStates eNewState = NO_UNIT_TRAVEL_STATE);
	// R&R, ray, Port Royal
	bool canSailToPortRoyal(const CvPlot* pPlot, UnitTravelStates eNewState = NO_UNIT_TRAVEL_STATE) const;
	void sailToPortRoyal(UnitTravelStates eNewState = NO_UNIT_TRAVEL_STATE);

	// WTP, ray, prevent Coastal Ships to Display EUROPE, AFRICA and Port Royal in GO-TO -START
	int canCrossCoastOnly() const;
	// WTP, ray, prevent Coastal Ships to Display EUROPE, AFRICA and Port Royal in GO-TO - END

	bool isProfessionalMilitary() const;

	// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - START
	void spawnOwnPlayerUnitOnPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;
	void spawnBarbarianUnitOnPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;
	void spawnOwnPlayerUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;
	void spawnBarbarianUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;

	bool isOwnPlayerUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;
	bool isBarbarianUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const;
	// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - END

	bool isShipTravelState(UnitTravelStates state) const;
protected:

	void updateVisibilityCache(int iNewRange);


	int m_iID;
	int m_iGroupID;
	int m_iHotKeyNumber;
	// int m_iX;
	// int m_iY;
	Coordinates m_coord;
	int m_iLastMoveTurn;
	int m_iGameTurnCreated;
	int m_iDamage;
	int m_iMoves;
	int m_iExperience;
	int m_iLevel;
	int m_iCargo;
	int m_iCargoCapacity;
	int m_iAttackPlotX;
	int m_iAttackPlotY;
	int m_iCombatTimer;
	int m_iCombatDamage;
	int m_iFortifyTurns;
	int m_iBlitzCount;
	int m_iAmphibCount;
	int m_iRiverCount;
	int m_iEnemyRouteCount;
	int m_iAlwaysHealCount;
	int m_iHillsDoubleMoveCount;
	int m_iVisibilityRange;
	int m_iExtraMoves;
	int m_iExtraMoveDiscount;
	int m_iExtraWithdrawal;
	int m_iExtraBombardRate;
	int m_iExtraEnemyHeal;
	int m_iExtraNeutralHeal;
	int m_iExtraFriendlyHeal;
	int m_iSameTileHeal;
	int m_iAdjacentTileHeal;
	int m_iExtraCombatPercent;
	int m_iExtraCityAttackPercent;
	int m_iExtraCityDefensePercent;
	int m_iExtraHillsAttackPercent;
	int m_iExtraHillsDefensePercent;
	int m_iExtraDomesticBonusPercent;
	int m_iPillageChange;
	int m_iAnimalGoldChange; //WTP, ray, Animal Promotions increase gold from Animals
	int m_iSlaveRevoltReductionBonus; //WTP, ray, Slave Hunter and Slave Master
	int m_iSlaveWorkerProductionBonus; //WTP, ray, Slave Hunter and Slave Master
	int m_iAdditionalLawToCityUnit; // WTP, ray, Lawkeeper Promotion - START
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iBaseCombat;
	DirectionTypes m_eFacingDirection;
	int m_iImmobileTimer;
	int m_iYieldStored;
	int m_iExtraWorkRate;
	int m_iUnitTravelTimer;
	int m_iBadCityDefenderCount;
	int m_iUnarmedCount;

	bool m_bMadeAttack;
	bool m_bPromotionReady;
	bool m_bDeathDelay;
	bool m_bCombatFocus;
	bool m_bInfoBarDirty;
	bool m_bColonistLocked;
	// < JAnimals Mod Start >
	bool m_bBarbarian;
	// < JAnimals Mod End >
	//TAC Whaling, ray
	bool m_bGatheringResource;
	//End TAC Whaling, ray
	// TAC - Trade Routes Advisor - koma13 - START
	bool m_bIgnoreDanger;
	// TAC - Trade Routes Advisor - koma13 - END

	PlayerTypes m_eOwner;
	PlayerTypes m_eCapturingPlayer;
	UnitTypes m_eUnitType;
	UnitTypes m_eLeaderUnitType;
	CvUnitInfo *m_pUnitInfo;
	ProfessionTypes m_eProfession;

	// TAC - LbD - Ray - START
	int m_iLbDrounds;
	ProfessionTypes m_eLastProfession;
	// TAC - LbD - Ray - END

	// WTP, ray, saving 1 more Profession for Fisher Issue - START
	int m_iLbDroundsBefore;
	ProfessionTypes m_eLastProfessionBefore;
	// WTP, ray, saving 1 more Profession for Fisher Issue - END

	//ray18
	int m_iMoneyToBuyLand;
	PlayerTypes m_ePlayerToBuyLand;
	//ray18 End

	// R&R, ray, Natives Trading - START
	int m_iAmountForNativeTrade;
	YieldTypes m_eYieldForNativeTrade;
	// R&R, ray, Natives Trading - END

	UnitTravelStates m_eUnitTravelState;

	IDInfo m_combatUnit;
	IDInfo m_transportUnit;
	IDInfo m_homeCity;
	int m_iPostCombatPlotIndex;

	DomainArray<int> m_ja_iExtraDomainModifier;

	CvWString m_szName;
	CvString m_szScriptData;

	EnumMap<PromotionTypes, bool> m_embHasRealPromotion;
	PromotionArray<int> m_ja_iFreePromotionCount;
	TerrainArray<int> m_ja_iTerrainDoubleMoveCount;
	FeatureArray<int> m_ja_iFeatureDoubleMoveCount;
	TerrainArray<int> m_ja_iExtraTerrainAttackPercent;
	TerrainArray<int> m_ja_iExtraTerrainDefensePercent;
	FeatureArray<int> m_ja_iExtraFeatureAttackPercent;
	FeatureArray<int> m_ja_iExtraFeatureDefensePercent;
	UnitClassArray<int> m_ja_iExtraUnitClassAttackModifier;
	UnitClassArray<int> m_ja_iExtraUnitClassDefenseModifier;
	UnitCombatArray<int> m_ja_iExtraUnitCombatModifier;

	bool canAdvance(const CvPlot* pPlot, int iThreshold) const;

	int planBattle( CvBattleDefinition & kBattleDefinition ) const;
	int computeUnitsToDie( const CvBattleDefinition & kDefinition, bool bRanged, BattleUnitTypes iUnit ) const;
	bool verifyRoundsValid( const CvBattleDefinition & battleDefinition ) const;
	void increaseBattleRounds( CvBattleDefinition & battleDefinition ) const;
	int computeWaveSize( bool bRangedRound, int iAttackerMax, int iDefenderMax ) const;

	void getDefenderCombatValues(CvUnit& kDefender, const CvPlot* pPlot, int iOurStrength, int iOurFirepower, int& iTheirOdds, int& iTheirStrength, int& iOurDamage, int& iTheirDamage, CombatDetails* pTheirDetails = NULL) const;

	bool isCombatVisible(const CvUnit* pDefender) const;
	void resolveCombat(CvUnit* pDefender, CvPlot* pPlot, CvBattleDefinition& kBattle);

	void doUnitTravelTimer();
	void processPromotion(PromotionTypes ePromotion, int iChange);
	UnitCombatTypes getProfessionUnitCombatType(ProfessionTypes eProfession) const;
	void processUnitCombatType(UnitCombatTypes eUnitCombat, int iChange);
	void doUnloadYield(int iAmount);
	bool raidWeapons(std::vector<int>& aYields);
	bool isPrisonerOrSlave() const;

// unit yield cache - start - Nightinggale
protected:
	void updateYieldCache();
	YieldTypes getYieldUncached() const;
	YieldTypes m_eCachedYield;
// unit yield cache - end - Nightinggale
	int getCargoValue(Port port) const;
	// WTP, ray, prevent Coastal Ships to Display EUROPE, AFRICA and Port Royal in GO-TO - START
	//int canCrossCoastOnly() const;
	// WTP, ray, prevent Coastal Ships to Display EUROPE, AFRICA and Port Royal in GO-TO - END

	EnumMap<PromotionTypes, bool> m_embisPromotionApplied;

public:
	int getForcedLaborFactor() const;
	int getDiscriminationFactor() const;

	bool isForcedLaborer() const;
};

inline bool CvUnit::isHasPromotion(PromotionTypes eIndex) const
{
	return m_embisPromotionApplied.get(eIndex);
}

#endif
