#include "pch.h"
#include "ClientPacketHandler.h"
#include "User.h"
#include "DetectingSession.h"
#include "BufferReader.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];


struct PKT_C_AUDIO_DATA
{
	uint16 packetSize; // 공용 헤더
	uint16 packetId; // 공용 헤더
	bool result;

	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_C_AUDIO_DATA);
		if (packetSize < size)
			return false;

		return true;
	}
};

void ClientPacketHandler::HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case C_AUDIO_DATA:
		Handle_C_AUDIO_DATA(session, buffer, len);
		break;
	}
}

bool ClientPacketHandler::Handle_C_LOGIN(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	//if (pkt.success() == false)
	//	return true;

	//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(audioDataPkt);
	//session->Send(sendBuffer);

	return true;
}

bool ClientPacketHandler::Handle_C_AUDIO_DATA(PacketSessionRef & session, BYTE * buffer, int32 len)
{
	
	// TODO
	BufferReader br(buffer, len);

	PKT_C_AUDIO_DATA* pkt = reinterpret_cast<PKT_C_AUDIO_DATA*>(buffer);

	if (pkt->Validate() == false)
		return false;

	cout << "Handle_C_Audio" << endl;
	return true;
}


