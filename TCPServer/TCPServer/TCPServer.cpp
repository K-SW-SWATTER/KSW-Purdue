#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "DetectingSession.h"
#include "DetectingSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "PacketProtocol.h"
#include "Job.h"
#include "User.h"

#define PY_SSIZE_T_CLEAN
#define PY_LIMITED_API
#include <Python.h>


enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount_64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetEpollCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	//AcceptEvent* acceptEvent = xnew<AcceptEvent>();
	PyObject* pName, * pModule, * pArgs, * pFunc, * pValue, * pData, * pModelPath;
	const char* fileName = "server.server_main";
	const char* funcName = "detect_result";
	const char* modelPath = "/root/projects/Object_Detection/save/svm_model.pkl";
	float data[40] = { -6.26099670e+02,  6.08171043e+01,  1.21538630e+01,  1.10330324e+01,
 -3.38118315e+00,  7.72425175e+00, -8.84613228e+00, -5.71947002e+00,
 -9.74624252e+00, -1.90162218e+00, -1.41135235e+01, -9.04522324e+00,
 -2.92101121e+00, -6.41781187e+00, -4.54213810e+00, -8.88110352e+00,
 -4.29623079e+00, -3.78915739e+00, -6.61831951e+00, -8.36560535e+00,
 -5.85096216e+00, -7.87125444e+00, -6.12021399e+00, -5.98344231e+00,
 -5.71011066e+00, -5.42288208e+00, -4.55634403e+00, -4.21312284e+00,
 -2.76323390e+00, -1.03867352e+00,  2.73384601e-01,  4.97162676e+00,
  4.76197100e+00,  4.28834200e+00,  1.30950105e+00, -2.93097210e+00,
 -2.33082223e+00, -1.85949576e+00, -1.12502873e+00,  3.47768217e-01 };
	uint8 dataLen = sizeof(data) / sizeof(float);


	Py_Initialize();
	pName = PyUnicode_DecodeFSDefault(fileName);
	pModelPath = PyUnicode_DecodeFSDefault(modelPath);
	/* Error checking of pName left out */

	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule == NULL)
	{
		PyErr_Print();
		fprintf(stderr, "Failed to load \"%s\"\n", fileName);
		ASSERT_CRASH(true == false);
	}

	pFunc = PyObject_GetAttrString(pModule, funcName);
	/* pFunc is a new reference */

	

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress("127.0.0.1", 631),
		MakeShared<EpollCore>(),
		MakeShared<DetectingSession>,
		10);

	ASSERT_CRASH(service->Start());

	/*for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}*/

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}