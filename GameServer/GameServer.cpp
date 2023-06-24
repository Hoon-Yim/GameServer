#include "pch.h"
#include "CorePch.h"

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>

std::queue<int32> q;
std::stack<int32> s;

std::mutex m;

void Push()
{
	while (true)
	{
		std::lock_guard<std::mutex> lock(m);

		int32 value = rand() % 100;
		q.push(value);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Pop()
{
	while (true)
	{
		std::lock_guard<std::mutex> lock(m);

		if (q.empty())
			continue;

		int32 data = q.front();

		q.pop();
		std::cout << data << std::endl;
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
