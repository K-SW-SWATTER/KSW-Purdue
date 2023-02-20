#include "pch.h"
#include "EpollCore.h"

EpollCore::EpollCore()
{
	_epollHandle = ::epoll_create(512);
	ASSERT_CRASH(_epollHandle >= 0);
}

EpollCore::~EpollCore()
{
	close(_epollHandle);
}

bool EpollCore::Register(EpollObjectRef epollObject, EpollEvent* epollEvent)
{
	if (epoll_ctl(_epollHandle, EPOLL_CTL_ADD, epollObject->GetHandle(), epollEvent->GetEpoll_Event()) < 0)
	{
		cout << strerror(errno) << endl;
		return false;
	}
	return true;
}

bool EpollCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;

	int n = epoll_wait(_epollHandle, _events, MAXEVENTS, -1);

	for (int i = 0; i < n; i++)
	{
		epoll_event epoll_Event = _events[i];
		EpollEvent* epollEvent = static_cast<EpollEvent*>(epoll_Event.data.ptr);
		EpollObjectRef epollObject = epollEvent->owner;

		/*EpollEvent* epollEvent = static_cast<EpollEvent*>(&_events[i]);
		EpollObjectRef epollObject = epollEvent->owner;*/
		epollObject->Dispatch(epollEvent);
	}
}