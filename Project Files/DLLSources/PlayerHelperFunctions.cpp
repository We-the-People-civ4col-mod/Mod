#include "CvGameCoreDLL.h"
#include "PlayerHelperFunctions.h"

LocationFlags::LocationFlags()
	: none(false)
	, coastal(false)
	, deepCoastal(false)
	, inland(false)
	, europe(false)
{
}

LocationFlags::LocationFlags(Flags flag)
{
	none        = flag == LocationFlagNone;
	coastal     = flag == LocationFlagCoastal;
	deepCoastal = flag == LocationFlagDeepCoastal;
	inland      = flag == LocationFlagInland;
	europe      = flag == LocationFlagEurope;
}
// make sure LocationFlags is a drop in replacement for just using an enum type
BOOST_STATIC_ASSERT(sizeof(LocationFlags) <= 4);
