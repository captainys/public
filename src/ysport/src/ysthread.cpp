/* ////////////////////////////////////////////////////////////

File Name: ysthread.cpp
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

#include <ysclass.h>
#include "ysthread.h"


#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#include <errno.h>
#endif



YsMutex::YsMutex()
{
#ifndef _WIN32
	pthread_mutex_init(&mutex,NULL);
#endif
}

YsMutex::~YsMutex()
{
#ifndef _WIN32
	pthread_mutex_destroy(&mutex);
#endif
}


void YsMutex::Lock(void)
{
#ifdef _WIN32
	mutex.lock();
#else
	pthread_mutex_lock(&mutex);
#endif
}

void YsMutex::Unlock(void)
{
#ifdef _WIN32
	mutex.unlock();
#else
	pthread_mutex_unlock(&mutex);
#endif
}

YSRESULT YsMutex::LockNoWait(void)
{
#ifdef _WIN32
	if(true==mutex.try_lock())
	{
		return YSOK;
	}
#else
	if(0==pthread_mutex_trylock(&mutex))
	{
		return YSOK;
	}
#endif
	return YSERR;
}


////////////////////////////////////////////////////////////

unsigned int YsThreadController::GetNumCPU(void)
{
#ifdef _WIN32
	return std::thread::hardware_concurrency();
#elif defined(__APPLE__)
	unsigned int nCpu;
	size_t nCpuSize=sizeof(nCpu);
	int mib[2]={CTL_HW,HW_NCPU};
	int res=sysctl(mib,2,&nCpu,&nCpuSize,NULL,0);
	if(0==res)
	{
		return nCpu;
	}
	else if(ENOMEM==errno)
	{
		unsigned long int nCpu;
		size_t nCpuSize=sizeof(nCpu); // Try long int then.
		if(0==sysctl(mib,2,&nCpu,&nCpuSize,NULL,0))
		{
			return (unsigned int)nCpu;
		}
	}
	return 1; // This OS didn't tell me.
#else
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

#ifndef _WIN32
static void *YsPthreadLaunch(void *info)
{
	YsTask *task=(YsTask *)info;
	task->StartLocal();
	pthread_exit(NULL);
	return NULL;
}
#endif

YSRESULT YsThreadController::RunParallel(int nTask,YsTask *const task[])
{
	if(0>=nTask)
	{
		return YSOK;
	}
	else if(1==nTask)
	{
		task[0]->StartLocal();
		return YSOK;
	}
	else
	{
#ifdef _WIN32
		// Ironically VS2012 seems to support std::thread library better.
		YsArray <std::thread,16> threadArray;
		threadArray.Resize(nTask-1);
		for(int i=0; i<nTask-1; ++i)
		{
			threadArray[i]=std::thread(YsTask::Start,task[i]);
		}
		task[nTask-1]->StartLocal();
		for(int i=0; i<nTask-1; ++i)
		{
			threadArray[i].join();
		}
#else
		YsArray <pthread_t,16> hThread(nTask-1,NULL);
		for(int i=0; i<nTask-1; i++)
		{
			pthread_create(&hThread[i],NULL,YsPthreadLaunch,(void *)task[i]);
		}
		task[nTask-1]->StartLocal();
		for(int i=0; i<nTask; i++)
		{
			pthread_join(hThread[i],NULL);
		}
#endif
	}

	return YSOK;
}

