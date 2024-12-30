
#include "CvGameCoreDLL.h"
#include "CvGlobals.h"

#include "CvFatherAI.h"

#include "CvEnums.h"

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
        return 1.0 - (static_cast<double>(turnsUntilAffordable) / remainingTurns);  
    }
}

bool CvFatherAI::evaluateFutureFoundingFathers(FatherTypes currentFather, const CvTeamAI& teamAI, int currentFatherValue) const
{
    for (int i = 0; i < GC.getNumFatherInfos(); ++i)
    {
        FatherTypes futureFather = static_cast<FatherTypes>(i);
        if (futureFather == currentFather || !teamAI.canConvinceFather(futureFather))
            continue;

        int futureValue = fatherValue(futureFather, teamAI);
        if (futureValue > currentFatherValue)
        {
            return true; // There's a better Father worth waiting for
        }
    }
    return false; // No better Fathers available
}

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
        FatherPointTypes ePrimaryPointType = getPrimaryPointType(eFather);
        FatherPointTypes eSecondaryPointType = getSecondaryPointType(eFather);

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

void CvFatherAI::AI_evaluateFoundingFathers(const std::vector<FatherTypes>& fatherCandidates, CvTeamAI& teamAI)
{
    FatherTypes bestFather = NO_FATHER;
    double bestValuePerPoint = 0.0;

    for (std::vector<FatherTypes>::const_iterator it = fatherCandidates.begin(); it != fatherCandidates.end(); ++it)
    {
        FatherTypes eFather = *it;

        // Evaluate this father's total value
        int totalValue = fatherValue(eFather, teamAI);
        if (totalValue <= 0)
        {
            continue; // Skip fathers with non-positive value
        }

        // Calculate cost (primary + secondary points required)
        int requiredPrimaryPoints = teamAI.getFatherPointCost(eFather, FATHER_POINT_POLITICAL);
        FatherPointTypes eSecondaryType = getSecondaryPointType(eFather);
        int requiredSecondaryPoints = (eSecondaryType != NO_FATHER_POINT_TYPE) ? teamAI.getFatherPointCost(eFather, eSecondaryType) : 0;

        int totalPointsRequired = requiredPrimaryPoints + requiredSecondaryPoints;

        // Avoid division by zero
        if (totalPointsRequired <= 0)
        {
            continue;
        }

        // Compute value per point
        double valuePerPoint = static_cast<double>(totalValue) / totalPointsRequired;

        // Track the best candidate
        if (valuePerPoint > bestValuePerPoint)
        {
            bestValuePerPoint = valuePerPoint;
            bestFather = eFather;
        }
    }

    // If a best father is found, convince it
    if (bestFather != NO_FATHER)
    {
        teamAI.convinceFather(bestFather, true);
    }
}

bool CvFatherAI::evaluateFoundingFatherDecision(FatherTypes eFather, const CvTeamAI& teamAI) const
{
    // Validate the input
    if (eFather == NO_FATHER)
    {
        return false; // Invalid father type
    }

    // Retrieve the precomputed value for the father
    int fatherValue = teamAI.m_fatherValues[eFather];

    // If the value is zero or negative, we do not accept this father
    if (fatherValue <= 0)
    {
        return false;
    }

    // Check if there are significantly better fathers available in the near future
    bool shouldWaitForBetterFather = evaluateFutureFoundingFathers(eFather, teamAI, fatherValue);

    // Accept the current father if no significantly better option is available
    return !shouldWaitForBetterFather;
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

CvFatherAI::TraitValue CvFatherAI::evaluateTraitModifiers(const CvTraitInfo& kTraitInfo, const CvTeamAI& teamAI)
{
    int iValue = 0;
    int iEconomicValue = 0;
    int iMilitaryValue = 0;


    // Economic Modifiers
    for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++) {
        int iYieldModifier = kTraitInfo.getYieldModifier(iYield);
        if (iYieldModifier > 0) {
            int iCurrentYield = 0; // teamAI.getTotalYieldOutput((YieldTypes)iYield);
            iValue += (iCurrentYield * iYieldModifier) / 100; // Scale by current output
        }

        int iExtraYieldThreshold = kTraitInfo.getExtraYieldThreshold(iYield);
        if (iExtraYieldThreshold > 0) {
            iValue += iExtraYieldThreshold * 2; // Boost value for extra yield thresholds
        }

        int iCityExtraYield = kTraitInfo.getCityExtraYield(iYield);
        if (iCityExtraYield > 0) {
            int iNumCities = teamAI.getNumCities();
            iValue += iCityExtraYield * iNumCities * 5; // Add per-city extra yields
        }

        if (kTraitInfo.isTaxYieldModifier(iYield)) {
            iValue += 20; // Flat value for tax yield modifiers
        }

        int iBuildingRequiredYieldModifier = kTraitInfo.getBuildingRequiredYieldModifier(iYield);
        if (iBuildingRequiredYieldModifier != 0) {
            iValue += iBuildingRequiredYieldModifier * 2; // Adjust based on building requirements
        }
    }

#if 0
    for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); iBuildingClass++) {
        int iBuildingYieldChange = kTraitInfo.getBuildingYieldChange(iBuildingClass, 0); // First yield type
        if (iBuildingYieldChange > 0) {
            int iNumBuildings = teamAI.getBuildingCount((BuildingClassTypes)iBuildingClass);
            iValue += iBuildingYieldChange * iNumBuildings * 10; // 10 points per yield change
        }

        if (kTraitInfo.isFreeBuildingClass(iBuildingClass)) {
            iValue += 100; // Flat value for free buildings
        }

        int iProductionModifier = kTraitInfo.getBuildingProductionModifier(iBuildingClass);
        if (iProductionModifier > 0) {
            int iNumBuildings = teamAI.getBuildingCount((BuildingClassTypes)iBuildingClass);
            iValue += iProductionModifier * iNumBuildings * 5; // Points per production modifier
        }
    }

    // Exploration Modifiers
    int iGoodUniqueLand = kTraitInfo.getGoodUniqueGoodyChanceModifierLand();
    int iGoodUniqueWater = kTraitInfo.getGoodUniqueGoodyChanceModifierWater();
    int iExpectedGoodies = teamAI.getExpectedRemainingGoodies();
    iValue += ((iGoodUniqueLand + iGoodUniqueWater) * iExpectedGoodies) / 100;

    for (int iGoody = 0; iGoody < GC.getNumGoodyInfos(); iGoody++) {
        int iGoodyFactor = kTraitInfo.getGoodyFactor(iGoody);
        if (iGoodyFactor > 0) {
            iValue += iGoodyFactor * 10; // Scale goody factors
        }
    }
#endif

    // Military Modifiers
    for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++) {
        int iMoveChange = kTraitInfo.getUnitMoveChange(iUnitClass);
        int iStrengthModifier = kTraitInfo.getUnitStrengthModifier(iUnitClass);
        int iUnitCount = teamAI.getUnitClassCount((UnitClassTypes)iUnitClass);

        iValue += iUnitCount * (iMoveChange * 5 + iStrengthModifier * 10); // Weight movement and combat

        if (kTraitInfo.isFreePromotionUnitCombat(iUnitClass)) {
            iValue += 50; // Flat value for free promotions
        }
    }

#if 0
    for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); iProfession++) {
        int iMoveChange = kTraitInfo.getProfessionMoveChange(iProfession);
        if (iMoveChange != 0) {
            int iRelevantUnits = teamAI.getProfessionCount((ProfessionTypes)iProfession);
            iValue += iRelevantUnits * iMoveChange * 5;
        }

        int iEquipmentModifier = kTraitInfo.getProfessionEquipmentModifier(iProfession);
        if (iEquipmentModifier < 0) {
            int iRelevantUnits = teamAI.getProfessionCount((ProfessionTypes)iProfession);
            iValue += -iEquipmentModifier * iRelevantUnits; // Convert negative modifier to positive value
        }
    }
#endif

    for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++) {
        if (kTraitInfo.isFreePromotion(iPromotion)) {
            iValue += 100; // Flat value for free promotions
        }
    }

    // Happiness Modifiers
    iValue += kTraitInfo.getUnhappinessFromSlavesModifier() * -2; // Unhappiness reduces value

    // Strategic and Miscellaneous Modifiers
    iValue += kTraitInfo.getMercantileFactor() * 5; // Trade profit adjustment
    
    // AI ignores native anger
    iValue += kTraitInfo.getNativeAngerModifier() * 0; // Negative impact for native anger
    
    iValue += kTraitInfo.getChiefGoldModifier() / 2; // Scale chief gold rewards
    
    // AI does not currently trade with the natives
    iValue += kTraitInfo.getNativeTradeModifier() * 0; // Value for native trade adjustment

    // Known values: 100
    iMilitaryValue += kTraitInfo.getGreatGeneralRateModifier() * 40; // Great general creation rate
    
    // Known values: 50-100
    iMilitaryValue += kTraitInfo.getDomesticGreatGeneralRateModifier() * 20; // Domestic great general creation rate
    
    // Known values: 100
    iMilitaryValue += kTraitInfo.getRebelCombatModifier() * 50; // Rebel combat bonus

    iValue += kTraitInfo.getCultureLevelModifier() * -10; // Adjust culture growth impact
    iValue += kTraitInfo.getEuropeTravelTimeModifier() * -10; // Adjust Europe travel time
    iValue += kTraitInfo.getSpecialistPriceModifier() * -4; // Adjust cost of specialists
    iValue += kTraitInfo.getStorageCapacityModifier() * 2; // Adjust storage impact

    iValue += kTraitInfo.getMaxTaxRateThresholdDecrease() * -5; // Reduce max tax rate threshold
    iValue += kTraitInfo.getTaxRateThresholdModifier() * -5; // Tax rate threshold adjustment
    iValue += kTraitInfo.getNativeAttitudeChange() * 10; // Native attitude adjustment
    iValue += kTraitInfo.getEuropeanAttitudeChange() * 10; // European attitude adjustment
    iValue += kTraitInfo.getKingAttitudeChange() * 10; // King attitude adjustment
    iValue += kTraitInfo.getImprovementPriceModifier() * -3; // Improvement price adjustment

    // AI does not suffer from runaway / revolts
    iValue += kTraitInfo.getLearningByDoingRunawayModifier() * 0; // LBD runaway modifier
    iValue += kTraitInfo.getLearningByDoingRevoltModifier() * 0; // LBD revolt modifier
    
    iValue += kTraitInfo.getLearningByDoingFreeModifier() * 3; // Free LBD modifier

    return TraitValue(iEconomicValue, iMilitaryValue);
}

int CvFatherAI::fatherValue(FatherTypes eFather, const CvTeamAI& teamAI) const {
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

    return iTotalValue;
}
