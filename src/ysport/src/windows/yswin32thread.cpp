/* ////////////////////////////////////////////////////////////

File Name: yswin32thread.cpp
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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#if _WIN32_WINNT<0x0501
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <windows.h>
#include <process.h>

#undef rad0
#undef rad1


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
// { Windows Thread implementation >>
class YsWin32Mutex : public YsMutex
{
public:
	HANDLE hMutex;

	YsWin32Mutex();
	virtual ~YsWin32Mutex();
	void Initialize(void);

	virtual void Lock(void);
	virtual void Unlock(void);
	virtual YSRESULT LockNoWait(void);
};

YsWin32Mutex::YsWin32Mutex()
{
	hMutex=CreateMutex(NULL,FALSE,NULL);
	Initialize();
}

YsWin32Mutex::~YsWin32Mutex()
{
	if(NULL!=hMutex)
	{
		CloseHandle(hMutex);
	}
}

void YsWin32Mutex::Initialize(void)
{
}

void YsWin32Mutex::Lock(void)
{
	WaitForSingleObject(hMutex,INFINITE);
}

void YsWin32Mutex::Unlock(void)
{
	ReleaseMutex(hMutex);
}

YSRESULT YsWin32Mutex::LockNoWait(void)
{
	unsigned int res=WaitForSingleObject(hMutex,0);
	if(res==WAIT_OBJECT_0)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}








class YsThreadCache
{
public:
	YSBOOL hot;
	YsThreadTask *task;
	HANDLE hThread;
	HANDLE taskWaitMutex,endWaitMutex;
	YSBOOL terminate;

	YsThreadCache();       // In controlling thread
	~YsThreadCache();      // In controlling thread
	void Initialize(void); // In controlling thread

	void RunLoop(void);    // In child thread
};

static DWORD WINAPI YsWin32EnterThreadCacheRunLoop(void *info)
{
	YsThreadCache *cache=(YsThreadCache *)info;
	cache->RunLoop();
	return 0;
}

YsThreadCache::YsThreadCache()       // In controlling thread
{
	taskWaitMutex=CreateMutex(NULL,TRUE,NULL);
	endWaitMutex=CreateMutex(NULL,FALSE,NULL);
	Initialize();
	hThread=CreateThread(NULL,1024*1024,YsWin32EnterThreadCacheRunLoop,this,0,NULL);
}

YsThreadCache::~YsThreadCache()      // In controlling thread
{
	terminate=YSTRUE;
	ReleaseMutex(taskWaitMutex);
	ReleaseMutex(endWaitMutex);

	WaitForSingleObject(hThread,INFINITE);

	CloseHandle(taskWaitMutex);
	CloseHandle(endWaitMutex);
	CloseHandle(hThread);
}

void YsThreadCache::Initialize(void) // In controlling thread
{
	task=NULL;
	hot=YSFALSE;
	terminate=YSFALSE;
}

void YsThreadCache::RunLoop(void)    // In child thread
{
	while(YSTRUE!=terminate)
	{
		WaitForSingleObject(taskWaitMutex,INFINITE);
		hot=YSTRUE;
		if(NULL!=task)
		{
			task->EntryPoint();
			task=NULL;
		}
		ReleaseMutex(taskWaitMutex);

		WaitForSingleObject(endWaitMutex,INFINITE);
		hot=YSFALSE;
		ReleaseMutex(endWaitMutex);
	}
}





YsThreadController::YsThreadController()
{
	// nParallel=GetNumCPU();
	// mutex=CreateSingleMutex();
}

YsThreadController::~YsThreadController()
{
	// DeleteSingleMutex(mutex);
	if(0<threadCache.GetN())
	{
		int i;
		for(i=0; i<threadCache.GetN(); i++)
		{
			delete threadCache[i];
		}
	}
}

YsMutex *YsThreadController::CreateSingleMutex(void)
{
	YsWin32Mutex *mutex=new YsWin32Mutex;
	return mutex;
}

YSRESULT YsThreadController::DeleteSingleMutex(YsMutex *mutex)
{
	delete mutex;
	return YSOK;
}

unsigned int YsThreadController::GetNumCPU(void)
{
	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);  // Why not GetSystenInfo?  I am not 100% sure about what is WOW64 is.
	return sysInfo.dwNumberOfProcessors;
}

void YsThreadController::CacheThread(int nThread)
{
	threadCache.Set(nThread,NULL);

	int i;
	for(i=0; i<nThread; i++)
	{
		threadCache[i]=new YsThreadCache;
	}
}

static DWORD WINAPI YsWin32ThreadLaunch(void *info)
{
	/* YsThreadSession *session=(YsThreadSession *)info;
	session->EntryPoint(); */

	YsThreadTask *task=(YsThreadTask *)info;
	task->EntryPoint();
	return 0;
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

	if(nTask-1<=threadCache.GetN())
	{
		YsArray <HANDLE,16> hMutex;
		int nRunCached=nTask-1;

		while(1)  // This loop makes sure cached threads are free from the previously-assigned tasks.
		{
			int nHot=0;
			for(i=0; i<nRunCached; i++)
			{
				if(YSTRUE==threadCache[i]->hot)
				{
					nHot++;
					break;
				}
			}
			if(0==nHot)
			{
				break;
			}
		}

		hMutex.Set(nRunCached,NULL);
		for(i=0; i<nRunCached; i++)
		{
			hMutex[i]=threadCache[i]->taskWaitMutex;

			threadCache[i]->task=task[i];
			WaitForSingleObject(threadCache[i]->endWaitMutex,INFINITE);
			ReleaseMutex(threadCache[i]->taskWaitMutex);                // Controlling thread releases ownership of taskWaitMutex.
		}

		task[nTask-1]->EntryPoint();

		while(1)  // This loop makes sure the child threads acquire taskWaitMutex.
		{
			int nCold=0;
			for(i=0; i<nRunCached; i++)
			{
				if(YSTRUE!=threadCache[i]->hot)
				{
					nCold++;
					break;
				}
			}
			if(0==nCold)
			{
				break;
			}
		}

		WaitForMultipleObjects(nRunCached,hMutex,TRUE,INFINITE);   // Controlling thread re-acquire ownership of taskWaitMutex.
		for(i=0; i<nRunCached; i++)
		{
			ReleaseMutex(threadCache[i]->endWaitMutex);
		}
	}
	else
	{
		YsArray <HANDLE,16> hThread;
		hThread.Set(nTask-1,0);
		for(i=0; i<nTask-1; i++)
		{
			hThread[i]=CreateThread(NULL,32768,YsWin32ThreadLaunch,task[i],0,NULL);
		}
		task[nTask-1]->EntryPoint();
		WaitForMultipleObjects(nTask-1,hThread,TRUE,INFINITE);
		for(i=0; i<nTask-1; i++)
		{
			CloseHandle(hThread[i]);
		}
	}

	return YSOK;
}
// } Windows Thread implementation <<
#endif

