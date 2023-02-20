#pragma once

#include "EpollCore.h"
#include "NetAddress.h"

class ServerService;

/*--------------
	Listener
---------------*/

class Listener : public EpollObject
{
public:
	Listener() = default;
	~Listener();

public:
	/* 외부에서 사용 */
	bool StartAccept(ServerServiceRef service);
	void CloseSocket();

public:
	/* 인터페이스 구현 */
	virtual int GetHandle() override;
	virtual void Dispatch(EpollEvent* acceptEvent) override;

private:
	/* 수신 관련 */
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);

protected:
	Atomic<bool> _registeredOnEpoll = false;
	SOCKET _socket;
	Vector<AcceptEvent*> _acceptEvents;
	ServerServiceRef _service;
	unordered_map<EpollEvent*, SOCKET> _SocketMap;
};

