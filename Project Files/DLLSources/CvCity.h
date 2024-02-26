#pragma once
// city.h
#ifndef CIV4_CITY_H
#define CIV4_CITY_H
#include "CvDLLEntity.h"
#include "LinkedList.h"
#include "CvPlotFunctions.h"
#include "CvCityYields.h"

class CvPlot;
class CvArea;
class CvGenericBuilding;
class CvInfoBase;

class CvCity : public CvDLLEntity
{
public:

	void UpdateBuildingAffectedCache(); // building affected cache - Nightinggale
/** NBMOD EDU **/

// NBMOD EDU cache - start - Nightinggale
protected:
	void NBMOD_SetCityTeachLevelCache();
	int m_NBMOD_CityTeachLevel_cache; // nosave cache

public:
// NBMOD EDU cache - end - Nightinggale
	int NBMOD_GetCityTeachLevel() const;

/** NBMOD EDU **/

// cache getMaxYieldCapacity - start - Nightinggale
protected:
	int getMaxYieldCapacityUncached() const;
	int m_cache_MaxYieldCapacity; // nosave cache
public:
// cache getMaxYieldCapacity - end - Nightinggale

	CvCity();
	virtual ~CvCity();
	//void init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits);
	void init(int iID, PlayerTypes eOwner, Coordinates initCoord, bool bBumpUnits);
	void uninit();
	//void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, int iX = 0, int iY = 0, bool bConstructorCall = false);
	void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, Coordinates resetCoord = Coordinates(0, 0), bool bConstructorCall = false);
	void setupGraphical();
	void kill();

	// PatchMod: Achievements START
	bool isHasSpecialBuilding(int iValue) const;
	// PatchMod: Achievements END

	// TAC - LbD - Ray - START
	void doLbD();
	bool LbD_try_become_expert(CvUnit* convUnit, int base, int increase, int pre_rounds, int l_level, int chance_increase_expert_from_teacher, int pre_rounds_expert_decrease_from_teacher);
	bool LbD_try_get_free(CvUnit* convUnit, int base, int increase, int pre_rounds, int mod_crim, int mod_serv, int l_level);
	bool LbD_try_escape(CvUnit* convUnit, int base, int mod_crim, int mod_serv);
	// TAC - LbD - Ray - END

	// WTP, ray, LbD Slaves Revolt and Free - START
	bool LbD_try_revolt(CvUnit* convUnit, int base, int mod_crim, int mod_slave);
	// WTP, ray, LbD Slaves Revolt and Free - END

	// R&R, ray, Extra City Defense Attacks - START
	void doExtraCityDefenseAttacks();
	// R&R, ray, Extra City Defense Attacks - END

	void doTurn();
	bool isCitySelected() const;
	DllExport bool canBeSelected() const;
	DllExport void updateSelectedCity();
	void updateYield();
	void updateVisibility();
	void createGreatGeneral(UnitTypes eGreatGeneralUnit, bool bIncrementExperience);
	void createGreatAdmiral(UnitTypes eGreatAdmiralUnit, bool bIncrementExperience); // R&R, ray, Great Admirals

	// WTP, ray, Lieutenants and Captains - START
	void createBraveLieutenant(UnitTypes eGreatAdmiralUnit);
	void createCapableCaptain(UnitTypes eGreatAdmiralUnit);
	// WTP, ray, Lieutenants and Captains - END

	DllExport void doTask(TaskTypes eTask, int iData1 = -1, int iData2 = -1, bool bOption = false, bool bAlt = false, bool bShift = false, bool bCtrl = false);

	DllExport void chooseProduction(UnitTypes eTrainUnit = NO_UNIT, BuildingTypes eConstructBuilding = NO_BUILDING, bool bFinish = false, bool bFront = false);

	DllExport int getCityPlotIndex(const CvPlot* pPlot) const;
	CvPlot* getCityIndexPlot(CityPlotTypes eCityPlot) const;

	bool canWork(const CvPlot* pPlot) const;
	void verifyWorkingPlot(int iPlotIndex);
	void verifyWorkingPlots();
	int getNumAvailableWorkPlots() const;
	void clearWorkingOverride(int iIndex);
	int countNumImprovedPlots(ImprovementTypes eImprovement = NO_IMPROVEMENT, bool bPotential = false) const;
	int countNumWaterPlots() const;
	int countNumRiverPlots() const;

	int getNumProfessionBuildingSlots(ProfessionTypes eProfession) const;
	bool isAvailableProfessionSlot(ProfessionTypes eProfession, const CvUnit* pUnit) const;

	//Androrc Multiple Professions per Building
	bool isAvailableBuildingSlot(BuildingTypes eBuilding, const CvUnit* pUnit) const;
	//Androrc End

	int professionCount(ProfessionTypes eProfession) const;

	int findPopulationRank() const;
	int findBaseYieldRateRank(YieldTypes eYield) const;
	int findYieldRateRank(YieldTypes eYield) const;

	UnitTypes allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount = 0) const;

	DllExport bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bIgnoreUpgrades = false) const;
	bool canTrain(UnitCombatTypes eUnitCombat) const;
	DllExport bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false) const;
	DllExport bool canConvince(FatherPointTypes eFatherPoint, bool bContinue = false, bool bTestVisible = false) const;

	int getFoodTurnsLeft() const;
	DllExport bool isProduction() const;
	bool isProductionUnit() const;
	bool isProductionBuilding() const;
	bool isProductionConvince() const;

	bool canContinueProduction(OrderData order) const;
	int getProductionExperience(UnitTypes eUnit = NO_UNIT) const;
	void addProductionExperience(CvUnit* pUnit);

	UnitTypes getProductionUnit() const;
	UnitAITypes getProductionUnitAI() const;
	BuildingTypes getProductionBuilding() const;
	FatherPointTypes getProductionFatherPoint() const;
	const CvInfoBase* getProductionInfo() const;
	const wchar* getProductionName() const;
	const wchar* getProductionNameKey() const;
	int getGeneralProductionTurnsLeft() const;

	int getFirstUnitOrder(UnitTypes eUnit) const;
	int getFirstBuildingOrder(BuildingTypes eBuilding) const;
	int getNumTrainUnitAI(UnitAITypes eUnitAI) const;

	int getProduction() const;
	int getProductionNeeded(YieldTypes eYield) const;
	int getYieldProductionNeeded(UnitTypes eUnit, YieldTypes eYield) const;
	int getYieldProductionNeeded(BuildingTypes eBuilding, YieldTypes eYield) const;
	int getProductionTurnsLeft() const;
	DllExport int getProductionTurnsLeft(UnitTypes eUnit, int iNum) const;
	DllExport int getProductionTurnsLeft(BuildingTypes eBuilding, int iNum) const;
	int getProductionTurnsLeft(int iProductionNeeded, int iProduction, int iFirstProductionDifference, int iProductionDifference) const;
	void setProduction(int iNewValue);
	void changeProduction(int iChange);

	int getProductionModifier() const;
	int getProductionModifier(UnitTypes eUnit) const;
	int getProductionModifier(BuildingTypes eBuilding) const;

	int getCurrentProductionDifference(bool bOverflow) const;

	DllExport bool canHurry(HurryTypes eHurry, bool bTestVisible = false) const;
	void hurry(HurryTypes eHurry);

	void processBuilding(BuildingTypes eBuilding, int iChange);
	HandicapTypes getHandicapType() const;
	DllExport CivilizationTypes getCivilizationType() const;
	LeaderHeadTypes getPersonalityType() const;
	ArtStyleTypes getArtStyleType() const;
	DllExport CitySizeTypes getCitySizeType() const;

	bool hasTrait(TraitTypes eTrait) const;
	bool isHuman() const;
	bool isNative() const;
	DllExport bool isVisible(TeamTypes eTeam, bool bDebug) const;

	DllExport bool isCapital() const;
	bool isCoastal(int iMinWaterSize) const;
	bool isDisorder() const;

	int extraPopulation() const;

	int foodConsumption(int iExtra = 0) const;
	int foodDifference() const;
	int growthThreshold() const;

	int productionLeft() const;
	int hurryCost(bool bExtra, bool bIgnoreNew) const;
	int getHurryCostModifier(bool bIgnoreNew = false) const;
	DllExport int hurryGold(HurryTypes eHurry) const;
	DllExport int hurryPopulation(HurryTypes eHurry) const;
	int hurryProduction(HurryTypes eHurry) const;
	int maxHurryPopulation() const;
	int hurryYield(HurryTypes eHurry, YieldTypes eYield) const;

	CultureLevelTypes cultureDistance(int iDX, int iDY) const;

	// Custom_House_Mod Start
	bool isBestPortCity() const;
	// Custom_House_Mod End

	bool isHasBuildingClass(BuildingClassTypes eIndex) const;
	DllExport bool isHasBuilding(BuildingTypes eIndex) const;
	int getProfessionOutput(ProfessionTypes eProfession, const CvUnit* pUnit, BuildingTypes* peBuilding = NULL) const;
	int getProfessionInput(ProfessionTypes eProfession, const CvUnit* pUnit) const;
	BuildingTypes getYieldBuilding(YieldTypes eYield) const;
	DllExport int getID() const;
	int getIndex() const;
	DllExport IDInfo getIDInfo() const;
	void setID(int iID);
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
	bool at(int iX, int iY) const;
	bool at(Coordinates coord) const;
	bool at(const CvPlot* pPlot) const;
	DllExport CvPlot* plot() const;
	int getArea() const;
	DllExport CvArea* area() const;
	CvArea* waterArea() const;

	CvPlot* getRallyPlot() const;
	void setRallyPlot(CvPlot* pPlot);
	int getGameTurnFounded() const;
	void setGameTurnFounded(int iNewValue);
	int getGameTurnAcquired() const;
	void setGameTurnAcquired(int iNewValue);
	int getPopulation() const;
	void setPopulation(int iNewValue);
	void changePopulation(int iChange);
	void updatePopulation(int iOldPopulation);
	long getRealPopulation() const;

	// WTP, ray, Native Trade Posts - START
	int getNativeTradePostGold() const;
	void setNativeTradePostGold (int iNewValue);
	void changeNativeTradePostGold(int iChange);
	// WTP, ray, Native Trade Posts - END

	int getHighestPopulation() const;
	void setHighestPopulation(int iNewValue);

	int getWorksWaterCount() const;
	bool isWorksWater() const;
	void changeWorksWaterCount(int iChange);

	int getHealRate() const;
	void changeHealRate(int iChange);
	int getFood() const;
	void setFood(int iNewValue, bool bAllowNegative = false);
	void changeFood(int iChange, bool bAllowNegative = false);

	int getFoodKept() const;
	void setFoodKept(int iNewValue);
	void changeFoodKept(int iChange);
	int getMaxFoodKeptPercent() const;
	void changeMaxFoodKeptPercent(int iChange);
	int getOverflowProduction() const;
	void setOverflowProduction(int iNewValue);
	void changeOverflowProduction(int iChange, int iProductionModifier);
	int getMilitaryProductionModifier() const;
	void changeMilitaryProductionModifier(int iChange);

	int getBuildingDefense() const;
	void changeBuildingDefense(int iChange);
	int getBuildingBombardDefense() const;
	void changeBuildingBombardDefense(int iChange);
	int getFreeExperience() const;
	void changeFreeExperience(int iChange);

	int getDefenseDamage() const;
	void changeDefenseDamage(int iChange);
	void changeDefenseModifier(int iChange);

	int getLastDefenseDamage() const;
	void setLastDefenseDamage(int iNewValue);
	bool isBombardable(const CvUnit* pUnit) const;
	int getTotalDefense() const;
	int getDefenseModifier() const;

	int getOccupationTimer() const;
	bool isOccupation() const;
	void setOccupationTimer(int iNewValue);
	void changeOccupationTimer(int iChange);

	int getCultureUpdateTimer() const;
	void setCultureUpdateTimer(int iNewValue);
	void changeCultureUpdateTimer(int iChange);

	int getCitySizeBoost() const;
	void setCitySizeBoost(int iBoost);
	bool isNeverLost() const;
	void setNeverLost(bool bNewValue);

	bool isBombarded() const;
	void setBombarded(bool bNewValue);

	void setAllCitizensAutomated(bool bAutomated);
	void setCitizenAutomated(int iUnitID);

	bool isProductionAutomated() const;
	void setProductionAutomated(bool bNewValue, bool bClear);

	/* allows you to programatically specify a cities walls rather than having them be generated automagically */
	DllExport bool isWallOverride() const;
	void setWallOverride(bool bOverride);
	DllExport bool isBillboardDirty() const;
	DllExport void setBillboardDirty(bool bNewValue);
	DllExport bool isLayoutDirty() const;
	DllExport void setLayoutDirty(bool bNewValue);

	DllExport PlayerTypes getOwner() const;
	inline PlayerTypes getOwnerINLINE() const
	{
		return m_eOwner;
	}
	DllExport TeamTypes getTeam() const;

	PlayerTypes getPreviousOwner() const;
	void setPreviousOwner(PlayerTypes eNewValue);
	PlayerTypes getOriginalOwner() const;
	void setOriginalOwner(PlayerTypes eNewValue);
	CultureLevelTypes getCultureLevel() const;
	int getCultureThreshold() const;
	int getCultureThreshold(CultureLevelTypes eLevel) const;
	void setCultureLevel(CultureLevelTypes eNewValue);
	void updateCultureLevel();
	int getLandPlotYield(YieldTypes eIndex) const; // R&R, ray, Landplot Yields
	void changeLandPlotYield(YieldTypes eIndex, int iChange); // R&R, ray, Landplot Yields
	int getSeaPlotYield(YieldTypes eIndex) const;
	void changeSeaPlotYield(YieldTypes eIndex, int iChange);
	int getRiverPlotYield(YieldTypes eIndex) const;
	void changeRiverPlotYield(YieldTypes eIndex, int iChange);
	int getRawYieldProduced(YieldTypes eIndex) const;
	int getRawYieldConsumed(YieldTypes eIndex) const;
	int getBaseYieldRateModifier(YieldTypes eIndex, int iExtra = 0) const;
	int getYieldRate(YieldTypes eIndex) const;
	int getCultureRate() const;
	void setYieldRateDirty();
	int getYieldRateModifier(YieldTypes eIndex) const;
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);
	// TAC - AI Economy - koma13 - START
	int getProductionOutput(ProfessionTypes eProfession) const;
	int getPotentialProductionOutput(ProfessionTypes eProfession) const;
	bool hasOtherProductionBuilding(BuildingTypes eBuilding, int iMax) const;
	// TAC - AI Economy - koma13 - END
	int getDomainFreeExperience(DomainTypes eIndex) const;
	void changeDomainFreeExperience(DomainTypes eIndex, int iChange);
	int getDomainProductionModifier(DomainTypes eIndex) const;
	void changeDomainProductionModifier(DomainTypes eIndex, int iChange);
	int getCulture(PlayerTypes eIndex) const;
	int countTotalCulture() const;
	PlayerTypes findHighestCulture() const;
	int calculateCulturePercent(PlayerTypes eIndex) const;
	int calculateTeamCulturePercent(TeamTypes eIndex) const;
	void setCulture(PlayerTypes eIndex, int iNewValue, bool bPlots);
	void changeCulture(PlayerTypes eIndex, int iChange, bool bPlots);
	int getTotalYieldStored() const; // In python
	void setTotalYieldStored(int iValue);
	void changeTotalYieldStored(int iChange);
	int getYieldStored(YieldTypes eYield) const;
	void setYieldStored(YieldTypes eYield, int iValue);
	void changeYieldStored(YieldTypes eYield, int iChange);
	int getYieldRushed(YieldTypes eYield) const;
	void changeYieldRushed(YieldTypes eYield, int iChange);
	void calculateNetYields(int aiYields[], int* aiProducedYields = NULL, int* aiConsumedYields = NULL, bool bPrintWarning = false) const;
	int calculateNetYield(YieldTypes eYield) const;
	int calculateActualYieldProduced(YieldTypes eYield) const;
	int calculateActualYieldConsumed(YieldTypes eYield) const;

	// R&R, Androrc, Domestic Market
	int getYieldBuyPriceUnmodified(YieldTypes eYield) const;
	int getYieldBuyPrice(YieldTypes eYield) const;
	void setYieldBuyPrice(YieldTypes eYield, int iPrice);
	int getYieldDemand(YieldTypes eYield) const;
	void getYieldDemands(YieldCargoArray<int> &aYields) const;
	//Androrc End

	bool isEverOwned(PlayerTypes eIndex) const;
	void setEverOwned(PlayerTypes eIndex, bool bNewValue);
	DllExport bool isRevealed(TeamTypes eIndex, bool bDebug) const;
	void setRevealed(TeamTypes eIndex, bool bNewValue);

	DllExport const CvWString getName(uint uiForm = 0) const;
	DllExport const wchar* getNameKey() const;
	void setName(const wchar* szNewValue, bool bFound = false);
	void doFoundMessage();
	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	int getBuildingProduction(BuildingTypes eIndex) const;
	void setBuildingProduction(BuildingTypes eIndex, int iNewValue);
	void changeBuildingProduction(BuildingTypes eIndex, int iChange);

	int getBuildingProductionTime(BuildingTypes eIndex) const;
	void setBuildingProductionTime(BuildingTypes eIndex, int iNewValue);
	void changeBuildingProductionTime(BuildingTypes eIndex, int iChange);

	PlayerTypes getBuildingOriginalOwner(BuildingTypes eIndex) const;
	int getBuildingOriginalTime(BuildingTypes eIndex) const;

	int getUnitProduction(UnitTypes eIndex) const;
	void setUnitProduction(UnitTypes eIndex, int iNewValue);
	void changeUnitProduction(UnitTypes eIndex, int iChange);

	int getUnitProductionTime(UnitTypes eIndex) const;
	void setUnitProductionTime(UnitTypes eIndex, int iNewValue);
	void changeUnitProductionTime(UnitTypes eIndex, int iChange);

	int getUnitCombatFreeExperience(UnitCombatTypes eIndex) const;
	void changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange);
	int getFreePromotionCount(PromotionTypes eIndex) const;
	bool isFreePromotion(PromotionTypes eIndex) const;
	void changeFreePromotionCount(PromotionTypes eIndex, int iChange);
	CvUnit* getUnitWorkingPlot(CityPlotTypes ePlotIndex) const;
	bool isPlotProducingYields(CityPlotTypes ePlotIndex) const;
	bool isUnitWorkingAnyPlot(const CvUnit* pUnit) const;
	CvUnit* getUnitWorkingPlot(const CvPlot* pPlot) const;
	bool isPlotProducingYields(const CvPlot* pPlot) const;
	void clearUnitWorkingPlot(int iPlotIndex);
	void clearUnitWorkingPlot(CvPlot* pPlot);
	void alterUnitWorkingPlot(int iPlotIndex, int iUnitId, bool bAskProfession);
	//Androrc Multiple Professions per Building
	void alterUnitWorkingBuilding(BuildingTypes eBuilding, int iUnitId, bool bAskProfession);
	//Androrc End
	void alterUnitProfession(int iUnitId, ProfessionTypes eProfession);
	void ejectToTransport(int iUnitId, int iTransportId);
	void replaceCitizen(int iUnitId, int iReplacedUnitId, bool bAskProfession);
	bool isHasRealBuilding(BuildingTypes eIndex) const;
	void setHasRealBuilding(BuildingTypes eIndex, bool bNewValue);
	void setHasRealBuildingTimed(BuildingTypes eIndex, bool bNewValue, bool bFirst, PlayerTypes eOriginalOwner, int iOriginalTime);
	bool isValidBuildingLocation(const CvBuildingInfo& kBuilding) const;
	bool isHasFreeBuilding(BuildingTypes eIndex) const;
	void setHasFreeBuilding(BuildingTypes eIndex, bool bNewValue);
	bool isHasConceptualBuilding(BuildingTypes eIndex) const;
	bool isDominantSpecialBuilding(BuildingTypes eIndex) const;
	BuildingTypes getDominantBuilding(SpecialBuildingTypes eSpecialBuilding) const;
	void clearOrderQueue();
	void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce = false);
	void pushOrder(OrderData order, bool bPop, bool bAppend, bool bForce = false);
	void popOrder(int iNum, bool bFinish = false, bool bChoose = false);
	bool processRequiredYields(int iNum);
	bool checkRequiredYields(OrderTypes eOrder, BuildingTypes, UnitTypes, YieldTypes eYieldException = NO_YIELD) const;
	bool checkRequiredYields(OrderTypes eOrder, int iData1, YieldTypes eYieldException = NO_YIELD) const;
	bool checkRequiredYields(OrderData, YieldTypes eYieldException = NO_YIELD) const;
	void checkCompletedBuilds(YieldTypes eYield, int iChange);
	void getOrdersWaitingForYield(std::vector< std::pair<OrderTypes, int> >& aOrders, YieldTypes eYield, bool bYieldsComplete, int iChange) const;
	void startHeadOrder();
	void stopHeadOrder();
	int getOrderQueueLength() const;
	OrderData* getOrderFromQueue(int iIndex);
	DllExport CLLNode<OrderData>* nextOrderQueueNode(CLLNode<OrderData>* pNode) const;
	DllExport CLLNode<OrderData>* headOrderQueueNode() const;
	CLLNode<OrderData>* tailOrderQueueNode() const;
	// fill the kVisible array with buildings that you want shown in city, as well as the number of generics
	// This function is called whenever CvCity::setLayoutDirty() is called
	DllExport void getVisibleBuildings(std::list<BuildingTypes>& kVisible, int& iNumGenerics) const;
	// Fill the kEffectNames array with references to effects in the CIV4EffectInfos.xml to have a
	// city play a given set of effects. This is called whenever the interface updates the city billboard
	// or when the zoom level changes
	DllExport void getVisibleEffects(ZoomLevelTypes eCurrentZoom, std::vector<char const*>& kEffectNames) const;

	// Billboard appearance controls
	DllExport void getCityBillboardSizeIconColors(NiColorA& kDotColor, NiColorA& kTextColor) const;
	DllExport char const* getCityBillboardProductionIcon() const;
	DllExport bool getCityBillboardTopBarValues(float& fStored, float& fRate, float& fRateExtra) const;
	DllExport bool getCityBillboardBottomBarValues(float& fStored, float& fRate, float& fRateExtra) const;

	void setWallOverridePoints(const std::vector< std::pair<float, float> >& kPoints); /* points are given in world space ... i.e. PlotXToPointX, etc */
	DllExport const std::vector< std::pair<float, float> >& getWallOverridePoints() const;
	int getTriggerValue(EventTriggerTypes eTrigger) const;
	bool canApplyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;
	void applyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData, bool bClear);
	bool isEventOccured(EventTypes eEvent) const;
	void setEventOccured(EventTypes eEvent, bool bOccured);
	int getBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const;
	void setBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange);
	void changeBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange);
	PlayerTypes getLiberationPlayer(bool bConquest) const;
	void liberate(bool bConquest);

	int getMaxYieldCapacity() const;
	bool isAutoRaze() const;
	void setScoutVisited(TeamTypes eTeam, bool bVisited);
	bool isScoutVisited(TeamTypes eTeam) const;
	GoodyTypes getGoodyType(const CvUnit* pUnit) const;
	int doGoody(CvUnit* pUnit, GoodyTypes eGoody);
	PlayerTypes getMissionaryPlayer() const;
	CivilizationTypes getMissionaryCivilization() const;
	void setMissionaryPlayer(PlayerTypes ePlayer, bool bBurnMessage = true);
	int getMissionaryRate() const;
	void setMissionaryRate(int iRate);

	// WTP, ray, Native Trade Posts - START
	PlayerTypes getTradePostPlayer() const;
	CivilizationTypes getTradePostCivilization() const;
	void setTradePostPlayer(PlayerTypes ePlayer, bool bBurnMessage = true);
	int getNativeTradeRate() const;
	void setNativeTradeRate(int iRate);
	// WTP, ray, Native Trade Posts - END

	// R&R, ray , Stirring Up Natives - START
	bool getHasBeenStirredUp() const;
	void setHasBeenStirredUp(bool stirredUp);
	// R&R, ray , Stirring Up Natives - END

	int getRebelPercent() const;
	int getRebelSentiment() const;
	void setRebelSentiment(int iValue);

	// R&R, ray, Health - START
	int getCityHealth() const;
	int getCityHealthChange() const;
	int getCityHealthChangeFromPopulation() const;
	void setCityHealth(int iValue);
	void changeCityHealth(int iValue);
	// R&R, ray, Health - END

	// WTP, ray, Health Overhaul - START
	int getCityHealthChangeFromCentralPlot() const;
	int getCityHealthChangeFromRessourcesInCityRadius() const;
	// WTP, ray, Health Overhaul - END

	// WTP, ray, new Harbour System - START
	int getCityHarbourSpace() const;
	void changeCityHarbourSpace(int iValue);
	int getCityHarbourSpaceUsed() const;
	bool bShouldShowCityHarbourSystem() const;
	// WTP, ray, new Harbour System - END

	// WTP, ray, new Barracks System - START
	int getCityBarracksSpace() const;
	void changeCityBarracksSpace(int iValue);
	int getCityBarracksSpaceUsed() const;
	bool bShouldShowCityBarracksSystem() const;
	// WTP, ray, new Barracks  System - END

	// WTP, ray, Improvements give Bonus to their City - START
	int getMonasteryCrossBonusForCity() const;
	int getFortDefenseBonusForCity() const;
	// WTP, ray, Improvements give Bonus to their City - END

	// WTP, ray, Improvements give Bonus to their City - PART 2 - START
	int getImprovementFoodModifierForCity() const;
	int getImprovementHammersModifierForCity() const;
	int getImprovementToolsModifierForCity() const;
	// WTP, ray, Improvements give Bonus to their City - PART 2 - END

	// WTP, ray, Improvements give Bonus to their City - PART 3 - START
	int getImprovementStorageModifierForCity() const;
	// WTP, ray, Improvements give Bonus to their City - PART 3 - END

	// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - START
	void spawnOwnPlayerUnitOnPlotOfCity(int /*UnitClassTypes*/ iIndex) const;
	void spawnBarbarianUnitOnPlotOfCity(int /*UnitClassTypes*/ iIndex) const; // careful with this, because will take over City for Barbarians
	void spawnOwnPlayerUnitOnAdjacentPlotOfCity(int /*UnitClassTypes*/ iIndex) const;
	void spawnBarbarianUnitOnAdjacentPlotOfCity(int /*UnitClassTypes*/ iIndex) const;

	bool isOwnPlayerUnitOnAdjacentPlotOfCity(int /*UnitClassTypes*/ iIndex) const;
	bool isBarbarianUnitOnAdjacentPlotOfCity(int /*UnitClassTypes*/ iIndex) const;
	// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - END

	// WTP, ray, Happiness - START
	int getCityHappiness() const;
	void setCityHappiness(int iValue);
	void updateCityHappiness();
	int getCityUnHappiness() const;
	void setCityUnHappiness(int iValue);
	void updateCityUnHappiness();

	int getUnhappinessFromPopulation() const;
	int getUnhappinessFromCrime() const;
	int getUnhappinessFromSlavery() const;
	int getUnhappinessFromWars() const;
	int getUnhappinessFromMissingDefense() const;

	int getHappinessFromCrosses() const;
	int getHappinessFromBells() const;
	int getHappinessFromHealth() const;
	int getHappinessFromCulture() const;
	int getHappinessFromLaw() const;
	int getHappinessFromEducation() const;
	int getHappinessFromDomesticDemandsFulfilled() const;
	int getHappinessFromTreaties() const;
	int getUnhappinessFromTaxRate() const;

	int getCityTimerFestivitiesOrUnrest() const;
	void setCityTimerFestivitiesOrUnrest(int iValue);
	void changeCityTimerFestivitiesOrUnrest(int iValue);
	// WTP, ray, Happiness - END

	// WTP, ray, Crime and Law - START
	int getCityLaw() const;
	void setCityLaw(int iValue);
	void updateCityLaw();
	int getCityCrime() const;
	void setCityCrime(int iValue);
	void updateCityCrime();

	int getLawFromCityDefenders() const;
	int getLawFromCrosses() const;

	int getCrimeFromPopulation() const;
	int getCrimeFromUnhappiness() const;
	int getCrimeFromWars() const;
	int getCrimBonusFactorFromOverflow() const;
	// WTP, ray, Crime and Law - END

	int getTeachUnitMultiplier() const;
	void setTeachUnitMultiplier(int iModifier);
	UnitClassTypes bestTeachUnitClass();

	UnitClassTypes bestGrowthUnitClass(); // WTP, ray, Ethnically correct Population Growth - START

	int getEducationThresholdMultiplier() const;
	void setEducationThresholdMultiplier(int iModifier);
	int educationThreshold() const;

	CvUnit* ejectBestDefender(CvUnit* pCurrentBest, CvUnit* pAttacker);
	CvUnit* getBestDefender(ProfessionTypes* peProfession, CvUnit* pCurrentBest, const CvUnit* pAttacker) const;

	bool canTradeAway(PlayerTypes eToPlayer) const;

	void resetSavedData(int iID, PlayerTypes eOwner, int iX, int iY, bool bConstructorCall);
	void resetSavedData(int iID, PlayerTypes eOwner, Coordinates resetCoord, bool bConstructorCall);
	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_doTurn() = 0;
	virtual void AI_doNative() = 0;
	virtual void AI_assignWorkingPlots() = 0;
	virtual void AI_updateAssignWork() = 0;
	virtual bool AI_avoidGrowth() const = 0;
	virtual void AI_setAvoidGrowth(bool bNewValue) = 0;
	virtual void AI_chooseProduction() = 0;
	virtual UnitTypes AI_bestUnit(bool bAsync = false, UnitAITypes* peBestUnitAI = NULL, bool bPickAny = false) const = 0;
	virtual UnitTypes AI_bestUnitAI(UnitAITypes eUnitAI, bool bAsync = false) const = 0;
	virtual BuildingTypes AI_bestBuilding(int iFocusFlags = 0, int iMaxTurns = MAX_INT, bool bAsync = false) const = 0;
	virtual int AI_buildingValue(BuildingTypes eBuilding, int iFocusFlags = 0) const = 0;
	virtual int AI_neededSeaWorkers() const = 0;
	virtual bool AI_isDefended(int iExtra = 0) const = 0;
	virtual bool AI_isDanger() const = 0;
	virtual int AI_neededDefenders() const = 0;
	virtual int AI_numDefenders(bool bDefenseOnly = true, bool bIncludePotential = true) const = 0;
	virtual int AI_minDefenders() const = 0;
	virtual bool AI_isEmphasizeAvoidGrowth() const = 0;
	virtual bool AI_isAssignWorkDirty() const = 0;
	virtual CvCity* AI_getRouteToCity() const = 0;
	virtual void AI_setAssignWorkDirty(bool bNewValue) = 0;
	virtual bool AI_isChooseProductionDirty() const = 0;
	virtual void AI_setChooseProductionDirty(bool bNewValue) = 0;
	virtual bool AI_isEmphasize(EmphasizeTypes eIndex) const = 0;
	virtual void AI_setEmphasize(EmphasizeTypes eIndex, bool bNewValue) = 0;
	virtual int AI_getEmphasizeYieldCount(YieldTypes eIndex) const = 0;
	virtual int AI_getBestBuildValue(int iIndex) const = 0;
	virtual int AI_totalBestBuildValue(CvArea* pArea) const = 0;
	virtual int AI_countBestBuilds(CvArea* pArea) const = 0;
	virtual BuildTypes AI_getBestBuild(int iIndex) const = 0;
	virtual void AI_updateBestBuild() = 0;
	virtual int AI_cityValue() const = 0;
	virtual int AI_clearFeatureValue(int iIndex) const = 0;
	virtual int AI_calculateCulturePressure() const = 0;
	virtual int AI_calculateWaterWorldPercent() const = 0;
	virtual int AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance = 7) const = 0;
	virtual int AI_cityThreat(bool bDangerPercent = false) const = 0;
	virtual BuildingTypes AI_bestAdvancedStartBuilding(int iPass) const = 0;
	virtual int AI_getWorkersHave() const = 0;
	virtual int AI_getWorkersNeeded() const = 0;
	virtual void AI_changeWorkersHave(int iChange) = 0;
	virtual bool AI_removeWorstPopulationUnit(bool bDelete) = 0;
	virtual CvUnit* AI_bestPopulationUnit(UnitAITypes eUnitAI, ProfessionTypes eProfession = NO_PROFESSION) = 0;
	virtual ProfessionTypes AI_bestPlotProfession(const CvUnit* pUnit, const CvPlot* pPlot) const = 0;
	virtual int AI_bestProfessionPlot(ProfessionTypes eProfession, const CvUnit* pUnit) const = 0;
	virtual int AI_professionValue(ProfessionTypes eProfession, const CvUnit* pUnit, const CvPlot* pPlot, const CvUnit* pDisplaceUnit) const = 0;
	virtual int AI_professionBasicOutput(ProfessionTypes eProfession, UnitTypes eUnit, const CvPlot* pPlot) const = 0;
	virtual int AI_unitJoinCityValue(CvUnit* pUnit, ProfessionTypes* peNewProfession) const = 0;
	virtual int AI_unitJoinReplaceValue(CvUnit* pUnit, CvUnit** pReplaceUnit = NULL) const = 0;
	virtual bool AI_canMakeGift() const = 0;
	virtual int AI_getGiftTimer() const = 0;
	virtual void AI_setGiftTimer(int iNewValue) = 0;
	virtual void AI_changeGiftTimer(int iChange) = 0;
	// R&R, ray, Natives Trading - START
	virtual bool AI_canMakeTrade() const = 0;
	virtual int AI_getTradeTimer() const = 0;
	virtual void AI_setTradeTimer(int iNewValue) = 0;
	virtual void AI_changeTradeTimer(int iChange) = 0;
	// R&R, ray, Natives Trading - END
	virtual int AI_maxGoldTrade(PlayerTypes ePlayer) const = 0;
	virtual int AI_estimateYieldValue(YieldTypes eYield, int iAmount) const = 0;
	virtual int AI_getTargetSize() const = 0;
	virtual void AI_setTargetSize(int iNewValue) = 0;
	virtual int AI_getYieldOutputWeight(YieldTypes eYield) const = 0;
	virtual void AI_setYieldOutputWeight(YieldTypes eYield, int iNewValue) = 0;
	virtual int AI_getNeededYield(YieldTypes eYield) const = 0;
	virtual void AI_setNeededYield(YieldTypes eYield, int iNewValue) = 0;
	virtual int AI_getTradeBalance(YieldTypes eYield) const = 0;
	virtual void AI_changeTradeBalance(YieldTypes eYield, int iAmount) = 0;
	virtual int AI_getYieldAdvantage(YieldTypes eYield) const = 0;
	virtual void AI_setYieldAdvantage(YieldTypes eYield, int iNewValue) = 0;
	virtual void AI_assignDesiredYield() = 0;
	virtual YieldTypes AI_getDesiredYield() const = 0;
	virtual int AI_getFoodGatherable(int iPop, int iPlotFoodThreshold) const = 0;
	virtual bool AI_shouldImportYield(YieldTypes eYield) const = 0;
	virtual bool AI_shouldExportYield(YieldTypes eYield) const = 0;
	virtual int AI_getTransitYield(YieldTypes eYield) const = 0;
	virtual bool AI_isPort() const = 0;
	virtual void AI_setPort(bool iNewValue) = 0;
	virtual int AI_getRequiredYieldLevel(YieldTypes eYield) = 0;
	virtual void AI_educateStudent(int iUnitId) = 0;
	virtual bool AI_isWorkforceHack() const = 0;
	virtual void AI_setWorkforceHack(bool bNewValue) = 0;
	virtual int AI_calculateAlarm(PlayerTypes eIndex) const = 0;

	void invalidatePopulationRankCache();
	void invalidateYieldRankCache(YieldTypes eYield = NO_YIELD);
	int getBestYieldAmountAvailable(ProfessionTypes eProfession, const CvUnit* pUnit) const;
	// R&R, ray , MYCP partially based on code of Aymerick - START
	int getBestYieldsAmountAvailable(YieldTypes eYield, ProfessionTypes eProfession, const CvUnit* pUnit) const;
	// R&R, ray , MYCP partially based on code of Aymerick - END
	void addPopulationUnit(CvUnit* pUnit, ProfessionTypes eProfession);
	bool removePopulationUnit(AssertCallerData assertData, CvUnit* pUnit, bool bDelete, ProfessionTypes eProfession, bool conquest = false);
	CvUnit* removeUnitType(UnitTypes eUnit, ProfessionTypes eProfession);
	void removeNonCityPopulationUnits();
	int getPopulationUnitId(int iPlotIndex) const;
	CvUnit* getPopulationUnitById(int iUnitId) const;
	CvUnit* getPopulationUnitByIndex(int iUnitIndex) const;
	int getPopulationUnitIndex(const CvUnit& kUnit) const;
	CvPlot* getPlotWorkedByUnit(const CvUnit* pUnit) const;
	CvUnit* createYieldUnit(YieldTypes eYield, PlayerTypes ePlayer, int iYieldAmount);
	UnitClassTypes getTeachUnitClass() const;
	void setTeachUnitClass(UnitClassTypes eUnitClass);
	void ejectTeachUnits();
	void ejectMissionary();
	void ejectTrader();
	bool canProduceYield(YieldTypes eYield) const;

	bool educateStudent(int iUnitId, UnitTypes eUnit);
	bool canTeach(UnitTypes eUnit) const;
	int getSpecialistTuition(UnitTypes eUnit) const;

	bool isExport(YieldTypes eYield) const;
	void addExport(YieldTypes eYield, bool bUpdateRoutes = true);
	void removeExport(YieldTypes eYield, bool bUpdateRoutes = true);
	void updateExport(YieldTypes eYield);
	void updateExports();
	bool isImport(YieldTypes eYield) const;
	void addImport(YieldTypes eYield, bool bUpdateRoutes = true);
	void removeImport(YieldTypes eYield, bool bUpdateRoutes = true);
	void updateImport(YieldTypes eYield);
	void updateImports();
	void removeTradeRoutes();
	void setMaintainLevel(YieldTypes eYield, int iMaintainLevel);
	int getMaintainLevel(YieldTypes eYield) const;
	// R&R mod, vetiarvind, max yield import limit - Start
	void setImportsLimit(YieldTypes eYield, int iValue);
	int getImportsLimit(YieldTypes eYield) const;
	// R&R mod, vetiarvind, max yield import limit - End
	// R&R, ray, finishing Custom House Screen START
	void setCustomHouseSellThreshold(YieldTypes eYield, int iThreshold);
	int getCustomHouseSellThreshold(YieldTypes eYield) const;
	void setCustomHouseNeverSell(YieldTypes eYield, bool bNeverSell);
	bool isCustomHouseNeverSell(YieldTypes eYield) const;
	void createFleeingUnit(UnitTypes eUnit, bool bDefautAI); // WTP, ray, LbD Slaves Revolt and Free - START - adjusted to also have DefaultAI
	// R&R, ray, finishing Custom House Screen END
	// Teacher List - start - Nightinggale
	int getOrderedStudents(UnitTypes eUnit);
	bool getOrderedStudentsRepeat(UnitTypes eUnit);
	// Teacher List - end - Nightinggale

	// WTP, ray, Center Plot specific Backgrounds - Start
	TerrainTypes getCenterPlotTerrainType() const;
	// WTP, ray, Center Plot specific Backgrounds - End

	YieldTypes getPreferredYieldAtCityPlot() const { return m_ePreferredYieldAtCityPlot; }

	void setBarrackHarbourCache();

	      CvCityYields& yields()       { return m_yields; }
	const CvCityYields& yields() const { return m_yields; }

	void writeDesyncLog(FILE *f) const;

protected:
	CvCityYields m_yields;
	int m_iID;
	Coordinates m_coord;
	Coordinates m_rallyCoordinates;
	int m_iGameTurnFounded;
	int m_iGameTurnAcquired;
	int m_iHighestPopulation;
	int m_iWorkingPopulation;
	int m_iNumBuildings;
	int m_iWorksWaterCount;
	int m_iHealRate;
	int m_iFoodKept;
	int m_iMaxFoodKeptPercent;
	int m_iOverflowProduction;
	int m_iMilitaryProductionModifier;
	int m_iBuildingDefense;
	int m_iBuildingBombardDefense;
	int m_iFreeExperience;
	int m_iDefenseDamage;
	int m_iLastDefenseDamage;
	int m_iOccupationTimer;
	int m_iCultureUpdateTimer;
	int m_iCitySizeBoost;
	int m_iHammers;
	int m_iMissionaryRate;
	int m_iNativeTradeRate; // WTP, ray, Native Trade Posts - START
	int m_iTradePostGold; // WTP, ray, Native Trade Posts - START
	bool m_bStirredUp; // R&R, ray , Stirring Up Natives
	int m_iRebelSentiment;
	int m_iCityHarbourSpace; // WTP, ray, new Harbour System - START
	int m_iCityBarracksSpace; // WTP, ray, new Barracks System - START
	int m_iCityHealth; // R&R, ray, Health
	int m_iCityHappiness; // WTP, ray, Happiness
	int m_iCityUnHappiness; // WTP, ray, Happiness
	int m_iCityTimerFestivitiesOrUnrest; // WTP, ray, Happiness
	int m_iCityLaw; // WTP, ray, Crime and Law - START
	int m_iCityCrime; // WTP, ray, Crime and Law - START
	int m_iDomesticDemandEventDuration; // WTP, ray Domestic Market Events - START
	int m_iDomesticDemandEventTimer; // WTP, ray Domestic Market Events - START
	int m_iDomesticDemandEventPriceModifier; // WTP, ray Domestic Market Events - START
	int m_iDomesticDemandEventDemandModifier; // WTP, ray Domestic Market Events - START
	int m_iTeachUnitMultiplier;
	int m_iEducationThresholdMultiplier;
	int m_iTotalYieldStored;

	bool m_bNeverLost;
	bool m_bBombarded;
	bool m_bProductionAutomated;
	bool m_bWallOverride;
	bool m_bInfoDirty;
	bool m_bLayoutDirty;
	PlayerTypes m_eOwner;
	PlayerTypes m_ePreviousOwner;
	PlayerTypes m_eOriginalOwner;
	CultureLevelTypes m_eCultureLevel;
	UnitClassTypes m_eTeachUnitClass;

	PlayerTypes m_eMissionaryPlayer;
	PlayerTypes m_eTradePostPlayer; // WTP, ray, Native Trade Posts - START
	EnumMap<YieldTypes,int> m_em_iLandPlotYield; // R&R, ray, Landplot Yields
	EnumMap<YieldTypes,int> m_em_iSeaPlotYield;
	EnumMap<YieldTypes,int> m_em_iRiverPlotYield;
	EnumMap<YieldTypes,int> m_em_iYieldRateModifier;
	EnumMap<YieldTypes,int> m_em_iYieldStored;
	EnumMap<YieldTypes,int> m_em_iYieldRushed;
	// R&R, Androrc, Domestic Market
	EnumMap<YieldTypes,int> m_em_iYieldBuyPrice;
	//Androrc End

	// R&R, ray, finishing Custom House Screen
	EnumMap<YieldTypes,int> m_em_iCustomHouseSellThreshold;
	EnumMap<YieldTypes,bool> m_em_bCustomHouseNeverSell;
	// R&R, ray, finishing Custom House Screen END

	// Teacher List - start - Nightinggale
	EnumMap<UnitTypes,int> m_em_iOrderedStudents;
	EnumMap<UnitTypes,bool> m_em_bOrderedStudentsRepeat;
	// Teacher List - end - Nightinggale
	EnumMap<DomainTypes,int> m_em_iDomainFreeExperience;
	EnumMap<DomainTypes,int> m_em_iDomainProductionModifier;
	EnumMap<PlayerTypes,int> m_em_iCulture;
	EnumMap<PlayerTypes,bool> m_em_bEverOwned;
	EnumMap<TeamTypes,bool> m_em_bRevealed;
	EnumMap<TeamTypes,bool> m_em_bScoutVisited;
	CvWString m_szName;
	CvString m_szScriptData;
	EnumMap<BuildingTypes,int> m_em_iBuildingProduction;
	EnumMap<BuildingTypes,int> m_em_iBuildingProductionTime;
	EnumMap<BuildingTypes,PlayerTypes> m_em_eBuildingOriginalOwner;
	EnumMap<BuildingTypes,int> m_em_iBuildingOriginalTime;
	EnumMap<UnitTypes,int> m_em_iUnitProduction;
	EnumMap<UnitTypes,int> m_em_iUnitProductionTime;
	EnumMap<UnitTypes,int> m_em_iSpecialistWeights;
	EnumMap<UnitCombatTypes,int> m_em_iUnitCombatFreeExperience;
	EnumMap<PromotionTypes,int> m_em_iFreePromotionCount;
	EnumMap<BuildingTypes,bool> m_em_bHasRealBuilding;
	EnumMap<BuildingTypes,bool> m_em_bHasFreeBuilding;
	EnumMap<CityPlotTypes, int, -1> m_em_iWorkingPlot;
	IDInfo* m_paTradeCities;
	mutable CLinkList<OrderData> m_orderQueue;
	std::vector< std::pair < float, float> > m_kWallOverridePoints;
	std::vector<EventTypes> m_aEventsOccured;
	std::vector<BuildingYieldChange> m_aBuildingYieldChange;
	std::vector<CvUnit*> m_aPopulationUnits;

	// CACHE: cache frequently used values
	mutable int	m_iPopulationRank;
	mutable bool m_bPopulationRankValid;
	mutable EnumMap<YieldTypes,int,-1> m_em_iBaseYieldRank;
	mutable EnumMap<YieldTypes,bool> m_em_bBaseYieldRankValid;
	mutable EnumMap<YieldTypes,int,-1> m_em_iYieldRank;
	mutable EnumMap<YieldTypes,bool> m_em_bYieldRankValid;

	bool m_bHasHurried; // Needed to remember (cache) if a hurry was conducted and we should complete the current build
	int m_iSlaveWorkerProductionBonus;

	void doGrowth();
	void doYields();
	void addTempHurryYieldsForProduction();
	void doEntertainmentBuildings(); // R&R, ray, Entertainment Buildings

	void doCulture();
	void doPlotCulture(bool bUpdate, PlayerTypes ePlayer, int iCultureRate);
	void doSpecialists();
	void doProduction(bool bAllowNoProduction);
	void doPrices(); // R&R, Androrc, Domestic Market
	void doDecay();
	void doMissionaries();
	void doNativeTradePost(); // WTP, ray, Native Trade Posts - START
	void doRebelSentiment();
	void doCityHealth(); // R&R, ray, Health - START
	void doCityHappiness(); // WTP, ray, Happiness - START
	void doCityUnHappiness(); // WTP, ray, Happiness - START
	void doCityLaw(); // WTP, ray, Crime and Law - START
	void doCityCrime(); // WTP, ray, Crime and Law - START

	// WTP, ray Domestic Market Events - START
protected:
	void checkForDomesticDemandEvent();

	void setDurationDomesticDemandEvent(int iDuration);
	void reduceRemainingDurationDomesticDemandEvent();
	int getRemainingDurationDomesticDemandEvent() const;

	void setTimerDomesticDemandEvent(int iTimer);
	void reduceTimerDomesticDemandEvent();
	int getTimerDomesticDemandEvent() const;

	void setDomesticDemandEventPriceModifier(int iDomesticDemandEventModifier);
	void setDomesticDemandEventDemandModifier(int iDomesticDemandEventModifier);

public:
	int getDomesticDemandEventPriceModifier() const;
	int getDomesticDemandEventDemandModifier() const;
	// WTP, ray Domestic Market Events - END

protected:
	bool doCheckProduction();
	void doCheat(bool bAlt, bool bShift, bool bCtrl);
	int getExtraProductionDifference(int iExtra, int iModifier) const;
	int getHurryCostModifier(UnitTypes eUnit, bool bIgnoreNew) const;
	int getHurryCostModifier(BuildingTypes eBuilding, bool bIgnoreNew) const;
	int getHurryCostModifier(int iBaseModifier, int iProduction, bool bIgnoreNew) const;
	int getHurryCost(bool bExtra, int iProductionLeft, int iHurryModifier, int iModifier) const;
	int getHurryPopulation(HurryTypes eHurry, int iHurryCost) const;
	int getHurryYieldDeficit(HurryTypes eHurry, YieldTypes eYield) const;
	int getHurryYieldNeeded(HurryTypes eHurry, YieldTypes eYield) const;
	int getProductionDifference(int iProductionModifier, bool bOverflow, bool bUseStoredHammers) const;
	int getStoredProductionDifference() const;
	// R&R, ray , MYCP partially based on code of Aymerick - START
	int getProfessionInputs(ProfessionTypes eProfession, const CvUnit* pUnit) const;
	// R&R, ray , MYCP partially based on code of Aymerick - END
	// Teacher List - start - Nightinggale
	void checkOrderedStudentsForRepeats(UnitTypes eUnit);
	void setOrderedStudents(UnitTypes eUnit, int iCount, bool bRepeat, bool bUpdateRepeat = true, bool bClearAll = false);
	// Teacher List - end - Nightinggale
	void setUnitWorkingPlot(int iPlotIndex, int iUnitId);
	void setUnitWorkingPlot(const CvPlot* pPlot, int iUnitId);
	int getNextFreeUnitId() const;

	// transport feeder - start - Nightinggale
public:
 	bool getImportsMaintain(YieldTypes eYield) const;
	bool isAutoImportStopped(YieldTypes eYield) const;
	int getAutoMaintainThreshold(YieldTypes eYield) const;
	int getProductionNeededUncached(YieldTypes eYield) const;
	void checkImportsMaintain(YieldTypes eYield, bool bUpdateScreen = false);

	// WARNING: setAutoThresholdCache will cause desyncs if not called by all computers in MP in sync
	void setAutoThresholdCache(YieldTypes eYield);
	void setAutoThresholdCache();

	bool isEuropeAccessable() const; // WTP, ray, fix for SailTo - for the City

	//WTP, ray, Slave Hunter and Slave Master
	int getSlaveRevoltReductionBonus() const;
	int getSlaveWorkerProductionBonus() const;
	//WTP, ray, Slave Hunter and Slave Master
	void updateSlaveWorkerProductionBonus(int iBonus = 0);

protected:
	// traderoute popup arrays
	EnumMap<YieldTypes, bool> m_em_bTradeImports;
	EnumMap<YieldTypes, bool> m_em_bTradeExports;
	EnumMap<YieldTypes, bool> m_em_bTradeImportsMaintain;
	EnumMap<YieldTypes, bool> m_em_bTradeAutoExport;
	EnumMap<YieldTypes, unsigned short> m_em_iTradeMaxThreshold;// R&R mod, vetiarvind, max yield import limit
	EnumMap<YieldTypes, unsigned short> m_em_iTradeThreshold;

	// saves the hysteresis state
	EnumMap<YieldTypes,bool> m_em_bTradeStopAutoImport;
	EnumMap<YieldTypes,int> m_em_iTradeAutoThreshold; // nosave - recalculate on load
	EnumMap<YieldTypes,int> m_em_iProductionNeeded; // nosave - recalculate on load

	// setImportsMaintain() is only allowed to be called by doTask() or it will cause desyncs
	void setImportsMaintain(YieldTypes eYield, bool bSetting);
	// transport feeder - end - Nightinggale

	// auto traderoute - start - Nightinggale
public:
	bool isAutoExport(YieldTypes eYield) const {return m_em_bTradeAutoExport.get(eYield);};
protected:
	void setAutoExport(YieldTypes eYield, bool bExport);
	void doAutoExport(YieldTypes eYield);
	void handleAutoTraderouteSetup(bool bReset, bool bImportAll, bool bAutoExportAll);

	// auto traderoute - end - Nightinggale

	//bobisback import mod
	void handleDemandedImport();
	void handleConstructionImport();

public:
	int getMarketModifier() const { return m_iCacheMarketModifier; }
	const YieldCargoArray<int>& getBuildingYieldDemands() const { return m_ja_iBuildingYieldDemands; }

protected:
	int m_iCacheMarketModifier;
	YieldCargoArray<int> m_ja_iBuildingYieldDemands;

	virtual bool AI_addBestCitizen() = 0;
	virtual bool AI_removeWorstCitizen() = 0;

	YieldTypes m_ePreferredYieldAtCityPlot;
	void setPreferredYieldAtCityPlot(YieldTypes eYield);

	//storage loss trading (aka customs house and related things)
protected:
	int m_iStorageLossSellPercentage;
	bool m_bIgnoresBoycott;
	bool m_bHasUnlockedStorageLossTradeSettings;//If this city has unlocked trade settings for the storage loss (aka "Customs house popup").

	void initCacheStorageLossTradeValues();
	void cache_storageLossTradeValues_usingCachedData(BuildingTypes eBuilding);
	void cache_storageLossTradeValues_usingRawData();
	void updateCacheStorageLossTradingValues(BuildingTypes eBuilding, bool bWasAdded);
	void pushOrderInternal(BuildingTypes);
	void pushOrderInternal(UnitTypes);
	void pushOrderInternal(OrderData&);
	void popOrderInternal();

public:
	inline int getStorageLossSellPercentage()const					{ return m_iStorageLossSellPercentage; }
	inline bool getIgnoresBoycott()const							{ return m_bIgnoresBoycott; }
	inline bool getHasUnlockedStorageLossTradeSettings()const		{ return m_bHasUnlockedStorageLossTradeSettings; }

	int getMaxImportAmount(YieldTypes eYield) const;

protected:
	int m_iOppressometer;
	int m_iOppressometerGrowthModifier;

	int getOppressometerGrowthModifier() const
	{
		return m_iOppressometerGrowthModifier;
	}
	void changeOppressometer(int iChange);
	void changeOppressometerGrowthModifier(int iChange);
	void doOppressometerDecay();
	void doOppressometerGrowth();

public:
	int getOppressometer() const
	{
		return m_iOppressometer;
	}
	void growOppressometer(int iChange);
};

// NBMOD EDU cache - start - Nightinggale
inline int CvCity::NBMOD_GetCityTeachLevel() const
{
	return m_NBMOD_CityTeachLevel_cache;
};
// NBMOD EDU cache - end - Nightinggale

// cache getMaxYieldCapacity - start - Nightinggale
inline int CvCity::getMaxYieldCapacity() const
{
	// WTP, ray, Improvements give Bonus to their City - PART 3 - START
	int iAdditionalStorageFromImprovements = getImprovementStorageModifierForCity();
	//return m_cache_MaxYieldCapacity;
	return m_cache_MaxYieldCapacity + iAdditionalStorageFromImprovements;
	// WTP, ray, Improvements give Bonus to their City - PART 3 - END
};
// cache getMaxYieldCapacity - end - Nightinggale

// transport feeder - start - Nightinggale
inline bool CvCity::getImportsMaintain(YieldTypes eYield) const
{
	return m_em_bTradeImportsMaintain.get(eYield);
}

inline bool CvCity::isAutoImportStopped(YieldTypes eYield) const
{
	return m_em_bTradeStopAutoImport.get(eYield);
}

inline int CvCity::getAutoMaintainThreshold(YieldTypes eYield) const
{
	return m_em_iTradeAutoThreshold.get(eYield);
}

inline int CvCity::getProductionNeeded(YieldTypes eYield) const
{
	return m_em_iProductionNeeded.get(eYield);
}
// transport feeder - end - Nightinggale
#endif
