/* ////////////////////////////////////////////////////////////

File Name: ysshelllattice.cpp
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


////////////////////////////////////////////////////////////

YsShellOctree::YsShellOctree()
{
	shl=NULL;
	nInsideOutsideComputed=0;
	nDerivedFromNeighbor=0;
}

YSSIDE YsShellOctree::CheckInside(const YsVec3 &pos) const
{
	const YsOctreeNode <YSSIDE> *node;
	node=GetNode(pos);
	if(node!=NULL)
	{
		return node->dat;
	}
	else
	{
		return YSOUTSIDE;
	}
}

YSRESULT YsShellOctree::TraverseFunc(YsOctreeNode <YSSIDE> *node,const YsVec3 &min,const YsVec3 &max,int param1,int)
{
	if(param1==-1) // Build Inside/Outside
	{
		if(node->dat==YSUNKNOWNSIDE && node->IsBottomMostNode()==YSTRUE)
		{
			YsVec3 cen;
			cen=(max+min)/2.0;
			node->dat=shl->CheckInsideSolid(cen);
			nInsideOutsideComputed++;
			SpreadInsideOutside(node, 1, 0, 0,node->dat,16);
			SpreadInsideOutside(node, 0, 1, 0,node->dat,16);
			SpreadInsideOutside(node, 0, 0, 1,node->dat,16);
			SpreadInsideOutside(node,-1, 0, 0,node->dat,16);
			SpreadInsideOutside(node, 0,-1, 0,node->dat,16);
			SpreadInsideOutside(node, 0, 0,-1,node->dat,16);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellOctree::SpreadInsideOutside(YsOctreeNode <YSSIDE> *node,int vx,int vy,int vz,YSSIDE side,int depthLimit)
{
	int ix,iy,iz;
	int nx,ny,nz;
	YsOctreeNode <YSSIDE> *parent,*seeker,*onestep;

	if(depthLimit>0)
	{
		// Climb up until it can move one step to the (vx,vy,vz)
		// Easily found neighbor
		//   Siblings of direct parent
		//   Direct child of some higher parent

		// So, let's check siblings first
		seeker=node;
		parent=node->GetParent();
		if(parent!=NULL)
		{
			if(parent->GetIndexNumberOf(ix,iy,iz,seeker)==YSOK)
			{
				nx=ix+vx;
				ny=iy+vy;
				nz=iz+vz;
				onestep=parent->GetChild(nx,ny,nz);
				if(onestep!=NULL)
				{
					SpreadInsideOutsideInverse(onestep,vx,vy,vz,side,depthLimit);
				}
			}
		}


		// Check direct children of some higher parent
		onestep=NULL;
		seeker=node;
		parent=node->GetParent();
		while(parent!=NULL)
		{
			if(parent->GetIndexNumberOf(ix,iy,iz,seeker)==YSOK)
			{
				nx=ix+vx;
				ny=iy+vy;
				nz=iz+vz;
				onestep=parent->GetChild(nx,ny,nz);
				if(onestep!=NULL)
				{
					SpreadInsideOutsideInverse(onestep,vx,vy,vz,side,1); // Note: depthLimit must be one
					break;
				}
			}
			seeker=parent;
			parent=parent->GetParent();
		}
		// Note : If no parent, cannot go any farther to (vx,vy,vz).
	}
	return YSOK;
}

YSRESULT YsShellOctree::SpreadInsideOutsideInverse(YsOctreeNode <YSSIDE> *node,int vx,int vy,int vz,YSSIDE side,int depthLimit)
{
	int i;
	int dirTable[6][3]=
	{
		{ 1, 0, 0},
		{ 0, 1, 0},
		{ 0, 0, 1},
		{-1, 0, 0},
		{ 0,-1, 0},
		{ 0, 0,-1}
	};

	if(depthLimit>0)
	{
		if(node->IsBottomMostNode()==YSTRUE)
		{
			if(node->dat!=YSUNKNOWNSIDE)
			{
				return YSOK;  // This node is already visited.
			}
			else
			{
				node->dat=side;
				nDerivedFromNeighbor++;
				for(i=0; i<6; i++)
				{
					if(vx==-dirTable[i][0] && vy==-dirTable[i][1] && vz==-dirTable[i][2])
					{
						// Don't take it.
					}
					else
					{
						SpreadInsideOutside(node,dirTable[i][0],dirTable[i][1],dirTable[i][2],side,depthLimit-1);
					}
				}
			}
		}
		else
		{
			// Note : Go backward direction
			if(vx==-1)
			{
				SpreadInsideOutsideInverse(node->GetChild(1,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,0,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,1),vx,vy,vz,side,depthLimit-1);
			}
			else if(vx==1)
			{
				SpreadInsideOutsideInverse(node->GetChild(0,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,0,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,1,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,1,1),vx,vy,vz,side,depthLimit-1);
			}
			else if(vy==-1)
			{
				SpreadInsideOutsideInverse(node->GetChild(0,1,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,1,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,1),vx,vy,vz,side,depthLimit-1);
			}
			else if(vy==1)
			{
				SpreadInsideOutsideInverse(node->GetChild(0,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,0,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,0,1),vx,vy,vz,side,depthLimit-1);
			}
			else if(vz==-1)
			{
				SpreadInsideOutsideInverse(node->GetChild(0,0,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,1,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,0,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,1),vx,vy,vz,side,depthLimit-1);
			}
			else if(vz==1)
			{
				SpreadInsideOutsideInverse(node->GetChild(0,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,1,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,0),vx,vy,vz,side,depthLimit-1);
			}
		}
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

YsShellLatticeElem::YsShellLatticeElem()
{
	ompLock=NULL;
}

YsShellLatticeElem::~YsShellLatticeElem()
{
}

////////////////////////////////////////////////////////////

YsShellLattice::YsShellLattice(int hashSize) : plgToCellTable(hashSize)
{
	shl=NULL;
	latticeMayBeSmallerThanShl=YSFALSE;

	isSubLattice=YSFALSE;
	subLattice=NULL;
	subLatBx0=-1;
	subLatBy0=-1;
	subLatBz0=-1;
	subLatBx1=-1;
	subLatBy1=-1;
	subLatBz1=-1;

	checkInsideSolidLog=YSFALSE;
	ray1.Set(0.0,0.0,0.0);
	ray2.Set(0.0,0.0,0.0);
	indeterminantCellList.Set(0,NULL);

	enablePlgToCellTable=YSTRUE;
	enableInsideOutsideInfo=YSTRUE;

	plgToCellTable.tableName="PlgToLtcCell";
	plgToCellHashSize=hashSize;

	xyPlane=YSFALSE;
	xzPlane=YSFALSE;
	yzPlane=YSFALSE;

	plgToCellTable_ompLock=NULL;
	n_elem_ompLock=0;
	elem_ompLock=NULL;
	InitLock();

	EnableAutoResizingHashTable();
}

YsShellLattice::~YsShellLattice()
{
	if(subLattice!=NULL)
	{
		delete subLattice;
	}

	DestroyLock();
	DestroyElementLock();
}

void YsShellLattice::EnableAutoResizingHashTable(int minSize,int maxSize)
{
	plgToCellTable.EnableAutoResizing(minSize,maxSize);
}

void YsShellLattice::EnableAutoResizingHashTable(void)
{
	plgToCellTable.EnableAutoResizing();
}

void YsShellLattice::DisablePolygonToCellHashTable(void)
{
	enablePlgToCellTable=YSFALSE;
	plgToCellTable.PrepareTable(1);
}

void YsShellLattice::DisableInsideOutsideInfo(void)
{
	enableInsideOutsideInfo=YSFALSE;
}

void YsShellLattice::Print(void) const
{
	for(int bz=0; bz<GetNumBlockX(); ++bz)
	{
		printf("Z=%d\n",bz);
		for(int by=0; by<GetNumBlockY(); ++by)
		{
			for(int bx=0; bx<GetNumBlockX(); ++bx)
			{
				switch(GetBlock(bx,by,bz)->side)
				{
				case YSUNKNOWNSIDE:
					printf("U");
					break;
				case YSINSIDE:
					printf("I");
					break;
				case YSOUTSIDE:
					printf("O");
					break;
				case YSBOUNDARY:
					printf("B");
					break;
				default:
					printf("?");
					break;
				}
			}
			printf("\n");
		}
	}
}

void YsShellLattice::CleanUp(void)
{
	plgToCellTable.CleanUp();
	YsLattice3d <YsShellLatticeElem>::CleanUp();
	if(NULL!=subLattice)
	{
		delete subLattice;
		subLattice=NULL;
	}
}

void YsShellLattice::SetIsSubLattice(YSBOOL boo)
{
	isSubLattice=boo;
}

YSRESULT YsShellLattice::PrepareLattice(const class YsShell &shl,YSSIZE_T nCell)
{
	YsVec3 bbx[2],d;
	double vol;
	int nx,ny,nz;

	shl.GetBoundingBox(bbx[0],bbx[1]);
	d=bbx[1]-bbx[0];
	vol=d.x()*d.y()*d.z();

	if(YsTolerance<vol)
	{
		vol/=(double)nCell;

		const double siz=pow(vol,1.0/3.0);

		nx=YsGreater(1,(int)(d.x()/siz));
		ny=YsGreater(1,(int)(d.y()/siz));
		nz=YsGreater(1,(int)(d.z()/siz));
	}
	else
	{
		nx=1;
		ny=1;
		nz=1;
	}

	return PrepareLattice(shl,nx,ny,nz);
}

YSRESULT YsShellLattice::PrepareLattice(const class YsShell &shl,const YsVec3 &bbxMin,const YsVec3 &bbxMax,YSSIZE_T nCell)
{
	YsVec3 bbx[2],d;
	double vol;
	int nx,ny,nz;
	YsBoundingBoxMaker3 mkBbx;

	mkBbx.Add(bbxMin);
	mkBbx.Add(bbxMax);
	mkBbx.Get(bbx[0],bbx[1]);

	d=bbx[1]-bbx[0];
	vol=d.x()*d.y()*d.z();
	if(YsTolerance<vol)
	{
		vol/=(double)nCell;

		const double siz=pow(vol,1.0/3.0);

		nx=YsGreater(1,(int)(d.x()/siz));
		ny=YsGreater(1,(int)(d.y()/siz));
		nz=YsGreater(1,(int)(d.z()/siz));
	}
	else
	{
		nx=1;
		ny=1;
		nz=1;
	}
	return PrepareLattice(shl,bbx[0],bbx[1],nx,ny,nz);
}

YSRESULT YsShellLattice::PrepareLattice(const class YsShell &shl,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz)
{
	YsVec3 bbxMin,bbxMax;

	shl.GetBoundingBox(bbxMin,bbxMax);

	// >> Added 03/08/2002
	YsVec3 bbxSize,bbxExpand;
	double margine;
	bbxSize=bbxMax-bbxMin;
	margine=YsGreater(YsGreater(bbxSize.x(),bbxSize.y()),bbxSize.z());
	margine*=0.02;
	bbxExpand.Set(margine,margine,margine);
	bbxMin-=bbxExpand;
	bbxMax+=bbxExpand;
	// << Added 03/08/2002

	return PrepareLattice(shl,bbxMin,bbxMax,nx,ny,nz);
}

YSRESULT YsShellLattice::PrepareLattice(const class YsShell &shl,const YsVec3 &bbx0,const YsVec3 &bbx1,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz)
{
	YsVec3 bbxMin,bbxMax;
	YsBoundingBoxMaker3 makeBbx;

	if(enablePlgToCellTable==YSTRUE)
	{
		plgToCellTable.PrepareTable();
	}
	if(NULL!=subLattice)
	{
		delete subLattice;
		subLattice=NULL;
	}

	makeBbx.Add(bbx0);
	makeBbx.Add(bbx1);
	makeBbx.Get(bbxMin,bbxMax);

	this->shl=&shl;

	return Create(nx,ny,nz,bbxMin,bbxMax);
}


YSRESULT YsShellLattice::SetDomain(const class YsShell &shl,YSSIZE_T nCell)
{
	if(YSOK==PrepareLattice(shl,nCell))
	{
		auto res=RegisterShell(shl);
		if(YSOK==res)
		{
			auto n=GetNumCellMarkedAsInsideOrOutside();
			if(0<n)
			{
				return YSOK;
			}
			else
			{
				return SetDomainWithIncreasedLatticeSize(shl);
			}
		}
	}
	YsErrOut("%s\n",__FUNCTION__);
	YsErrOut("  Cannot create lattice\n");
	return YSERR;
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &shl,const YsVec3 &bbxMin,const YsVec3 &bbxMax,YSSIZE_T nCell)
{
	if(YSOK==PrepareLattice(shl,bbxMin,bbxMax,nCell))
	{
		auto res=RegisterShell(shl);
		if(YSOK==res)
		{
			auto n=GetNumCellMarkedAsInsideOrOutside();
			if(0<n)
			{
				return YSOK;
			}
			else
			{
				return SetDomainWithIncreasedLatticeSize(shl);
			}
		}
	}
	YsErrOut("%s\n",__FUNCTION__);
	YsErrOut("  Cannot create lattice\n");
	return YSERR;
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &shl,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz)
{
	if(YSOK==PrepareLattice(shl,nx,ny,nz))
	{
		return RegisterShell(shl);
	}
	YsErrOut("%s\n",__FUNCTION__);
	YsErrOut("  Cannot create lattice\n");
	return YSERR;
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &shl,const YsVec3 &bbx0,const YsVec3 &bbx1,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz)
{
	if(YSOK==PrepareLattice(shl,bbx0,bbx1,nx,ny,nz))
	{
		return RegisterShell(shl);
	}
	YsErrOut("%s\n",__FUNCTION__);
	YsErrOut("  Cannot create lattice\n");
	return YSERR;
}

YSRESULT YsShellLattice::SetDomainWithIncreasedLatticeSize(const class YsShell &shl)
{
	printf("Re-creating with increased lattice size.\n");
	YsVec3 newMin=GetMin()-GetBlockDimension(),newMax=GetMax()+GetBlockDimension();
	int newNx=GetNumBlockX()+2;
	int newNy=GetNumBlockY()+2;
	int newNz=GetNumBlockZ()+2;
	if(YSOK==PrepareLattice(shl,newMin,newMax,newNx,newNy,newNz))
	{
		return RegisterShell(shl);
	}
	printf("%s %d\n",__FUNCTION__,__LINE__);
	printf("  Cannot create lattice.\n");
	return YSERR;
}

YSRESULT YsShellLattice::RegisterShell(const class YsShell &sh)
{
	AssignElementLock();

	shl=&sh;

	for(int bz=0; bz<=GetNumBlockZ(); bz++)
	{
		for(int by=0; by<=GetNumBlockY(); by++)
		{
			for(int bx=0; bx<=GetNumBlockX(); bx++)
			{
				if(bx<GetNumBlockX() && by<GetNumBlockY() && bz<GetNumBlockZ())
				{
					GetEditableBlock(bx,by,bz)->side=YSUNKNOWNSIDE;
				}
				else
				{
					GetEditableBlock(bx,by,bz)->side=YSBOUNDARY;  // <- 2006/06/29
				}
			}
		}
	}

	YsLoopCounter ctr;

	xyPlane=YSTRUE;
	xzPlane=YSTRUE;
	yzPlane=YSTRUE;

	YsShellVertexHandle vtHd=NULL;
	auto n=sh.GetNumVertex();
	int i=0;
	YsPrintf("Set Vertices\n");

	ctr.BeginCounter(n);
	while((vtHd=sh.FindNextVertex(vtHd))!=NULL)
	{
		ctr.Increment();

		YsVec3 pos;
		sh.GetVertexPosition(pos,vtHd);
		if(YsEqual(pos.x(),0.0)!=YSTRUE)
		{
			yzPlane=YSFALSE;
		}
		if(YsEqual(pos.y(),0.0)!=YSTRUE)
		{
			xzPlane=YSFALSE;
		}
		if(YsEqual(pos.z(),0.0)!=YSTRUE)
		{
			xyPlane=YSFALSE;
		}

		if(AddVertex(vtHd)!=YSOK && isSubLattice!=YSTRUE)
		{
			YsErrOut("YsShellLattice::SetDomain()\n");
			YsErrOut("  Cannot add vertex\n");
			YsVec3 bbx[2];

			sh.GetBoundingBox(bbx[0],bbx[1]);
			YsErrOut("Bbx %s %s\n",bbx[0].Txt(),bbx[1].Txt());
			YsErrOut("Vtx %s\n",pos.Txt());
			return YSERR;
		}
		i++;
	}
	ctr.EndCounter();

	YsPrintf("Setting Polygons\n");
	YsArray <YsShellPolygonHandle> allPlHd;
	allPlHd.Set(sh.GetNumPolygon(),NULL);

	YsShellPolygonHandle plHd=NULL;
	i=0;
	while((plHd=sh.FindNextPolygon(plHd))!=NULL)
	{
		allPlHd[i++]=plHd;
	}
	if(SetDomain_Polygon(sh,allPlHd.GetN(),allPlHd)!=YSOK)
	{
		return YSERR;
	}

	/* YsShellPolygonHandle plHd;
	plHd=NULL;
	n=sh.GetNumPolygon();
	i=0;
	ctr.BeginCounter(n);
	while((plHd=sh.FindNextPolygon(plHd))!=NULL)
	{
		ctr.Increment();

		if(AddPolygon(plHd)!=YSOK)
		{
			YsErrOut("YsShellLattice::SetDomain()\n");
			YsErrOut("  Cannot add polygon\n");
			return YSERR;
		}
		i++;
	}
	ctr.EndCounter(); */

	if(enableInsideOutsideInfo==YSTRUE)
	{
		FillUndeterminedBlockRange(sh,0,0,0,GetNumBlockX()-1,GetNumBlockY()-1,GetNumBlockZ()-1);
	}

	return YSOK;
}

YSSIZE_T YsShellLattice::GetNumCellMarkedAsInsideOrOutside(void) const
{
	YSSIZE_T n=0;
	for(int bx=0; bx<GetNumBlockX(); ++bx)
	{
		for(int by=0; by<GetNumBlockY(); ++by)
		{
			for(int bz=0; bz<GetNumBlockZ(); ++bz)
			{
				auto side=GetBlock(bx,by,bz)->side;
				if(YSINSIDE==side || YSOUTSIDE==side)
				{
					++n;
				}
			}
		}
	}
	return n;
}

YSRESULT YsShellLattice::SetDomain2DMode(const class YsShell &shl,YSSIZE_T nCell)
{
	YsVec3 bbx[2],dgn;
	shl.GetBoundingBox(bbx[0],bbx[1]);
	dgn=bbx[1]-bbx[0];

	if(fabs(dgn.x())<fabs(dgn.y()) && fabs(dgn.x())<fabs(dgn.z()))
	{
		return SetDomainYZMode(shl,nCell);
	}
	else if(fabs(dgn.y())<fabs(dgn.z()))
	{
		return SetDomainXZMode(shl,nCell);
	}
	else
	{
		return SetDomainXYMode(shl,nCell);
	}
}

YSRESULT YsShellLattice::SetDomainXYMode(const class YsShell &shl,YSSIZE_T nCell)
{
	YsVec3 bbx[2],dgn;
	double areaBbx,areaPerCell;
	double lCell;
	int nx,ny;

	enableInsideOutsideInfo=YSFALSE;

	shl.GetBoundingBox(bbx[0],bbx[1]);
	dgn=bbx[1]-bbx[0];

	areaBbx=dgn.x()*dgn.y();
	areaPerCell=areaBbx/((double)nCell+1);

	lCell=sqrt(areaPerCell);

	bbx[0].AddX(-dgn.x()*0.01);
	bbx[1].AddX( dgn.x()*0.01);

	bbx[0].AddY(-dgn.y()*0.01);
	bbx[1].AddY( dgn.y()*0.01);

	bbx[0].AddZ(-lCell/2.0);
	bbx[1].AddZ( lCell/2.0);

	nx=(int)(dgn.x()/lCell)+1;
	ny=(int)(dgn.y()/lCell)+1;

	return SetDomain(shl,bbx[0],bbx[1],nx,ny,1);
}

YSRESULT YsShellLattice::SetDomainXZMode(const class YsShell &shl,YSSIZE_T nCell)
{
	YsVec3 bbx[2],dgn;
	double areaBbx,areaPerCell;
	double lCell;
	int nx,nz;

	enableInsideOutsideInfo=YSFALSE;

	shl.GetBoundingBox(bbx[0],bbx[1]);
	dgn=bbx[1]-bbx[0];

	areaBbx=dgn.x()*dgn.z();
	areaPerCell=areaBbx/((double)nCell+1);

	lCell=sqrt(areaPerCell);

	bbx[0].AddX(-dgn.x()*0.01);
	bbx[1].AddX( dgn.x()*0.01);

	bbx[0].AddY(-lCell/2.0);
	bbx[1].AddY( lCell/2.0);

	bbx[0].AddZ(-dgn.z()*0.01);
	bbx[1].AddZ( dgn.z()*0.01);

	nx=(int)(dgn.x()/lCell)+1;
	nz=(int)(dgn.z()/lCell)+1;

	return SetDomain(shl,bbx[0],bbx[1],nx,1,nz);
}

YSRESULT YsShellLattice::SetDomainYZMode(const class YsShell &shl,YSSIZE_T nCell)
{
	YsVec3 bbx[2],dgn;
	double areaBbx,areaPerCell;
	double lCell;
	int ny,nz;

	enableInsideOutsideInfo=YSFALSE;

	shl.GetBoundingBox(bbx[0],bbx[1]);
	dgn=bbx[1]-bbx[0];

	areaBbx=dgn.y()*dgn.z();
	areaPerCell=areaBbx/((double)nCell+1);

	lCell=sqrt(areaPerCell);

	bbx[0].AddX(-lCell/2.0);
	bbx[1].AddX( lCell/2.0);

	bbx[0].AddY(-dgn.y()*0.01);
	bbx[1].AddY( dgn.y()*0.01);

	bbx[0].AddZ(-dgn.z()*0.01);
	bbx[1].AddZ( dgn.z()*0.01);

	ny=(int)(dgn.y()/lCell)+1;
	nz=(int)(dgn.z()/lCell)+1;

	return SetDomain(shl,bbx[0],bbx[1],1,ny,nz);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[],YSSIZE_T nCell)
{
	YsVec3 bbx[2],d;
	double vol,siz;
	int nx,ny,nz;

	shl.GetBoundingBox(bbx[0],bbx[1]);
	d=bbx[1]-bbx[0];
	vol=d.x()*d.y()*d.z();
	vol/=(double)nCell;

	siz=pow(vol,1.0/3.0);

	nx=YsGreater(1,(int)(d.x()/siz));
	ny=YsGreater(1,(int)(d.y()/siz));
	nz=YsGreater(1,(int)(d.z()/siz));
	return SetDomain(shl,nPl,plHdList,nx,ny,nz);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[],const YsVec3 &bbxMin,const YsVec3 &bbxMax,YSSIZE_T nCell)
{
	YsVec3 bbx[2],d;
	double vol,siz;
	int nx,ny,nz;
	YsBoundingBoxMaker3 mkBbx;

	mkBbx.Add(bbxMin);
	mkBbx.Add(bbxMax);
	mkBbx.Get(bbx[0],bbx[1]);

	d=bbx[1]-bbx[0];
	vol=d.x()*d.y()*d.z();
	vol/=(double)nCell;

	siz=pow(vol,1.0/3.0);

	nx=YsGreater(1,(int)(d.x()/siz));
	ny=YsGreater(1,(int)(d.y()/siz));
	nz=YsGreater(1,(int)(d.z()/siz));
	return SetDomain(shl,nPl,plHdList,bbx[0],bbx[1],nx,ny,nz);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &sh,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[],YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz)
{
	YsVec3 bbxMin,bbxMax;

	sh.GetBoundingBox(bbxMin,bbxMax);

	// >> Added 03/08/2002
	YsVec3 bbxSize,bbxExpand;
	double margine;
	bbxSize=bbxMax-bbxMin;
	margine=YsGreater(YsGreater(bbxSize.x(),bbxSize.y()),bbxSize.z());
	margine*=0.02;
	bbxExpand.Set(margine,margine,margine);
	bbxMin-=bbxExpand;
	bbxMax+=bbxExpand;
	// << Added 03/08/2002

	return SetDomain(sh,nPl,plHdList,bbxMin,bbxMax,nx,ny,nz);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &sh,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[],const YsVec3 &bbx0,const YsVec3 &bbx1,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz)
{
	int n,i,bx,by,bz;
	YsVec3 bbxMin,bbxMax;
	YsBoundingBoxMaker3 makeBbx;
	YsLoopCounter ctr;

	YsPrintf("Building Shell Lattice\n");

	if(enablePlgToCellTable==YSTRUE)
	{
		plgToCellTable.PrepareTable();
	}
	if(NULL!=subLattice)
	{
		delete subLattice;
		subLattice=NULL;
	}

	makeBbx.Add(bbx0);
	makeBbx.Add(bbx1);
	makeBbx.Get(bbxMin,bbxMax);

	if(Create(nx,ny,nz,bbxMin,bbxMax)==YSOK)
	{
		AssignElementLock();

		shl=&sh;

		for(bz=0; bz<=GetNumBlockZ(); bz++)
		{
			for(by=0; by<=GetNumBlockY(); by++)
			{
				for(bx=0; bx<=GetNumBlockX(); bx++)
				{
					if(bx<GetNumBlockX() && by<GetNumBlockY() && bz<GetNumBlockZ())
					{
						GetEditableBlock(bx,by,bz)->side=YSUNKNOWNSIDE;
					}
					else
					{
						GetEditableBlock(bx,by,bz)->side=YSBOUNDARY;  // <- 2006/06/29
					}
				}
			}
		}


		xyPlane=YSTRUE;
		xzPlane=YSTRUE;
		yzPlane=YSTRUE;

		YsShellVertexHandle vtHd;
		vtHd=NULL;
		n=sh.GetNumVertex();
		i=0;
		YsPrintf("Set Vertices\n");
		ctr.BeginCounter(n);
		while((vtHd=sh.FindNextVertex(vtHd))!=NULL)
		{
			ctr.Increment();

			YsVec3 pos;
			sh.GetVertexPosition(pos,vtHd);
			if(YsEqual(pos.x(),0.0)!=YSTRUE)
			{
				yzPlane=YSFALSE;
			}
			if(YsEqual(pos.y(),0.0)!=YSTRUE)
			{
				xzPlane=YSFALSE;
			}
			if(YsEqual(pos.z(),0.0)!=YSTRUE)
			{
				xyPlane=YSFALSE;
			}

			if(AddVertex(vtHd)!=YSOK && isSubLattice!=YSTRUE)
			{
				YsPrintf("YsShellLattice::SetDomain()\n");
				YsPrintf("  Cannot add vertex\n");
				YsVec3 bbx[2];

				sh.GetBoundingBox(bbx[0],bbx[1]);
				YsPrintf("Bbx %s %s\n",bbx[0].Txt(),bbx[1].Txt());
				YsPrintf("Vtx %s\n",pos.Txt());
				return YSERR;
			}
			i++;
		}
		ctr.EndCounter();

		YsPrintf("Setting Polygons\n");
		if(SetDomain_Polygon(sh,nPl,plHdList)!=YSOK)
		{
			return YSERR;
		}

		if(enableInsideOutsideInfo==YSTRUE)
		{
			YsShellPolygonStore *limitterPtr=NULL,limitter(sh);
			if(0<nPl && NULL!=plHdList)
			{
				limitter.AddPolygon(nPl,plHdList);
				limitterPtr=&limitter;
			}
			FillUndeterminedBlockRange(sh,0,0,0,GetNumBlockX()-1,GetNumBlockY()-1,GetNumBlockZ()-1,limitterPtr,NULL);
		}

		return YSOK;
	}
	else
	{
		YsErrOut("YsShellLattice::SetDomain()\n");
		YsErrOut("  Cannot create lattice\n");
		return YSERR;
	}
}

YSRESULT YsShellLattice::FillUndeterminedBlockRange(const class YsShell &shl,int bx0,int by0,int bz0,int bx1,int by1,int bz1,const YsShellPolygonStore *limitToThesePlg,const YsShellPolygonStore *excludePlg)
{
	int n,nBlk,nBlkDone,bx,by,bz;
	YsLoopCounter ctr;

	nBlk=(bx1-bx0+1)*(by1-by0+1)*(bz1-bz0+1);
	nBlkDone=0;

	n=0;
	YsPrintf("Filling Undetermined Blocks\n");
	ctr.BeginCounter(nBlk);
	for(bz=bz0; bz<=bz1; bz++)
	{
		for(by=by0; by<=by1; by++)
		{
			for(bx=bx0; bx<=bx1; bx++)
			{
				ctr.Increment();

				if(GetBlock(bx,by,bz)->side==YSUNKNOWNSIDE)
				{
					YSSIDE side=YSUNKNOWNSIDE;
					YsVec3 blk1,blk2,cen;
					GetBlockRange(blk1,blk2,bx,by,bz);
					cen=(blk1+blk2)/2.0;

					// 2015/06/29 For the island problem >>
					for(int dx=-2; dx<=2; ++dx)
					{
						for(int dy=-2; dy<=2; ++dy)
						{
							for(int dz=-2; dz<=2; ++dz)
							{
								if(dx!=0 || dy!=0 || dz!=0)
								{
									auto cx=bx+dx;
									auto cy=by+dy;
									auto cz=bz+dz;
									auto nearBlk=GetBlock(cx,cy,cz);
									if(NULL!=nearBlk && YSBOUNDARY!=nearBlk->side && YSUNKNOWNSIDE!=nearBlk->side)
									{
										side=ShootFiniteRay(cen,bx,by,bz,cx,cy,cz,nearBlk->side);
									}
								}
								if(YSUNKNOWNSIDE!=side)
								{
									break;
								}
							}
							if(YSUNKNOWNSIDE!=side)
							{
								break;
							}
						}
						if(YSUNKNOWNSIDE!=side)
						{
							break;
						}
					}

					if(side==YSUNKNOWNSIDE)
					{
						side=shl.CheckInsideSolid(cen,limitToThesePlg,excludePlg);
					}
					// 2015/06/29 For the island problem <<

					FillUndeterminedBlock(bx,by,bz,side);
					n++;
				}

				nBlkDone++;
			}
		}
	}
	ctr.EndCounter();
	// YsPrintf("INFO: %d times FillUndeterminedBlock is fired.\n",n);

	return YSOK;
}

YSRESULT YsShellLattice::RepairLatticeAfterModification(YsShell &shl,YSSIZE_T np,const YsVec3 p[])
{
	int bx0,by0,bz0,bx1,by1,bz1;
	GetBlockIndexRange(bx0,by0,bz0,bx1,by1,bz1,np,p);

	int bx,by,bz;
	for(bz=bz0; bz<=bz1; bz++)
	{
		for(by=by0; by<=by1; by++)
		{
			for(bx=bx0; bx<=bx1; bx++)
			{
				if(GetBlock(bx,by,bz)->side!=YSBOUNDARY)
				{
					YsShellLatticeElem *elm;
					elm=GetEditableBlock(bx,by,bz);
					elm->side=YSUNKNOWNSIDE;
				}
			}
		}
	}

	if(enableInsideOutsideInfo==YSTRUE)
	{
		FillUndeterminedBlockRange(shl,bx0,by0,bz0,bx1,by1,bz1);
	}

	return YSOK;
}

YSRESULT YsShellLattice::AddVertex(YsShellVertexHandle vtHd)
{
	YsVec3 vtxPos;

	if(shl->GetVertexPosition(vtxPos,vtHd)==YSOK)
	{
		int bx,by,bz;
		if(GetBlockIndex(bx,by,bz,vtxPos)==YSOK)
		{
			YsShellLatticeElem *elm;
			elm=GetEditableBlock(bx,by,bz);
			if(elm!=NULL)
			{
				elm->vtxList.AppendItem(vtHd);
				return YSOK;
			}
		}
	}
	return YSERR;
}

YSRESULT YsShellLattice::DeleteVertex(YsShellVertexHandle vtHd)
{
	YsVec3 vtxPos;

	if(shl->GetVertexPosition(vtxPos,vtHd)==YSOK)
	{
		int bx,by,bz;
		if(GetBlockIndex(bx,by,bz,vtxPos)==YSOK)
		{
			YsShellLatticeElem *elm;
			elm=GetEditableBlock(bx,by,bz);
			if(elm!=NULL)
			{
				int i;
				for(i=0; i<elm->vtxList.GetNumItem(); i++)
				{
					if(elm->vtxList[i]==vtHd)
					{
						elm->vtxList.Delete(i);
						return YSOK;
					}
				}
			}
		}
	}
	return YSERR;
}

YSRESULT YsShellLattice::AddPolygon(YsShellPolygonHandle plHd)
{
	YsArray <YsVec3,16> plg;
	int bx1,by1,bz1,bx2,by2,bz2;
	YSBOOL watch=YSFALSE;

	if(PreparePolygon(plg,bx1,by1,bz1,bx2,by2,bz2,plHd,0.0)==YSOK)
	{
		YsCollisionOfPolygon coll;

		// Check Intersection and mark
		int nCompute,nWaste;
		coll.SetPolygon1(plg.GetNumItem(),plg.GetArray());
		coll.PrecomputeProjectionOfPolygon1();

		if(watch==YSTRUE)
		{
			printf("A %d %d %d %d %d %d\n",bx1,by1,bz1,bx2,by2,bz2);
		}

		nCompute=0;
		nWaste=0;
		for(YSSIZE_T bz=bz1; bz<=bz2; ++bz)
		{
			for(YSSIZE_T by=by1; by<=by2; ++by)
			{
				for(YSSIZE_T bx=bx1; bx<=bx2; ++bx)
				{
					nCompute++;
					if(YSTRUE!=TryAddPolygonToBlock(bx,by,bz,coll,plHd,watch))
					{
						nWaste++;
					}
				}
			}
		}
		// YsPrintf("Computed %d Wasted %d\n",nCompute,nWaste);
	}

	return YSOK;
}

YSRESULT YsShellLattice::AddPolygonWithThickness(YsShellPolygonHandle plHd,const double thickness)
{
	YsArray <YsVec3,16> plg;
	int bx1,by1,bz1,bx2,by2,bz2;
	YSBOOL watch=YSFALSE;

	if(PreparePolygon(plg,bx1,by1,bz1,bx2,by2,bz2,plHd,thickness)==YSOK)
	{
		YsVec3 nom;
		shl->GetNormalOfPolygon(nom,plHd);
		if(YsOrigin()==nom)
		{
			YsGetAverageNormalVector(nom,plg.GetN(),plg);
		}

		YsCollisionOfPolygon coll[3];

		// Check Intersection and mark
		int nCompute,nWaste;
		coll[0].SetPolygon1(plg.GetN(),plg);
		coll[0].PrecomputeProjectionOfPolygon1();

		YsArray <YsVec3,16> offsetPlg(plg.GetN(),NULL);
		for(int i=1; i<3; ++i)
		{
			const double signedOffset=(1==i ? thickness/2.0 : -thickness/2.0);
			for(int j=0; j<plg.GetN(); ++j)
			{
				offsetPlg[j]=plg[j]+signedOffset*nom;
			}
			coll[i].SetPolygon1(offsetPlg.GetN(),offsetPlg);
			coll[i].PrecomputeProjectionOfPolygon1();
		}

		nCompute=0;
		nWaste=0;
		for(YSSIZE_T bz=bz1; bz<=bz2; ++bz)
		{
			for(YSSIZE_T by=by1; by<=by2; ++by)
			{
				for(YSSIZE_T bx=bx1; bx<=bx2; ++bx)
				{
					nCompute++;
					if(YSTRUE!=TryAddPolygonToBlock(bx,by,bz,coll[0],plHd,watch) &&
					   YSTRUE!=TryAddPolygonToBlock(bx,by,bz,coll[1],plHd,watch) &&
					   YSTRUE!=TryAddPolygonToBlock(bx,by,bz,coll[2],plHd,watch))
					{
						nWaste++;
					}
				}
			}
		}
		// YsPrintf("Computed %d Wasted %d\n",nCompute,nWaste);
	}

	return YSOK;
}

YSBOOL YsShellLattice::TryAddPolygonToBlock(YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz,YsCollisionOfPolygon &coll,YsShellPolygonHandle plHd,YSBOOL watch)
{
	YsVec3 blk1,blk2;
	GetBlockRange(blk1,blk2,bx,by,bz);

	if(YSTRUE==watch)
	{
		printf("(%s)-(%s)\n",blk1.Txt(),blk2.Txt());
		printf("%d %d %d %d\n",(int)bx,(int)by,(int)bz,coll.CheckIntersectionBetweenPolygon1AndBlock(blk1,blk2));
	}

	YsShellLatticeElem *elm;
	if(YSTRUE==coll.CheckIntersectionBetweenPolygon1AndBlock(blk1,blk2) &&
	   NULL!=(elm=GetEditableBlock(bx,by,bz)))
	{
		elm->Lock();
		elm->side=YSBOUNDARY;
		elm->plgList.AppendItem(plHd);
		elm->Unlock();

		if(enablePlgToCellTable==YSTRUE)
		{
			unsigned plgKey[1]={shl->GetSearchKey(plHd)};

			YsFixedLengthArray <int,4> cell;
			cell.dat[0]=(int)bx;
			cell.dat[1]=(int)by;
			cell.dat[2]=(int)bz;
			cell.dat[3]=(int)elm->plgList.GetNumItem()-1;  // Index number can become less
			LockPlgToCellTable();
			plgToCellTable.AddElement(1,plgKey,cell);
			UnlockPlgToCellTable();
		}

		return YSTRUE;
	}
	return YSFALSE;
}

YSRESULT YsShellLattice::DeletePolygon(YsShellPolygonHandle plHd)
{
	// const YsArray <YsFixedLengthArray <int,4>,16> *cellList;
	const YsArray <YsFixedLengthArray <int,4>,4> *cellList;

	unsigned plgKey[1];
	int i,k,bx,by,bz;
	YsShellLatticeElem *elm;
	YSBOOL found;

	if(enablePlgToCellTable!=YSTRUE)
	{
		YsPrintf("YsShellLattice::DeletePolygon is unavailable because\n");
		YsPrintf("plgToCellTable is disabled.\n");
		return YSERR;
	}


	plgKey[0]=shl->GetSearchKey(plHd);
	cellList=plgToCellTable.FindElement(1,plgKey);
	if(cellList!=NULL && cellList->GetNumItem()>0)
	{
		for(i=0; i<cellList->GetNumItem(); i++)
		{
			bx=(*cellList)[i].dat[0];
			by=(*cellList)[i].dat[1];
			bz=(*cellList)[i].dat[2];

			elm=GetEditableBlock(bx,by,bz);

			if(elm!=NULL)
			{
				found=YSFALSE;
				k=YsSmaller((*cellList)[i].dat[3],(int)elm->plgList.GetNumItem()-1);
				while(k>=0)
				{
					if(elm->plgList.GetItem(k)==plHd)
					{
						found=YSTRUE;
						elm->plgList.Delete(k);
						if(elm->plgList.GetNumItem()==0)
						{
							YsFixedLengthArray <int,3> cellPosition;
							cellPosition.dat[0]=bx;
							cellPosition.dat[1]=by;
							cellPosition.dat[2]=bz;
							indeterminantCellList.AppendItem(cellPosition);
						}
						break;
					}
					k--;
				}
				if(found!=YSTRUE)
				{
					YsErrOut("YsShellLattice::DeletePolygon()\n");
					YsErrOut("  Lattice and plgToCellTable do not match.\n");
					return YSERR;
				}
			}
			else
			{
				YsErrOut("YsShellLattice::DeletePolygon()\n");
				YsErrOut("  Internal Error\n");
				return YSERR;
			}
		}
		plgToCellTable.DeleteKey(1,plgKey);
		return YSOK;
	}

	YsErrOut("YsShellLattice::DeletePolygon()\n");
	YsErrOut("  Tried to delete a polygon that is not listed.\n");
	return YSERR;
}

YSRESULT YsShellLattice::MakeSortedCellPolygonBalance(YsArray <int> &balance) const
{
	YsArray <YsFixedLengthArray <int,3> > xyz;
	YsArray <int> n;
	int x,y,z,i;

	xyz.Set(GetNumBlockX()*GetNumBlockY()*GetNumBlockZ(),NULL);
	n.Set(xyz.GetN(),NULL);

	i=0;
	for(x=0; x<GetNumBlockX(); x++)
	{
		for(y=0; y<GetNumBlockY(); y++)
		{
			for(z=0; z<GetNumBlockZ(); z++)
			{
				const YsShellLatticeElem *elm;
				elm=GetBlock(x,y,z);
				xyz[i][0]=x;
				xyz[i][1]=y;
				xyz[i][2]=z;
				n[i]=(int)elm->plgList.GetN();
				i++;
			}
		}
	}

	YsQuickSort <int,YsFixedLengthArray <int,3> > (n.GetN(),n,xyz);
	i=0;
	balance.Set(n.GetN()*4,NULL);
	for(i=0; i<n.GetN(); i++)
	{
		balance[i*4  ]=xyz[n.GetN()-1-i][0];
		balance[i*4+1]=xyz[n.GetN()-1-i][1];
		balance[i*4+2]=xyz[n.GetN()-1-i][2];
		balance[i*4+3]=n[n.GetN()-1-i];
	}

	return YSOK;
}

YSBOOL YsShellLattice::CheckSubLatticeNecessity
    (int &bx1,int &by1,int &bz1,int &bx2,int &by2,int &bz2,YsVec3 bbx[2],int &percentPlg,int &percentVol)
{
	if(YSTRUE==CheckSubLatticeNecessity_SimpleCase(bx1,by1,bz1,bx2,by2,bz2,bbx,percentPlg,percentVol))
	{
		return YSTRUE;
	}

	const int nHist=20;
	int i,n,nPlgTotal,hist[nHist];
	YsArray <int> cellBalance;
	MakeSortedCellPolygonBalance(cellBalance);



	nPlgTotal=0;
	for(i=0; i<cellBalance.GetN(); i+=4)
	{
		n=cellBalance[i+3];
		nPlgTotal+=n;
		if(n==0)
		{
			cellBalance.Resize(i);
			break;
		}
	}

	// printf("nPlgTotal %d (actual %d)\n",nPlgTotal,shl->GetNumPolygon());


	if(nPlgTotal>0)
	{
		for(i=0; i<nHist; i++)
		{
			hist[i]=0;
		}
		for(i=0; i<cellBalance.GetN(); i+=4)
		{
			n=cellBalance[i+3];
			hist[i*10/cellBalance.GetN()]+=n;
		}

		// for(i=0; i<nHist; i++)
		// {
		// 	YsPrintf("[%d] %d (%d%%)\n",i,hist[i],hist[i]*100/nPlgTotal);
		// }

		int nMainCell,nSubCell;
		int nPlgSub,nTest;
		nMainCell=GetNumBlockX()*GetNumBlockY()*GetNumBlockZ();

		bx1=cellBalance[0];
		by1=cellBalance[1];
		bz1=cellBalance[2];
		bx2=cellBalance[0];
		by2=cellBalance[1];
		bz2=cellBalance[2];

		nPlgSub=cellBalance[3];
		nTest=1;
		for(i=4; i<cellBalance.GetN(); i+=4)
		{
			int cx1,cy1,cz1,cx2,cy2,cz2;

			// YsPrintf("(%d %d %d) %d\n",cellBalance[i],cellBalance[i+1],cellBalance[i+2],cellBalance[i+3]);

			cx1=YsSmaller(bx1,cellBalance[i  ]);
			cy1=YsSmaller(by1,cellBalance[i+1]);
			cz1=YsSmaller(bz1,cellBalance[i+2]);
			cx2=YsGreater(bx2,cellBalance[i  ]);
			cy2=YsGreater(by2,cellBalance[i+1]);
			cz2=YsGreater(bz2,cellBalance[i+2]);

			nSubCell=(cx2-cx1+1)*(cy2-cy1+1)*(cz2-cz1+1);
			if(nMainCell/10<nSubCell)
			{
				printf("Exceeds 10%% of # of total cells (%d,%d)\n",nMainCell/10,nSubCell);
				break;
			}
			if(cellBalance[i+3]<(nPlgSub/nTest)/10)  // Polygon count drops 10% of the average
			{
				YsPrintf("Drop below 10%% of average (%d)\n",(nPlgSub/nTest)/10);
				break;
			}

			bx1=cx1;
			by1=cy1;
			bz1=cz1;
			bx2=cx2;
			by2=cy2;
			bz2=cz2;
			nPlgSub+=cellBalance[i+3];
			nTest++;
		}

		YsPrintf("Range (%d %d %d)-(%d %d %d)\n",bx1,by1,bz1,bx2,by2,bz2);

		nSubCell=(bx2-bx1+1)*(by2-by1+1)*(bz2-bz1+1);
		if(nSubCell<nMainCell/10)
		{
			int x,y,z,nPlg,k;
			YsHashTable <int> visited(shl->GetNumPolygon()/10+1);

			nPlg=0;
			for(x=bx1; x<=bx2; x++)
			{
				for(y=by1; y<=by2; y++)
				{
					for(z=bz1; z<=bz2; z++)
					{
						const YsShellLatticeElem *elm;
						elm=GetBlock(x,y,z);
						forYsArray(k,elm->plgList)
						{
							if(visited.CheckKeyExist(shl->GetSearchKey(elm->plgList[k]))!=YSTRUE)
							{
								visited.AddElement(shl->GetSearchKey(elm->plgList[k]),0);
								nPlg++;
							}
						}
					}
				}
			}

			percentPlg=nPlg*100/shl->GetNumPolygon();
			percentVol=nSubCell*100/nMainCell;
			YsPrintf("%d%% of polygons are concentrated in %d%% of the bounding volume.\n",percentPlg,percentVol);

			// Ad-hoc conditions :-P
			// 1% -> 30%
			// 5% -> 50%
			// 9% -> 70%
			// (percentVol-1)*5+30<percentPlg  =>  percentVol*5+25<percentPlg
			if(percentVol*5+25<percentPlg)
			{
				GetGridPosition(bbx[0],bx1  ,by1  ,bz1);
				GetGridPosition(bbx[1],bx2+1,by2+1,bz2+1);
				return YSTRUE;
			}
		}
	}

	percentPlg=0;
	percentVol=0;
	return YSFALSE;
}

YSBOOL YsShellLattice::CheckSubLatticeNecessity_SimpleCase(
    int &bx1,int &by1,int &bz1,int &bx2,int &by2,int &bz2,YsVec3 bbx[2],int &percentPlg,int &percentVol)
{
	bx1=GetNumBlockX()/2;
	by1=GetNumBlockY()/2;
	bz1=GetNumBlockZ()/2;
	bx2=GetNumBlockX()/2;
	by2=GetNumBlockY()/2;
	bz2=GetNumBlockZ()/2;

	for(int bx=0; bx<GetNumBlockX(); ++bx)
	{
		for(int by=0; by<GetNumBlockY(); ++by)
		{
			for(int bz=0; bz<GetNumBlockZ(); ++bz)
			{
				if(0<GetBlock(bx,by,bz)->plgList.GetN())
				{
					bx1=YsSmaller(bx1,bx);
					bx2=YsGreater(bx2,bx);
					by1=YsSmaller(by1,by);
					by2=YsGreater(by2,by);
					bz1=YsSmaller(bz1,bz);
					bz2=YsGreater(bz2,bz);
				}
			}
		}
	}

	++bx1;
	--bx2;
	++by1;
	--by2;
	++bz1;
	--bz2;

	if(bx2<=bx1 || by2<=by1 || bz2<=bz1)
	{
		return YSFALSE;
	}

	for(;;)
	{
		YSBOOL progress=YSFALSE;

		YSSIZE_T n1=0,n2=0;
		for(int by=by1; by<=by2; ++by)
		{
			for(int bz=bz1; bz<=bz2; ++bz)
			{
				n1+=GetBlock(bx1,by,bz)->plgList.GetN();
				n2+=GetBlock(bx2,by,bz)->plgList.GetN();
			}
		}
		if(0==n1 && bx1<bx2)
		{
			++bx1;
			progress=YSTRUE;
		}
		if(0==n2 && bx1<bx2)
		{
			--bx2;
			progress=YSTRUE;
		}

		n1=0;
		n2=0;
		for(int bx=bx1; bx<=bx2; ++bx)
		{
			for(int bz=bz1; bz<=bz2; ++bz)
			{
				n1+=GetBlock(bx,by1,bz)->plgList.GetN();
				n2+=GetBlock(bx,by2,bz)->plgList.GetN();
			}
		}
		if(0==n1 && by1<by2)
		{
			++by1;
			progress=YSTRUE;
		}
		if(0==n2 && by1<by2)
		{
			--by2;
			progress=YSTRUE;
		}

		n1=0;
		n2=0;
		for(int bx=bx1; bx<=bx2; ++bx)
		{
			for(int by=by1; by<=by2; ++by)
			{
				n1+=GetBlock(bx,by,bz1)->plgList.GetN();
				n2+=GetBlock(bx,by,bz2)->plgList.GetN();
			}
		}
		if(0==n1 && bz1<bz2)
		{
			++bz1;
			progress=YSTRUE;
		}
		if(0==n2 && bz1<bz2)
		{
			--bz2;
			progress=YSTRUE;
		}

		if(YSTRUE!=progress)
		{
			break;
		}
	}

	// Snap to the boundary, for symmetric case >>
	if(1==bx1)
	{
		bx1=0;
	}
	if(1==by1)
	{
		by1=0;
	}
	if(1==bz1)
	{
		bz1=0;
	}
	if(GetNumBlockX()-2==bx2)
	{
		bx2=GetNumBlockX()-1;
	}
	if(GetNumBlockY()-2==by2)
	{
		by2=GetNumBlockY()-1;
	}
	if(GetNumBlockZ()-2==bz2)
	{
		bz2=GetNumBlockZ()-1;
	}
	// Snap to the boundary, for symmetric case <<

	// Expand by one block so that it really covers the projectile.  Ad-hoc, but I cannot think of any better way now. >>
	if(0<bx1)
	{
		--bx1;
	}
	if(0<by1)
	{
		--by1;
	}
	if(0<bz1)
	{
		--bz1;
	}
	if(bx2<GetNumBlockX()-1)
	{
		++bx2;
	}
	if(by2<GetNumBlockY()-1)
	{
		++by2;
	}
	if(bz2<GetNumBlockZ()-1)
	{
		++bz2;
	}
	
	// Expand by one block so that it really covers the projectile.  Ad-hoc, but I cannot think of any better way now. <<

	YsArray <YsShellPolygonHandle> allPlHd;
	for(int bx=bx1; bx<=bx2; ++bx)
	{
		for(int by=by1; by<=by2; ++by)
		{
			for(int bz=bz1; bz<=bz2; ++bz)
			{
				allPlHd.Append(GetBlock(bx,by,bz)->plgList);
			}
		}
	}
	YsRemoveDuplicateInUnorderedArray(allPlHd);

	const YSSIZE_T nIn=(YSSIZE_T)(bx2-bx1+1)*(YSSIZE_T)(by2-by1+1)*(YSSIZE_T)(bz2-bz1+1);
	const YSSIZE_T nAll=(YSSIZE_T)GetNumBlockX()*(YSSIZE_T)GetNumBlockY()*(YSSIZE_T)GetNumBlockZ();

	percentPlg=(int)(allPlHd.GetN()*100/shl->GetNumPolygon());
	percentVol=(int)(nIn*100/nAll);

	YsPrintf("Sub-Lattice Simple Test\n");
	YsPrintf("%d%% of polygons are concentrated in %d%% of the bounding volume.\n",percentPlg,percentVol);

	if(percentVol*5+25<percentPlg)
	{
		GetGridPosition(bbx[0],bx1  ,by1  ,bz1);
		GetGridPosition(bbx[1],bx2+1,by2+1,bz2+1);
		return YSTRUE;
	}

	return YSFALSE;
}

YSBOOL YsShellLattice::MakeSubLattice(int nCell)
{
	YsVec3 bbx[2];
	int percentPlg,percentVol,nx,ny,nz;
	int bx0,by0,bz0,bx1,by1,bz1;

	if(CheckSubLatticeNecessity(bx0,by0,bz0,bx1,by1,bz1,bbx,percentPlg,percentVol)==YSTRUE)
	{
		CalculateNxNyNzFromNCell(nx,ny,nz,bbx[0],bbx[1],nCell);

		if(NULL!=subLattice)
		{
			delete subLattice;
		}
		subLattice=new YsShellLattice(plgToCellHashSize);
		subLatBx0=bx0;
		subLatBy0=by0;
		subLatBz0=bz0;
		subLatBx1=bx1;
		subLatBy1=by1;
		subLatBz1=bz1;

		subLattice->enablePlgToCellTable=enablePlgToCellTable;
		subLattice->enableInsideOutsideInfo=enableInsideOutsideInfo;

		subLattice->isSubLattice=YSTRUE;  // <- must come before SetDomain

		subLattice->SetDomain(*shl,bbx[0],bbx[1],nx,ny,nz);

		return YSTRUE;
	}

	return YSFALSE;
}

const YsShellLattice *YsShellLattice::GetSubLattice(void) const
{
	return subLattice;
}

YSRESULT YsShellLattice::AddEdgePiece(const YsShellVertexHandle edVtHd[2],YSHASHKEY ceKey)
{
	YsVec3 p1,p2;
	YsArray <unsigned,64> blkList;

	shl->GetVertexPosition(p1,edVtHd[0]);
	shl->GetVertexPosition(p2,edVtHd[1]);

	if(MakeIntersectingBlockList(blkList,p1,p2)==YSOK)
	{
		int i;
		YsShellLatticeEdgePiece piece;
		piece.edVtHd[0]=edVtHd[0];
		piece.edVtHd[1]=edVtHd[1];
		piece.ceKey=ceKey;
		for(i=0; i<=blkList.GetN()-3; i+=3)
		{
			YsShellLatticeElem *elm;
			elm=GetEditableBlock(blkList[i],blkList[i+1],blkList[i+2]);
			if(elm!=NULL)
			{
				elm->edgList.Append(piece);
			}
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellLattice::AddEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,YSHASHKEY ceKey)
{
	const YsShellVertexHandle edVtHd[2]=
	{
		edVtHd0,
		edVtHd1
	};
	return AddEdgePiece(edVtHd,ceKey);
}

YSRESULT YsShellLattice::DeleteEdgePiece(const YsShellVertexHandle edVtHd[2])
{
	YsVec3 p1,p2;
	YsArray <unsigned,64> blkList;

	shl->GetVertexPosition(p1,edVtHd[0]);
	shl->GetVertexPosition(p2,edVtHd[1]);

	if(MakeIntersectingBlockList(blkList,p1,p2)==YSOK)
	{
		for(YSSIZE_T i=0; i<=blkList.GetN()-3; i+=3)
		{
			YsShellLatticeElem *elm=GetEditableBlock(blkList[i],blkList[i+1],blkList[i+2]);
			if(NULL!=elm)
			{
				for(auto j=elm->edgList.GetN()-1; 0<=j; --j)
				{
					if(YSTRUE==YsSameEdge(edVtHd,elm->edgList[j].edVtHd))
					{
						elm->edgList.DeleteBySwapping(j);
					}
				}
			}
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellLattice::FixIndeterminantCell(void)
{
	YSSIZE_T n,i;
	n=indeterminantCellList.GetNumItem();
	for(i=n-1; i>=0; i--)
	{
		int bx,by,bz;
		bx=indeterminantCellList[i].dat[0];
		by=indeterminantCellList[i].dat[1];
		bz=indeterminantCellList[i].dat[2];

		YsShellLatticeElem *elm;
		elm=GetEditableBlock(bx,by,bz);

		if(elm->plgList.GetNumItem()>0)   // <- Means the cell got a new intersection
		{
			indeterminantCellList.Delete(i);
		}
		else                              // <- Means the cell must become YSINSIDE or YSOUTSIDE
		{
			int j,nei[6][3]=
			{
				{ 1, 0, 0},
				{-1, 0, 0},
				{ 0, 1, 0},
				{ 0,-1, 0},
				{ 0, 0, 1},
				{ 0, 0,-1}
			};

			YSSIDE side;
			side=YSBOUNDARY;
			for(j=0; j<6; j++)
			{
				const YsShellLatticeElem *ref;
				int dx,dy,dz;
				dx=nei[j][0];
				dy=nei[j][1];
				dz=nei[j][2];
				ref=GetBlock(bx+dx,by+dy,bz+dz);
				if(ref!=NULL && (ref->side==YSINSIDE || ref->side==YSOUTSIDE))
				{
					side=ref->side;
					break;
				}
			}

			if(side==YSBOUNDARY)   // <- Neighbor cells are all YSBOUNDARY
			{
				YsVec3 blk1,blk2;
				GetBlockRange(blk1,blk2,bx,by,bz);
				side=CheckInsideSolid((blk1+blk2)/2.0);
				if(side==YSBOUNDARY || side==YSUNKNOWNSIDE)
				{
					YsErrOut("YsShellLattice::FixIndeterminantCell()\n");
					YsErrOut("  Cannot fix an indeterminant cell %d %d %d\n",bx,by,bz);
					YsErrOut("  SIDE=%s\n",(side==YSBOUNDARY ? "BOUNDARY" : "UNKNOWN"));
				}
			}

			if(side!=YSBOUNDARY)
			{
				elm->side=side;
				indeterminantCellList.Delete(i);
			}
		}
	}

	if(indeterminantCellList.GetNumItem()==0)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSSIDE YsShellLattice::CheckInsideSolid(const YsVec3 &pnt,YSBOOL showWhichFunctionDetermined) const
{
	const YsShellLatticeElem *elm;
	int bx,by,bz;



	if(enableInsideOutsideInfo!=YSTRUE)
	{
		YsErrOut("YsShellLattice::CheckInsideSolid()\n");
		YsErrOut("  Inside/Outside information is disabled.\n");
		return YSUNKNOWNSIDE;
	}


	if(NULL!=subLattice)
	{
		YSSIDE side;
		YsVec3 subLatRange[2];
		subLattice->GetLatticeRange(subLatRange[0],subLatRange[1]);
		if(YsCheckInsideBoundingBox3(pnt,subLatRange[0],subLatRange[1])==YSTRUE)
		{
			side=subLattice->CheckInsideSolid(pnt,showWhichFunctionDetermined);
			if(side!=YSUNKNOWNSIDE)
			{
				return side;
			}
		}
	}



	if(checkInsideSolidLog==YSTRUE)
	{
		foundIntersection.Set(0,NULL);
		foundPolygon.Set(0,NULL);
	}

	if(GetBlockIndex(bx,by,bz,pnt)==YSOK && (elm=GetBlock(bx,by,bz))!=NULL)
	{
		if(elm->side==YSINSIDE || elm->side==YSOUTSIDE)
		{
			if(showWhichFunctionDetermined==YSTRUE)
			{
				YsPrintf("YsShellLattice::CheckInsideSolid()\n");
				YsPrintf("  Check 1\n");
			}
			return elm->side;
		}
		else if(elm->side==YSBOUNDARY)
		{
			if(showWhichFunctionDetermined==YSTRUE)
			{
				YsPrintf("YsShellLattice::CheckInsideSolid()\n");
				YsPrintf("  Check 2\n");
			}
			return SophisticatedCheckInsideSolidWhenTheCellIsMarkedAsBoundary(pnt,showWhichFunctionDetermined);
		}
		else
		{
			YsErrOut("YsShellLattice::CheckInsideSolid()\n");
			YsErrOut("  Error : Undetermined Cell Remains!\n");
			if(isSubLattice!=YSTRUE)
			{
				return shl->CheckInsideSolid(pnt);
			}
			else
			{
				return YSUNKNOWNSIDE;
			}
		}
	}
	else
	{
		// Outside the lattice. Cannot do much on in.
		if(showWhichFunctionDetermined==YSTRUE)
		{
			YsPrintf("YsShellLattice::CheckInsideSolid()\n");
			YsPrintf("  Report : Outside of the grids.  Assuming outside of the solid.\n");
		}

		// Modified 2000/11/10
		// return shl->CheckInsideSolid(pnt);

		if(isSubLattice!=YSTRUE)
		{
			return YSOUTSIDE;
		}
		else
		{
			return YSUNKNOWNSIDE;
		}
	}
}

YSRESULT YsShellLattice::FindFirstIntersection
    (YsShellPolygonHandle &iPlHd,YsVec3 &itsc,
     const YsVec3 &org,const YsVec3 &dir,YsShellPolygonHandle plHdExclude) const
{
	if(plHdExclude!=NULL)
	{
		return FindFirstIntersectionWithPrecisionFlag(iPlHd,itsc,org,dir,YSFALSE,1,&plHdExclude);
	}
	else
	{
		return FindFirstIntersectionWithPrecisionFlag(iPlHd,itsc,org,dir,YSFALSE,0,NULL);
	}
}

YSRESULT YsShellLattice::FindFirstIntersection
    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
     const YsVec3 &org,const YsVec3 &dir,YSSIZE_T nPlHdExclude,const YsShellPolygonHandle plHdExclude[]) const
{
	return FindFirstIntersectionWithPrecisionFlag(iPlHd,itsc,org,dir,YSFALSE,nPlHdExclude,plHdExclude);
}

YSRESULT YsShellLattice::FindFirstIntersectionHighPrecision
    (YsShellPolygonHandle &iPlHd,YsVec3 &itsc,
     const YsVec3 &org,const YsVec3 &dir,YsShellPolygonHandle plHdExclude) const
{
	if(plHdExclude!=NULL)
	{
		return FindFirstIntersectionWithPrecisionFlag(iPlHd,itsc,org,dir,YSTRUE,1,&plHdExclude);
	}
	else
	{
		return FindFirstIntersectionWithPrecisionFlag(iPlHd,itsc,org,dir,YSTRUE,0,NULL);
	}
}

YSRESULT YsShellLattice::FindFirstIntersectionHighPrecision
    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
     const YsVec3 &org,const YsVec3 &dir,YSSIZE_T nPlHdExclude,const YsShellPolygonHandle plHdExclude[]) const
{
	return FindFirstIntersectionWithPrecisionFlag(iPlHd,itsc,org,dir,YSTRUE,nPlHdExclude,plHdExclude);
}

YSRESULT YsShellLattice::FindFirstIntersection(
    YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
    const YsVec3 &org,const YsVec3 &dir,const YsConstArrayMask <YsShell::PolygonHandle> &plHdExclude) const
{
	return FindFirstIntersection(iPlHd,itsc,org,dir,plHdExclude.GetN(),plHdExclude);
}


YsShell::Elem YsShellLattice::ConvertIntersection(YsShell::PolygonHandle PlHd,const YsVec3 &Itsc) const
{
	YsShell::Elem elem;


	int nPlVt;
	const YsShell::VertexHandle *plVtHdPtr;
	shl->GetPolygon(nPlVt,plVtHdPtr,PlHd);

	YsConstArrayMask <YsShell::VertexHandle> plVtHd(nPlVt,plVtHdPtr);

	for(auto edIdx : plVtHd.AllIndex())
	{
		const YsVec3 edVtPos[2]=
		{
			shl->GetVertexPosition(plVtHd[edIdx]),
			shl->GetVertexPosition(plVtHd.GetCyclic(edIdx+1))
		};

		if(Itsc==edVtPos[0])
		{
			elem.elemType=YSSHELL_VERTEX;
			elem.vtHd=plVtHd[edIdx];
			return elem;
		}

		if(YSTRUE==YsCheckInBetween3(Itsc,edVtPos) &&
		   YsTolerance>YsGetPointLineDistance3(edVtPos,Itsc))
		{
			elem.elemType=YSSHELL_EDGE;
			elem.edVtHd[0]=plVtHd[edIdx];
			elem.edVtHd[1]=plVtHd.GetCyclic(edIdx+1);
			return elem;
		}
	}

	elem.elemType=YSSHELL_POLYGON;
	elem.plHd=PlHd;
	return elem;
}

YSRESULT YsShellLattice::FindFirstIntersectionWithPrecisionFlag
    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
     const YsVec3 &org,const YsVec3 &dir,YSBOOL highPrecision,YSSIZE_T nPlHdExclude,const YsShellPolygonHandle plHdExclude[]) const
{
	if(NULL!=subLattice)
	{
		YsVec3 subLatRange[2];
		subLattice->GetLatticeRange(subLatRange[0],subLatRange[1]);
		if(YsCheckInsideBoundingBox3(org,subLatRange[0],subLatRange[1])==YSTRUE &&
		   subLattice->FindFirstIntersectionWithPrecisionFlag(iPlHd,itsc,org,dir,highPrecision,nPlHdExclude,plHdExclude)==YSOK &&
		   iPlHd!=NULL)
		{
			return YSOK;
		}
	}



	YsArray <YsVec3,4> plg;
	YsShellPolygonHandle visitTest;
	YsHashTable <YsShellPolygonHandle> visited(255);
	YSBOOL watch;

	watch=YSFALSE;

	iPlHd=NULL;
	itsc=YsOrigin();

	YsVec3 lat[2],p1,p2;
	double diagonal;
	GetLatticeRange(lat[0],lat[1]);
	diagonal=(lat[1]-lat[0]).GetLength();
	if(YsCheckInsideBoundingBox3(org,lat[0],lat[1])==YSTRUE)
	{
		p1=org;
	}
	else
	{
		YsVec3 c1,c2;

		if(YsClipInfiniteLine3(c1,c2,org,dir,lat[0],lat[1])==YSOK)
		{
			if(YsCheckInsideBoundingBox3(c1,lat[0],lat[1])!=YSTRUE)
			{
				YsErrOut("YsShellLattice::FindFirstIntersection()\n");
				YsErrOut("  YsClipInfiniteLine returned YSOK, but the point doesn't give voxel index.\n");
				return YSERR;
			}
			p1=c1;
		}
		else
		{
			// Implication: This ray doesn't even hit the bounding box of the shell.
			return YSOK;
		}
	}



	YsArray <unsigned,64> blkList;
	if(MakeIntersectingBlockList(blkList,p1,p1+dir*diagonal)==YSOK)
	{
		YSBOOL needRecheckWithSubLattice=YSFALSE;
		for(int i=0; i<blkList.GetN(); i+=3)
		{
			const YsShellLatticeElem *elm;
			YsShellPolygonHandle plHd;
			elm=GetBlock(blkList[i],blkList[i+1],blkList[i+2]);

			if(NULL!=subLattice &&
			   subLatBx0<=(int)blkList[i  ] && (int)blkList[i  ]<=subLatBx1 &&
			   subLatBy0<=(int)blkList[i+1] && (int)blkList[i+1]<=subLatBy1 &&
			   subLatBz0<=(int)blkList[i+2] && (int)blkList[i+2]<=subLatBz1)
			{
				needRecheckWithSubLattice=YSTRUE;
			}
			else if(elm!=NULL && elm->plgList.GetN()>0)
			{
				int k;
				for(k=0; k<elm->plgList.GetN(); k++)
				{
					plHd=elm->plgList[k];


					if(watch==YSTRUE)
					{
						printf("PlKey=%d Blk=%d %d %d\n",shl->GetSearchKey(plHd),blkList[i],blkList[i+1],blkList[i+2]);
					}

					if(visited.FindElement(visitTest,shl->GetSearchKey(plHd))==YSOK)
					{
						continue;
					}

					visited.AddElement(shl->GetSearchKey(plHd),plHd);

					if(YsIsIncluded <YsShellPolygonHandle> (nPlHdExclude,plHdExclude,plHd)!=YSTRUE && 
					   shl->GetVertexListOfPolygon(plg,plHd)==YSOK)
					{
						int j;
						int nForward;
						nForward=0;
						for(j=0; j<plg.GetN(); j++)
						{
							if((plg[j]-org)*dir>-YsTolerance)
							{
								nForward++;
								break;
							}
						}

						if(nForward>0)
						{
							YsPlane pln;
							YsVec3 crs;
							if(pln.MakeBestFitPlane(plg.GetN(),plg)==YSOK)
							{
								if(watch==YSTRUE)
								{
									printf("GetItsc %d\n",pln.GetIntersectionHighPrecision(crs,org,dir));
									printf("Pos %.20lf %.20lf %.20lf\n",crs.x(),crs.y(),crs.z());
									printf("DotProd %lf\n",(crs-org)*dir);
								}

								const YSRESULT haveItsc=(YSTRUE==highPrecision ? pln.GetIntersectionHighPrecision(crs,org,dir) : pln.GetIntersection(crs,org,dir));
								if(haveItsc==YSOK && (crs-org)*dir>-YsTolerance)
								{
									YSSIDE side;
									if(plg.GetN()==3)
									{
										side=YsCheckInsideTriangle3(crs,plg);
									}
									else
									{
									 	side=YsCheckInsidePolygon3(crs,plg.GetN(),plg,pln.GetNormal());
									}
									if(watch==YSTRUE)
									{
										printf("Side %s\n",YsSideToStr(side));
									}

								 	if(side==YSBOUNDARY || side==YSINSIDE)
								 	{
										if(iPlHd==NULL)
										{
											itsc=crs;
											iPlHd=plHd;
										}
										else
										{
											if((crs-org).GetSquareLength()<(itsc-org).GetSquareLength())
											{
												itsc=crs;
												iPlHd=plHd;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		if(YSTRUE==needRecheckWithSubLattice)
		{
			YsShellPolygonHandle plHd;
			YsVec3 crs;
			if(YSOK==subLattice->FindFirstIntersectionWithPrecisionFlag(plHd,crs,org,dir,highPrecision,nPlHdExclude,plHdExclude) &&
			   nullptr!=plHd)
			{
				if(iPlHd==NULL)
				{
					itsc=crs;
					iPlHd=plHd;
				}
				else
				{
					if((crs-org).GetSquareLength()<(itsc-org).GetSquareLength())
					{
						itsc=crs;
						iPlHd=plHd;
					}
				}
			}
		}
	}
	return YSOK;
}

YSRESULT YsShellLattice::GetCellListFromPolygon
    (YsArray <YsFixedLengthArray <int,4>,4> &cell,YsShellPolygonHandle plHd) const
{
	unsigned int key[1];
	const YsArray <YsFixedLengthArray <int,4>,4> *found;

	key[0]=shl->GetSearchKey(plHd);
	found=plgToCellTable.FindElement(1,key);
	if(found!=NULL)
	{
		cell.Set(found->GetN(),*found);
		return YSOK;
	}
	cell.Set(0,NULL);
	return YSERR;
}

YSRESULT YsShellLattice::SelfDiagnostic(YsArray <YsVec3> &disagree)
{
	int i,n,bx,by,bz,nTest,tx,ty,tz;
	YsLoopCounter ctr;

	disagree.Set(0,NULL);
	n=GetNumBlockX()*GetNumBlockY()*GetNumBlockZ();
	i=0;
	ctr.BeginCounter(n);
	for(bz=0; bz<GetNumBlockZ(); bz++)
	{
		for(by=0; by<GetNumBlockY(); by++)
		{
			for(bx=0; bx<GetNumBlockX(); bx++)
			{
				ctr.Increment();

				const YsShellLatticeElem *elm;
				elm=GetBlock(bx,by,bz);
				if(elm->side==YSINSIDE || elm->side==YSOUTSIDE)
				{
					nTest=1;
				}
				else
				{
					nTest=8;
				}

				YsVec3 blk1,blk2,cen;
				GetBlockRange(blk1,blk2,bx,by,bz);
				for(tx=1; tx<=nTest; tx++)
				{
					for(ty=1; ty<=nTest; ty++)
					{
						for(tz=1; tz<=nTest; tz++)
						{
							double xp,yp,zp;
							xp=double(tx)/double(nTest+1);
							yp=double(ty)/double(nTest+1);
							zp=double(tz)/double(nTest+1);

							cen.Set(blk1.x()*xp+blk2.x()*(1.0-xp),
							        blk1.y()*yp+blk2.y()*(1.0-yp),
							        blk1.z()*zp+blk2.z()*(1.0-zp));

							YSSIDE side1,side2;
							side1=CheckInsideSolid(cen);
							side2=shl->CheckInsideSolid(cen);
							if(side1!=side2)
							{
								disagree.AppendItem(cen);
							}
						}
					}
				}

				i++;
			}
		}
	}
	ctr.EndCounter();
	YsPrintf("(# of Bad Result %d)\n",(int)disagree.GetN());

	if(disagree.GetNumItem()==0)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShellLattice::MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plgList,YSSIZE_T np,const YsVec3 p[]) const
{
	int i;
	YsBoundingBoxMaker3 bbx;
	YsVec3 bbxMin,bbxMax;
	int bx1,by1,bz1,bx2,by2,bz2;

	bbx.Begin(p[0]);
	for(i=1; i<np; i++)
	{
		bbx.Add(p[i]);
	}

	bbx.Get(bbxMin,bbxMax);

	if(NULL!=subLattice &&  // 2006/08/15
	   YsCheckInsideBoundingBox3(bbxMin,subLattice->min,subLattice->max)==YSTRUE &&
	   YsCheckInsideBoundingBox3(bbxMax,subLattice->min,subLattice->max)==YSTRUE)
	{
		return subLattice->MakePossibllyCollidingPolygonList(plgList,np,p);
	}

	if(GetBlockIndexAutoBound(bx1,by1,bz1,bbxMin)==YSOK &&
	   GetBlockIndexAutoBound(bx2,by2,bz2,bbxMax)==YSOK)
	{
		return MakeListOfPolygonsThatGoThroughTheBlockRegion(plgList,bx1,by1,bz1,bx2,by2,bz2);
	}
	return YSERR;
}

YsArray <YsShell::PolygonHandle> YsShellLattice::GetPolygonIntersectionCandidate(YSSIZE_T np,const YsVec3 p[]) const
{
	if(NULL!=subLattice)
	{
		YsBoundingBoxMaker3 bbx;
		YsVec3 bbxMin,bbxMax;

		bbx.Begin(p[0]);
		for(int i=1; i<np; i++)
		{
			bbx.Add(p[i]);
		}

		bbx.Get(bbxMin,bbxMax);


		if(YsCheckInsideBoundingBox3(bbxMin,subLattice->min,subLattice->max)==YSTRUE &&
		   YsCheckInsideBoundingBox3(bbxMax,subLattice->min,subLattice->max)==YSTRUE)
		{
			return subLattice->GetPolygonIntersectionCandidate(np,p);
		}
	}
	
	YsKeyStore visited;
	YsArray <YsShell::PolygonHandle> plgList;
	for(auto i : this->GetIntersectingBlockListPolygon(np,p))
	{
		auto elm=GetBlock(i.x(),i.y(),i.z());
		if(elm!=NULL)
		{
			for(auto plHd : elm->plgList)
			{
				if(YSTRUE!=visited.IsIncluded(shl->GetSearchKey(plHd)))
				{
					visited.Add(shl->GetSearchKey(plHd));
					plgList.Append(plHd);
				}
			}
		}
	}
	return plgList;
}

YSRESULT YsShellLattice::MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plgList,const YsVec3 &vt1,const YsVec3 &vt2) const
{
	YsBoundingBoxMaker3 bbx;
	YsVec3 bbxMin,bbxMax;
	int bx1,by1,bz1,bx2,by2,bz2;

	bbx.Begin(vt1);
	bbx.Add(vt2);

	bbx.Get(bbxMin,bbxMax);

	if(GetBlockIndexAutoBound(bx1,by1,bz1,bbxMin)==YSOK &&
	   GetBlockIndexAutoBound(bx2,by2,bz2,bbxMax)==YSOK)
	{
		return MakeListOfPolygonsThatGoThroughTheBlockRegion(plgList,bx1,by1,bz1,bx2,by2,bz2);
	}
	return YSERR;
}

YSRESULT YsShellLattice::MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plHdList,YsShellPolygonHandle plHd) const
{
	unsigned plgKey[1];
	// const YsArray <YsFixedLengthArray <int,4>,16> *cellList;
	const YsArray <YsFixedLengthArray <int,4>,4> *cellList;


	if(enablePlgToCellTable!=YSTRUE)
	{
		YsPrintf("YsShellLattice::MakePossibllyCollidingPolygonList(plHdList,plHd)\n");
		YsPrintf("  This function is unavailable because plgToCellTable is disabled.\n");
		return YSERR;
	}


	// 2013/06/24 I forgot enabling sub-lattice in here. >>
	if(NULL!=subLattice)
	{
		YsArray <YsVec3,16> plVtPos;
		shl->GetVertexListOfPolygon(plVtPos,plHd);

		YSBOOL outside=YSFALSE;
		for(int i=0; i<plVtPos.GetN(); i++)
		{
			if(YSTRUE!=YsCheckInsideBoundingBox3(plVtPos[i],subLattice->min,subLattice->max))
			{
				outside=YSTRUE;
				break;
			}
		}

		if(YSTRUE!=outside)
		{
			return subLattice->MakePossibllyCollidingPolygonList(plHdList,plHd);
		}
	}
	// 2013/06/24 I forgot enabling sub-lattice in here. <<


	plgKey[0]=shl->GetSearchKey(plHd);
	cellList=plgToCellTable.FindElement(1,plgKey);

	plHdList.Set(0,NULL);

	if(cellList!=NULL)
	{
		YSSIZE_T i;
		for(i=0; i<cellList->GetN(); i++)
		{
			const YsShellLatticeElem *elm;
			elm=GetBlock((*cellList)[i][0],(*cellList)[i][1],(*cellList)[i][2]);
			if(elm!=NULL)
			{
				plHdList.Append(elm->plgList.GetN(),elm->plgList);
			}
		}

		/* YsArray <unsigned,256> plKeyList;
		plKeyList.Set(plHdList.GetN(),NULL);
		for(i=0; i<plHdList.GetN(); i++)
		{
			plKeyList[i]=shl->GetSearchKey(plHdList[i]);
		}
		YsQuickSort <unsigned,YsShellPolygonHandle> (plKeyList.GetN(),plKeyList,plHdList); */ // Commented out 2009/03/30
		YsQuickSortAny <YsShellPolygonHandle,int> (plHdList.GetN(),plHdList,NULL);  // Replace the above block 2009/03/30

		for(i=plHdList.GetN()-1; i>0; i--)
		{
			if(plHdList[i]==plHdList[i-1] || plHdList[i]==plHd)
			{
				plHdList.DeleteBySwapping(i);
			}
		}
		if(plHdList[0]==plHd)  // The previous for-loop doesn't cover plHdList[0], covers only down to plHdList[1].
		{
			plHdList.DeleteBySwapping(0);
		}
	}

	return YSOK;
}

YSRESULT YsShellLattice::MakePossibllyIncludedVertexList
        (YsArray <YsShellVertexHandle,16> &vtxList,YSSIZE_T np,const YsVec3 p[]) const
{
	YSSIZE_T i;
	YsBoundingBoxMaker3 bbx;
	YsVec3 bbxMin,bbxMax;
	int bx1,by1,bz1,bx2,by2,bz2;


	bbx.Begin(p[0]);
	for(i=1; i<np; i++)
	{
		bbx.Add(p[i]);
	}

	bbx.Get(bbxMin,bbxMax);

	vtxList.Set(0,NULL);

	if(GetBlockIndexAutoBound(bx1,by1,bz1,bbxMin)==YSOK &&
	   GetBlockIndexAutoBound(bx2,by2,bz2,bbxMax)==YSOK)
	{
		YSSIZE_T i;
		int bx,by,bz;
		for(bx=bx1; bx<=bx2; bx++)
		{
			for(by=by1; by<=by2; by++)
			{
				for(bz=bz1; bz<=bz2; bz++)
				{
					const YsShellLatticeElem *elm;
					elm=GetBlock(bx,by,bz);
					if(elm!=NULL)
					{
						for(i=0; i<elm->vtxList.GetNumItem(); i++)
						{
							vtxList.AppendItem(elm->vtxList[i]);
						}
					}
					else
					{
						YsErrOut("YsShellLattice::MakePossibllyIncludedVertexList()\n");
						YsErrOut("  Internal Error(1)\n");
						return YSERR;
					}
				}
			}
		}

		// >> 2007/06/27
		YsArray <unsigned int,16> vtKeyList;
		vtKeyList.Set(vtxList.GetN(),NULL);
		forYsArray(i,vtxList)
		{
			vtKeyList[i]=shl->GetSearchKey(vtxList[i]);
		}
		YsQuickSort <unsigned,YsShellVertexHandle> (vtKeyList.GetN(),vtKeyList,vtxList);

		for(i=vtxList.GetN()-1; i>0; i--)
		{
			if(vtxList[i]==vtxList[i-1])
			{
				vtxList.DeleteBySwapping(i);
			}
		}
		// << 2007/06/27

		return YSOK;
	}

	return YSERR;
}

YSRESULT YsShellLattice::MakeEdgePieceListByBoundingBox(
     YsArray <YsShellVertexHandle,16> &edVtHd,YsArray <unsigned int,16> &ceKey,
     const YsVec3 &vt1,const YsVec3 &vt2) const
{
	YsBoundingBoxMaker3 bbx;
	YsVec3 bbxMin,bbxMax;
	int bx1,by1,bz1,bx2,by2,bz2;
	YsFixedLengthToMultiHashTable <unsigned int,2,1> visitedEdge(256);

	visitedEdge.EnableAutoResizing(256,256+shl->GetNumPolygon()/10);

	bbx.Add(vt1);
	bbx.Add(vt2);
	bbx.Get(bbxMin,bbxMax);

	edVtHd.Set(0,NULL);
	ceKey.Set(0,NULL);

	if(GetBlockIndexAutoBound(bx1,by1,bz1,bbxMin)==YSOK &&
	   GetBlockIndexAutoBound(bx2,by2,bz2,bbxMax)==YSOK)
	{
		int bx,by,bz,i;
		for(bx=bx1; bx<=bx2; bx++)
		{
			for(by=by1; by<=by2; by++)
			{
				for(bz=bz1; bz<=bz2; bz++)
				{
					const YsShellLatticeElem *elm;
					elm=GetBlock(bx,by,bz);
					if(elm!=NULL)
					{
						forYsArray(i,elm->edgList)
						{
							unsigned int edVtKey[2];
							edVtKey[0]=shl->GetSearchKey(elm->edgList[i].edVtHd[0]);
							edVtKey[1]=shl->GetSearchKey(elm->edgList[i].edVtHd[1]);
							if(visitedEdge.CheckKeyExist(2,edVtKey)!=YSTRUE)
							{
								visitedEdge.AddElement(2,edVtKey,0);
								edVtHd.Append(2,elm->edgList[i].edVtHd);
								ceKey.Append(elm->edgList[i].ceKey);
							}
						}
					}
					else
					{
						YsErrOut("YsShellLattice::MakeEdgePieceListByBoundingBox()\n");
						YsErrOut("  Internal Error(1)\n");
						return YSERR;
					}
				}
			}
		}
		return YSOK;
	}

	return YSERR;
}

YSRESULT YsShellLattice::MakeEdgePieceListByPointAndBlockDistance(
	     YsArray <YsShellVertexHandle,16> &edVtHd,YsArray <YSHASHKEY,16> &ceKey,YSSIZE_T &nTested,
	     const YsVec3 &refPos,int blockDist) const
{
	edVtHd.CleanUp();
	ceKey.CleanUp();
	nTested=0;

	YsVec3i idx;
	if(YSOK==GetBlockIndex(idx,refPos))
	{
		const int x0=idx.x()-blockDist;
		const int y0=idx.y()-blockDist;
		const int z0=idx.z()-blockDist;
		const int x1=idx.x()+blockDist;
		const int y1=idx.y()+blockDist;
		const int z1=idx.z()+blockDist;

		if(0<blockDist)
		{
			for(int bx=x0; bx<=x1; ++bx)
			{
				for(int by=y0; by<=y1; ++by)
				{
					auto blk=GetBlock(bx,by,z0);
					if(NULL!=blk)
					{
						++nTested;
						for(auto &edgePiece : blk->edgList)
						{
							edVtHd.Append(2,edgePiece.edVtHd);
							ceKey.Append(edgePiece.ceKey);
						}
					}
					blk=GetBlock(bx,by,z1);
					if(NULL!=blk)
					{
						++nTested;
						for(auto &edgePiece : blk->edgList)
						{
							edVtHd.Append(2,edgePiece.edVtHd);
							ceKey.Append(edgePiece.ceKey);
						}
					}
				}
			}
			for(int bz=z0+1; bz<=z1-1; ++bz)
			{
				for(int bx=x0; bx<=x1; ++bx)
				{
					auto blk=GetBlock(bx,y0,bz);
					if(NULL!=blk)
					{
						++nTested;
						for(auto &edgePiece : blk->edgList)
						{
							edVtHd.Append(2,edgePiece.edVtHd);
							ceKey.Append(edgePiece.ceKey);
						}
					}
					blk=GetBlock(bx,y1,bz);
					if(NULL!=blk)
					{
						++nTested;
						for(auto &edgePiece : blk->edgList)
						{
							edVtHd.Append(2,edgePiece.edVtHd);
							ceKey.Append(edgePiece.ceKey);
						}
					}
				}
			}
			for(int bz=z0+1; bz<=z1-1; ++bz)
			{
				for(int by=y0+1; by<=y1-1; ++by)
				{
					auto blk=GetBlock(x0,by,bz);
					if(NULL!=blk)
					{
						++nTested;
						for(auto &edgePiece : blk->edgList)
						{
							edVtHd.Append(2,edgePiece.edVtHd);
							ceKey.Append(edgePiece.ceKey);
						}
					}
					blk=GetBlock(x1,by,bz);
					if(NULL!=blk)
					{
						++nTested;
						for(auto &edgePiece : blk->edgList)
						{
							edVtHd.Append(2,edgePiece.edVtHd);
							ceKey.Append(edgePiece.ceKey);
						}
					}
				}
			}
		}
		else
		{
			auto blk=GetBlock(idx.x(),idx.y(),idx.z());
			if(NULL!=blk)
			{
				++nTested;
				for(auto &edgePiece : blk->edgList)
				{
					edVtHd.Append(2,edgePiece.edVtHd);
					ceKey.Append(edgePiece.ceKey);
				}
			}
		}

		return YSOK;
	}
	return YSERR;
}

YSBOOL YsShellLattice::CheckPolygonIntersection(YsShellPolygonHandle plHd) const
{
	int i;
	YsArray <YsShellPolygonHandle,16> collPlHdList;
	int nOrgPlVtHd;
	const YsShellVertexHandle *orgPlVtHd;
	YsCollisionOfPolygon collision;
	const YsShell &shl=*(this->shl);
	YsArray <YsVec3,4> plVtPos;

	shl.GetVertexListOfPolygon(plVtPos,plHd);
	collision.SetPolygon1(plVtPos.GetN(),plVtPos);

	shl.GetVertexListOfPolygon(nOrgPlVtHd,orgPlVtHd,plHd);

	MakePossibllyCollidingPolygonList(collPlHdList,plHd);
	forYsArray(i,collPlHdList)
	{
		int j,nPlVt;
		const YsShellVertexHandle *plVtHd;
		YsVec3  edVtPos[2];
		YsShellVertexHandle edVtHd[2];

		shl.GetVertexListOfPolygon(nPlVt,plVtHd,collPlHdList[i]);

		edVtHd[1]=plVtHd[nPlVt-1];
		shl.GetVertexPosition(edVtPos[1],plVtHd[nPlVt-1]);
		for(j=0; j<nPlVt; j++)
		{
			edVtHd[0]=edVtHd[1];
			edVtHd[1]=plVtHd[j];

			edVtPos[0]=edVtPos[1];
			shl.GetVertexPosition(edVtPos[1],plVtHd[j]);

			if(YsIsIncluded <YsShellVertexHandle> (nOrgPlVtHd,orgPlVtHd,edVtHd[0])==YSTRUE ||
			   YsIsIncluded <YsShellVertexHandle> (nOrgPlVtHd,orgPlVtHd,edVtHd[1])==YSTRUE)
			{
				goto NEXTEDGE;
			}

			if(collision.CheckIntersectionBetweenPolygon1AndLineSegment(edVtPos[0],edVtPos[1])==YSTRUE)
			{
				return YSTRUE;
			}
		NEXTEDGE:
			;
		}
	}
	return YSFALSE;
}

YSRESULT YsShellLattice::BeginSetDomain(const class YsShell &shl,YSSIZE_T nCell,const double margin)
{
	YsVec3 bbxMin,bbxMax;

	shl.GetBoundingBox(bbxMin,bbxMax);

	bbxMin.SubX(margin);
	bbxMin.SubY(margin);
	bbxMin.SubZ(margin);

	bbxMax.AddX(margin);
	bbxMax.AddY(margin);
	bbxMax.AddZ(margin);

	const YsVec3 d=bbxMax-bbxMin;
	const double vol=(d.x()*d.y()*d.z())/(double)nCell;

	const double siz=pow(vol,1.0/3.0);

	const YSSIZE_T nx=YsGreater <YSSIZE_T> (1,(YSSIZE_T)(d.x()/siz));
	const YSSIZE_T ny=YsGreater <YSSIZE_T> (1,(YSSIZE_T)(d.y()/siz));
	const YSSIZE_T nz=YsGreater <YSSIZE_T> (1,(YSSIZE_T)(d.z()/siz));

	return BeginSetDomain(shl,bbxMin,bbxMax,nx,ny,nz);
}

YSRESULT YsShellLattice::BeginSetDomain(const class YsShell &shl,const YsVec3 &bbx0,const YsVec3 &bbx1,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz)
{
	YsVec3 bbxMin,bbxMax;
	YsBoundingBoxMaker3 makeBbx;

	if(enablePlgToCellTable==YSTRUE)
	{
		plgToCellTable.PrepareTable();
	}
	if(NULL!=subLattice)
	{
		delete subLattice;
		subLattice=NULL;
	}

	makeBbx.Add(bbx0);
	makeBbx.Add(bbx1);
	makeBbx.Get(bbxMin,bbxMax);

	if(Create(nx,ny,nz,bbxMin,bbxMax)==YSOK)
	{
		AssignElementLock();

		this->shl=&shl;

		for(YSSIZE_T bz=0; bz<=GetNumBlockZ(); bz++)
		{
			for(YSSIZE_T by=0; by<=GetNumBlockY(); by++)
			{
				for(YSSIZE_T bx=0; bx<=GetNumBlockX(); bx++)
				{
					if(bx<GetNumBlockX() && by<GetNumBlockY() && bz<GetNumBlockZ())
					{
						GetEditableBlock(bx,by,bz)->side=YSUNKNOWNSIDE;
					}
					else
					{
						GetEditableBlock(bx,by,bz)->side=YSBOUNDARY;  // <- 2006/06/29
					}
				}
			}
		}

		xyPlane=YSTRUE;
		xzPlane=YSTRUE;
		yzPlane=YSTRUE;

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellLattice::FillUndeterminedBlock(int bx,int by,int bz,YSSIDE side)
{
	YsArray <YsFixedLengthArray <int,3>,4096> *todo;
	todo=new YsArray <YsFixedLengthArray <int,3>,4096>;
	if(todo!=NULL)
	{
		auto elm=GetEditableBlock(bx,by,bz);
		if(NULL!=elm && YSUNKNOWNSIDE==elm->side)
		{
			YsFixedLengthArray <int,3> b;
			b.dat[0]=bx;
			b.dat[1]=by;
			b.dat[2]=bz;
			todo->AppendItem(b);

			// This single line was missing for loooooong years.
			// Why didn't I notice it?  Why were my programs working?
			// It was ok when the lattice density was dense enough.
			// I was forgetting to set elm->side at bx,by,bz, but if 
			// at least one of the neighboring blocks was UNKNOWNSIDE,
			// the traversal will back fire to bx,by,bz, and elm->side
			// was updating from there.  However, when bx,by,bz was
			// surrounded by non-UNKNOWNSIDE blocks, neighboring blocks
			// was never added to the to-do list, therefore, no lukcy
			// backfire.  Since bx,by,bz was surrounded by non-UNKNOWNSIDE
			// blocks, no traversal from another block was leaking into 
			// bx,by,bz.
			elm->side=side;

			const int sixSide[6][3]=
			{
				{+1, 0, 0},
				{-1, 0, 0},
				{ 0,+1, 0},
				{ 0,-1, 0},
				{ 0, 0,+1},
				{ 0, 0,-1}
			};

			YSSIZE_T n;
			while((n=todo->GetN())>0)
			{
				const int x=(*todo)[n-1].dat[0];
				const int y=(*todo)[n-1].dat[1];
				const int z=(*todo)[n-1].dat[2];
				todo->Delete(n-1);

				for(int s=0; s<6; ++s)
				{
					YsFixedLengthArray <int,3> nei;
					nei.dat[0]=x+sixSide[s][0];
					nei.dat[1]=y+sixSide[s][1];
					nei.dat[2]=z+sixSide[s][2];
					auto elm=GetEditableBlock(nei.dat[0],nei.dat[1],nei.dat[2]);
					if(NULL!=elm && YSUNKNOWNSIDE==elm->side)
					{
						elm->side=side;
						todo->Append(nei);
					}
				}
			}
		}

		delete todo;
		return YSOK;
	}
	else
	{
		return RecursivelyFillUndeterminedBlock(bx,by,bz,side);
	}
}

YSRESULT YsShellLattice::RecursivelyFillUndeterminedBlock(int bx,int by,int bz,YSSIDE side,int limitter)
{
	if(limitter>0)
	{
		YsShellLatticeElem *elm;
		elm=GetEditableBlock(bx,by,bz);
		if(elm!=NULL && elm->side==YSUNKNOWNSIDE)
		{
			elm->side=side;
			RecursivelyFillUndeterminedBlock(bx+1,by  ,bz  ,side,limitter-1);
			RecursivelyFillUndeterminedBlock(bx-1,by  ,bz  ,side,limitter-1);
			RecursivelyFillUndeterminedBlock(bx  ,by+1,bz  ,side,limitter-1);
			RecursivelyFillUndeterminedBlock(bx  ,by-1,bz  ,side,limitter-1);
			RecursivelyFillUndeterminedBlock(bx  ,by  ,bz+1,side,limitter-1);
			RecursivelyFillUndeterminedBlock(bx  ,by  ,bz-1,side,limitter-1);
		}
	}
	return YSOK;
}

YSSIDE YsShellLattice::SophisticatedCheckInsideSolidWhenTheCellIsMarkedAsBoundary(const YsVec3 &pnt,YSBOOL /*showDetail*/) const
{
	// Idea : find nearest cell that is marked as inside or outside,
	//        shoot finite ray from pnt to the center of that cell,
	//        count number of intersection.

	int searchRange,searchRangeLimit;
	int i,bx=0,by=0,bz=0,cx=0,cy=0,cz=0;
	const YsShellLatticeElem *elm;

	GetBlockIndex(bx,by,bz,pnt);


	// But, first check if it is on boundary.
	elm=GetBlock(bx,by,bz);
	for(i=0; i<elm->plgList.GetNumItem(); i++)
	{
		YsArray <YsVec3,4> plg;
		YsShellPolygonHandle plHd;
		YsPlane pln;

		plHd=elm->plgList[i];
		if(shl->GetVertexListOfPolygon(plg,plHd)==YSOK)
		{
			YsVec3 prj;

			pln.MakeBestFitPlane(plg.GetNumItem(),plg.GetArray());
			if(pln.CheckOnPlane(pnt)==YSTRUE)
			{
				YSSIDE side;
				side=YsCheckInsidePolygon3(pnt,plg.GetNumItem(),plg.GetArray());
				if(side==YSBOUNDARY || side==YSINSIDE)
				{
					return YSBOUNDARY;
				}
			}
		}
	}


	searchRangeLimit=GetNumBlockX();
	searchRangeLimit=YsGreater(GetNumBlockY(),searchRangeLimit);
	searchRangeLimit=YsGreater(GetNumBlockZ(),searchRangeLimit);

	int failCount;
	failCount=0;
	for(searchRange=1; searchRange<searchRangeLimit && failCount<10; searchRange++)
	{
		int a,b;

		for(a=-searchRange; a<=searchRange && failCount<10; a++)
		{
			for(b=-searchRange; b<=searchRange && failCount<10; b++)
			{
				for(i=0; i<6; i++)
				{
					switch(i)
					{
					case 0:
						cx=bx+a;
						cy=by+b;
						cz=bz-searchRange;
						break;
					case 1:
						cx=bx+a;
						cy=by+b;
						cz=bz+searchRange;
						break;
					case 2:
						cx=bx+a;
						cy=by-searchRange;
						cz=bz+b;
						break;
					case 3:
						cx=bx+a;
						cy=by+searchRange;
						cz=bz+b;
						break;
					case 4:
						cx=bx-searchRange;
						cy=by+a;
						cz=bz+b;
						break;
					case 5:
						cx=bx+searchRange;
						cy=by+a;
						cz=bz+b;
						break;
					}

					elm=GetBlock(cx,cy,cz);

					YSSIDE blockSide=YSUNKNOWNSIDE;
					if(nullptr!=elm)
					{
						blockSide=elm->side;
					}
					// Added 2016/08/11
					if(YSTRUE!=isSubLattice && nullptr==elm)
					{
						blockSide=YSOUTSIDE;
					}

					if(blockSide==YSINSIDE || blockSide==YSOUTSIDE)
					{
						YSSIDE side;
						side=ShootFiniteRay(pnt,bx,by,bz,cx,cy,cz,blockSide);
						if(side!=YSUNKNOWNSIDE)
						{
							return side;
						}
						else if((++failCount)>=10)
						{
							YsErrOut("Failed 10 times.  Giving Up.\n");
							YsErrOut("Position %s\n",pnt.Txt());
							break;
						}
					}
				}
			}
		}
	}
	YsErrOut("YsShellLattice::SophisticatedCheckInsideSolidWhenTheCellIsMarkedAsBoundary()\n");
	YsErrOut("  Reached dead end!\n");
	return YSUNKNOWNSIDE;
}

YSSIDE YsShellLattice::ShootFiniteRay(const YsVec3 &pnt,int bx,int by,int bz,int cx,int cy,int cz,YSSIDE sideAtC) const
{
	// Idea : Shoot a finite ray from pnt to center of cx,cy,cz
	//        Count intersection.
	//        If even, return sideAtC
	//        If odd, return (sideAtC==YSINSIDE ? YSOUTSIDE : YSINSIDE)

	YsArray <YsShellPolygonHandle,16> plgList;
	if(MakeListOfPolygonsThatGoThroughTheBlockRegion(plgList,bx,by,bz,cx,cy,cz)==YSOK)
	{
		int i,nIntersection;
		YsArray <YsVec3,4> plg;
		YsVec3 blk1,blk2,rayTarget;
		YsShellPolygonHandle plHd;

		GetBlockRange(blk1,blk2,cx,cy,cz);
		rayTarget=(blk1+blk2)/2.0;


		if(checkInsideSolidLog==YSTRUE)
		{
			ray1=pnt;
			ray2=rayTarget;
		}

		nIntersection=0;
		for(i=0; i<plgList.GetNumItem(); i++)
		{
			plHd=plgList[i];
			if(shl->GetVertexListOfPolygon(plg,plHd)==YSOK)
			{
				YSBOOL hit;
				YsVec3 crs;
				if(CalculatePolygonRayIntersection(hit,crs,plg.GetN(),plg,pnt,rayTarget)==YSOK)
				{
					if(hit==YSTRUE)
					{
						if(checkInsideSolidLog==YSTRUE)
						{
							foundIntersection.AppendItem(crs);
							foundPolygon.AppendItem(plHd);
						}
						nIntersection++;
					}
				}
				else
				{
					return YSUNKNOWNSIDE;
				}
			}
			else
			{
				YsErrOut("YsShellLattice::ShootFiniteRay()\n");
				YsErrOut("  Cannot get polygon vertices\n");
				return YSUNKNOWNSIDE;
			}
		}

		if((nIntersection%2)==0)
		{
			return sideAtC;
		}
		else
		{
			return (sideAtC==YSINSIDE ? YSOUTSIDE : YSINSIDE);
		}
	}

	return YSUNKNOWNSIDE;
}

YSRESULT YsShellLattice::MakeListOfPolygonsThatGoThroughTheBlockRegion
    (YsArray <YsShellPolygonHandle,16> &plgList,int x1,int y1,int z1,int x2,int y2,int z2) const
{
	int bx,by,bz,bx1,by1,bz1,bx2,by2,bz2;
	int i;
	const YsShellLatticeElem *elm;
	YsShellPolygonHandle visitTest;
	YsHashTable <YsShellPolygonHandle> visited(255);

	bx1=YsSmaller(x1,x2);
	by1=YsSmaller(y1,y2);
	bz1=YsSmaller(z1,z2);
	bx2=YsGreater(x1,x2);
	by2=YsGreater(y1,y2);
	bz2=YsGreater(z1,z2);

	plgList.Set(0,NULL);
	for(bx=bx1; bx<=bx2; bx++)
	{
		for(by=by1; by<=by2; by++)
		{
			for(bz=bz1; bz<=bz2; bz++)
			{
				elm=GetBlock(bx,by,bz);
				if(elm!=NULL)
				{
					for(i=0; i<elm->plgList.GetN(); i++)
					{
						if(visited.FindElement(visitTest,shl->GetSearchKey(elm->plgList[i]))!=YSOK)
						{
							visited.AddElement(shl->GetSearchKey(elm->plgList[i]),elm->plgList[i]);
							plgList.Append(elm->plgList[i]);
						}
					}
				}
				// 2016/08/11
				//   Ignore out-of-range index.  Out-of-range index may be given from SophisticatedCheckInsideSolidWhenTheCellIsMarkedAsBoundary.
				//   If the lattice is not a sub-lattice, out-of-range blocks must be taken as YSOUTSIDE.
				// else
				// {
				// 	YsErrOut("YsShellLattice::MakeListOfPolygonsThatGoThroughTheBlockRegion()\n");
				// 	YsErrOut("  Block Index Out Of Range %d %d %d\n",bx,by,bz);
				// }
			}
		}
	}

	return YSOK;
}

YSSIDE YsShellLattice::ShootRay(
	YsArray <YsVec3,16> &itscPos,YsArray <YsShellPolygonHandle,16> &itscPlHd,
	const YsVec3 &org,const YsVec3 &vec,YSBOOL watch) const
{
	itscPos.Set(0,NULL);
	itscPlHd.Set(0,NULL);

	double l;
	const YsVec3 &p1=org;
	YsVec3 p2;
	YsArray <unsigned,64> blkList;
	YsHashTable <YsShellPolygonHandle> visited(255);
	YsShellPolygonHandle tst;
	YsArray <YsVec3,4> plg;
	YSBOOL hitEdge;

	l=(max-min).GetLength();
	p2=vec;
	hitEdge=YSFALSE;
	if(p2.Normalize()==YSOK)
	{
		p2=org+p2*(l*2.0);

		if(MakeIntersectingBlockList(blkList,p1,p2)==YSOK)
		{
			int b,p;
			int itscCount;
			itscCount=0;
			for(b=0; b<=blkList.GetN()-3; b+=3)
			{
				const YsShellLatticeElem *elm;
				elm=GetBlock(blkList[b],blkList[b+1],blkList[b+2]);
				for(p=0; p<elm->plgList.GetN(); p++)
				{
					if(visited.FindElement(tst,shl->GetSearchKey(elm->plgList[p]))!=YSOK)
					{
						if(watch==YSTRUE)
						{
							printf("PlId %d\n",shl->GetPolygonIdFromHandle(elm->plgList[p]));
						}


						YSBOOL hit;
						YsVec3 itsc;
						visited.AddElement(shl->GetSearchKey(elm->plgList[p]),elm->plgList[p]);
						shl->GetVertexListOfPolygon(plg,elm->plgList[p]);
						if(CalculatePolygonRayIntersection(hit,itsc,plg.GetN(),plg,p1,p2,watch)!=YSOK)
						{
							if(watch==YSTRUE)
							{
								printf("Hit Edge.\n");
							}
							hitEdge=YSTRUE;
						}

						if(watch==YSTRUE)
						{
							printf("Hit=%d\n",hit);
						}

						if(hit==YSTRUE)
						{
							itscPos.Append(itsc);
							itscPlHd.Append(elm->plgList[p]);
							itscCount++;
						}
					}
				}
			}

			if(hitEdge!=YSTRUE)
			{
				if(itscCount%2==0)
				{
					return YSINSIDE;
				}
				else
				{
					return YSOUTSIDE;
				}
			}
			
		}
	}
	return YSUNKNOWNSIDE;
}

YSRESULT YsShellLattice::ShootFiniteRay(
    YsArray <YsVec3,16> &itsc,YsArray <YsShellPolygonHandle,16> &itscPlHd,
    const YsVec3 &fr,const YsVec3 &to,
    int nPlHdExclude,const YsShellPolygonHandle plHdExclude[],YSBOOL watch) const
{
	return ShootFiniteRayWithPrecisionFlag(itsc,itscPlHd,fr,to,YSFALSE,nPlHdExclude,plHdExclude,watch);
}

YSRESULT YsShellLattice::ShootFiniteRayHighPrecision(
    YsArray <YsVec3,16> &itsc,YsArray <YsShellPolygonHandle,16> &itscPlHd,
    const YsVec3 &fr,const YsVec3 &to,
    int nPlHdExclude,const YsShellPolygonHandle plHdExclude[],YSBOOL watch) const
{
	return ShootFiniteRayWithPrecisionFlag(itsc,itscPlHd,fr,to,YSTRUE,nPlHdExclude,plHdExclude,watch);
}

YSRESULT YsShellLattice::ShootFiniteRayWithPrecisionFlag(
    YsArray <YsVec3,16> &itscPos,YsArray <YsShellPolygonHandle,16> &itscPlHd,
    const YsVec3 &fr,const YsVec3 &to,YSBOOL highPrecision,
    int nPlHdExclude,const YsShellPolygonHandle plHdExclude[],YSBOOL watch) const
{
	itscPos.Set(0,NULL);
	itscPlHd.Set(0,NULL);

	const YsVec3 &p1=fr;
	const YsVec3 &p2=to;
	const YsVec3 dir=to-fr;
	YsArray <unsigned,48> blkList;
	YsHashTable <YsShellPolygonHandle> visited(255);
	YsShellPolygonHandle tst;
	YsArray <YsVec3,4> plg;

	if(MakeIntersectingBlockList(blkList,p1,p2)==YSOK)
	{
		int b,k;
		int itscCount;
		itscCount=0;
		for(b=0; b<=blkList.GetN()-3; b+=3)
		{
			const YsShellLatticeElem *elm;
			elm=GetBlock(blkList[b],blkList[b+1],blkList[b+2]);
			for(k=0; k<elm->plgList.GetN(); k++)
			{
				if(visited.FindElement(tst,shl->GetSearchKey(elm->plgList[k]))!=YSOK)
				{
					const YsShellPolygonHandle plHd=elm->plgList[k];


					if(watch==YSTRUE)
					{
						printf("PlId=%d Blk=%d %d %d\n",shl->GetPolygonIdFromHandle(plHd),blkList[b],blkList[b+1],blkList[b+2]);
					}

					visited.AddElement(shl->GetSearchKey(plHd),plHd);

					if(YsIsIncluded <YsShellPolygonHandle> (nPlHdExclude,plHdExclude,plHd)!=YSTRUE && 
					   shl->GetVertexListOfPolygon(plg,plHd)==YSOK)
					{
						int j;
						int nForward;
						nForward=0;
						for(j=0; j<plg.GetN(); j++)
						{
							if((plg[j]-fr)*dir>0.0)
							{
								nForward++;
								break;
							}
						}

						if(nForward>0)
						{
							YsPlane pln;
							YsVec3 crs;
							if(pln.MakeBestFitPlane(plg.GetN(),plg)==YSOK)
							{
								if(watch==YSTRUE)
								{
									printf("GetItsc %d\n",pln.GetIntersection(crs,fr,dir));
									printf("Pos %s\n",crs.Txt());
									printf("DotProd %lf\n",(crs-fr)*dir);
								}

								const YSRESULT haveItsc=
								    (YSTRUE==highPrecision ? pln.GetIntersectionHighPrecision(crs,fr,dir) : pln.GetIntersection(crs,fr,dir));
								if(YSOK==haveItsc && YSTRUE==YsCheckInBetween3(crs,fr,to))
								{
									YSSIDE side;
									if(plg.GetN()==3)
									{
										side=YsCheckInsideTriangle3(crs,plg);
									}
									else
									{
									 	side=YsCheckInsidePolygon3(crs,plg.GetN(),plg,pln.GetNormal());
									}
									if(watch==YSTRUE)
									{
										printf("Side %d\n",side);
									}

								 	if(side==YSBOUNDARY || side==YSINSIDE)
								 	{
										itscPos.Append(crs);
										itscPlHd.Append(plHd);
									}
								}
							}
						}
					}
				}
			}
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellLattice::CalculatePolygonRayIntersection
	(YSBOOL &hit,YsVec3 &itsc,YSSIZE_T np,const YsVec3 p[],const YsVec3 &ray1,const YsVec3 &ray2,YSBOOL watch) const
{
	hit=YSFALSE;

	if(np==0)
	{
		return YSERR;
	}

	int j;
	int nForward;
	nForward=0;
	for(j=0; j<np; j++)
	{
		if((p[j]-ray1)*(ray2-ray1)>-YsTolerance)
		{
			nForward++;
			break;
		}
	}

	if(watch==YSTRUE)
	{
		printf("nForward=%d\n",nForward);
	}

	if(nForward>0)
	{
		YsPlane pln;
		if(pln.MakeBestFitPlane(np,p)==YSOK)
		{
			if(pln.GetPenetration(itsc,ray1,ray2)==YSOK)
			{
				YSSIDE side;
			 	side=YsCheckInsidePolygon3(itsc,np,p);

				if(watch==YSTRUE)
				{
					printf("itsc %s side %d\n",itsc.Txt(),side);
				}

			 	if(side==YSBOUNDARY)
			 	{
					hit=YSTRUE;
					return YSERR;
				}
				else if(side==YSINSIDE)
				{
					hit=YSTRUE;
					return YSOK;
				}
				else
				{
					hit=YSFALSE;
					return YSOK;
				}
			}
		}
		else
		{
			// It's a bad polygon, but it is ok as long as it is far from the ray.
			int m;
			YsVec3 p1,p2,neaP,neaQ;
			YSRESULT err;
			err=YSOK;
			p2=p[np-1];
			for(m=0; m<np; m++)
			{
				p1=p2;
				p2=p[m];
				if(p1==ray1 || p1==ray2 || p2==ray1 || p2==ray2)
				{
					err=YSERR;
					break;
				}
				if(YsGetNearestPointOfTwoLine(neaP,neaQ,p1,p2,ray1,ray2)==YSOK && neaP==neaQ)
				{
					err=YSERR;
					break;
				}
			}
			if(err!=YSOK)
			{
				YsErrOut("YsShellLattice::CalculatePolygonRayIntersection()\n");
				YsErrOut("  The ray hits a bad polygon\n");
				return YSERR;
			}
		}
	}
	return YSOK;
}

YsShellPolygonHandle YsShellLattice::FindNearestPolygon(
    double &nearDist,YsVec3 &nearPos,const YsVec3 &from,YSBOOL att4Test,int att4Value) const
{
	return FindNearestPolygonWithPrecisionFlag(nearDist,nearPos,from,YSFALSE,att4Test,att4Value);
}

YsShellPolygonHandle YsShellLattice::FindNearestPolygonHighPrecision(
    double &nearDist,YsVec3 &nearPos,const YsVec3 &from,YSBOOL att4Test,int att4Value) const
{
	return FindNearestPolygonWithPrecisionFlag(nearDist,nearPos,from,YSTRUE,att4Test,att4Value);
}

YsShellPolygonHandle YsShellLattice::FindNearestPolygonWithPrecisionFlag(
    double &nearDist,YsVec3 &nearPos,const YsVec3 &from,YSBOOL highPrecision,YSBOOL att4Test,int att4Value) const
{
	int x0,y0,z0;
	YsHashTable <int> visited(16);
	visited.EnableAutoResizing(16,256);

	YSBOOL first;
	YsShellPolygonHandle nearPlHd;
	YsArray <YsVec3,16> plVtPos;
	double lBlkMin;

	YSBOOL watch;

	watch=YSFALSE;

	lBlkMin=YsSmallestOf(lxBlk,lyBlk,lzBlk);

	nearPlHd=NULL;
	nearDist=YsInfinity*YsInfinity;
	first=YSTRUE;

	YSSIZE_T range,range0,searchRange;

	if(GetBlockIndex(x0,y0,z0,from)==YSOK)
	{
		range0=0;
		searchRange=YsGreatestOf(nBlkX,nBlkY,nBlkZ);
	}
	else
	{
		YSSIZE_T dx,dy,dz,lx,ly,lz;

		GetBlockIndexUnconstrained(x0,y0,z0,from);

		dx=(x0<0 ? -x0 : x0-GetNumBlockX()+1);
		dy=(y0<0 ? -y0 : y0-GetNumBlockY()+1);
		dz=(z0<0 ? -z0 : z0-GetNumBlockZ()+1);
		range0=YsGreatestOf(dx,dy,dz);

		lx=dx+nBlkX;
		ly=dy+nBlkY;
		lz=dz+nBlkZ;
		searchRange=YsGreatestOf(lx,ly,lz);
	}

	YSSIZE_T xMin,xMax,yMin,yMax,zMin,zMax;
	xMin=0;
	yMin=0;
	zMin=0;
	xMax=nBlkX-1;
	yMax=nBlkY-1;
	zMax=nBlkZ-1;

	for(range=range0; range<=searchRange; range++)  // This must be <=   See memo/20061208.doc
	{
		int i,j,pln;
		YSSIZE_T du,dv;
		YSSIZE_T bx=0,by=0,bz=0;

		if(watch==YSTRUE)
		{
			YsPrintf("Range %d/%d\n",(int)range,(int)searchRange);
		}

		for(du=-range; du<=range; du++)
		{
			for(dv=-range; dv<=range; dv++)
			{
				for(pln=0; pln<6; pln++)
				{
					switch(pln)
					{
					case 0:
						bx=x0-range;
						by=y0+du;
						bz=z0+dv;
						break;
					case 1:
						bx=x0+range;
						by=y0+du;
						bz=z0+dv;
						break;
					case 2:
						bx=x0+du;
						by=y0+range;
						bz=z0+dv;
						break;
					case 3:
						bx=x0+du;
						by=y0-range;
						bz=z0+dv;
						break;
					case 4:
						bx=x0+du;
						by=y0+dv;
						bz=z0+range;
						break;
					case 5:
						bx=x0+du;
						by=y0+dv;
						bz=z0-range;
						break;
					}

					// dx=YsAbs(bx-x0)+1;   <- Apparently, it's wrong
					// dy=YsAbs(by-y0)+1;
					// dz=YsAbs(bz-z0)+1;
					// lx=(double)dx*lxBlk;
					// ly=(double)dy*lyBlk;
					// lz=(double)dz*lzBlk;
					// if(nearDist<lx*lx+ly*ly+lz*lz)
					// {
					// 	continue;
					// }

					const YsShellLatticeElem *elm;
					elm=GetBlock(bx,by,bz);
					if(elm!=NULL && elm->plgList.GetN()>0)
					{
						int nPlVt;
						const YsShellVertexHandle *plVtHd;
						YsVec3 plNom,vtPos,plCen;
						double dist;
						YsPlane pln;

						for(i=0; i<elm->plgList.GetN(); i++)
						{
							if(att4Test==YSTRUE && shl->GetPolygon(elm->plgList[i])->freeAttribute4!=att4Value)
							{
								continue;
							}

							if(visited.CheckKeyExist(shl->GetSearchKey(elm->plgList[i]))!=YSTRUE)
							{
								YSBOOL hit;
								hit=YSFALSE;

								visited.AddElement(shl->GetSearchKey(elm->plgList[i]),0);

								shl->GetVertexListOfPolygon(nPlVt,plVtHd,elm->plgList[i]);
								shl->GetNormalOfPolygon(plNom,elm->plgList[i]);

								plVtPos.Set(0,NULL);
								plCen=YsOrigin();
								for(j=0; j<nPlVt; j++)
								{
									shl->GetVertexPosition(vtPos,plVtHd[j]);
									dist=(vtPos-from).GetSquareLength();
									plVtPos.Append(vtPos);
									plCen+=vtPos;
									if(first==YSTRUE || dist<nearDist)
									{
										first=YSFALSE;
										hit=YSTRUE;

										nearDist=dist;
										nearPlHd=elm->plgList[i];
										nearPos=vtPos;
									}
								}

								plCen/=(double)nPlVt;
								if(YSTRUE!=highPrecision && plNom!=YsOrigin())
								{
									pln.Set(plCen,plNom);
								}
								else
								{
									pln.MakeBestFitPlane(nPlVt,plVtPos);
								}

								YSRESULT haveNearPos=(YSTRUE==highPrecision ? pln.GetNearestPointHighPrecision(vtPos,from) : pln.GetNearestPoint(vtPos,from));
								if(haveNearPos==YSOK)
								{
									dist=(vtPos-from).GetSquareLength();
									if(YSTRUE==first || dist<nearDist)
									{
										YSSIDE side;
										if(nPlVt==3)
										{
											side=YsCheckInsideTriangle3(vtPos,plVtPos);
										}
										else if(plNom!=YsOrigin())
										{
											side=YsCheckInsidePolygon3(vtPos,plVtPos.GetN(),plVtPos,plNom);
										}
										else
										{
											side=YsCheckInsidePolygon3(vtPos,plVtPos.GetN(),plVtPos);
										}

										if(side==YSINSIDE || side==YSBOUNDARY)
										{
											first=YSFALSE;
											hit=YSTRUE;

											nearPlHd=elm->plgList[i];
											nearPos=vtPos;
											nearDist=dist;
										}
									}
								}

								int prevJ;
								prevJ=nPlVt-1;
								for(j=0; j<nPlVt; j++)
								{
									if(YsGetNearestPointOnLine3(vtPos,plVtPos[prevJ],plVtPos[j],from)==YSOK &&
									   YsCheckInBetween3(vtPos,plVtPos[prevJ],plVtPos[j])==YSTRUE)
									{
										dist=(vtPos-from).GetSquareLength();
										if(YSTRUE==first || dist<nearDist)
										{
											first=YSFALSE;
											hit=YSTRUE;

											nearPlHd=elm->plgList[i];
											nearPos=vtPos;
											nearDist=dist;
										}
									}
									prevJ=j;
								}

								if(hit==YSTRUE)
								{
									YsVec3 dif;
									dif=nearPos-from;
									dist=dif.GetLength();

									YSSIZE_T rangeX,rangeY,rangeZ,newRange;
									rangeX=YsSmaller <YSSIZE_T> (1+(int)(dist/lxBlk),nBlkX);
									rangeY=YsSmaller <YSSIZE_T> (1+(int)(dist/lyBlk),nBlkY);
									rangeZ=YsSmaller <YSSIZE_T> (1+(int)(dist/lzBlk),nBlkZ);
									if(xyPlane==YSTRUE)
									{
										rangeZ=0;
									}
									if(xzPlane==YSTRUE)
									{
										rangeY=0;
									}
									if(yzPlane==YSTRUE)
									{
										rangeX=0;
									}
									newRange=YsGreatestOf(rangeX,rangeY,rangeZ);

									searchRange=YsSmaller(searchRange,newRange);

									if(watch==YSTRUE)
									{
										YsPrintf("Search Range Updated to %d\n",(int)newRange);
									}

									// dist/=lBlkMin;
									// searchRange=YsSmaller(searchRange,1+(int)dist);
								}
							}
						}
					}

					if(range==0)
					{
						break;
					}
				}
			}
		}
	}

	if(nearPlHd!=NULL)  // 2007/07/05
	{
		nearDist=sqrt(nearDist);
	}

	return nearPlHd;
}

YSRESULT YsShellLattice::FindNearestEdgePiece
    (YsShellVertexHandle nearEdVtHd[2],YSHASHKEY &nearCeKey,double &nearDist,YsVec3 &nearPos,const YsVec3 &from,
     const YsVec3 &dissimilarConst) const
{
	int x0,y0,z0;
	YsFixedLengthToMultiHashTable <int,2,1> visited(16);
	visited.EnableAutoResizing(16,256);

	nearEdVtHd[0]=NULL;
	nearEdVtHd[1]=NULL;
	nearCeKey=YSNULLHASHKEY;


	YSSIZE_T range0,searchRange; 

	if(GetBlockIndex(x0,y0,z0,from)==YSOK)
	{
		range0=0;
		searchRange=YsGreatestOf(nBlkX,nBlkY,nBlkZ);
	}
	else
	{
		YSSIZE_T dx,dy,dz,lx,ly,lz;

		GetBlockIndexUnconstrained(x0,y0,z0,from);

		dx=(x0<0 ? -x0 : x0-GetNumBlockX()+1);
		dy=(y0<0 ? -y0 : y0-GetNumBlockY()+1);
		dz=(z0<0 ? -z0 : z0-GetNumBlockZ()+1);
		range0=YsGreatestOf(dx,dy,dz);

		lx=dx+nBlkX;
		ly=dy+nBlkY;
		lz=dz+nBlkZ;
		searchRange=YsGreatestOf(lx,ly,lz);
	}


	YSBOOL testDissimilarity;
	testDissimilarity=(dissimilarConst!=YsOrigin() ? YSTRUE : YSFALSE);

	YsVec3 edVtPos[2];
	double lBlkMin;
	lBlkMin=YsSmallestOf(lxBlk,lyBlk,lzBlk);

	nearDist=YsInfinity*YsInfinity;

	for(auto range=range0; range<=searchRange; range++)  // This must be <=   See memo/20061208.doc
	{
		YSSIZE_T i,j,pln,du,dv,du0,du1,dv0,dv1,bx,by,bz;
		YSSIZE_T *uPtr,*vPtr;
		// double dx,dy,dz,lx,ly,lz;
		for(pln=0; pln<6; pln++)
		{
			du0=-range;
			du1= range;
			dv0=-range;
			dv1= range;
			switch(pln)
			{
			default:
			case 0:  // bx=x0-range  U->y  V->z
				if(x0-range<0)
				{
					continue;
				}
				du0=YsGreater <YSSIZE_T> (0    ,y0-range);
				du1=YsSmaller <YSSIZE_T> (nBlkY,y0+range);
				dv0=YsGreater <YSSIZE_T> (0    ,z0-range);
				dv1=YsSmaller <YSSIZE_T> (nBlkZ,z0+range);
				bx=x0-range;
				uPtr=&by;
				vPtr=&bz;
				break;
			case 1:  // bx=x0+range
				if(nBlkX<x0+range)
				{
					continue;
				}
				du0=YsGreater <YSSIZE_T> (0    ,y0-range);
				du1=YsSmaller <YSSIZE_T> (nBlkY,y0+range);
				dv0=YsGreater <YSSIZE_T> (0    ,z0-range);
				dv1=YsSmaller <YSSIZE_T> (nBlkZ,z0+range);
				bx=x0+range;
				uPtr=&by;
				vPtr=&bz;
				break;
			case 2:  // by=y0+range  U->X  V->Z
				if(nBlkY<y0+range)
				{
					continue;
				}
				du0=YsGreater <YSSIZE_T> (0    ,x0-range);
				du1=YsSmaller <YSSIZE_T> (nBlkX,x0+range);
				dv0=YsGreater <YSSIZE_T> (0    ,z0-range);
				dv1=YsSmaller <YSSIZE_T> (nBlkZ,z0+range);
				by=y0+range;
				uPtr=&bx;
				vPtr=&bz;
				break;
			case 3:  // by=y0-range
				if(y0-range<0)
				{
					continue;
				}
				du0=YsGreater <YSSIZE_T> (0    ,x0-range);
				du1=YsSmaller <YSSIZE_T> (nBlkX,x0+range);
				dv0=YsGreater <YSSIZE_T> (0    ,z0-range);
				dv1=YsSmaller <YSSIZE_T> (nBlkZ,z0+range);
				by=y0-range;
				uPtr=&bx;
				vPtr=&bz;
				break;
			case 4:  // bz=z0+range  U->X  V->Y
				if(nBlkZ<z0+range)
				{
					continue;
				}
				du0=YsGreater <YSSIZE_T> (0    ,x0-range);
				du1=YsSmaller <YSSIZE_T> (nBlkX,x0+range);
				dv0=YsGreater <YSSIZE_T> (0    ,y0-range);
				dv1=YsSmaller <YSSIZE_T> (nBlkY,y0+range);
				bz=z0+range;
				uPtr=&bx;
				vPtr=&by;
				break;
			case 5:  // bz=z0-range
				if(z0-range<0)
				{
					continue;
				}
				du0=YsGreater <YSSIZE_T> (0    ,x0-range);
				du1=YsSmaller <YSSIZE_T> (nBlkX,x0+range);
				dv0=YsGreater <YSSIZE_T> (0    ,y0-range);
				dv1=YsSmaller <YSSIZE_T> (nBlkY,y0+range);
				bz=z0-range;
				uPtr=&bx;
				vPtr=&by;
				break;
			}

			for(du=du0; du<=du1; du++)
			{
				for(dv=dv0; dv<=dv1; dv++)
				{
//					switch(pln)
//					{
//					case 0:
//						bx=x0-range;
//						by=du;
//						bz=dv;
//						break;
//					case 1:
//						bx=x0+range;
//						by=du;
//						bz=dv;
//						break;
//					case 2:
//						bx=du;
//						by=y0+range;
//						bz=dv;
//						break;
//					case 3:
//						bx=du;
//						by=y0-range;
//						bz=dv;
//						break;
//					case 4:
//						bx=du;
//						by=dv;
//						bz=z0+range;
//						break;
//					case 5:
//						bx=du;
//						by=dv;
//						bz=z0-range;
//						break;
//					}

					*uPtr=du;
					*vPtr=dv;


					//dx=YsAbs(bx-x0);
					//dy=YsAbs(by-y0);
					//dz=YsAbs(bz-z0);
					//lx=(double)dx*lxBlk;
					//ly=(double)dy*lyBlk;
					//lz=(double)dz*lzBlk;
					//if(nearDist<lx*lx+ly*ly+lz*lz)
					//{
					//	continue;
					//}



					const YsShellLatticeElem *elm;
					elm=GetBlock(bx,by,bz);
					if(elm!=NULL)
					{
						double dist;

						for(i=0; i<elm->edgList.GetN(); i++)
						{
							unsigned edVtKey[2];
							edVtKey[0]=shl->GetSearchKey(elm->edgList[i].edVtHd[0]);
							edVtKey[1]=shl->GetSearchKey(elm->edgList[i].edVtHd[1]);

							if(visited.CheckKeyExist(2,edVtKey)!=YSTRUE)
							{
								YSBOOL hit;
								hit=YSFALSE;

								visited.AddElement(2,edVtKey,0);

								shl->GetVertexPosition(edVtPos[0],elm->edgList[i].edVtHd[0]);
								shl->GetVertexPosition(edVtPos[1],elm->edgList[i].edVtHd[1]);

								if(testDissimilarity==YSTRUE)
								{
									YsVec3 vec;
									vec=edVtPos[1]-edVtPos[0];
									if(vec.Normalize()!=YSOK || fabs(vec*dissimilarConst)>0.707107)
									{
										continue;  // next i
									}
								}

								for(j=0; j<2; j++)
								{
									dist=(edVtPos[j]-from).GetSquareLength();
									if(nearEdVtHd[0]==NULL || dist<nearDist)
									{
										hit=YSTRUE;

										nearDist=dist;
										nearEdVtHd[0]=elm->edgList[i].edVtHd[0];
										nearEdVtHd[1]=elm->edgList[i].edVtHd[1];
										nearCeKey=elm->edgList[i].ceKey;
										nearPos=edVtPos[j];
									}
								}

								YsVec3 tst;
								if(YsGetNearestPointOnLine3(tst,edVtPos[0],edVtPos[1],from)==YSOK &&
								   YsCheckInBetween3(tst,edVtPos[0],edVtPos[1])==YSTRUE)
								{
									dist=(tst-from).GetSquareLength();
									if(NULL==nearEdVtHd[0] || dist<nearDist)
									{
										hit=YSTRUE;
										nearDist=dist;
										nearEdVtHd[0]=elm->edgList[i].edVtHd[0];
										nearEdVtHd[1]=elm->edgList[i].edVtHd[1];

										nearCeKey=elm->edgList[i].ceKey;
										nearPos=tst;
									}
								}

								if(hit==YSTRUE)
								{
									YsVec3 dif;
									dif=nearPos-from;
									dist=dif.GetLength();

									YSSIZE_T rangeX,rangeY,rangeZ,newRange;
									rangeX=YsSmaller <YSSIZE_T> (1+(int)(dist/lxBlk),nBlkX);
									rangeY=YsSmaller <YSSIZE_T> (1+(int)(dist/lyBlk),nBlkY);
									rangeZ=YsSmaller <YSSIZE_T> (1+(int)(dist/lzBlk),nBlkZ);
									if(xyPlane==YSTRUE)
									{
										rangeZ=0;
									}
									if(xzPlane==YSTRUE)
									{
										rangeY=0;
									}
									if(yzPlane==YSTRUE)
									{
										rangeX=0;
									}
									newRange=YsGreatestOf(rangeX,rangeY,rangeZ);

									searchRange=YsSmaller(searchRange,newRange);

									// dist/=lBlkMin;
									// searchRange=YsSmaller(searchRange,1+(int)dist);
								}
							}
						}
					}

					if(range==0)
					{
						break;
					}
				}
			}
		}
	}

	if(nearEdVtHd[0]!=NULL)
	{
		nearDist=sqrt(nearDist);
		return YSOK;
	}
	return YSERR;
}

YsShell::Edge YsShellLattice::FindNearestEdgePiece(YSHASHKEY &ceKey,double &dist,YsVec3 &nearPos,const YsVec3 &from,const YsVec3 &dissimilarConst) const
{
	YsShell::Edge edge;
	if(YSOK!=FindNearestEdgePiece(edge.edVtHd,ceKey,dist,nearPos,from,dissimilarConst))
	{
		edge.edVtHd[0]=NULL;
		edge.edVtHd[1]=NULL;
	}
	return edge;
}

YSRESULT YsShellLattice::PreparePolygon
    (YsArray <YsVec3,16> &plg,int &bx1,int &by1,int &bz1,int &bx2,int &by2,int &bz2,YsShellPolygonHandle plHd,const double thickness)
{
	YsVec3 bbxMin,bbxMax;

	// Get Vertex List
	if(shl->GetVertexListOfPolygon(plg,plHd)==YSOK)
	{
		// Get Bounding Box
		YsBoundingBoxMaker3 bbx;
		bbx.Begin(plg[0]);
		for(YSSIZE_T i=1; i<plg.GetN(); i++)
		{
			bbx.Add(plg[i]);
		}

		bbx.Get(bbxMin,bbxMax);

		bbxMin.SubX(thickness);
		bbxMin.SubY(thickness);
		bbxMin.SubZ(thickness);

		bbxMax.AddX(thickness);
		bbxMax.AddY(thickness);
		bbxMax.AddZ(thickness);

		// Get Block Index Range of Bounding Box
		if(GetBlockIndexAutoBound(bx1,by1,bz1,bbxMin)==YSOK &&
		   GetBlockIndexAutoBound(bx2,by2,bz2,bbxMax)==YSOK)
		{
			return YSOK;
		}
	}
	return YSERR;
}

