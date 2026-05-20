#pragma once

#include "../CvGameCoreDLL.h"

class CvSavegameReader;
class CvSavegameWriter;

class EventTriggeredData
{
public:
	EventTriggeredData();

	int m_iId;
	EventTriggerTypes m_eTrigger;
	int m_iTurn;
	PlayerTypes m_ePlayer;
	int m_iCityId;
	int m_iPlotX;
	int m_iPlotY;
	int m_iUnitId;
	PlayerTypes m_eOtherPlayer;
	int m_iOtherPlayerCityId;
	BuildingTypes m_eBuilding;
	CvWString m_szText;
	CvWString m_szGlobalText;

	int getID() const;
	void setID(int iID);

	Coordinates coordinates() const;

	// gives the random number for the event in question, returns 0 if event not set/found
	int getRandomNumber(EventTypes eEvent) const;
	int getRandomNumberForIndex(int iEventIndexInTriggerInfo) const;

	void setRandomNumbers();

	void resetSavedData();

	void read(CvSavegameReader& reader);
	void readVanilla(CvSavegameReader& reader);
	void write(CvSavegameWriter& writer) const;

	struct RandomContainer
	{
		EventTypes event;
		short number;
	};

private:

	std::vector<RandomContainer> m_RandomNumbers;
};
