#pragma once

#include <thread>
#include <functional>

/*------------------
	ThreadManager
------------------*/

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(std::function<void(void)> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

private:
	Mutex					 mLock;
	std::vector<std::thread> mThreads;
};

