#include "pch.h"
#include "EpollEvent.h"

EpollEvent::EpollEvent()
{

}

EpollEvent::EpollEvent(EventType type) : eventType(type)
{
	//Init();
	switch (eventType)
	{
	case EventType::Accept:
	case EventType::Recv:
		_epollEvent.events = EPOLLIN | EPOLLET;
		_epollEvent.data.ptr = this;
		break;
	default:
		break;
	}
}

void EpollEvent::Init()
{
	
}