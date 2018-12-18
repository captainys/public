/* ////////////////////////////////////////////////////////////

File Name: main.cpp
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

#include <ysshellextedit.h>
#include <ysshellextio.h>
#include <geblworkorder/geblworkorder.h>
#include <geblcmdparam/cmdparam.h>
#include <geblfileio/fileio.h>


int main(int ac,char *av[])
{
	GeblCmd_CommandParameterInfo cpi;
	if(YSOK==cpi.RecognizeCommandParameter(ac,av))
	{
		YsShellDnmContainer <YsShellExtEdit> shlGrp;
		YsShellDnmContainer <YsShellExtEdit>::Node *slHd=shlGrp.CreateShell(NULL);

		if(0==cpi.InputFileName().Strlen() || YSOK==GeblCmd_FileIo_ReadFile(*slHd,cpi.InputFileName()))
		{
			GeblCmd_WorkOrder work;

			work.SetCurrentShellGroup(shlGrp);
			work.SetCurrentShell(slHd);

			auto workOrderArray=cpi.WorkOrderArray();
			for(auto workOrder : workOrderArray)
			{
				if(YSOK!=work.RunWorkOrder(workOrder))
				{
					return 1;
				}
			}

			if(0<cpi.OutputFileNameArray().GetN() && YSOK!=GeblCmd_FileIo_SaveFile(cpi,*slHd))
			{
				return 1;
			}
		}
		else
		{
			fprintf(stderr,"Cannot read the input file!\n");
			return 1;
		}

		return 0;
	}
	else
	{
		return 1;
	}
}
