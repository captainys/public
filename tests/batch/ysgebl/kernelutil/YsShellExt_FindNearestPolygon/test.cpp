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

#include <ysclass.h>
#include <ysshellext.h>
#include <ysshellext_proximityutil.h>

YSRESULT Test(void)
{
	YSRESULT res=YSOK;

	YsShellExt shl;
	YsVec3 min(-1,-1,-1),max(1,1,1);

	shl.CleanUp();

	const YsShell::VertexHandle cubeVtHd[8]=
	{
		shl.AddVertex(min),
		shl.AddVertex(YsVec3(min.x(),max.y(),min.z())),
		shl.AddVertex(YsVec3(max.x(),max.y(),min.z())),
		shl.AddVertex(YsVec3(max.x(),min.y(),min.z())),

		shl.AddVertex(YsVec3(min.x(),min.y(),max.z())),
		shl.AddVertex(YsVec3(min.x(),max.y(),max.z())),
		shl.AddVertex(max),
		shl.AddVertex(YsVec3(max.x(),min.y(),max.z())),
	};

	const YsShell::VertexHandle cubePlHd[6][4]=
	{
		{cubeVtHd[0],cubeVtHd[1],cubeVtHd[2],cubeVtHd[3]},

		{cubeVtHd[1],cubeVtHd[0],cubeVtHd[4],cubeVtHd[5]},
		{cubeVtHd[2],cubeVtHd[1],cubeVtHd[5],cubeVtHd[6]},
		{cubeVtHd[3],cubeVtHd[2],cubeVtHd[6],cubeVtHd[7]},
		{cubeVtHd[0],cubeVtHd[3],cubeVtHd[7],cubeVtHd[4]},

		{cubeVtHd[7],cubeVtHd[6],cubeVtHd[5],cubeVtHd[4]},
	};

	const YsVec3 cubePlNom[6]=
	{
		YsVec3( 0.0, 0.0,-1.0),

		YsVec3(-1.0, 0.0, 0.0),
		YsVec3( 0.0, 1.0, 0.0),
		YsVec3( 1.0, 0.0, 0.0),
		YsVec3( 0.0,-1.0, 0.0),

		YsVec3( 0.0, 0.0, 1.0),
	};
	for(YSSIZE_T idx=0; idx<6; ++idx)
	{
		auto plHd=shl.AddPolygon(4,cubePlHd[idx]);
		shl.SetPolygonNormal(plHd,cubePlNom[idx]);
	}



	int A=0,B=0,C=0,D=0,E=0,F=0;

	for(int a=0; a<360; ++a)
	{
		double radian=(double)a*YsPi/180.0;
		double x=5.0*cos(radian);
		double z=5.0*sin(radian);

		double nearDist;
		YsVec3 nearPos;
		YsShellExt::PassAll cond;
		YsShell::PolygonHandle nearPlHd=YsShellExt_FindNearestPolygon(nearDist,nearPos,shl,YsVec3(x,0.0,z),cond);

		if(nullptr==nearPlHd)
		{
			fprintf(stderr,"Cannot find the nearest point.\n");
			return YSERR;
		}

		if(-1.0<=x && x<=1.0)
		{
			++A;
			YsVec3 mustBeNearPos(x,0,0);
			if(z<0.0)
			{
				mustBeNearPos.SetZ(-1.0);
			}
			else
			{
				mustBeNearPos.SetZ(1.0);
			}
			if(mustBeNearPos!=nearPos)
			{
				fprintf(stderr,"Failed to calculate the nearest position.\n");
				fprintf(stderr,"Supposed to be %s\n",mustBeNearPos.Txt());
				fprintf(stderr,"Calculated %s\n",nearPos.Txt());
				return YSERR;
			}
		}
		else if(-1.0<=z && z<=1.0)
		{
			++B;
			YsVec3 mustBeNearPos(0,0,z);
			if(x<0.0)
			{
				mustBeNearPos.SetX(-1.0);
			}
			else
			{
				mustBeNearPos.SetX(1.0);
			}
			if(mustBeNearPos!=nearPos)
			{
				fprintf(stderr,"Failed to calculate the nearest position.\n");
				fprintf(stderr,"Supposed to be %s\n",mustBeNearPos.Txt());
				fprintf(stderr,"Calculated %s\n",nearPos.Txt());
				return YSERR;
			}
		}
		else
		{
			++C;
			YsVec3 mustBeNearPos=YsVec3::Origin();
			if(x<0.0)
			{
				mustBeNearPos.SetX(-1.0);
			}
			else
			{
				mustBeNearPos.SetX( 1.0);
			}
			if(z<0.0)
			{
				mustBeNearPos.SetZ(-1.0);
			}
			else
			{
				mustBeNearPos.SetZ( 1.0);
			}
			if(mustBeNearPos!=nearPos)
			{
				fprintf(stderr,"Failed to calculate the nearest position.\n");
				fprintf(stderr,"Supposed to be %s\n",mustBeNearPos.Txt());
				fprintf(stderr,"Calculated %s\n",nearPos.Txt());
				return YSERR;
			}
		}
	}


	for(int a=0; a<360; ++a)
	{
		double radian=(double)a*YsPi/180.0;
		double x=5.0*cos(radian);
		double y=5.0*sin(radian);

		double nearDist;
		YsVec3 nearPos;
		YsShellExt::PassAll cond;
		YsShell::PolygonHandle nearPlHd=YsShellExt_FindNearestPolygon(nearDist,nearPos,shl,YsVec3(x,y,0.0),cond);

		if(nullptr==nearPlHd)
		{
			fprintf(stderr,"Cannot find the nearest point.\n");
			return YSERR;
		}

		if(-1.0<=x && x<=1.0)
		{
			++D;
			YsVec3 mustBeNearPos(x,0,0);
			if(y<0.0)
			{
				mustBeNearPos.SetY(-1.0);
			}
			else
			{
				mustBeNearPos.SetY(1.0);
			}
			if(mustBeNearPos!=nearPos)
			{
				fprintf(stderr,"Failed to calculate the nearest position.\n");
				fprintf(stderr,"Supposed to be %s\n",mustBeNearPos.Txt());
				fprintf(stderr,"Calculated %s\n",nearPos.Txt());
				return YSERR;
			}
		}
		else if(-1.0<=y && y<=1.0)
		{
			++E;
			YsVec3 mustBeNearPos(0,y,0);
			if(x<0.0)
			{
				mustBeNearPos.SetX(-1.0);
			}
			else
			{
				mustBeNearPos.SetX(1.0);
			}
			if(mustBeNearPos!=nearPos)
			{
				fprintf(stderr,"Failed to calculate the nearest position.\n");
				fprintf(stderr,"Supposed to be %s\n",mustBeNearPos.Txt());
				fprintf(stderr,"Calculated %s\n",nearPos.Txt());
				return YSERR;
			}
		}
		else
		{
			++F;
			YsVec3 mustBeNearPos=YsVec3::Origin();
			if(x<0.0)
			{
				mustBeNearPos.SetX(-1.0);
			}
			else
			{
				mustBeNearPos.SetX( 1.0);
			}
			if(y<0.0)
			{
				mustBeNearPos.SetY(-1.0);
			}
			else
			{
				mustBeNearPos.SetY( 1.0);
			}
			if(mustBeNearPos!=nearPos)
			{
				fprintf(stderr,"Failed to calculate the nearest position.\n");
				fprintf(stderr,"Supposed to be %s\n",mustBeNearPos.Txt());
				fprintf(stderr,"Calculated %s\n",nearPos.Txt());
				return YSERR;
			}
		}
	}

	printf("%d %d %d %d %d %d\n",A,B,C,D,E,F);

	if(0==A || 0==B || 0==C || 0==D || 0==E || 0==F)
	{
		fprintf(stderr,"Not all cases have been covered.\n");
		return YSERR;
	}



	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

int main(void)
{
	int nFail=0;
	if(YSOK!=Test())
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
