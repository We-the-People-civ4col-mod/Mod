#ifndef CVMACRO_H
#define CVMACRO_H
#pragma once

// Helper for creating unique iteration variable
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE_VAR(base) TOKENPASTE2(base, __LINE__)

// shortcut to looping enums
// Example: FOREACH(Yield) will loop all yields using the loop variable eLoopYield

#define FOREACH(TYPE) \
for (TYPE##Types eLoop##TYPE = (TYPE##Types)0; \
eLoop##TYPE < NUM_##TYPE##_TYPES; \
eLoop##TYPE=(TYPE##Types)(eLoop##TYPE + 1))

// Note: Requires a class that provides getX_INLINE() / getY_INLINE()
#define FOR_EACH_PLOT_IN_RANGE(iSearchRange, ACTION) \
    for (int iDX = -(iSearchRange); iDX <= (iSearchRange); iDX++) \
        for (int iDY = -(iSearchRange); iDY <= (iSearchRange); iDY++) \
        { \
            CvPlot* const pLoopPlot = ::plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY); \
            if (pLoopPlot != NULL) \
            { \
                ACTION; \
            } \
        }

#define FOR_EACH_PLOT_IN_RANGE_OF(pCenterPlot, iSearchRange, ACTION) \
    for (int iDX = -(iSearchRange); iDX <= (iSearchRange); iDX++) \
        for (int iDY = -(iSearchRange); iDY <= (iSearchRange); iDY++) \
        { \
            CvPlot* const pLoopPlot = ::plotXY(pCenterPlot->getX_INLINE(), pCenterPlot->getY_INLINE(), iDX, iDY); \
            if (pLoopPlot != NULL) \
            { \
                ACTION; \
            } \
        }

// Allows loop variable to be specified
#define FOR_EACH_PLOT_IN_RANGE_OF2(pCenterPlot, pLoopPlot, iSearchRange, ACTION) \
    for (int iDX = -(iSearchRange); iDX <= (iSearchRange); iDX++) \
        for (int iDY = -(iSearchRange); iDY <= (iSearchRange); iDY++) \
        { \
            CvPlot* const pLoopPlot = ::plotXY(pCenterPlot->getX_INLINE(), pCenterPlot->getY_INLINE(), iDX, iDY); \
            if (pLoopPlot != NULL) \
            { \
                ACTION; \
            } \
        }

#define FOR_EACH_UNITAI_ON_PLOT(pPlot) \
    for (const CLLNode<IDInfo>* pUnitNode = (pPlot)->headUnitNode(); \
         pUnitNode != NULL; \
         pUnitNode = (pPlot)->nextUnitNodeInternal(pUnitNode)) \
         if (CvUnitAI* pLoopUnit = static_cast<CvUnitAI*>(::getUnit(pUnitNode->m_data)))

#define FOR_EACH_UNIT_ON_PLOT(pPlot) \
    for (const CLLNode<IDInfo>* pUnitNode = (pPlot)->headUnitNode(); \
         pUnitNode != NULL; \
         pUnitNode = (pPlot)->nextUnitNodeInternal(pUnitNode)) \
         if (CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data))

#define FOR_EACH_GROUPAI(pLoopUnit, kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (const CvSelectionGroupAI* pLoopSelectionGroup = static_cast<const CvSelectionGroupAI*>((kOwner).firstSelectionGroup(&UNIQUE_VAR(iLoop))); \
         pLoopSelectionGroup != NULL; pLoopSelectionGroup = static_cast<const CvSelectionGroupAI*>((kOwner).nextSelectionGroup(&UNIQUE_VAR(iLoop))))

#define FOR_EACH_GROUP_VAR(pGroup, kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvSelectionGroup* pGroup = (kOwner).firstSelectionGroup(&UNIQUE_VAR(iLoop)); \
    pGroup != NULL; pGroup = (kOwner).nextSelectionGroup(&UNIQUE_VAR(iLoop)))

#define FOR_EACH_GROUPAI_VAR(pGroup, kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvSelectionGroupAI* pGroup = static_cast<CvSelectionGroupAI*>((kOwner).firstSelectionGroup(&UNIQUE_VAR(iLoop))); \
         pGroup != NULL; pGroup = static_cast<CvSelectionGroupAI*>((kOwner).nextSelectionGroup(&UNIQUE_VAR(iLoop))))

#define FOR_EACH_UNITAI_VAR(pLoopUnit, kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvUnitAI* pLoopUnit = static_cast<CvUnitAI*>((kOwner).firstUnit(&UNIQUE_VAR(iLoop))); \
         pLoopUnit != NULL; pLoopUnit = static_cast<CvUnitAI*>((kOwner).nextUnit(&UNIQUE_VAR(iLoop))))

// TODO: should this use const units?
#define FOR_EACH_UNIT_IN(pLoopUnit, kGroup) \
    for (CLLNode<IDInfo>* pUnitNode = (kGroup).headUnitNode(); pUnitNode != NULL; pUnitNode = (kGroup).nextUnitNode(pUnitNode)) \
        if (CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data)) \
            if (pLoopUnit != NULL)

#define FOR_EACH_UNITAI_IN(pLoopUnit, kGroup) \
    for (CLLNode<IDInfo>* pUnitNode = (kGroup).headUnitNode(); pUnitNode != NULL; pUnitNode = (kGroup).nextUnitNode(pUnitNode)) \
        if (const CvUnitAI* pLoopUnit = static_cast<const CvUnitAI*>(::getUnit(pUnitNode->m_data))) \
            if (pLoopUnit != NULL)

#define FOR_EACH_UNIT_VAR_IN(pLoopUnit, kGroup) \
    for (CLLNode<IDInfo>* pUnitNode = (kGroup).headUnitNode(); pUnitNode != NULL; pUnitNode = (kGroup).nextUnitNode(pUnitNode)) \
        if (CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data)) \
            if (pLoopUnit != NULL)

#define FOR_EACH_UNITAI_VAR_IN(pLoopUnit, kGroup) \
    for (CLLNode<IDInfo>* pUnitNode = (kGroup).headUnitNode(); pUnitNode != NULL; pUnitNode = (kGroup).nextUnitNode(pUnitNode)) \
        if (CvUnitAI* pLoopUnit = static_cast<CvUnitAI*>(::getUnit(pUnitNode->m_data))) \
            if (pLoopUnit != NULL)


#define FOR_EACH_NON_CENTER_CITY_PLOT(pLoopPlot, targetCity) \
    for (CityPlotTypes eCityPlot = FIRST_CITY_PLOT; eCityPlot < NUM_CITY_PLOTS; ++eCityPlot) \
        if (CvPlot* const pLoopPlot = targetCity->getCityIndexPlot(eCityPlot)) \
            if (eCityPlot != CITY_HOME_PLOT && pLoopPlot != NULL)

// TODO: What about checking if we have met the player ?
/*
#define FOR_EACH_OTHER_ALIVE_POTENTIAL_ENEMY_COLONIAL_POWER_TEAM(id, teamVar, ACTION) \
    for (int iI = 0; iI < MAX_TEAMS; iI++) { \
        const CvTeamAI& teamVar = GET_TEAM(iI); \
        if (iI != id && teamVar.isAlive() && teamVar.AI_isColonialPower()) { \
            ACTION; \
        } \
    }
*/

/*
#define FOR_EACH_OTHER_ALIVE_POTENTIAL_ENEMY_COLONIAL_POWER_TEAM(teamId, kTeam, ACTION) \
    for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam) { \
        if (eTeam != (teamId)) { \
            const CvTeamAI& kTeam = GET_TEAM(eTeam); \
            ACTION; \
        } \
    }
*/
/*
#define FOR_EACH_TEAM(kTeam, ACTION) \
    do { \
        for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam) { \
            const CvTeamAI& (kTeam) = GET_TEAM(eTeam); \
            ACTION; \
        } \
    } while (0)
 */

/*
#define FOR_EACH_TEAM(kTeam, ACTION) \
    for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam) \
        if (1) \
        { \
    		const CvTeamAI& kTeam = GET_TEAM(eTeam); \
            ACTION; \
        } \
 */

#define FOREACH_CITY_OF_OWNER(pLoopCity, kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvCity* pLoopCity = (kOwner).firstCity(&UNIQUE_VAR(iLoop)); pLoopCity != NULL; pLoopCity = (kOwner).nextCity(&UNIQUE_VAR(iLoop)))    

#define FOR_EACH_CITYAI_OF_OWNER(pLoopCity, kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvCityAI* pLoopCity = static_cast<CvCityAI*>((kOwner).firstCity(&UNIQUE_VAR(iLoop))); pLoopCity != NULL; pLoopCity = static_cast<CvCityAI*>((kOwner).nextCity(&UNIQUE_VAR(iLoop)))) 

#define FOR_EACH_ADJ_PLOT(pCenterPlot, ACTION) \
    for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++) \
    { \
        CvPlot* const pAdjacentPlot = plotDirection(pCenterPlot->getX_INLINE(), pCenterPlot->getY_INLINE(), ((DirectionTypes)iI)); \
        if (pAdjacentPlot != NULL) \
        { \
                ACTION; \
        } \
    }

#endif
