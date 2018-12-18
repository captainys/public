/* ////////////////////////////////////////////////////////////

File Name: ysshelllattice.h
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

#ifndef YSSHELLLATTICE_IS_INCLUDED
#define YSSHELLLATTICE_IS_INCLUDED
/* { */





// Body of YsShellLattice is written in ysshelllattice.cpp

class YsShellLatticeEdgePiece
{
public:
	YsShellVertexHandle edVtHd[2];
	unsigned int ceKey;
};

class YsShellLatticeElem
{
public:
	YSSIDE side;
	YsArray <YsShellVertexHandle> vtxList;
	YsArray <YsShellPolygonHandle> plgList;
	YsArray <YsShellLatticeEdgePiece> edgList;

	void *ompLock;  // <- omp_lock_t *


	YsShellLatticeElem();
	~YsShellLatticeElem();
	void Lock(void);
	void Unlock(void);
};

class YsShellLattice : public YsLattice3d <YsShellLatticeElem>
{
public:
	YSBOOL isSubLattice;  // If true, CheckInsideSolid will not call shl->CheckInsideSolid and return YSUNKNOWNSIDE when undetermined.
	mutable YSBOOL checkInsideSolidLog;
	mutable YsArray <YsVec3> foundIntersection;
	mutable YsArray <YsShellPolygonHandle,16> foundPolygon;
	mutable YsVec3 ray1,ray2;

private:
	YSBOOL latticeMayBeSmallerThanShl;

protected:
	const YsShell *shl;
	YSBOOL enablePlgToCellTable;
	YSBOOL enableInsideOutsideInfo;
	int plgToCellHashSize;

	YSBOOL xyPlane,xzPlane,yzPlane;

	YsShellLattice *subLattice;
	int subLatBx0,subLatBy0,subLatBz0;
	int subLatBx1,subLatBy1,subLatBz1;

	//  plgToCellTable is used in AddPolygon, DeletePolygon, and MakePossibllyCollidingPolygonList.
	//  Disabling plgToCellTable will make DeletePolygon and MakePossibllyCollidingPolygonList unavailable.

	YsArray <YsFixedLengthArray <int,3>,16> indeterminantCellList;
	void *plgToCellTable_ompLock; // <- omp_lock_t *
	YSSIZE_T n_elem_ompLock;
	void *elem_ompLock; // <- omp_lock_t *
	YsFixedLengthToMultiHashTable <YsFixedLengthArray <int,4>,1,4> plgToCellTable;

public:
	YsShellLattice(int hashSize=256);
	~YsShellLattice();

protected:
	void InitLock(void);
	void DestroyLock(void);
	void LockPlgToCellTable(void);
	void UnlockPlgToCellTable(void);

	void AssignElementLock(void);
	void DestroyElementLock(void);


public:
	void EnableAutoResizingHashTable(int minSize,int maxSize);
	void EnableAutoResizingHashTable(void);
	void DisablePolygonToCellHashTable(void);
	void DisableInsideOutsideInfo(void);

	void Print(void) const;

	/*! Cleans up the lattice. */
	void CleanUp(void);

	/*! Whem the lattice may cover only part of a YsShell, call this function with YSTRUE, or SetDomain will fail.
	   */
	void SetIsSubLattice(YSBOOL);

	/*! This function creates lattice, but does not register vertices and polygons.  Useful when only edge-proximity is needed. */
	YSRESULT PrepareLattice(const class YsShell &shl,YSSIZE_T nCell);


	/*! This function creates lattice, but does not register vertices and polygons.  Useful when only edge-proximity is needed. */
	YSRESULT PrepareLattice(const class YsShell &shl,const YsVec3 &bbxMin,const YsVec3 &bbxMax,YSSIZE_T nCell);

	/*! This function creates lattice, but does not register vertices and polygons.  Useful when only edge-proximity is needed. */
	YSRESULT PrepareLattice(const class YsShell &shl,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz);

	/*! This function creates lattice, but does not register vertices and polygons.  Useful when only edge-proximity is needed. */
	YSRESULT PrepareLattice(const class YsShell &shl,const YsVec3 &bbxMin,const YsVec3 &bbxMax,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz);

	YSRESULT SetDomain(const class YsShell &shl,YSSIZE_T nCell);
	YSRESULT SetDomain(const class YsShell &shl,const YsVec3 &bbxMin,const YsVec3 &bbxMax,YSSIZE_T nCell);
	YSRESULT SetDomain(const class YsShell &shl,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz);
	YSRESULT SetDomain(const class YsShell &shl,const YsVec3 &bbxMin,const YsVec3 &bbxMax,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz);

private:
	// 2015/08/11 If a simple thin rectangular-solid is given, all cells may be marked as boundary.
	//            SophisticatedCheckInsideSolidWhenTheCellIsMarkedAsBoundary will fail.
	//            Need to increase the lattice size so that some cells are at least marked as outside.
	YSRESULT SetDomainWithIncreasedLatticeSize(const class YsShell &shl);
	YSRESULT RegisterShell(const class YsShell &shl);
	YSSIZE_T GetNumCellMarkedAsInsideOrOutside(void) const;

public:
	YSRESULT SetDomain2DMode(const class YsShell &shl,YSSIZE_T nCell);
	YSRESULT SetDomainXYMode(const class YsShell &shl,YSSIZE_T nCell);
	YSRESULT SetDomainXZMode(const class YsShell &shl,YSSIZE_T nCell);
	YSRESULT SetDomainYZMode(const class YsShell &shl,YSSIZE_T nCell);

	YSRESULT SetDomain(const class YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[],YSSIZE_T nCell);
	YSRESULT SetDomain(const class YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[],const YsVec3 &bbxMin,const YsVec3 &bbxMax,YSSIZE_T nCell);
	YSRESULT SetDomain(const class YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[],YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz);
	YSRESULT SetDomain(const class YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[],const YsVec3 &bbxMin,const YsVec3 &bbxMax,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz);
protected:
	YSRESULT SetDomain_Polygon(const class YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[]);

public:
	// Steps to build a custom shell lattice:
	//   (1) One of BeginSetDomains.
	//   (2) Call AddVertex and AddPolygon functions as needed.
	//   (3) FillUndeterminedBlockRange if inside/outside information is needed.
	YSRESULT BeginSetDomain(const class YsShell &shl,YSSIZE_T nCell,const double margin);
	YSRESULT BeginSetDomain(const class YsShell &shl,const YsVec3 &bbxMin,const YsVec3 &bbxMax,YSSIZE_T nx,YSSIZE_T ny,YSSIZE_T nz);

	YSRESULT FillUndeterminedBlockRange(const class YsShell &shl,int bx0,int by0,int bz0,int bx1,int by1,int bz1,const YsShellPolygonStore *limitToThesePlg=NULL,const YsShellPolygonStore *excludePlg=NULL);
	YSRESULT RepairLatticeAfterModification(YsShell &shl,YSSIZE_T np,const YsVec3 p[]);
	YSRESULT AddVertex(YsShellVertexHandle vtHd);
	YSRESULT DeleteVertex(YsShellVertexHandle vtHd);
	YSRESULT AddPolygon(YsShellPolygonHandle plHd);
	/*! The polygon will be offsetted by thickness/2.0 to both sides before being registered in the lattice. */
	YSRESULT AddPolygonWithThickness(YsShellPolygonHandle plHd,const double thickness);
private:
	// TryAddPolygonToBlock returns YSTRUE if the polygon really intersects with bx,by,bz
	YSBOOL TryAddPolygonToBlock(YSSIZE_T bx,YSSIZE_T by,YSSIZE_T bz,YsCollisionOfPolygon &coll,YsShellPolygonHandle plHd,YSBOOL watch);
public:
	YSRESULT DeletePolygon(YsShellPolygonHandle plHd);

	YSRESULT MakeSortedCellPolygonBalance(YsArray <int> &balance) const; // x y z N, x y z N, ....
	YSBOOL CheckSubLatticeNecessity(
	    int &bx1,int &by1,int &bz1,int &bx2,int &by2,int &bz2,YsVec3 bbx[2],int &percentPlg,int &percentVol);
	YSBOOL CheckSubLatticeNecessity_SimpleCase(
	    int &bx1,int &by1,int &bz1,int &bx2,int &by2,int &bz2,YsVec3 bbx[2],int &percentPlg,int &percentVol);
	YSBOOL MakeSubLattice(int nCell);
	const YsShellLattice *GetSubLattice(void) const;

	YSRESULT AddEdgePiece(const YsShellVertexHandle edVtHd[2],YSHASHKEY ceKey);
	YSRESULT AddEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,YSHASHKEY ceKey);
	YSRESULT DeleteEdgePiece(const YsShellVertexHandle edVtHd[2]);

	YSRESULT FixIndeterminantCell(void);

	YSSIDE CheckInsideSolid(const YsVec3 &pnt,YSBOOL showWhichFunctionDetermined=YSFALSE) const;

	/*! This function return YSOK even when no intersection is found.  
	    The calling function must check iPlHd!=NULL to test if it found an intersection.
	*/
	YSRESULT FindFirstIntersection
	    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
	     const YsVec3 &org,const YsVec3 &dir,YsShellPolygonHandle plHdExclude=NULL) const;
	YSRESULT FindFirstIntersection
	    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
	     const YsVec3 &org,const YsVec3 &dir,YSSIZE_T nPlHdExclude,const YsShellPolygonHandle plHdExclude[]) const;

	YSRESULT FindFirstIntersection
	    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
	     const YsVec3 &org,const YsVec3 &dir,const YsConstArrayMask <YsShell::PolygonHandle> &plHdExclude ) const;

	YSRESULT FindFirstIntersectionHighPrecision
	    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
	     const YsVec3 &org,const YsVec3 &dir,YsShellPolygonHandle plHdExclude=NULL) const;
	YSRESULT FindFirstIntersectionHighPrecision
	    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
	     const YsVec3 &org,const YsVec3 &dir,YSSIZE_T nPlHdExclude,const YsShellPolygonHandle plHdExclude[]) const;

	/*! This function converts an intersection Itsc to YsShellElemType. 
	    The return value can be YsshellElemType of YSSHELL_VERTEX, YSSHELL_EDGE, of YSSHELL_POLYGON 
	    depending on the location of Itsc relative to the polygon.
	*/
	YsShellElemType ConvertIntersection(YsShell::PolygonHandle PlHd,const YsVec3 &Itsc) const;

private:
	YSRESULT FindFirstIntersectionWithPrecisionFlag
	    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
	     const YsVec3 &org,const YsVec3 &dir,YSBOOL highPrecision,YSSIZE_T nPlHdExclude,const YsShellPolygonHandle plHdExclude[]) const;

public:
	YSRESULT GetCellListFromPolygon
	    (YsArray <YsFixedLengthArray <int,4>,4> &cell,YsShellPolygonHandle plHd) const;


	YSRESULT SelfDiagnostic(YsArray <YsVec3> &disagree);

	/*! This function finds polygons that may extend into the bounding box of p[0],p[1],...,p[np-1].
	    It is NOT for finding a polygons that may be intersecting with a polygon defined by (np,p).
	    It therefore gives conservative list of polygons.
	    
	    It may be useful when (np,p) is simply a point cloud, or when (np,p) is a polygon that is 
	    not lying on a plane, or when (np,p) is too thin to calculate a normal vector.

	    On the other hand, this function may return more polygons than those actually needs to be
	    checked for intersection, especially (np,p) are not axis-aligned.

	    When a shorter list of polygons that can be intersecting with a polygon is needed, use
	    GetPolygonIntersectionCandidate instead.
	*/
	YSRESULT MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plgList,YSSIZE_T np,const YsVec3 p[]) const;

	YSRESULT MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plgList,const YsVec3 &vt1,const YsVec3 &vt2) const;
	YSRESULT MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plgList,YsShellPolygonHandle plHd) const;
	YSRESULT MakePossibllyIncludedVertexList
	    (YsArray <YsShellVertexHandle,16> &vtxList,YSSIZE_T np,const YsVec3 p[]) const;
	YSRESULT MakeEdgePieceListByBoundingBox(
	     YsArray <YsShellVertexHandle,16> &edVtHd,YsArray <unsigned int,16> &ceKey,
	     const YsVec3 &vt1,const YsVec3 &vt2) const;


	/*! This function finds polygons that may be intersecting with a polygon defined by (np,p).

	    This function assumes that all points are lying on a plane.  The result may be unreliable otherwise.
	*/
	YsArray <YsShell::PolygonHandle> GetPolygonIntersectionCandidate(YSSIZE_T np,const YsVec3 p[]) const;


	/*! This function returns edge pieces that are intersecting with blocks that are blockDist away from the block in either X-,Y-, or Z-direction that encloses refPos.
	    For finding a nearest edge that satisfies a certain condition, the program can start search from blockDist=0, and increment blockDist until the edge is found,
	    or the block goes beyond the range covered by the lattice.
	    When this function returns, nTested stores the number of blocks tested.  If nTested==0, it means that there is no more blocks included when blockDist is incremented.
	*/
	YSRESULT MakeEdgePieceListByPointAndBlockDistance(
	     YsArray <YsShellVertexHandle,16> &edVtHd,YsArray <YSHASHKEY,16> &ceKey,YSSIZE_T &nTested,
	     const YsVec3 &refPos,int blockDist) const;

	YSBOOL CheckPolygonIntersection(YsShellPolygonHandle plHd) const;

	template <int minSize>
	inline YSRESULT GetIntersectingPolygon(YsArray <YsShellPolygonHandle,minSize> &itscPlHd,YsShellPolygonHandle fromPlHd) const;
	template <int minSize>
	inline YSRESULT GetIntersectingPolygon(YsArray <YsShellPolygonHandle,minSize> &itscPlHd,const YsVec3 &vtPos1,const YsVec3 &vtPos2) const;
	template <int minSize>
	inline YSRESULT GetIntersectingPolygon(YsArray <YsShellPolygonHandle,minSize> &itscPlHd,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;
	template <int minSize>
	inline YSRESULT GetIntersectingPolygon(YsArray <YsShellPolygonHandle,minSize> &itscPlHd,const YsPlane &pln) const;


	YSSIDE ShootRay
	(YsArray <YsVec3,16> &itsc,YsArray <YsShellPolygonHandle,16> &itscPlHd,
	 const YsVec3 &org,const YsVec3 &vec,YSBOOL watch=YSFALSE) const;
	YSRESULT CalculatePolygonRayIntersection  // Returns YSERR when the ray hits an edge or bad polygon.
	(YSBOOL &hit,YsVec3 &itsc,YSSIZE_T np,const YsVec3 p[],
	 const YsVec3 &ray1,const YsVec3 &ray2,YSBOOL watch=YSFALSE) const;


	YSRESULT ShootFiniteRay(
	    YsArray <YsVec3,16> &itsc,YsArray <YsShellPolygonHandle,16> &itscPlHd,
	    const YsVec3 &fr,const YsVec3 &to,
	    int nPlHdExclude=0,const YsShellPolygonHandle plHdExclude[]=NULL,YSBOOL watch=YSFALSE) const;
	YSRESULT ShootFiniteRayHighPrecision(
	    YsArray <YsVec3,16> &itsc,YsArray <YsShellPolygonHandle,16> &itscPlHd,
	    const YsVec3 &fr,const YsVec3 &to,
	    int nPlHdExclude=0,const YsShellPolygonHandle plHdExclude[]=NULL,YSBOOL watch=YSFALSE) const;

private:
	YSRESULT ShootFiniteRayWithPrecisionFlag(
	    YsArray <YsVec3,16> &itsc,YsArray <YsShellPolygonHandle,16> &itscPlHd,
	    const YsVec3 &fr,const YsVec3 &to,YSBOOL highPrecision,
	    int nPlHdExclude,const YsShellPolygonHandle plHdExclude[],YSBOOL watch) const;


public:
	YsShellPolygonHandle FindNearestPolygon
	   (double &dist,YsVec3 &nearPos,const YsVec3 &from,YSBOOL att4Test=YSFALSE,int att4Value=0) const;
	YsShellPolygonHandle FindNearestPolygonHighPrecision
	   (double &dist,YsVec3 &nearPos,const YsVec3 &from,YSBOOL att4Test=YSFALSE,int att4Value=0) const;

private:
	YsShellPolygonHandle FindNearestPolygonWithPrecisionFlag
	   (double &dist,YsVec3 &nearPos,const YsVec3 &from,YSBOOL highPrecision,YSBOOL att4Test=YSFALSE,int att4Value=0) const;

public:
	YSRESULT FindNearestEdgePiece(
	    YsShellVertexHandle edVtHd[2],YSHASHKEY &ceKey,double &dist,YsVec3 &nearPos,const YsVec3 &from,
	    const YsVec3 &dissimilarConst=YsOrigin()) const;

	YsShell::Edge FindNearestEdgePiece(YSHASHKEY &ceKey,double &dist,YsVec3 &nearPos,const YsVec3 &from,
	    const YsVec3 &dissimilarConst=YsOrigin()) const;

private:
	YSRESULT FillUndeterminedBlock(int bx,int by,int bz,YSSIDE side);

	YSRESULT RecursivelyFillUndeterminedBlock(int bx,int by,int bz,YSSIDE side,int limitter=64);

	YSSIDE SophisticatedCheckInsideSolidWhenTheCellIsMarkedAsBoundary(const YsVec3 &pnt,YSBOOL showDetail) const;
	YSSIDE ShootFiniteRay(const YsVec3 &pnt,int bx,int by,int bz,int cx,int cy,int cz,YSSIDE sideAtC) const;

	YSRESULT MakeListOfPolygonsThatGoThroughTheBlockRegion(
	    YsArray <YsShellPolygonHandle,16> &plgList,int x1,int y1,int z1,int x2,int y2,int z2) const;

	YSRESULT PreparePolygon(
	    YsArray <YsVec3,16> &plg,int &bx1,int &by1,int &bz1,int &bx2,int &by2,int &bz2,YsShellPolygonHandle plHd,const double thickness);
};

template <int minSize>
inline YSRESULT YsShellLattice::GetIntersectingPolygon(YsArray <YsShellPolygonHandle,minSize> &itscPlHd,YsShellPolygonHandle plHd) const
{
	YsArray <YsShellPolygonHandle,16> collPlHdList;
	int nOrgPlVtHd;
	const YsShellVertexHandle *orgPlVtHd;
	YsCollisionOfPolygon collision;
	const YsShell &shl=*(this->shl);
	YsArray <YsVec3,4> plVtPos;

	shl.GetVertexListOfPolygon(plVtPos,plHd);
	collision.SetPolygon1(plVtPos.GetN(),plVtPos);

	shl.GetVertexListOfPolygon(nOrgPlVtHd,orgPlVtHd,plHd);

	itscPlHd.CleanUp();

	if(YSOK==MakePossibllyCollidingPolygonList(collPlHdList,plHd))
	{
		for(YSSIZE_T i=0; i<collPlHdList.GetN(); ++i)
		{
			int nPlVt;
			const YsShellVertexHandle *plVtHd;
			YsVec3  edVtPos[2];
			YsShellVertexHandle edVtHd[2];

			shl.GetVertexListOfPolygon(nPlVt,plVtHd,collPlHdList[i]);

			edVtHd[1]=plVtHd[nPlVt-1];
			shl.GetVertexPosition(edVtPos[1],plVtHd[nPlVt-1]);
			for(YSSIZE_T j=0; j<nPlVt; j++)
			{
				edVtHd[0]=edVtHd[1];
				edVtHd[1]=plVtHd[j];

				edVtPos[0]=edVtPos[1];
				shl.GetVertexPosition(edVtPos[1],plVtHd[j]);

				if(YsIsIncluded <YsShellVertexHandle> (nOrgPlVtHd,orgPlVtHd,edVtHd[0])!=YSTRUE &&
				   YsIsIncluded <YsShellVertexHandle> (nOrgPlVtHd,orgPlVtHd,edVtHd[1])!=YSTRUE &&
				   collision.CheckIntersectionBetweenPolygon1AndLineSegment(edVtPos[0],edVtPos[1])==YSTRUE)
				{
					itscPlHd.Append(collPlHdList[i]);
				}
			}
		}
		return YSOK;
	}
	return YSERR;
}

template <int minSize>
inline YSRESULT YsShellLattice::GetIntersectingPolygon(YsArray <YsShellPolygonHandle,minSize> &itscPlHd,const YsVec3 &vtPos1,const YsVec3 &vtPos2) const
{
	YsArray <YsShellPolygonHandle,16> cannPlHd;

	itscPlHd.Set(0,NULL);
	if(MakePossibllyCollidingPolygonList(cannPlHd,vtPos1,vtPos2)==YSOK)
	{
		int i;
		YsCollisionOfPolygon collision;
		YsArray <YsVec3,4> plVtPos;

		forYsArray(i,cannPlHd)
		{
			shl->GetVertexListOfPolygon(plVtPos,cannPlHd[i]);
			collision.SetPolygon1(plVtPos.GetN(),plVtPos);

			if(collision.CheckIntersectionBetweenPolygon1AndLineSegment(vtPos1,vtPos2)==YSTRUE)
			{
				itscPlHd.Append(cannPlHd[i]);
			}
		}

		return YSOK;
	}
	return YSERR;
}

template <int minSize>
inline YSRESULT YsShellLattice::GetIntersectingPolygon(YsArray <YsShellPolygonHandle,minSize> &itscPlHd,const YsPlane &pln) const
{
	YsArray <unsigned,16> blkList;
	if(YSOK==MakeIntersectingBlockListPlane(blkList,0,0,0,nBlkX,nBlkY,nBlkZ,pln))
	{
		int i;
		YsArray <YsVec3,4> plVtPos;
		YsShellPolygonStore visited(*shl);
		for(i=0; i<=blkList.GetN()-3; i+=3)
		{
			const int bx=blkList[i];
			const int by=blkList[i+1];
			const int bz=blkList[i+2];

			const YsShellLatticeElem *elm=GetBlock(bx,by,bz);
			if(NULL!=elm)
			{
				int j;
				for(j=0; j<elm->plgList.GetN(); j++)
				{
					const YsShellPolygonHandle plHd=elm->plgList[j];
					if(YSTRUE!=visited.IsIncluded(plHd))
					{
						visited.AddPolygon(plHd);

						shl->GetVertexListOfPolygon(plVtPos,plHd);

						int k;
						YSBOOL side[2]={YSFALSE,YSFALSE};
						for(k=0; k<plVtPos.GetN() && (YSTRUE!=side[0] || YSTRUE!=side[1]); k++)
						{
							const int sideOfPln=pln.GetSideOfPlane(plVtPos[k]);
							if(0==sideOfPln)
							{
								side[0]=YSTRUE;
								side[1]=YSTRUE;
							}
							else if(0>sideOfPln)
							{
								side[1]=YSTRUE;
							}
							else
							{
								side[0]=YSTRUE;
							}
						}

						if(YSTRUE==side[0] && YSTRUE==side[1])
						{
							itscPlHd.Append(elm->plgList[j]);
						}
					}
				}
			}
		}

		// The following block is needed for preventing non-determinisim. >>
		YsArray <unsigned int,minSize> itscPlKey;
		itscPlKey.Set(itscPlHd.GetN(),NULL);
		for(i=0; i<itscPlHd.GetN(); i++)
		{
			itscPlKey[i]=shl->GetSearchKey(itscPlHd[i]);
		}
		YsQuickSort <unsigned int,YsShellPolygonHandle> (itscPlKey.GetN(),itscPlKey,itscPlHd);
		// The above block is needed for preventing non-determinisim. <<

		return YSOK;
	}
	itscPlHd.Clear();
	return YSERR;
}

template <int minSize>
inline YSRESULT YsShellLattice::GetIntersectingPolygon(YsArray <YsShellPolygonHandle,minSize> &itscPlHd,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	YsVec3 edVtPos[2];
	shl->GetVertexPosition(edVtPos[0],edVtHd1);
	shl->GetVertexPosition(edVtPos[1],edVtHd2);

	if(GetIntersectingPolygon(itscPlHd,edVtPos[0],edVtPos[1])==YSOK)
	{
		int i,j;
		int nPlVt;
		const YsShellVertexHandle *plVtHd;
		forYsArrayRev(i,itscPlHd)
		{
			shl->GetVertexListOfPolygon(nPlVt,plVtHd,itscPlHd[i]);

			for(j=0; j<nPlVt; j++)
			{
				if(plVtHd[j]==edVtHd1 || plVtHd[j]==edVtHd2)
				{
					itscPlHd.DeleteBySwapping(j);
					break;
				}
			}
		}

		return YSOK;
	}
	return YSERR;
}

/* } */
#endif
