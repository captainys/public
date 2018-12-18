#ifndef YSSHELLEXT_CRAWLINGUTIL_IS_INCLUDED
#define YSSHELLEXT_CRAWLINGUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>

class YsShellExt_EdgeCrawler : protected YsShellCrawler
{
public:
	class CanPassFunction
	{
	public:
		virtual YSBOOL CanPassEdge(const YsShellExt &shl,const YsShell::Edge &edge) const=0;
	};
	class CanPassOnConstEdge : public CanPassFunction
	{
	public:
		YsShellExt::ConstEdgeHandle ceHd;
		CanPassOnConstEdge()
		{
			ceHd=nullptr;
		}
		virtual YSBOOL CanPassEdge(const YsShellExt &shl,const YsShell::Edge &edge) const
		{
			auto edCeHd=shl.FindConstEdgeFromEdgePiece(edge[0],edge[1]);
			if(YSTRUE==edCeHd.IsIncluded(ceHd))
			{
				return YSTRUE;
			}
			return YSFALSE;
		}
	};

protected:
	const CanPassFunction *canPassEdge;

public:
	using YsShellCrawler::SetMode;
	using YsShellCrawler::DeadEnd;
	using YsShellCrawler::ReachedGoal;
	using YsShellCrawler::ReachedNearGoal;
	using YsShellCrawler::SetGoal;
	using YsShellCrawler::State;
	using YsShellCrawler::GetCurrentState;

	YsShellExt_EdgeCrawler();

	/*! Create a crawling state that can be given to Restart function to continue crawling.
	    Since the initial direction is not given to this function, member variable dir must be specified before
	    the returned state is given to Restart function.
	*/
	static State Create(const YsShellExt &shl,YsShellVertexHandle startVtHd);

	/*! Sets the can-pass function.  
	    Can-Pass function is a requirement.  It must be set before using the edge crawler.
	    Can-Pass function must be set before Start.
	*/
	void SetCanPassFunc(const CanPassFunction *func);

	/*! Start crawling. */
	YSRESULT Start(const YsShellExt &shl,const YsVec3 &startDir,YsShellVertexHandle startVtHd);

	/*! Restarts crawling from the given state.  The state must be ON_VERTEX or ON_EDGE. Otherwise, it returns YSERR.
	*/
	YSRESULT Restart(const YsShellExt &shl,State sta);

	/*! Moves the crawler by the given distance.  If the given distance is zero, it moves the crawler to the next polygon.
	    This function returns YSERR when an error prevents the crawler from moving, unless otherwise it returns YSOK.
	    The crawler may hit the closest position to the goal, or hit the non-manifold edge, or hit a polygon that can-pass function returns YSFALSE,
	    and then stops.  In that situation, this function still returns YSOK, but DeadEnd, ReachedGoal, or ReachedNearGoal function may return YSTRUE.
	*/
	YSRESULT Crawl(const YsShellExt &shl,const double &dist,YSBOOL watch=YSFALSE);

private:
	YsArray <YsShell::VertexHandle,2> PossibleNextVertex(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd) const;
	YsArray <YsShell::VertexHandle,2> PossibleNextVertexForDirection(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd,const YsVec3 &dir) const;
	YsArray <YsShell::VertexHandle,2> PossibleNextVertexTowardGoal(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd,const YsVec3 &goal) const;
};

/* } */
#endif
