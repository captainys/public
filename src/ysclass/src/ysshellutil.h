/* ////////////////////////////////////////////////////////////

File Name: ysshellutil.h
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

#ifndef YSSHELLUTIL_IS_INCLUDED
#define YSSHELLUTIL_IS_INCLUDED
/* { */

YSRESULT YsShellMakeQuadFromEdge
    (YsShellVertexHandle quad[4],YsShellPolygonHandle plHd[2],
     const YsShell &shl,const YsShellVertexHandle vtHd1,const YsShellVertexHandle vtHd2);

YSRESULT YsShellMakeConnectedPolygonList
    (YsArray <YsShellPolygonHandle> &plHdList,
     const YsShell &shl,YsShellPolygonHandle initPlHd,YSBOOL takeNonManifold,int hashSize=65533);


YSRESULT YsSlidePointOnShell
    (YsVec3 &newPos,YsVec3 &lastDir,YsShellPolygonHandle &newPlHd,
     YSBOOL &reachedDeadEnd,
     YsArray <YsVec3,16> *pathPnt,YsArray <YsShellPolygonHandle,16> *pathPlHd,
     const YsShell &shl,const YsVec3 &oldPos,const YsVec3 &displacement,YsShellPolygonHandle oldPlHd);

class YsShellVectorFunction
{
public:
	virtual YSRESULT GetVectorOnPolygon(YsVec3 &dir,const YsShell &shl,const YsShellPolygonHandle plHd,const YsVec3 &pos) const;
	virtual YSRESULT GetVectorOnVertex(YsVec3 &dir,const YsShell &shl,const YsShellVertexHandle vtHd) const;
};

/*! This function returns YSTRUE if a vector dir from point plg[fromIdx] is pointing inside of the polygon.
*/
YSBOOL YsVectorPointingInside(YSSIZE_T np,const YsVec3 plg[],const YsVec3 &nom,int fromIdx,const YsVec3 &dir);

/*! This function returns YSTRUE if a vector dir from edge plg[fromIdx]-plg[(fromIdx+1)%np] is pointing inside of the polygon.
*/
YSBOOL YsVectorPointingInsideFromEdge(YSSIZE_T np,const YsVec3 plg[],const YsVec3 &nom,int fromEdIdx,const YsVec3 &dir);


class YsShellCrawler
{
public:
	class State
	{
	public:
		YsShell::PolygonHandle plHd;

		/*! The crawler is not on an edge if edVtHd[0]==nullptr.
	 		The crawler is on a vertex if edVtHd[0]==edVtHd[1].
	 		Can be verified with checking state.
		*/
		YsShell::VertexHandle edVtHd[2];

		int state;
		YsVec3 pos,dir;

		State()
		{
			Initialize();
		}
		void Initialize(void)
		{
			plHd=nullptr;
			edVtHd[0]=nullptr;
			edVtHd[1]=nullptr;
			state=0;
			pos=YsVec3::Origin();
			dir=YsVec3::Origin();
		}
	};

	/*! A stopper function defines a condition when the crawling should be terminated.
	    If the crawler hits a vertex, edge, or polygon that the stopper function returns YSTRUE,
	    reachedDeadEnd will be set to YSTRUE, and the crawler won't move farther.
	*/
	class StopperFunction
	{
	public:
		virtual YSBOOL IsStopperVertex(const YsShell &shl,YsShellVertexHandle vtHd) const;
		virtual YSBOOL IsStopperEdge(const YsShell &shl,const YsShellVertexHandle edVtHd[2]) const;
		virtual YSBOOL IsStopperPolygon(const YsShell &shl,const YsShellPolygonHandle plHd) const;
	};

	/*! Can-pass fucntion defines where the crawler can pass through.
	    It is particularly useful when a crawling starts from or need to pass through a non-manifold edge, and
	    the crawler must go through specific polygons.

	    Difference between the stopper function is that it is a positive list, not a negative list.
	    Also, the crawler does not stop when this function return YSFALSE, instead the crawler will
	    look for another vertex, edge, or polygon, if there is a possibility.

	    LowPriorityPolygon return YSTRUE if a polygon is passable but want to avoid.  It is like weakly unpassable.
	    Since there is no other thoice, this function is only used when the case the crawler is moving across or starting from a vertex.

	    Can-pass function must be set before calling Start.
	*/
	class CanPassFunction
	{
	public:
		virtual YSBOOL CanPassPolygon(const YsShell &shl,const YsShellPolygonHandle plHd) const;
		virtual YSBOOL IsLowPriorityPolygon(const YsShell &shl,const YsShellPolygonHandle plHd) const;
	};

	enum
	{
		STATE_IN_POLYGON=0,
		STATE_ON_EDGE=1,
		STATE_ON_VERTEX=2
	};

	enum
	{
		CRAWL_TO_DIRECTION,
		CRAWL_TO_GOAL,
		CRAWL_ON_PLANE
	};

	YsShellPolygonHandle currentPlHd;
	YsVec3 currentPos,currentDir;
	const YsShellVectorFunction *crawlDirFunc;  // 2006/08/02
	const StopperFunction *crawlStopFunc; // 2007/02/07
private:
	const CanPassFunction *canPassFunc;

protected:
	YSBOOL reachedNearGoal;

public:
	YsVec3 goal;  // Used only when crawlingMode==1
	YsPlane constPlane;  // Used only when crawlingMode==2

	int currentState;  // 0:Inside the polygon    1:On an edge of the polygon    2:On a vertex of the polygon
	                   // Number shouldn't be changed.  Already some programs are using currentState value.
	int crawlingMode;  // 0:Follow the curvature (default)  1:Head to the goal  2:Crawler on a plane

	YsShellVertexHandle currentEdVtHd[2];

	YSBOOL reachedDeadEnd;
	YSBOOL reachedGoal;
	YSBOOL firstStep;

	YsArray <YsVec3,32> pathPnt;
	YsArray <YsShellPolygonHandle,32> pathPlHd;
	YsArray <YsVec2,32> prjPlg;
	YsVec2 prjPos,prjDir;


	YsShellCrawler();
	void Initialize(void);

	void SetMode(int mode);

	void SetConstraintPlane(const YsPlane &pln);

	/*! This function has no effect on the crawling result unless the mode is set to CRAWL_TO_GOAL.
	*/
	void SetGoal(const YsVec3 &Goal);

	/*! This function gives a can-pass function. 
	    Give nullptr to reset the function.  */
	void SetCanPassFunc(CanPassFunction *canPassFunc);


	/*! If crawling to goal, give goalposition-startPos as startDir. 
	    If the crawling mode is CRAWL_TO_GOAL, startDir will be ignored.
	*/
	YSRESULT Start(const YsShell &shl,const YsVec3 &startPos,const YsVec3 &startDir,YsShellPolygonHandle startPlHd,YSBOOL watch=YSFALSE);
	YSRESULT Start(const YsShell &shl,const YsVec3 &startPos,const YsVec3 &startDir,const YsShellVertexHandle startEdVtHd[2],YSBOOL watch=YSFALSE);
	YSRESULT Start(const YsShell &shl,const YsVec3 &startPos,const YsVec3 &startDir,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,YSBOOL watch=YSFALSE);
	YSRESULT Start(const YsShell &shl,const YsVec3 &startPos,const YsVec3 &startDir,YsShellVertexHandle startVtHd,YSBOOL watch=YSFALSE);
	YSRESULT Start(const YsShell &shl,const YsVec3 &startPos,const YsVec3 &startDir,YsShell::Elem startElem,YSBOOL watch=YSFALSE);

	/*! Moves the crawler by the given distance.  If the given distance is zero, it moves the crawler to the next polygon.
	    This function returns YSERR when an error prevents the crawler from moving, unless otherwise it returns YSOK.
	    The crawler may hit the closest position to the goal, or hit the non-manifold edge, or hit a polygon that can-pass function returns YSFALSE,
	    and then stops.  In that situation, this function still returns YSOK, but DeadEnd, ReachedGoal, or ReachedNearGoal function may return YSTRUE.
	*/
	YSRESULT Crawl(const YsShell &shl,const double &dist,YSBOOL watch=YSFALSE);  // dist==0.0 -> Move to the next polygon.
private:
	void FindOutGoingEdge(
	    double &moveDist,YsShell::VertexHandle nextEdVtHd[2],double &nextEdParam,

		const YsShell &shl,
	    const YsShell::VertexHandle currentEdVtHd[2],
	    int nPlVt,const YsShell::VertexHandle plVtHd[],const YsVec2 prjPlg[],
	    const YsVec2 &prjGoal,

	    YSBOOL watch);

public:
	/*! Returns YSTRUE if the crawler reaches a dead end. I.e., cannot crawl any longer. */
	YSBOOL DeadEnd(void) const;

	/*! Return YSTRUE if the crawler reached the goal. 
	*/
	YSBOOL ReachedGoal(void) const;

	/*! Return YSTRUE if the crawler can no longer move closer to the goal.
	    In such a situation, DeadEnd() will also return YSTRUE.
	*/
	YSBOOL ReachedNearGoal(void) const;


	/*! Create a crawling state that can be given to Restart function to continue crawling.
	    Since the initial direction is not given to this function, member variable dir must be specified before
	    the returned state is given to Restart function.
	*/
	static State Create(const YsShell &shl,YsShellVertexHandle startVtHd);

	/*! There might be a situation that there are multiple possible initial polygons or edges, one (or many) of which might give a 
	    desired crawling path.  To explore all possible crawling paths, use this function to obtain possible initial state, and 
	    use Restart to start from each initial state.
	*/
	YsArray <State> PossibleInitialState(const YsShell &shl,const YsVec3 &startPos,const YsVec3 &startDir,YsShellVertexHandle startVtHd) const;

	/*! Use PossibleInitialState instead.
	
	    This function returns possible polygons that the crawler may go in.
	    If the polygon can be uniquely identified, the length of the returned array is 1.
	    If there is an error, the length of the returned array is 0.
	    When the crawling starts from a vertex or an edge, there may be multiple possible polygons, in which case the length will be more than 1.
	    StartPos and StartDir must be the vertex position and a vector from the vertex position to the goal position respectively if the crawling mode is CRAWL_TO_GOAL.
	*/
	YsArray <YsShell::PolygonAndIndex> PossiblePolygonToGoIn(const YsShell &shl,const YsVec3 &startPos,const YsVec3 &startDir,YsShellVertexHandle startVtHd) const;
	YsArray <YsShell::PolygonAndIndex> PossiblePolygonToGoIn(const YsShell &shl,const YsVec3 &startPos,const YsVec3 &startDir,YsShell::Edge startEdge) const;
	YsArray <YsShell::PolygonAndIndex> PossiblePolygonToGoIn(const YsShell &shl,const YsVec3 &startPos,const YsVec3 &startDir,YsShell::Elem startElem) const;

protected:
	void LandOnEdge(
	    YsShell::VertexHandle nextEdVtHd[2],double &nextEdParam,
	    const YsVec2 &np,const YsVec2 &edVtPos0,const YsVec2 &edVtPos1,
	    const YsShell::VertexHandle tstEdVtHd[2]) const;

public:
	double GetTotalLength(void) const;

	YSRESULT Restart(const YsShell &shl,State sta,YSBOOL watch=YSFALSE);
	State GetCurrentState(void) const;

	YsConstArrayMask <YsVec3> GetPathPoint(void) const;

protected:
	// goal will be set to 0,0 if the crawling mode is not CRAWL_TO_GOAL
	void CrawlCalculateProjectedPolygonAndDirection(
	    const YsShell &shl,YsVec2 &dir,YsVec2 &goal,YsArray <YsVec2,32> &plg,int nPlVt,const YsShellVertexHandle plVtHd[],const YsVec3 &nom);
	YSRESULT MoveAcrossEdge(const YsShell &shl,const YsShellSearchTable *search,const YsVec3 &nom,const YsShellVertexHandle nextEdVtHd[2],const double &nextEdParam);
	YSRESULT MoveAcrossVertex(const YsShell &shl,const YsShellSearchTable *search,const YsVec3 &nom,const YsShellVertexHandle nextEdVtHd);
};

typedef YsShellCrawler::StopperFunction YsShellCrawlStopperFunction;

YSRESULT YsGetLoopAroundShellPolygon
   (YsArray <YsShellVertexHandle> &loop,const YsShell &shl,YsShellPolygonHandle plHd);

double YsGetCosDihedralAngleAcrossShellEdge
   (const YsShell &shl,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2);

YSRESULT YsShellFindPath
	    (YsArray <YsShellVertexHandle,16> &pathVtHd,
	     const YsShell &shl,YsShellVertexHandle from,YsShellVertexHandle to);

YSRESULT YsShellFindProximityPolygonListByVertex
   (YsArray <YsShellPolygonHandle,16> &plHdList,
    const YsShell &shl,YsShellVertexHandle from,const double &dist);
YSRESULT YsShellFindProximityPolygonListByEdge
   (YsArray <YsShellPolygonHandle,16> &plHdList,
    const YsShell &shl,YsShellVertexHandle fromEdVtHd[2],const double &dist);

YSRESULT YsShellMakePolygonStrip(
    YSBOOL &isLoop,YsArray <YsShellPolygonHandle> &plgStrip,
    const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[]);

YSRESULT YsMakePolygonSubGroup(
    YsArray <YsShellPolygonHandle,64> &plHdList,
    const YsShell &shl,YsShellPolygonHandle seedPlHd,const YsKeyStore &includedPlKey);



template <class T>
YSRESULT YsGetNthEdge(T edVt[2],int nPlVt,const T vt[],int n)
{
	if(0<=n && n<nPlVt-1)
	{
		edVt[0]=vt[n];
		edVt[1]=vt[n+1];
		return YSOK;
	}
	else if(n==nPlVt-1)
	{
		edVt[0]=vt[n];
		edVt[1]=vt[0];
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <class T>
YSRESULT YsGetNthEdgeCyclic(T edVt[2],int nPlVt,const T vt[],int n)
{
	while(n<0)
	{
		n+=nPlVt;
	}
	while(n>=nPlVt)
	{
		n-=nPlVt;
	}

	if(0<=n && n<nPlVt-1)
	{
		edVt[0]=vt[n];
		edVt[1]=vt[n+1];
		return YSOK;
	}
	else if(n==nPlVt-1)
	{
		edVt[0]=vt[n];
		edVt[1]=vt[0];
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShellGetNthEdge(YsShellVertexHandle edVtHd[2],int nPlVt,const YsShellVertexHandle plVtHd[],int n);
YSRESULT YsShellGetNthEdgeCyclic(YsShellVertexHandle edVtHd[2],int nPlVt,const YsShellVertexHandle plVtHd[],int n);


YSRESULT YsMergeShellPolygon(YsShell &shl,int nOrgPlg,const YsShellPolygonHandle orgPlHd[]);
YSRESULT YsGlueShellPolygon
    (int &nvNew,YsShellVertexHandle vNew[],
     int nv1,const YsShellVertexHandle v1[],int nv2,const YsShellVertexHandle v2[],
     YsShellVertexHandle sharedEdVtHd1,YsShellVertexHandle sharedEdVtHd2);


YSRESULT YsGetCrawlToNearestPointOnShell(YsVec3 &nearPos,YsShellElemType &nearElem,const YsShell &shl,const YsShellElemType &from,const YsVec3 &to);


////////////////////////////////////////////////////////////

template <class toFind>
class YsShellVertexAttribTable : public YsHandleToAttribTable <toFind,YsShell,YsShellVertexHandle>
{
public:
	YsShellVertexAttribTable();
	YsShellVertexAttribTable(const YsShell &shl);
	YsShellVertexAttribTable(const YsShellVertexAttribTable &incoming);
	YsShellVertexAttribTable(YsShellVertexAttribTable &&incoming);
	YsShellVertexAttribTable <toFind> &operator=(const YsShellVertexAttribTable &incoming);
	YsShellVertexAttribTable <toFind> &operator=(YsShellVertexAttribTable &&incoming);
	void SetShell(const YsShell &shl);
};
template <class toFind>
YsShellVertexAttribTable<toFind>::YsShellVertexAttribTable()
{
}
template <class toFind>
YsShellVertexAttribTable<toFind>::YsShellVertexAttribTable(const YsShell &shl)
{
	// GCC is falling apart.  It is obvious that SetStrage function here is the
	// SetStorage function of the base class which is:
	//    YsHandleToAttribTable <toFind,YsShell,YsShellVertexHandle>YsHandleToAttribTable <toFind,YsShell,YsShellVertexHandle>
	// But GCC requires to explicitly write the class name here.  I am not using multiple inheirance or anything complex.
	// Compiler knows which SetStorage to be used.
	// GCC used to be a good compiler.
	YsHandleToAttribTable <toFind,YsShell,YsShellVertexHandle>::SetStorage(shl);
}
template <class toFind>
YsShellVertexAttribTable<toFind>::YsShellVertexAttribTable(const YsShellVertexAttribTable <toFind> &incoming)
{
	this->CopyFrom(incoming);
}
template <class toFind>
YsShellVertexAttribTable<toFind>::YsShellVertexAttribTable(YsShellVertexAttribTable <toFind> &&incoming)
{
	this->MoveFrom(incoming);
}
template <class toFind>
YsShellVertexAttribTable <toFind> &YsShellVertexAttribTable<toFind>::operator=(const YsShellVertexAttribTable <toFind> &incoming)
{
	this->CopyFrom(incoming);
	return *this;
}
template <class toFind>
YsShellVertexAttribTable <toFind> &YsShellVertexAttribTable<toFind>::operator=(YsShellVertexAttribTable <toFind> &&incoming)
{
	this->MoveFrom(incoming);
	return *this;
}
template <class toFind>
void YsShellVertexAttribTable <toFind>::SetShell(const YsShell &shl)
{
	YsHandleToAttribTable <toFind,YsShell,YsShellVertexHandle>::SetStorage(shl);
}

////////////////////////////////////////////////////////////

template <class toFind>
class YsShellPolygonAttribTable : public YsHandleToAttribTable <toFind,YsShell,YsShellPolygonHandle>
{
public:
	YsShellPolygonAttribTable();
	YsShellPolygonAttribTable(const YsShell &shl);
	YsShellPolygonAttribTable(const YsShellPolygonAttribTable &incoming);
	YsShellPolygonAttribTable(YsShellPolygonAttribTable &&incoming);
	YsShellPolygonAttribTable <toFind> &operator=(const YsShellPolygonAttribTable &incoming);
	YsShellPolygonAttribTable <toFind> &operator=(YsShellPolygonAttribTable &&incoming);
	void SetShell(const YsShell &shl);
};
template <class toFind>
YsShellPolygonAttribTable<toFind>::YsShellPolygonAttribTable()
{
}
template <class toFind>
YsShellPolygonAttribTable<toFind>::YsShellPolygonAttribTable(const YsShell &shl)
{
	YsHandleToAttribTable <toFind,YsShell,YsShellPolygonHandle>::SetStorage(shl);
}
template <class toFind>
YsShellPolygonAttribTable<toFind>::YsShellPolygonAttribTable(const YsShellPolygonAttribTable <toFind> &incoming)
{
	this->CopyFrom(incoming);
}
template <class toFind>
YsShellPolygonAttribTable<toFind>::YsShellPolygonAttribTable(YsShellPolygonAttribTable <toFind> &&incoming)
{
	this->MoveFrom(incoming);
}
template <class toFind>
YsShellPolygonAttribTable <toFind> &YsShellPolygonAttribTable<toFind>::operator=(const YsShellPolygonAttribTable <toFind> &incoming)
{
	this->CopyFrom(incoming);
	return *this;
}
template <class toFind>
YsShellPolygonAttribTable <toFind> &YsShellPolygonAttribTable<toFind>::operator=(YsShellPolygonAttribTable <toFind> &&incoming)
{
	this->MoveFrom(incoming);
	return *this;
}
template <class toFind>
void YsShellPolygonAttribTable <toFind>::SetShell(const YsShell &shl)
{
	YsHandleToAttribTable <toFind,YsShell,YsShellPolygonHandle>::SetStorage(shl);
}

////////////////////////////////////////////////////////////

template <class toFind>
class YsShellEdgeAttribTable : public YsMultiHandleToAttribTable <toFind,YsShell,YsShellVertexHandle,2>
{
public:
	YsShellEdgeAttribTable();
	YsShellEdgeAttribTable(const YsShell &shl);
	YsShellEdgeAttribTable(const YsShellEdgeAttribTable <toFind>&incoming);
	YsShellEdgeAttribTable(YsShellEdgeAttribTable <toFind>&&incoming);
	YsShellEdgeAttribTable <toFind> &operator=(const YsShellEdgeAttribTable <toFind>&incoming);
	YsShellEdgeAttribTable <toFind> &operator=(YsShellEdgeAttribTable <toFind>&&incoming);
	void SetShell(const YsShell &shl);

	using YsMultiHandleToAttribTable <toFind,YsShell,YsShellVertexHandle,2>::FindAttrib;
	toFind *FindAttrib(YsShellVertexHandle hd0,YsShellVertexHandle hd1);
	const toFind *FindAttrib(YsShellVertexHandle hd0,YsShellVertexHandle hd1) const;
};

template <class toFind>
YsShellEdgeAttribTable<toFind>::YsShellEdgeAttribTable()
{
}
template <class toFind>
YsShellEdgeAttribTable<toFind>::YsShellEdgeAttribTable(const YsShell &shl)
{
	YsMultiHandleToAttribTable <toFind,YsShell,YsShellVertexHandle,2>::SetStorage(shl);
}
template <class toFind>
YsShellEdgeAttribTable<toFind>::YsShellEdgeAttribTable(const YsShellEdgeAttribTable <toFind> &incoming)
{
	this->CopyFrom(incoming);
}
template <class toFind>
YsShellEdgeAttribTable<toFind>::YsShellEdgeAttribTable(YsShellEdgeAttribTable <toFind> &&incoming)
{
	this->MoveFrom(incoming);
}
template <class toFind>
YsShellEdgeAttribTable <toFind> &YsShellEdgeAttribTable<toFind>::operator=(const YsShellEdgeAttribTable &incoming)
{
	this->CopyFrom(incoming);
	return *this;
}
template <class toFind>
YsShellEdgeAttribTable <toFind> &YsShellEdgeAttribTable<toFind>::operator=(YsShellEdgeAttribTable &&incoming)
{
	this->MoveFrom(incoming);
	return *this;
}
template <class toFind>
void YsShellEdgeAttribTable<toFind>::SetShell(const YsShell &shl)
{
	YsMultiHandleToAttribTable <toFind,YsShell,YsShellVertexHandle,2>::SetStorage(shl);
}
template <class toFind>
toFind *YsShellEdgeAttribTable<toFind>::FindAttrib(YsShellVertexHandle hd0,YsShellVertexHandle hd1)
{
	const YsShellVertexHandle edVtHd[2]={hd0,hd1};
	return FindAttrib(edVtHd);
}
template <class toFind>
const toFind *YsShellEdgeAttribTable<toFind>::FindAttrib(YsShellVertexHandle hd0,YsShellVertexHandle hd1) const
{
	const YsShellVertexHandle edVtHd[2]={hd0,hd1};
	return FindAttrib(edVtHd);
}

////////////////////////////////////////////////////////////

template <class toFind>
class YsShellEdgeMultiAttribTable : public YsMultiHandleToMultiAttribTable <toFind,YsShell,YsShellVertexHandle,2>
{
public:
	YsShellEdgeMultiAttribTable();
	YsShellEdgeMultiAttribTable(const YsShell &shl);
	YsShellEdgeMultiAttribTable(const YsShellEdgeMultiAttribTable &incoming);
	YsShellEdgeMultiAttribTable(YsShellEdgeMultiAttribTable &&incoming);
	YsShellEdgeMultiAttribTable <toFind> &operator=(const YsShellEdgeMultiAttribTable <toFind> &incoming);
	YsShellEdgeMultiAttribTable <toFind> &operator=(YsShellEdgeMultiAttribTable <toFind> &&incoming);
	void SetShell(const YsShell &shl);

	using YsMultiHandleToMultiAttribTable <toFind,YsShell,YsShellVertexHandle,2>::CleanUp;
	using YsMultiHandleToMultiAttribTable <toFind,YsShell,YsShellVertexHandle,2>::FindAttrib;
	YSRESULT FindAttrib(int &nFind,const toFind *&find,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
	YSRESULT FindAttrib(int &nFind,const toFind *&find,YSHASHKEY edVtHd0,YSHASHKEY edVtHd1) const;
	YsArray <toFind> FindAttrib(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
	YsArray <toFind> FindAttrib(YSHASHKEY edVtHd0,YSHASHKEY edVtHd1) const;
};

template <class toFind>
YsShellEdgeMultiAttribTable<toFind>::YsShellEdgeMultiAttribTable()
{
}
template <class toFind>
YsShellEdgeMultiAttribTable<toFind>::YsShellEdgeMultiAttribTable(const YsShell &shl)
{
	YsMultiHandleToMultiAttribTable <toFind,YsShell,YsShellVertexHandle,2>::SetShell(shl);
}
template <class toFind>
YsShellEdgeMultiAttribTable<toFind>::YsShellEdgeMultiAttribTable(const YsShellEdgeMultiAttribTable &incoming)
{
	this->CopyFrom(incoming);
}
template <class toFind>
YsShellEdgeMultiAttribTable<toFind>::YsShellEdgeMultiAttribTable(YsShellEdgeMultiAttribTable &&incoming)
{
	this->MoveFron(incoming);
}
template <class toFind>
YsShellEdgeMultiAttribTable <toFind> &YsShellEdgeMultiAttribTable<toFind>::operator=(const YsShellEdgeMultiAttribTable <toFind> &incoming)
{
	this->CopyFrom(incoming);
	return *this;
}
template <class toFind>
YsShellEdgeMultiAttribTable <toFind> &YsShellEdgeMultiAttribTable<toFind>::operator=(YsShellEdgeMultiAttribTable <toFind> &&incoming)
{
	this->MoveFrom(incoming);
	return *this;
}
template <class toFind>
void YsShellEdgeMultiAttribTable<toFind>::SetShell(const YsShell &shl)
{
	YsMultiHandleToMultiAttribTable <toFind,YsShell,YsShellVertexHandle,2>::SetStorage(shl);
}
template <class toFind>
YSRESULT YsShellEdgeMultiAttribTable<toFind>::FindAttrib(int &nFind,const toFind *&find,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	const YsShellVertexHandle edVtHd[2]={edVtHd0,edVtHd1};
	return FindAttrib(nFind,find,edVtHd);
}
template <class toFind>
YSRESULT YsShellEdgeMultiAttribTable<toFind>::FindAttrib(int &nFind,const toFind *&find,YSHASHKEY edVtHd0,YSHASHKEY edVtHd1) const
{
	const YSHASHKEY edVtKey[2]={edVtHd0,edVtHd1};
	return FindAttrib(nFind,find,edVtKey);
}

template <class toFind>
YsArray <toFind> YsShellEdgeMultiAttribTable<toFind>::FindAttrib(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	YsArray <toFind> result;

	int nFound;
	const toFind *found;
	if(YSOK==FindAttrib(nFound,found,edVtHd0,edVtHd1))
	{
		result.Set(nFound,found);
	}
	return result;
}

template <class toFind>
YsArray <toFind> YsShellEdgeMultiAttribTable<toFind>::FindAttrib(YSHASHKEY edVtHd0,YSHASHKEY edVtHd1) const
{
	YsArray <toFind> result;

	int nFound;
	const toFind *found;
	if(YSOK==FindAttrib(nFound,found,edVtHd0,edVtHd1))
	{
		result.Set(nFound,found);
	}
	return result;
}

/* } */
#endif
