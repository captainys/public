/* ////////////////////////////////////////////////////////////

File Name: yswizard.h
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

#ifndef YSWIZARD_IS_INCLUDED
#define YSWIZARD_IS_INCLUDED
/* { */

#include "ysshell.h"
#include "ysrect.h"

// Declaration /////////////////////////////////////////////
class YsWizard
{
public:
	static const char *ClassName;
	virtual const char *WhatItIs(void){return YsWizard::ClassName;}
};

// Declaration /////////////////////////////////////////////
class YsCollisionOfPolygon : public YsWizard
{
public:
	static const char *ClassName;
	virtual const char *WhatItIs(void){return YsCollisionOfPolygon::ClassName;}

	YsCollisionOfPolygon();

	int GetNumVertexOfPolygon1(void) const;
	const YsVec3 *GetVertexListOfPolygon1(void) const;
	int GetNumVertexOfPolygon2(void) const;
	const YsVec3 *GetVertexListOfPolygon2(void) const;

	void SetPolygon1(YSSIZE_T nv1,const YsVec3 v1[]);
	void SetPolygon2(YSSIZE_T nv2,const YsVec3 v2[]);
	YSRESULT PrecomputeProjectionOfPolygon1(void);
	YSRESULT PrecomputeProjectionOfPolygon2(void);
	void SetPolygon1(YSSIZE_T nv1,const YsVec3 v1[],const YsVec3 &nom1);
	void SetPolygon2(YSSIZE_T nv2,const YsVec3 v2[],const YsVec3 &nom2);
	YSBOOL CheckOnTheSamePlane(void) const;
	YSBOOL CheckCollision(void) const;  // Just check true/false
	YSBOOL CheckCollision(YsVec3 &firstFound) const;  // Just check true/false
	YSINTERSECTION CheckPenetration(void) const;  // Check aprt/touch/penetrate

	YSBOOL OneEdgeLiesOnTheOtherPlane(void) const;

	YSBOOL CheckIntersectionBetweenPolygon1AndBlock(const YsVec3 &min,const YsVec3 &max) const;
	YSBOOL CheckIntersectionBetweenPolygon1AndLineSegment(const YsVec3 &p1,const YsVec3 &p2) const;
	YSBOOL CheckIntersectionBetweenPolygon1AndLineSegment(YsVec3 &crs,const YsVec3 &p1,const YsVec3 &p2) const;  // 2008/11/06

protected:
	YSSIZE_T np1;
	const YsVec3 *p1;
	YsVec3 p1min,p1max;
	YsPlane pln1;

	YsArray <YsVec2,16> p1Prj;
	YsVec2 p1PrjMin,p1PrjMax;
	YsMatrix4x4 p1PrjMat;

	YSSIZE_T np2;
	const YsVec3 *p2;
	YsVec3 p2min,p2max;
	YsPlane pln2;

	YsArray <YsVec2,16> p2Prj;
	YsVec2 p2PrjMin,p2PrjMax;
	YsMatrix4x4 p2PrjMat;

	YSSIDE CheckInsideOfPolygon1(const YsVec3 &tst) const;
	YSSIDE CheckInsideOfPolygon2(const YsVec3 &tst) const;
};

// Declaration /////////////////////////////////////////////
class YsSwordPolygonAttr
{
public:
	YsSwordPolygonAttr();
	YSBOOL isOverlappingPolygon;  // For finding overlapping polygon
	int noteOverlapping;          // for polygon ID or any purpose
};

class YsSwordPolygon
{
public:
	YsSwordPolygonAttr attrSet;

	YsArray <YsVec3> plg;  // Original Polygon
	YsArray <YsVec2> prj;  // Projected Polygon
	YsArray <int> vtId;    // Corresponding Vertex Id (optional)
	YsArray <YsShellVertexHandle> vtHd;  // Corresponding Vertex Handle (optional)
};

// Declaration /////////////////////////////////////////////
class YsSwordNewVertexLog
{
public:
	YsSwordNewVertexLog();

	int freeAttribute;
	int betweenVt1,betweenVt2;
	int idCreated;
	YsVec3 pos;
};

// Declaration /////////////////////////////////////////////

class YsSword : public YsWizard
{
private:
	const YsSword &operator=(const YsSword &from);  // Don't use copy operator
	YsSword(const YsSword &from); // Don't use copy constructor
public:
	static const char *ClassName;
	virtual const char *WhatItIs(void){return YsSword::ClassName;}

	YsSword();
	~YsSword();

	void Encache(void) const;

	int GetNumPolygon(void) const;
	int GetNumVertexOfPolygon(YSSIZE_T id) const;
	const YsArray <YsVec3> *GetPolygon(YSSIZE_T id) const;
	const YsArray <int> *GetVertexIdList(YSSIZE_T id) const;
	const YsArray <YsShell::VertexHandle> *GetVtHdList(YSSIZE_T id) const;

	int GetNumNewVertex(void) const;
	const YsSwordNewVertexLog *GetNewVertexLog(int i) const;

	// For 3D Operation (Slash by plane)
	YSRESULT SetInitialPolygon(YSSIZE_T np,const YsVec3 p[]);
	YSRESULT SetInitialPolygon(YSSIZE_T np,const YsVec3 p[],const int id[]);
	YSRESULT SetInitialPolygon(YSSIZE_T np,const YsVec3 p[],const int id[],const YsShell::VertexHandle vtHd[]);

public:
	YSRESULT Slash(const YsPlane &pln);
	YSRESULT Slash(YSSIZE_T nv,const YsVec3 v[]);
	YSRESULT SlashByOverlappingPolygon3(YSSIZE_T nv,const YsVec3 v[],int note=0);
	YSBOOL IsFromOverlappingPolygon3(YSSIZE_T i) const;
	YSBOOL IsFromOverlappingPolygon3(YSSIZE_T i,int &note) const;
	YSRESULT GetVertexListOfPolygon(YsVec3 vec[],YSSIZE_T maxcount,int id) const;
	YSRESULT GetVertexListOfPolygon(const YsVec3 *vec[],YSSIZE_T maxcount,int id) const;

	// For 2D Operation (Slash by line)
	YSRESULT SetInitialPolygon(YSSIZE_T np,const YsVec2 p[],const int id[]=NULL);
	YSRESULT Slash(const YsVec2 &org,const YsVec2 &vec);
	YSRESULT GetVertexListOfPolygon(YsVec2 vec[],YSSIZE_T maxcount,int id) const;

	// For both
	YSRESULT Triangulate(YSCONVEXNIZESTRATEGY strategy=YSCONVEXNIZEDEFAULT);
	YSRESULT Convexnize(YSCONVEXNIZESTRATEGY strategy=YSCONVEXNIZEDEFAULT);

	// Update Ids
	YSRESULT UpdateVertexId(int oldId,int newId);

	YSBOOL debugMode;

protected:
	YsList <YsSwordPolygon> *SeparateByTwo(YsList <YsSwordPolygon> *org,int id1,int id2);

	YsList <YsSwordPolygon> *SeparateByMulti(YsList <YsSwordPolygon> *org,YsArray <int> &cutPoint);


	YSRESULT SlashOnePolygon(YsList <YsSwordPolygon> *target,const YsPlane &pln,YSSIZE_T nv=0,const YsVec3 v[]=NULL);
	YSRESULT FindAddSlashPoint
	    (YsArray <int> &idx,YsList <YsSwordPolygon> *taget,const YsPlane &pln,
	     int nCuttingPolygonVtx=0,const YsVec3 *cuttingPolygon=NULL);
	YSRESULT FindAddSlashPointToNeighborPolygons
		(const YsVec3 &v1,const YsVec3 &v2,
		 const YsVec3 &newVtx3,const YsVec2 &newVtx2,int newCreatedId,YsList <YsSwordPolygon> *exclude);

	YSRESULT ConvexnizeGo(YSBOOL tri,YsList <YsSwordPolygon> *target,YSCONVEXNIZESTRATEGY strategy);
	YsList <YsSwordPolygon> *ConvexnizeOnePolygon
	    (YSBOOL tri,YsList <YsSwordPolygon> *target,YSCONVEXNIZESTRATEGY strategy);

	YSRESULT FindConvexnizeVertexPair(int &id1,int &id2,YsList <YsSwordPolygon> *target,YSCONVEXNIZESTRATEGY strategy);

	int newCreateId;
	YsMatrix4x4 plgPrjMatrix;
	YsList <YsSwordPolygon> *plg;
	YsList <YsSwordNewVertexLog> *cre;
};

// Declaration /////////////////////////////////////////////
class YsBoundingBoxMaker2 : public YsWizard
{
public:
	static const char *ClassName;
	virtual const char *WhatItIs(void){return YsBoundingBoxMaker2::ClassName;}

	YsBoundingBoxMaker2();

	void Begin(void);
	void Begin(const YsVec2 &vec);
	void Add(const YsVec2 &vec);
	void Make(YSSIZE_T n,const YsVec2 v[]);
	void Make(YsList <YsVec2> *lst);
	void Make(const YsArray <YsVec2> &ary);
	void Get(YsVec2 &min,YsVec2 &max) const;
	void Get(YsVec2 minmax[]) const;
	const YsVec2 &GetCenter(YsVec2 &cen) const;

	YSBOOL IsInside(const YsVec2 &pos) const;  // 2008/09/16
	YSBOOL IsInside(const YsVec3 &pos) const;  // 2008/09/16
	YsVec2 Center(void) const;                 // 2008/09/16

	YsVec2 GetDimension(void) const;

	const YsVec2 &operator[](YSSIZE_T idx) const;

protected:
	YSBOOL first;
	YsVec2 min,max;
};

// Declaration /////////////////////////////////////////////
class YsBoundingBoxMaker3 : public YsWizard
{
public:
	static const char *ClassName;
	virtual const char *WhatItIs(void){return YsBoundingBoxMaker3::ClassName;}

	YsBoundingBoxMaker3();
	template <const int N>
	inline YsBoundingBoxMaker3(const YsArray <YsVec3,N> &ary);

	void Begin(void);
	void Begin(const YsVec3 &vec);

	void Add(const YsVec3 &vec);
	void Add(YSSIZE_T n,const YsVec3 v[]);
	template <const int N>
	void Add(const YsArray <YsVec3,N> &v);

	void Make(YSSIZE_T n,const YsVec3 v[]);
	void Make(const YsList <YsVec3> *lst);
	template <const int N>
	void Make(const YsArray <YsVec3,N> &ary);

	void Make(const YsShell &shl,YSSIZE_T nVt,const YsShellVertexHandle vtHd[]);
	void Get(YsVec3 &min,YsVec3 &max) const;
	void Get(YsVec3 minMax[2]) const;
	const YsVec3 &GetCenter(YsVec3 &cen) const;
	YsVec3 GetCenter(void) const;

	YSBOOL IsInside(const YsVec3 &pos) const;    // 2008/09/16
	YSBOOL IsInside(const YsVec2 &pos) const;    // 2008/09/16
	YSBOOL IsInsideXY(const YsVec3 &pos) const;  // 2008/09/16
	YSBOOL IsInsideXY(const YsVec2 &pos) const;  // 2008/09/16
	YsVec3 Center(void) const;                   // 2008/09/16

	YsVec3 GetDimension(void) const;

	/*! Returns the length of minimum to maximum. */
	const double GetDiagonalLength(void) const;

	const YsVec3 &operator[](YSSIZE_T idx) const;

protected:
	YSBOOL first;
	YsVec3 min,max;
};

template <const int N>
inline YsBoundingBoxMaker3::YsBoundingBoxMaker3(const YsArray <YsVec3,N> &ary)
{
	first=YSTRUE;
	Make(ary);
}

template <const int N>
void YsBoundingBoxMaker3::Add(const YsArray <YsVec3,N> &v)
{
	return Add(v.GetN(),v);
}

template <const int N>
void YsBoundingBoxMaker3::Make(const YsArray <YsVec3,N> &ary)
{
	if(ary.GetNumItem()>0)
	{
		Begin(ary.GetItem(0));
	}
	int i;
	for(i=0; i<ary.GetNumItem(); i++)
	{
		Add(ary.GetItem(i));
	}
}

////////////////////////////////////////////////////////////

template <class VecType>
class YsBoundingBoxMaker : public YsWizard, public YsRectTemplate <VecType>
{
private:
	YSBOOL first;

public:
	using YsRectTemplate<VecType>::Min;
	using YsRectTemplate<VecType>::Max;
	using YsRectTemplate<VecType>::MinMax;
	using YsRectTemplate<VecType>::GetCenter;
	using YsRectTemplate<VecType>::GetSize;
	using YsRectTemplate<VecType>::GetDimension;
	using YsRectTemplate<VecType>::GetDiagonalLength;
	using YsRectTemplate<VecType>::Set;
	using YsRectTemplate<VecType>::operator[];
	using YsRectTemplate<VecType>::IsInside;



	/*! Default constructor.
	*/
	YsBoundingBoxMaker()
	{
		Begin();
	}
	/*! Constructor that constructs a bounding box from an array of VecTypes.
	*/
	YsBoundingBoxMaker(const YsConstArrayMask <VecType> &ary)
	{
		Begin();
		Add(ary);
	}
	/*! Begin constructing a bounding box.
	*/
	void Begin(void)
	{
		first=YSTRUE;
		Set(VecType::Origin(),VecType::Origin());
	}
	/*! Begin constructing a bounding box with the first vector.
	*/
	void Begin(const VecType &vec)
	{
		first=YSFALSE;
		YsRectTemplate<VecType>::Set(vec,vec);
	}
	/*! Update the bounding box with the new vector.
	*/
	void Add(const VecType &vec)
	{
		if(YSTRUE==first)
		{
			first=YSFALSE;
			Set(vec,vec);
		}
		else
		{
			auto min=YsRectTemplate<VecType>::Min(),max=YsRectTemplate<VecType>::Max();
			for(int i=0; i<VecType::NDIMENSION; ++i)
			{
				YsMakeSmaller(min[i],vec[i]);
				YsMakeGreater(max[i],vec[i]);
			}
			YsRectTemplate<VecType>::Set(min,max);
		}
	}
	/*! Update the bounding box with the new vectors.
	*/
	void Add(YSSIZE_T n,const VecType v[])
	{
		for(int i=0; i<n; ++i)
		{
			Add(v[i]);
		}
	}
	/*! Update the bounding box with the new array of VecTypes.
	*/
	void Add(const YsConstArrayMask <VecType> &ary)
	{
		for(auto &v : ary)
		{
			Add(v);
		}
	}
	/*! Update the bounding box by adding another rect.
	*/
	void Add(const YsRectTemplate <VecType> &rect)
	{
		Add(rect.Min());
		Add(rect.Max());
	}
	/*! Make a bounding box with an array of vectors.
	*/
	void Make(YSSIZE_T n,const VecType v[])
	{
		Begin();
		Add(n,v);
	}
	/*! Make a bounding box with an array of vectors.
	*/
	void Make(const YsConstArrayMask <VecType> ary)
	{
		Begin();
		Add(ary);
	}
	/*! Returns the constructed bounding box.
	*/
	void Get(VecType &min,VecType &max) const
	{
		min=YsRectTemplate<VecType>::Min();
		max=YsRectTemplate<VecType>::Max();
	}
	/*! Returns the constructed bounding box.
	*/
	void Get(YsVec3 minMax[2]) const
	{
		minMax[0]=YsRectTemplate<VecType>::Min();
		minMax[1]=YsRectTemplate<VecType>::Max();
	}
	/*! Returns the center of the bounding box.
	*/
	const VecType &GetCenter(VecType &cen) const
	{
		cen=YsRectTemplate<VecType>::GetCenter();
		return cen;
	}

	/*! Returns the center of the bounding box.
	*/
	VecType GetCenter(void) const
	{
		return YsRectTemplate<VecType>::GetCenter();
	}

	/*! Returns the bounding box as an array.
	*/
	const VecType &operator[](YSSIZE_T idx) const
	{
		return YsRectTemplate<VecType>::MinMax()[idx];
	}

	/*! Returns the diagonal (max-min). 
	*/
	const VecType GetDiagonal(void) const
	{
		return YsRectTemplate<VecType>::Max()-YsRectTemplate<VecType>::Min();
	}
};


////////////////////////////////////////////////////////////

class YsTraceLineSegment
{
public:
	class Tracer
	{
	public:
		YsVec3 pos;
		YSSIZE_T seg;
		double segParam;

		/* This flag will be set to YSTRUE when the tracer crosses the first point if the line segment is a loop.
		   This flag will be ignored by the comparison operators.
		*/
		YSBOOL wentAround;

		const YsVec3 &GetPosition(void) const;
		YSSIZE_T GetSegment(void) const;

		bool operator<(const Tracer &t)
		{
			if(this->seg<t.seg)
			{
				return true;
			}
			if(this->seg==t.seg)
			{
				return this->segParam<t.segParam;
			}
			return false;
		}
		bool operator>(const Tracer &t)
		{
			if(this->seg>t.seg)
			{
				return true;
			}
			if(this->seg==t.seg)
			{
				return this->segParam>t.segParam;
			}
			return false;
		}
		bool operator<=(const Tracer &t)
		{
			if(this->seg<t.seg)
			{
				return true;
			}
			if(this->seg==t.seg)
			{
				return this->segParam<=t.segParam;
			}
			return false;
		}
		bool operator>=(const Tracer &t)
		{
			if(this->seg>t.seg)
			{
				return true;
			}
			if(this->seg==t.seg)
			{
				return this->segParam>=t.segParam;
			}
			return false;
		}
		bool operator==(const Tracer &t)
		{
			if(this->seg==t.seg && fabs(this->segParam-t.segParam)<YsTolerance)
			{
				return true;
			}
			return false;
		}
	};

protected:
	YSBOOL isLoop;
	YsArray <YsVec3> lSeg;

	Tracer curPos;

	double totalLength;
	YsArray <double> segLength;
	YsArray <double> segLengthAddUp;

	YSRESULT ResetLineSegment(void);

public:
	/*! Set line segments in the data structure.
	    This function also calculates length cache.
	*/
	YSRESULT SetLineSegment(YSSIZE_T np,const YsVec3 p[],YSBOOL isLoop);
	template <const int N>
	YSRESULT SetLineSegment(YsArray <YsVec3,N> &p,YSBOOL isLoop);

	YSSIZE_T GetCurrentSegment(void) const;
	const YsVec3 &GetCurrentPosition(void) const;
	const YsVec3 GetCurrentTangent(void) const;
	const double &GetTotalLength(void) const;

	/*! Returns YSTRUE if it is a loop, YSFALSE otherwise.
	*/
	YSBOOL IsLoop(void) const;

	YSRESULT MoveByDistance(const double &dist);
	YSRESULT SetPositionByParameter(const double &t);

	/*! Return a tracer at the first point of the line segments.
	*/
	Tracer Head(void) const;

	/*! Return a tracer at the last point of the line segments.
	    If it is a loop, it will be the last point.
	*/
	Tracer Tail(void) const;

	YSRESULT MoveByDistance(Tracer &tracer,const double &dist) const;

	/*! Starting from the current tracer position, this function moves the tracer to the location
	    where the distance from the first point is dist.
	*/
	YSRESULT MoveToDistanceFromP0(Tracer &tracer,double dist) const;

	YSRESULT SetPosition(Tracer &tracer,const YsVec3 &pos) const;

	YSRESULT SetPositionByParameter(Tracer &tracer,const double &t) const;
	YSRESULT SetPositionByVertex(Tracer &tracer,int idx) const;
	double GetParameter(const Tracer &tracer) const;
};

typedef YsTraceLineSegment::Tracer YsLineSegmentTracer;



template <const int N>
YSRESULT YsTraceLineSegment::SetLineSegment(YsArray <YsVec3,N> &p,YSBOOL islp)
{
	lSeg=p;
	isLoop=islp;
	if(islp==YSTRUE)
	{
		lSeg.Append(lSeg[0]);
	}
	return ResetLineSegment();
}



class YsUniqueColorAssignmentUtil
{
public:
	class ColorInfo
	{
	public:
		YsColor col;
		YSBOOL used;
	};
private:
	YsArray <ColorInfo,8> table;

public:
	YsUniqueColorAssignmentUtil();
	void UseBlack(void);
	void UseWhite(void);

	void ClearUsed(void);
	void Use(YsColor c);

	/*! This function finds and returns an unused color in the color table.
	    If all colors are used, it creates one with MakeNewColor function.
	*/
	const YsColor GetUnused(void);

	const YsColor *FindUnused(void) const;
	const YsColor MakeNewColor(void);
private:
	int GetMaximumSpacing(YsArray <int> &v) const;
};



/*! This class is used for finding the most similar pair from two sets of unit vectors.
*/
class YsSimilarVectorFinder
{
public:
	/*! If YSTRUE, two vectors making nearly 180 degrees angle are considered similar.
	    Or, if YSTRUE, it uses fabs(a*b) instead of a*b for checking similarity between two vectors a and b.
	*/
	YSBOOL biDirectional;

	/*! After FindSimilarVector, this array will retain indices to the most similar vectors.
	*/
	int similarIdx[2];

	/*! After FindSimilarVector, this array will retain the most similar vectors.
	*/
	YsVec3 similarVec[2];

	/*! If YSTRUE==biDirectional, this value will be set to fabs(similarVec[0]*similarVec[1]), or
	    if YSFALSE==biDirectional, similarVec[0]*similarVec[1].
	*/
	double similarity;

	YsSimilarVectorFinder();

	YSRESULT FindSimilarVector(YSSIZE_T n1,const YsVec3 v1[],YSSIZE_T n2,const YsVec3 v2[]);

	/*! This function finds the most similar vector to ref among v[0] to v[n-1].
	    similarIdx[0] and similarVec[0] will be the most similar vector.
	    similarIdx[1] will be -1, and similarVec[1] will be a zero vector.
	*/
	YSRESULT FindSimilarVector(YSSIZE_T n,const YsVec3 v[],const YsVec3 &ref);
};

/* } */
#endif
