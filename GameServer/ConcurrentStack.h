#pragma once

#include <mutex>

template<typename T>
class LockStack
{
public:
	LockStack() { }

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mStack.push(std::move(value));
		mCV.notify_one();
	}

	bool TryPop(T& value)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mStack.empty())
		{
			return false;
		}

		value = std::move(mStack.top());
		mStack.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		std::unique_lock<std::mutex> lock(mMutex);
		mCV.wait(lock, [this] { return mStack.empty() == false; });
		value = std::move(mStack.top());
		mStack.pop();
	}

	/* 
	checking if the container is empty or not is useless in multi threading environment
	because at the moment we check about it, other threads can interrupt and affect the container
	for example if the container is not empty so the programmer tries to pop the data
	but right at that moment, another thread can cut in and take out the data

	bool Empty() 
	{
		lock_guard<mutex> lock(mMutex);
		return mStack.empty();
	}
	*/

private:
	std::stack<T> mStack;
	std::mutex mMutex;
	std::condition_variable mCV;
};

