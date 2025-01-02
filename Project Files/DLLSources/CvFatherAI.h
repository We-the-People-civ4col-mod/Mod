#ifndef CV_FATHER_AI_H
#define CV_FATHER_AI_H

#include <deque>
#include "CvGameCoreDLL.h"
#include "CvEnums.h"

class CvTeamAI;

class CvFatherAI
{
public:

    struct TraitValue 
    {
        const int economicValue;
        const int militaryValue;

        TraitValue(int ecoValue, int milValue)
            : economicValue(ecoValue), militaryValue(milValue) {}
    };

    CvFatherAI(CvTeamAI& teamAI);

    // Must be called once per turn
    void AI_updateFatherEvaluation(CvTeamAI& teamAI);
    
    // To be called whenever we need to test for a father becoming available. At least once per turn
    FatherTypes evaluateBestFoundingFatherDecision(const std::vector<FatherTypes>& availableFathers, const CvTeamAI& teamAI) const;

protected:

    // Main Evaluation Function
    int fatherValue(FatherTypes eFather, const CvTeamAI& teamAI) const;

    // Estimation Functions
    int estimateTurnsUntilAffordable(
        int requiredPoliticalPoints,
        int requiredOtherPoints,
        const std::deque<int>& politicalHistory,
        const std::deque<int>& otherHistory,
        int totalPoliticalPoints,
        int totalOtherPoints,
        int turnsElapsed) const;


    FatherPointTypes getPrimaryPointType(FatherTypes eFather) const;
    FatherPointTypes getSecondaryPointType(FatherTypes eFather) const;
    TraitValue evaluateTrait(const CvTraitInfo& kTraitInfo, const CvTeamAI& teamAI) const;

    CvTeamAI& m_teamAI;

};

#endif // CV_FATHER_AI_H
