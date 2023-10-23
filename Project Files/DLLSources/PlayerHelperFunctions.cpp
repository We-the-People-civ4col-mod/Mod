#include "CvGameCoreDLL.h"
#include "PlayerHelperFunctions.h"

LocationFlags::LocationFlags()
	: none(false)
	, coastal(false)
	, deepCoastal(false)
	, inland(false)
	, europe(false)
	, largeRiver(false)
	, shallowCoastal(false)
{
}

LocationFlags::LocationFlags(int serializedValue) {
	none = serializedValue & 1;
	coastal = (serializedValue >> 1) & 1;
	deepCoastal = (serializedValue >> 2) & 1;
	inland = (serializedValue >> 3) & 1;
	europe = (serializedValue >> 4) & 1;
	largeRiver = (serializedValue >> 5) & 1;
	shallowCoastal = (serializedValue >> 6) & 1;
}


LocationFlags::LocationFlags(Flags flag)
{
	none           = flag == LocationFlagNone;
	coastal        = flag == LocationFlagCoastal;
	deepCoastal    = flag == LocationFlagDeepCoastal;
	inland         = flag == LocationFlagInland;
	europe         = flag == LocationFlagEurope;
	largeRiver     = flag == LocationFlagLargeRiver;
	shallowCoastal = flag == LocationFlagShallowCoastal;
}

int LocationFlags::serialize() const
{
	int result = 0;

	result |= (none << 0);
	result |= (coastal << 1);
	result |= (deepCoastal << 2);
	result |= (inland << 3);
	result |= (europe << 4);
	result |= (largeRiver << 5);
	result |= (shallowCoastal << 6);

	return result;
}

// make sure LocationFlags is a drop in replacement for just using an enum type
BOOST_STATIC_ASSERT(sizeof(LocationFlags) <= 4);
