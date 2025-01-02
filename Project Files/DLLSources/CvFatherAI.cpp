
#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvPlayer.h"
#include "CvPlayerAI.h"
#include "CvTeam.h"

#include "CvFatherAI.h"

//#include "CvEnums.h"
#include "BetterBTSAI.h"

CvFatherAI::CvFatherAI(CvTeamAI& teamAI)
    : m_teamAI(teamAI)
{
}

namespace
{
    double calculateRecentTrend(const std::deque<int>& history)
    {
        if (history.empty())
            return 0.0;

        double weightedSum = 0.0;
        double weight = 1.0;
        double weightSum = 0.0;

        for (std::deque<int>::const_reverse_iterator it = history.rbegin(); it != history.rend(); ++it)
        {
            weightedSum += (*it) * weight;
            weightSum += weight;
            weight *= 0.9; // Exponential decay factor
        }

        return weightedSum / weightSum;
    }

    double calculateHybridAverage(const std::deque<int>& history, int totalPoints, int turnsElapsed)
    {
        if (turnsElapsed == 0)
            return 0.0;

        double longTermAverage = static_cast<double>(totalPoints) / turnsElapsed;
        double recentTrend = calculateRecentTrend(history);

        // Blend the two with a weighting
        return (recentTrend * 0.7) + (longTermAverage * 0.3);
    }

    bool isPointGenerationTerminated(const std::deque<int>& history)
    {
        const int minRecentTurns = 5; // Number of recent turns to analyze
        const int threshold = 1;     // Minimum activity threshold

        if (history.size() < static_cast<std::size_t>(minRecentTurns))
            return false; // Not enough data

        // Check if the last `minRecentTurns` are all below the threshold
        for (std::deque<int>::const_reverse_iterator it = history.rbegin();
            it != history.rbegin() + minRecentTurns; ++it)
        {
            if (*it > threshold)
                return false; // Points are still being generated
        }
        return true;
    }

    double calculateDiscountFactor(int turnsUntilAffordable, int maxTurns, int currentTurn)
    {
        if (maxTurns == 0 || turnsUntilAffordable <= 0)
            return 1.0; // No discount

        double remainingTurns = maxTurns - currentTurn;
        if (remainingTurns <= 0)
            return 0.0; // No remaining turns, discount factor becomes 0

        return 1.0 - (static_cast<double>(turnsUntilAffordable) / remainingTurns);
    }
} // end anon namespace

int CvFatherAI::estimateTurnsUntilAffordable(
    int requiredPoliticalPoints,
    int requiredOtherPoints,
    const std::deque<int>& politicalHistory,
    const std::deque<int>& otherHistory,
    int totalPoliticalPoints,
    int totalOtherPoints,
    int turnsElapsed) const
{
    if (turnsElapsed == 0)
        return -1; // Prevent division by zero

    // Detect termination for both point types
    bool politicalTerminated = isPointGenerationTerminated(politicalHistory);
    bool otherTerminated = isPointGenerationTerminated(otherHistory);

    if (politicalTerminated && requiredOtherPoints > 0 && otherTerminated)
    {
        return -1; // Both categories are no longer generating points
    }

    // Calculate hybrid averages for both categories
    double politicalPointsPerTurn = calculateHybridAverage(politicalHistory, totalPoliticalPoints, turnsElapsed);
    double otherPointsPerTurn = calculateHybridAverage(otherHistory, totalOtherPoints, turnsElapsed);

    // If no points are being generated for a required category
    if ((politicalPointsPerTurn <= 0.0 && requiredPoliticalPoints > totalPoliticalPoints) ||
        (otherPointsPerTurn <= 0.0 && requiredOtherPoints > totalOtherPoints))
    {
        return -1; // Points cannot be generated further
    }

    // Estimate the number of turns required for each category
    int remainingPoliticalPoints = requiredPoliticalPoints - totalPoliticalPoints;
    int remainingOtherPoints = requiredOtherPoints - totalOtherPoints;

    int turnsForPolitical = (remainingPoliticalPoints > 0)
        ? static_cast<int>(std::ceil(remainingPoliticalPoints / politicalPointsPerTurn))
        : 0;

    int turnsForOther = (remainingOtherPoints > 0)
        ? static_cast<int>(std::ceil(remainingOtherPoints / otherPointsPerTurn))
        : 0;

    // The required turns will be the maximum of the two
    return std::max(turnsForPolitical, turnsForOther);
}

void CvFatherAI::AI_updateFatherEvaluation(CvTeamAI& teamAI)
{
    // Resize the vector to match the number of Fathers
    teamAI.m_fatherValues.assign(GC.getNumFatherInfos(), 0);

    for (int i = 0; i < GC.getNumFatherInfos(); ++i)
    {
        FatherTypes eFather = static_cast<FatherTypes>(i);

        // Evaluate this Father
        const CvFatherInfo& kFather = GC.getFatherInfo(eFather);
        int value = fatherValue(eFather, teamAI);

        // Get the primary and secondary point types for this Father
        const FatherPointTypes ePrimaryPointType = getPrimaryPointType(eFather);
        const FatherPointTypes eSecondaryPointType = getSecondaryPointType(eFather);

        // Retrieve the point costs
        int requiredPrimaryPoints = teamAI.getFatherPointCost(eFather, ePrimaryPointType);
        int requiredSecondaryPoints = (eSecondaryPointType != NO_FATHER_POINT_TYPE)
            ? teamAI.getFatherPointCost(eFather, eSecondaryPointType)
            : 0;

        // Retrieve the current points
        int totalPrimaryPoints = teamAI.getFatherPoints(ePrimaryPointType);
        int totalSecondaryPoints = (eSecondaryPointType != NO_FATHER_POINT_TYPE)
            ? teamAI.getFatherPoints(eSecondaryPointType)
            : 0;

        // Get the point histories
        const std::deque<int>& primaryHistory = teamAI.getPointHistory(ePrimaryPointType);
        const std::deque<int>& secondaryHistory = (eSecondaryPointType != NO_FATHER_POINT_TYPE)
            ? teamAI.getPointHistory(eSecondaryPointType)
            : std::deque<int>();

        // Estimate turns until affordable
        int turnsUntilAffordable = estimateTurnsUntilAffordable(
            requiredPrimaryPoints,
            requiredSecondaryPoints,
            primaryHistory,
            secondaryHistory,
            totalPrimaryPoints,
            totalSecondaryPoints,
            GC.getGameINLINE().getGameTurn());

        // Check affordability
        const int LOOK_AHEAD_TURNS = 10;
        if (turnsUntilAffordable == -1 || turnsUntilAffordable > LOOK_AHEAD_TURNS)
        {
            teamAI.m_fatherValues[i] = 0; // Unaffordable Fathers have a value of 0
            continue;
        }

        // Discount future value
        double discountFactor = calculateDiscountFactor(
            turnsUntilAffordable,
            GC.getGameINLINE().getMaxTurns(),
            GC.getGameINLINE().getGameTurn());
        int discountedValue = static_cast<int>(value * discountFactor);

        // Store the discounted value
        teamAI.m_fatherValues[i] = discountedValue;
    }
}

FatherTypes CvFatherAI::evaluateBestFoundingFatherDecision(const std::vector<FatherTypes>& availableFathers, const CvTeamAI& teamAI) const
{
    // Define constants
    const int MINIMUM_THRESHOLD = 2000; // Minimum value to consider a Father
    FatherTypes bestFather = NO_FATHER;
    int bestValue = 0;

    // Iterate through all available Fathers
    for (std::vector<FatherTypes>::const_iterator it = availableFathers.begin(); it != availableFathers.end(); ++it)
    {
        FatherTypes eFather = *it;

        // Retrieve the precomputed value for the Father
        int fatherValue = teamAI.m_fatherValues[eFather];

        // Skip if the value is below the threshold
        if (fatherValue < MINIMUM_THRESHOLD)
        {
            if (gTeamLogLevel >= 1)
            {
                logBBAI("   CvFatherAI::evaluateBestFoundingFatherDecision Team %d skips Father %d due to low value (%d)",
                    teamAI.getID(), eFather, fatherValue);
            }
            continue;
        }

        // Get point costs and histories
        const FatherPointTypes ePrimaryPointType = getPrimaryPointType(eFather);
        const FatherPointTypes eSecondaryPointType = getSecondaryPointType(eFather);

        int requiredPrimaryPoints = teamAI.getFatherPointCost(eFather, ePrimaryPointType);
        int requiredSecondaryPoints = (eSecondaryPointType != NO_FATHER_POINT_TYPE)
            ? teamAI.getFatherPointCost(eFather, eSecondaryPointType)
            : 0;

        int totalPrimaryPoints = teamAI.getFatherPoints(ePrimaryPointType);
        int totalSecondaryPoints = (eSecondaryPointType != NO_FATHER_POINT_TYPE)
            ? teamAI.getFatherPoints(eSecondaryPointType)
            : 0;

        const std::deque<int>& primaryHistory = teamAI.getPointHistory(ePrimaryPointType);
        const std::deque<int>& secondaryHistory = (eSecondaryPointType != NO_FATHER_POINT_TYPE)
            ? teamAI.getPointHistory(eSecondaryPointType)
            : std::deque<int>();

        // Estimate turns until affordable
        int turnsUntilAffordable = estimateTurnsUntilAffordable(
            requiredPrimaryPoints,
            requiredSecondaryPoints,
            primaryHistory,
            secondaryHistory,
            totalPrimaryPoints,
            totalSecondaryPoints,
            GC.getGameINLINE().getGameTurn());

        if (turnsUntilAffordable == -1)
        {
            if (gTeamLogLevel >= 1)
            {
                logBBAI("   CvFatherAI::evaluateBestFoundingFatherDecision Team %d skips Father %d due to affordability issues",
                    teamAI.getID(), eFather);
            }
            continue;
        }

        // Discount the Father's value based on time to afford
        double discountFactor = calculateDiscountFactor(
            turnsUntilAffordable,
            GC.getGameINLINE().getMaxTurns(),
            GC.getGameINLINE().getGameTurn());

        int discountedValue = static_cast<int>(fatherValue * discountFactor);

        // Check against opportunity cost: see if waiting for a better Father is worth it
        bool shouldWait = false;
        for (int i = 0; i < GC.getNumFatherInfos(); ++i)
        {
            FatherTypes futureFather = static_cast<FatherTypes>(i);
            if (futureFather == eFather || !teamAI.canConvinceFather(futureFather))
                continue;

            int futureValue = this->fatherValue(futureFather, teamAI);
            if (futureValue > discountedValue)
            {
                shouldWait = true;
                if (gTeamLogLevel >= 1)
                {
                    logBBAI("   CvFatherAI::evaluateBestFoundingFatherDecision Team %d waits for better Father %d (value: %d > %d)",
                        teamAI.getID(), futureFather, futureValue, discountedValue);
                }
                break;
            }
        }

        if (shouldWait)
        {
            continue; // Skip this Father in favor of a better future option
        }

        // Update the best Father if this one is better
        if (discountedValue > bestValue)
        {
            bestValue = discountedValue;
            bestFather = eFather;
        }
    }

    // Log the decision
    if (gTeamLogLevel >= 1)
    {
        if (bestFather != NO_FATHER)
        {
            logBBAI("   CvFatherAI::evaluateBestFoundingFatherDecision Team %d selects Father %d with value %d",
                teamAI.getID(), bestFather, bestValue);
        }
        else
        {
            logBBAI("   CvFatherAI::evaluateBestFoundingFatherDecision Team %d finds no suitable Father",
                teamAI.getID());
        }
    }

    return bestFather;
}

FatherPointTypes CvFatherAI::getPrimaryPointType(FatherTypes eFather) const
{
    return FATHER_POINT_POLITICAL;
}

FatherPointTypes CvFatherAI::getSecondaryPointType(FatherTypes eFather) const
{
    // Find the second-highest point type cost
    FatherPointTypes ePrimaryPointType = getPrimaryPointType(eFather);
    FatherPointTypes eSecondaryPointType = NO_FATHER_POINT_TYPE;

    for (FatherPointTypes ePointType = FIRST_FATHER_POINT; ePointType < NUM_FATHER_POINT_TYPES; ++ePointType)
    {
        if (ePointType != ePrimaryPointType &&
            GC.getFatherInfo(eFather).getPointCost(ePointType) > 0)
        {
            eSecondaryPointType = ePointType;
            break;
        }
    }

    return eSecondaryPointType;
}


namespace
{
    // Evaluate the value of free units provided by a Founding Father
    int evaluateFreeUnits(const CvFatherInfo& kFatherInfo, const CvTeamAI& teamAI, bool bMilitary) {
        int iValue = 0;

        for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++) {
            int iFreeUnits = kFatherInfo.getFreeUnits(iUnitClass);
            if (iFreeUnits <= 0) {
                continue;
            }

            const UnitClassTypes eUnitClass = (UnitClassTypes)iUnitClass;
            const UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(GC.getGameINLINE().getActiveCivilizationType()).getCivilizationUnits(eUnitClass);
            const CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);

            // Military evaluation
            if (bMilitary)
            {
                if (!kUnitInfo.isOnlyDefensive() && kUnitInfo.getCombat() > 0) {

                    // Special case: Europe price is -1
                    if (kUnitInfo.getEuropeCost() == -1)
                    {
                        // Combat
                        iValue += kUnitInfo.getCombat() * 100; // 100 points per combat strength
                        iValue += kUnitInfo.getWithdrawalProbability() * 10;
                        iValue += kUnitInfo.getCityAttackModifier() * 10;
                        iValue += kUnitInfo.getCityDefenseModifier() * 10;
                        iValue += kUnitInfo.getHillsAttackModifier() * 10;
                        iValue += kUnitInfo.getHillsDefenseModifier() * 10;
                        iValue += kUnitInfo.getDomesticBonusModifier() * 10;
                        iValue += kUnitInfo.getBombardRate() * 10;
                        iValue += kUnitInfo.isNoDefensiveBonus() ? -200 : 0;

                        // Movement
                        iValue += kUnitInfo.getMoves() > 1 ? (kUnitInfo.getMoves() - 1) * 300 : 0;
                        iValue += kUnitInfo.allowsMoveIntoPeak() ? 200 : 0;
                        iValue += kUnitInfo.isCanMoveImpassable() ? 200 : 0;
                        iValue += kUnitInfo.isCanMoveAllTerrain() ? 200 : 0;
                        iValue += kUnitInfo.isFlatMovementCost() ? 200 : 0;
                        iValue += kUnitInfo.isIgnoreTerrainCost() ? 200 : 0;
                    }
                    else
                        iValue += kUnitInfo.getEuropeCost();
                }
            }
            // Economic evaluation
            else
            {
                // Special case: Europe price is -1
                if (kUnitInfo.getEuropeCost() == -1 && kUnitInfo.getAfricaCost() == -1)
                {
                    // This is the rare case when non-military unit cannot be bought in either port
                    iValue += 1000;
                }
                else
                {
                    // Unit can be bought in either of these ports
                    const int iCost = std::min(kUnitInfo.getEuropeCost(), kUnitInfo.getAfricaCost());

                    // In case the unit is available in PR which is unsupported for the AI
                    if (iCost != -1)
                    {
                        iValue = iCost;
                    }
                }
            }
        }

        return iValue;
    }
}

// Refined evaluation of revealing improvements provided by a Founding Father
int evaluateRevealImprovements(const CvFatherInfo& kFatherInfo) {
    int iValue = 0;

    // Calculate the remaining turns and apply linear scaling.
    // The scaling factor ensures earlier game reveals are more valuable.
    // The constant 0.0536 was tuned to ensure a total value of ~5000 for the example case.
    int iRemainingTurns = GC.getGameINLINE().getMaxTurns() - GC.getGameINLINE().getElapsedGameTurns();
    double fTurnScalingFactor = (static_cast<double>(iRemainingTurns) / GC.getGameINLINE().getMaxTurns()) * 0.0536;

    // Assume a fixed Goody density for simplicity.
    // This represents the ratio of Goody huts to the total map area (50 huts on a 100x100 map).
    double fGoodyDensity = 50.0 / 10000.0;

    for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++) {
        if (kFatherInfo.isRevealImprovement(iImprovement)) {
            const CvImprovementInfo& kImprovementInfo = GC.getImprovementInfo((ImprovementTypes)iImprovement);

            // Focus only on improvements that are Goody huts
            if (kImprovementInfo.isGoody()) {
                int iBaseValue = 10; // Each Goody hut has a base value of 10

                // Final value scales based on turn progression and Goody density
                iValue += static_cast<int>(iBaseValue * fTurnScalingFactor / fGoodyDensity);
            }
        }
    }

    return iValue;
}

// TODO: Many of the evaluations should be relative!
CvFatherAI::TraitValue CvFatherAI::evaluateTrait(const CvTraitInfo& kTraitInfo, const CvTeamAI& teamAI) const
{
    int iEconomicValue = 0;
    int iMilitaryValue = 0;

    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    {
        const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
        if (kPlayer.isAlive() && kPlayer.getTeam() == teamAI.getID())
        {
            for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
            {
                const int iYieldModifier = kTraitInfo.getYieldModifier(eYield);
                const int iExtraYieldThreshold = kTraitInfo.getExtraYieldThreshold(eYield);
                const int iCityExtraYield = kTraitInfo.getCityExtraYield(eYield);
                const int iTaxYieldModifier = kTraitInfo.isTaxYieldModifier(eYield);
                int iLoop;
                for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
                {
                    const int iRawYield = pLoopCity->yields().getBaseRawYieldProduced(eYield); // Base raw yield for this city

                    if (iYieldModifier != 0)
                    {
                        // Calculate the additional yield provided by the modifier
                        const int iAdditionalYield = (iRawYield * iYieldModifier) / 100;
                        // Estimate the value added by the additional yield
                        iEconomicValue += pLoopCity->AI_estimateYieldValue(eYield, iAdditionalYield);
                    }

                    // Evaluate extra yield threshold (additive)
                    if (iExtraYieldThreshold > 0 && iRawYield >= iExtraYieldThreshold)
                    {
                        int iExtraYield = iRawYield - iExtraYieldThreshold; // Yields beyond the threshold
                        iEconomicValue += pLoopCity->AI_estimateYieldValue(eYield, iExtraYield);
                    }

                    if (iCityExtraYield != 0)
                    {
                        iEconomicValue += pLoopCity->AI_estimateYieldValue(eYield, iCityExtraYield);
                    }
                
                    if (iTaxYieldModifier != 0)
                    {
                        const int iTaxRate = kPlayer.getTaxRate();
                        int iTaxYieldExtra = (iRawYield * iTaxRate) / 100;
                        iEconomicValue += pLoopCity->AI_estimateYieldValue(eYield, iTaxYieldExtra);
                    }

                    for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
                    {
                        const BuildingTypes eBuilding = static_cast<BuildingTypes>(kPlayer.getCivilizationInfo().getCivilizationBuildings(iBuildingClass));

                        if (pLoopCity->isHasBuilding(eBuilding))
                        {
                            int iBuildingYieldChange = kTraitInfo.getBuildingYieldChange(iBuildingClass, eYield);
                            if (iBuildingYieldChange != 0)
                            {              
                                // TODO: Check for building actually producing something!
                                iEconomicValue += pLoopCity->AI_estimateYieldValue(eYield, iBuildingYieldChange);
                            }
                        }
                    }

                } // End city loop
            
                // XML range: -25. Difficult to evaluate, yield discount for future buildings
                int iBuildingRequiredYieldModifier = kTraitInfo.getBuildingRequiredYieldModifier(eYield);
                if (iBuildingRequiredYieldModifier != 0) 
                {
                    iEconomicValue += iBuildingRequiredYieldModifier * -40;
                }
            } // End yield loop


            for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++)
            {
                const BuildingTypes eBuilding = static_cast<BuildingTypes>(kPlayer.getCivilizationInfo().getCivilizationBuildings(iBuildingClass));
                const int iProductionModifier = kTraitInfo.getBuildingProductionModifier(iBuildingClass);

                // XML range: 100 (i.e. doubles hammer production for city for this particular building)
                if (iProductionModifier != 0)
                {
                    int iLoop;
                    for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
                    {
                        if (pLoopCity->isHasBuilding(eBuilding))
                        {
                            // TODO: Use actual yield cost of building and likelihood of making use of it
                            iEconomicValue += 10 * iProductionModifier;
                        }
                    }
                }

                if (kTraitInfo.isFreeBuildingClass(iBuildingClass))
                {
                    int iCitiesWithoutBulding = 0;
                    int iLoop;
                    for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
                    {
                        if (!pLoopCity->isHasBuilding(eBuilding))
                        {
                            iCitiesWithoutBulding++;
                        }

                    }

                    // TODO: Use actual yield cost  making use of it
                    iEconomicValue += 200 * iCitiesWithoutBulding;
                }
            }
        } // End player loop
    }

    // Exploration Modifiers
    
    // XML range: 100
    int iGoodUniqueLand = kTraitInfo.getGoodUniqueGoodyChanceModifierLand();
    int iGoodUniqueWater = kTraitInfo.getGoodUniqueGoodyChanceModifierWater();

    if (iGoodUniqueLand != 0 || iGoodUniqueWater != 0) {
        int iCurrentTurn = GC.getGame().getGameTurn();
        int iGameSpeedModifier = GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getGrowthPercent();
        int iMapSizeModifier = GC.getMap().getWorldSize();

        const int INITIAL_GOODIES_COUNT = 50 * iMapSizeModifier / 100; // Scale goodies count by map size
        const int BASE_VALUE_PER_GOODY = 50; // Base value for each goody
        const int GAME_LENGTH = GC.getGame().getEstimateEndTurn();

        double fTurnScalingFactor = static_cast<double>(GAME_LENGTH - iCurrentTurn) / GAME_LENGTH;
        int iExpectedRemainingGoodies = static_cast<int>(INITIAL_GOODIES_COUNT * fTurnScalingFactor);

        int iGoodieLandValue = (iGoodUniqueLand * iExpectedRemainingGoodies * BASE_VALUE_PER_GOODY) / 100;
        int iGoodieWaterValue = (iGoodUniqueWater * iExpectedRemainingGoodies * BASE_VALUE_PER_GOODY) / 100;

        iEconomicValue += iGoodieLandValue + iGoodieWaterValue;
    }
    
    // Military Modifiers
    for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++) 
    {
        const int iMoveChange = kTraitInfo.getUnitMoveChange(iUnitClass);
        const int iStrengthModifier = kTraitInfo.getUnitStrengthModifier(iUnitClass);
        const int iUnitCount = teamAI.getUnitClassCount((UnitClassTypes)iUnitClass);

        // TODO: Check for valid unit!
        const UnitClassTypes eUnitClass = (UnitClassTypes)iUnitClass;
        const UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(GC.getGameINLINE().getActiveCivilizationType()).getCivilizationUnits(eUnitClass);
        const CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);

        // TODO: We should only consider the units we already have!
        if (iMoveChange != 0)
        {
            if (!kUnitInfo.isOnlyDefensive() && kUnitInfo.getCombat() > 0)
            {
                iMilitaryValue += (2000 * iMoveChange);
            }
            else
            {
                iEconomicValue += (2000 * iMoveChange);
            }
        }

        if (iStrengthModifier != 0)
        {
            if (!kUnitInfo.isOnlyDefensive() && kUnitInfo.getCombat() > 0)
            {
                iMilitaryValue += (1000 * iStrengthModifier);
            }
            else
            {
                // Strength upgrade for non-combat units is not very useful
                iEconomicValue += (100 * iStrengthModifier);
            }
        }
       
        if (kTraitInfo.isFreePromotionUnitCombat(iUnitClass)) 
        {
            for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
            {
                if (kTraitInfo.isFreePromotion(iPromotion))
                {
                    // TODO: Check that we don't already have it
                    iMilitaryValue += 500;
                }
            }
        }
    }

    for (ProfessionTypes eProfession = FIRST_PROFESSION; eProfession < NUM_PROFESSION_TYPES; ++eProfession)
    {
        const CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
        const int iMoveChange = kTraitInfo.getProfessionMoveChange(eProfession);
        if (iMoveChange != 0) 
        {
            
            if (kProfession.isCitizen())
                continue;

            if (!kProfession.isScout() && kProfession.getCombatChange() > 0)
            {
                iMilitaryValue += 2000;
            }
            else if (kProfession.isScout() || kProfession.getWorkRate() > 0 || kProfession.getMissionaryRate() > 0 ||
                kProfession.getNativeTradeRate() > 0)
            {
                iEconomicValue += 2000;
            }
        }

        // XML range: -25, -50
        int iEquipmentModifier = kTraitInfo.getProfessionEquipmentModifier(eProfession);
        if (iEquipmentModifier != 0) 
        {
            for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
            {
                CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
                if (kPlayer.isAlive() && kPlayer.getTeam() == teamAI.getID())
                {
                    for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
                    {
                        // TODO: Determine if this is a civilian or military profession!
                        const int iEquipmentCost = kPlayer.getYieldEquipmentAmount(eProfession, eYield);
                        if (iEquipmentCost > 0) 
                        {
                            // Note: modifier is negative for discounts
                            const int iDiscount = (iEquipmentCost * -iEquipmentModifier) / 100;
                            
                            // TODO: Attempt to estimate the future number of these professions
                            const int iEstimatedFutureProfessionCount = 5;
                            if (!kProfession.isScout() && kProfession.getCombatChange() > 0)
                            {
                                // Why is AI_yieldValue not const ?
                                iMilitaryValue += kPlayer.AI().AI_yieldValue(eYield) * iDiscount * iEstimatedFutureProfessionCount;
                            }
                            else if (kProfession.isScout() || kProfession.getWorkRate() > 0 || kProfession.getMissionaryRate() > 0 ||
                                kProfession.getNativeTradeRate() > 0)
                            {
                                iEconomicValue += kPlayer.AI().AI_yieldValue(eYield) * iDiscount * iEstimatedFutureProfessionCount;
                            }
                        }
                    }
                }
            }
        }
    }

    // XML range: 50, 10
    iEconomicValue += kTraitInfo.getPioneerSpeedModifier() * 200; // Work rate modifier
     
    // XML range: 25. AI mostly ingores happiness
    iEconomicValue += kTraitInfo.getUnhappinessFromSlavesModifier() * 0; // Unhappiness reduces value

    // Strategic and Miscellaneous Modifiers
    
    // XML range: -50
    iEconomicValue += kTraitInfo.getMercantileFactor() * -40; // Reduced impact of sales in any port
    
    // AI ignores native anger
    iEconomicValue += kTraitInfo.getNativeAngerModifier() * 0; // Negative impact for native anger
    
    // XML range: 30
    iEconomicValue += kTraitInfo.getChiefGoldModifier() * 50; // Scale chief gold rewards
    
    // XML range: 10,25,30
    iEconomicValue += kTraitInfo.getNativeTradeModifier() * 50; // Native trading post income modifier 

    // XML range: 100
    iMilitaryValue += kTraitInfo.getGreatGeneralRateModifier() * 40; // Great general creation rate
    
    // XML range: 50-100
    iMilitaryValue += kTraitInfo.getDomesticGreatGeneralRateModifier() * 20; // Domestic great general creation rate
    
    // XML range: 100. Very strong, wins the game! (but only if the AI gets there)
    iMilitaryValue += kTraitInfo.getRebelCombatModifier() * 50; // Rebel combat bonus

    // XML range: -20
    iEconomicValue += kTraitInfo.getCultureLevelModifier() * -50; // Lowers culture threshold, easier border-pop
    
    // XML range: -50
    iEconomicValue += kTraitInfo.getEuropeTravelTimeModifier() * -50; // Reduces Europe (and other ports) travel time
    
    // XML range: -50
    iEconomicValue += kTraitInfo.getSpecialistPriceModifier() * -10; // Reduces graduation (education) price
    
    // XML range: 25
    iEconomicValue += kTraitInfo.getStorageCapacityModifier() * 40; // Adjust storage impact

    // XML range: 5. AI pays low taxes to begin with, not very impacful
    iEconomicValue += kTraitInfo.getMaxTaxRateThresholdDecrease() * 10; // Reduce max tax rate threshold
    
    // XML range: 5
    iEconomicValue += kTraitInfo.getTaxRateThresholdModifier() * 10; // Tax rate threshold adjustment
    
    // TODO: Need to check for chosen war and war success for these
    // XML range: 2,4. Note that this forces peace with natives so it could be very negative!
    iMilitaryValue += kTraitInfo.getNativeAttitudeChange() * 0; // Native attitude adjustment

    // XML range: 2,4. Note that this forces peace with Europeans so it could be very negative!
    iMilitaryValue += kTraitInfo.getEuropeanAttitudeChange() * 0; // European attitude adjustment

    // XML range: 2,5
    iEconomicValue += kTraitInfo.getKingAttitudeChange() * 100; // King attitude adjustment
    
    // XML range: -50 (TODO: do these stack?)
    iEconomicValue += kTraitInfo.getImprovementPriceModifier() * -50; // Improvement price adjustment

    // XML range: -25,-50 (TODO: do these stack?)
    iEconomicValue += kTraitInfo.getImprovementGrowthTimeModifier() * -40; // Improvement price adjustment

    // AI does not suffer from runaway / revolts
    iEconomicValue += kTraitInfo.getLearningByDoingRunawayModifier() * 0; // LBD runaway modifier
    iEconomicValue += kTraitInfo.getLearningByDoingRevoltModifier() * 0; // LBD revolt modifier

    // XML range: -50
    iEconomicValue += kTraitInfo.getLearningByDoingFreeModifier() * -10; // Free LBD modifier

    // XML range: 25. Less impactful than Europe but still strong
    iEconomicValue += kTraitInfo.getAfricaSellProfitModifierInPercent() * 40; // Africa sale price modifier
    
    // XML range: 25. PR is not implemented for the AI
    iEconomicValue += kTraitInfo.getPortRoyalSellProfitModifierInPercent() * 0; // PR sale price modiifer

    // XML range: 25
    iEconomicValue += kTraitInfo.getDomesticMarketProfitModifierInPercent() * 20;

    // XML range: -20,-25,-50. AI never buys land
    iEconomicValue += kTraitInfo.getLandPriceDiscount() * 0; // Owned land (natives) price modifier

    // XML range: 20. Very impactful for both kinds of purchases
    iEconomicValue += kTraitInfo.getRecruitPriceDiscount() * 100; // Europe unit discount
    iMilitaryValue += kTraitInfo.getRecruitPriceDiscount() * 100;

    // XML range: 25.
    iEconomicValue += kTraitInfo.getRecruitPriceDiscountAfrica() * 50; // Africa unit discount
    iMilitaryValue += kTraitInfo.getRecruitPriceDiscountAfrica() * 0; // No military available (TODO: check!)

    // XML range: 25. PR is not implemented for the AI
    iEconomicValue += kTraitInfo.getRecruitPriceDiscountPortRoyal() * 0; // Port Royal unit discount
    iMilitaryValue += kTraitInfo.getRecruitPriceDiscountPortRoyal() * 0;

    // XML range: -10,-20,-30
    iMilitaryValue += kTraitInfo.getLevelExperienceModifier() * -100;
    
    // XML range: -25,-50
    iEconomicValue += kTraitInfo.getLearnTimeModifier() * -50;

    // XML range: 25
    iMilitaryValue += kTraitInfo.getNativeCombatModifier() * 50;
    
    // XML range: 50,100,150
    iEconomicValue += kTraitInfo.getMissionaryModifier() * 10;
    
    // XML range: 20,25,100
    iEconomicValue += kTraitInfo.getTreasureModifier() * 20;
    
    // XML range: -10,-25
    // TODO: Should depend on city count/size and need for more pop
    iEconomicValue += kTraitInfo.getPopGrowthThresholdModifier() * -50;

    // XML range: 10,25
    iMilitaryValue += kTraitInfo.getCityDefense() * 50;

    // XML range: 2
    for (int iGoody = 0; iGoody < GC.getNumGoodyInfos(); ++iGoody)
    {
        // TODO: Differntiate bases on type
        iEconomicValue += kTraitInfo.getGoodyFactor(iGoody) * 1000;
    }
    
    // XML range: -10,-33
    iEconomicValue += kTraitInfo.getImmigrationThresholdModifier() * -200;

    return TraitValue(iEconomicValue, iMilitaryValue);
}

int CvFatherAI::fatherValue(FatherTypes eFather, const CvTeamAI& teamAI) const 
{
    // Initialize total value
    int iTotalValue = 0;

    // Get the Founding Father info
    const CvFatherInfo& kFatherInfo = GC.getFatherInfo(eFather);

    // Economic Value
    int iEconomicValue = 0;
    iEconomicValue += evaluateFreeUnits(kFatherInfo, teamAI, false);

    // Military Value
    int iMilitaryValue = 0;
    iMilitaryValue += evaluateFreeUnits(kFatherInfo, teamAI, true);

    // Strategic Value (Revealing Improvements)
    iEconomicValue += evaluateRevealImprovements(kFatherInfo);

    // Combine values (equal weighting for now)
    iTotalValue = iEconomicValue + iMilitaryValue;

    const TraitTypes eTrait = kFatherInfo.getTrait();

    if (eTrait != NO_TRAIT)
    {
        const CvTraitInfo& kTrait = GC.getTraitInfo(kFatherInfo.getTrait());
        TraitValue tv = evaluateTrait(kTrait, teamAI);
        iTotalValue += tv.economicValue + tv.militaryValue;

    }    
    
    return iTotalValue;
}
