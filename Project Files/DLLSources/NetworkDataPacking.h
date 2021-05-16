#ifndef NETWORK_DATA_PACKING_H
#define NETWORK_DATA_PACKING_H

// Firaxis has provided us with network packages where the number of arguments are fixed.
// This becomes a problem if we need to transfer more variables than there are available arguments.
//
// The solution is either to send multiple packages or to pack multiple variables into a single int.
// The less data being transmitted the better meaning packing variables is preferred if possible.
// However merging variables into a single int requires bitmasks, bitshifts and as such magic numbers, which are prone to bugs.
// It becomes even worse if the programmer isn't used to working with binary data like that.
//
// The goal of this file is to make a human readable interface between being a single int and multiple variables.
// The idea is to make structs (everything public) consisting of a union of an int and a struct, both nameless.
// The inner struct and the int are then in the same memory (hence the union) and the struct is used with sensible variable names for reading and writing.
// The network interface then read/write to the int.
// Each such struct should be static asserted against using more than 4 bytes (size of int)
//
// If size is an issue and the compiler won't handle it automatically, then bitfields can be used.
// bool bSomeBool :1;
// That will specify that bool to use only a single bit, allowing up to 32 bools in a single int.
// The tradeoff with bitfields is that they can be slower, particularly when writing to the variable.
// However performance is not a concern for this use case.
//


struct NetworkDataTradeRouteInts
{
	NetworkDataTradeRouteInts(int a = 0) : iNetwork(a) {}

	union
	{
		int iNetwork;
		struct
		{
			unsigned short iImportLimitLevel;
			unsigned short iMaintainLevel;
		};
	};
};

BOOST_STATIC_ASSERT(sizeof(NetworkDataTradeRouteInts) == sizeof(int));

#endif // !NETWORK_DATA_PACKING_H
