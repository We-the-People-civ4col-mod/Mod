#include "../CvGameCoreDLL.h"
#include "../EXE/Network.h"
#include "CvNetwork.h"
#include "CvNetworkMessage.h"

std::vector<CvNetwork*> receiving_vector;

//Constructor for packets-to-be-sent.
CvNetwork::CvNetwork(PacketTypes type)
{
	m_data.push_back(type);

	//Push back a temporary size of zero, to be overwritten with the final vector length later.
	m_data.push_back(0);
}

//Constructor for packets-to-be-received.
CvNetwork::CvNetwork(PlayerTypes &ePlayer, const int (&data)[iPacketSize])
{
	//m_iDataIndex starts at DATA_START - 1 because it's incremented with each getData() call,
	//so that the first data pull is actually DATA_START
	//-=Best not to touch this unless you understand what the above sentence means.=-
	m_iDataIndex = DATA_START - 1;

	addToVector(data);
}

CvNetwork::~CvNetwork()
{

}

// Switch to return a CvNetworkMessage based on the packet type.
// Update with each newly added CvNetworkMessage type.
CvNetwork* CvNetwork::createReceivedMessage(PlayerTypes& ePlayer, const int (&data)[CvNetwork::iPacketSize])
{
	switch ((PacketTypes)data[PACKET_TYPE])
	{
	case packet_test: return new CvNetworkMessage<packet_test>(ePlayer, data);
	}
	FAssertMsg(false, CvString::format("missing case %d", data[PACKET_TYPE]).c_str());
	return NULL;
}

// Simply used to add more received data from a packet to the data vector.
void CvNetwork::addToVector(const int (&data)[CvNetwork::iPacketSize])
{
	for (int i = 0; i < iPacketSize; ++i)
	{
		m_data.push_back(data[i]);
	}
}

// Called by EXE_CvPlayer::setCivic(CivicOptionTypes eIndex, CivicTypes eNewValue)
// upon receipt of a packet by the exe.
void CvNetwork::receive(PlayerTypes ePlayer, const int (&data)[CvNetwork::iPacketSize])
{
	if (ePlayer < receiving_vector.size() && receiving_vector[ePlayer] != NULL)
	{
		// already receiving a multi part message
		receiving_vector[ePlayer]->addToVector(data);
	}
	else
	{
		// ensure that the pointer exist for the index in question
		while (ePlayer >= receiving_vector.size())
		{
			receiving_vector.push_back(NULL);
		}
		receiving_vector[ePlayer] = createReceivedMessage(ePlayer, data);
	}

	if (receiving_vector[ePlayer]->isFullyReceived())
	{
		// single part message
		// execute and release memory
		receiving_vector[ePlayer]->unpack();
		SAFE_DELETE_ARRAY(receiving_vector[ePlayer]);
	}
}

bool CvNetwork::isFullyReceived() const
{
	if ((unsigned int)m_data.at(MESSAGE_LENGTH) <= m_data.size())
	{
		return true;
	}
	return false;
}

// should be called from CvGame constructor or similar to ensure the buffer is free at game start
void CvNetwork::reset()
{
	for (unsigned int i = 0; i < receiving_vector.size(); ++i)
	{
		SAFE_DELETE_ARRAY(receiving_vector[i]);
	}
}

// used by CvNetworkMessage::pack() to build the arrays to transmit
void CvNetwork::addData(int iData)
{
	m_data.push_back(iData);
}

// Used by CvNetworkMessage:unpack() to execute received message.
int CvNetwork::getData()
{
	m_iDataIndex++;
	if (m_iDataIndex < m_data.size())
	{
		return m_data[m_iDataIndex];
	}
	else
	{
		return -1;
	}
}

// Returns packet type.
int CvNetwork::getType()
{
	return m_data[PACKET_TYPE];
}

//Returns packet message length.
int CvNetwork::getLength()
{
	return m_data[MESSAGE_LENGTH];
}

// transmits the vector
void CvNetwork::transmit()
{
	// If the size of the array to be transmitted is greater than the max
	// packet size, distribute the array into multiple sequential packets.
	const int size = m_data.size();

	// Overwrite the placeholder zero with the actual vector size.
	m_data[MESSAGE_LENGTH] = size;

	int buffer_index = 0;
	int buffer[iPacketSize];

	// copy the vector into the buffer
	for (int index = 0; index < size; ++index)
	{
		if (buffer_index == iPacketSize)
		{
			// buffer is full. Transmit it and start over filling the same array
			buffer_index = 0;
			EXE::network::sendArray(buffer);
		}
		buffer[buffer_index] = m_data[index];
		buffer_index++;
	}

	// fill the rest of buffer with padding
	for (; buffer_index < iPacketSize; ++buffer_index)
	{
		buffer[buffer_index] = 0;
	}

	// transmit the last now full buffer
	EXE::network::sendArray(buffer);
}
/*
// for exe interface
int getNumCivicOptionInfos()
{
	// this is only used for determining the length of the argument array in CvDLLUtilityIFaceBase::sendUpdateCivics
	// the exe doesn't care about civics or civic options otherwise
	return CvNetwork::iPacketSize;
}

//Should be placed into EXE_CvPlayer
void setCivic(CivicOptionTypes eIndex, CivicTypes eNewValue)
{
	// use a static buffer. The function is called in a loop and messages will never be mixed.
	static int buffer[CvNetwork::iPacketSize];

	if (eIndex >= 0 && eIndex < CvNetwork::iPacketSize)
	{
		buffer[eIndex] = eNewValue;

		// if it's the last index, forward the entire buffer to CvNetwork
		if (eIndex == (CvNetwork::iPacketSize - 1))
		{
			CvNetwork::receive(getID(), buffer);
		}
	}
}
*/
