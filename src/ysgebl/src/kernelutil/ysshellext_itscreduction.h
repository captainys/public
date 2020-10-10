#ifndef YSSHELLEXT_ITSCREDUCTION_IS_INCLUDED
#define YSSHELLEXT_ITSCREDUCTION_IS_INCLUDED
/* { */

#include "ysshellext.h"
#include "ysshellext_localop.h"

/* Used for preparation for boolean.
 */
class YsShellExt_IntersectionReducingEdgeSwapping
{
public:
	class Option
	{
	public:
		double dhaThr=YsPi/60.0; // 3 degrees
	};

	YsArray <YsShell::Edge> GetIntersectingEdge(const YsShellExt &shl,const YsShellExt &toolShl,const YsShellLattice &toolShlLattice,const Option &opt) const;

	/*! Returns YSTRUE if the new diagonal does not intersect with the tool shell.
	    It also checks conditions described in opt.
	*/
	YSBOOL DoesReduceIntersection(const YsShellExt &shl,const YsShellExt &toolShl,const YsShellLattice &toolShlLattice,const Option &opt,const YsShell_SwapInfo &swapInfo) const;

	template <class SHLCLASS>
	int ApplySwapping(SHLCLASS &shl,const YsShellExt &toolShl,const YsShellLattice &toolShlLattice,const Option &opt)
	{
		int n=0;
		for(auto edge : GetIntersectingEdge(shl.Conv(),toolShl,toolShlLattice,opt))
		{
			YsShell_SwapInfo swapInfo;
			swapInfo.MakeInfo(shl.Conv(),edge);
			if(YSOK==swapInfo.ConvexityCheck(shl.Conv()) &&
			   YSOK==swapInfo.SelfIntersectionCheck(shl.Conv()) &&
			   YSOK==swapInfo.NewDiagonalIsInside(shl.Conv()) &&
			   YSOK==swapInfo.GeometryCheck(shl.Conv()))
			{
				if(YSTRUE==DoesReduceIntersection(shl.Conv(),toolShl,toolShlLattice,opt,swapInfo))
				{
					swapInfo.Apply(shl);
					++n;
				}
			}
		}
		return n;
	}

	// Visual C++ 2019 still fails to start a thread if I pass a reference.
	// All needs to be a pointer.
	template <class SHLCLASS>
	void ApplySwappingThread(SHLCLASS *shlPtr,const YsShellExt *toolShlPtr,const Option *optPtr)
	{
		auto &shl=*shlPtr;
		auto &toolShl=*toolShlPtr;
		auto &opt=*optPtr;
		YsShellLattice toolLtc;
		toolLtc.SetDomain(toolShl.Conv(),toolShl.GetNumPolygon()+1);
		ApplySwapping(shl,toolShl,toolLtc,opt);
	}
};

/* } */
#endif
