#pragma once

// selectionGroup.h

#ifndef CIV4_GROUP_H
#define CIV4_GROUP_H

//#include "CvStructs.h"
#include "LinkedList.h"
#include "KmodPathFinder.h"

class CvPlot;
class CvArea;
class FAStarNode;

class CvSelectionGroup
{

public:

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

	bool showMoves() const;

	void updateTimers();
	bool doDelayedDeath();

	void playActionSound();
	DllExport void pushMission(MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, CvPlot* pMissionAIPlot = NULL, CvUnit* pMissionAIUnit = NULL);
	void popMission();
	DllExport void autoMission();
	void updateMission();
	DllExport CvPlot* lastMissionPlot();

	bool canStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot = NULL, bool bTestVisible = false, bool bUseCache = false);
	void startMission();
	void continueMission(int iSteps = 0);
	DllExport bool canDoInterfaceMode(InterfaceModeTypes eInterfaceMode);
	DllExport bool canDoInterfaceModeAt(InterfaceModeTypes eInterfaceMode, CvPlot* pPlot);

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bUseCache = false);
	bool canEverDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache);
	void setupActionCache();
	bool isHuman();
	DllExport bool isBusy();
	bool isCargoBusy();
	int baseMoves();
	bool isWaiting() const;
	bool isFull();
	bool hasCargo() const;
	int getCargo() const;
	bool buildCargoUnitList(CLinkList<IDInfo>& unitList) const;
	DllExport bool canAllMove();
	bool canAnyMove();
	bool hasMoved();
	bool canEnterTerritory(PlayerTypes ePlayer, bool bIgnoreRightOfPassage = false) const;
	bool canEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage = false) const;
	DllExport bool canMoveInto(CvPlot* pPlot, bool bAttack = false);
	DllExport bool canMoveOrAttackInto(CvPlot* pPlot, bool bDeclareWar = false);
	bool canMoveOrAttackInto(CvPlot const& kPlot, bool bDeclareWar = false, bool bCheckMoves = false, bool bAssumeVisible = true) const;
	bool canMoveThrough(CvPlot const& kPlot, bool bDeclareWar = false, bool bAssumeVisible = true) const; // Exposed to Python, K-Mod added bDeclareWar and bAssumeVisible; advc: CvPlot const&
	bool canFight();
	bool canDefend();
	bool canBombard(const CvPlot* pPlot);
	bool visibilityRange();

	int getBombardTurns( CvCity* pCity );	// TAC - AI Attack City - koma13, jdog5000(BBAI)

	bool isPromotionReady() const;
	bool canChangeProfession() const;

	void unloadAll();
	bool alwaysInvisible() const;
	bool isInvisible(TeamTypes eTeam) const;
	int countNumUnitAIType(UnitAITypes eUnitAI);
	bool IsSelected();
	DllExport void NotifyEntity(MissionTypes eMission);

	int getX() const;
	int getY() const;
	bool at(int iX, int iY) const;
	bool atPlot(const CvPlot* pPlot) const;
	DllExport CvPlot* plot() const;
	int getArea() const;
	CvArea* area() const;
	DomainTypes getDomainType() const;
	bool canBuildRoute(CvPlot* pPlot, RouteTypes ePreferredRoute = NO_ROUTE) const;
	BuildTypes getBestBuildRouteBuild(CvPlot *pPlot, RouteTypes ePreferredRoute) const;
	RouteTypes getBestBuildRoute(CvPlot* pPlot, BuildTypes* peBestBuild = NULL, RouteTypes ePreferredRoute = NO_ROUTE) const;

	bool groupDeclareWar(CvPlot* pPlot, bool bForce = false);
	bool groupAttack(int iX, int iY, int iFlags, bool& bFailedAlreadyFighting);
	void groupMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit = NULL, bool bEndMove = false);
	bool groupPathTo(int iX, int iY, int iFlags);
	bool groupRouteTo(int iX, int iY, int iFlags, RouteTypes ePreferredRoute = ROUTE_PLASTERED_ROAD);
	bool groupBuild(BuildTypes eBuild);
	void setTransportUnit(CvUnit* pTransportUnit);
	bool isAmphibPlot(const CvPlot* pPlot) const;
	bool groupAmphibMove(CvPlot* pPlot, int iFlags);
	DllExport bool readyToSelect(bool bAny = false);
	bool readyToMove(bool bAny = false);
	bool readyToAuto();
	bool isOnMap() const;
	int getID() const;
	void setID(int iID);

	int getMissionTimer() const;
	void setMissionTimer(int iNewValue);
	void changeMissionTimer(int iChange);
	void updateMissionTimer(int iSteps = 0);

	bool isForceUpdate();
	void setForceUpdate(bool bNewValue);
	DllExport PlayerTypes getOwner() const;
#ifdef _USRDLL
	inline PlayerTypes getOwnerINLINE() const
	{
		return m_eOwner;
	}
#endif
	TeamTypes getTeam() const;

	ActivityTypes getActivityType() const;
	void setActivityType(ActivityTypes eNewValue);

	AutomateTypes getAutomateType() const;
	bool isAutomated();
	void setAutomateType(AutomateTypes eNewValue);

	// FAStarNode* getPathLastNode() const; // disabled by K-Mod. Use path_finder methods instead.
	CvPlot* getPathFirstPlot() const;
	CvPlot* getPathEndTurnPlot() const;
	CvPlot* getPathSecondLastPlot() const;
	int getPathCost() const;

	// TAC - AI Improved Naval AI - koma13 - START
	CvPlot* getPathPlotByIndex(int iIndex) const;
	int getPathLength() const;
	//bool generatePath(const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL) const;
	//bool generatePath(const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL, bool bIgnoreDanger = true) const;
	// TAC - AI Improved Naval AI - koma13 - END
	bool generatePath(const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL, int iMaxPath = -1) const; // Exposed to Python (K-mod added iMaxPath)
	// void resetPath() const; // disabled by K-mod. Use path_finder.Reset instead. (was exposed to Python)

	DllExport void clearUnits();
	DllExport bool addUnit(CvUnit* pUnit, bool bMinimalChange);
	void removeUnit(CvUnit* pUnit);
	void mergeIntoGroup(CvSelectionGroup* pSelectionGroup);
	CvSelectionGroup* splitGroup(int iSplitSize, CvUnit* pNewHeadUnit = NULL);

	DllExport CLLNode<IDInfo>* deleteUnitNode(CLLNode<IDInfo>* pNode);
	DllExport inline CLLNode<IDInfo>* nextUnitNode(CLLNode<IDInfo>* pNode) const
	{
		return m_units.next(pNode); // advc.inl
	} // <advc.003s> Safer in 'for' loops
	inline CLLNode<IDInfo> const* nextUnitNode(CLLNode<IDInfo> const* pNode) const
	{
		return m_units.next(pNode);
	} // </advc.003s>
	DllExport int getNumUnits() const;
	DllExport int getUnitIndex(CvUnit* pUnit, int maxIndex = -1) const;
	DllExport CLLNode<IDInfo>* headUnitNode() const;
	DllExport CvUnit* getHeadUnit() const;
	CvUnit* getUnitAt(int index) const;
	UnitAITypes getHeadUnitAI() const;
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
	AutomateTypes m_eAutomateType;

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

};

#endif
