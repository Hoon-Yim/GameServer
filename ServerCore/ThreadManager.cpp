#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(std::function<void(void)> callback)
{
	LockGuard guard(mLock);
	mThreads.push_back(std::thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	for (std::thread& t : mThreads)
	{
		if (t.joinable())
		{
			t.join();
		}
	}

	mThreads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadID = 1;
	LThreadID = SThreadID.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}
