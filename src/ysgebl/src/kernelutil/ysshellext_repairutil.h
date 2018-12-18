#ifndef YSSHELLEXT_REPAIRUTIL_IS_INCLUDED
#define YSSHELLEXT_REPAIRUTIL_IS_INCLUDED
/* { */



#include "ysshellext.h"
#include "ysshellextmisc.h"
#include "ysshellext_localop.h"

class YsShellExt_IdenticalPolygonRemover
{
private:
	YsShellPolygonStore toDel;

public:
	void MakeDuplicatePolygonList(const YsShellExt &shl,YSBOOL takeReverse=YSTRUE);

	template <class SHLCLASS>
	void DeleteDuplicatePolygon(SHLCLASS &shl)
	{
		typename SHLCLASS::StopIncUndo incUndo(shl);
		for(auto plHd : toDel)
		{
			shl.ForceDeletePolygon(plHd);
		}
	}
};



class YsShellExt_FlatTriangleKiller
{
public:
	enum FLAT_TRIANGLE_CLASSIFICATION
	{
		CANNOT_COMPUTE_NORMAL,
		NARROWER_THAN_TOLERANCE
	};

	enum REPAIR_OPTION
	{
		COLLAPSE_SHORTER_THAN_TOLERANCE,
		SWAPPING_LONGEST_EDGE,
		MERGE_LONGEST_EDGE
	};

	FLAT_TRIANGLE_CLASSIFICATION classifier;
	YsArray <REPAIR_OPTION> repairOption;

	template <class SHLCLASS>
	void Apply(SHLCLASS &shl)
	{
		typename SHLCLASS::StopIncUndo incUndo(shl);
		for(auto opt : repairOption)
		{
			YSSIZE_T prevN=shl.GetNumPolygon()+1;
			for(;;)
			{
				YsArray <YSHASHKEY> toVisit;
				for(auto plHd : shl.AllPolygon())
				{
					if(YSTRUE==IsFlatTriangle(shl.Conv(),plHd))
					{
						toVisit.push_back(shl.GetSearchKey(plHd));
					}
				}
				if(prevN<=toVisit.size())
				{
					break;
				}
				prevN=toVisit.size();
				for(auto plKey : toVisit)
				{
					auto plHd=shl.FindPolygon(plKey);
					if(nullptr==plHd)
					{
						continue;
					}
					switch(opt)
					{
					case COLLAPSE_SHORTER_THAN_TOLERANCE:
						{
							YsShell_CollapseInfo info;
							if(YSTRUE==IsShortestEdgeSafeToCollapse(info,shl.Conv(),plHd))
							{
								info.Apply(shl,YSFALSE);
							}
						}
						break;
					case SWAPPING_LONGEST_EDGE:
						{
							YsShell_SwapInfo info;
							if(YSTRUE==IsLongestEdgeSafeToSwap(info,shl.Conv(),plHd))
							{
								info.Apply(shl);
							}
						}
						break;
					case MERGE_LONGEST_EDGE:
					    {
							YsShell::Edge edge;
							if(YSTRUE==IsLongestEdgeSafeToMerge(edge,shl.Conv(),plHd))
							{
								printf("Not implemented yet.\n");
							}
						}
						break;
					}
				}
			}
		}
	}

private:
	YSBOOL IsFlatTriangle(const YsShellExt &shl,YsShell::PolygonHandle plHd) const;

	// Returns YSTRUE if the shortest edge is shorter than tolerance and will not create new non-manifold edge.
	YSBOOL IsShortestEdgeSafeToCollapse(YsShell_CollapseInfo &info,const YsShellExt &shl,YsShell::PolygonHandle plHd) const;

	YSBOOL IsLongestEdgeSafeToSwap(YsShell_SwapInfo &info,const YsShellExt &shl,YsShell::PolygonHandle plHd) const;
	YSBOOL IsLongestEdgeSafeToMerge(YsShell::Edge &edge,const YsShellExt &shl,YsShell::PolygonHandle plHd) const;
};


/* } */
#endif
