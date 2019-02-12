/* ////////////////////////////////////////////////////////////

File Name: yslattice.h
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

#ifndef YSLATTICE_IS_INCLUDED
#define YSLATTICE_IS_INCLUDED
/* { */

#include "yspredefined.h"
#include "yswizard.h"
#include "ysveci.h"
#include "ysclip.h"
#include "ysadvgeometry.h"

template <class T>
class YsLattice2d
{
public:
	YsLattice2d();
	YsLattice2d(const YsLattice2d <T> &from);
	YsLattice2d(YSSIZE_T nx,YSSIZE_T ny,const YsVec2 &min,const YsVec2 &max);
	~YsLattice2d();
	void CleanUp(void);

	/*! Returns the dimension of the whole lattice. */
	YsVec2 GetLatticeDimension(void) const;

	const YsLattice2d <T> &operator=(const YsLattice2d <T> &from);

	YSRESULT Create(YSSIZE_T nBlk,const YsVec2 minmax[2]);
	YSRESULT Create(YSSIZE_T nBlk,const YsVec2 &min,const YsVec2 &max);
	YSRESULT Create(YSSIZE_T nBlkX,YSSIZE_T nBlkY,const YsVec2 &min,const YsVec2 &max);

	YSRESULT GetBlockIndex(int &x,int &y,const YsVec2 &pos) const;
	YSRESULT GetBlockIndex(YsVec2i &idx,const YsVec2 &pos) const;
	YSRESULT GetBlockIndexAutoBound(int &x,int &y,const YsVec2 &pos) const;
	YSRESULT GetBlockIndexAutoBound(YsVec2i &idx,const YsVec2 &pos) const;
	YsVec2i GetBlockIndex(const YsVec2 &pos) const;
	YsVec2i GetBlockIndexAutoBound(const YsVec2 &pos) const;

	YSSIZE_T GetLinearIndex(const YsVec2i idx) const;

	T *GetEditableBlock(YSSIZE_T x,YSSIZE_T y);  // Left for backward compatibility

	T *GetBlock(YSSIZE_T x,YSSIZE_T y);
	const T *GetBlock(YSSIZE_T x,YSSIZE_T y) const;
	T *GetBlock(const YsVec2i &idx);
	const T *GetBlock(const YsVec2i &idx) const;
	T &operator[](const YsVec2i &idx);
	const T &operator[](const YsVec2i &idx) const;

	const YsVec2 GetBlockDimension(void) const;
	void GetBlockRange(YsVec2 &mn,YsVec2 &mx,YSSIZE_T bx,YSSIZE_T by) const;
	void GetBlockRect(YsVec2 rect[4],YSSIZE_T bx,YSSIZE_T by) const;
	void GetBlockRect(YsVec2 rect[4],const YsVec2i &idx) const;
	void GetGridPosition(YsVec2 &pos,YSSIZE_T bx,YSSIZE_T by) const;
	const YsVec2 GetGridPosition(YSSIZE_T bx,YSSIZE_T by) const;
	const YsVec2 GetGridPosition(const YsVec2i &idx) const;

	YSRESULT SetBlock(YSSIZE_T x,YSSIZE_T y,const T &blk);

	int GetNumBlockX(void) const;
	int GetNumBlockY(void) const;

	void GetLatticeRange(YsVec2 &min,YsVec2 &max) const;

protected:
	YSRESULT Release(void);

	YsVec2 min,max;
	YSSIZE_T nBlkX,nBlkY;
	double lxBlk,lyBlk;
	T *blk;
};

template <class T>
YsLattice2d <T>::YsLattice2d()
{
	nBlkX=0;
	nBlkY=0;
	lxBlk=0.0;
	lyBlk=0.0;
	min.Set(0.0,0.0);
	max.Set(0.0,0.0);
	blk=nullptr;
}

template <class T>
YsLattice2d <T>::YsLattice2d(const YsLattice2d <T> &from)
{
	blk=nullptr;	// 2019/01/25  Was missing.  This bug was hiding for almost two decades.
	*this=from;
}

template <class T>
YsLattice2d <T>::YsLattice2d(YSSIZE_T nx,YSSIZE_T ny,const YsVec2 &mn,const YsVec2 &mx)
{
	nBlkX=0;
	nBlkY=0;
	lxBlk=0.0;
	lyBlk=0.0;
	min.Set(0.0,0.0);
	max.Set(0.0,0.0);
	blk=NULL;  // <- Necessary because Create function will call Release function
	Create(nx,ny,mn,mx);
}

template <class T>
YsLattice2d <T>::~YsLattice2d()
{
	Release();
}

template <class T>
YsVec2 YsLattice2d<T>::GetLatticeDimension(void) const
{
	return max-min;
}

template <class T>
void YsLattice2d <T>::CleanUp(void)
{
	Release();
}


template <class T>
const YsLattice2d <T> &YsLattice2d <T>::operator=(const YsLattice2d <T> &from)
{
	Release();

	nBlkX=from.nBlkX;
	nBlkY=from.nBlkY;

	lxBlk=from.lxBlk;
	lyBlk=from.lyBlk;

	min=from.min;
	max=from.max;

	blk=new T [(nBlkX+1)*(nBlkY+1)];

	int i;
	for(i=0; i<(nBlkX+1)*(nBlkY+1); i++)
	{
		blk[i]=from.blk[i];
	}

	return *this;
}

template <class T>
YSRESULT YsLattice2d<T>::Create(YSSIZE_T nBlk,const YsVec2 minmax[2])
{
	return Create(nBlk,minmax[0],minmax[1]);
}

template <class T>
YSRESULT YsLattice2d <T>::Create(YSSIZE_T nBlk,const YsVec2 &min,const YsVec2 &max)
{
	int nx,ny;
	double area,lBlk,lx,ly;

	lx=fabs(max.x()-min.x());
	ly=fabs(max.y()-min.y());

	area=fabs(lx*ly)/(double)nBlk;

	lBlk=sqrt(area);

	nx=YsGreater(1,(int)(lx/lBlk));
	ny=YsGreater(1,(int)(ly/lBlk));

	return Create(nx,ny,min,max);
}


template <class T>
YSRESULT YsLattice2d <T>::Create(YSSIZE_T nx,YSSIZE_T ny,const YsVec2 &mn,const YsVec2 &mx)
{
	Release();

	T *newBlk;
	newBlk=new T [(nx+1)*(ny+1)];

	if(newBlk!=NULL)
	{
		nBlkX=nx;
		nBlkY=ny;

		min.Set(YsSmaller(mn.x(),mx.x()),YsSmaller(mn.y(),mx.y()));
		max.Set(YsGreater(mn.x(),mx.x()),YsGreater(mn.y(),mx.y()));

		lxBlk=(max.x()-min.x())/double(nx);
		lyBlk=(max.y()-min.y())/double(ny);

		blk=newBlk;

		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


template <class T>
YSRESULT YsLattice2d <T>::GetBlockIndex(int &x,int &y,const YsVec2 &pos) const
{
	if(min.x()<=pos.x() && pos.x()<max.x() && min.y()<=pos.y() && pos.y()<max.y())
	{
		YsVec2 rel;
		rel=pos-min;

		int bx,by;
		bx=int(rel.x()/lxBlk);
		by=int(rel.y()/lyBlk);

		if(0<=bx && bx<nBlkX && 0<=by && by<nBlkY) // Redundant check
		{
			x=bx;
			y=by;
			return YSOK;
		}
		else
		{
			// 2016/07/27 Can make an out-of-range error, but the position is already checked against the min-max.
			//            If it gets out of the index range, it must be on the min or max, and should
			//            be safe to round it.
			x=YsBound<int>(bx,0,(int)nBlkX-1);
			y=YsBound<int>(by,0,(int)nBlkY-1);
			return YSOK;
		}
	}
	x=0;
	y=0;
	return YSERR;
}

template <class T>
YSRESULT YsLattice2d <T>::GetBlockIndexAutoBound(int &x,int &y,const YsVec2 &pos) const
{
	YsVec2 bnd;
	bnd.Set(YsBound(pos.x(),min.x()+YsTolerance,max.x()-YsTolerance),
	        YsBound(pos.y(),min.y()+YsTolerance,max.y()-YsTolerance));
	return GetBlockIndex(x,y,bnd);
}

template <class T>
YSRESULT YsLattice2d <T>::GetBlockIndex(YsVec2i &idx,const YsVec2 &pos) const
{
	int x,y;
	if(YSOK==GetBlockIndex(x,y,pos))
	{
		idx.Set(x,y);
		return YSOK;
	}
	return YSERR;
}

template <class T>
YSRESULT YsLattice2d <T>::GetBlockIndexAutoBound(YsVec2i &idx,const YsVec2 &pos) const
{
	int x,y;
	if(YSOK==GetBlockIndexAutoBound(x,y,pos))
	{
		idx.Set(x,y);
		return YSOK;
	}
	return YSERR;
}

template <class T>
YsVec2i YsLattice2d <T>::GetBlockIndex(const YsVec2 &pos) const
{
	YsVec2i idx;
	GetBlockIndex(idx,pos);
	return idx;
}

template <class T>
YsVec2i YsLattice2d <T>::GetBlockIndexAutoBound(const YsVec2 &pos) const
{
	YsVec2i idx;
	GetBlockIndexAutoBound(idx,pos);
	return idx;
}

template <class T>
YSSIZE_T YsLattice2d<T>::GetLinearIndex(const YsVec2i idx) const
{
	return idx.y()*(nBlkX+1)+idx.x();
}

template <class T>
T *YsLattice2d <T>::GetEditableBlock(YSSIZE_T x,YSSIZE_T y)
{
	return (T *)GetBlock(x,y);
}

template <class T>
T *YsLattice2d <T>::GetBlock(YSSIZE_T x,YSSIZE_T y)
{
	if(0<=x && x<nBlkX && 0<=y && y<nBlkY)
	{
		return &blk[y*(nBlkX+1)+x];
	}
	else
	{
		return NULL;
	}
}

template <class T>
const T *YsLattice2d <T>::GetBlock(YSSIZE_T x,YSSIZE_T y) const
{
	if(0<=x && x<nBlkX && 0<=y && y<nBlkY)
	{
		return &blk[y*(nBlkX+1)+x];
	}
	else
	{
		return NULL;
	}
}

template <class T>
T *YsLattice2d<T>::GetBlock(const YsVec2i &idx)
{
	return GetBlock(idx.x(),idx.y());
}
template <class T>
const T *YsLattice2d<T>::GetBlock(const YsVec2i &idx) const
{
	return GetBlock(idx.x(),idx.y());
}
template <class T>
T &YsLattice2d<T>::operator[](const YsVec2i &idx)
{
	return blk[idx.y()*(nBlkX+1)+idx.x()];
}
template <class T>
const T &YsLattice2d<T>::operator[](const YsVec2i &idx) const
{
	return blk[idx.y()*(nBlkX+1)+idx.x()];
}

template <class T>
const YsVec2 YsLattice2d <T>::GetBlockDimension(void) const
{
	return YsVec2(lxBlk,lyBlk);
}

template <class T>
void YsLattice2d <T>::GetBlockRange(YsVec2 &mn,YsVec2 &mx,YSSIZE_T bx,YSSIZE_T by) const
{
	// YsVec2 u,v;
	// u.Set(lxBlk,0.0);
	// v.Set(0.0,lyBlk);
	// mn=min+u*double(bx)+v*double(by);
	// mx=mn+u+v;

	mn=min;
	mn.AddX((double)bx*lxBlk);
	mn.AddY((double)by*lyBlk);

	mx=mn;
	mx.AddX(lxBlk);
	mx.AddY(lyBlk);
}

template <class T>
void YsLattice2d<T>::GetBlockRect(YsVec2 rect[4],YSSIZE_T bx,YSSIZE_T by) const
{
	auto dim=GetBlockDimension();
	GetGridPosition(rect[0],bx,by);
	rect[1].Set(rect[0].x()+dim.x(),rect[0].y());
	rect[2]=rect[0]+dim;
	rect[3].Set(rect[0].x()        ,rect[0].y()+dim.y());
}

template <class T>
void YsLattice2d<T>::GetBlockRect(YsVec2 rect[4],const YsVec2i &idx) const
{
	return GetBlockRect(rect,idx.x(),idx.y());
}

template <class T>
void YsLattice2d<T>::GetGridPosition(YsVec2 &pos,YSSIZE_T bx,YSSIZE_T by) const
{
	pos=min;
	pos.AddX((double)bx*lxBlk);
	pos.AddY((double)by*lyBlk);
}

template <class T>
const YsVec2 YsLattice2d<T>::GetGridPosition(YSSIZE_T bx,YSSIZE_T by) const
{
	auto pos=min;
	pos.AddX((double)bx*lxBlk);
	pos.AddY((double)by*lyBlk);
	return pos;
}

template <class T>
const YsVec2 YsLattice2d<T>::GetGridPosition(const YsVec2i &idx) const
{
	return GetGridPosition(idx.x(),idx.y());
}

template <class T>
YSRESULT YsLattice2d <T>::SetBlock(YSSIZE_T x,YSSIZE_T y,const T &blk)
{
	T *desBlk;
	desBlk=GetEditableBlock(x,y);
	if(desBlk!=NULL)
	{
		*desBlk=blk;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


template <class T>
int YsLattice2d <T>::GetNumBlockX(void) const
{
	return (int)nBlkX;
}

template <class T>
int YsLattice2d <T>::GetNumBlockY(void) const
{
	return (int)nBlkY;
}

template <class T>
void YsLattice2d <T>::GetLatticeRange(YsVec2 &n,YsVec2 &x) const
{
	n=min;
	x=max;
}

template <class T>
YSRESULT YsLattice2d <T>::Release(void)
{
	if(blk!=NULL)
	{
		delete [] blk;
		blk=NULL;
	}

	nBlkX=0;
	nBlkY=0;
	lxBlk=0.0;
	lyBlk=0.0;
	min.Set(0.0,0.0);
	max.Set(0.0,0.0);
	blk=NULL;

	return YSOK;
}

////////////////////////////////////////////////////////////

template <class T>
class YsLattice3d
{
public:
	YsLattice3d();
	YsLattice3d(const YsLattice3d <T> &from);
	YsLattice3d(YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz,const YsVec3 &min,const YsVec3 &max);
	~YsLattice3d();

	const YsLattice3d <T> &operator=(const YsLattice3d <T> &from);

	/*! Cleans up the object. */
	void CleanUp(void);

	/*! Create a lattice with nx*ny*nz blocks.  To store nodal information, the length of the
	    storage actually allocated will be (nx+1)*(ny+1)*(nz+1).
	*/
	YSRESULT Create(YSSIZE_T nCell,const YsVec3 &min,const YsVec3 &max);
	YSRESULT Create(YSSIZE_T nBlkX,YSSIZE_T nBlkY,YSSIZE_T nBlkZ,const YsVec3 &min,const YsVec3 &max);

	/*! Create a lattice that has the same bounding box and resolution as the given lattice.
	*/
	template <class T2>
	YSRESULT CreateCompatible(const YsLattice3d <T2> &incoming);

	/*! Returns the dimension of the whole lattice. */
	YsVec3 GetLatticeDimension(void) const;

	/*! Returns the dimension of a block. */
	const YsVec3 GetBlockDimension(void) const;

	void CalculateNxNyNzFromNCell(int &nx,int &ny,int &nz,const YsVec3 &min,const YsVec3 &max,YSSIZE_T nCell);

	/*! Returns number of blocks in YsVec3i. */
	YsVec3i GetNumBlock(void) const;

	/*! Returns number of blocks in X direction.
	*/
	YSSIZE_T Nx(void) const;

	/*! Returns number of blocks in Y direction.
	*/
	YSSIZE_T Ny(void) const;

	/*! Returns number of blocks in Z direction.
	*/
	YSSIZE_T Nz(void) const;

	/*! Returns the minimum (x,y,z) of the block at (bx,by,bz).
	    (bx,by,bz) can be out of range of the lattice, in which case the returned position will be outside of the lattice.
	*/
	YsVec3 GetBlockPosition(YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz) const;

	/*! Returns the minimum (x,y,z) of the block at (idx.x(),idx.y(),idx.z()).
	    (bx,by,bz) can be out of range of the lattice, in which case the returned position will be outside of the lattice.
	*/
	YsVec3 GetBlockPosition(const YsVec3i &idx) const;

	YSRESULT GetBlockIndex(int &x,int &y,int &z,const YsVec3 &pos) const;
	YSRESULT GetBlockIndex(YsVec3i &idx,const YsVec3 &pos) const;
	YSRESULT GetBlockIndexUnconstrained(int &x,int &y,int &z,const YsVec3 &pos) const;
	YSRESULT GetBlockIndexRange(int &x1,int &y1,int &z1,int &x2,int &y2,int &z2,YSSIZE_T np,const YsVec3 p[]) const;
	YSRESULT GetBlockIndexRange(int &x1,int &y1,int &z1,int &x2,int &y2,int &z2,const YsVec3 &p1,const YsVec3 &p2) const;
	YSRESULT GetBlockIndexAutoBound(int &x,int &y,int &z,const YsVec3 &pos) const;

	/*! Returns the index of the block that encloses the given position.
	    Equivalent to GetBlockIndexUnconstrained.
	*/
	YsVec3i GetBlockIndex(const YsVec3 &pos) const;

	/*! Returns the index of the block that encloses the given position.
	    If the position is outside of the lattice, it will clamp the index so that the index will yield a non-null pointer
	    when given to GetBlock.
	*/
	YsVec3i GetBlockIndexAutoBound(const YsVec3 &pos) const;

	/*! Returns the index of the block that encloses the given position.
	    If the position is oustside of the lattice, it will return an index outside of the index range.
	*/
	YsVec3i GetBlockIndexUnconstrained(const YsVec3 &pos) const;

	/*! Returns a linearized index to the block.
	    The return value is idx.z()*(Nx()+1)*(Ny()+1)+idx.y()*(Nx+1)+idx.z().
	    It uniquely identifies a block for this lattice.
	*/
	YSSIZE_T GetLinearIndex(const YsVec3i idx) const;

	/*! Returns a block index from linear index.

		blkX=(idx%(Nx+1));
		blkY=((idx%((Nx+1)*(Ny+1)))/(Nx+1));
		blkZ=(idx/((Nx+1)*(Ny+1)));
	*/
	YsVec3i LinearToBlock(YSSIZE_T idx) const;

	/*! Returns YSTRUE if (idx.x(),idx.y(),idx.z()) is in the range of node indexes or YSFALSE otherwise. 
	    (i.e., GetBlock(idx) does not return NULL.) 
	    For testing if idx is within block-index range, use IsInBlockRange instead. */
	YSBOOL IsInRange(const YsVec3i idx) const;

	/*! Returns YSTRUE if (idx.x(),idx.y(),idx.z()) is in the range of block indexes or YSFALSE otherwise.
	    If idx.x(), y(), z() is equal to the number of blocks in X, Y, Z direction, this function
	    returns YSFALSE although GetBlock(idx) may return a non-NULL pointer, since the number of blocks in 
	    each axial direction is shorter than the number of nodes by one. */
	YSBOOL IsInBlockRange(const YsVec3i idx) const;

	/*! Returns the block where pos is lying. */
	T *GetEditableBlock(const YsVec3 &pos);

	/*! Returns the block where pos is lying. */
	T *GetBlock(const YsVec3 &pos);

	/*! Returns the block where pos is lying. */
	const T *GetBlock(const YsVec3 &pos) const;

	T *GetEditableBlock(YSSIZE_T x,YSSIZE_T y,YSSIZE_T z);  // For backward compatibility.

	/*! Returns the block pointer to (idx.x(),idx.y(),idx.z()) */
	T *GetBlock(const YsVec3i idx);

	T *GetBlock(YSSIZE_T x,YSSIZE_T y,YSSIZE_T z);

	/*! Returns the block pointer to (idx.x(),idx.y(),idx.z()) */
	const T *GetBlock(const YsVec3i idx) const;

	const T *GetBlock(YSSIZE_T x,YSSIZE_T y,YSSIZE_T z) const;

	/*! Returns the block content.  The caller must be responsible for making sure idx is in range. */
	const T &operator[](const YsVec3i idx) const;

	/*! Returns the block content.  The caller must be responsible for making sure idx is in range. */
	T &operator[](const YsVec3i idx);

	/*! Returns minimum and maximum coordinates of the block (bx,by,bz). */
	void GetBlockRange(YsVec3 &mn,YsVec3 &mx,YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz) const;

	/*! Returns minimum and maximum coordinates of the block (bx,by,bz). */
	void GetBlockRange(YsVec3 minmax[2],YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz) const;

	/*! Returns minimum and maximum coordinates of the block (bx,by,bz). */
	void GetBlockRange(YsVec3 &mn,YsVec3 &mx,const YsVec3i idx) const;

	/*! Returns minimum and maximum coordinates of the block (bx,by,bz). */
	void GetBlockRange(YsVec3 minmax[2],const YsVec3i idx) const;

	/*! /deprecated
	    Left for backward compatibility.  Use GetBlockPosition instead. */
	void GetGridPosition(YsVec3 &pos,YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz) const;
	/*! /deprecated
	    Left for backward compatibility.  Use GetBlockPosition instead. */
	YsVec3 GetGridPosition(YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz) const;
	/*! /deprecated
	    Left for backward compatibility.  Use GetBlockPosition instead. */
	YsVec3 GetGridPosition(const YsVec3i &idx) const;

	YSRESULT SetBlock(YSSIZE_T x,YSSIZE_T y,YSSIZE_T z,const T &blk);

	YSRESULT SetBlock(const YsVec3i idx,const T &blk);

	/*! Returns the largest of the number of blocks in X-, Y-, and Z- directions. */
	int GetMaxNumBlock(void) const;

	int GetNumBlockX(void) const;
	int GetNumBlockY(void) const;
	int GetNumBlockZ(void) const;
	void GetLatticeRange(YsVec3 &min,YsVec3 &max) const;

	/*! Returns the minimum XYZ coordinate that the lattice covers. */
	const YsVec3 &GetMin(void) const;

	/*! Returns the maximum XYZ coordinate that the lattice covers. */
	const YsVec3 &GetMax(void) const;

	/*! This function is left for backward compatibility.  Use MakeIntersectingBlockList instead.
	    This function has a limitation that both p1 and p2 must be within the rectangular solid volume covered by the lattice.
	    MakeintersectingBlockList does not have such limitation.
	*/
	YSRESULT GetIntersectingCellList
	    (YsArray <YsFixedLengthArray <int,3> > &cellLst,const YsVec3 &p1,const YsVec3 &p2) const;

	/*! Returns indices of blocks that are intersecting with a line segment p1-p2.
	    First parameter can be a reference to YsArray <YsVec3i,N> or YsArray <unsigned int,N> where N is an arbitrary integer 0<=N.
	*/
	template <class indexReturnType>
	YSRESULT MakeIntersectingBlockList(indexReturnType &blkList,const YsVec3 &p1,const YsVec3 &p2) const;

	/*! Returns indices of blocks that are intersecting with a line segment p1-p2.
	    Both p1 and p2 must be inside the lattice volume.
	    First parameter can be a reference to YsArray <YsVec3i,N> or YsArray <unsigned int,N> where N is an arbitrary integer 0<=N.
	*/
	template <class indexReturnType>
	YSRESULT MakeIntersectingBlockListAfterClipping(indexReturnType &blkList,const YsVec3 &p1,const YsVec3 &p2) const;

	template <class indexReturnType>
	YSRESULT MakePotentiallyIntersectingBlockListPolygon(indexReturnType &blkList,YSSIZE_T np,const YsVec3 p[]) const;

	/*! Returns indices of blocks that are intersecting with a polygon p[0],p[1],...,p[np-1].
	    The polygon can extend outside the lattice.
	    First parameter can be a reference to YsArray <YsVec3i,N> or YsArray <unsigned int,N> where N is an arbitrary integer 0<=N.
	*/
	template <class indexReturnType>
	YSRESULT MakeIntersectingBlockListPolygon(indexReturnType &blkList,YSSIZE_T np,const YsVec3 p[]) const;

	/*! Returns indices of blocks that are intersecting witha polygon p[0]...p[np-1]. 
	    It uses MakeIntersectingBlockListPolygon to make a list of indices.
	*/
	YsArray <YsVec3i> GetIntersectingBlockListPolygon(YSSIZE_T np,const YsVec3 p[]) const;

	template <class indexReturnType>
	YSRESULT MakeIntersectingBlockListPlane(indexReturnType &blkList,YSSIZE_T bx1,YSSIZE_T by1,YSSIZE_T bz1,YSSIZE_T bx2,YSSIZE_T by2,YSSIZE_T bz2,const YsPlane &pln) const;
private:
	template <const int N>
	static void AddBlockIndex(YsArray <unsigned int,N> &blkList,YSSIZE_T x,YSSIZE_T y,YSSIZE_T z)
	{
		blkList.Add((unsigned int)x);
		blkList.Add((unsigned int)y);
		blkList.Add((unsigned int)z);
	}
	template <const int N>
	static void AddBlockIndex(YsArray <YsVec3i,N> &blkList,YSSIZE_T x,YSSIZE_T y,YSSIZE_T z)
	{
		blkList.Increment();
		blkList.Last().Set((int)x,(int)y,(int)z);
	}

public:
	/*! Returns a range of Grid indices.  It will iterate all nodes (grids).  Therefore,
	        0<=idx.x<=Nx()
	        0<=idx.y<=Ny()
	        0<=idx.z<=Nz()
	*/
	YsVec3iRange GetGridIndexRange(void) const
	{
		YsVec3i min(0,0,0);
		YsVec3i max(Nx(),Ny(),Nz());
		return YsVec3iRange(min,max);
	}

	/*! Returns a range of Block indices.  It will NOT iterate all nodes (grids).  Therefore,
	        0<=idx.x<Nx()
	        0<=idx.y<Ny()
	        0<=idx.z<Nz()
	*/
	YsVec3iRange GetBlockIndexRange(void) const
	{
		YsVec3i min(0,0,0);
		YsVec3i max(Nx()-1,Ny()-1,Nz()-1);
		return YsVec3iRange(min,max);
	}


public:
	/*! Returns the number of nodes. */
	YSSIZE_T NumNode(void) const
	{
		return (Nx()+1)*(Ny()+1)*(Nz()+1);
	}

	/*! Returns the number of blocks. */
	YSSIZE_T NumBlock(void) const
	{
		return Nx()*Ny()*Nz();
	}


protected:
	YSRESULT Release(void);


	YsVec3 min,max;
	YSSIZE_T nBlkX,nBlkY,nBlkZ;
	double lxBlk,lyBlk,lzBlk;
	T *blk;
};

template <class T>
YsLattice3d <T>::YsLattice3d()
{
	blk=nullptr;
	CleanUp();
}

template <class T>
YsLattice3d <T>::YsLattice3d(const YsLattice3d <T> &from)
{
	blk=nullptr;	// 2019/01/25  Was missing.  This bug was hiding for almost two decades.
	*this=from;
}

template <class T>
YsLattice3d <T>::YsLattice3d(YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz,const YsVec3 &mn,const YsVec3 &mx)
{
	YsLattice3d();
	Create(nx,ny,nz,mn,mx);
}

template <class T>
YsLattice3d <T>::~YsLattice3d()
{
	Release();
}

template <class T>
const YsLattice3d <T> &YsLattice3d <T>::operator=(const YsLattice3d <T> &from)
{
	int i,nBlk;
	T *buf;
	nBlk=(from.GetNumBlockX()+1)*(from.GetNumBlockY()+1)*(from.GetNumBlockZ()+1);
	buf=new T [nBlk];
	if(buf!=NULL)
	{
		Release();
		blk=buf;

		for(i=0; i<nBlk; i++)
		{
			blk[i]=from.blk[i];
		}

		min=from.min;
		max=from.max;
		nBlkX=from.nBlkX;
		nBlkY=from.nBlkY;
		nBlkZ=from.nBlkZ;
		lxBlk=from.lxBlk;
		lyBlk=from.lyBlk;
		lzBlk=from.lzBlk;
	}
	else
	{
		YsErrOut("YsLattice3d::operator=()\n");
		YsErrOut("  Low memory warning!\n");
	}

	return *this;
}

template <class T>
void YsLattice3d<T>::CleanUp(void)
{
	if(NULL!=blk)
	{
		delete [] blk;
		blk=NULL;
	}
	min.Set(0.0,0.0,0.0);
	max.Set(0.0,0.0,0.0);
	nBlkX=0;
	nBlkY=0;
	nBlkZ=0;
	lxBlk=0.0;
	lyBlk=0.0;
	lzBlk=0.0;
}

template <class T>
YSRESULT YsLattice3d <T>::Create(YSSIZE_T nCell,const YsVec3 &mn,const YsVec3 &mx)
{
	YsVec3 dg;
	double vol,cellVol,l;
	int nx,ny,nz;

	dg=mx-mn;
	vol=fabs(dg.x()*dg.y()*dg.z());
	cellVol=vol/(double)nCell;
	l=pow(cellVol,1.0/3.0);

	nx=YsGreater(1,(int)(fabs(dg.x())/l));
	ny=YsGreater(1,(int)(fabs(dg.y())/l));
	nz=YsGreater(1,(int)(fabs(dg.z())/l));

	return Create(nx,ny,nz,mn,mx);
}

template <class T>
template <class T2>
YSRESULT YsLattice3d<T>::CreateCompatible(const YsLattice3d <T2> &incoming)
{
	return Create(incoming.Nx(),incoming.Ny(),incoming.Nz(),incoming.GetMin(),incoming.GetMax());
}

template <class T>
void YsLattice3d <T>::CalculateNxNyNzFromNCell(int &nx,int &ny,int &nz,const YsVec3 &min,const YsVec3 &max,YSSIZE_T nCell)
{
	double lx,ly,lz,volCell,lCell;
	lx=fabs(max.x()-min.x());
	ly=fabs(max.y()-min.y());
	lz=fabs(max.z()-min.z());

	volCell=(lx*ly*lz)/(double)nCell;
	lCell=pow(volCell,1.0/3.0);

	nx=1+(int)(lx/lCell);
	ny=1+(int)(ly/lCell);
	nz=1+(int)(lz/lCell);
}

template <class T>
YSRESULT YsLattice3d <T>::Create(YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz,const YsVec3 &mn,const YsVec3 &mx)
{
	T *buf;
	buf=new T [(nx+1)*(ny+1)*(nz+1)];
	if(buf!=NULL)
	{
		Release();
		blk=buf;
		nBlkX=nx;
		nBlkY=ny;
		nBlkZ=nz;

		YsBoundingBoxMaker3 bbx;
		bbx.Begin(mn);
		bbx.Add(mx);

		YsVec3 trueMin,trueMax,dif;
		bbx.Get(trueMin,trueMax);
		min=trueMin;
		max=trueMax;

		dif=trueMax-trueMin;
		lxBlk=dif.x()/double(nx);
		lyBlk=dif.y()/double(ny);
		lzBlk=dif.z()/double(nz);

		blk=buf;

		return YSOK;
	}
	else
	{
		YsErrOut("YsLattice3d::Create()\n");
		YsErrOut("  Low memory warning!!\n");
		return YSERR;
	}
}

template <class T>
YsVec3 YsLattice3d<T>::GetLatticeDimension(void) const
{
	return max-min;
}

template <class T>
YsVec3 YsLattice3d<T>::GetBlockPosition(YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz) const
{
	return GetGridPosition(bx,by,bz);
}
template <class T>
YsVec3 YsLattice3d<T>::GetBlockPosition(const YsVec3i &idx) const
{
	return GetGridPosition(idx);
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndex(int &x,int &y,int &z,const YsVec3 &pos) const
{

	if(min.x()<=pos.x() && pos.x()<max.x() &&
	   min.y()<=pos.y() && pos.y()<max.y() &&
	   min.z()<=pos.z() && pos.z()<max.z())
	{
		YsVec3 rel;
		rel=pos-min;

		int bx=int(rel.x()/lxBlk);
		int by=int(rel.y()/lyBlk);
		int bz=int(rel.z()/lzBlk);

		if(0<=bx && bx<nBlkX &&
		   0<=by && by<nBlkY &&
		   0<=bz && bz<nBlkZ) // Redundant check
		{
			x=bx;
			y=by;
			z=bz;
			return YSOK;
		}
		else
		{
			// 2016/07/27 Can make an out-of-range error, but the position is already checked against the min-max.
			//            If it gets out of the index range, it must be on the min or max, and should
			//            be safe to round it.
			x=YsBound<int>(bx,0,(int)nBlkX-1);
			y=YsBound<int>(by,0,(int)nBlkY-1);
			z=YsBound<int>(bz,0,(int)nBlkZ-1);
			return YSOK;
		}
	}
	x=0;
	y=0;
	z=0;
	return YSERR;
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndex(YsVec3i &idx,const YsVec3 &pos) const
{
	int x,y,z;
	if(YSOK==GetBlockIndex(x,y,z,pos))
	{
		idx.Set(x,y,z);
		return YSOK;
	}
	return YSERR;
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndexUnconstrained(int &x,int &y,int &z,const YsVec3 &pos) const
{
	YsVec3 rel;
	rel=pos-min;
	x=int(rel.x()/lxBlk);
	y=int(rel.y()/lyBlk);
	z=int(rel.z()/lzBlk);
	return YSOK;
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndexRange
    (int &x1,int &y1,int &z1,int &x2,int &y2,int &z2,YSSIZE_T np,const YsVec3 p[]) const
{
	int i;
	YsVec3 p1,p2;
	YsBoundingBoxMaker3 bbx;
	bbx.Begin(p[0]);
	for(i=1; i<np; i++)
	{
		bbx.Add(p[i]);
	}
	bbx.Get(p1,p2);

	if(GetBlockIndexAutoBound(x1,y1,z1,p1)==YSOK &&
	   GetBlockIndexAutoBound(x2,y2,z2,p2)==YSOK)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndexRange
    (int &x1,int &y1,int &z1,int &x2,int &y2,int &z2,const YsVec3 &p1,const YsVec3 &p2) const
{
	YsVec3 q1,q2;
	YsBoundingBoxMaker3 bbx;
	bbx.Begin(p1);
	bbx.Add(p2);

	bbx.Get(q1,q2);
	if(GetBlockIndexAutoBound(x1,y1,z1,q1)==YSOK &&
	   GetBlockIndexAutoBound(x2,y2,z2,q2)==YSOK)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndexAutoBound(int &x,int &y,int &z,const YsVec3 &pos) const
{
	YsVec3 bnd;
	bnd.Set(YsBound(pos.x(),min.x()+YsTolerance,max.x()-YsTolerance),
	        YsBound(pos.y(),min.y()+YsTolerance,max.y()-YsTolerance),
	        YsBound(pos.z(),min.z()+YsTolerance,max.z()-YsTolerance));
	return GetBlockIndex(x,y,z,bnd);
}

template <class T>
YsVec3i YsLattice3d<T>::GetBlockIndexUnconstrained(const YsVec3 &pos) const
{
	int ix,iy,iz;
	GetBlockIndexUnconstrained(ix,iy,iz,pos);
	return YsVec3i(ix,iy,iz);
}

template <class T>
YsVec3i YsLattice3d<T>::LinearToBlock(YSSIZE_T idx) const
{
	YsVec3i blkIdx;
	blkIdx.SetZ(idx/((Nx()+1)*(Ny()+1)));
	blkIdx.SetY((idx%((Nx()+1)*(Ny()+1)))/(Nx()+1));
	blkIdx.SetX(idx%(Nx()+1));
	return blkIdx;
}

template <class T>
YsVec3i YsLattice3d<T>::GetBlockIndexAutoBound(const YsVec3 &pos) const
{
	int x,y,z;
	GetBlockIndexAutoBound(x,y,z,pos);
	return YsVec3i(x,y,z);
}

template <class T>
YsVec3i YsLattice3d<T>::GetBlockIndex(const YsVec3 &pos) const
{
	return GetBlockIndexUnconstrained(pos);
}

template <class T>
YSSIZE_T YsLattice3d<T>::GetLinearIndex(const YsVec3i idx) const
{
	return idx.z()*(nBlkX+1)*(nBlkY+1)+idx.y()*(nBlkX+1)+idx.x();
}

template <class T>
YSBOOL YsLattice3d<T>::IsInRange(const YsVec3i idx) const
{
	if(0<=idx.x() && idx.x()<=nBlkX &&
	   0<=idx.y() && idx.y()<=nBlkY &&
	   0<=idx.z() && idx.z()<=nBlkZ)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class T>
YSBOOL YsLattice3d<T>::IsInBlockRange(const YsVec3i idx) const
{
	if(0<=idx.x() && idx.x()<nBlkX &&
	   0<=idx.y() && idx.y()<nBlkY &&
	   0<=idx.z() && idx.z()<nBlkZ)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class T>
T *YsLattice3d <T>::GetEditableBlock(const YsVec3 &pos)
{
	int bx,by,bz;
	GetBlockIndexUnconstrained(bx,by,bz,pos);
	return GetEditableBlock(bx,by,bz);
}

template <class T>
T *YsLattice3d <T>::GetBlock(const YsVec3 &pos)
{
	int bx,by,bz;
	GetBlockIndexUnconstrained(bx,by,bz,pos);
	return GetEditableBlock(bx,by,bz);
}

template <class T>
const T *YsLattice3d <T>::GetBlock(const YsVec3 &pos) const
{
	int bx,by,bz;
	GetBlockIndexUnconstrained(bx,by,bz,pos);
	return GetBlock(bx,by,bz);
}

template <class T>
T *YsLattice3d <T>::GetEditableBlock(YSSIZE_T x,YSSIZE_T y,YSSIZE_T z)
{
	if(0<=x && x<=nBlkX &&
	   0<=y && y<=nBlkY &&
	   0<=z && z<=nBlkZ)
	{
		return &blk[z*(nBlkX+1)*(nBlkY+1)+y*(nBlkX+1)+x];
	}
	else
	{
		return NULL;
	}
}

template <class T>
T *YsLattice3d <T>::GetBlock(YSSIZE_T x,YSSIZE_T y,YSSIZE_T z)
{
	if(0<=x && x<=nBlkX &&
	   0<=y && y<=nBlkY &&
	   0<=z && z<=nBlkZ)
	{
		return &blk[z*(nBlkX+1)*(nBlkY+1)+y*(nBlkX+1)+x];
	}
	else
	{
		return NULL;
	}
}

template <class T>
const T *YsLattice3d <T>::GetBlock(YSSIZE_T x,YSSIZE_T y,YSSIZE_T z) const
{
	if(0<=x && x<=nBlkX &&
	   0<=y && y<=nBlkY &&
	   0<=z && z<=nBlkZ)
	{
		return &blk[z*(nBlkX+1)*(nBlkY+1)+y*(nBlkX+1)+x];
	}
	else
	{
		return NULL;
	}
}

template <class T>
T *YsLattice3d<T>::GetBlock(const YsVec3i idx)
{
	return GetBlock(idx.x(),idx.y(),idx.z());
}
template <class T>
const T *YsLattice3d<T>::GetBlock(const YsVec3i idx) const
{
	return GetBlock(idx.x(),idx.y(),idx.z());
}

template <class T>
const T &YsLattice3d<T>::operator[](const YsVec3i idx) const
{
	return *GetBlock(idx);
}

template <class T>
T &YsLattice3d<T>::operator[](const YsVec3i idx)
{
	return *GetBlock(idx);
}

template <class T>
void YsLattice3d <T>::GetBlockRange(YsVec3 &mn,YsVec3 &mx,YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz) const
{
	// YsVec3 u,v,w;
	// u.Set(lxBlk,0.0,0.0);
	// v.Set(0.0,lyBlk,0.0);
	// w.Set(0.0,0.0,lzBlk);
	// mn=min+u*double(bx)+v*double(by)+w*double(bz);
	// mx=mn+u+v+w;

	mn=min;
	mn.AddX((double)bx*lxBlk);
	mn.AddY((double)by*lyBlk);
	mn.AddZ((double)bz*lzBlk);

	mx=mn;
	mx.AddX(lxBlk);
	mx.AddY(lyBlk);
	mx.AddZ(lzBlk);
}

template <class T>
void YsLattice3d <T>::GetBlockRange(YsVec3 minmax[2],YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz) const
{
	GetBlockRange(minmax[0],minmax[1],bx,by,bz);
}

template <class T>
void YsLattice3d<T>::GetBlockRange(YsVec3 &mn,YsVec3 &mx,const YsVec3i idx) const
{
	GetBlockRange(mn,mx,idx.x(),idx.y(),idx.z());
}

template <class T>
void YsLattice3d<T>::GetBlockRange(YsVec3 minmax[2],const YsVec3i idx) const
{
	GetBlockRange(minmax[0],minmax[1],idx.x(),idx.y(),idx.z());
}

template <class T>
YsVec3i YsLattice3d<T>::GetNumBlock(void) const
{
	return YsVec3i((int)nBlkX,(int)nBlkY,(int)nBlkZ);
}

template <class T>
YSSIZE_T YsLattice3d<T>::Nx(void) const
{
	return nBlkX;
}
template <class T>
YSSIZE_T YsLattice3d<T>::Ny(void) const
{
	return nBlkY;
}
template <class T>
YSSIZE_T YsLattice3d<T>::Nz(void) const
{
	return nBlkZ;
}

template <class T>
const YsVec3 YsLattice3d <T>::GetBlockDimension(void) const
{
	return YsVec3(lxBlk,lyBlk,lzBlk);
}

template <class T>
void YsLattice3d <T>::GetGridPosition(YsVec3 &pos,YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz) const
{
	// YsVec3 u,v,w;
	// u.Set(lxBlk,0.0,0.0);
	// v.Set(0.0,lyBlk,0.0);
	// w.Set(0.0,0.0,lzBlk);
	// pos=min+u*double(bx)+v*double(by)+w*double(bz);

	pos=min;
	pos.AddX((double)bx*lxBlk);
	pos.AddY((double)by*lyBlk);
	pos.AddZ((double)bz*lzBlk);
}

template <class T>
YsVec3 YsLattice3d<T>::GetGridPosition(const YsVec3i &idx) const
{
	return GetGridPosition(idx.x(),idx.y(),idx.z());
}

template <class T>
YsVec3 YsLattice3d <T>::GetGridPosition(YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz) const
{
	auto pos=min;
	pos.AddX((double)bx*lxBlk);
	pos.AddY((double)by*lyBlk);
	pos.AddZ((double)bz*lzBlk);
	return pos;
}

template <class T>
YSRESULT YsLattice3d <T>::SetBlock(YSSIZE_T x,YSSIZE_T y,YSSIZE_T z,const T &blk)
{
	T *b;
	b=GetEditableBlock(x,y,z);
	if(b!=NULL)
	{
		*b=blk;
		return YSOK;
	}
	else
	{
		YsErrOut("YsLattice3d::SetBlock()\n");
		YsErrOut("  Out of range\n");
		return YSERR;
	}
}

template <class T>
YSRESULT YsLattice3d <T>::SetBlock(const YsVec3i idx,const T &blk)
{
	return SetBlock(idx.x(),idx.y(),idx.z(),blk);
}

template <class T>
int YsLattice3d<T>::GetMaxNumBlock(void) const
{
	return (int)YsGreatestOf(nBlkX,nBlkY,nBlkZ);
}

template <class T>
int YsLattice3d <T>::GetNumBlockX(void) const
{
	return (int)nBlkX;
}

template <class T>
int YsLattice3d <T>::GetNumBlockY(void) const
{
	return (int)nBlkY;
}

template <class T>
int YsLattice3d <T>::GetNumBlockZ(void) const
{
	return (int)nBlkZ;
}

template <class T>
void YsLattice3d <T> ::GetLatticeRange(YsVec3 &m1,YsVec3 &m2) const
{
	m1=min;
	m2=max;
}

template <class T>
const YsVec3 &YsLattice3d <T> ::GetMin(void) const
{
	return min;
}

template <class T>
const YsVec3 &YsLattice3d <T> ::GetMax(void) const
{
	return max;
}


template <class T>
YSRESULT YsLattice3d <T>::GetIntersectingCellList
	    (YsArray <YsFixedLengthArray <int,3> > &cellLst,const YsVec3 &p1,const YsVec3 &p2) const
{
	// Remark : p1,p2 must be already clipped by external function.
	int bx1,by1,bz1,bx2,by2,bz2,vx,vy,vz;
	int x,y,z,i,j,k;
	YsVec3 blk[2],clip[2];
	YsArray <YsFixedLengthArray <int,3>,32> toLookAt;

	YsFixedLengthArray <int,3> cellCoord;
	GetBlockIndexAutoBound(bx1,by1,bz1,p1);
	GetBlockIndexAutoBound(bx2,by2,bz2,p2);

	vx=bx2-bx1;
	vy=by2-by1;
	vz=bz2-bz1;
	if(vx!=0)
	{
		vx=(vx>0 ? 1 : -1);
	}
	if(vy!=0)
	{
		vy=(vy>0 ? 1 : -1);
	}
	if(vz!=0)
	{
		vz=(vz>0 ? 1 : -1);
	}

	cellLst.Set(0,NULL);

	x=bx1;
	y=by1;
	z=bz1;
	for(i=-1; i<=1; i++)
	{
		for(j=-1; j<=1; j++)
		{
			for(k=-1; k<=1; k++)
			{
				cellCoord.dat[0]=x+i;
				cellCoord.dat[1]=y+j;
				cellCoord.dat[2]=z+k;
				toLookAt.AppendItem(cellCoord);
			}
		}
	}
	while(toLookAt.GetN()>0)
	{
		int i;

		x=toLookAt[0].dat[0];
		y=toLookAt[0].dat[1];
		z=toLookAt[0].dat[2];

		if(0<=x && x<nBlkX && 0<=y && y<nBlkY && 0<=z && z<nBlkZ)
		{
			for(i=cellLst.GetNumItem()-1; i>=0; i--)
			{
				if(cellLst[i].dat[0]==x &&
				   cellLst[i].dat[1]==y &&
				   cellLst[i].dat[2]==z)
				{
					goto NEXTCELL;
				}
			}

			GetBlockRange(blk[0],blk[1],x,y,z);
			if(YsClipInfiniteLine3(clip[0],clip[1],p1,p2-p1,blk[0],blk[1])==YSOK)
			{
				cellLst.AppendItem(toLookAt.GetItem(0));

				cellCoord.dat[0]=x+vx;
				cellCoord.dat[1]=y;
				cellCoord.dat[2]=z;
				toLookAt.Append(cellCoord);
				cellCoord.dat[0]=x;
				cellCoord.dat[1]=y+vy;
				cellCoord.dat[2]=z;
				toLookAt.Append(cellCoord);
				cellCoord.dat[0]=x;
				cellCoord.dat[1]=y;
				cellCoord.dat[2]=z+vz;
				toLookAt.Append(cellCoord);
			}
		}
	NEXTCELL:
		toLookAt.Delete(0);
	}

	return YSOK;
}

template <class T>
template <class indexReturnType>
YSRESULT YsLattice3d <T>::MakeIntersectingBlockList(indexReturnType &blkList,const YsVec3 &p1,const YsVec3 &p2) const
{
	YsVec3 lat[2],c1,c2;
	GetLatticeRange(lat[0],lat[1]);

	blkList.Set(0,NULL);

	if(YsCheckInsideBoundingBox3(p1,lat[0],lat[1])==YSTRUE &&
	   YsCheckInsideBoundingBox3(p2,lat[0],lat[1])==YSTRUE)
	{
		return MakeIntersectingBlockListAfterClipping(blkList,p1,p2);
	}
	else
	{
		if(YsClipLineSeg3(c1,c2,p1,p2,lat[0],lat[1])==YSOK)
		{
			return MakeIntersectingBlockListAfterClipping(blkList,c1,c2);
		}
	}
	return YSOK;
}

// p1,p2 must be clipped before this function.
template <class T>
template <class indexReturnType>
YSRESULT YsLattice3d <T>::MakeIntersectingBlockListAfterClipping(indexReturnType &blkList,const YsVec3 &p1,const YsVec3 &p2) const
{
	int goAlong,toCalc[2];
	YsVec3 v,o;
	YSBOOL watch;

	watch=YSFALSE;

	blkList.CleanUp();

	// if(p1==YsVec3(26.702956,22.297454,-20.583457))
	// {
	// 	watch=YSTRUE;
	// 	printf("MakeIntersectingBlockListAfterClipping Watch!\n");
	// }

	v=p2-p1;
	if(fabs(v.x())>fabs(v.y()) && fabs(v.x())>fabs(v.z()))  // Proceed along X direction
	{
		goAlong=0;    // Go along X direction.
		toCalc[0]=1;  // Calculate Y range and
		toCalc[1]=2;  // Calculate Z range for each step.
	}
	else if(fabs(v.y())>fabs(v.z()))  // Proceed along Y direction
	{
		goAlong=1;    // Go along Y direction.
		toCalc[0]=0;  // Calculate X range and
		toCalc[1]=2;  // Calculate Z range for each step.
	}
	else // Proceed along Z direction
	{
		goAlong=2;    // Go along Z direction.
		toCalc[0]=0;  // Calculate X range and
		toCalc[1]=1;  // Calculate Y range for each step.
	}

	v[toCalc[0]]/=v[goAlong];
	v[toCalc[1]]/=v[goAlong];
	v[goAlong]=1.0;

	YsBoundingBoxMaker3 makeBbx;
	YsVec3 bbx[2];
	int bIdx[2][3];
	makeBbx.Begin(p1);
	makeBbx.Add(p2);
	makeBbx.Get(bbx[0],bbx[1]);
	GetBlockIndexAutoBound(bIdx[0][0],bIdx[0][1],bIdx[0][2],bbx[0]);
	GetBlockIndexAutoBound(bIdx[1][0],bIdx[1][1],bIdx[1][2],bbx[1]);

	YsVec3 blkStep;
	YSSIZE_T nBlk[3];
	nBlk[0]=nBlkX;
	nBlk[1]=nBlkY;
	nBlk[2]=nBlkZ;
	blkStep[goAlong]=(max[goAlong]-min[goAlong])/(double)nBlk[goAlong];
	blkStep[toCalc[0]]=0.0;
	blkStep[toCalc[1]]=0.0;

	// YsVec3 blkMargine;
	// blkMargine=max-min;
	// blkMargine[toCalc[0]]*=0.02/(double)nBlk[toCalc[0]];
	// blkMargine[toCalc[1]]*=0.02/(double)nBlk[toCalc[1]];
	// blkMargine[goAlong]=0.0;

	if(p1[goAlong]<p2[goAlong])
	{
		o=p1;
	}
	else
	{
		o=p2;
	}

	int p,q,r,n,k;
	int lastRange[2][3];
	n=0;
	lastRange[0][0]=-1;
	lastRange[0][1]=-1;
	lastRange[0][2]=-1;
	lastRange[1][0]=-1;
	lastRange[1][1]=-1;
	lastRange[1][2]=-1;
	for(k=bIdx[0][goAlong]; k<=bIdx[1][goAlong]; k++)
	{
		YsVec3 itsc[2];

		itsc[0]=o+(blkStep[goAlong]*(double)n)*v;
		itsc[1]=o+(blkStep[goAlong]*(double)(n+1))*v;

		makeBbx.Begin(itsc[0]);
		makeBbx.Add(itsc[1]);
		makeBbx.Get(bbx[0],bbx[1]);

		// bbx[0]-=blkMargine;
		// bbx[1]+=blkMargine;

		int idx[2][3];
		GetBlockIndexAutoBound(idx[0][0],idx[0][1],idx[0][2],bbx[0]);
		GetBlockIndexAutoBound(idx[1][0],idx[1][1],idx[1][2],bbx[1]);

		for(p=idx[0][0]; p<=idx[1][0]; p++)
		{
			for(q=idx[0][1]; q<=idx[1][1]; q++)
			{
				for(r=idx[0][2]; r<=idx[1][2]; r++)
				{
					if(p<lastRange[0][0] || lastRange[1][0]<p ||
					   q<lastRange[0][1] || lastRange[1][1]<q ||
					   r<lastRange[0][2] || lastRange[1][2]<r)
					{
						AddBlockIndex(blkList,p,q,r);
						if(watch==YSTRUE)
						{
							printf("%d %d %d\n",p,q,r);
						}
					}
				}
			}
		}
		lastRange[0][0]=idx[0][0];
		lastRange[0][1]=idx[0][1];
		lastRange[0][2]=idx[0][2];
		lastRange[1][0]=idx[1][0];
		lastRange[1][1]=idx[1][1];
		lastRange[1][2]=idx[1][2];

		// for(q=idx[0][toCalc[0]]; q<=idx[1][toCalc[0]]; q++)
		// {
		// 	for(r=idx[0][toCalc[1]]; r<=idx[1][toCalc[1]]; r++)
		// 	{
		// 		unsigned c[3];
		// 		c[goAlong]=p;
		// 		c[toCalc[0]]=q;
		// 		c[toCalc[1]]=r;
		// 		AddBlockIndex(blkList,c[0],c[1],c[2]);
 		// 		if(watch==YSTRUE)
 		// 		{
 		// 			printf("%d %d %d\n",c[0],c[1],c[2]);
 		// 		}
 		// 	}
 		// }

		n++;
	}
	return YSOK;
}

template <class T>
template <class indexReturnType>
YSRESULT YsLattice3d <T>::MakePotentiallyIntersectingBlockListPolygon(indexReturnType &blkList,YSSIZE_T np,const YsVec3 p[]) const
{
	YsPlane pln;
	YsBoundingBoxMaker3 makeBbx;
	YsVec3 bbx[2];
	int bx1,by1,bz1,bx2,by2,bz2;

	makeBbx.Make(np,p);
	makeBbx.Get(bbx[0],bbx[1]);
	bbx[0]-=YsXYZ()*YsTolerance;
	bbx[1]+=YsXYZ()*YsTolerance;
	if(GetBlockIndexRange(bx1,by1,bz1,bx2,by2,bz2,bbx[0],bbx[1])==YSOK)
	{
		double a,b,c,d;
		if(YsFindLeastSquareFittingPlane(a,b,c,d,np,p)==YSOK)
		{
			int i;
			YsVec3 cen,nom;

			cen=p[0];
			for(i=1; i<np; i++)
			{
				cen+=p[i];
			}
			cen/=double(np);
			nom.Set(a,b,c);
			pln.Set(cen,nom);
			return MakeIntersectingBlockListPlane(blkList,bx1,by1,bz1,bx2,by2,bz2,pln);
		}
		else
		{
			// Cannot find a least-square fitting plane means that the polygon may be too thin.
			// Still there is a possibility that an edge of the polygon is intersecting with some blocks.
			YsKeyStore visited;
			for(YSSIZE_T idx=0; idx<np; ++idx)
			{
				YsArray <YsVec3i> blkListLine;
				if(YSOK==MakeIntersectingBlockList(blkListLine,p[idx],p[(idx+1)%np]))
				{
					for(auto i : blkListLine)
					{
						auto code=(YSHASHKEY)GetLinearIndex(YsVec3i(i.x(),i.y(),i.z()));
						if(YSTRUE!=visited.IsIncluded(code))
						{
							visited.Add(code);
							AddBlockIndex(blkList,i.x(),i.y(),i.z());
						}
					}
				}
			}
		}
	}
	return YSERR;
}

template <class T>
template <class indexReturnType>
YSRESULT YsLattice3d <T>::MakeIntersectingBlockListPolygon(indexReturnType &blkList,YSSIZE_T np,const YsVec3 p[]) const
{
	YsArray <unsigned,64> potential;
	blkList.CleanUp();
	if(MakePotentiallyIntersectingBlockListPolygon(potential,np,p)==YSOK)
	{
		int i;
		YsCollisionOfPolygon coll;
		coll.SetPolygon1(np,p);
		for(i=0; i<potential.GetN(); i+=3)
		{
			YsVec3 range[2];
			GetBlockRange(range[0],range[1],potential[i],potential[i+1],potential[i+2]);
			if(coll.CheckIntersectionBetweenPolygon1AndBlock(range[0],range[1])==YSTRUE)
			{
				AddBlockIndex(blkList,potential[i],potential[i+1],potential[i+2]);
			}
		}

		return YSOK;
	}
	return YSERR;
}

template <class T>
YsArray <YsVec3i> YsLattice3d<T>::GetIntersectingBlockListPolygon(YSSIZE_T np,const YsVec3 p[]) const
{
	YsArray <YsVec3i> blkIdx;
	MakeIntersectingBlockListPolygon <YsArray <YsVec3i> > (blkIdx,np,p);
	return blkIdx;
}

template <class T>
template <class indexReturnType>
YSRESULT YsLattice3d <T>::MakeIntersectingBlockListPlane(indexReturnType &blkList,YSSIZE_T bx1,YSSIZE_T by1,YSSIZE_T bz1,YSSIZE_T bx2,YSSIZE_T by2,YSSIZE_T bz2,const YsPlane &pln) const
{
	int scan1,scan2;
	int ray,b0[3],b1[3];
	YsVec3 rayDir;
	YsArray <double,64> itscLattice;


	// const int tstIdx[3]={6,4,3};


	blkList.CleanUp();

	if(fabs(pln.GetNormal().x())>fabs(pln.GetNormal().y()) && fabs(pln.GetNormal().x())>fabs(pln.GetNormal().z()))
	{
		ray  =0;
		scan1=1;
		scan2=2;
		rayDir=YsXVec();
	}
	else if(fabs(pln.GetNormal().y())>fabs(pln.GetNormal().z()))
	{
		scan1=0;
		ray  =1;
		scan2=2;
		rayDir=YsYVec();
	}
	else
	{
		scan1=0;
		scan2=1;
		ray  =2;
		rayDir=YsZVec();
	}

	b0[0]=(int)YsSmaller(bx1,bx2);
	b0[1]=(int)YsSmaller(by1,by2);
	b0[2]=(int)YsSmaller(bz1,bz2);

	b1[0]=(int)YsGreater(bx1,bx2);
	b1[1]=(int)YsGreater(by1,by2);
	b1[2]=(int)YsGreater(bz1,bz2);

	int p,q,r,nNode1,nNode2;
	nNode1=(b1[scan1]-b0[scan1]+2);
	nNode2=(b1[scan2]-b0[scan2]+2);
	itscLattice.Set(nNode1*nNode2,NULL);
	for(p=b0[scan1]; p<=b1[scan1]+1; p++)
	{
		for(q=b0[scan2]; q<=b1[scan2]+1; q++)
		{
			int idx[3];
			YsVec3 range[2],itsc;
			idx[scan1]=p;
			idx[scan2]=q;
			idx[ray]=0;

			GetBlockRange(range[0],range[1],idx[0],idx[1],idx[2]);
			if(pln.GetIntersection(itsc,range[0],rayDir)==YSOK)
			{
				itscLattice[(p-b0[scan1])+(q-b0[scan2])*(nNode1)]=itsc[ray];
			}
		}
	}

	double lUnit,lMin,lMax,lBlk[3];
	lBlk[0]=lxBlk;
	lBlk[1]=lyBlk;
	lBlk[2]=lzBlk;
	lUnit=lBlk[ray];
	lMin=this->min[ray];
	lMax=this->max[ray];
	for(p=b0[scan1]; p<=b1[scan1]; p++)
	{
		for(q=b0[scan2]; q<=b1[scan2]; q++)
		{
			int r0,r1,idx[3];
			double rangeMin,rangeMax;
			rangeMin=itscLattice[(p-b0[scan1])+(q-b0[scan2])*(nNode1)];
			rangeMax=itscLattice[(p-b0[scan1])+(q-b0[scan2])*(nNode1)];

			rangeMin=YsSmaller(rangeMin,itscLattice[(p-b0[scan1]+1)+(q-b0[scan2]  )*(nNode1)]);
			rangeMax=YsGreater(rangeMax,itscLattice[(p-b0[scan1]+1)+(q-b0[scan2]  )*(nNode1)]);

			rangeMin=YsSmaller(rangeMin,itscLattice[(p-b0[scan1]+1)+(q-b0[scan2]+1)*(nNode1)]);
			rangeMax=YsGreater(rangeMax,itscLattice[(p-b0[scan1]+1)+(q-b0[scan2]+1)*(nNode1)]);

			rangeMin=YsSmaller(rangeMin,itscLattice[(p-b0[scan1]  )+(q-b0[scan2]+1)*(nNode1)]);
			rangeMax=YsGreater(rangeMax,itscLattice[(p-b0[scan1]  )+(q-b0[scan2]+1)*(nNode1)]);

			r0=(int)((rangeMin-YsTolerance-lMin)/lUnit);
			r1=(int)((rangeMax+YsTolerance-lMin)/lUnit);


			// if(p==tstIdx[scan1] && q==tstIdx[scan2])
			// {
			// 	printf("%d,%d\n",p,q);
			// 	printf(" %d->%d  %lf->%lf\n",r0,r1,rangeMin,rangeMax);
			// 	printf(" Tol %lf\n",YsTolerance);
			// }


			for(r=YsGreater(r0,b0[ray]); r<=YsSmaller(r1,b1[ray]); r++)
			{
				idx[scan1]=p;
				idx[scan2]=q;
				idx[ray]=r;
				AddBlockIndex(blkList,idx[0],idx[1],idx[2]);
			}
		}
	}

	return YSOK;
}

template <class T>
YSRESULT YsLattice3d <T>::Release(void)
{
	CleanUp();
	return YSOK;
}


////////////////////////////////////////////////////////////

template <typename ATTRIB>
class YsPointFinderLattice3dElement
{
public:
	YsVec3 pos;
	ATTRIB attrib;
};

template <typename ATTRIB>
class YsPointFinderLattice3d : public YsLattice3d <YsArray <YsPointFinderLattice3dElement <ATTRIB> > >
{
public:
	YSRESULT Add(const YsVec3 &pos,const ATTRIB &attrib);

	// If "from" is outside of the lattice, the coordinate will be rounded to the lattice dimension.
	const YsPointFinderLattice3dElement <ATTRIB> *FindNearest(const YsVec3 &from) const;

	YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> FindNNearest(const YsVec3 &from,const int N) const;

protected:
	const YsPointFinderLattice3dElement <ATTRIB> *FindNearestWithinBlock(const YsArray <YsPointFinderLattice3dElement <ATTRIB> > &blk,const YsVec3 &from) const;

	void FindNNearestWithinBlock(
		YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> &blkCandidatePtr,
		YsArray <double> &blkCandidateDist,
		const YsArray <YsPointFinderLattice3dElement <ATTRIB> > &blk,
		const YsVec3 &from,
		const int N,
		const double distLimit) const;

	void MergeCandidate(
		YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> &candidatePtr,
		YsArray <double> &candidateDist,
		const YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> &blkCandidatePtr,
		const YsArray <double> &blkCandidateDist,
		const int N) const;
};


template <typename ATTRIB>
YSRESULT YsPointFinderLattice3d <ATTRIB>::Add(const YsVec3 &pos,const ATTRIB &attrib)
{
	YsVec3i idx;
	if(YSOK==this->GetBlockIndex(idx,pos))
	{
		auto &blk=(*this)[idx];
		blk.Increment();
		blk.Last().pos=pos;
		blk.Last().attrib=attrib;
		return YSOK;
	}
	return YSERR;
}

template <typename ATTRIB>
const YsPointFinderLattice3dElement <ATTRIB> *YsPointFinderLattice3d <ATTRIB>::FindNearest(const YsVec3 &from) const
{
	auto nNearest=FindNNearest(from,1);
	if(1==nNearest.GetN())
	{
		return nNearest[0];
	}
	return NULL;
}

template <typename ATTRIB>
YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> YsPointFinderLattice3d<ATTRIB>::FindNNearest(const YsVec3 &from,const int N) const
{
	auto d=this->GetBlockDimension();
	// double rLimit=YsInfinity;

	auto blk0=this->GetBlockIndexAutoBound(from);

	int rxLimit=this->GetNumBlockX();
	int ryLimit=this->GetNumBlockY();
	int rzLimit=this->GetNumBlockZ();


	// For large N, it should use a smarter sorting algorithm like AVL tree,
	// but for small N, plain sorting is good enough.
	YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> candidatePtr;
	YsArray <double> candidateDist;
	FindNNearestWithinBlock(candidatePtr,candidateDist,(*this)[blk0],from,N,YsInfinity);

	double maxDist=YsInfinity;
	if(0<candidatePtr.GetN() && N<=candidatePtr.GetN()) // 2017/02/17 N<=candidatePtr.GetN() added.  Range must be updated only after N nearest candidates are found.
	{
		maxDist=candidateDist.Last();
		rxLimit=YsSmaller<int>(rxLimit,1+maxDist/d.x());
		ryLimit=YsSmaller<int>(ryLimit,1+maxDist/d.y());
		rzLimit=YsSmaller<int>(rzLimit,1+maxDist/d.z());
	}


	int rx=1,ry=1,rz=1;
	while(rx<=rxLimit || ry<=ryLimit || rz<=rzLimit)
	{
		// minZ & maxZ
		for(int z=blk0.z()-rz; z<=blk0.z()+rz; z+=rz*2)
		{
			if(0<=z && z<this->GetNumBlockZ())
			{
				const int x0=YsGreater(0,blk0.x()-rx);
				const int y0=YsGreater(0,blk0.y()-ry);
				const int x1=YsSmaller(this->GetNumBlockX()-1,blk0.x()+rx);
				const int y1=YsSmaller(this->GetNumBlockY()-1,blk0.y()+ry);

				for(int x=x0; x<=x1; ++x)
				{
					for(int y=y0; y<=y1; ++y)
					{
						auto *blk=this->GetBlock(x,y,z);

						YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> blkCandidatePtr;
						YsArray <double> blkCandidateDist;
						FindNNearestWithinBlock(blkCandidatePtr,blkCandidateDist,*blk,from,N,maxDist);
						if(0<blkCandidatePtr.GetN())
						{
							MergeCandidate(candidatePtr,candidateDist,blkCandidatePtr,blkCandidateDist,N);
							if(candidateDist.Last()<maxDist && N<=candidatePtr.GetN()) // 2017/02/17 N<=candidatePtr.GetN() added.  Range must be updated only after N nearest candidates are found.
							{
								maxDist=candidateDist.Last();
								rxLimit=YsSmaller<int>(rxLimit,1+maxDist/d.x());
								ryLimit=YsSmaller<int>(ryLimit,1+maxDist/d.y());
								rzLimit=YsSmaller<int>(rzLimit,1+maxDist/d.z());
							}
						}
					}
				}
			}
		}

		// minY & maxY
		for(int y=blk0.y()-ry; y<=blk0.y()+ry; y+=ry*2)
		{
			if(0<=y && y<this->GetNumBlockY())
			{
				const int x0=YsGreater(0,blk0.x()-rx);
				const int z0=YsGreater(0,blk0.z()-rz+1);
				const int x1=YsSmaller(this->GetNumBlockX()-1,blk0.x()+rx);
				const int z1=YsSmaller(this->GetNumBlockZ()-1,blk0.z()+rz-1);

				for(int x=x0; x<=x1; ++x)
				{
					for(int z=z0; z<=z1; ++z)
					{
						auto *blk=this->GetBlock(x,y,z);
						YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> blkCandidatePtr;
						YsArray <double> blkCandidateDist;
						FindNNearestWithinBlock(blkCandidatePtr,blkCandidateDist,*blk,from,N,maxDist);
						if(0<blkCandidatePtr.GetN())
						{
							MergeCandidate(candidatePtr,candidateDist,blkCandidatePtr,blkCandidateDist,N);
							if(candidateDist.Last()<maxDist && N<=candidatePtr.GetN()) // 2017/02/17 N<=candidatePtr.GetN() added.  Range must be updated only after N nearest candidates are found.
							{
								maxDist=candidateDist.Last();
								rxLimit=YsSmaller<int>(rxLimit,1+maxDist/d.x());
								ryLimit=YsSmaller<int>(ryLimit,1+maxDist/d.y());
								rzLimit=YsSmaller<int>(rzLimit,1+maxDist/d.z());
							}
						}
					}
				}
			}
		}

		// minX & maxX
		for(int x=blk0.x()-rx; x<=blk0.x()+rx; x+=rx*2)
		{
			if(0<=x && x<this->GetNumBlockX())
			{
				const int y0=YsGreater<int>(0,blk0.y()-ry+1);
				const int z0=YsGreater<int>(0,blk0.z()-rz+1);
				const int y1=YsSmaller<int>(this->GetNumBlockY()-1,blk0.y()+ry-1);
				const int z1=YsSmaller<int>(this->GetNumBlockZ()-1,blk0.z()+rz-1);

				for(int y=y0; y<=y1; ++y)
				{
					for(int z=z0; z<=z1; ++z)
					{
						auto *blk=this->GetBlock(x,y,z);
						YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> blkCandidatePtr;
						YsArray <double> blkCandidateDist;
						FindNNearestWithinBlock(blkCandidatePtr,blkCandidateDist,*blk,from,N,maxDist);
						if(0<blkCandidatePtr.GetN())
						{
							MergeCandidate(candidatePtr,candidateDist,blkCandidatePtr,blkCandidateDist,N);
							if(candidateDist.Last()<maxDist && N<=candidatePtr.GetN()) // 2017/02/17 N<=candidatePtr.GetN() added.  Range must be updated only after N nearest candidates are found.
							{
								maxDist=candidateDist.Last();
								rxLimit=YsSmaller<int>(rxLimit,1+maxDist/d.x());
								ryLimit=YsSmaller<int>(ryLimit,1+maxDist/d.y());
								rzLimit=YsSmaller<int>(rzLimit,1+maxDist/d.z());
							}
						}
					}
				}
			}
		}


		rx++;
		ry++;
		rz++;
	}

	return candidatePtr;
}

template <typename ATTRIB>
void YsPointFinderLattice3d<ATTRIB>::MergeCandidate(
	YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> &candidatePtr,
	YsArray <double> &candidateDist,
	const YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> &blkCandidatePtr,
	const YsArray <double> &blkCandidateDist,
	const int N) const
{
	YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> newCandidatePtr;
	YsArray <double> newCandidateDist;

	int i=0,j=0;
	while((i<candidatePtr.GetN() || j<blkCandidatePtr.GetN()) && newCandidatePtr.GetN()<N)
	{
		if(candidatePtr.GetN()<=i)
		{
			newCandidatePtr.Append(blkCandidatePtr[j]);
			newCandidateDist.Append(blkCandidateDist[j]);
			++j;
		}
		else if(blkCandidatePtr.GetN()<=j)
		{
			newCandidatePtr.Append(candidatePtr[i]);
			newCandidateDist.Append(candidateDist[i]);
			++i;
		}
		else
		{
			if(candidateDist[i]<blkCandidateDist[j])
			{
				newCandidatePtr.Append(candidatePtr[i]);
				newCandidateDist.Append(candidateDist[i]);
				++i;
			}
			else
			{
				newCandidatePtr.Append(blkCandidatePtr[j]);
				newCandidateDist.Append(blkCandidateDist[j]);
				++j;
			}
		}
	}
	candidatePtr.MoveFrom(newCandidatePtr);
	candidateDist.MoveFrom(newCandidateDist);
}

template <typename ATTRIB>
const YsPointFinderLattice3dElement <ATTRIB> *YsPointFinderLattice3d <ATTRIB>::FindNearestWithinBlock(const YsArray <YsPointFinderLattice3dElement <ATTRIB> > &blk,const YsVec3 &from) const
{
	const YsPointFinderLattice3dElement <ATTRIB> *closest=NULL;
	double minDist=YsInfinity;
	for(auto &pnt : blk)
	{
		const double dist=(from-pnt.pos).GetSquareLength();
		if(NULL==closest || dist<minDist)
		{
			closest=&pnt;
			minDist=dist;
		}
	}
	return closest;
}

template <typename ATTRIB>
void YsPointFinderLattice3d<ATTRIB>::FindNNearestWithinBlock(
	YsArray <const YsPointFinderLattice3dElement <ATTRIB> *> &blkCandidatePtr,
	YsArray <double> &blkCandidateDist,
	const YsArray <YsPointFinderLattice3dElement <ATTRIB> > &blk,
	const YsVec3 &from,
	const int N,
	const double distLimit) const
{
	blkCandidatePtr.CleanUp();
	blkCandidateDist.CleanUp();

	for(auto &pnt : blk)
	{
		const double dist=(from-pnt.pos).GetSquareLength();
		if(dist<distLimit && (blkCandidateDist.GetN()<N || dist<blkCandidateDist.Last()))
		{
			for(int i=blkCandidateDist.GetN(); 0<=i; --i)
			{
				if(0==i || blkCandidateDist[i-1]<=dist)
				{
					blkCandidatePtr.Insert(i,&pnt);
					blkCandidateDist.Insert(i,dist);
					if(N<blkCandidatePtr.GetN())
					{
						blkCandidatePtr.Resize(N);
						blkCandidateDist.Resize(N);
					}
					break;
				}
			}
		}
	}
}

/* } */
#endif
