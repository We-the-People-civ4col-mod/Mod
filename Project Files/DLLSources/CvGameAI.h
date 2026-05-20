#pragma once

// gameAI.h

#ifndef CIV4_GAME_AI_H
#define CIV4_GAME_AI_H

#include "CvGame.h"

class CvGameAI : public CvGame
{

public:

  CvGameAI();
  virtual ~CvGameAI();

  void AI_init();
  void AI_uninit();
	void AI_reset();

	void AI_makeAssignWorkDirty();
	void AI_updateAssignWork();
	int AI_combatValue(UnitTypes eUnit);

	int AI_turnsPercent(int iTurns, int iPercent);
	int AI_adjustedTurn(int iTurn);
	int AI_gameCompletePercent();

	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);
	void AI_resetSavedData();

	void postLoadFixes();

protected:

  int m_iPad;

};

#endif
