/* ////////////////////////////////////////////////////////////

File Name: ysunixthread.cpp
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
#include "../ysthread.h"


#include <pthread.h>
#include <unistd.h>

#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#include <errno.h>
#endif





#ifdef _OPENMP
// { OpenMP implementation >>

#include <omp.h>

class YsOpenMPMutex : public YsMutex
{
public:
	omp_lock_t lock;

	YsOpenMPMutex();
	virtual ~YsOpenMPMutex();
	void Initialize(void);

	virtual void Lock(void);
	virtual void Unlock(void);
	virtual YSRESULT LockNoWait(void);
};

YsOpenMPMutex::YsOpenMPMutex()
{
	omp_init_lock(&lock);
	Initialize();
}

YsOpenMPMutex::~YsOpenMPMutex()
{
	omp_destroy_lock(&lock);
}

void YsOpenMPMutex::Initialize(void)
{
}

void YsOpenMPMutex::Lock(void)
{
	omp_set_lock(&lock);
}

void YsOpenMPMutex::Unlock(void)
{
	omp_unset_lock(&lock);
}

YSRESULT YsOpenMPMutex::LockNoWait(void)
{
	if(true==omp_test_lock(&lock))
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


YsThreadController::YsThreadController()
{
}

YsThreadController::~YsThreadController()
{
}

YsMutex *YsThreadController::CreateSingleMutex(void)
{
	YsOpenMPMutex *mutex=new YsOpenMPMutex;
	return mutex;
}

YSRESULT YsThreadController::DeleteSingleMutex(YsMutex *mutex)
{
	delete mutex;
	return YSOK;
}

unsigned int YsThreadController::GetNumCPU(void)
{
	return omp_get_num_procs();
}

void YsThreadController::CacheThread(int)
{
}

YSRESULT YsThreadController::RunParallel(int nTask,YsThreadTask *task[])
{
	if(0>=nTask)
	{
		return YSOK;
	}

	int i;
	for(i=0; i<nTask; i++)
	{
		task[i]->taskId=i;
		task[i]->sta=YSTHREADTASKSTATE_IDLE;
	}

	if(1==nTask)
	{
		task[0]->EntryPoint();
		return YSOK;
	}
	else
	{
		#pragma omp parallel for
		for(i=0; i<nTask; i++)
		{
			task[i]->EntryPoint();
		}
	}

	return YSOK;
}



// } OpenMP implementation <<
#else
// { Pthread implementation >>

class YsPthreadMutex : public YsMutex
{
public:
	pthread_mutex_t mutex;

	YsPthreadMutex();
	virtual ~YsPthreadMutex();
	void Initialize(void);

	virtual void Lock(void);
	virtual void Unlock(void);
	virtual YSRESULT LockNoWait(void);
};

YsPthreadMutex::YsPthreadMutex()
{
	pthread_mutex_init(&mutex,NULL);
	Initialize();
}

YsPthreadMutex::~YsPthreadMutex()
{
	pthread_mutex_destroy(&mutex);
}

void YsPthreadMutex::Initialize(void)
{
}

void YsPthreadMutex::Lock(void)
{
	pthread_mutex_lock(&mutex);
}

void YsPthreadMutex::Unlock(void)
{
	pthread_mutex_unlock(&mutex);
}

YSRESULT YsPthreadMutex::LockNoWait(void)
{
	int res=pthread_mutex_trylock(&mutex);
	if(res==0)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}





YsThreadController::YsThreadController()
{
}

YsThreadController::~YsThreadController()
{
}

YsMutex *YsThreadController::CreateSingleMutex(void)
{
	YsPthreadMutex *mutex=new YsPthreadMutex;
	return mutex;
}

YSRESULT YsThreadController::DeleteSingleMutex(YsMutex *mutex)
{
	delete mutex;
	return YSOK;
}

unsigned int YsThreadController::GetNumCPU(void)
{
#ifdef __APPLE__
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

void YsThreadController::CacheThread(int)
{
}

static void *YsPthreadLaunch(void *info)
{
	YsThreadTask *task=(YsThreadTask *)info;
	task->EntryPoint();
	pthread_exit(NULL);
	return NULL;
}

YSRESULT YsThreadController::RunParallel(int nTask,YsThreadTask *task[])
{
	if(0>=nTask)
	{
		return YSOK;
	}

	int i;
	for(i=0; i<nTask; i++)
	{
		task[i]->taskId=i;
		task[i]->sta=YSTHREADTASKSTATE_IDLE;
	}

	if(1==nTask)
	{
		task[0]->EntryPoint();
		return YSOK;
	}

	YsArray <pthread_t,16> hThread;
	hThread.Set(nTask,0);
	for(i=0; i<nTask; i++)
	{
		pthread_create(&hThread[i],NULL,YsPthreadLaunch,(void *)task[i]);
	}
	for(i=0; i<nTask; i++)
	{
		pthread_join(hThread[i],NULL);
	}

	// Do I have to clean threads?  Does pthread_destory exist?

	return YSOK;
}

// } Pthread implementation <<
#endif
