/* ////////////////////////////////////////////////////////////

File Name: ysprogressivetask.h
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

#ifndef YSPROGRESSIVETASK_IS_INCLUDED
#define YSPROGRESSIVETASK_IS_INCLUDED
/* { */

/*!  YsProgressiveTask is a task that can run progressively.  Unlike YsTask class, which is intended for multi-threading, YsProgressiveTask is designed for single-threaded application.
     This class gives a framework for the task such that the program needs to show a progress bar for the user's comfort but it is too risky to run as a background thread.
     The progressive task should be run one step by RunOneStep() function, and Finished() must return YSTRUE when the task is done, or YSFALSE if the task is still running.
 */
class YsProgressiveTask
{
public:
	virtual YSBOOL Finished(void) const=0;
	virtual void RunOneStep(void)=0;

	virtual YSBOOL CanAbort(void) const=0;
	virtual void Abort(void)=0;

	virtual YSBOOL ProvideTaskProgress(void)=0;
	virtual YSBOOL ProvideSubTaskProgress(void)=0;
	virtual YSBOOL ProvideSubSubTaskProgress(void)=0;

	virtual double TaskProgress(void)=0;
	virtual double SubTaskProgress(void)=0;
	virtual double SubSubTaskProgress(void)=0;
}

/* } */
#endif
