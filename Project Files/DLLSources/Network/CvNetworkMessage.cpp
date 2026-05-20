#include "../CvGameCoreDLL.h"
#include "CvNetworkMessage.h"

//====
//packet_test pack and unpack functions follow.
//====

// moves the variables in this class into the vector using addData()
void CvNetworkMessage<CvNetwork::packet_test>::pack()
{
	//Assign variables to be sent.
	A = 5;
	B = 15;
	C = 45;

	//Prepare the variables to be added to a packet by passing to CvNetwork.
	addData(A);
	addData(B);
	addData(C);

	//Now ask CvNetwork to transmit the packet.
	transmit();
}

// uses getData to copy from the vector to the variables in this class
// once done, call whatever in the code, which is supposed to receive this message
void CvNetworkMessage<CvNetwork::packet_test>::unpack()
{
	int packetType = getType();      //Index of 0
	int messageSize = getLength();  //Index of 1
	A = getData();
	B = getData();
	C = getData();

	FAssertMsg(false, CvString::format("packetType = %d, messageSize = %d, A = %d, B = %d, C = %d", packetType, messageSize, A, B, C));
}

//====
//End packet_test pack and unpack functions.
//====
