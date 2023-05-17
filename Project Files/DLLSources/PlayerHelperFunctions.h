#pragma once

struct LocationFlags
{
	enum Flags
	{
		LocationFlagNone,
		LocationFlagCoastal,
		LocationFlagDeepCoastal,
		LocationFlagInland,
		LocationFlagEurope,
	};
	bool none        : 1;
	bool coastal     : 1;
	bool deepCoastal : 1;
	bool inland      : 1;
	bool europe      : 1;

	LocationFlags();
	LocationFlags(Flags);
};
