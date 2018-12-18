/* ////////////////////////////////////////////////////////////

File Name: ysthreadpool.cpp
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

#include "ysthreadpool.h"



/*
Initial set up:

1. Everything starts from the main thread taking readyLock.  
   The main thread locks readyLock before starting the sub-threads.

2. The main thread launches the sub-threads.

3. The main thread waits for ready signal from the sub-threads.

4. Sub-threads take taskLock.

5. Sub-threads send ready notification to the main thread.


*** First problem & solution ***
Step 5 must occur after Step 3.  If the sub-threads send ready notification before the main thread
is waiting for the notification, the notification won't be received by the main thread.

Solution => The sub-thread takes ready lock, set ready flag, and release the ready lock before 
sending the ready notification.  The sub-thread cannot take ready lock before the main thread 
releases the lock by readyWait.wait()

6. Sub-threads waits for the task notification from the main thread.



Running tasks and waiting for the completion:

7. The using program gives tasks by Run function.

8. The main thread assigns a task to a sub-thread, and send a task notification.

9. Sub-threads start running.

10. The main thread waits for the ready notification.

11. Sub-threads complete tasks and send a ready notification to the main thread.

12. Sub-threads waits for the task notification from the main thread.

*** Second problem & solution ***

Step 8 must occur after Step 6 (or Step 12).  If the sub-thread runs slower, the main thread may end up sending 
the task notification before the sub-thread start waiting.

Solution => The main thread takes the task lock before assigining a task to a sub-thread.  The task lock is 
owned by a sub-thread except while the sub-thread is waiting for the notification.  Therefore, by taking 
the task lock, the main thread can make sure that the sub-thread is waiting for a task assignment.

*** Third problem & solution ***

Step 10 must occur before Step 11.  Similar approach.  In Step 8, the main thread also changes the ready 
flag of the sub-thread to false.  It may look strange because ready flag seems to be controlled by the 
sub-thread.  However, in fact, the main thread owns the ready lock, and the ready lock can be controlled 
by whom is owning the ready lock.  The sub-thread changes the ready flag to true only when it owns the
ready lock, which is when the task is finished.

The sub-thread makes sure the main thread is waiting for the ready notification by taking the ready lock,
and then changing the ready flag from false to true.  The sub-thread can take the ready lock only while 
the main thread is waiting.  This locking guarantees that Step 11 occurs after Step 10.



*/


YsThreadPool::SubThread::SubThread()
{
	// This function is in the main thread.
	bailOut=false;
	ready=false;
}

void YsThreadPool::SubThread::ThreadFunc(void)
{
	// When the main thread starts the ThreadFunc, the main thread owns readyMutex.

	// The main thread must make sure that the sub-thread owns taskMutex before submitting a task.
	std::unique_lock <std::mutex> taskLock(taskMutex);
	mustStart=false;

	{
		// This attempt to take readyMutex waits until readyMutex is released by the main thread.
		// The main thread releases it when the main thread reaches readyWait.wait().
		std::lock_guard <std::mutex> readyLock(readyMutex);

		// Being able to own readyMutex means that the main thread is waiting.
		ready=true;
		printf("Thread %d ready.\n",threadIdx);

		// Release the readyMutex.  The main thread starts running and owns readyMutex.
		readyWait.notify_all();
	}

	for(;;)
	{
		// It is safe to say nTask=0 here since this thread owns taskMutex until it starts waiting.
		// Main thread won't write to nTask until it owns taskMutex.
		mustStart=false;

		//printf("Thread %d waiting.\n",threadIdx);

		// The ready flag needs to be set false from the main thread since the main thread owns the readyMutex at this moment.
		while(true!=mustStart && true!=bailOut)
		{
			taskWait.wait(taskLock);
			// At the exit of wait, this thread holds taskLock.
		}

		// Main thread holds readyMutex, until it reaches readyWait.wait()
		// This thread holds taskMutex.

		if(true==bailOut)
		{
			break;
		}

		//printf("Thread %d start.\n",threadIdx);

		for(;;)
		{
			std::function <void()> task=nullptr;
			{
				std::lock_guard <std::mutex> lock(threadPoolPtr->taskMutex);
				if(threadPoolPtr->nTask<=threadPoolPtr->nTaskDone)
				{
					break;
				}
				else
				{
					task=threadPoolPtr->taskArray[threadPoolPtr->nTaskDone];
					++threadPoolPtr->nTaskDone;
				}
			}
			task();
		}

		//printf("Thread %d ready.\n",threadIdx);

		{
			std::lock_guard <std::mutex> readyLock(readyMutex);
			// This lock makes sure that the main thread is waiting for the ready notification.
			ready=true;
		}

		// What if the main thread spuriously wake up here, and
		readyWait.notify_all();
		// then starts waiting?  Or, is it possible?
		// No, because even if the wakeup was supurious, ready is already true.  Therefore, it won't repeat the while loop and wait again.
	}

	ready=false;
	taskLock.unlock();
}



////////////////////////////////////////////////////////////



/* static */ void YsThreadPool::ThreadEntry(SubThread *subThread)
{
	subThread->ThreadFunc();
}

YsThreadPool::YsThreadPool(int nThread)
{
	this->nThread=nThread;
	threadArray=new SubThread [nThread];

	// The main thread takes ownership of readyLock of each thread.
	for(int idx=0; idx<nThread; ++idx)
	{
		threadArray[idx].threadIdx=idx;
		threadArray[idx].threadPoolPtr=this;
		threadArray[idx].readyLock=new std::unique_lock <std::mutex> (threadArray[idx].readyMutex);
	}

	// Then start threads.
	for(int idx=0; idx<nThread; ++idx)
	{
		std::thread t(ThreadEntry,&threadArray[idx]);
		threadArray[idx].t.swap(t);
	}

	// Wait for the first ready notification.
	for(int idx=0; idx<nThread; ++idx)
	{
		printf("Waiting for thread #%d gets ready.\n",idx);
		while(true!=threadArray[idx].ready)
		{
			threadArray[idx].readyWait.wait(*threadArray[idx].readyLock);
		}
	}
}

YsThreadPool::~YsThreadPool()
{
	for(long long int idx=0; idx<nThread; ++idx)
	{
		{
			std::lock_guard <std::mutex> taskLock(threadArray[idx].taskMutex);
			// This lock makes sure that the sub-thread is waiting for the
			// task notification.
			threadArray[idx].bailOut=true;
		}
		threadArray[idx].taskWait.notify_all();
	}

	for(long long int idx=0; idx<nThread; ++idx)
	{
		threadArray[idx].t.join();
		delete threadArray[idx].readyLock;
	}

	delete [] threadArray;
}

void YsThreadPool::Run(long long int nTask,const std::function<void()> taskArray[])
{
	if(true==preventRecursiveUse.try_lock())
	{
		for(decltype(nThread) idx=0; idx<nThread; ++idx)
		{
			threadArray[idx].taskMutex.lock();
		}

		// All threads are waiting for taskMutex.

		this->nTask=nTask;
		this->nTaskDone=0;
		this->taskArray=taskArray;

		for(decltype(nThread) idx=0; idx<nThread; ++idx)
		{
			threadArray[idx].ready=false;
			threadArray[idx].mustStart=true;
			threadArray[idx].taskMutex.unlock();
			threadArray[idx].taskWait.notify_all();
		}

		for(decltype(nThread) idx=0; idx<nThread; ++idx)
		{
			// A sub-thread may be running, or waiting for the ready_lock.
			// printf("Start waiting for thread %lld\n",idx);
			while(true!=threadArray[idx].ready)
			{
				// This wait releases readyLock.  The sub-thread is now able to write to ready.
				threadArray[idx].readyWait.wait(*threadArray[idx].readyLock);
				// Definitely, the sub-thread released readyLock.  By then, ready is flipped to true.
				// The notification may be spurious.  But, if ready is already flipped to true, the loop is done.
			}
		}
		preventRecursiveUse.unlock();
	}
	else
	{
		// This pool is already in use.  Run sequentially.
		for(decltype(nTask) idx=0; idx<nTask; ++idx)
		{
			taskArray[idx]();
		}
	}
}

long long int YsThreadPool::GetN(void) const
{
	return nThread;
}

long long int YsThreadPool::size(void) const
{
	return nThread;
}
