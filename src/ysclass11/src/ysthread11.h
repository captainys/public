/* ////////////////////////////////////////////////////////////

File Name: ysthread11.h
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

#ifndef YSTHREAD___IS_INCLUDED
#define YSTHREAD___IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ystask.h>
#include <thread>



/*! This function runs multiple tasks in parallel. */
void YsRunTaskParallel(YSSIZE_T nTask,YsTask *const task[]);



/*! This function runs multiple tasks in parallel. */
template <class T,const int N>
void YsRunTaskParallelDirect(YsArray <T,N> &taskArray)
{
	if(1==taskArray.GetN())
	{
		taskArray[0].StartLocal();
	}
	else if(0<taskArray.GetN())
	{
		const auto nTask=taskArray.GetN();
		YsArray <std::thread,64> threadArray;
		threadArray.Resize(nTask-1);
		for(int idx=0; idx<nTask-1; ++idx)
		{
			std::thread t(YsTask::Start,&taskArray[idx]);
			threadArray[idx]=(std::thread &&)t;
		}
		taskArray[nTask-1].StartLocal();
		for(int idx=0; idx<nTask-1; ++idx)
		{
			threadArray[idx].join();
		}
	}
}



/*! This function runs multiple tasks in parallel.
    T needs to be a class derived from YsTask.  
    Tasks in 'taskArray' may be input and output.  Therefore it shouldn't made const. */
template <const int N>
void YsRunTaskParallel(const YsArray <YsTask *,N> &taskArray)
{
	YsRunTaskParallel(taskArray.GetN(),taskArray);
}



/* } */
#endif
