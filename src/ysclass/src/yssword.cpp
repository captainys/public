/* ////////////////////////////////////////////////////////////

File Name: yssword.cpp
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


YsSwordPolygonAttr::YsSwordPolygonAttr()
{
	isOverlappingPolygon=YSFALSE;
}

YsSwordNewVertexLog::YsSwordNewVertexLog()
{
	freeAttribute=0;
	betweenVt1=0;
	betweenVt2=0;
	idCreated=0;
}

// Implementation //////////////////////////////////////////

////////////////////////////////////////////////////////////
// Note:plg1->  0..id1 + id1...
//      plg2->  id1...id2
template <class T>
YSRESULT YsSwordSeparate(YsArray <T> &plg1,YsArray <T> &plg2,YsArray <T> &org,YSSIZE_T id1,YSSIZE_T id2)
{
	YSSIZE_T i;
	YSSIZE_T n=org.GetN();
	for(i=0; i<=id1; i++)
	{
		plg1.Append(org[i]);
	}
	for(i=id2; i<n; i++)
	{
		plg1.Append(org[i]);
	}
	for(i=id1; i<=id2; i++)
	{
		plg2.Append(org[i]);
	}
	return YSOK;
}
////////////////////////////////////////////////////////////



const char *YsSword::ClassName="YsSword";

const YsSword &YsSword::operator=(const YsSword &)
{
	// Forbidden
	// Ken Wo Midari Ni Fukusei Suru Bekarazu
	//    Soji
	return *this;
}

YsSword::YsSword()
{
	plg=NULL;
	cre=NULL;
	newCreateId=-1;

	debugMode=YSFALSE;
}

YsSword::YsSword(const YsSword &from)
{
	*this=from;

	debugMode=YSFALSE;
}

YsSword::~YsSword()
{
	if(plg!=NULL)
	{
		plg->DeleteList();
	}
	if(cre!=NULL)
	{
		cre->DeleteList();
	}
}

void YsSword::Encache(void) const
{
	plg->Encache();
	cre->Encache();
}

int YsSword::GetNumPolygon(void) const
{
	return plg->GetNumObject();
}

int YsSword::GetNumVertexOfPolygon(YSSIZE_T id) const
{
	const YsArray <YsVec3> *plg;
	plg=GetPolygon(id);
	if(plg!=NULL)
	{
		return (int)plg->GetN();  // I don't think we need to think about a polygon with 2billion+ vertices for a while.
	}
	return 0;
}

const YsArray <YsVec3> *YsSword::GetPolygon(YSSIZE_T i) const
{
	YsList <YsSwordPolygon> *seeker;
	seeker=plg->Seek(i);
	if(seeker!=NULL)
	{
		return &seeker->dat.plg;
	}
	else
	{
		return NULL;
	}
}

const YsArray <int> *YsSword::GetVertexIdList(YSSIZE_T i) const
{
	YsList <YsSwordPolygon> *seeker;
	seeker=plg->Seek(i);
	if(seeker!=NULL)
	{
		return &seeker->dat.vtId;
	}
	else
	{
		return NULL;
	}
}

const YsArray <YsShell::VertexHandle> *YsSword::GetVtHdList(YSSIZE_T i) const
{
	YsList <YsSwordPolygon> *seeker;
	seeker=plg->Seek(i);
	if(seeker!=NULL)
	{
		return &seeker->dat.vtHd;
	}
	else
	{
		return NULL;
	}
}

int YsSword::GetNumNewVertex(void) const
{
	return cre->GetNumObject();
}

const YsSwordNewVertexLog *YsSword::GetNewVertexLog(int i) const
{
	YsList <YsSwordNewVertexLog> *seeker;
	seeker=cre->Seek(i);
	if(seeker!=NULL)
	{
		return &seeker->dat;
	}
	else
	{
		return NULL;
	}
}

YSRESULT YsSword::SetInitialPolygon(YSSIZE_T np,const YsVec3 p[])
{
	return SetInitialPolygon(np,p,nullptr,nullptr);
}

YSRESULT YsSword::SetInitialPolygon(YSSIZE_T np,const YsVec3 p[],const int id[])
{
	return SetInitialPolygon(np,p,id,nullptr);
}

YSRESULT YsSword::SetInitialPolygon(YSSIZE_T np,const YsVec3 p[],const int id[],const YsShell::VertexHandle vtHd[])
{
	if(plg!=nullptr)
	{
		plg->DeleteList();
		plg=nullptr;
	}
	if(cre!=nullptr)
	{
		cre->DeleteList();
		cre=nullptr;
	}
	newCreateId=-1;

	if(YsGetPolygonProjectionMatrix(plgPrjMatrix,np,p)==YSOK)
	{
		YsList <YsSwordPolygon> *neo;
		neo=new YsList <YsSwordPolygon>;
		if(neo!=NULL)
		{
			int i;
			for(i=0; i<np; i++)
			{
				YsVec3 prj3;
				YsVec2 prj2;
				prj3=plgPrjMatrix*p[i];
				prj2.Set(prj3.x(),prj3.y());
				if(neo->dat.prj.AppendItem(prj2)!=YSOK)
				{
					delete neo;
					return YSERR;
				}
			}

			if(neo->dat.plg.Set(np,p)!=YSOK)
			{
				delete neo;
				return YSERR;
			}

			if(id!=NULL && neo->dat.vtId.Set(np,id)!=YSOK)
			{
				delete neo;
				return YSERR;
			}
			if(nullptr!=vtHd && YSOK!=neo->dat.vtHd.Set(np,vtHd))
			{
				delete neo;
				return YSERR;
			}

			plg=plg->Append(neo);
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsSword::GetVertexListOfPolygon(YsVec3 vec[],YSSIZE_T maxcount,int id) const
{
	const YsArray <YsVec3> *plg;
	plg=GetPolygon(id);
	if(plg!=NULL)
	{
		YSSIZE_T i;
		for(i=0; i<plg->GetN() && i<maxcount; i++)
		{
			vec[i]=(*plg)[i];
		}
		if(plg->GetN()<=maxcount)
		{
			return YSOK;
		}
		else
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsSword::GetVertexListOfPolygon(const YsVec3 *vec[],YSSIZE_T maxcount,int id) const
{
	const YsArray <YsVec3> *plg;
	plg=GetPolygon(id);
	if(plg!=NULL)
	{
		YSSIZE_T i;
		for(i=0; i<plg->GetN() && i<maxcount; i++)
		{
			vec[i]=&(*plg)[i];
		}
		if(plg->GetN()<=maxcount)
		{
			return YSOK;
		}
		else
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsSword::SetInitialPolygon(YSSIZE_T np,const YsVec2 p[],int const id[])
{
	YSSIZE_T i;
	YsVec3 *p3;
	YSRESULT res;
	YsOneMemoryBlock <YsVec3,128> memBlock;

	p3=memBlock.GetMemoryBlock(np);

	if(p3!=NULL)
	{
		for(i=0; i<np; i++)
		{
			p3[i].Set(p[i].x(),p[i].y(),0.0);
		}
		res=SetInitialPolygon(np,p3,id);

		return res;
	}
	return YSERR;
}

YSRESULT YsSword::Slash(const YsVec2 &org,const YsVec2 &vec)
{
	YsVec3 o3,v3;
	o3.Set(org.x(),org.y(),0.0);
	v3.Set(-vec.y(),vec.x(),0.0);
	if(v3.GetSquareLength()>YsSqr(YsTolerance))  // Modified 2000/11/10
	{
		YsPlane pln(o3,v3);
		Slash(pln);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSword::GetVertexListOfPolygon(YsVec2 vec[],YSSIZE_T maxcount,int id) const
{
	const YsArray <YsVec3> *plg;
	plg=GetPolygon(id);
	if(plg!=NULL)
	{
		YSSIZE_T i;
		for(i=0; i<plg->GetN() && i<maxcount; i++)
		{
			vec[i].Set(plg->v(i).x(),plg->v(i).y());
		}
		if(plg->GetN()<=maxcount)
		{
			return YSOK;
		}
		else
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsSword::Slash(const YsPlane &pln)
{
	return SlashOnePolygon(plg,pln);
}

YSRESULT YsSword::Slash(YSSIZE_T nv,const YsVec3 v[])
{
	YsPlane pln;
	pln.MakeBestFitPlane(nv,v);
	return SlashOnePolygon(plg,pln,nv,v);
}

YSRESULT YsSword::SlashByOverlappingPolygon3(YSSIZE_T nv,const YsVec3 v[],int note)
{
	int i;
	YsVec3 p[4],normalOfInputPolygon;

	if(YsGetAverageNormalVector(normalOfInputPolygon,nv,v)==YSOK)
	{
		for(i=0; i<nv; i++)
		{
			p[0]=v[i       ]+normalOfInputPolygon;
			p[1]=v[(i+1)%nv]+normalOfInputPolygon;
			p[2]=v[(i+1)%nv]-normalOfInputPolygon;
			p[3]=v[i       ]-normalOfInputPolygon;
			Slash(4,p);
		}

		YsList <YsSwordPolygon> *seeker;
		YsVec3 inside;
		for(seeker=plg; seeker!=NULL; seeker=seeker->Next())
		{
			YSSIZE_T nVtx;
			const YsVec3 *vtx;
			nVtx=seeker->dat.plg.GetN();
			vtx=seeker->dat.plg.GetArray();
			if(YsGetArbitraryInsidePointOfPolygon3(inside,nVtx,vtx)==YSOK)
			{
				if(YsCheckInsidePolygon3(inside,nv,v)==YSINSIDE)
				{
					seeker->dat.attrSet.isOverlappingPolygon=YSTRUE;
					seeker->dat.attrSet.noteOverlapping=note;
				}
			}
		}
		return YSOK;
	}
	return YSERR;
}

YSBOOL YsSword::IsFromOverlappingPolygon3(YSSIZE_T i) const
{
	int note;
	return IsFromOverlappingPolygon3(i,note);
}

YSBOOL YsSword::IsFromOverlappingPolygon3(YSSIZE_T i,int &note) const
{
	YsList <YsSwordPolygon> *seeker;
	seeker=plg->Seek((int)i);
	if(seeker!=NULL)
	{
		note=seeker->dat.attrSet.noteOverlapping;
		return seeker->dat.attrSet.isOverlappingPolygon;
	}
	return YSFALSE;
}




YSRESULT YsSword::SlashOnePolygon
 (YsList <YsSwordPolygon> *target,const YsPlane &pln,YSSIZE_T nv,const YsVec3 v[])
{
	if(target==NULL)
	{
		return YSOK;
	}

	if(SlashOnePolygon(target->Next(),pln,nv,v)!=YSOK)
	{
		return YSERR;
	}

	if(nv>0 && v!=NULL)
	{
		YSINTERSECTION i;
		YsCollisionOfPolygon wiz;
		wiz.SetPolygon1(target->dat.plg.GetN(),target->dat.plg.GetArray());
		wiz.SetPolygon2(nv,v);


		i=wiz.CheckPenetration();

		if((i!=YSINTERSECT && i!=YSTOUCH) ||     // Means apart
		   (i==YSTOUCH && wiz.OneEdgeLiesOnTheOtherPlane()!=YSTRUE))
		{
			if(debugMode==YSTRUE)
			{
				YsPrintf("(1) YsSword::SlashOnePolygon\n");
			}

			YsArray <int> cutPointIndex;
			FindAddSlashPoint(cutPointIndex,target,pln);  //  ,nv,v);   // Exhaustive
			return YSOK;  // Just make vertices and done.
		}

	}

	YsArray <int> cutPointIndex;
	FindAddSlashPoint(cutPointIndex,target,pln);

	if(cutPointIndex.GetN()<2)
	{
		if(debugMode==YSTRUE)
		{
			YsPrintf("(2) YsSword::SlashOnePolygon\n");
		}
		return YSOK;
	}
	else if(cutPointIndex.GetN()==2)
	{
		// Shall I skip checking separatability?
		// No. Single line polygon or dot polygon could be generated.
		int id1,id2;
		id1=cutPointIndex[0];
		id2=cutPointIndex[1];

		if(debugMode==YSTRUE)
		{
			YsPrintf("(3) YsSword::SlashOnePolygon\n");
		}

		if(YsCheckSeparatability2(
		     target->dat.prj.GetN(),
		     target->dat.prj.GetArray(),
		     id1,id2)==YSTRUE)
		{
			if(debugMode==YSTRUE)
			{
				YsPrintf("(3a) YsSword::SlashOnePolygon\n");
			}

			YsList <YsSwordPolygon> *newPolygon;
			newPolygon=SeparateByTwo(target,id1,id2);
			if(newPolygon!=NULL)
			{
				plg=plg->Append(newPolygon);
				plg=target->DeleteFromList();
			}
		}

		if(debugMode==YSTRUE)
		{
			YsPrintf("(3b) YsSword::SlashOnePolygon\n");
		}
		return YSOK;
	}
	else if(cutPointIndex.GetN()>=3)
	{
		if(debugMode==YSTRUE)
		{
			YsPrintf("(4) YsSword::SlashOnePolygon\n");
		}

		// Oops. It must be a concave polygon.
		YsList <YsSwordPolygon> *newPolygon;
		newPolygon=SeparateByMulti(target,cutPointIndex);
		if(newPolygon!=NULL)
		{
			plg=plg->Append(newPolygon);
			plg=target->DeleteFromList();
		}
		return YSOK;
	}

	if(debugMode==YSTRUE)
	{
		YsPrintf("(5) YsSword::SlashOnePolygon\n");
	}

	return YSERR;
}

YSRESULT YsSword::FindAddSlashPoint
    (YsArray <int> &idx,YsList <YsSwordPolygon> *target,const YsPlane &pln,
     int nCuttingVtx,const YsVec3 *cuttingVtx)
{
	int i,vtId;
	YsArray <YsVec3> plg;
	YsArray <YsVec2> prj;
	YsArray <int> idLst;
	YsVec3 crs;
	const YsVec3 *v1,*v2;

	YSSIZE_T n=target->dat.plg.GetN();
	vtId=0;
	for(i=0; i<n; i++)
	{
		plg.Append(target->dat.plg[i]);
		prj.Append(target->dat.prj[i]);
		if(target->dat.vtId.GetN()==target->dat.plg.GetN())
		{
			idLst.AppendItem(target->dat.vtId[i]);
		}

		if(0<target->dat.vtId.GetN() && target->dat.vtId.GetN()!=target->dat.plg.GetN())
		{
			YsErrOut("YsSword::FindAddSlashPoint()\n  Warning2\n");
		}

		v1=&target->dat.plg[i];
		v2=&target->dat.plg[(i+1)%n];

		if(pln.CheckOnPlane(*v1)==YSTRUE ||
		   (pln.GetIntersection(crs,*v1,*v2-*v1)==YSOK && *v1==crs))
		{
			idx.AppendItem(vtId);
		}
		else if(pln.GetPenetration(crs,*v1,*v2)==YSOK && *v2!=crs)
		{
			YsVec3 crs3;
			YsVec2 crs2;

			if(cuttingVtx!=NULL)
			{
				YSSIDE s;
				s=YsCheckInsidePolygon3(crs,nCuttingVtx,cuttingVtx);
				if(s!=YSINSIDE && s!=YSBOUNDARY)
				{
					goto SKIP;
				}
			}

			crs3=plgPrjMatrix*crs;
			crs2.Set(crs3.x(),crs3.y());

			vtId++;
			plg.AppendItem(crs);
			prj.AppendItem(crs2);
			idx.AppendItem(vtId);
			if(0<target->dat.vtId.GetN())
			{
				YsList <YsSwordNewVertexLog> *neocre,*ptr;
				for(ptr=cre; ptr!=NULL; ptr=ptr->Next())
				{
					if(ptr->dat.pos==crs)
					{
						idLst.AppendItem(ptr->dat.idCreated);
						goto ALREADYEXIST;
					}
				}

				idLst.AppendItem(newCreateId);
				neocre=new YsList <YsSwordNewVertexLog>;
				if(neocre!=NULL)
				{
					neocre->dat.betweenVt1=target->dat.vtId[i];
					neocre->dat.betweenVt2=target->dat.vtId[(i+1)%n];
					neocre->dat.idCreated=newCreateId;
					neocre->dat.pos=crs;
					cre=cre->Append(neocre);
				}


				// Add Same Vertex to other polygons that have same edge *v1-*v2
				// New feature 2000/04/09. (Now turned off. I'm taking exhaustive slashing method)
				// Non Exhaustive
				// FindAddSlashPointToNeighborPolygons(*v1,*v2,crs,crs2,newCreateId,target);

				newCreateId--;
			ALREADYEXIST:
				;
			}
		SKIP:
			;
		}
		vtId++;
	}

	if(n<vtId)
	{
		target->dat.plg=plg;
		target->dat.prj=prj;
		if(target->dat.vtId.GetN()>0)
		{
			target->dat.vtId=idLst;
			if(plg.GetN()!=idLst.GetN())
			{
				YsErrOut("YsSword::FindAddSlashPoint()\n  Warning1\n");
			}
		}
	}

	return YSOK;
}

YSRESULT YsSword::FindAddSlashPointToNeighborPolygons
		(const YsVec3 &v1,const YsVec3 &v2,
		 const YsVec3 &newVtx3,const YsVec2 &newVtx2,int newCreatedId,YsList <YsSwordPolygon> *exclude)
{
	int i;
	YsList <YsSwordPolygon> *neighbor;
	for(neighbor=plg; neighbor!=NULL; neighbor=neighbor->Next())
	{
		if(neighbor!=exclude)
		{
			YSSIZE_T n=neighbor->dat.plg.GetN();
			for(i=0; i<n; i++)
			{
				if((neighbor->dat.plg[i]==v1 && neighbor->dat.plg[(i+1)%n]==v2) ||
				   (neighbor->dat.plg[i]==v2 && neighbor->dat.plg[(i+1)%n]==v1))
				{
					neighbor->dat.plg.Insert(i+1,newVtx3);
					neighbor->dat.prj.Insert(i+1,newVtx2);
					if(neighbor->dat.vtId.GetN()>0)
					{
						neighbor->dat.vtId.Insert(i+1,newCreatedId);
					}
				}
			}
		}
	}
	return YSOK;
}

YsList <YsSwordPolygon> *YsSword::SeparateByTwo(YsList <YsSwordPolygon> *org,int id1,int id2)
{
	YsArray <YsVec3> plg1,plg2;
	YsArray <YsVec2> prj1,prj2;
	YsArray <int> vtId1,vtId2;
	YsArray <YsShellVertexHandle> vtHd1,vtHd2;
	YSBOOL identical;  // 2005/02/08

	identical=YSFALSE;
	if(org->dat.plg[id1]==org->dat.plg[id2])  // 2005/02/08
	{
		identical=YSTRUE;
	}


	YsSwordSeparate <YsVec3> (plg1,plg2,org->dat.plg,id1,id2);
	YsSwordSeparate <YsVec2> (prj1,prj2,org->dat.prj,id1,id2);
	if(org->dat.vtId.GetN()==org->dat.plg.GetN())
	{
		YsSwordSeparate <int> (vtId1,vtId2,org->dat.vtId,id1,id2);
	}
	if(org->dat.vtHd.GetN()==org->dat.plg.GetN())
	{
		YsSwordSeparate <YsShellVertexHandle> (vtHd1,vtHd2,org->dat.vtHd,id1,id2);
	}



	if(identical==YSTRUE)  // 2005/02/08
	{
		YSSIZE_T i;
		YsVec3 prv;
		prv=plg1[0];
		for(i=plg1.GetN()-1; i>=0; i--)
		{
			if(plg1[i]==prv)
			{
				plg1.Delete(i);
				prj1.Delete(i);
				if(org->dat.vtId.GetN()==org->dat.plg.GetN())
				{
					vtId1.Delete(i);
				}
				if(org->dat.vtHd.GetN()==org->dat.plg.GetN())
				{
					vtHd1.Delete(i);
				}
			}
			else
			{
				prv=plg1[i];
			}
		}

		for(i=plg2.GetN()-1; i>=0; i--)
		{
			if(plg2[i]==prv)
			{
				plg2.Delete(i);
				prj2.Delete(i);
				if(org->dat.vtId.GetN()==org->dat.plg.GetN())
				{
					vtId2.Delete(i);
				}
				if(org->dat.vtHd.GetN()==org->dat.plg.GetN())
				{
					vtHd2.Delete(i);
				}
			}
			else
			{
				prv=plg2[i];
			}
		}
	}



	YsList <YsSwordPolygon> *lst1,*lst2;

	if(plg1.GetN()>=3 && plg2.GetN()>=3)
	{
		lst1=new YsList <YsSwordPolygon>;
		if(lst1!=NULL)
		{
			lst2=new YsList <YsSwordPolygon>;
			if(lst2!=NULL)
			{
				lst1->dat.plg=plg1;
				lst1->dat.prj=prj1;
				lst1->dat.vtId=vtId1;
				lst1->dat.vtHd=vtHd1;
				lst1->dat.attrSet=org->dat.attrSet;

				lst2->dat.plg=plg2;
				lst2->dat.prj=prj2;
				lst2->dat.vtId=vtId2;
				lst2->dat.vtHd=vtHd2;
				lst2->dat.attrSet=org->dat.attrSet;

				lst1=lst1->Append(lst2);
				return lst1;
			}
			delete lst1;
		}
	}
	else if(plg1.GetN()>=3)
	{
		lst1=new YsList <YsSwordPolygon>;
		if(lst1!=NULL)
		{
			lst1->dat.plg=plg1;
			lst1->dat.prj=prj1;
			lst1->dat.vtId=vtId1;
			lst1->dat.vtHd=vtHd1;
			lst1->dat.attrSet=org->dat.attrSet;
			return lst1;
		}
	}
	else if(plg2.GetN()>=3)
	{
		lst2=new YsList <YsSwordPolygon>;
		if(lst2!=NULL)
		{
			lst2->dat.plg=plg2;
			lst2->dat.prj=prj2;
			lst2->dat.vtId=vtId2;
			lst2->dat.vtHd=vtHd2;
			lst2->dat.attrSet=org->dat.attrSet;
			return lst2;
		}
	}
	return NULL;
}

YsList <YsSwordPolygon> *YsSword::SeparateByMulti(YsList <YsSwordPolygon> *org,YsArray <int> &cutPoint)
{
	int i,j,k,id1,id2;
	YsArray <YsVec3> p1,p2;
	YsArray <YsVec2> prj1,prj2;
	YsArray <int> vtId1,vtId2;
	YsArray <YsShellVertexHandle> vtHd1,vtHd2;
	YsArray <int> cp1,cp2;

	YSSIZE_T n=cutPoint.GetN();
	for(i=0; i<n; i++)
	{
		for(j=i+1; j<n; j++)
		{
			id1=cutPoint[i];
			id2=cutPoint[j];  // Note: Always id1<id2
			if(id1>=id2)
			{
				YsErrOut("YsSword::SeparateByMulti()\n  Warning (Potentially an internal error)\n");
			}
			if(YsCheckSeparatability2(org->dat.prj.GetN(),org->dat.prj,id1,id2)==YSTRUE)
			{
				YsSwordSeparate <YsVec3> (p1,p2,org->dat.plg,id1,id2);
				YsSwordSeparate <YsVec2> (prj1,prj2,org->dat.prj,id1,id2);
				if(org->dat.vtId.GetN()==org->dat.plg.GetN())
				{
					YsSwordSeparate <int> (vtId1,vtId2,org->dat.vtId,id1,id2);
				}
				if(org->dat.vtHd.GetN()==org->dat.plg.GetN())
				{
					YsSwordSeparate <YsShellVertexHandle> (vtHd1,vtHd2,org->dat.vtHd,id1,id2);
				}
				for(k=0; k<n; k++)
				{
					int x;
					x=cutPoint[k];
					if(x<=id1)
					{
						cp1.Append(cutPoint[k]);
					}
					else if(id1<=x && x<=id2)
					{
						cp2.Append(cutPoint[k]-id1);
					}
					else if(id2<=cutPoint[k])
					{
						cp1.Append(cutPoint[k]-id2+id1+1);
					}
				}

				YsList <YsSwordPolygon> plg1,plg2,*plgLst1,*plgLst2;
				plg1.dat.plg=p1;
				plg1.dat.prj=prj1;
				plg1.dat.vtId=vtId1;
				plg1.dat.vtHd=vtHd1;

				plg2.dat.plg=p2;
				plg2.dat.prj=prj2;
				plg2.dat.vtId=vtId2;
				plg2.dat.vtHd=vtHd2;

				plgLst1=SeparateByMulti(&plg1,cp1);
				plgLst2=SeparateByMulti(&plg2,cp2);
				if(plgLst1==NULL || plgLst2==NULL)
				{
					YsErrOut("YsSword::SeparateByMulti()\n  Warning!!\n");
				}
				plgLst1=plgLst1->Append(plgLst2);
				return plgLst1;
			}
		}
	}
	// Loopout -> means there's no other separatable combinations.
	YsList <YsSwordPolygon> *neo;
	neo=new YsList <YsSwordPolygon>;
	neo->dat.plg=org->dat.plg;
	neo->dat.prj=org->dat.prj;
	neo->dat.vtId=org->dat.vtId;
	neo->dat.attrSet=org->dat.attrSet;
	return neo;
}



YSRESULT YsSword::Triangulate(YSCONVEXNIZESTRATEGY strategy)
{
	return ConvexnizeGo(YSTRUE,plg,strategy);
}

YSRESULT YsSword::Convexnize(YSCONVEXNIZESTRATEGY strategy)
{
	return ConvexnizeGo(YSFALSE,plg,strategy);
}

YSRESULT YsSword::UpdateVertexId(int oldId,int newId)
{
	int i;

	// Update Ids in polygons
	YsList <YsSwordPolygon> *plgSeek;
	for(plgSeek=plg; plgSeek!=NULL; plgSeek=plgSeek->Next())
	{
		for(i=0; i<plgSeek->dat.vtId.GetN(); i++)
		{
			if(plgSeek->dat.vtId[i]==oldId)
			{
				plgSeek->dat.vtId[i]=newId;
			}
		}
	}

	// Update Ids in new vertex log
	YsList <YsSwordNewVertexLog> *logSeek;
	for(logSeek=cre; logSeek!=NULL; logSeek=logSeek->Next())
	{
		if(logSeek->dat.betweenVt1==oldId)
		{
			logSeek->dat.betweenVt1=newId;
		}
		if(logSeek->dat.betweenVt2==oldId)
		{
			logSeek->dat.betweenVt2=newId;
		}
		if(logSeek->dat.idCreated==oldId)
		{
			logSeek->dat.idCreated=newId;
		}
	}

	return YSOK;
}

YSRESULT YsSword::ConvexnizeGo
   (YSBOOL tri,YsList <YsSwordPolygon> *target,YSCONVEXNIZESTRATEGY strategy)
{
	if(target==NULL)
	{
		return YSOK;
	}

	if(ConvexnizeGo(tri,target->Next(),strategy)!=YSOK)
	{
		return YSERR;
	}

	YsList <YsSwordPolygon> *newPolygon;
	newPolygon=ConvexnizeOnePolygon(tri,target,strategy);
	if(newPolygon!=NULL && newPolygon!=target)
	{
		plg=target->DeleteFromList();
		plg=plg->Append(newPolygon);
	}

	return YSOK;
}

YsList <YsSwordPolygon> *YsSword::ConvexnizeOnePolygon
  (YSBOOL tri,YsList <YsSwordPolygon> *target,YSCONVEXNIZESTRATEGY strategy)
{
	int id1,id2;
	if(tri==YSFALSE && YsCheckConvexByAngle2(target->dat.prj.GetN(),target->dat.prj)==YSTRUE)
	{
		return target;
	}

	if(FindConvexnizeVertexPair(id1,id2,target,strategy)==YSOK)
	{
		YsList <YsSwordPolygon> *newPolygon,*plg1,*plg2,*tPlg1,*tPlg2;
		newPolygon=SeparateByTwo(target,id1,id2);

		// 2005/02/08 Now SeparateByTwo may return NULL when a polygon is like a quad A-B-C-B, and it is not an error.
		// if(newPolygon==NULL || newPolygon->Next()==NULL)
		// {
		// 	YsErrOut("YsSword::ConvexnizeOnePolygon()\n  Warning (Low memory?)!!\n");
		// 	return newPolygon;
		// }
		if(newPolygon==NULL)
		{
			return NULL;
		}


		plg1=newPolygon;
		plg2=newPolygon->Next();

		if(plg2!=NULL)
		{
			tPlg2=ConvexnizeOnePolygon(tri,plg2,strategy);
			if(tPlg2!=plg2)
			{
				newPolygon=plg2->DeleteFromList();
				newPolygon=newPolygon->Append(tPlg2);
			}
		}

		tPlg1=ConvexnizeOnePolygon(tri,plg1,strategy);
		if(tPlg1!=plg1)
		{
			newPolygon=plg1->DeleteFromList();
			newPolygon=newPolygon->Append(tPlg1);
		}
		return newPolygon;
	}
	return target;
}

YSRESULT YsSword::FindConvexnizeVertexPair
	    (int &id1,int &id2,
	     YsList <YsSwordPolygon> *target,YSCONVEXNIZESTRATEGY strategy)
{
	int i,j;
	double ref;
	id1=-1;
	id2=-1;
	ref=0.0;
	YSSIZE_T n=target->dat.plg.GetN();
	for(i=0; i<n; i++)
	{
		for(j=i+1; j<n; j++)
		{
			if(YsCheckSeparatability2(target->dat.prj.GetN(),target->dat.prj,i,j)==YSTRUE)
			{
				if(strategy==YSCONVEXNIZEDEFAULT || strategy==YSCONVEXNIZEBALANCEDIST)
				{
					int k;
					YsVec2 vij,foot;
					double minDist;
					vij=target->dat.prj[j]-target->dat.prj[i];
					if(vij.Normalize()==YSOK)
					{
						minDist=YsInfinity;
						for(k=0; k<n; k++)
						{
							if(k!=i && k!=j)
							{
								foot=target->dat.prj[i]+vij*(vij*(target->dat.prj[k]-target->dat.prj[i]));
								minDist=YsSmaller(minDist,(target->dat.prj[k]-foot).GetSquareLength());
								if(minDist<ref)
								{
									break;
								}
							}
						}

						if(ref<minDist)
						{
							id1=i;
							id2=j;
							ref=minDist;
						}
					}
				}
				if(strategy==YSCONVEXNIZENOSMALLANGLE || strategy==YSCONVEXNIZE90DEGREE)
				{
					double cosm,cos0,cos1,cos2,cos3;
					YsVec2 vij,vji,v1,v2,v3,v4;

					vij=target->dat.prj[j]       -target->dat.prj[i];
					vji=-vij;
					v1=target->dat.prj[(i+1)%n]  -target->dat.prj[i];
					v2=target->dat.prj[(i+n-1)%n]-target->dat.prj[i];
					v3=target->dat.prj[(j+1)%n]  -target->dat.prj[j];
					v4=target->dat.prj[(j+n-1)%n]-target->dat.prj[j];

					vij.Normalize();
					vji.Normalize();
					v1.Normalize();
					v2.Normalize();
					v3.Normalize();
					v4.Normalize();

					if(strategy==YSCONVEXNIZE90DEGREE || strategy==YSCONVEXNIZEDEFAULT)
					{
						cos0=fabs(vij*v1);
						cos1=fabs(vij*v2);
						cos2=fabs(vji*v3);
						cos3=fabs(vji*v4);
					}
					else
					{
						cos0=vij*v1;
						cos1=vij*v2;
						cos2=vji*v3;
						cos3=vji*v4;
					}

					cosm=YsGreater(cos0,cos1);
					cosm=YsGreater(cosm,cos2);
					cosm=YsGreater(cosm,cos3);

					cosm=2.0-cosm;  // 1.0 Redundancy

					if(ref<=cosm)
					{
						id1=i;
						id2=j;
						ref=cosm;
					}
				}
				else if(strategy==YSCONVEXNIZEFASTEST)
				{
					id1=i;
					id2=j;
					return YSOK;
				}
			}
			else if(target->dat.plg[i]==target->dat.plg[j])   // 2005/02/08  (*) Should compare plg.  Not prj.
			{                                                 // 2005/02/08
				id1=i;                                        // 2005/02/08
				id2=j;                                        // 2005/02/08
				return YSOK;                                  // 2005/02/08
			}                                                 // 2005/02/08
		}
	}

	if(id1>=0 && id2>=0 && id1!=id2)
	{
		return YSOK;
	}

	return YSERR;
}

