/* ////////////////////////////////////////////////////////////

File Name: ysthreadpool.h
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#ifndef YSTHREADPOOL_IS_INCLUDED
#define YSTHREADPOOL_IS_INCLUDED
/* { */

#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>

class YsThreadPool
{
public:
	class SubThread
	{
	public:
		int threadIdx;

		std::thread t;

		std::mutex taskMutex;
		std::condition_variable taskWait;
		// taskLock owned by the sub thread.
		// The main thread will notify.

		std::mutex readyMutex;
		std::condition_variable readyWait;
		std::unique_lock <std::mutex> *readyLock;
		// readyWait owned by the main thread.
		// The sub thread will notify.

		YsThreadPool *threadPoolPtr;
		bool bailOut,ready,mustStart;

		SubThread();
		void ThreadFunc(void);
	};

private:
	friend class SubThread;
	std::mutex taskMutex;
	int nTask,nTaskDone;
	const std::function <void()> *taskArray;

private:
	// This thread pool may be shared from multiple objects, and may even be
	// used from the inside of a task running from this thread pool.
	// In that case, further branching the threads will only make CPU traffic jam,
	// and also destroys the integrity of the pool.
	// Therefore, this thread pool should not be used while the pooled threads are
	// already running.
	std::mutex preventRecursiveUse;

	long long int nThread;
	SubThread *threadArray;

	static void ThreadEntry(SubThread *subThread);

public:
	YsThreadPool(int nThread=4);
	~YsThreadPool();

	/*! This function runs given tasks using the cached threads.
	    The function will return when all the tasks are completed. */
	void Run(long long int nTask,const std::function <void()> taskArray[]);

	/*! Returns the number of threads in the pool. */
	long long int GetN(void) const;

	/*! Returns the number of threads in the pool. */
	long long int size(void) const;
};

/* } */
#endif
