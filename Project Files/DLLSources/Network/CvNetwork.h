#pragma once

#include <vector>

class CvNetwork
{
public:
	// Denotes the different message types. Update this with each newly
	// added message type.
	enum PacketTypes
	{
		packet_test,
	};

	static const int iPacketSize = 20;

	// This function should be implemented for all CvNetworkMessage in order
	// to implement execution of the received message.
	virtual void unpack() = 0;

	// This function should be implemented for all CvNetworkMessage in order
	// to implement actual packaging of the message to be sent.
	virtual void pack() = 0;

	// Called by EXE_CvPlayer::setCivic(CivicOptionTypes eIndex, CivicTypes eNewValue)
	// upon receipt of a packet by the exe.
	static void receive(enum PlayerTypes, const int (&data)[iPacketSize]);

	bool isFullyReceived() const;

	virtual ~CvNetwork();

	// should be called from CvGame constructor or similar to ensure the buffer is free at game start
	static void reset();

protected:
	//Defines wherein m_data these special indices are located.
	enum IndexOf
	{
		PACKET_TYPE,
		MESSAGE_LENGTH,
		DATA_START,
	};

	//Constructor for packets-to-be-sent.
	CvNetwork(PacketTypes type);

	//Constructor for packets-to-be-received.
	CvNetwork(PlayerTypes &ePlayer, const int (&data)[iPacketSize]);

	// Switch to return a CvNetworkMessage based on the packet type.
	// Update with each newly added CvNetworkMessage type.
	static CvNetwork* createReceivedMessage(PlayerTypes& ePlayer, const int (&data)[CvNetwork::iPacketSize]);

	// Simply used to add more received data from a packet to the data vector.
	void addToVector(const int(&data)[iPacketSize]);

	// used by CvNetworkMessage::pack() to build the arrays to transmit
	void addData(int iData);

	// Used by CvNetworkMessage:unpack() to execute received message.
	int getData();

	//Returns packet type.
	int getType();

	//Returns packet message length.
	int getLength();
	
	// transmits the vector
	void transmit();

private:
	std::vector<int> m_data;

	//This counter is used by getData() to pull variables out of the vector.
	//-=Don't touch this unless you know what you're doing.=-
	unsigned int m_iDataIndex;
};

template <CvNetwork::PacketTypes T>
class CvNetworkMessage
{
};
