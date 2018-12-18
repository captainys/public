/* ////////////////////////////////////////////////////////////

File Name: ysshellstl.cpp
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

#if (defined(_WIN32) || defined(_WIN64)) && !defined(_CRT_SECURE_NO_WARNINGS)
	// This disables annoying warnings VC++ gives for use of C standard library.  Shut the mouth up.
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "ysclass.h"




static float YsGetFloat(const unsigned char dat[])
{
	int a;
	char *intelOrMotorola;
	a=0x7f;
	intelOrMotorola=(char *)(&a);
	if(intelOrMotorola[0]==0x7f)
	{
		// For Intel chip
		return *((float *)dat);
	}
	else
	{
		// For Motorola chip
		unsigned char conv[4];
		conv[0]=dat[3];
		conv[1]=dat[2];
		conv[2]=dat[1];
		conv[3]=dat[0];
		return *((float *)conv);
	}
}

YSRESULT YsShell::LoadStl(const char fn[])
{
	CleanUp();
	return MergeStl(fn);
}

YSRESULT YsShell::MergeStl(const char fn[])
{
	FILE *fp;
	fp=fopen(fn,"rb");
	if(fp!=NULL)
	{
		char buf[1024];
		int i;
		Ys3dTree <YsShellVertexHandle> vtKdTree;

		YSSIZE_T headerReadSize=fread(buf,1,1024,fp);
		fclose(fp);

		for(i=0; i<headerReadSize; i++)
		{
			if('A'<=buf[i] && buf[i]<='Z')
			{
				buf[i]=buf[i]+('a'-'A');
			}
		}

		// Is this a binary or ascii?
		YSBOOL solid,facet,loop,vertex;
		solid=YSFALSE;
		facet=YSFALSE;
		loop=YSFALSE;
		vertex=YSFALSE;
		for(i=0; i<1018; i++)
		{
			if(strncmp(buf+i,"solid",5)==0)
			{
				solid=YSTRUE;
			}
			else if(strncmp(buf+i,"facet",5)==0)
			{
				facet=YSTRUE;
			}
			else if(strncmp(buf+i,"loop",4)==0)
			{
				loop=YSTRUE;
			}
			else if(strncmp(buf+i,"vertex",6)==0)
			{
				vertex=YSTRUE;
			}
		}

		if(solid==YSTRUE && facet==YSTRUE && loop==YSTRUE && vertex==YSTRUE)  // It's an ascii STL
		{
			int ac;
			char *av[256];
			int state;   // 0:Outside     1:insie "outer"->"endloop"
			YsVec3 dv,nom;
			YsArray <YsVec3> plVtPos;
			YsArray <YsShellVertexHandle> plVtHd,findVtHd;
			YsLoopCounter ctr;

			YsPrintf("It looks like an ASCII stl.\n");

			fp=fopen(fn,"r");

			int lng,nxt,cur;
			fseek(fp,0,SEEK_END);
			lng=ftell(fp);
			fseek(fp,0,SEEK_SET);

			state=0;
			dv.Set(YsTolerance,YsTolerance,YsTolerance);

			ctr.BeginCounter(lng);

			nxt=0;
			nom=YsOrigin();
			while(fgets(buf,256,fp)!=NULL)
			{
				cur=ftell(fp);
				ctr.ShowCounter(cur);

				YsUncapitalize(buf);
				if(YsArguments(&ac,av,256,buf)==YSOK)
				{
					if(state==0)
					{
						if(ac>=5 && strcmp(av[0],"facet")==0 && strcmp(av[1],"normal")==0)
						{
							double x,y,z;
							x=atof(av[2]);
							y=atof(av[3]);
							z=atof(av[4]);
							nom.Set(x,y,z);
						}
						else if(ac>=2 && strcmp(av[0],"outer")==0 && strcmp(av[1],"loop")==0)
						{
							state=1;
							plVtPos.Set(0,NULL);
						}
					}
					else if(state==1)
					{
						if(ac>=4 && strcmp(av[0],"vertex")==0)
						{
							YsVec3 v;
							v.Set(atof(av[1]),atof(av[2]),atof(av[3]));
							plVtPos.Append(v);
						}
						else if(ac>=1 && strcmp(av[0],"endloop")==0)
						{
							if(plVtPos.GetN()!=3)
							{
								YsPrintf("Warning! Non-triangular polygon exists.\n");
							}

							plVtHd.Set(0,NULL);
							int i;
							for(i=0; i<plVtPos.GetN(); i++)
							{
								if(vtKdTree.MakeItemList(findVtHd,plVtPos[i]-dv,plVtPos[i]+dv)==YSOK &&
								   findVtHd.GetN()>0)
								{
									plVtHd.Append(findVtHd[0]);
								}
								else
								{
									YsShellVertexHandle newVtHd;
									newVtHd=AddVertexH(plVtPos[i]);
									plVtHd.Append(newVtHd);
									vtKdTree.AddPoint(plVtPos[i],newVtHd);
								}
							}

							YsShellPolygonHandle plHd;
							plHd=AddPolygonH(plVtHd.GetN(),plVtHd);
							SetColorOfPolygon(plHd,YsWhite());
							SetNormalOfPolygon(plHd,nom);

							plVtPos.Set(0,NULL);
							plVtHd.Set(0,NULL);
							nom=YsOrigin();
							state=0;
						}
					}
				}
			}

			fclose(fp);
		}
		else if(facet!=YSTRUE && loop!=YSTRUE && vertex!=YSTRUE)  // It's a binary STL
		{
			unsigned char buf[256];
			unsigned char conv[4];
			YsVec3 nom,tri[3];
			YsVec3 dv;
			YsShellVertexHandle plVtHd[3];
			int nxt,cur,nPlg;
			YsLoopCounter ctr;
			YsArray <YsShellVertexHandle> findVtHd;

			nPlg=0;
			dv.Set(YsTolerance,YsTolerance,YsTolerance);

			YsPrintf("It looks like a binary stl.\n");

			fp=fopen(fn,"rb");
			fread(buf,1,80,fp);
			fread(conv,4,1,fp);

			// nPlg=conv[3]+conv[2]*0x100+conv[1]*0x10000+conv[0]*0x1000000;
			nPlg=conv[0]+conv[1]*0x100+conv[2]*0x10000+conv[3]*0x1000000;
			YsPrintf("%d Polygons\n",nPlg);

			ctr.BeginCounter(nPlg);

			cur=0;
			nxt=0;
			while(fread(buf,1,50,fp)==50)
			{
				ctr.ShowCounter(cur);
				cur++;

				nom.Set   (YsGetFloat(buf   ),YsGetFloat(buf+ 4),YsGetFloat(buf+ 8));
				tri[0].Set(YsGetFloat(buf+12),YsGetFloat(buf+16),YsGetFloat(buf+20));
				tri[1].Set(YsGetFloat(buf+24),YsGetFloat(buf+28),YsGetFloat(buf+32));
				tri[2].Set(YsGetFloat(buf+36),YsGetFloat(buf+40),YsGetFloat(buf+44));

				for(i=0; i<3; i++)
				{
					if(vtKdTree.MakeItemList(findVtHd,tri[i]-dv,tri[i]+dv)==YSOK &&
					   findVtHd.GetN()>0)
					{
						plVtHd[i]=findVtHd[0];
					}
					else
					{
						YsShellVertexHandle newVtHd;
						newVtHd=AddVertexH(tri[i]);
						plVtHd[i]=newVtHd;
						vtKdTree.AddPoint(tri[i],newVtHd);
					}
				}

				YsShellPolygonHandle plHd;
				plHd=AddPolygonH(3,plVtHd);
				SetColorOfPolygon(plHd,YsWhite());
				SetNormalOfPolygon(plHd,nom);
			}
			ctr.EndCounter();

			YsPrintf("nPlg in the file %d\n",nPlg);
			YsPrintf("Actual           %d\n",cur);

			fclose(fp);
		}
		else
		{
			YsPrintf("Cannot identify the STL file (binary or ascii).\n");
			return YSERR;
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SaveStl(int &nIgnored,const char fn[],const char title[]) const
{
	FILE *fp;
	fp=fopen(fn,"w");
	if(NULL!=fp)
	{
		YSRESULT res=SaveStl(nIgnored,fp,title);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT YsShell::SaveStl(int &nIgnored,FILE *fp,const char title[]) const
{
	nIgnored=0;

	if(title==NULL)
	{
		fprintf(fp,"solid STL created by YSCLASS\n");
	}
	else
	{
		fprintf(fp,"solid \"%s\"\n",title);
	}

	YsShellPolygonHandle plHd;
	YsArray <YsVec3> plVtPos;
	YsVec3 nom;
	int i;

	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		GetVertexListOfPolygon(plVtPos,plHd);
		GetNormalOfPolygon(nom,plHd);
		if(nom.GetSquareLength()<=YsSqr(YsTolerance))
		{
			nom=(plVtPos[1]-plVtPos[0])^(plVtPos[2]-plVtPos[0]);
			nom.Normalize();
		}

		if(GetNumVertexOfPolygon(plHd)==3)
		{
			fprintf(fp,"facet normal %lf %lf %lf\n",nom.x(),nom.y(),nom.z());
			fprintf(fp,"outer loop\n");
			for(i=0; i<3; i++)
			{
				fprintf(fp,"vertex %lf %lf %lf\n",plVtPos[i].x(),plVtPos[i].y(),plVtPos[i].z());
			}
			fprintf(fp,"endloop\n");
			fprintf(fp,"endfacet\n");
		}
		else
		{
			YsSword sword;
			sword.SetInitialPolygon(plVtPos.GetN(),plVtPos,NULL);
			if(YSOK==sword.Triangulate())
			{
				for(int triIdx=0; triIdx<sword.GetNumPolygon(); ++triIdx)
				{
					auto tri=sword.GetPolygon(triIdx);
					if(NULL!=tri && 3==tri->GetN())
					{
						fprintf(fp,"facet normal %lf %lf %lf\n",nom.x(),nom.y(),nom.z());
						fprintf(fp,"outer loop\n");
						for(i=0; i<3; i++)
						{
							fprintf(fp,"vertex %lf %lf %lf\n",(*tri)[i].x(),(*tri)[i].y(),(*tri)[i].z());
						}
						fprintf(fp,"endloop\n");
						fprintf(fp,"endfacet\n");
					}
				}
			}
			else
			{
				nIgnored++;
			}
		}
	}
	fprintf(fp,"end solid\n");

	if(nIgnored>0)
	{
		YsPrintf("%d polygons were not written in STL because they are not triangular.\n",nIgnored);
	}
	return YSOK;
}

YSRESULT YsShell::SaveStl(const char fn[],const char title[]) const
{
	int n;
	return SaveStl(n,fn,title);
}

YSRESULT YsShell::SaveStl(FILE *fp,const char title[]) const
{
	int n;
	return SaveStl(n,fp,title);
}




static const int LITTLEENDIAN=0;   // eg. Intel CPU
static const int BIGENDIAN=1;      // eg. Motorola CPU

static int ByteOrder(void)
{
	int a=1;
	unsigned char *tst;
	tst=(unsigned char *)&a;
	if(tst[0]==1)
	{
		return LITTLEENDIAN;
	}
	else
	{
		return BIGENDIAN;
	}
}

static void WriteBinStlTitle(FILE *fp,const char title[])
{
	int i;
	char buf[256];
	sprintf(buf,"binstl%s",title);
	for(i=(int)strlen(buf); i<80; i++)
	{
		buf[i]=' ';
	}
	fwrite(buf,1,80,fp);
}

static void WriteBinStlUnsignedInt(FILE *fp,unsigned int dat)
{
	unsigned char buf[4];
	buf[0]=(unsigned char)( dat     &0xff);
	buf[1]=(unsigned char)((dat>> 8)&0xff);
	buf[2]=(unsigned char)((dat>>16)&0xff);
	buf[3]=(unsigned char)((dat>>24)&0xff);
	fwrite(buf,1,4,fp);
}

static void WriteBinStlFloat(FILE *fp,float dat,int byteOrder)
{
	// This function assumes a float takes four bytes.
	unsigned char buf[4],*ptr;
	ptr=(unsigned char *)(&dat);
	if(byteOrder==LITTLEENDIAN)
	{
		fwrite(ptr,1,4,fp);
	}
	else
	{
		buf[0]=ptr[3];
		buf[1]=ptr[2];
		buf[2]=ptr[1];
		buf[3]=ptr[0];
		fwrite(buf,1,4,fp);
	}
}

static void WriteBinStlTriangle(FILE *fp,const double nom[],const double v1[],const double v2[], const double v3[])
{
	int byteOrder;
	byteOrder=ByteOrder();

	WriteBinStlFloat(fp,(float)nom[0],byteOrder);
	WriteBinStlFloat(fp,(float)nom[1],byteOrder);
	WriteBinStlFloat(fp,(float)nom[2],byteOrder);

	WriteBinStlFloat(fp,(float)v1[0],byteOrder);
	WriteBinStlFloat(fp,(float)v1[1],byteOrder);
	WriteBinStlFloat(fp,(float)v1[2],byteOrder);
	WriteBinStlFloat(fp,(float)v2[0],byteOrder);
	WriteBinStlFloat(fp,(float)v2[1],byteOrder);
	WriteBinStlFloat(fp,(float)v2[2],byteOrder);
	WriteBinStlFloat(fp,(float)v3[0],byteOrder);
	WriteBinStlFloat(fp,(float)v3[1],byteOrder);
	WriteBinStlFloat(fp,(float)v3[2],byteOrder);

	char padding[2]={0,0};
	fwrite(padding,1,2,fp);
}



YSRESULT YsShell::SaveBinStl(int &nIgnored,const char fn[],const char title[]) const
{
	FILE *fp;
	fp=fopen(fn,"wb");
	if(fp!=NULL)
	{
		YSRESULT res=SaveBinStl(nIgnored,fp,title);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT YsShell::SaveBinStl(int &nIgnored,FILE *fp,const char title[]) const
{
	nIgnored=0;

	YsShellPolygonHandle plHd;
	YsArray <YsVec3> plVtPos;
	YsVec3 nom;
	unsigned nTri;
	YsArray <YsShellPolygonHandle> dividedTriPlHd; // dividedTriPlHd[i] -> dividedTriVtHd[i*3],[i*3+1],[i*3+2]
	YsArray <YsShellVertexHandle> dividedTriVtHd;


	nTri=0;
	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		if(GetNumVertexOfPolygon(plHd)==3)
		{
			nTri++;
		}
		else
		{
			auto plVtHd=GetPolygonVertex(plHd);
			auto nom=GetNormal(plHd);

			YsArray <YsVec3,4> plVtPos;
			YsArray <int,4> plVtIdx;
			for(int i=0; i<plVtHd.GetN(); ++i)
			{
				plVtPos.Append(GetVertexPosition(plVtHd[i]));
				plVtIdx.Append(i);
			}

			YsSword sword;
			sword.SetInitialPolygon(plVtPos.GetN(),plVtPos,plVtIdx);
			if(YSOK==sword.Triangulate())
			{
				for(int triIdx=0; triIdx<sword.GetNumPolygon(); ++triIdx)
				{
					auto tri=sword.GetVertexIdList(triIdx);
					if(NULL!=tri && 3==tri->GetN())
					{
						++nTri;
						dividedTriPlHd.Append(plHd);
						dividedTriVtHd.Append(plVtHd[(*tri)[0]]);
						dividedTriVtHd.Append(plVtHd[(*tri)[1]]);
						dividedTriVtHd.Append(plVtHd[(*tri)[2]]);
					}
				}
			}
			else
			{
				nIgnored++;
			}
		}
	}

	if(title==NULL)
	{
		WriteBinStlTitle(fp,"STL Created by YSCLASS");
	}
	else
	{
		WriteBinStlTitle(fp,title);
	}
	WriteBinStlUnsignedInt(fp,nTri);


	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		GetVertexListOfPolygon(plVtPos,plHd);
		GetNormalOfPolygon(nom,plHd);
		if(nom.GetSquareLength()<=YsSqr(YsTolerance))
		{
			nom=(plVtPos[1]-plVtPos[0])^(plVtPos[2]-plVtPos[0]);
			nom.Normalize();
		}

		if(GetNumVertexOfPolygon(plHd)==3)
		{
			WriteBinStlTriangle(fp,nom,plVtPos[0],plVtPos[1],plVtPos[2]);
		}
	}

	for(YSSIZE_T idx=0; idx<dividedTriPlHd.GetN(); ++idx)
	{
		auto plHd=dividedTriPlHd[idx];
		YsVec3 tri[3]=
		{
			GetVertexPosition(dividedTriVtHd[idx*3]),
			GetVertexPosition(dividedTriVtHd[idx*3+1]),
			GetVertexPosition(dividedTriVtHd[idx*3+2])
		};

		auto nom=GetNormal(plHd);
		if(nom.GetSquareLength()<=YsSqr(YsTolerance))
		{
			nom=YsUnitVector((tri[1]-tri[0])^(tri[2]-tri[0]));
		}
		WriteBinStlTriangle(fp,nom,tri[0],tri[1],tri[2]);
	}

	return YSOK;
}



YSRESULT YsShell::SaveBinStl(const char fn[],const char title[]) const
{
	int n;
	return SaveBinStl(n,fn,title);
}

YSRESULT YsShell::SaveBinStl(FILE *fp,const char title[]) const
{
	int n;
	return SaveBinStl(n,fp,title);
}

