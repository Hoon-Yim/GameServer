#pragma once

#include <mutex>

template<typename T>
class LockQueue
{
public:
	LockQueue() { }

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;

	void Push(T value)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mQueue.push(std::move(value));
		mCV.notify_one();
	}
	
	bool TryPop(T& value)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mQueue.empty())
		{
			return false;
		}

		value = std::move(mQueue.front());
		mQueue.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		std::unique_lock<std::mutex> lock(mMutex);
		mCV.wait(lock, [this] { return mQueue.empty() == false; });
		value = std::move(mQueue.front());
		mQueue.pop();
	}

private:
	std::queue<T> mQueue;
	std::mutex mMutex;
	std::condition_variable mCV;
};
