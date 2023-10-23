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
		LocationFlagLargeRiver,
		LocationFlagShallowCoastal
	};
	bool none           : 1;
	bool coastal        : 1;
	bool deepCoastal    : 1;
	bool inland         : 1;
	bool europe         : 1;
	bool largeRiver     : 1;
	bool shallowCoastal : 1;

	LocationFlags();
	LocationFlags(Flags);
	LocationFlags(int);

	int serialize() const;
};
