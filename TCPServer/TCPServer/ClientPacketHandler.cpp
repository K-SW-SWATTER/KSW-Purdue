#include "pch.h"
#include "ClientPacketHandler.h"
#include "User.h"
#include "DetectingSession.h"
#include "BufferReader.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];


struct PKT_C_AUDIO_DATA
{
	uint16 packetSize; // 공용 헤더, 패킷의 유효성 검증을 위한 것
	uint16 packetId; // 공용 헤더, 받은 패킷이 어떤 종류의 패킷인지 구분하기 위한 것
	uint16 featureOffset; // feature 배열의 시작 주소
	uint16 featureCount = 40; // 40개의 데이터를 보낼 것이므로 40

	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_C_AUDIO_DATA);
		if (packetSize < size)
			return false;

		if (featureOffset + featureCount * sizeof(float) > packetSize)
			return false;

		size += featureCount * sizeof(float);

		if (size != packetSize)
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

	GMLManager.RunModel((float*)&buffer[pkt->featureOffset]); // from featureOffset, byte -> float

	cout << "Handle_C_Audio" << endl;
	return true;
}


