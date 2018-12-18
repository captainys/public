/* ////////////////////////////////////////////////////////////

File Name: ysthread.h
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

#ifndef YSTHREAD_IS_INCLUDED
#define YSTHREAD_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ystask.h>

#ifdef _WIN32
	#include <thread>
	#include <mutex>
#else
	#include <pthread.h>
	#include <unistd.h>
#endif


/*! 

Apparently, C++11's std::thread::hardware_concurrency has a bug.  Not in implementation, but in the specification.
It must not be allowed to happen, but std::thread::hardware_concurrency can return zero, as in the C++11 spec.
I have confirmed that std::thread::hardware_concurrency can return zero with g++.
It is a critical flaw in design.  Very sad  :-P
Nonetheless, C++11's standard threading library takes care of most of the multi-threading necessities.
Therefore, this class is almost deprecated.  Please do not use except YsThreadController::GetNumCpu.

 */
class YsMutex
{
public:
#ifdef _WIN32
	std::mutex mutex;
#else
	pthread_mutex_t mutex;
#endif

	YsMutex();
	~YsMutex();

	void Lock(void);
	void Unlock(void);
	YSRESULT LockNoWait(void);
};



class YsThreadController
{
public:
	static unsigned int GetNumCPU(void);
	static YSRESULT RunParallel(int nTask,YsTask *const task[]);

	template <const int N>
	static YSRESULT RunParallel(const YsArray <YsTask *> &taskArray);
};

template <const int N>
YSRESULT YsThreadController::RunParallel(const YsArray <YsTask *> &taskArray)
{
	return RunParallel(taskArray.GetN(),taskArray);
}

/* } */
#endif
