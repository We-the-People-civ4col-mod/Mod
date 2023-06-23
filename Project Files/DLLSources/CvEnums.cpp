#include "CvGameCoreDLL.h"

template<>
const char* getTypeStr(int eIndex)
{
	if (eIndex == MAX_INT)
	{
		return "MAX_INT";
	}
	if (eIndex == MIN_INT)
	{
		return "MIN_INT";
	}

	static CvString str; // static to keep the memory even after returning;

	str = CvString::format("%d", eIndex);
	return str.c_str();
}

template<>
const char* getTypeStr(unsigned int eIndex)
{
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(short eIndex)
{
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(unsigned short eIndex)
{
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(char eIndex)
{
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(unsigned char eIndex)
{
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(bool eIndex)
{
	return eIndex ? "TRUE" : "FALSE";
}

template<>
const char* getTypeStr(unsigned long eIndex)
{
	static CvString str; // static to keep the memory even after returning;

	str = CvString::format("%llu", eIndex);
	return str.c_str();
}



template<>
const char* getTypeStr(ActivityTypes eIndex)
{
	switch (eIndex)
	{
	case NO_ACTIVITY: return "NO_ACTIVITY";
	case ACTIVITY_AWAKE: return "ACTIVITY_AWAKE";
	case ACTIVITY_HOLD: return "ACTIVITY_HOLD";
	case ACTIVITY_SLEEP: return "ACTIVITY_SLEEP";
	case ACTIVITY_HEAL: return "ACTIVITY_HEAL";
	case ACTIVITY_SENTRY: return "ACTIVITY_SENTRY";
	case ACTIVITY_MISSION: return "ACTIVITY_MISSION";
	}
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(AreaAITypes eIndex)
{
	switch (eIndex)
	{
	case NO_AREAAI: return "NO_AREAAI";
	case AREAAI_OFFENSIVE: return "AREAAI_OFFENSIVE";
	case AREAAI_BALANCED: return "AREAAI_BALANCED";
	case AREAAI_DEFENSIVE: return "AREAAI_DEFENSIVE";
	case AREAAI_MASSING: return "AREAAI_MASSING";
	case AREAAI_ASSAULT: return "AREAAI_ASSAULT";
	case AREAAI_ASSAULT_MASSING: return "AREAAI_ASSAULT_MASSING";
	case AREAAI_NEUTRAL: return "AREAAI_NEUTRAL";
	}
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(ButtonPopupTypes eIndex)
{
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(ChatTargetTypes eIndex)
{
	switch (eIndex)
	{
	case NO_CHATTARGET: return "NO_CHATTARGET";
	case CHATTARGET_ALL: return "CHATTARGET_ALL";
	case CHATTARGET_TEAM: return "CHATTARGET_TEAM";
	}
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(CardinalDirectionTypes eIndex)
{
	switch (eIndex)
	{
	case NO_CARDINALDIRECTION: return "NO_CARDINALDIRECTION";
	case CARDINALDIRECTION_NORTH: return "CARDINALDIRECTION_NORTH";
	case CARDINALDIRECTION_EAST: return "CARDINALDIRECTION_EAST";
	case CARDINALDIRECTION_SOUTH: return "CARDINALDIRECTION_SOUTH";
	case CARDINALDIRECTION_WEST: return "CARDINALDIRECTION_WEST";
	}
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(GameStateTypes eIndex)
{
	switch (eIndex)
	{
	case GAMESTATE_ON: return "GAMESTATE_ON";
	case GAMESTATE_OVER: return "GAMESTATE_OVER";
	case GAMESTATE_EXTENDED: return "GAMESTATE_EXTENDED";
	}
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(InterfaceMessageTypes eIndex)
{
	switch (eIndex)
	{
	case NO_MESSAGE_TYPE: return "NO_MESSAGE_TYPE";
	case MESSAGE_TYPE_INFO: return "MESSAGE_TYPE_INFO";
	case MESSAGE_TYPE_LOG_ONLY: return "MESSAGE_TYPE_LOG_ONLY";
	case MESSAGE_TYPE_DISPLAY_ONLY: return "MESSAGE_TYPE_DISPLAY_ONLY";
	case MESSAGE_TYPE_MAJOR_EVENT: return "MESSAGE_TYPE_MAJOR_EVENT";
	case MESSAGE_TYPE_MINOR_EVENT: return "MESSAGE_TYPE_MINOR_EVENT";
	case MESSAGE_TYPE_CHAT: return "MESSAGE_TYPE_CHAT";
	case MESSAGE_TYPE_COMBAT_MESSAGE: return "MESSAGE_TYPE_COMBAT_MESSAGE";
	case MESSAGE_TYPE_QUEST: return "MESSAGE_TYPE_QUEST";
	case MESSAGE_TYPE_TUTORIAL: return "MESSAGE_TYPE_TUTORIAL";
	}
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(MissionAITypes eIndex)
{
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(PlayerTypes eIndex)
{
	if (eIndex == NO_PLAYER)
	{
		return "NO_PLAYER";
	}
	static CvString name;
	name = CvString::format("PLAYER_%d", eIndex);
	return name.c_str();
}

template<>
const char* getTypeStr(ReplayMessageTypes eIndex)
{
	switch (eIndex)
	{
	case NO_REPLAY_MESSAGE: return "NO_REPLAY_MESSAGE";
	case REPLAY_MESSAGE_MAJOR_EVENT: return "REPLAY_MESSAGE_MAJOR_EVENT";
	case REPLAY_MESSAGE_CITY_FOUNDED: return "REPLAY_MESSAGE_CITY_FOUNDED";
	case REPLAY_MESSAGE_PLOT_OWNER_CHANGE: return "REPLAY_MESSAGE_PLOT_OWNER_CHANGE";
	}
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(TeamTypes eIndex)
{
	if (eIndex == NO_TEAM)
	{
		return "NO_TEAM";
	}
	static CvString name;
	name = CvString::format("TEAM_%d", eIndex);
	return name.c_str();
}

template<>
const char* getTypeStr(UnitAIStates eIndex)
{
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(UnitTravelStates eIndex)
{
	return getTypeStr((int)eIndex);
}

template<>
const char* getTypeStr(WarPlanTypes eIndex)
{
	switch (eIndex)
	{
	case NO_WARPLAN: return "NO_WARPLAN";
	case WARPLAN_ATTACKED_RECENT: return "WARPLAN_ATTACKED_RECENT";
	case WARPLAN_ATTACKED: return "WARPLAN_ATTACKED";
	case WARPLAN_PREPARING_LIMITED: return "WARPLAN_PREPARING_LIMITED";
	case WARPLAN_PREPARING_TOTAL: return "WARPLAN_PREPARING_TOTAL";
	case WARPLAN_LIMITED: return "WARPLAN_LIMITED";
	case WARPLAN_TOTAL: return "WARPLAN_TOTAL";
	case WARPLAN_DOGPILE: return "WARPLAN_DOGPILE";
	case WARPLAN_EXTORTION: return "WARPLAN_EXTORTION";
	}
	return getTypeStr((int)eIndex);
}
