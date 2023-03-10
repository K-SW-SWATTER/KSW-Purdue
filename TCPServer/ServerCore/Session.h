#pragma once
#include "EpollCore.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

/*--------------
	Session
---------------*/

class Session : public EpollObject
{
	friend class Listener;
	friend class EpollCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, // 64KB
	};

public:
	Session();
	virtual ~Session();

public:
	void				Send(SendBufferRef sendBuffer);
	bool				Connect();
	void				Disconnect(const char* cause);

	shared_ptr<Service>	GetService() { return _service.lock(); }
	void				SetService(shared_ptr<Service> service) { _service = service; }

public:
	void				SetNetAddress(NetAddress address) { _netAddress = address; }
	void				SetSocket(SOCKET socket) { _socket = socket; }
	NetAddress			GetAddress() { return _netAddress; }
	SOCKET				GetSocket() { return _socket; }
	bool				IsConnected() { return _connected; }
	SessionRef			GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
	virtual int			GetHandle() override;
	virtual void		Dispatch(EpollEvent* epollEvent) override;

private:
	bool				RegisterConnect();
	bool				RegisterDisconnect();
	void				RegisterRecv();
	void				RegisterSend();

	void				ProcessConnect();
	void				ProcessDisconnect();
	void				ProcessRecv();
	void				ProcessSend(int32 numOfBytes);

	void				HandleError(int32 errorCode);

protected:
	virtual void		OnConnected() { }
	virtual int32		OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void		OnSend(int32 len) { }
	virtual void		OnDisconnected() { }

private:
	weak_ptr<Service>	_service;
	SOCKET				_socket;
	NetAddress			_netAddress = {};
	Atomic<bool>		_connected = false;
	Atomic<bool>		_registeredOnEpoll = false;

private:
	USE_LOCK;

	/* concerned with receive */
	RecvBuffer				_recvBuffer;

	/* concerned with send */
	Queue<SendBufferRef>	_sendQueue;
	Atomic<bool>			_sendRegistered = false;

private:
	/* EpollEvent re-use */
	ConnectEvent		_connectEvent;
	DisconnectEvent		_disconnectEvent;
	RecvEvent			_recvEvent;
	SendEvent			_sendEvent;
};

/*-----------------
	PacketSession
------------------*/

struct PacketHeader
{
	uint16 size;
	uint16 id; // ProtocolID (ex. 1=login, 2=audio data)
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	PacketSessionRef	GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32		OnRecv(BYTE* buffer, int32 len) final;
	virtual void		OnRecvPacket(BYTE* buffer, int32 len) = 0;
};
