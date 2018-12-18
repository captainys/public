/* ////////////////////////////////////////////////////////////

File Name: ysshelllattice_singleThread.cpp
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

#include "ysclass.h"
#include "ysshell.h"



void YsShellLatticeElem::Lock(void)
{
}

void YsShellLatticeElem::Unlock(void)
{
}

////////////////////////////////////////////////////////////

void YsShellLattice::InitLock(void)
{
}

void YsShellLattice::DestroyLock(void)
{
}

void YsShellLattice::LockPlgToCellTable(void)
{
}

void YsShellLattice::UnlockPlgToCellTable(void)
{
}

void YsShellLattice::AssignElementLock(void)
{
}

void YsShellLattice::DestroyElementLock(void)
{
}

////////////////////////////////////////////////////////////

YSRESULT YsShellLattice::SetDomain_Polygon(const class YsShell &,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[])
{
	int i;
	YsShellPolygonHandle plHd;
	YsLoopCounter ctr;
	YSRESULT res;

	plHd=NULL;
	res=YSOK;

	ctr.BeginCounter(nPl);
	for(i=0; i<nPl; i++)
	{
		ctr.Increment();
		if(YSOK==res)
		{
			if(AddPolygon(plHdList[i])!=YSOK)
			{
				YsErrOut("YsShellLattice::SetDomain()\n");
				YsErrOut("  Cannot add polygon\n");
				res=YSERR;
			}
		}
	}
	ctr.EndCounter();

	return res;
}
