#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"
#include "PacketProtocol.h"
#include "MLManager.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

extern MLManager GMLManager;

enum PacketNum : unsigned short
{
	C_LOGIN = 1000,
	S_LOGIN = 1001,
	C_AUDIO_DATA = 1002,
	S_DETECTION_RESULT = 1003,
};

// Custom Handlers


class ClientPacketHandler
{
public:
	static void HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len);

	static bool Handle_C_LOGIN(PacketSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_C_AUDIO_DATA(PacketSessionRef& session, BYTE* buffer, int32 len);
};

// [ PKT_S_DETECTION_RESULT ][result]
struct PKT_S_DETECTION_RESULT
{
	uint16 packetSize; // 공용 헤더
	uint16 packetId; // 공용 헤더
	bool result;
};

// [ PKT_S_DETECTION_RESULT ][result]
class PKT_S_DETECTION_RESULT_WRITE
{
public:

	PKT_S_DETECTION_RESULT_WRITE(uint64 id, bool result)
	{
		_sendBuffer = GSendBufferManager->Open(5);
		_bw = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->AllocSize());

		_pkt = _bw.Reserve<PKT_S_DETECTION_RESULT>();
		_pkt->packetSize = 0; // To Fill
		_pkt->packetId = S_DETECTION_RESULT;
		_pkt->result = result;
	}

	SendBufferRef CloseAndReturn()
	{
		// 패킷 사이즈 계산
		_pkt->packetSize = _bw.WriteSize();

		_sendBuffer->Close(_bw.WriteSize());
		return _sendBuffer;
	}

private:
	PKT_S_DETECTION_RESULT* _pkt = nullptr;
	SendBufferRef _sendBuffer;
	BufferWriter _bw;
};


#pragma pack()