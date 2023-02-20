#pragma once
#include "sys/epoll.h"

typedef unsigned int		SOCKET;

enum class EventType : uint32
{
	Connect,
	Disconnect,
	Send,
	Accept,
	Recv,
	//PreRecv,
	
};

class EpollEvent
{
public:
	EpollEvent();
	EpollEvent(EventType type);

	void			Init();
	epoll_event* GetEpoll_Event() { return &_epollEvent; }

public:
	EventType eventType;
	EpollObjectRef owner;

private:
	epoll_event _epollEvent;
};

class ConnectEvent : public EpollEvent
{
public:
	ConnectEvent() : EpollEvent(EventType::Connect) { }
};

/*--------------------
	DisconnectEvent
----------------------*/

class DisconnectEvent : public EpollEvent
{
public:
	DisconnectEvent() : EpollEvent(EventType::Disconnect) { }
};

/*----------------
	AcceptEvent
-----------------*/

class AcceptEvent : public EpollEvent
{
public:
	AcceptEvent() : EpollEvent(EventType::Accept) { }

public:
	SessionRef	session = nullptr;
};

/*----------------
	RecvEvent
-----------------*/

class RecvEvent : public EpollEvent
{
public:
	RecvEvent() : EpollEvent(EventType::Recv) { }
};

/*----------------
	SendEvent
-----------------*/

class SendEvent : public EpollEvent
{
public:
	SendEvent() : EpollEvent(EventType::Send) { }

	Vector<SendBufferRef> sendBuffers;
};