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

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& value) : data(value), next(nullptr) {}

		T data;
		Node* next;
	};

public:
	void Push(const T& value)
	{
		// 1) create a new node
		// 2) new node's nest -> mHead
		// 3) mHead = new node

		Node* node = new Node(value);
		node->next = mHead;
		
		/*
		if (mHead == node->next)
		{
			mHead = node;
			return true;
		}
		else
		{
			node->next = mHead;
			return false;
		}
		*/
		while (mHead.compare_exchange_weak(node->next, node) == false) {}
	}

	bool TryPop(T& value)
	{
		++mPopCount;

		Node* previousHead = mHead;

		while (previousHead &&
				mHead.compare_exchange_weak(previousHead, previousHead->next) == false) {}

		if (previousHead == nullptr)
		{
			--mPopCount;
			return false;
		}

		value = previousHead->data;
		TryDelete(previousHead);

		//delete oldHead;
		return true;
	}

	void TryDelete(Node* previousHead)
	{
		// if there is no other threads doing Pop but me
		if (mPopCount == 1)
		{
			// trying to delete the pending list also
			Node* node = mPendingList.exchange(nullptr);
			
			// if there was no interruption
			// even if someone interferes, the data is already separated
			if (--mPopCount == 0)
			{
				DeleteNodes(node);
			}
			else if (node)
			{
				ChainPendingNodeList(node);
			}

			delete previousHead;
		}
		else
		{
			ChainPendingNode(previousHead);
			--mPopCount;
		}
	}

	void ChainPendingNodeList(Node* first, Node* last)
	{
		last->next = mPendingList;

		while (mPendingList.compare_exchange_weak(last->next, first) == false) {}
	}

	void ChainPendingNodeList(Node* node)
	{
		Node* last = node;

		while (last->next)
		{
			last = last->next;
		}

		ChainPendingNodeList(node, last);
	}

	void ChainPendingNode(Node* node)
	{
		ChainPendingNodeList(node, node);
	}

	static void DeleteNodes(Node* node)
	{
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}
	}

private:
	std::atomic<Node*> mHead;
	std::atomic<uint32> mPopCount = 0; // the number of threads using TryPop()
	std::atomic<Node*> mPendingList;
};
