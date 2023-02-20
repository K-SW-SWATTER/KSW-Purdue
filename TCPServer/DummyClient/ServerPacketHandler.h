#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"
#include "PacketProtocol.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum PacketNum : unsigned short
{
	C_LOGIN = 1000,
	S_LOGIN = 1001,
	C_AUDIO_DATA = 1002,
	S_DETECTION_RESULT = 1003,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);


class ServerPacketHandler
{
public:
	static void HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len);

	static bool Handle_S_LOGIN(PacketSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_S_DETECTION_RESULT(PacketSessionRef& session, BYTE* buffer, int32 len);
};

template<typename T, typename C>
class PacketIterator
{
public:
	PacketIterator(C& container, uint16 index) : _container(container), _index(index) { }

	bool				operator!=(const PacketIterator& other) const { return _index != other._index; }
	const T& operator*() const { return _container[_index]; }
	T& operator*() { return _container[_index]; }
	T* operator->() { return &_container[_index]; }
	PacketIterator& operator++() { _index++; return *this; }
	PacketIterator		operator++(int32) { PacketIterator ret = *this; ++_index; return ret; }

private:
	C& _container;
	uint16			_index;
};

template<typename T>
class PacketList
{
public:
	PacketList() : _data(nullptr), _count(0) { }
	PacketList(T* data, uint16 count) : _data(data), _count(count) { }

	T& operator[](uint16 index)
	{
		ASSERT_CRASH(index < _count);
		return _data[index];
	}

	uint16 Count() { return _count; }

	// ranged-base for 지원
	PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
	PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, _count); }

private:
	T* _data;
	uint16		_count;
};

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

class PKT_C_AUDIO_DATA_WRITE
{
public:
	// 서버에 packet id, 사이즈, feature 값들을 보낸다. packetSize는 패킷의 데이터 유효성 검증을 위한 것
	PKT_C_AUDIO_DATA_WRITE(uint64 id, float* inFeatures)
	{
		_sendBuffer = GSendBufferManager->Open(sizeof(PKT_C_AUDIO_DATA));
		_bw = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->AllocSize());

		_pkt = _bw.Reserve<PKT_C_AUDIO_DATA>();
		_pkt->packetSize = 0; // To Fill
		_pkt->packetId = C_AUDIO_DATA;
		float* firstFeature = _bw.Reserve<float>(_pkt->featureCount);

		for (int i = 0; i < _pkt->featureCount; ++i)
			firstFeature[i] = inFeatures[i];

		_pkt->featureOffset = (uint64)firstFeature - (uint64)_pkt;
	}



	SendBufferRef CloseAndReturn()
	{
		// 패킷 사이즈 계산
		_pkt->packetSize = _bw.WriteSize();

		_sendBuffer->Close(_bw.WriteSize());
		return _sendBuffer;
	}

private:
	PKT_C_AUDIO_DATA* _pkt = nullptr;
	SendBufferRef _sendBuffer;
	BufferWriter _bw;
};