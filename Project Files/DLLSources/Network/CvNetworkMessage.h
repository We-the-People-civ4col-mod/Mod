#pragma once

#include "CvNetwork.h"

template <>
class CvNetworkMessage<CvNetwork::packet_test> : public CvNetwork
{
public:
	CvNetworkMessage() : CvNetwork(CvNetwork::packet_test) {}
	CvNetworkMessage(PlayerTypes &ePlayer, const int(&data)[iPacketSize]) : CvNetwork(ePlayer, data) {}

	// moves the variables in this class into the vector using addData()
	void pack();

	// uses getData to copy from the vector to the variables in this class
	// once done, call whatever in the code, which is supposed to receive this message
	void unpack();

private:
	// example variables. They can be more complex than that
	int A;
	int B;
	int C;
};
