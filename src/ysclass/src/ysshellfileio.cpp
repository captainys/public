/* ////////////////////////////////////////////////////////////

File Name: ysshellfileio.cpp
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
#include "ysclass.h"

YSRESULT YsShell::LoadSrf(
    const char fn[],YsArray <YsShellPolygonHandle> *noShadingPolygon,YsArray <YsShellVertexHandle> *roundVtx)
{
	FILE *fp;
	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		YSRESULT res=LoadSrf(fp,noShadingPolygon,roundVtx);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT YsShell::LoadSrf(
    FILE *fp,YsArray <YsShellPolygonHandle> *noShadingPolygon,YsArray <YsShellVertexHandle> *roundVtx)
{
	if(NULL!=fp)
	{
		YsArray <YsString,16> args;

		if(noShadingPolygon!=NULL)
		{
			noShadingPolygon->Set(0,NULL);
		}
		if(roundVtx!=NULL)
		{
			roundVtx->Set(0,NULL);
		}
		if(BeginReadSrf()==YSOK)
		{
			YsString str;
			while(str.Fgets(fp)!=NULL)
			{
				if(ReadSrfOneLine(str,noShadingPolygon,roundVtx)!=YSOK)
				{
					str.Arguments <16> (args);
					if(args.GetN()>0)
					{
						YSBOOL srmExtension;
						srmExtension=YSFALSE;

						args[0].Capitalize();

						switch(args[0][0])
						{
						case 'G':  // GE, GL, GF
						case 'Z':  // ZE, ZF, ZNBE,ZNBF, ZT, ZA, ZZ, ZH, ZU
						case 'Y':  // YE, YF, YN, Y1
						case 'M':  // M
						case 'C':  // CV
						case 'X':  // X
						case 'L':  // LF, LE, LL
							srmExtension=YSTRUE;
							break;
						}

						if(srmExtension!=YSTRUE)
						{
							return YSERR;
						}
					}
				}
			}
			EndReadSrf();
			return YSOK;
		}
	}

	return YSERR;
}

YSRESULT YsShell::MergeSrf(
    const char fn[],YsArray <YsShellPolygonHandle> *noShadingPolygon,YsArray <YsShellVertexHandle> *roundVtx)
{
	FILE *fp;
	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		YSRESULT res=MergeSrf(fp,noShadingPolygon,roundVtx);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT YsShell::MergeSrf(
    FILE *fp,YsArray <YsShellPolygonHandle> *noShadingPolygon,YsArray <YsShellVertexHandle> *roundVtx)
{
	if(NULL!=fp)
	{
		if(noShadingPolygon!=NULL)
		{
			noShadingPolygon->Set(0,NULL);
		}
		if(roundVtx!=NULL)
		{
			roundVtx->Set(0,NULL);
		}
		if(BeginMergeSrf()==YSOK)
		{
			YsString str;
			while(str.Fgets(fp)!=NULL)
			{
				if(ReadSrfOneLine(str,noShadingPolygon,roundVtx)!=YSOK)
				{
					return YSERR;
				}
			}
			EndReadSrf();
			return YSOK;
		}
	}

	return YSERR;
}

YSRESULT YsShell::LoadTri(const char fn[])
{
	FILE *fp;
	int nNod,nTri;
	YsShellVertexHandle *nodVtHd;
	int ac;
	char buf[256],str[256],*av[32];

	nNod=0;
	nTri=0;
	nodVtHd=NULL;

	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		while(fgets(str,256,fp)!=NULL)
		{
			strcpy(buf,str);
			YsArguments(&ac,av,32,buf);
			if(ac>0)
			{
				if(strcmp(av[0],"nofnod")==0)
				{
					int i;
					nNod=atoi(av[1]);
					nodVtHd=new YsShellVertexHandle [nNod+1];
					if(nodVtHd==NULL)
					{
						YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell::LoadTri()");
					}
					for(i=0; i<nNod; i++)
					{
						nodVtHd[i]=NULL;
					}
				}
				else if(strcmp(av[0],"noftri")==0)
				{
					nTri=atoi(av[1]);
				}
				else if(strcmp(av[0],"nod")==0)
				{
					if(nodVtHd!=NULL)
					{
						int ndId;
						YsVec3 pos;
						ndId=atoi(av[1]);
						pos.Set(atof(av[2]),atof(av[3]),atof(av[4]));
						nodVtHd[ndId]=AddVertexH(pos);
					}
					else
					{
						YsErrOut("YsShell::LoadTri()\n");
						YsErrOut("  nofnod didn't come before nod\n");
						fclose(fp);
						goto ERREND;
					}
				}
				else if(strcmp(av[0],"tri")==0)
				{
					YsShellVertexHandle vtHd[3];
					vtHd[0]=nodVtHd[atoi(av[2])];
					vtHd[1]=nodVtHd[atoi(av[3])];
					vtHd[2]=nodVtHd[atoi(av[4])];
					if(vtHd[0]!=NULL && vtHd[1]!=NULL && vtHd[2]!=NULL)
					{
						AddPolygonH(3,vtHd);
					}
					else
					{
						YsErrOut("YsShell::LoadTri()\n");
						YsErrOut("  Nodes must be defined before triangles.\n");
						fclose(fp);
						goto ERREND;
					}
				}
			}
		}
	}
	fclose(fp);
	return YSOK;

ERREND:
	if(nodVtHd!=NULL)
	{
		delete [] nodVtHd;
	}
	return YSERR;
}

YSRESULT YsShell::LoadMsh(const char fn[])
{
	CleanUp();
	return MergeMsh(fn);
}

YSRESULT YsShell::MergeMsh(const char fn[])
{
	FILE *fp;
	int nNod,nEl2;
	YsShellVertexHandle *nodVtHd;
	int ac;
	char buf[256],str[256],*av[32];

	nNod=0;
	nEl2=0;
	nodVtHd=NULL;

	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		while(fgets(str,256,fp)!=NULL)
		{
			int i;
			for(i=0; str[i]!=0; i++)
			{
				if(str[i]=='#')
				{
					str[i]=0;
					break;
				}
			}

			strcpy(buf,str);
			YsArguments(&ac,av,32,buf);
			if(ac>0)
			{
				if(strcmp(av[0],"nofnod")==0)
				{
					int i;
					nNod=atoi(av[1]);
					nodVtHd=new YsShellVertexHandle [nNod+1];
					if(nodVtHd==NULL)
					{
						YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell::MergeMsh()");
					}
					for(i=0; i<nNod; i++)
					{
						nodVtHd[i]=NULL;
					}
				}
				else if(strcmp(av[0],"nofel2")==0)
				{
					nEl2=atoi(av[1]);
				}
				else if(strcmp(av[0],"nod")==0)
				{
					if(nodVtHd!=NULL)
					{
						int ndId;
						YsVec3 pos;
						ndId=atoi(av[1]);
						pos.Set(atof(av[2]),atof(av[3]),atof(av[4]));
						nodVtHd[ndId]=AddVertexH(pos);
					}
					else
					{
						YsErrOut("YsShell::LoadTri()\n");
						YsErrOut("  nofnod didn't come before nod\n");
						fclose(fp);
						goto ERREND;
					}
				}
				else if(strcmp(av[0],"el2")==0)
				{
					YsShellVertexHandle vtHd[4];
					if(atoi(av[2])==3)
					{
						vtHd[0]=nodVtHd[atoi(av[3])];
						vtHd[1]=nodVtHd[atoi(av[4])];
						vtHd[2]=nodVtHd[atoi(av[5])];
						if(vtHd[0]!=NULL && vtHd[1]!=NULL && vtHd[2]!=NULL)
						{
							AddPolygonH(3,vtHd);
						}
					}
					else if(atoi(av[2])==4)
					{
						vtHd[0]=nodVtHd[atoi(av[3])];
						vtHd[1]=nodVtHd[atoi(av[4])];
						vtHd[2]=nodVtHd[atoi(av[5])];
						vtHd[3]=nodVtHd[atoi(av[6])];
						if(vtHd[0]!=NULL && vtHd[1]!=NULL && vtHd[2]!=NULL && vtHd[3]!=NULL)
						{
							AddPolygonH(4,vtHd);
						}
					}
					else
					{
						YsErrOut("YsShell::LoadMsh()\n");
						YsErrOut("  Load Error\n");
						YsErrOut("  Refered to undefined node or non-tri&non-quad element\n");
						fclose(fp);
						goto ERREND;
					}
				}
			}
		}
	}
	fclose(fp);
	return YSOK;

ERREND:
	if(nodVtHd!=NULL)
	{
		delete [] nodVtHd;
	}
	return YSERR;
}

YSRESULT YsShell::LoadMs3(const char fn[])
{
	CleanUp();
	return MergeMs3(fn);
}

YSRESULT YsShell::MergeMs3(const char fn[])
{
	FILE *fp;
	YsArray <YsShellVertexHandle> vtHdList;
	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		char buf[256];
		YsShellVertexHandle plVtHd[4];
		while(fgets(buf,255,fp)!=NULL)
		{
			if('#'==buf[0])
			{
				continue;
			}

			int ac;
			char *av[16];
			if(YsArguments(&ac,av,16,buf)==YSOK && ac>0)
			{
				if(av[0][0]=='v' || av[0][0]=='V')
				{
					if(ac>=4)
					{
						YsShellVertexHandle vtHd;
						YsVec3 pos;
						double x,y,z;
						x=atof(av[1]);
						y=atof(av[2]);
						z=atof(av[3]);
						pos.Set(x,y,z);
						vtHd=AddVertexH(pos);
						vtHdList.Append(vtHd);
					}
					else
					{
						goto ERREND;
					}
				}
				else if(av[0][0]=='D' || av[0][0]=='d')  // D:Delta=Triangle
				{
					if(ac>=4)
					{
						plVtHd[0]=vtHdList[atoi(av[1])];
						plVtHd[1]=vtHdList[atoi(av[2])];
						plVtHd[2]=vtHdList[atoi(av[3])];
						SetColorOfPolygon(AddPolygonH(3,plVtHd),YsBlue());
					}
					else
					{
						goto ERREND;
					}
				}
				else if(av[0][0]=='Q' || av[0][0]=='q')
				{
					if(ac>=5)
					{
						plVtHd[0]=vtHdList[atoi(av[1])];
						plVtHd[1]=vtHdList[atoi(av[2])];
						plVtHd[2]=vtHdList[atoi(av[3])];
						plVtHd[3]=vtHdList[atoi(av[4])];
						SetColorOfPolygon(AddPolygonH(4,plVtHd),YsBlue());
					}
					else
					{
						goto ERREND;
					}
				}
			}
		}
		fclose(fp);
		return YSOK;
	}

ERREND:
	if(fp!=NULL)
	{
		fclose(fp);
	}
	return YSERR;
}

YSRESULT YsShell::LoadDat(const char fn[])
{
	CleanUp();
	return MergeDat(fn);
}

YSRESULT YsShell::MergeDat(const char fn[])
{
	FILE *fp;
	YsArray <YsShellVertexHandle> vtHdList;
	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		char buf[4096];
		YsArray <YsShellVertexHandle,64> plVtHd;
		while(fgets(buf,4095,fp)!=NULL)
		{
			int ac;
			char *av[1024];
			if(YsArguments(&ac,av,1024,buf)==YSOK && ac>0)
			{
				if(av[0][0]=='v' || av[0][0]=='V')
				{
					if(ac>=4)
					{
						YsShellVertexHandle vtHd;
						YsVec3 pos;
						double x,y,z;
						x=atof(av[1]);
						y=atof(av[2]);
						z=atof(av[3]);
						pos.Set(x,y,z);
						vtHd=AddVertexH(pos);
						vtHdList.Append(vtHd);
					}
					else
					{
						goto ERREND;
					}
				}
				else if(av[0][0]=='f' || av[0][0]=='F')  // D:Delta=Triangle
				{
					if(ac>=4)
					{
						int i;
						YsShellPolygonHandle plHd;
						plVtHd.Set(ac-1,NULL);
						for(i=0; i<ac-1; i++)
						{
							plVtHd[i]=vtHdList[atoi(av[i+1])];
						}
						plHd=AddPolygonH(ac-1,vtHdList);
						SetColorOfPolygon(plHd,YsBlue());
					}
					else
					{
						goto ERREND;
					}
				}
			}
		}
		fclose(fp);
		return YSOK;
	}

ERREND:
	if(fp!=NULL)
	{
		fclose(fp);
	}
	return YSERR;
}

YSRESULT YsShell::SaveSrf(
    const char fn[],
    YSSIZE_T nNoShadingPolygon,const YsShellPolygonHandle noShadingPolygon[],
    YSSIZE_T nRoundVtx,const YsShellVertexHandle roundVtx[]) const
{
	FILE *fp;
	fp=fopen(fn,"w");
	if(fp!=NULL)
	{
		YsTextFileOutputStream textOut(fp);
		YSRESULT res=SaveSrf(textOut,nNoShadingPolygon,noShadingPolygon,nRoundVtx,roundVtx);
		fclose(fp);
		return res;
	}
	return YSERR;

}

YSRESULT YsShell::SaveSrf(
    class YsTextOutputStream &textOut,
    YSSIZE_T nNoShadingPolygon,const YsShellPolygonHandle noShadingPolygon[],
    YSSIZE_T nRoundVtx,const YsShellVertexHandle roundVtx[]) const
{
	YsShellVertexHandle vtHd,tstVtHd;
	YsShellPolygonHandle plHd,tstPlHd;

	YsHashTable <YsShellVertexHandle> roundVtxTable(nRoundVtx+1);
	YsHashTable <YsShellPolygonHandle> noShadingPolygonTable(nNoShadingPolygon+1);

	int i;
	if(noShadingPolygon!=NULL)
	{
		for(i=0; i<nNoShadingPolygon; i++)
		{
			noShadingPolygonTable.AddElement(GetSearchKey(noShadingPolygon[i]),noShadingPolygon[i]);
		}
	}
	if(roundVtx!=NULL)
	{
		for(i=0; i<nRoundVtx; i++)
		{
			roundVtxTable.AddElement(GetSearchKey(roundVtx[i]),roundVtx[i]);
		}
	}


	Encache();


	textOut.Printf("SURF\n");

	vtHd=NULL;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		YsVec3 pos;
		GetVertexPosition(pos,vtHd);

		if(roundVtxTable.FindElement(tstVtHd,GetSearchKey(vtHd))!=YSOK)
		{
			textOut.Printf("V %lf %lf %lf\n",pos.x(),pos.y(),pos.z());
		}
		else
		{
			textOut.Printf("V %lf %lf %lf R\n",pos.x(),pos.y(),pos.z());
		}
	}

	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		int i,n;
		YsVec3 cen,nom,pos;
		const YsShellVertexHandle *plVtHd;
		n=GetNumVertexOfPolygon(plHd);
		plVtHd=GetVertexListOfPolygon(plHd);

		textOut.Printf("F\n");

		if(noShadingPolygonTable.FindElement(tstPlHd,GetSearchKey(plHd))==YSOK)
		{
			textOut.Printf("B\n");
		}

		cen.Set(0.0,0.0,0.0);
		for(i=0; i<n; i++)
		{
			if(i%8==0)
			{
				textOut.Printf("V");
			}

			textOut.Printf(" %d",GetVertexIdFromHandle(plVtHd[i]));

			if(i%8==7 || i==n-1)
			{
				textOut.Printf("\n");
			}

			GetVertexPosition(pos,plVtHd[i]);
			cen+=pos;
		}

		cen/=double(n);
		GetNormalOfPolygon(nom,plHd);


		YsColor col;
//		unsigned int r,g,b;
		GetColorOfPolygon(col,plHd);
//		r=YsBound(int(col.Rd()*31.0),0,31);
//		g=YsBound(int(col.Gd()*31.0),0,31);
//		b=YsBound(int(col.Bd()*31.0),0,31);
//		textOut.Printf("C %d\n",g*1024+r*32+b);
		textOut.Printf("C %d\n",col.Get15BitColor());

		textOut.Printf("N %lf %lf %lf %lf %lf %lf\n",cen.x(),cen.y(),cen.z(),nom.x(),nom.y(),nom.z());
		textOut.Printf("E\n");
	}

	for(auto mdHd : AllMetaData())
	{
		auto keyValue=GetMetaData(mdHd);
		keyValue.key.EncodeEscape();
		keyValue.value.EncodeEscape();
		textOut.Printf("MDT %s %s\n",keyValue.key.c_str(),keyValue.value.c_str());
	}

	textOut.Printf("E\n");
	return YSOK;
}

YSRESULT YsShell::SaveMsh(int &nIgnored,const char fn[]) const
{
	FILE *fp;
	YsShellVertexHandle vtHd;
	YsShellPolygonHandle plHd;

	Encache();
	nIgnored=0;

	fp=fopen(fn,"w");
	if(fp!=NULL)
	{
		int id,nofel2;

		fprintf(fp,"nofnod %d\n",GetNumVertex());

		id=1;
		vtHd=NULL;
		while((vtHd=FindNextVertex(vtHd))!=NULL)
		{
			YsVec3 pos;
			GetVertexPosition(pos,vtHd);
			fprintf(fp,"nod %d %lf %lf %lf\n",id,pos.x(),pos.y(),pos.z());
			id++;
		}

		nofel2=0;
		plHd=NULL;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			if(GetNumVertexOfPolygon(plHd)==3 || GetNumVertexOfPolygon(plHd)==4)
			{
				nofel2++;
			}
			else
			{
				nIgnored++;
			}
		}
		fprintf(fp,"nofel2 %d\n",nofel2);

		id=1;
		plHd=NULL;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			int i,n;
			const YsShellVertexHandle *plVtHd;
			n=GetNumVertexOfPolygon(plHd);
			plVtHd=GetVertexListOfPolygon(plHd);

			if(n==3 || n==4)
			{
				fprintf(fp,"el2 %d %d",id,n);
				for(i=0; i<n; i++)
				{
					fprintf(fp," %d",GetVertexIdFromHandle(plVtHd[i])+1);
				}
				fprintf(fp,"\n");
				id++;
			}
		}

		fclose(fp);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SaveMs3(int &nIgnored,const char fn[]) const
{
	FILE *fp;
	YsShellVertexHandle vtHd;
	YsShellPolygonHandle plHd;

	Encache();
	nIgnored=0;

	fp=fopen(fn,"w");
	if(fp!=NULL)
	{
		vtHd=NULL;
		while((vtHd=FindNextVertex(vtHd))!=NULL)
		{
			YsVec3 pos;
			GetVertexPosition(pos,vtHd);
			fprintf(fp,"V %lf %lf %lf\n",pos.x(),pos.y(),pos.z());
		}

		plHd=NULL;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			int n;
			const YsShellVertexHandle *plVtHd;
			n=GetNumVertexOfPolygon(plHd);
			plVtHd=GetVertexListOfPolygon(plHd);

			if(n==3)
			{
				fprintf(fp,"D %d %d %d\n",   // D of Delta
				    GetVertexIdFromHandle(plVtHd[0]),
				    GetVertexIdFromHandle(plVtHd[1]),
				    GetVertexIdFromHandle(plVtHd[2]));
			}
			else if(n==4)
			{
				fprintf(fp,"Q %d %d %d %d\n",
				    GetVertexIdFromHandle(plVtHd[0]),
				    GetVertexIdFromHandle(plVtHd[1]),
				    GetVertexIdFromHandle(plVtHd[2]),
				    GetVertexIdFromHandle(plVtHd[3]));
			}
		}

		fclose(fp);
		return YSOK;
	}
	return YSERR;
}

