/* ////////////////////////////////////////////////////////////

File Name: ystask.h
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

#ifndef YSTASK_IS_INCLUDED
#define YSTASK_IS_INCLUDED
/* { */

#include "ysdef.h"
#include "ysarray.h"

/*! YsTask class is a base class for creating a task.  
    A sub-class of this class must implement Run(void) function, which will be called from Start(void *contextPtr) function. */
class YsTask
{
protected:
	/*! Actual task.  A sub-class must implement this function.  */
	virtual void Run(void)=0;
public:
	enum TASKSTATE
	{
		STATE_IDLE,
		STATE_RUNNING,
		STATE_FINISHED
	};

private:
	TASKSTATE state;

public:
	/*! Default constructor. */
	YsTask();

	/*! This function changes the state from STATE_FINISHED to STATE_IDLE.
        It cannot change the state if the thread state is still STATE_RUNNING. 
        It will return YSOK if it succeeds, or will fail and return YSERR if the thread state is STATE_RUNNING. */
	YSRESULT SetIdle(void);

	/*! Returns the state of the task. */
	TASKSTATE GetState(void) const;

	/*! This is an entry-point function that can be called from a thread library or any external library.
	    The first parameter, contextPtr, must be a pointer to a sub-class of YsTask. 
		This function will not start a task unless the task state is STATE_IDLE.

	    Example:

			#include <thread>

			class YsActualTask : public YsTask;

			YsActualTask task;
			std::thread t(YsTask::Start,&task);
			t.detach();

			while(YSOK!=task.SetIdle())
			{
				// Wait until the task is done.
			}
	    */
	static void Start(void *contextPtr);

	/*! This function runs the task in the same thread.
	    This function can be useful to run the task in a single-threaded environment.
	    Or, it can also be used for running a task in the main thread while other tasks are running in the sub-threads. 
	    This function can run the task only when the state is STATE_IDLE, in which case the function will return YSOK.
	    Otherwise, it will return YSERR. */
	YSRESULT StartLocal(void);


	/*
		RunParallel, RunParallelDirect function had to be moved out to YSCLASS11 library in order to make YSCLASS library
		free of C++11 libraries.  XCode 4.x and later supports C++11 features.  However, as soon as I include C++11 libraries,
		it loses support for Mac OSX 10.6.
	*/
};

/* } */
#endif
