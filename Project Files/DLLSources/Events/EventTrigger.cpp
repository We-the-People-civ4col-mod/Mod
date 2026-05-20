#include "EventTrigger.h"
#include "../DesyncMonitor.h"

EventTriggeredData::EventTriggeredData()
{
	resetSavedData();
}

int EventTriggeredData::getID() const
{
	return m_iId;
}

void EventTriggeredData::setID(int iID)
{
	m_iId = iID;
}

Coordinates EventTriggeredData::coordinates() const
{
	return Coordinates(m_iPlotX, m_iPlotY);
}

// gives the random number for the event in question, returns 0 if event not set/found
int EventTriggeredData::getRandomNumber(EventTypes eEvent) const
{
	for (std::vector<RandomContainer>::const_iterator it = m_RandomNumbers.begin(); it != m_RandomNumbers.end(); ++it)
	{
		if (it->event == eEvent)
		{
			return it->number;
		}
	}
	return 0;
}

int EventTriggeredData::getRandomNumberForIndex(int iEventIndexInTriggerInfo) const
{
	if (iEventIndexInTriggerInfo >= 0 && isInRange(m_eTrigger))
	{
		const CvEventTriggerInfo& info = GC.getEventTriggerInfo(m_eTrigger);
		const InfoArray<EventTypes>& IA = info.getEvents();
		if (IA.getLength() > iEventIndexInTriggerInfo)
		{
			const EventTypes eEvent = IA.getEvent(iEventIndexInTriggerInfo);
			return getRandomNumber(eEvent);
		}
	}
	return 0;
}

void EventTriggeredData::setRandomNumbers()
{
	FAssert(isInRange(m_eTrigger));
	FAssert(m_RandomNumbers.size() == 0);
	FAssert(CxDesyncMonitor::isCurrentlySync());
	
	const CvEventTriggerInfo& info = GC.getEventTriggerInfo(m_eTrigger);
	const InfoArray<EventTypes>& IA = info.getEvents();

	const int length = IA.getLength();

	if (length > 0)
	{
		m_RandomNumbers.reserve(length);

		for (int i = 0; i < length; ++i)
		{
			const EventTypes eEvent = IA.getEvent(i);

			RandomContainer container;
			container.event = eEvent;
			container.number = GC.getGameINLINE().getSorenRandNum(1000, "Event random number");
			m_RandomNumbers.push_back(container);
		}
	}
}

