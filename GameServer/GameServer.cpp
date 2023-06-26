#include "pch.h"
#include "CorePch.h"

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>

#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

LockQueue<int32> q;
LockFreeStack<int32> s;

std::mutex m;

void Push()
{
	while (true)
	{
		//std::lock_guard<std::mutex> lock(m);

		int32 value = rand() % 100;
		s.Push(value);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Pop()
{
	while (true)
	{
		int32 data = 0;

		if (s.TryPop(OUT data))
		{
			std::cout << data << std::endl;
		}

	}
}

int main()
{
	std::thread t1(Push);
	std::thread t2(Pop);
	std::thread t4(Pop);

	t1.join();
	t2.join();
	t4.join();
}
