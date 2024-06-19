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

#define FOR_EACH_UNITAI_ON_PLOT(pPlot) \
    for (const CLLNode<IDInfo>* pUnitNode = (pPlot)->headUnitNode(); \
         pUnitNode != NULL; \
         pUnitNode = (pPlot)->nextUnitNodeInternal(pUnitNode)) \
         if (CvUnitAI* pLoopUnit = static_cast<CvUnitAI*>(::getUnit(pUnitNode->m_data)))

#define FOR_EACH_GROUPAI(pLoopUnit, kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (const CvSelectionGroupAI* pLoopSelectionGroup = static_cast<const CvSelectionGroupAI*>((kOwner).firstSelectionGroup(&UNIQUE_VAR(iLoop))); \
         pLoopSelectionGroup != NULL; pLoopSelectionGroup = static_cast<const CvSelectionGroupAI*>((kOwner).nextSelectionGroup(&UNIQUE_VAR(iLoop))))

#define FOR_EACH_GROUPAI_VAR(pLoopUnit, kOwner) \
    for (int UNIQUE_VAR(iLoop) = 0, _dummy_flag_ = 1; _dummy_flag_ && ((_dummy_flag_ = 0) || true);) \
    for (CvSelectionGroupAI* pLoopGroup = static_cast<CvSelectionGroupAI*>((kOwner).firstSelectionGroup(&UNIQUE_VAR(iLoop))); \
         pLoopGroup != NULL; pLoopGroup = static_cast<CvSelectionGroupAI*>((kOwner).nextSelectionGroup(&UNIQUE_VAR(iLoop))))

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


#endif
