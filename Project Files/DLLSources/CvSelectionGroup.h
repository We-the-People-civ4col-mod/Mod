#pragma once

// selectionGroup.h

#ifndef CIV4_GROUP_H
#define CIV4_GROUP_H

//#include "CvStructs.h"
#include "LinkedList.h"
#include "KmodPathFinder.h"
#include "CvInitCore.h"

class CvPlot;
class CvArea;
class CvSelectionGroupAI;
class FAStarNode;
class CvSelectionGroup
{

public:

	__forceinline CvSelectionGroupAI& AI() { return (CvSelectionGroupAI&)*this; }
	__forceinline const CvSelectionGroupAI& AI() const { return (CvSelectionGroupAI&)*this; }

    /** NBMOD REF **/

    float NBMOD_GetShipStrength() const;

    /** NBMOD REF **/

	CvSelectionGroup();
	virtual ~CvSelectionGroup();

	DllExport void init(int iID, PlayerTypes eOwner);
	DllExport void uninit();
	DllExport void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall = false);

	void kill();

	void doTurn();

	bool showMoves( /* <advc.102> */ CvPlot const& kFromPlot) const;
	void setInitiallyVisible(bool b); // </advc.102>

	void updateTimers();
	bool doDelayedDeath();

	void playActionSound();
	DllExport void pushMission(MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, CvPlot* pMissionAIPlot = NULL, CvUnit* pMissionAIUnit = NULL);
	void popMission();
	DllExport void autoMission();
	bool autoMissionInternal();
	void updateMission();
	DllExport CvPlot* lastMissionPlot();

	bool canStartMission(MissionTypes eMission, int iData1, int iData2,									// Exposed to Python
		CvPlot const* pPlot = NULL, bool bTestVisible = false,
		bool bUseCache = false) const;
	void startMission();
	//void continueMission(int iSteps = 0);
	// K-Mod: Split continueMission into two functions to remove the recursion.
	void continueMission();
	bool continueMission_bulk(int iSteps); // K-Mod
	DllExport bool canDoInterfaceMode(InterfaceModeTypes eInterfaceMode);
	DllExport bool canDoInterfaceModeAt(InterfaceModeTypes eInterfaceMode, CvPlot* pPlot);

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bUseCache = false);
	bool canEverDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache);
	void setupActionCache();
	bool isHuman() const;
	DllExport bool isBusy();
	bool isBusyInternal() const;
	bool isCargoBusy();
	int baseMoves() const;
	bool isWaiting() const;
	bool isCycleGroup() const { return getNumUnits() > 0 && !isWaiting() && !isAutomated(); }
	bool isFull() const;
	bool hasCargo() const;
	int getCargo() const;
	bool buildCargoUnitList(CLinkList<IDInfo>& unitList) const;
	DllExport bool canAllMove();
	bool canAllMoveInternal() const;
	
	bool canAnyMove() const;
	bool hasMoved();
	bool canEnterTerritory(PlayerTypes ePlayer, bool bIgnoreRightOfPassage = false) const;
	bool canEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage = false) const;
	bool canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage = false) const;									// Exposed to Python
	bool canEnterArea(TeamTypes eTeam, CvArea const& kArea, bool bIgnoreRightOfPassage = false) const;					// Exposed to Python
	DllExport bool canMoveInto(CvPlot* pPlot, bool bAttack = false);
	DllExport bool canMoveOrAttackInto(CvPlot* pPlot, bool bDeclareWar = false);
	bool canMoveOrAttackInto(CvPlot const& kPlot, bool bDeclareWar = false, bool bCheckMoves = false, bool bAssumeVisible = true) const;
	bool canMoveThrough(CvPlot const& kPlot, bool bDeclareWar = false, bool bAssumeVisible = true) const; // Exposed to Python, K-Mod added bDeclareWar and bAssumeVisible; advc: CvPlot const&
	bool canFight() const;
	bool canDefend() const;
	bool canBombard(const CvPlot* pPlot) const;
	int visibilityRange() const;

	int getBombardTurns( CvCity* pCity );	// TAC - AI Attack City - koma13, jdog5000(BBAI)

	bool isPromotionReady() const;
	bool canChangeProfession() const;

	void unloadAll();
	bool alwaysInvisible() const;
	bool isInvisible(TeamTypes eTeam) const;
	int countNumUnitAIType(UnitAITypes eUnitAI) const;
	bool IsSelected();
	DllExport void NotifyEntity(MissionTypes eMission);

	int getX() const;
	int getY() const;
	bool at(int iX, int iY) const;
	bool atPlot(const CvPlot* pPlot) const;
	DllExport CvPlot* plot() const;
	CvPlot& getPlot() const { return *plot(); } // advc
	int getArea() const;
	CvArea* area() const;
	DomainTypes getDomainType() const;
	bool canBuildRoute(CvPlot const *pPlot, RouteTypes ePreferredRoute = NO_ROUTE) const;
	BuildTypes getBestBuildRouteBuild(CvPlot *pPlot, RouteTypes ePreferredRoute) const;
	RouteTypes getBestBuildRoute(const CvPlot* pPlot, BuildTypes* peBestBuild = NULL, RouteTypes ePreferredRoute = NO_ROUTE) const;

	bool groupDeclareWar(CvPlot* pPlot, bool bForce = false);
	bool groupAttack(AssertCallerData assertData, int iX, int iY, int iFlags, bool& bFailedAlreadyFighting);
	void groupMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit = NULL, bool bEndMove = false);
	bool groupPathTo(int iX, int iY, int iFlags);
	bool groupRouteTo(int iX, int iY, int iFlags, RouteTypes ePreferredRoute = ROUTE_PLASTERED_ROAD);
	// Wrapper for AdvCiv code
	bool groupRoadTo(int iX, int iY, MovementFlags eFlags, RouteTypes ePreferredRoute = ROUTE_PLASTERED_ROAD)
	{
		return groupRouteTo(iX, iY, eFlags, ePreferredRoute);
	}
	bool groupBuild(BuildTypes eBuild);
	void setTransportUnit(CvUnit* pTransportUnit,
		CvSelectionGroup** pOtherGroup = NULL); // BBAI
	bool isAmphibPlot(const CvPlot* pPlot) const;
	bool groupAmphibMove(CvPlot const& kPlot, MovementFlags eFlags);
	DllExport bool readyToSelect(bool bAny = false);
	bool readyToMove(bool bAny = false);
	bool readyToAuto() const;
	// K-Mod.
	bool readyForMission() const;
	bool canDoMission(MissionTypes eMission, int iData1, int iData2, CvPlot const* pPlot,
		bool bTestVisible, bool bCheckMoves) /* advc.002i: */ const;
	// K-Mod end
	bool isOnMap() const;
	int getID() const;
	void setID(int iID);

	int getMissionTimer() const;
	void setMissionTimer(int iNewValue);
	void changeMissionTimer(int iChange);
	void updateMissionTimer(int iSteps = 0, /* advc.102: */ CvPlot* pFromPlot = NULL);

	bool isForceUpdate();
	void setForceUpdate(bool bNewValue);
	DllExport PlayerTypes getOwner() const;
	inline PlayerTypes getOwnerINLINE() const
	{
		return m_eOwner;
	}
	TeamTypes getTeam() const;
	// <advc>
	bool isActiveOwned() const { return (GC.getInitCore().getActivePlayer() == getOwner()); }
	bool isActiveTeam() const { return (GC.getInitCore().getActiveTeam() == getTeam()); } // </advc>

	ActivityTypes getActivityType() const;
	void setActivityType(ActivityTypes eNewValue);

	AutomateTypes getAutomateType() const { return m->eAutomateType; }																									// Exposed to Python
	bool isAutomated() const { return (getAutomateType() != NO_AUTOMATE); }
	void setAutomateType(AutomateTypes eNewValue);

	// FAStarNode* getPathLastNode() const; // disabled by K-Mod. Use path_finder methods instead.
	CvPlot* getPathFirstPlot() const;
	CvPlot* getPathEndTurnPlot() const;
	CvPlot* getPathSecondLastPlot() const;
	int getPathCost() const;

	// TAC - AI Improved Naval AI - koma13 - START
	CvPlot* getPathPlotByIndex(int iIndex) const;
	int getPathLength() const;
	// TAC - AI Improved Naval AI - koma13 - END
	bool generatePath(const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL, int iMaxPath = -1, bool bUseTempFinder = false, bool bCalledFromPython = false) const; // Exposed to Python (K-mod added iMaxPath)
	// void resetPath() const; // disabled by K-mod. Use path_finder.Reset instead. (was exposed to Python)

	DllExport void clearUnits();
	DllExport bool addUnit(CvUnit* pUnit, bool bMinimalChange);
	void removeUnit(CvUnit* pUnit);
	void mergeIntoGroup(CvSelectionGroup* pSelectionGroup, UnitAITypes eUnitAI = NO_UNITAI);
	CvSelectionGroup* splitGroup(int iSplitSize, CvUnit* pNewHeadUnit = NULL, CvSelectionGroup** ppOtherGroup = NULL);

	DllExport CLLNode<IDInfo>* deleteUnitNode(CLLNode<IDInfo>* pNode);
	DllExport inline CLLNode<IDInfo>* nextUnitNode(CLLNode<IDInfo>* pNode) const
	{
		return m_units.next(pNode); // advc.inl
	} // <advc.003s> Safer in 'for' loops
	inline CLLNode<IDInfo> const* nextUnitNode(CLLNode<IDInfo> const* pNode) const
	{
		return m_units.next(pNode);
	} // </advc.003s>

	// Returns the unit the node is pointing to and then the node will point to the next unit node
	CvUnit* getUnitNodeLoop(CLLNode<IDInfo>*& pUnitNode) const;

	// Returns the unit the node is pointing to and then the node will point to the next unit node
	CvUnit* getUnitNodeLoop(const CLLNode<IDInfo>*& pUnitNode) const;

	DllExport int getNumUnits() const;
	DllExport int getUnitIndex(CvUnit* pUnit, int maxIndex = -1) const;
	DllExport CLLNode<IDInfo>* headUnitNode() const;
	DllExport CvUnit* getHeadUnit() const;
	CvUnit* getUnitAt(int iIndex) const;
	UnitAITypes getHeadUnitAI() const;
	UnitAITypes getHeadUnitAIType() const { return getHeadUnitAI(); }

	PlayerTypes getHeadOwner() const;
	TeamTypes getHeadTeam() const;
	void clearMissionQueue();
	int getLengthMissionQueue() const;
	MissionData* getMissionFromQueue(int iIndex) const;
	void insertAtEndMissionQueue(MissionData mission, bool bStart = true);
	CLLNode<MissionData>* deleteMissionQueueNode(CLLNode<MissionData>* pNode);
	DllExport CLLNode<MissionData>* nextMissionQueueNode(CLLNode<MissionData>* pNode) const;
	CLLNode<MissionData>* prevMissionQueueNode(CLLNode<MissionData>* pNode) const;
	DllExport CLLNode<MissionData>* headMissionQueueNode() const;
	CLLNode<MissionData>* tailMissionQueueNode() const;
	int getMissionType(int iNode) const;
	int getMissionData1(int iNode) const;
	int getMissionData2(int iNode) const;

	bool canAssignTradeRoute(int iRouteID, bool bReusePath = false) const;
	void assignTradeRoute(int iRouteID, bool bAssign);
	bool isAssignedTradeRoute(int iRouteId) const;
	void clearTradeRoutes();

	void speakWithChief();

	int maxMoves() const; // K-Mod
	int movesLeft() const; // K-Mod

	int groupCycleDistance(CvSelectionGroup const& kOther) const; // </advc>

	// for serialization
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);

	void resetSavedData(int iID, PlayerTypes eOwner);

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_separate() = 0;

	virtual void AI_separateEmptyTransports() = 0;	// TAC - AI Assault Sea - koma13, jdog5000(BBAI)
	bool hasShipInPort() const;

	virtual bool AI_update() = 0;
	virtual int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy) const = 0;
	virtual CvUnit* AI_getBestGroupAttacker(const CvPlot* pPlot, bool bPotentialEnemy, int& iUnitOdds, bool bForce = false, bool bNoBlitz = false) const = 0;
	virtual CvUnit* AI_getBestGroupSacrifice(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce = false, bool bNoBlitz = false) const = 0;
	virtual int AI_compareStacks(const CvPlot* pPlot, bool bPotentialEnemy, bool bCheckCanAttack = false, bool bCheckCanMove = false) const = 0;
	virtual int AI_sumStrength(const CvPlot* pAttackedPlot = NULL, DomainTypes eDomainType = NO_DOMAIN, bool bCheckCanAttack = false, bool bCheckCanMove = false) const = 0;
	virtual void AI_queueGroupAttack(int iX, int iY) = 0;
	virtual void AI_cancelGroupAttack() = 0;
	virtual bool AI_isGroupAttack() = 0;

	virtual bool AI_isControlled() = 0;
	virtual bool AI_isDeclareWar(const CvPlot* pPlot = NULL) = 0;
	virtual CvPlot* AI_getMissionAIPlot() = 0;
	virtual bool AI_isForceSeparate() = 0;
	virtual void AI_makeForceSeparate() = 0;
	virtual MissionAITypes AI_getMissionAIType() = 0;
	virtual void AI_setMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit) = 0;
	virtual CvUnit* AI_getMissionAIUnit() = 0;
	virtual CvUnit* AI_ejectBestDefender(CvPlot* pTargetPlot) = 0;
	virtual void AI_seperateNonAI(UnitAITypes eUnitAI) = 0;
	virtual void AI_seperateAI(UnitAITypes eUnitAI) = 0;
	virtual bool AI_isFull() = 0;
	virtual bool AI_launchAssault(CvPlot* pTargetCityPlot) = 0;
	virtual void AI_groupBombard() = 0;

	virtual bool AI_tradeRoutes() = 0;

protected:
	// WARNING: adding to this class will cause the civ4 exe to crash

	int m_iID;
	int m_iMissionTimer;

	bool m_bForceUpdate;

	PlayerTypes m_eOwner;
	ActivityTypes m_eActivityType;
	//AutomateTypes m_eAutomateType;
	// <advc.003k> Pointer to additional data members
	class Data
	{
		AutomateTypes eAutomateType;
		//CLinkList<IDInfo> knownEnemies; // advc.004l
		bool bInitiallyVisible; // advc.102
		friend CvSelectionGroup;
	};
	Data* m; // dial m for members
	// </advc.003k>

	CLinkList<IDInfo> m_units;
	std::set<int> m_aTradeRoutes;

	CLinkList<MissionData> m_missionQueue;
	std::vector<CvUnit *> m_aDifferentUnitCache;
	bool m_bIsBusyCache;

	void activateHeadMission();
	void deactivateHeadMission();

	bool sentryAlert() const;

public:
	static KmodPathFinder path_finder; // K-Mod! I'd rather this not be static, but I can't do that here.
	static KmodPathFinder alt_finder; // advc.opt

	static KmodPathFinder& getClearPathFinder(); // advc.opt
};


#endif
