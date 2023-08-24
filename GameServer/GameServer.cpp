#include "pch.h"
#include "CoreMacro.h"
#include "ThreadManager.h"

CoreGlobal GCoreGlobal;

void ThreadMain()
{
	while (true)
	{
		std::cout << "Hello I am thread.. " << LThreadID << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main()
{
	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->Launch(ThreadMain);
	}

	GThreadManager->Join();
}
