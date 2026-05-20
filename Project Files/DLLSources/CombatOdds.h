#ifndef COMBAT_ODDS_H
#define COMBAT_ODDS_H

class CvUnit;
class CvPlot;

struct CombatPreviewOdds
{
	int iAttackerWin;
	int iDefenderWin;
	int iUnresolved;
};

/*
	Returns 0..1000 odds for the 3 preview buckets:
	- attacker win: attacker kills defender
	- defender win: defender kills attacker
	- unresolved: nobody dies (round limit, withdrawal, or evasion)
*/
CombatPreviewOdds getCombatPreviewOdds(CvUnit* pAttacker, CvUnit* pDefender, CvPlot* pPlot);

/*  Returns 0..1000 attacker win odds. */
int getCombatOdds(CvUnit* pAttacker, CvUnit* pDefender);

/*
	Returns 0..1000 pure round-limit draw odds only.
	This preserves the old meaning of "draw".
*/
int getCombatOddsDraw(CvUnit* pAttacker, CvUnit* pDefender);

/*
	Returns 0..1000 total unresolved odds.
	This is what the 3-color preview should use for the yellow bucket.
*/
int getCombatUnresolvedOdds(CvUnit* pAttacker, CvUnit* pDefender);

/*
	Returns 0..100 effective odds on the scale expected by AI_opportuneOdds style code.
	The formula is:
		effective = win + unresolved * weight
	where weight is in percent, usually 40.
*/
int getCombatAIEffectiveOdds100(CvUnit* pAttacker, CvUnit* pDefender,
		int iUnresolvedWeightPercent = 40);

#endif
