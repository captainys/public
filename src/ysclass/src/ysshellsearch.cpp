/* ////////////////////////////////////////////////////////////

File Name: ysshellsearch.cpp
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

////////////////////////////////////////////////////////////

int YsShellVtxToVtxTable::nAlloc=0;

YsShellVtxToVtxTable::YsShellVtxToVtxTable(int hashSize) : YsHashTable <YsShellVertexHandle> (hashSize)
{
	tableName="Vtx Table";
	nAlloc++;
}

YsShellVtxToVtxTable::~YsShellVtxToVtxTable()
{
	nAlloc--;
}

YSRESULT YsShellVtxToVtxTable::Prepare(void)
{
	return PrepareTable();
}

YSRESULT YsShellVtxToVtxTable::Resize(int newHashSize)
{
	return YsHashTable <YsShellVertexHandle>::Resize(newHashSize);
}

YSRESULT YsShellVtxToVtxTable::AddVertex(const YsShell &shl,YsShellVertexHandle vtHd)
{
	unsigned searchKey;
	searchKey=shl.GetSearchKey(vtHd);
	return AddElement(searchKey,vtHd);
}

YSRESULT YsShellVtxToVtxTable::DeleteVertex(const YsShell &shl,YsShellVertexHandle vtHd)
{
	unsigned searchKey;
	searchKey=shl.GetSearchKey(vtHd);
	return DeleteElement(searchKey,vtHd);
}

YsShellVertexHandle YsShellVtxToVtxTable::FindVertex(const YsShell &,unsigned searchKey) const
{
	YsShellVertexHandle vtHd;
	if(FindElement(vtHd,searchKey)==YSOK)
	{
		return vtHd;
	}
	else
	{
		return NULL;
	}
}

YSRESULT YsShellVtxToVtxTable::SelfDiagnostic(const YsShell &shl) const
{
	YSRESULT res;

	res=YSOK;

	YsPrintf("**** Self Diagnostics Vertex Search Key to Vertex Handle Table ****\n");
	if(YsHashTable <YsShellVertexHandle>::SelfDiagnostic()!=YSOK)
	{
		res=YSERR;
	}

	YsPrintf("Inverse Check (Shell->Table)\n");
	int nVtx,i;
	YsShellVertexHandle vtHd;
	YsLoopCounter ctr;

	vtHd=NULL;
	nVtx=shl.GetNumVertex();
	i=0;
	ctr.BeginCounter(0);
	while((vtHd=shl.FindNextVertex(vtHd))!=NULL)
	{
		ctr.Increment();

		unsigned searchKey;
		YsShellVertexHandle foundVtHd;

		searchKey=shl.GetSearchKey(vtHd);
		foundVtHd=FindVertex(shl,searchKey);

		if(foundVtHd!=vtHd)
		{
			YsPrintf("  Cannot find correct vertex Key=%d\n",searchKey);
			res=YSERR;
		}

		i++;
	}
	ctr.EndCounter();

	YsPrintf("Done\n");

	return res;
}

////////////////////////////////////////////////////////////

int YsShellPlgToPlgTable::nAlloc=0;

YsShellPlgToPlgTable::YsShellPlgToPlgTable(int hashSize) : YsHashTable <YsShellPolygonHandle>(hashSize)
{
	tableName="Plg Table";
	nAlloc++;
}

YsShellPlgToPlgTable::~YsShellPlgToPlgTable()
{
	nAlloc--;
}

YSRESULT YsShellPlgToPlgTable::Prepare(void)
{
	return PrepareTable();
}

YSRESULT YsShellPlgToPlgTable::Resize(int newHashSize)
{
	return YsHashTable <YsShellPolygonHandle>::Resize(newHashSize);
}

YSRESULT YsShellPlgToPlgTable::AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd)
{
	unsigned searchKey;
	searchKey=shl.GetSearchKey(plHd);
	return AddElement(searchKey,plHd);
}

YSRESULT YsShellPlgToPlgTable::DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd)
{
	unsigned searchKey;
	searchKey=shl.GetSearchKey(plHd);
	return DeleteElement(searchKey,plHd);
}

YsShellPolygonHandle YsShellPlgToPlgTable::FindPolygon(const YsShell &,unsigned searchKey) const
{
	YsShellPolygonHandle plHd;
	if(FindElement(plHd,searchKey)==YSOK)
	{
		return plHd;
	}
	else
	{
		return NULL;
	}
}

YSRESULT YsShellPlgToPlgTable::SelfDiagnostic(const YsShell &shl) const
{
	YSRESULT res;
	res=YSOK;

	YsPrintf("**** Self Diagnostics Polygon Search Key to Polygon Handle Table ****\n");
	if(YsHashTable <YsShellPolygonHandle>::SelfDiagnostic()!=YSOK)
	{
		res=YSERR;
	}

	YsPrintf("Inverse Check (Shell->Table)\n");
	int nPlg,i;
	YsShellPolygonHandle plHd;
	YsLoopCounter ctr;

	plHd=NULL;
	nPlg=shl.GetNumPolygon();
	i=0;
	ctr.BeginCounter(nPlg);
	while((plHd=shl.FindNextPolygon(plHd))!=NULL)
	{
		ctr.Increment();

		unsigned searchKey;
		YsShellPolygonHandle foundPlHd;

		searchKey=shl.GetSearchKey(plHd);
		foundPlHd=FindPolygon(shl,searchKey);

		if(foundPlHd!=plHd)
		{
			YsPrintf("  Cannot find correct polygon Key=%d\n",searchKey);
			res=YSERR;
		}

		i++;
	}
	ctr.EndCounter();

	YsPrintf("Done\n");

	return res;
}


////////////////////////////////////////////////////////////

int YsShellVtxToPlgTableEntry::nAlloc=0;
int YsShellVtxToPlgTable::nAlloc=0;

YsShellVtxToPlgTable::YsShellVtxToPlgTable(int hashSize) : YsHashTable <YsShellVtxToPlgTableEntry *>(hashSize)
{
	tableName="Vtx to Plg Table";
	nAlloc++;
}

YsShellVtxToPlgTable::~YsShellVtxToPlgTable()
{
	CleanUp();
	nAlloc--;
}

YSRESULT YsShellVtxToPlgTable::Prepare(void)
{
	CleanUp();
	YsHashTable <YsShellVtxToPlgTableEntry *>::PrepareTable();
	return YSOK;
}

YSRESULT YsShellVtxToPlgTable::Resize(int newHashSize)
{
	return YsHashTable <YsShellVtxToPlgTableEntry *>::Resize(newHashSize);
}

YSRESULT YsShellVtxToPlgTable::AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd)
{
	int i,nVtHd;
	const YsShellVertexHandle *vtHd;

	nVtHd=shl.GetNumVertexOfPolygon(plHd);
	vtHd=shl.GetVertexListOfPolygon(plHd);

	for(i=0; i<nVtHd; i++)
	{
		unsigned searchKey;
		YsShellVtxToPlgTableEntry *group;

		searchKey=shl.GetSearchKey(vtHd[i]);
		if(FindElement(group,searchKey)==YSOK)
		{
			group->plHdList.AppendItem(plHd);
		}
		else
		{
			group=new YsShellVtxToPlgTableEntry;
			if(group==NULL)
			{
				YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShellVtxToPlgTable::AddPolygon()");
				return YSERR;
			}

			group->plHdList.AppendItem(plHd);
			if(AddElement(searchKey,group)!=YSOK)
			{
				YsErrOut("YsShellVtxToPlgTable::AddPolygon()\n");
				YsErrOut("  Cannot add element\n");
			}
		}
	}
	return YSOK;
}

YSRESULT YsShellVtxToPlgTable::DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd)
{
	int i,j,nVtHd;
	const YsShellVertexHandle *vtHd;

	nVtHd=shl.GetNumVertexOfPolygon(plHd);
	vtHd=shl.GetVertexListOfPolygon(plHd);

	for(i=0; i<nVtHd; i++)
	{
		unsigned searchKey;
		YsShellVtxToPlgTableEntry *group;

		searchKey=shl.GetSearchKey(vtHd[i]);
		if(FindElement(group,searchKey)==YSOK)
		{
			for(j=0; j<group->plHdList.GetN(); j++)
			{
				if(group->plHdList[j]==plHd)
				{
					group->plHdList.Delete(j);
					if(group->plHdList.GetN()==0)
					{
						DeleteElement(searchKey,group);
						delete group;
					}
					break;
				}
			}
		}
		else
		{
			YsErrOut("YsShellVtxToPlgTable::DeletePolygon()\n");
			YsErrOut("  Cannot find an entry\n");
			return YSERR;
		}
	}
	return YSOK;
}

#ifndef YSLOWMEM
const YsArray <YsShellPolygonHandle> *YsShellVtxToPlgTable::FindPolygonList
    (const YsShell &shl,YsShellVertexHandle vtHd) const
#else
const YsArray <YsShellPolygonHandle,1,short> *YsShellVtxToPlgTable::FindPolygonList
    (const YsShell &shl,YsShellVertexHandle vtHd) const
#endif
{
	unsigned searchKey;
	YsShellVtxToPlgTableEntry *group;

	searchKey=shl.GetSearchKey(vtHd);
	if(FindElement(group,searchKey)==YSOK)
	{
		return &group->plHdList;
	}
	else
	{
		return NULL;
	}
}

YSRESULT YsShellVtxToPlgTable::SelfDiagnostic(const YsShell &shl) const
{
	YSRESULT res;
	res=YSOK;

	YsErrOut("**** Self Diagnostics Vertex To Polygon List Table ****\n");
	if(YsHashTable <YsShellVtxToPlgTableEntry *>::SelfDiagnostic()!=YSOK)
	{
		res=YSERR;
	}

	int nVtx,nPlg,i,j,k;
	YsShellVertexHandle vtHd;
	YsShellPolygonHandle plHd;
	int nVtHdPlg;
	const YsShellVertexHandle *vtHdPlg;
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle> *plHdList;
#else
	const YsArray <YsShellPolygonHandle,1,short> *plHdList;
#endif
	YsLoopCounter ctr;

	YsPrintf("Vertex to Polygon\n");

	nVtx=shl.GetNumVertex();
	i=0;
	vtHd=NULL;
	ctr.BeginCounter(nVtx);
	while((vtHd=shl.FindNextVertex(vtHd))!=NULL)
	{
		ctr.Increment();

		plHdList=FindPolygonList(shl,vtHd);
		if(plHdList!=NULL && plHdList->GetN()>0)
		{
			YsShellPolygonHandle plHd;
			YSBOOL found;
			int a,b;
			for(a=0; a<plHdList->GetN(); a++)
			{
				plHd=(*plHdList)[a];
				nVtHdPlg=shl.GetNumVertexOfPolygon(plHd);
				vtHdPlg=shl.GetVertexListOfPolygon(plHd);

				found=YSFALSE;
				for(b=0; b<nVtHdPlg; b++)
				{
					if(vtHdPlg[b]==vtHd)
					{
						found=YSTRUE;
						break;
					}
				}
				if(found!=YSTRUE)
				{
					YsPrintf("  The vertex and polygon mismatch.\n");
					res=YSERR;
				}
			}
		}
		else if(shl.GetVertexReferenceCount(vtHd)>0)
		{
			YsPrintf("  The vertex is used, but no polygon is found that is using the vertex.\n");
			res=YSERR;
		}

		i++;
	}
	ctr.EndCounter();


	YsPrintf("Polygon to Vertex (must find polygon itself)\n");

	nPlg=shl.GetNumPolygon();
	i=0;
	plHd=NULL;
	ctr.BeginCounter(nPlg);
	while((plHd=shl.FindNextPolygon(plHd))!=NULL)
	{
		ctr.Increment();

		nVtHdPlg=shl.GetNumVertexOfPolygon(plHd);
		vtHdPlg=shl.GetVertexListOfPolygon(plHd);

		for(j=0; j<nVtHdPlg; j++)
		{
			plHdList=FindPolygonList(shl,vtHdPlg[j]);
			if(plHdList!=NULL && plHdList->GetN()>0)
			{
				for(k=0; k<plHdList->GetN(); k++)
				{
					if((*plHdList)[k]==plHd)
					{
						goto NEXTPOLYGON;
					}
				}
			}
			else
			{
				printf("  Cannot find itself!\n");
			}
		}

	NEXTPOLYGON:
		i++;
	}
	ctr.EndCounter();
	YsPrintf("Done\n");

	return res;
}

YSRESULT YsShellVtxToPlgTable::CleanUp(void)
{
	YsHashElementEnumHandle handle;
	YsShellVtxToPlgTableEntry *elem;

	RewindElementEnumHandle(handle);
	for(;;) // Visual C++ gives a warning for "while(1)"
	{
		if(GetElement(elem,handle)==YSOK)
		{
			delete elem;
		}

		if(FindNextElement(handle)!=YSOK)
		{
			break;
		}
	}

	return YSOK;
}

////////////////////////////////////////////////////////////

int YsShellEdgToPlgTablePolygonList::nAlloc=0;
int YsShellEdgToPlgTableEdgeList::nAlloc=0;
int YsShellEdgToPlgTable::nAlloc=0;

YsShellEdgToPlgTableEdgeList::YsShellEdgToPlgTableEdgeList(YsListAllocator <YsShellEdgToPlgTablePolygonList> &edgeListAllocator) : plgGroupList(edgeListAllocator)
{
	nAlloc++;
}

YsShellEdgToPlgTableEdgeList::~YsShellEdgToPlgTableEdgeList()
{
	plgGroupList.CleanUp();
	nAlloc--;
}

YsShellEdgToPlgTable::YsShellEdgToPlgTable(int hashSize) : YsHashTable <YsShellEdgToPlgTableEdgeList *> (hashSize), edgeListAllocator(16)
{
	head=NULL;
	tail=NULL;
	nEdge=0;
	nAlloc++;

	tableName="Edge to Plg Table";
}

YsShellEdgToPlgTable::~YsShellEdgToPlgTable()
{
	CleanUp();
	edgeListAllocator.CollectGarbage();
	nAlloc--;
}

YSRESULT YsShellEdgToPlgTable::Prepare(void)
{
	CleanUp();
	YsHashTable <YsShellEdgToPlgTableEdgeList *>::PrepareTable();
	return YSOK;
}

YSRESULT YsShellEdgToPlgTable::Resize(int newHashSize)
{
	return YsHashTable <YsShellEdgToPlgTableEdgeList *>::Resize(newHashSize);
}

YSRESULT YsShellEdgToPlgTable::AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd)
{
	int i,nVtHd;
	const YsShellVertexHandle *vtHd;

	YsShellVertexHandle edVtHd1,edVtHd2;
	unsigned edVtKey1,edVtKey2,edVtKeySum;;

	nVtHd=shl.GetNumVertexOfPolygon(plHd);
	vtHd=shl.GetVertexListOfPolygon(plHd);

	edVtHd2=vtHd[nVtHd-1]; // 2005/06/22

	for(i=0; i<nVtHd; i++)
	{
		edVtHd1=edVtHd2;   // 2005/06/22 Old -> vtHd[i];
		edVtHd2=vtHd[i];   // 2005/06/22 Old -> vtHd[(i+1)%nVtHd];

		edVtKey1=shl.GetSearchKey(edVtHd1);
		edVtKey2=shl.GetSearchKey(edVtHd2);
		edVtKeySum=edVtKey1+edVtKey2;
		edVtKeySum+=(YsSmaller(edVtKey1,edVtKey2)&1023); // 2009/07/30 Hopefully improves the efficiency.

		YsShellEdgToPlgTableEdgeList *edgeGroup;
		if(FindElement(edgeGroup,edVtKeySum)==YSOK)
		{
			YsListItem <YsShellEdgToPlgTablePolygonList> *seek;

			seek=NULL;
			while((seek=edgeGroup->plgGroupList.FindNext(seek))!=NULL)
			{
				if((seek->dat.edVtHd1==edVtHd1 && seek->dat.edVtHd2==edVtHd2) ||
				   (seek->dat.edVtHd1==edVtHd2 && seek->dat.edVtHd2==edVtHd1))
				{
					seek->dat.plgList.Append(plHd);
					goto NEXTEDGE;
				}
			}
		}
		else
		{
			edgeGroup=new YsShellEdgToPlgTableEdgeList(edgeListAllocator);
			if(edgeGroup==NULL)
			{
				YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShellEdgToPlgTable::AddPolygon()\n");
				return YSERR;
			}

			if(AddElement(edVtKeySum,edgeGroup)!=YSOK)
			{
				YsErrOut("YsShellEdgToPlgTable::AddPolygon()\n");
				YsErrOut("  Cannot add an element\n");
				return YSERR;
			}
		}

		YsListItem <YsShellEdgToPlgTablePolygonList> *neo;
		neo=edgeGroup->plgGroupList.Create();
		neo->dat.edVtHd1=edVtHd1;
		neo->dat.edVtHd2=edVtHd2;
		neo->dat.plgList.Set(0,NULL);
		neo->dat.plgList.Append(plHd);

		nEdge++;
		neo->dat.next=NULL;
		neo->dat.prev=tail;
		if(tail!=NULL)
		{
			tail->next=&neo->dat;
			tail=&neo->dat;
		}
		else
		{
			head=&neo->dat;
			tail=&neo->dat;
		}

	NEXTEDGE:
		;
	}
	return YSOK;
}

YSRESULT YsShellEdgToPlgTable::DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd)
{
	int i,nVtHd;
	const YsShellVertexHandle *vtHd;

	YsShellVertexHandle edVtHd1,edVtHd2;
	unsigned edVtKey1,edVtKey2,edVtKeySum;;

	nVtHd=shl.GetNumVertexOfPolygon(plHd);
	vtHd=shl.GetVertexListOfPolygon(plHd);

	edVtHd2=vtHd[nVtHd-1]; // 2005/06/22

	for(i=0; i<nVtHd; i++)
	{
		edVtHd1=edVtHd2;   // 2005/06/22 Old -> vtHd[i];
		edVtHd2=vtHd[i];   // 2005/06/22 Old -> vtHd[(i+1)%nVtHd];

		edVtKey1=shl.GetSearchKey(edVtHd1);
		edVtKey2=shl.GetSearchKey(edVtHd2);
		edVtKeySum=edVtKey1+edVtKey2;
		edVtKeySum+=(YsSmaller(edVtKey1,edVtKey2)&1023); // 2009/07/30 Hopefully improves the efficiency.

		YsShellEdgToPlgTableEdgeList *edgeGroup;
		if(FindElement(edgeGroup,edVtKeySum)==YSOK)
		{
			YsListItem <YsShellEdgToPlgTablePolygonList> *seek;

			seek=NULL;
			while((seek=edgeGroup->plgGroupList.FindNext(seek))!=NULL)
			{
				if((seek->dat.edVtHd1==edVtHd1 && seek->dat.edVtHd2==edVtHd2) ||
				   (seek->dat.edVtHd1==edVtHd2 && seek->dat.edVtHd2==edVtHd1))
				{
					int a;

					for(a=0; a<seek->dat.plgList.GetN(); a++)
					{
						if(seek->dat.plgList[a]==plHd)
						{
							seek->dat.plgList.Delete(a);
							if(seek->dat.plgList.GetN()==0)
							{
								nEdge--;
								if(seek->dat.prev!=NULL)
								{
									seek->dat.prev->next=seek->dat.next;
								}
								if(seek->dat.next!=NULL)
								{
									seek->dat.next->prev=seek->dat.prev;
								}
								if(seek->dat.prev==NULL)
								{
									head=seek->dat.next;
								}
								if(seek->dat.next==NULL)
								{
									tail=seek->dat.prev;
								}


								edgeGroup->plgGroupList.Delete(seek);
								if(edgeGroup->plgGroupList.GetN()==0)
								{
									DeleteElement(edVtKeySum,edgeGroup);
									delete edgeGroup;
								}
							}
							goto NEXTEDGE;
						}
					}
					YsErrOut("YsShellEdgToPlgTable::DeletePolygon()\n");
					YsErrOut("  Cannot find a polygon to be deleted(0) %d %d\n",a,i);
					for(a=0; a<seek->dat.plgList.GetN(); a++)
					{
						YsErrOut("LIST %d\n",shl.GetSearchKey(seek->dat.plgList[a]));
					}
					YsErrOut("WANT %d\n",shl.GetSearchKey(plHd));
					return YSERR;
				}
			}
			YsErrOut("YsShellEdgToPlgTable::DeletePolygon()\n");
			YsErrOut("  Cannot find a polygon to be deleted(1)\n");
			return YSERR;
		}
		else
		{
			YsErrOut("YsShellEdgToPlgTable::DeletePolygon()\n");
			YsErrOut("  Cannot find a polygon to be deleted(2)\n");
			return YSERR;
		}

	NEXTEDGE:
		;
	}
	return YSOK;
}

#ifndef YSLOWMEM
const YsArray <YsShellPolygonHandle,2> *YsShellEdgToPlgTable::GetEdgToPlgTable
    (const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2) const
#else
const YsArray <YsShellPolygonHandle,2,short> *YsShellEdgToPlgTable::GetEdgToPlgTable
    (const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2) const
#endif
{
	unsigned edVtKey1,edVtKey2;
	edVtKey1=shl.GetSearchKey(vtHd1);
	edVtKey2=shl.GetSearchKey(vtHd2);
	return GetEdgToPlgTable(shl,edVtKey1,edVtKey2);
}

#ifndef YSLOWMEM
const YsArray <YsShellPolygonHandle,2> *YsShellEdgToPlgTable::GetEdgToPlgTable
	    (const YsShell &shl,unsigned edVtKey1,unsigned edVtKey2) const
#else
const YsArray <YsShellPolygonHandle,2,short> *YsShellEdgToPlgTable::GetEdgToPlgTable
	    (const YsShell &shl,unsigned edVtKey1,unsigned edVtKey2) const
#endif
{
	unsigned edVtKeySum;

	edVtKeySum=edVtKey1+edVtKey2;
	edVtKeySum+=(YsSmaller(edVtKey1,edVtKey2)&1023); // 2009/07/30 Hopefully improves the efficiency.

	YsShellEdgToPlgTableEdgeList *edgeGroup;
	if(FindElement(edgeGroup,edVtKeySum)==YSOK)
	{
		YsListItem <YsShellEdgToPlgTablePolygonList> *seek;

		seek=NULL;
		while((seek=edgeGroup->plgGroupList.FindNext(seek))!=NULL)
		{
			if((shl.GetSearchKey(seek->dat.edVtHd1)==edVtKey1 && shl.GetSearchKey(seek->dat.edVtHd2)==edVtKey2) ||
			   (shl.GetSearchKey(seek->dat.edVtHd1)==edVtKey2 && shl.GetSearchKey(seek->dat.edVtHd2)==edVtKey1))
			{
				return &seek->dat.plgList;
			}
		}
	}

	return NULL;
}

#ifndef YSLOWMEM
const YsArray <YsShellPolygonHandle,2> *YsShellEdgToPlgTable::GetEdgToPlgTable
	    (const YsShell &,const YsShellEdgeEnumHandle &handle) const
#else
const YsArray <YsShellPolygonHandle,2,short> *YsShellEdgToPlgTable::GetEdgToPlgTable
	    (const YsShell &,const YsShellEdgeEnumHandle &handle) const
#endif
{
	if(handle!=NULL)
	{
		return &handle->plgList;
	}
	else
	{
		return NULL;
	}
}

YSRESULT YsShellEdgToPlgTable::SelfDiagnostic(const YsShell &shl) const
{
	YSRESULT res;
	res=YSOK;

	YsPrintf("**** Self Diagnostics Edge To Polygon List Table ****\n");
	if(YsHashTable <YsShellEdgToPlgTableEdgeList *>::SelfDiagnostic()!=YSOK)
	{
		res=YSERR;
	}

	YsShellPolygonHandle plHd;
	YsShellVertexHandle edVt1,edVt2;
	int nPlVtHd;
	const YsShellVertexHandle *plVtHd;
	int i,j,k,nPlg;
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *plgList;
#else
	const YsArray <YsShellPolygonHandle,2,short> *plgList;
#endif
	YsLoopCounter ctr;

	YsPrintf("  Checking Polygon->Edge->Polygon\n");

	nPlg=shl.GetNumPolygon();
	i=0;
	plHd=NULL;
	ctr.BeginCounter(nPlg);
	while((plHd=shl.FindNextPolygon(plHd))!=NULL)
	{
		ctr.Increment();

		nPlVtHd=shl.GetNumVertexOfPolygon(plHd);
		plVtHd=shl.GetVertexListOfPolygon(plHd);

		for(j=0; j<nPlVtHd; j++)
		{
			edVt1=plVtHd[j];
			edVt2=plVtHd[(j+1)%nPlVtHd];

			plgList=GetEdgToPlgTable(shl,edVt1,edVt2);
			if(plgList!=NULL && plgList->GetN()>0)
			{
				for(k=0; k<plgList->GetN(); k++)
				{
					if((*plgList)[k]==plHd)
					{
						goto NEXTPOLYGON;
					}
				}
				YsPrintf("  Cannot find polygon from its own edge(1)\n");
			}
			else
			{
				YsPrintf("  Cannot find polygon from its own edge(2)\n");
			}
		}

	NEXTPOLYGON:
		i++;
	}
	ctr.EndCounter();


	YsPrintf("  Checking Edge->Polygon->Edge\n");

	YsShellVertexHandle edVtHd1,edVtHd2;
	unsigned edVtKey1,edVtKey2;
	YsShellEdgeEnumHandle handle;
	RewindEdgeEnumHandle(handle);
	i=0;
	ctr.BeginCounter(0);
	for(;;) // Visual C++ gives a warning for "while(1)"
	{
		GetEdge(edVtHd1,edVtHd2,handle);
		edVtKey1=shl.GetSearchKey(edVtHd1);
		edVtKey2=shl.GetSearchKey(edVtHd2);

		ctr.Increment();

		plgList=GetEdgToPlgTable(shl,edVtKey1,edVtKey2);
		if(plgList!=NULL && plgList->GetN()>0)
		{
			for(j=0; j<plgList->GetN(); j++)
			{
				YSBOOL found;
				plHd=(*plgList)[j];
				nPlVtHd=shl.GetNumVertexOfPolygon(plHd);
				plVtHd=shl.GetVertexListOfPolygon(plHd);

				found=YSFALSE;
				for(k=0; k<nPlVtHd; k++)
				{
					unsigned plEdVtKey1,plEdVtKey2;
					plEdVtKey1=shl.GetSearchKey(plVtHd[k]);
					plEdVtKey2=shl.GetSearchKey(plVtHd[(k+1)%nPlVtHd]);
					if((plEdVtKey1==edVtKey1 && plEdVtKey2==edVtKey2) ||
					   (plEdVtKey1==edVtKey2 && plEdVtKey2==edVtKey1))
					{
						found=YSTRUE;
						break;
					}
				}

				if(found!=YSTRUE)
				{
					YsPrintf("  Cannot find polygon from an edge(1)\n");
					res=YSERR;
				}
			}
		}
		else
		{
			YsPrintf("  Cannot find polygon from an edge(2)\n");
			res=YSERR;
		}

		i++;

		if(FindNextEdge(handle)!=YSOK)
		{
			break;
		}
	}

	ctr.EndCounter();
	YsPrintf("Done\n");
	return res;
}

int YsShellEdgToPlgTable::GetNumEdge(void) const
{
	return nEdge;
}


YSRESULT YsShellEdgToPlgTable::RewindEdgeEnumHandle(YsShellEdgeEnumHandle &handle) const
{
	handle=head;
	if(handle!=NULL)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShellEdgToPlgTable::FindNextEdge(YsShellEdgeEnumHandle &handle) const
{
	if(handle!=NULL)
	{
		handle=handle->next;
	}
	else
	{
		handle=head;
	}

	if(handle!=NULL)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShellEdgToPlgTable::GetEdge(YsShellVertexHandle &edVtHd1,YsShellVertexHandle &edVtHd2,YsShellEdgeEnumHandle handle) const
{
	if(handle!=NULL)
	{
		edVtHd1=handle->edVtHd1;
		edVtHd2=handle->edVtHd2;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellEdgToPlgTable::CleanUp(void)
{
	YsHashElementEnumHandle handle;
	YsShellEdgToPlgTableEdgeList *elem;

	RewindElementEnumHandle(handle);
	for(;;) // Visual C++ gives a warning for "while(1)"
	{
		if(GetElement(elem,handle)==YSOK)
		{
			delete elem;
		}

		if(FindNextElement(handle)!=YSOK)
		{
			break;
		}
	}

	nEdge=0;
	head=NULL;
	tail=NULL;

	return YSOK;
}

////////////////////////////////////////////////////////////

// static YsListAllocator <YsShellVtxLstToPlgTablePolygonGroup> vtxLstToPlgTableAllocator(65535);
// 
// YsShellVtxLstToPlgTableGroupList::YsShellVtxLstToPlgTableGroupList() : plgGroupList(vtxLstToPlgTableAllocator)
// {
// }
// 
// YsShellVtxLstToPlgTableGroupList::~YsShellVtxLstToPlgTableGroupList()
// {
// 	plgGroupList.CleanUp();
// }
// 
// 
// 
// YsShellVtxLstToPlgTable::YsShellVtxLstToPlgTable(int hashSize) : YsHashTable <YsShellVtxLstToPlgTableGroupList *> (ha// shSize)
// {
// 	overlapWarning=YSFALSE;
//    	tableName="Multi Vtx to Plg Table";
// }
// 
// YsShellVtxLstToPlgTable::~YsShellVtxLstToPlgTable()
// {
// 	CleanUp();
// }
// 
// YSRESULT YsShellVtxLstToPlgTable::Prepare(void)
// {
// 	CleanUp();
// 	PrepareTable();
// 	overlapWarning=YSFALSE;
// 	return YSOK;
// }
// 
// YSRESULT YsShellVtxLstToPlgTable::Resize(int newHashSize)
// {
// 	YsHashTable <YsShellVtxLstToPlgTableGroupList *>::Resize(newHashSize);
// 	return YSOK;
// }
// 
// YSRESULT YsShellVtxLstToPlgTable::AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd)
// {
// 	unsigned searchKey;
// 	YsArray <unsigned,4> keyArray;
// 	YSBOOL watch;
// 	watch=YSFALSE;
// 
// 	if(watch==YSTRUE)
// 	{
// 		printf("Watch!\n");
// 	}
// 
// 	searchKey=GetVertexKeyArray(keyArray,shl,plHd);
// 
// 	if(watch==YSTRUE)
// 	{
// 		int i;
// 		for(i=0; i<keyArray.GetN(); i++)
// 		{
// 			printf("[%d]",keyArray[i]);
// 		}
// 		printf("\n");
// 	}
// 
// 
// 	YsShellVtxLstToPlgTableGroupList *plgGroupList;
// 
// 	if(FindElement(plgGroupList,searchKey)==YSOK)
// 	{
// 		YsListItem <YsShellVtxLstToPlgTablePolygonGroup> *seek;
// 		seek=NULL;
// 		while((seek=plgGroupList->plgGroupList.FindNext(seek))!=NULL)
// 		{
// 			if(TwoPolygonsAreEqual(keyArray,seek->dat.plVtKey)==YSTRUE)
// 			{
// 				seek->dat.plgList.AppendItem(plHd);
// 				overlapWarning=YSTRUE;
// 				return YSOK;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		plgGroupList=new YsShellVtxLstToPlgTableGroupList;
// 		if(plgGroupList==NULL)
// 		{
// 			YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShellVtxLstToPlgTable::AddPolygon()");
// 			return YSERR;
// 		}
// 
// 		if(AddElement(searchKey,plgGroupList)!=YSOK)
// 		{
// 			YsErrOut("YsShellVtxLstToPlgTable::AddPolygon()\n");
// 			YsErrOut("  Cannot add an element!\n");
// 			return YSERR;
// 		}
// 	}
// 
// 	YsListItem <YsShellVtxLstToPlgTablePolygonGroup> *neo;
// 	neo=plgGroupList->plgGroupList.Create();
// 	neo->dat.plVtKey=keyArray;
// 	neo->dat.plgList.Set(0,NULL);
// 	neo->dat.plgList.Append(plHd);
// 
// 	return YSOK;
// }
// 
// YSRESULT YsShellVtxLstToPlgTable::DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd)
// {
// 	int i;
// 	unsigned searchKey;
// 	YsArray <unsigned,4> keyArray;
// 
// 	searchKey=GetVertexKeyArray(keyArray,shl,plHd);
// 
// 	YsShellVtxLstToPlgTableGroupList *plgGroupList;
// 
// 	if(FindElement(plgGroupList,searchKey)==YSOK)
// 	{
// 		YsListItem <YsShellVtxLstToPlgTablePolygonGroup> *seek;
// 		seek=NULL;
// 		while((seek=plgGroupList->plgGroupList.FindNext(seek))!=NULL)
// 		{
// 			for(i=0; i<seek->dat.plgList.GetN(); i++)
// 			{
// 				if(seek->dat.plgList[i]==plHd)
// 				{
// 					seek->dat.plgList.Delete(i);
// 					if(seek->dat.plgList.GetN()==0)
// 					{
// 						plgGroupList->plgGroupList.Delete(seek);
// 						if(plgGroupList->plgGroupList.GetN()==0)
// 						{
// 							DeleteElement(searchKey,plgGroupList);
// 							delete plgGroupList;  // 2002/08/12 It's a big leak, isn't it?
// 						}
// 					}
// 					return YSOK;
// 				}
// 			}
// 		}
// 		YsErrOut("YsShellVtxLstToPlgTable::DeletePolygon()\n");
// 		YsErrOut("  Element is found, but polygon is not found..\n");
// 		return YSERR;
// 	}
// 
// 	YsErrOut("YsShellVtxLstToPlgTable::DeletePolygon()\n");
// 	YsErrOut("  Cannot find an element.\n");
// 
// 	return YSERR;
// }
// 
// const YsArray <YsShellPolygonHandle> *YsShellVtxLstToPlgTable::GetVtxLstToPlgTable
//     (const YsShell &shl,int nVtHd,const YsShellVertexHandle vtHd[]) const
// {
// 	YsArray <unsigned,4> keyList;
// 	int i;
// 	for(i=0; i<nVtHd; i++)
// 	{
// 		keyList.AppendItem(shl.GetSearchKey(vtHd[i]));
// 	}
// 	return GetVtxLstToPlgTable(shl,nVtHd,keyList.GetArray());
// }
// 
// const YsArray <YsShellPolygonHandle> *YsShellVtxLstToPlgTable::GetVtxLstToPlgTable
//     (const YsShell &shl,int nVtKey,const unsigned vtKey[]) const
// {
// 	int i;
// 	unsigned searchKey;
// 	YsArray <unsigned,4> keyArray;
// 
// 	searchKey=0;
// 	for(i=0; i<nVtKey; i++)
// 	{
// 		searchKey+=vtKey[i];
// 		keyArray.AppendItem(vtKey[i]);
// 	}
// 
// 	YsQuickSort <unsigned,unsigned> (keyArray.GetN(),keyArray.GetEditableArray(),NULL);
// 
// 	YsShellVtxLstToPlgTableGroupList *plgGroupList;
// 
// 	if(FindElement(plgGroupList,searchKey)==YSOK)
// 	{
// 		YsListItem <YsShellVtxLstToPlgTablePolygonGroup> *seek;
// 		seek=NULL;
// 		while((seek=plgGroupList->plgGroupList.FindNext(seek))!=NULL)
// 		{
// 			if(TwoPolygonsAreEqual(seek->dat.plVtKey,keyArray)==YSTRUE)
// 			{
// 				return &seek->dat.plgList;
// 			}
// 		}
// 	}
// 	return NULL;
// }
// 
// YSRESULT YsShellVtxLstToPlgTable::SelfDiagnostic(const YsShell &shl) const
// {
// 	YSRESULT res;
// 
// 	res=YSOK;
// 
// 	YsPrintf("**** Self Diagnostics Vertex List To Polygon Handle Table ****\n");
// 	if(YsHashTable <YsShellVtxLstToPlgTableGroupList *>::SelfDiagnostic()!=YSOK)
// 	{
// 		res=YSERR;
// 	}
// 
// 	YsShellPolygonHandle plHd;
// 	int i,j,nPlg;
// 	int nPlVt;
// 	const YsShellVertexHandle *plVt;
// 	const YsArray <YsShellPolygonHandle> *plgList;
// 
// 	plHd=NULL;
// 	i=0;
// 	nPlg=shl.GetNumPolygon();
// 
// 	YsPrintf("Checking Shell->Table\n");
// 
// 	while((plHd=shl.FindNextPolygon(plHd))!=NULL)
// 	{
// 		nPlVt=shl.GetNumVertexOfPolygon(plHd);
// 		plVt=shl.GetVertexListOfPolygon(plHd);
// 
// 		plgList=GetVtxLstToPlgTable(shl,nPlVt,plVt);
// 		if(plgList!=NULL && plgList->GetN()>0)
// 		{
// 			YSBOOL found;
// 			found=YSFALSE;
// 			for(j=0; j<plgList->GetN(); j++)
// 			{
// 				if((*plgList)[j]==plHd)
// 				{
// 					found=YSTRUE;
// 					break;
// 				}
// 			}
// 
// 			if(found!=YSTRUE)
// 			{
// 				YsPrintf("Polygon not found(1)\n");
// 			}
// 
// 			if(plgList->GetN()>1)
// 			{
// 				YsPrintf("Overlapping polygon exists.\n");
// 			}
// 		}
// 		else
// 		{
// 			if(plgList!=NULL)
// 			{
// 				YsPrintf("Polygon not found (2) %d\n",plgList->GetN());
// 			}
// 			else
// 			{
// 				YsPrintf("Polygon not found(3)\n");
// 			}
// 			res=YSERR;
// 		}
// 
// 		i++;
// 	}
// 
// 	YsPrintf("Done\n");
// 
// 	return res;
// }
// 
// 
// YSRESULT YsShellVtxLstToPlgTable::CleanUp(void)
// {
// 	YsHashElementEnumHandle handle;
// 	YsShellVtxLstToPlgTableGroupList *elem;
// 
// 
// 	RewindElementEnumHandle(handle);
// 	while(1)
// 	{
// 
// 		if(GetElement(elem,handle)==YSOK)
// 		{
// 			delete elem;
// 		}
// 
// 
// 		if(FindNextElement(handle)!=YSOK)
// 		{
// 			break;
// 		}
// 
// 	}
// 	return YSOK;
// }
// 
// unsigned YsShellVtxLstToPlgTable::GetVertexKeyArray
//     (YsArray <unsigned,4> &vtKey,const YsShell &shl,YsShellPolygonHandle plHd) const
// {
// 	int i,nVtHd;
// 	const YsShellVertexHandle *vtHd;
// 	unsigned searchKey;
// 
// 	nVtHd=shl.GetNumVertexOfPolygon(plHd);
// 	vtHd=shl.GetVertexListOfPolygon(plHd);
// 
// 	vtKey.Set(nVtHd,NULL);
// 
// 	searchKey=0;
// 	for(i=0; i<nVtHd; i++)
// 	{
// 		vtKey.SetItem(i,shl.GetSearchKey(vtHd[i]));
// 		searchKey+=vtKey[i];
// 	}
// 
// 	YsQuickSort <unsigned,unsigned>(vtKey.GetN(),vtKey.GetEditableArray(),NULL);
// 
// 	return searchKey;
// }
// 
// YSBOOL YsShellVtxLstToPlgTable::TwoPolygonsAreEqual
//     (const YsArray <unsigned,4> &key1,const YsArray <unsigned,4> &key2) const
// {
// 	int i;
// 
// 	if(key1.GetN()==key2.GetN())
// 	{
// 		for(i=0; i<key1.GetN(); i++)
// 		{
// 			if(key1[i]!=key2[i])
// 			{
// 				return YSFALSE;
// 			}
// 		}
// 		return YSTRUE;
// 	}
// 	else
// 	{
// 		return YSFALSE;
// 	}
// }

////////////////////////////////////////////////////////////

int YsShellVtxToEdgTableElem::nAlloc=0;
int YsShellVtxToEdgTable::nAlloc=0;

YsShellVtxToEdgTable::YsShellVtxToEdgTable(int hashSize) : YsFixedLengthToMultiHashTable <YsShellVtxToEdgTableElem,1,8> (hashSize)
{
	tableName="Vtx to Edge Table";
	nAlloc++;
}

YsShellVtxToEdgTable::~YsShellVtxToEdgTable()
{
	nAlloc--;
}

YSRESULT YsShellVtxToEdgTable::Prepare(void)
{
	return PrepareTable();
}

YSRESULT YsShellVtxToEdgTable::Resize(int newHashSize)
{
	return YsFixedLengthToMultiHashTable <YsShellVtxToEdgTableElem,1,8>::Resize(newHashSize);
}


YSRESULT YsShellVtxToEdgTable::SelfDiagnostic(const YsShell &shl) const
{
	int counter;
	YSRESULT err;
	YsShellVertexHandle vtHd;
	YsShellPolygonHandle plHd;
	YsLoopCounter ctr;

	err=YSOK;

	YsPrintf("**** Self Diagnostics Vertex To Edge Table ****\n");

	vtHd=NULL;
	YsPrintf("Checking Table Entries\n");
	counter=0;

	ctr.BeginCounter(shl.GetNumVertex());
	while((vtHd=shl.FindNextVertex(vtHd))!=NULL)
	{
		ctr.Increment();

		int i,j;
		unsigned vtKey;
		const YsArray <YsShellVtxToEdgTableElem,8> *edgeList;

		if(shl.GetVertex(vtHd)->GetReferenceCount()==0)
		{
			continue;
		}

		edgeList=FindEdge(shl,vtHd);
		vtKey=shl.GetSearchKey(vtHd);


		// Check all edges in the list exists
		for(i=0; i<edgeList->GetN(); i++)
		{
			YsShellVertexHandle edVtHd[2];
			edVtHd[0]=vtHd;
			edVtHd[1]=(*edgeList)[i].connVtHd;

			YSBOOL exist=YSFALSE;

			plHd=NULL;
			while((plHd=shl.FindNextPolygon(plHd))!=NULL)
			{
				int nPlVt;
				const YsShellVertexHandle *plVtHd;

				nPlVt=shl.GetNumVertexOfPolygon(plHd);
				plVtHd=shl.GetVertexListOfPolygon(plHd);

				exist=YSFALSE;
				for(j=0; j<nPlVt; j++)
				{
					if(YsSameEdge(edVtHd[0],edVtHd[1],plVtHd[j],plVtHd[(j+1)%nPlVt])==YSTRUE)
					{
						exist=YSTRUE;
						goto FOUND;
					}
				}
			}
		FOUND:
			if(YSTRUE!=exist)
			{
				YsPrintf("Vertex to Edge Table is corrupted (2)\n");
				YsPrintf("  VtKey=(%d,%d)\n",shl.GetSearchKey(edVtHd[0]),shl.GetSearchKey(edVtHd[1]));
				err=YSERR;
				break;
			}
		}

		counter++;
	}
	ctr.EndCounter();

	// Inverse check
	plHd=NULL;
	YsPrintf("Checking Actual Edge -> Table Entry\n");
	counter=0;
	ctr.BeginCounter(shl.GetNumPolygon());
	while((plHd=shl.FindNextPolygon(plHd))!=NULL)
	{
		ctr.Increment();

		const YsArray <YsShellVtxToEdgTableElem,8> *edgeList;
		int i,j,k,nPlVt;
		const YsShellVertexHandle *plVtHd;
		YsShellVertexHandle edVtHd[2];

		nPlVt=shl.GetNumVertexOfPolygon(plHd);
		plVtHd=shl.GetVertexListOfPolygon(plHd);

		for(i=0; i<nPlVt; i++)
		{
			edVtHd[0]=plVtHd[i];
			edVtHd[1]=plVtHd[(i+1)%nPlVt];

			for(k=0; k<1; k++)
			{
				YSBOOL exist;
				edgeList=FindEdge(shl,edVtHd[k]);

				exist=YSFALSE;
				for(j=0; j<edgeList->GetN(); j++)
				{
					if((*edgeList)[j].connVtHd==edVtHd[1-k])
					{
						exist=YSTRUE;
						break;
					}
				}

				if(exist!=YSTRUE)
				{
					YsPrintf("Vertex To Edge Table is corrupted (3)\n");
					err=YSERR;
					break;
				}
			}
		}
		counter++;
	}
	ctr.EndCounter();
	YsPrintf("Done.\n");

	return err;
}

YSRESULT YsShellVtxToEdgTable::AddEdge(const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2)
{
	unsigned vtKey1,vtKey2;
	const YsArray <YsShellVtxToEdgTableElem,8> *edLstVt1,*edLstVt2;

	vtKey1=shl.GetSearchKey(vtHd1);
	vtKey2=shl.GetSearchKey(vtHd2);

	edLstVt1=FindElement(1,&vtKey1);
	if(edLstVt1!=NULL)
	{
		int i;
		YSBOOL found1;
		YsShellVtxToEdgTableElem *elm;

		found1=YSFALSE;
		for(i=0; i<edLstVt1->GetN(); i++)
		{
			if((*edLstVt1)[i].connVtHd==vtHd2)
			{
				found1=YSTRUE;
				elm=(YsShellVtxToEdgTableElem *)&(*edLstVt1)[i];
				elm->refCount++;
				break;
			}
		}

		if(found1==YSTRUE)
		{
			edLstVt2=FindElement(1,&vtKey2);
			if(edLstVt2!=NULL)
			{
				for(i=0; i<edLstVt2->GetN(); i++)
				{
					if((*edLstVt2)[i].connVtHd==vtHd1)
					{
						elm=(YsShellVtxToEdgTableElem *)&(*edLstVt2)[i];
						elm->refCount++;
						return YSOK;
					}
				}
			}
			YsErrOut("YsShellVtxToEdgTable::AddEdge()\n");
			YsErrOut("  Detected a conflict in the table.\n");
			return YSERR;
		}
	}

	YsShellVtxToEdgTableElem newElem1,newElem2;
	newElem1.connVtHd=vtHd2;
	newElem1.refCount=1;

	newElem2.connVtHd=vtHd1;
	newElem2.refCount=1;

	if(AddElement(1,&vtKey1,newElem1)!=YSOK ||
	   AddElement(1,&vtKey2,newElem2)!=YSOK)
	{
		YsErrOut("YsShellVtxToEdgTable::AddEdge()\n");
		YsErrOut("  Cannot add a new element.\n");
		return YSERR;
	}

	return YSOK;
}

YSRESULT YsShellVtxToEdgTable::DeleteEdge(const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2)
{
	unsigned vtKey[2];
	YsShellVertexHandle edVtHd[2];
	const YsArray <YsShellVtxToEdgTableElem,8> *edLst[2];

	edVtHd[0]=vtHd1;
	edVtHd[1]=vtHd2;

	vtKey[0]=shl.GetSearchKey(vtHd1);
	vtKey[1]=shl.GetSearchKey(vtHd2);

	edLst[0]=FindElement(1,&vtKey[0]);
	edLst[1]=FindElement(1,&vtKey[1]);
	if(edLst[0]!=NULL && edLst[1]!=NULL)
	{
		int i,e;
		YSBOOL found[2];
		YsShellVtxToEdgTableElem *elm;

		found[0]=YSFALSE;
		found[1]=YSFALSE;

		for(e=0; e<2; e++)
		{
			for(i=0; i<edLst[e]->GetN(); i++)
			{
				if((*edLst[e])[i].connVtHd==edVtHd[1-e])
				{
					found[e]=YSTRUE;
					elm=(YsShellVtxToEdgTableElem *)&(*edLst[e])[i];
					elm->refCount--;
					if(elm->refCount==0)
					{
						YsShellVtxToEdgTableElem x;   // Copy *elm to x for safety.
						x=*elm;
						DeleteElement(1,&vtKey[e],x);

						// 2005/02/10
						// DeleteElement may change the pointer edLst[1-e].
						// Thus it must be refreshed.
						if(e==0)
						{
							edLst[1]=FindElement(1,&vtKey[1]);
							if(edLst[1]==NULL)
							{
								YsErrOut("YsShellVtxToEdgTable::DeleteEdge()\n");
								YsErrOut("  Vertex to edge table has inconsistency.\n");
								return YSERR;
							}
						}
					}
					break;
				}
			}
		}

		if(found[0]!=YSTRUE || found[1]!=YSTRUE)
		{
			return YSERR;
		}
		return YSOK;
	}

	return YSERR;
}

const YsArray <YsShellVtxToEdgTableElem,8> *YsShellVtxToEdgTable::FindEdge(const YsShell &shl,YsShellVertexHandle vtHd) const
{
	unsigned vtKey;
	vtKey=shl.GetSearchKey(vtHd);
	return FindElement(1,&vtKey);
}

////////////////////////////////////////////////////////////

YsShellSearchTable::YsShellSearchTable() :
   	vtxToVtx(1024),
   	vtxToEdg(1024),
   	plgToPlg(1024),
   	vtxToPlg(1024),
   	edgToPlg(1024)
//    	vtxLstToPlg(1024)
{
	attachedShell=NULL;
	EnableAutoResizing();
}

YsShellSearchTable::YsShellSearchTable(int hashSize) :
   	vtxToVtx(hashSize),
   	vtxToEdg(hashSize),
   	plgToPlg(hashSize),
   	vtxToPlg(hashSize),
   	edgToPlg(hashSize)
//    	vtxLstToPlg(hashSize)
{
	attachedShell=NULL;
	EnableAutoResizing();
}

YsShellSearchTable::~YsShellSearchTable()
{
	if(NULL!=attachedShell)
	{
		attachedShell->DetachSearchTable();
	}
}

YSRESULT YsShellSearchTable::CollectGarbage(void)
{
//	edgToPlg.edgeListAllocator.CollectGarbage();
// 	vtxLstToPlgTableAllocator.CollectGarbage();
	return YSOK;
}

void YsShellSearchTable::SetShellPointer(const YsShell *shl)
{
	attachedShell=shl;
}

YSRESULT YsShellSearchTable::CleanUp(void)
{
	return Prepare();
}

YSRESULT YsShellSearchTable::Prepare(void)
{
	if(NULL==attachedShell)
	{
		vtxToVtx.Prepare();
		vtxToEdg.Prepare();
		plgToPlg.Prepare();
		vtxToPlg.Prepare();
		edgToPlg.Prepare();
		edgToPlg.edgeListAllocator.CollectGarbage();
	// 	vtxLstToPlg.Prepare();
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellSearchTable::ResizeForShell(const YsShell &shl)
{
	vtxToVtx.Resize(1+shl.GetNumVertex()/4);
	vtxToEdg.Resize(1+shl.GetNumVertex()/4);
	vtxToPlg.Resize(1+shl.GetNumVertex()/4);
	plgToPlg.Resize(1+shl.GetNumPolygon()/4);
	edgToPlg.Resize(1+shl.GetNumPolygon()*3/2); // Assuming triangle dominant
	return YSOK;
}

YSRESULT YsShellSearchTable::SelfDiagnostic(const YsShell &shl) const
{
	YSRESULT err;
	err=YSOK;
	if(vtxToVtx.SelfDiagnostic(shl)!=YSOK)
	{
		err=YSERR;
	}
	if(vtxToEdg.SelfDiagnostic(shl)!=YSOK)
	{
		err=YSERR;
	}
	if(plgToPlg.SelfDiagnostic(shl)!=YSOK)
	{
		err=YSERR;
	}
	if(vtxToPlg.SelfDiagnostic(shl)!=YSOK)
	{
		err=YSERR;
	}
	if(edgToPlg.SelfDiagnostic(shl)!=YSOK)
	{
		err=YSERR;
	}
// 	if(vtxLstToPlg.SelfDiagnostic(shl)!=YSOK)
// 	{
// 		err=YSERR;
// 	}
	return err;
}

void YsShellSearchTable::EnableAutoResizing(int minSize,int maxSize)
{
	vtxToVtx.EnableAutoResizing(minSize,maxSize);
	vtxToEdg.EnableAutoResizing(minSize,maxSize);
	plgToPlg.EnableAutoResizing(minSize,maxSize);
	vtxToPlg.EnableAutoResizing(minSize,maxSize);
	edgToPlg.EnableAutoResizing(minSize,maxSize);
// 	vtxLstToPlg.EnableAutoResizing(minSize,maxSize);
}

void YsShellSearchTable::EnableAutoResizing(void)
{
	vtxToVtx.EnableAutoResizing();
	vtxToEdg.EnableAutoResizing();
	plgToPlg.EnableAutoResizing();
	vtxToPlg.EnableAutoResizing();
	edgToPlg.EnableAutoResizing();
// 	vtxLstToPlg.EnableAutoResizing();
}


YSRESULT YsShellSearchTable::AddVertex(const YsShell &shl,YsShellVertexHandle vtHd)
{
	return vtxToVtx.AddVertex(shl,vtHd);
}

YSRESULT YsShellSearchTable::DeleteVertex(const YsShell &shl,YsShellVertexHandle vtHd)
{
	return vtxToVtx.DeleteVertex(shl,vtHd);
}

YsShellVertexHandle YsShellSearchTable::FindVertex(const YsShell &shl,unsigned searchKey) const
{
	return vtxToVtx.FindVertex(shl,searchKey);
}

YsShellPolygonHandle YsShellSearchTable::FindPolygon(const YsShell &shl,unsigned searchKey) const
{
	return plgToPlg.FindPolygon(shl,searchKey);
}

#ifndef YSLOWMEM
const YsArray <YsShellPolygonHandle> *YsShellSearchTable::FindPolygonListByVertex
    (const YsShell &shl,YsShellVertexHandle vtHd) const
#else
const YsArray <YsShellPolygonHandle,1,short> *YsShellSearchTable::FindPolygonListByVertex
    (const YsShell &shl,YsShellVertexHandle vtHd) const
#endif
{
	return vtxToPlg.FindPolygonList(shl,vtHd);
}

int YsShellSearchTable::GetNumPolygonUsingVertex(const YsShell &shl,YsShellVertexHandle vtHd) const
{
	int nPl;
	const YsShellPolygonHandle *plHdArray;
	if(YSOK==FindPolygonListByVertex(nPl,plHdArray,shl,vtHd))
	{
		return nPl;
	}
	return 0;
}


YSRESULT YsShellSearchTable::FindPolygonListByVertex
	    (int &nPl,const YsShellPolygonHandle *&plHdList,const YsShell &shl,YsShellVertexHandle vtHd) const
{
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle> *found;
#else
	const YsArray <YsShellPolygonHandle,1,short> *found;
#endif
	found=vtxToPlg.FindPolygonList(shl,vtHd);
	if(found!=NULL && found->GetN()>0)
	{
		nPl=(int)found->GetN();
		plHdList=(*found);
		return YSOK;
	}
	else
	{
		nPl=0;
		plHdList=NULL;
		return YSERR;
	}
}



#ifndef YSLOWMEM
const YsArray <YsShellPolygonHandle,2> *YsShellSearchTable::FindPolygonListByEdge
	    (const YsShell &shl,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2) const
#else
const YsArray <YsShellPolygonHandle,2,short> *YsShellSearchTable::FindPolygonListByEdge
	    (const YsShell &shl,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2) const
#endif
{
	return edgToPlg.GetEdgToPlgTable(shl,edVt1,edVt2);
}

YSRESULT YsShellSearchTable::FindPolygonListByEdge
    (int &nPl,const YsShellPolygonHandle *&plHdList,
     const YsShell &shl,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2) const
{
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *found;
#else
	const YsArray <YsShellPolygonHandle,2,short> *found;
#endif
	found=edgToPlg.GetEdgToPlgTable(shl,edVt1,edVt2);
	if(found!=NULL && found->GetN()>0)
	{
		nPl=(int)found->GetN();
		plHdList=(*found);
		return YSOK;
	}
	else
	{
		nPl=0;
		plHdList=NULL;
		return YSERR;
	}
}



#ifndef YSLOWMEM
const YsArray <YsShellPolygonHandle,2> *YsShellSearchTable::FindPolygonListByEdge
	    (const YsShell &shl,unsigned edVt1,unsigned edVt2) const
#else
const YsArray <YsShellPolygonHandle,2,short> *YsShellSearchTable::FindPolygonListByEdge
	    (const YsShell &shl,unsigned edVt1,unsigned edVt2) const
#endif
{
	return edgToPlg.GetEdgToPlgTable(shl,edVt1,edVt2);
}

YSRESULT YsShellSearchTable::FindPolygonListByEdge
    (int &nPl,const YsShellPolygonHandle *&plHdList,
     const YsShell &shl,unsigned edVtKey1,unsigned edVtKey2) const
{
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *found;
#else
	const YsArray <YsShellPolygonHandle,2,short> *found;
#endif
	found=edgToPlg.GetEdgToPlgTable(shl,edVtKey1,edVtKey2);
	if(found!=NULL && found->GetN()>0)
	{
		nPl=(int)found->GetN();
		plHdList=(*found);
		return YSOK;
	}
	else
	{
		nPl=0;
		plHdList=NULL;
		return YSERR;
	}
}




#ifndef YSLOWMEM
const YsArray <YsShellPolygonHandle,2> *YsShellSearchTable::FindPolygonListByEdge
	    (const YsShell &shl,const YsShellEdgeEnumHandle &handle) const
#else
const YsArray <YsShellPolygonHandle,2,short> *YsShellSearchTable::FindPolygonListByEdge
	    (const YsShell &shl,const YsShellEdgeEnumHandle &handle) const
#endif
{
	return edgToPlg.GetEdgToPlgTable(shl,handle);
}

YSRESULT YsShellSearchTable::FindPolygonListByEdge
    (int &nPl,const YsShellPolygonHandle *&plHdList,
     const YsShell &shl,const YsShellEdgeEnumHandle &handle) const
{
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *found;
#else
	const YsArray <YsShellPolygonHandle,2,short> *found;
#endif
	found=edgToPlg.GetEdgToPlgTable(shl,handle);
	if(found!=NULL && found->GetN()>0)
	{
		nPl=(int)found->GetN();
		plHdList=(*found);
		return YSOK;
	}
	else
	{
		nPl=0;
		plHdList=NULL;
		return YSERR;
	}
}




int YsShellSearchTable::GetNumPolygonUsingEdge(const YsShell &shl,const YsShellEdgeEnumHandle &handle) const
{
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *found;
#else
	const YsArray <YsShellPolygonHandle,2,short> *found;
#endif
	found=edgToPlg.GetEdgToPlgTable(shl,handle);
	if(found!=NULL)
	{
		return (int)found->GetN();
	}
	return 0;
}

int YsShellSearchTable::GetNumPolygonUsingEdge(const YsShell &shl,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *found;
#else
	const YsArray <YsShellPolygonHandle,2,short> *found;
#endif
	found=edgToPlg.GetEdgToPlgTable(shl,edVtHd1,edVtHd2);
	if(found!=NULL)
	{
		return (int)found->GetN();
	}
	return 0;
}

int YsShellSearchTable::GetNumPolygonUsingEdge(const YsShell &shl,unsigned int edVtKey1,unsigned int edVtKey2) const
{
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *found;
#else
	const YsArray <YsShellPolygonHandle,2,short> *found;
#endif
	found=edgToPlg.GetEdgToPlgTable(shl,edVtKey1,edVtKey2);
	if(found!=NULL)
	{
		return (int)found->GetN();
	}
	return 0;
}





YsShellPolygonHandle YsShellSearchTable::GetNeighborPolygon(const YsShell &shl,YsShellPolygonHandle plHd,YSSIZE_T n) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *sharedPlHdList;
#else
	const YsArray <YsShellPolygonHandle,2,short> *sharedPlHdList;
#endif

	shl.GetVertexListOfPolygon(nPlVt,plVtHd,plHd);

	if(0<=n && n<nPlVt-1)
	{
		sharedPlHdList=FindPolygonListByEdge(shl,plVtHd[n],plVtHd[n+1]);
	}
	else if(n==nPlVt-1)
	{
		sharedPlHdList=FindPolygonListByEdge(shl,plVtHd[0],plVtHd[nPlVt-1]);
	}
	else
	{
		return NULL;
	}

	if(sharedPlHdList!=NULL && sharedPlHdList->GetN()==2)
	{
		if((*sharedPlHdList)[0]==plHd)
		{
			return (*sharedPlHdList)[1];
		}
		else if((*sharedPlHdList)[1]==plHd)
		{
			return (*sharedPlHdList)[0];
		}
	}

	return NULL;
}

YsShellPolygonHandle YsShellSearchTable::GetNeighborPolygon(const YsShell &shl,YsShellPolygonHandle plHd,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	int nPl;
	const YsShellPolygonHandle *plHdList;
	if(FindPolygonListByEdge(nPl,plHdList,shl,edVtHd1,edVtHd2)==YSOK && nPl==2)
	{
		if(plHdList[0]==plHd)
		{
			return plHdList[1];
		}
		else if(plHdList[1]==plHd)
		{
			return plHdList[0];
		}
	}
	return NULL;
}

YSBOOL YsShellSearchTable::CheckPolygonNeighbor(YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2,const YsShell &shl) const
{
	int i,nPlVt1;
	nPlVt1=shl.GetNumVertexOfPolygon(plHd1);
	for(i=0; i<nPlVt1; i++)
	{
		if(GetNeighborPolygon(shl,plHd1,i)==plHd2)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSRESULT YsShellSearchTable::GetSharedEdge(YsShellVertexHandle edVtHd[2],const YsShell &shl,YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2) const
{
	YsShellPolygonHandle plHd[2];
	if(shl.GetNumVertexOfPolygon(plHd1)<=shl.GetNumVertexOfPolygon(plHd2))
	{
		plHd[0]=plHd1;
		plHd[1]=plHd2;
	}
	else
	{
		plHd[0]=plHd2;
		plHd[1]=plHd1;
	}

	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	shl.GetVertexListOfPolygon(nPlVt,plVtHd,plHd[0]);
	for(YSSIZE_T i=0; i<nPlVt; i++)
	{
		const YSSIZE_T iNext=(i+1<nPlVt ? i+1 : 0);
		const YsArray <YsShellPolygonHandle,2> *edPlHd=FindPolygonListByEdge(shl,plVtHd[i],plVtHd[iNext]);
		if(NULL!=edPlHd && YSTRUE==YsIsIncluded(edPlHd->GetN(),edPlHd->GetArray(),plHd[1]))
		{
			edVtHd[0]=plVtHd[i];
			edVtHd[1]=plVtHd[iNext];
			return YSOK;
		}
	}

	edVtHd[0]=NULL;
	edVtHd[1]=NULL;
	return YSERR;
}



// const YsArray <YsShellPolygonHandle> *YsShellSearchTable::FindPolygonListByVertexList
//         (const YsShell &shl,int nVtx,const YsShellVertexHandle vtHd[]) const
// {
// 	return vtxLstToPlg.GetVtxLstToPlgTable(shl,nVtx,vtHd);
// }
// 
// YSRESULT YsShellSearchTable::FindPolygonListByVertexList
//     (int &nPl,const YsShellPolygonHandle *&plHdList,
//      const YsShell &shl,int nVtx,const YsShellVertexHandle vtHd[]) const
// {
// 	const YsArray <YsShellPolygonHandle> *found;
// 	found=vtxLstToPlg.GetVtxLstToPlgTable(shl,nVtx,vtHd);
// 	if(found!=NULL && found->GetN()>0)
// 	{
// 		nPl=found->GetN();
// 		plHdList=(*found);
// 		return YSOK;
// 	}
// 	else
// 	{
// 		nPl=0;
// 		plHdList=NULL;
// 		return YSERR;
// 	}
// }



const YsArray <YsShellVtxToEdgTableElem,8> *YsShellSearchTable::FindEdge
    (const YsShell &shl,YsShellVertexHandle vtHd) const
{
	return vtxToEdg.FindEdge(shl,vtHd);
}

YSRESULT YsShellSearchTable::AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd)
{
	YSRESULT err;
	err=YSOK;

	if(plgToPlg.AddPolygon(shl,plHd)!=YSOK)
	{
		err=YSERR;
	}
	if(vtxToPlg.AddPolygon(shl,plHd)!=YSOK)
	{
		err=YSERR;
	}
	if(edgToPlg.AddPolygon(shl,plHd)!=YSOK)
	{
		err=YSERR;
	}

// 	if(vtxLstToPlg.AddPolygon(shl,plHd)!=YSOK)
// 	{
// 		err=YSERR;
// 	}

	int i,nVtHd;
	const YsShellVertexHandle *vtHdLst;
	nVtHd=shl.GetNumVertexOfPolygon(plHd);
	vtHdLst=shl.GetVertexListOfPolygon(plHd);
	for(i=0; i<nVtHd-1; i++)
	{
		vtxToEdg.AddEdge(shl,vtHdLst[i],vtHdLst[i+1]);
	}
	vtxToEdg.AddEdge(shl,vtHdLst[nVtHd-1],vtHdLst[0]);

	return err;
}

YSRESULT YsShellSearchTable::DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd)
{
	YSRESULT err;
	err=YSOK;
	if(plgToPlg.DeletePolygon(shl,plHd)!=YSOK)
	{
		err=YSERR;
	}
	if(vtxToPlg.DeletePolygon(shl,plHd)!=YSOK)
	{
		err=YSERR;
	}
	if(edgToPlg.DeletePolygon(shl,plHd)!=YSOK)
	{
		err=YSERR;
	}
// 	if(vtxLstToPlg.DeletePolygon(shl,plHd)!=YSOK)
// 	{
// 		err=YSERR;
// 	}

	int i,nVtHd;
	const YsShellVertexHandle *vtHdLst;
	nVtHd=shl.GetNumVertexOfPolygon(plHd);
	vtHdLst=shl.GetVertexListOfPolygon(plHd);
	for(i=0; i<nVtHd-1; i++)
	{
		vtxToEdg.DeleteEdge(shl,vtHdLst[i],vtHdLst[i+1]);
	}
	vtxToEdg.DeleteEdge(shl,vtHdLst[nVtHd-1],vtHdLst[0]);

	return err;
}

int YsShellSearchTable::GetNumEdge(const YsShell &) const
{
	return edgToPlg.GetNumEdge();
}

YSRESULT YsShellSearchTable::RewindEdgeEnumHandle(const YsShell &,YsShellEdgeEnumHandle &handle) const
{
	return edgToPlg.RewindEdgeEnumHandle(handle);
}

YSRESULT YsShellSearchTable::FindNextEdge(const YsShell &,YsShellEdgeEnumHandle &handle) const
{
	return edgToPlg.FindNextEdge(handle);
}

YSRESULT YsShellSearchTable::GetEdge(const YsShell &shl,unsigned &vtKey1,unsigned &vtKey2,YsShellEdgeEnumHandle handle) const
{
	YsShellVertexHandle edVtHd1,edVtHd2;
	if(edgToPlg.GetEdge(edVtHd1,edVtHd2,handle)==YSOK)
	{
		vtKey1=shl.GetSearchKey(edVtHd1);
		vtKey2=shl.GetSearchKey(edVtHd2);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellSearchTable::GetEdge(const YsShell &,YsShellVertexHandle &edVtHd1,YsShellVertexHandle &edVtHd2,YsShellEdgeEnumHandle handle) const
{
	return edgToPlg.GetEdge(edVtHd1,edVtHd2,handle);
}

YsShell::Edge YsShellSearchTable::GetEdge(const YsShell &shl,YsShellEdgeEnumHandle handle) const
{
	YsShell::Edge edge;
	if(YSOK!=GetEdge(shl,edge.edVtHd[0],edge.edVtHd[1],handle))
	{
		edge.edVtHd[0]=NULL;
		edge.edVtHd[1]=NULL;
	}
	return edge;
}

YSBOOL YsShellSearchTable::CheckPolygonExist(const YsShell &shl,int nPlVt,const YsShellVertexHandle plVtHd[]) const
{
	YsArray <YsShellPolygonHandle,16> plHdList;
	if(FindPolygonListByVertexList(plHdList,shl,nPlVt,plVtHd)==YSOK && plHdList.GetN()>0)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

// YSBOOL YsShellSearchTable::GetOverlapWarning(void) const
// {
// 	return vtxLstToPlg.overlapWarning;
// }
// 
// void YsShellSearchTable::ClearOverlapWarning(void) const
// {
// 	vtxLstToPlg.overlapWarning=YSFALSE;
// }

////////////////////////////////////////////////////////////

