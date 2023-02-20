#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "EpollEvent.h"
#include "Session.h"
#include "Service.h"

/*--------------
	Listener
---------------*/

Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		// TODO

		xdelete(acceptEvent);
	}
}

bool Listener::StartAccept(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr)
		return false;

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	//EpollEvent* acceptEvent = new AcceptEvent();
	//if (_service->GetEpollCore()->Register(shared_from_this(), acceptEvent) == false)
	//	return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return false;

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
		return false;

	if (IS_VALID_SOCKET(SocketUtils::MakeSocketNonBlocking(_socket)) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;


	const int32 acceptCount = 1;
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		acceptEvent->owner = shared_from_this();
		//epoll_event* acceptEvent = new epoll_event();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

int Listener::GetHandle()
{
	return _socket;
}

void Listener::Dispatch(EpollEvent* epollEvent)
{
	ASSERT_CRASH(epollEvent->eventType == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(epollEvent);
	cout << "Listener's Dispatch" << endl;
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = _service->CreateSession();

	acceptEvent->session = session;


	if (_registeredOnEpoll == false)
	{
		_registeredOnEpoll.store(true);
		if (epoll_ctl(_service->GetEpollCore()->GetHandle(), EPOLL_CTL_ADD, _socket, acceptEvent->GetEpoll_Event()) < 0)
		{
			cout << strerror(errno) << endl;
		}
	}
	else
	{
		
		//if (_SocketMap.find(acceptEvent) == _SocketMap.end())
		//{
		//	SOCKET newSocket = fcntl(_socket, F_DUPFD); // Duplicate the socket fd
		//	if (newSocket < 0)
		//	{
		//		cout << newSocket << "is less than zero" << endl;
		//		cout << strerror(errno) << endl;
		//		return;
		//	}
		//	else
		//	{
		//		_SocketMap[acceptEvent] = newSocket;
		//		if (epoll_ctl(_service->GetEpollCore()->GetHandle(), EPOLL_CTL_ADD, newSocket, acceptEvent->GetEpoll_Event()) < 0)
		//		{
		//			cout << strerror(errno) << endl;
		//		}
		//	}
		//	
		//}
	}
	
	
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	cout << "Listener's ProcessAccept" << endl;

	SessionRef session = acceptEvent->session;

	sockaddr_in inSockAddress; // Client's address
	socklen_t sizeOfSockAddr; // To show length of address
	sizeOfSockAddr = sizeof(inSockAddress);

	SOCKET acceptSocket = accept(_socket, (sockaddr*) &inSockAddress, &sizeOfSockAddr);
	ASSERT_CRASH(IS_VALID_SOCKET(SocketUtils::MakeSocketNonBlocking(acceptSocket)));

	//_SocketMap.erase(acceptEvent);

	session->SetSocket(acceptSocket);
	session->SetNetAddress(NetAddress(inSockAddress));
	session->ProcessConnect();


	// Create new session
	//SessionRef session = _service->CreateSession();
	//SessionRef* newSession = &session;
	//epoll_event* newEvent = new epoll_event();

	//newEvent->events = EPOLLIN | EPOLLET;
	//newEvent->data.ptr = newSession;

	//ASSERT_CRASH(IS_VALID_SOCKET(epoll_ctl(GET_EPOLL_HANDLE /*epoll_fd*/ , EPOLL_CTL_ADD, acceptSocket, connectEvent)));
	
	//SocketUtils::Bind(acceptSocket, inSockAddress);

	
	RegisterAccept(acceptEvent);
}