/* ////////////////////////////////////////////////////////////

File Name: test.cpp
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

#include <chrono>
#include <vector>

#include <ysclass.h>
#include <ysthreadpool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


void WaitForMilliSecond(int taskIdx,int ms)
{
	printf("Starting %d\n",taskIdx);

	auto start=std::chrono::system_clock::now();
	for(;;)
	{
		auto now=std::chrono::system_clock::now();
		auto passed=std::chrono::duration_cast <std::chrono::milliseconds> (now-start).count();
		if(ms<=passed)
		{
			break;
		}
	}

	printf("Ending %d\n",taskIdx);
}

YSRESULT YsArrayTest(YsThreadPool &pool,int nTask)
{
	// Just check it doesn't crash or go infinity.

	std::vector <std::function <void()> > taskList;

	// 100 ms each
	printf("100ms each\n");
	taskList.clear();
	for(int taskIdx=0; taskIdx<nTask; ++taskIdx)
	{
		taskList.push_back(std::bind(WaitForMilliSecond,taskIdx,100));
	}
	pool.Run(taskList.size(),taskList.data());

	printf("Random duration\n");
	taskList.clear();
	for(int taskIdx=0; taskIdx<nTask; ++taskIdx)
	{
		taskList.push_back(std::bind(WaitForMilliSecond,taskIdx,rand()%100));
	}
	pool.Run(taskList.size(),taskList.data());

	return YSOK;
}

int main(void)
{
	srand((int)time(nullptr));

	YsThreadPool pool8(8);

	int nFail=0;
	if(YSOK!=YsArrayTest(pool8,8))
	{
		++nFail;
	}

	YsThreadPool pool7(7);
	if(YSOK!=YsArrayTest(pool7,8))
	{
		++nFail;
	}

	YsThreadPool pool5(5);
	if(YSOK!=YsArrayTest(pool5,8))
	{
		++nFail;
	}

	printf("%d failed.\n",nFail);
	if(0<nFail)
	{
		return 1;
	}
	return 0;
}
