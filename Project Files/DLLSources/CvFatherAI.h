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

    void AI_updateFatherEvaluation(CvTeamAI& teamAI);
    void AI_evaluateFoundingFathers(const std::vector<FatherTypes>& fatherCandidates, CvTeamAI& teamAI);

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


    //int getTotalPoints(FatherPointTypes ePointType, const CvTeamAI& teamAI) const;
    bool evaluateFoundingFatherDecision(FatherTypes eFather, const CvTeamAI& teamAI) const;
    FatherPointTypes getPrimaryPointType(FatherTypes eFather) const;
    FatherPointTypes getSecondaryPointType(FatherTypes eFather) const;
    bool evaluateFutureFoundingFathers(FatherTypes currentFather, const CvTeamAI& teamAI, int currentFatherValue) const;
    TraitValue evaluateTraitModifiers(const CvTraitInfo& kTraitInfo, const CvTeamAI& teamAI);

    CvTeamAI& m_teamAI;

};

#endif // CV_FATHER_AI_H
