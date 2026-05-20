#include "CvGameCoreDLL.h"
#include "CombatOdds.h"
#include "CvUnit.h"
#include "CvPlot.h"
#include "ScaledNum.h"

namespace
{
	struct CombatPreviewParams
	{
		int iDefenderOdds;
		int iCombatDieSides;

		int iAttackerCurrHitPoints;
		int iDefenderCurrHitPoints;

		int iDamageToAttacker;
		int iDamageToDefender;

		int iAttackerWithdrawalProbability;
		int iAttackerEvasionProbability;
		int iDefenderEvasionProbability;

		int iMaxCombatRounds;
	};

	struct CombatPreviewOddsInternal
	{
		int iAttackerWin;
		int iDefenderWin;
		int iUnresolved;
		int iRoundLimitDraw;
	};

	enum
	{
		BASE_MAX_COMBAT_ROUNDS = 7,
		COMBAT_ODDS_FRONTIER_SIZE = BASE_MAX_COMBAT_ROUNDS + 1
	};

	/*
		ScaledNum<iSCALE, IntType> stores values in steps of 1 / iSCALE.
		For probability values constrained to [0, 1], range is not the issue,
		precision is. The largest convenient power-of-two scale that still fits
		comfortably into the signed int template parameter is 1 << 30.

		Why this is a good choice here:
		- epsilon is 1 / 2^30
		- values remain in [0, 1]
		- products of two probability numerators stay below 2^60
		- ScaledNum already widens 32-bit unsigned products to unsigned __int64
		- power-of-two scale gives the best chance for efficient code generation
	*/
	enum
	{
		COMBAT_PROB_SCALE = (1 << 30)
	};

	typedef ScaledNum<COMBAT_PROB_SCALE, uint> combat_prob_t;
	int clampProbabilityPercent(int iValue)
	{
		if (iValue < 0)
			return 0;
		if (iValue > 100)
			return 100;
		return iValue;
	}

	int getMaxCombatRounds(const CvUnit* pAttacker, const CvUnit* pDefender)
	{
		FAssert(pAttacker != NULL);
		FAssert(pDefender != NULL);
		const int iRounds = BASE_MAX_COMBAT_ROUNDS;
		FAssert(iRounds >= 0);
		FAssert(iRounds <= BASE_MAX_COMBAT_ROUNDS);
		return iRounds;
	}

	bool buildCombatPreviewParams(CvUnit* pAttacker, CvUnit* pDefender, CvPlot* pPlot,
			CombatPreviewParams& kParams)
	{
		FAssert(pAttacker != NULL);
		FAssert(pDefender != NULL);
		FAssert(pPlot != NULL);

		if (pAttacker == NULL || pDefender == NULL || pPlot == NULL)
			return false;

		const int iAttackerStrength = pAttacker->currCombatStr(NULL, NULL, NULL);
		const int iAttackerFirepower = pAttacker->currFirepower(NULL, NULL);

		int iDefenderStrength = 0; // Filled by getDefenderCombatValues; not needed here.
		int iAttackerDamage = 0;
		int iDefenderDamage = 0;
		int iDefenderOdds = 0;

		pAttacker->getDefenderCombatValues(*pDefender, pPlot,
				iAttackerStrength, iAttackerFirepower,
				iDefenderOdds, iDefenderStrength,
				iAttackerDamage, iDefenderDamage,
				NULL);

		const int iCombatDieSides = GLOBAL_DEFINE_COMBAT_DIE_SIDES;

		FAssert(iCombatDieSides > 0);
		FAssert(iDefenderOdds >= 0);
		FAssert(iDefenderOdds <= iCombatDieSides);
		FAssert(iAttackerDamage > 0);
		FAssert(iDefenderDamage > 0);

		if (iCombatDieSides <= 0 || iAttackerDamage <= 0 || iDefenderDamage <= 0)
			return false;

		kParams.iDefenderOdds = iDefenderOdds;
		kParams.iCombatDieSides = iCombatDieSides;

		kParams.iAttackerCurrHitPoints = pAttacker->currHitPoints();
		kParams.iDefenderCurrHitPoints = pDefender->currHitPoints();

		kParams.iDamageToAttacker = iAttackerDamage;
		kParams.iDamageToDefender = iDefenderDamage;

		kParams.iAttackerWithdrawalProbability =
				clampProbabilityPercent(pAttacker->withdrawalProbability());

		kParams.iAttackerEvasionProbability = 0;
		if (pAttacker->getEvasionCity() != NULL)
		{
			kParams.iAttackerEvasionProbability =
					clampProbabilityPercent(pAttacker->getEvasionProbability(*pDefender));
		}

		kParams.iDefenderEvasionProbability = 0;
		if (pDefender->getEvasionCity() != NULL)
		{
			kParams.iDefenderEvasionProbability =
					clampProbabilityPercent(pDefender->getEvasionProbability(*pAttacker));
		}

		kParams.iMaxCombatRounds = getMaxCombatRounds(pAttacker, pDefender);

		FAssert(kParams.iAttackerCurrHitPoints > 0);
		FAssert(kParams.iDefenderCurrHitPoints > 0);
		FAssert(kParams.iMaxCombatRounds >= 0);
		FAssert(kParams.iMaxCombatRounds <= BASE_MAX_COMBAT_ROUNDS);

		if (kParams.iAttackerCurrHitPoints <= 0 || kParams.iDefenderCurrHitPoints <= 0)
			return false;

		return true;
	}

	CombatPreviewOddsInternal calculateCombatPreviewOddsInternal(const CombatPreviewParams& kParams)
	{
		CombatPreviewOddsInternal kResult;
		kResult.iAttackerWin = 0;
		kResult.iDefenderWin = 1000;
		kResult.iUnresolved = 0;
		kResult.iRoundLimitDraw = 0;

		FAssert(kParams.iCombatDieSides > 0);
		FAssert(kParams.iDefenderOdds >= 0);
		FAssert(kParams.iDefenderOdds <= kParams.iCombatDieSides);
		FAssert(kParams.iAttackerCurrHitPoints > 0);
		FAssert(kParams.iDefenderCurrHitPoints > 0);
		FAssert(kParams.iDamageToAttacker > 0);
		FAssert(kParams.iDamageToDefender > 0);
		FAssert(kParams.iAttackerWithdrawalProbability >= 0 && kParams.iAttackerWithdrawalProbability <= 100);
		FAssert(kParams.iAttackerEvasionProbability >= 0 && kParams.iAttackerEvasionProbability <= 100);
		FAssert(kParams.iDefenderEvasionProbability >= 0 && kParams.iDefenderEvasionProbability <= 100);
		FAssert(kParams.iMaxCombatRounds >= 0);
		FAssert(kParams.iMaxCombatRounds <= BASE_MAX_COMBAT_ROUNDS);

		if (kParams.iCombatDieSides <= 0)
		{
			FAssert(false);
			return kResult;
		}

		if (kParams.iMaxCombatRounds <= 0)
		{
			kResult.iAttackerWin = 0;
			kResult.iDefenderWin = 0;
			kResult.iUnresolved = 1000;
			kResult.iRoundLimitDraw = 1000;
			return kResult;
		}

		const combat_prob_t rDefenderRoundWin(kParams.iDefenderOdds, kParams.iCombatDieSides);
		const combat_prob_t rAttackerWithdrawal(kParams.iAttackerWithdrawalProbability, 100);
		const combat_prob_t rAttackerEvasion(kParams.iAttackerEvasionProbability, 100);
		const combat_prob_t rDefenderEvasion(kParams.iDefenderEvasionProbability, 100);

		const int L = kParams.iMaxCombatRounds;

		combat_prob_t curr[COMBAT_ODDS_FRONTIER_SIZE];
		combat_prob_t next[COMBAT_ODDS_FRONTIER_SIZE];

		int i;
		for (i = 0; i < COMBAT_ODDS_FRONTIER_SIZE; ++i)
		{
			curr[i] = 0;
			next[i] = 0;
		}
		curr[0] = 1;

		combat_prob_t rAttackerWin = 0;
		combat_prob_t rDefenderWin = 0;
		combat_prob_t rUnresolvedEscape = 0;
		combat_prob_t rRoundLimitDraw = 0;

		for (int iRound = 0; iRound < L; ++iRound)
		{
			for (i = 0; i < COMBAT_ODDS_FRONTIER_SIZE; ++i)
				next[i] = 0;

			for (int iA = 0; iA <= iRound; ++iA)
			{
				const combat_prob_t rState = curr[iA];
				if (rState <= 0)
					continue;

				const int iD = iRound - iA;

				// Split by subtraction so that the two branches sum exactly to rState.
				const combat_prob_t rDefenderBranch = rState * rDefenderRoundWin;
				const combat_prob_t rAttackerBranch = rState - rDefenderBranch;

				// Defender wins this round; attacker is about to take damage.
				if (rDefenderBranch > 0)
				{
					const int iAttackerDamageAfterHit = iD + 1;
					const int iAttackerHitPointsAfterHit =
							kParams.iAttackerCurrHitPoints -
							(iAttackerDamageAfterHit * kParams.iDamageToAttacker);

					if (iAttackerHitPointsAfterHit <= 0)
					{
						// resolveCombat order:
						// 1) withdrawal
						// 2) attacker evasion
						// 3) death
						const combat_prob_t rWithdrawalBranch = rDefenderBranch * rAttackerWithdrawal;
						const combat_prob_t rAfterWithdrawalFail = rDefenderBranch - rWithdrawalBranch;
						const combat_prob_t rAttackerEvasionBranch = rAfterWithdrawalFail * rAttackerEvasion;
						const combat_prob_t rDeathBranch = rAfterWithdrawalFail - rAttackerEvasionBranch;

						rUnresolvedEscape += rWithdrawalBranch;
						rUnresolvedEscape += rAttackerEvasionBranch;
						rDefenderWin += rDeathBranch;
					}
					else
					{
						next[iA] += rDefenderBranch;
					}
				}

				// Attacker wins this round; defender is about to take damage.
				if (rAttackerBranch > 0)
				{
					const int iDefenderDamageAfterHit = iA + 1;
					const int iDefenderHitPointsAfterHit =
							kParams.iDefenderCurrHitPoints -
							(iDefenderDamageAfterHit * kParams.iDamageToDefender);

					if (iDefenderHitPointsAfterHit <= 0)
					{
						// resolveCombat order:
						// 1) defender evasion
						// 2) defender dies
						const combat_prob_t rDefenderEvasionBranch = rAttackerBranch * rDefenderEvasion;
						const combat_prob_t rKillBranch = rAttackerBranch - rDefenderEvasionBranch;

						rUnresolvedEscape += rDefenderEvasionBranch;
						rAttackerWin += rKillBranch;
					}
					else
					{
						next[iA + 1] += rAttackerBranch;
					}
				}
			}

			for (i = 0; i < COMBAT_ODDS_FRONTIER_SIZE; ++i)
				curr[i] = next[i];
		}

		for (i = 0; i <= L; ++i)
			rRoundLimitDraw += curr[i];


		const combat_prob_t rUnresolvedTotal = rRoundLimitDraw + rUnresolvedEscape;

		FAssert(rRoundLimitDraw <= rUnresolvedTotal);

		kResult.iAttackerWin = rAttackerWin.getPermille();
		kResult.iDefenderWin = rDefenderWin.getPermille();
		kResult.iUnresolved = 1000 - kResult.iAttackerWin - kResult.iDefenderWin;

		if (kResult.iUnresolved < 0)
			kResult.iUnresolved = 0;

		// Round pure round-limit draw separately, then clamp to unresolved total.
		// This avoids false assertion failures caused only by independent rounding.
		kResult.iRoundLimitDraw = rRoundLimitDraw.getPermille();
		if (kResult.iRoundLimitDraw > kResult.iUnresolved)
			kResult.iRoundLimitDraw = kResult.iUnresolved;

		FAssert(kResult.iAttackerWin >= 0);
		FAssert(kResult.iDefenderWin >= 0);
		FAssert(kResult.iUnresolved >= 0);
		FAssert(kResult.iRoundLimitDraw >= 0);
		FAssert(kResult.iAttackerWin <= 1000);
		FAssert(kResult.iDefenderWin <= 1000);
		FAssert(kResult.iUnresolved <= 1000);
		FAssert(kResult.iRoundLimitDraw <= 1000);
		FAssert(kResult.iAttackerWin + kResult.iDefenderWin + kResult.iUnresolved == 1000);
		FAssert(kResult.iRoundLimitDraw <= kResult.iUnresolved);
		return kResult;
	}

	CombatPreviewOddsInternal getCombatPreviewOddsInternal(CvUnit* pAttacker, CvUnit* pDefender, CvPlot* pPlot)
	{
		CombatPreviewOddsInternal kFallback;
		kFallback.iAttackerWin = 0;
		kFallback.iDefenderWin = 1000;
		kFallback.iUnresolved = 0;
		kFallback.iRoundLimitDraw = 0;

		CombatPreviewParams kParams;
		if (!buildCombatPreviewParams(pAttacker, pDefender, pPlot, kParams))
		{
			FAssert(false);
			return kFallback;
		}

		return calculateCombatPreviewOddsInternal(kParams);
	}
}

CombatPreviewOdds getCombatPreviewOdds(CvUnit* pAttacker, CvUnit* pDefender, CvPlot* pPlot)
{
	const CombatPreviewOddsInternal kInternal =
			getCombatPreviewOddsInternal(pAttacker, pDefender, pPlot);

	CombatPreviewOdds kResult;
	kResult.iAttackerWin = kInternal.iAttackerWin;
	kResult.iDefenderWin = kInternal.iDefenderWin;
	kResult.iUnresolved = kInternal.iUnresolved;
	return kResult;
}

int getCombatOdds(CvUnit* pAttacker, CvUnit* pDefender)
{
	if (pAttacker == NULL || pDefender == NULL)
	{
		FAssert(false);
		return 0;
	}

	CvPlot* const pPlot = pDefender->plot();
	if (pPlot == NULL)
	{
		FAssert(false);
		return 0;
	}

	return getCombatPreviewOddsInternal(pAttacker, pDefender, pPlot).iAttackerWin;
}

int getCombatOddsDraw(CvUnit* pAttacker, CvUnit* pDefender)
{
	if (pAttacker == NULL || pDefender == NULL)
	{
		FAssert(false);
		return 0;
	}

	CvPlot* const pPlot = pDefender->plot();
	if (pPlot == NULL)
	{
		FAssert(false);
		return 0;
	}

	return getCombatPreviewOddsInternal(pAttacker, pDefender, pPlot).iRoundLimitDraw;
}

int getCombatUnresolvedOdds(CvUnit* pAttacker, CvUnit* pDefender)
{
	if (pAttacker == NULL || pDefender == NULL)
	{
		FAssert(false);
		return 0;
	}

	CvPlot* const pPlot = pDefender->plot();
	if (pPlot == NULL)
	{
		FAssert(false);
		return 0;
	}

	return getCombatPreviewOddsInternal(pAttacker, pDefender, pPlot).iUnresolved;
}

int getCombatAIEffectiveOdds100(CvUnit* pAttacker, CvUnit* pDefender,
		int iUnresolvedWeightPercent)
{
	if (pAttacker == NULL || pDefender == NULL)
	{
		FAssert(false);
		return 0;
	}

	iUnresolvedWeightPercent = clampProbabilityPercent(iUnresolvedWeightPercent);

	CvPlot* const pPlot = pDefender->plot();
	if (pPlot == NULL)
	{
		FAssert(false);
		return 0;
	}

	const CombatPreviewOdds kOdds = getCombatPreviewOdds(pAttacker, pDefender, pPlot);
	const int iEffectivePermille =
			kOdds.iAttackerWin +
			((kOdds.iUnresolved * iUnresolvedWeightPercent + 50) / 100);

	return std::min(100, (iEffectivePermille + 5) / 10);
}
