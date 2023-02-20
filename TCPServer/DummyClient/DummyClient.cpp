#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ServerPacketHandler.h"

char sendData[] = "Hello World";

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		cout << "Connected To Server" << endl;
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketSessionRef session = GetPacketSessionRef();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		ServerPacketHandler::HandlePacket(session, buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
		cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		//cout << "Disconnected" << endl;
	}
};

int main()
{
	
	//this_thread::sleep_for(chrono::seconds(1));

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress("127.0.0.1", 631),
		MakeShared<EpollCore>(),
		MakeShared<ServerSession>, // TODO : SessionManager µî
		1);

	ASSERT_CRASH(service->Start());

	/*for (int32 i = 0; i < 1; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetEpollCore()->Dispatch();
				}
			});
	}*/

	PKT_C_AUDIO_DATA_WRITE pktWriter(C_AUDIO_DATA, 12);

	SendBufferRef sendBuffer = pktWriter.CloseAndReturn();

	while (true)
	{
		service->Broadcast(sendBuffer);
		this_thread::sleep_for(chrono::seconds(10));
	}

	GThreadManager->Join();
}