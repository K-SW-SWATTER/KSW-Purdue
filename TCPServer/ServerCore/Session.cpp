#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "EpollEvent.h"
#include "Service.h"

/*--------------
	Session
---------------*/

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

void Session::Send(SendBufferRef sendBuffer)
{
	if (IsConnected() == false)
		return;

	bool registerSend = false;

	// 현재 RegisterSend가 걸리지 않은 상태라면, 걸어준다
	{
		WRITE_LOCK;

		_sendQueue.push(sendBuffer);

		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const char* cause)
{
	if (_connected.exchange(false) == false)
		return;

	// TEMP
	wcout << "Disconnect : " << cause << endl;

	RegisterDisconnect();
}

int Session::GetHandle()
{
	return _socket;
}

void Session::Dispatch(EpollEvent* epollEvent)
{
	cout << "Session::Dispatch" << endl;
	if (epollEvent->eventType == EventType::Recv)
		ProcessRecv();
	else if (epollEvent->eventType == EventType::Disconnect)
		ProcessDisconnect();
}

bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(_socket, 0/*Any address remains*/) == false)
		return false;

	_connectEvent.owner = shared_from_this(); // ADD_REF

	sockaddr_in sockAddr = GetService()->GetNetAddress().GetSockAddr();
	socklen_t sockAddrLen = sizeof(sockAddr);
	if (connect(_socket, reinterpret_cast<sockaddr*>(&sockAddr), sockAddrLen) < 0)
	{
		return false;
	}
	
	ProcessConnect();

	return true;
}

bool Session::RegisterDisconnect()
{

	if (epoll_ctl(GetService()->GetEpollCore()->GetHandle(), EPOLL_CTL_DEL, _socket, _recvEvent.GetEpoll_Event()) < 0)
		return false;

	ProcessDisconnect();

	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.owner = shared_from_this(); // ADD_REF

	if (_registeredOnEpoll == false)
	{
		_registeredOnEpoll.store(true);
		if (epoll_ctl(GetService()->GetEpollCore()->GetHandle(), EPOLL_CTL_ADD, _socket, _recvEvent.GetEpoll_Event()) < 0)
			HandleError(errno);
		cout << "RegisterRecv succeed" << endl;
	}

	
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_sendEvent.owner = shared_from_this(); // ADD_REF

	// Register data to send on sendEvent
	{
		WRITE_LOCK;

		int32 writeSize = 0;
		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO : 예외 체크

			_sendQueue.pop();
			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather (흩어져 있는 데이터들을 모아서 한 방에 보낸다)
	Vector<BYTE> bufs;
	bufs.reserve(_sendEvent.sendBuffers.size());

	uint32 totalBytes = 0;

	for (SendBufferRef sendBuffer : _sendEvent.sendBuffers)
	{
		BYTE* bufOfSendBuf = sendBuffer->Buffer(); ////////////////////
		totalBytes += sendBuffer->WriteSize();

		for (int i = 0; i < sendBuffer->WriteSize(); i++)
		{
			bufs.push_back(bufOfSendBuf[i]);
		}
		//bufs.push_back(bufOfSendBuf);
	}

	//Vector<BYTE> finalBufs(totalBytes);

	int32 numOfBytes = send(_socket, bufs.data(), totalBytes, 0);
	ProcessSend(numOfBytes);

}

void Session::ProcessConnect() // Called in Listener when the Listener socket accept the connection
{
	_connectEvent.owner = nullptr; // RELEASE_REF

	_connected.store(true);


	// Register the session
	GetService()->AddSession(GetSessionRef());

	// Should be redefined in contents side
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent.owner = nullptr; // RELEASE_REF

	OnDisconnected(); // Should be redefined in contents side

	SocketUtils::Close(_socket);

	GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv()
{
	_recvEvent.owner = nullptr; // RELEASE_REF

	while (true)
	{

		int32 numOfBytes = ::recv(_socket, _recvBuffer.WritePos(), _recvBuffer.FreeSize(), 0);

		if (numOfBytes < 0 && errno == EWOULDBLOCK)
		{
			_recvBuffer.Clean();
			break;
		}

		if (numOfBytes == 0)
		{
			Disconnect("Recv 0");
			return;
		}

		_recvBuffer.OnWrite(numOfBytes);

		int32 dataSize = _recvBuffer.DataSize();
		int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize); // Should be redefined in contents side
		if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
		{
			Disconnect("OnRead Overflow");
			return;
		}

	}

	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent.owner = nullptr; // RELEASE_REF
	_sendEvent.sendBuffers.clear(); // RELEASE_REF

	ASSERT_CRASH(numOfBytes >= 0);

	if (numOfBytes == 0)
	{
		Disconnect("Send 0");
		return;
	}


	// Should be redefined in contents side
	OnSend(numOfBytes);

	WRITE_LOCK;
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend(); // If data was pushed in send queue, than try again to send
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case EEXIST:
		Disconnect("Already registered in epoll instance");
		break;
	default:
		// TODO : Log
		cout << "Handle Error : " << errorCode << endl;
		ASSERT_CRASH(false);
		break;
	}
}

/*-----------------
	PacketSession
------------------*/

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

// [size(2)][id(2)][data....][size(2)][id(2)][data....]
int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;
		// 최소한 헤더는 파싱할 수 있어야 한다
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다
		if (dataSize < header.size)
			break;

		// 패킷 조립 성공
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}
