#include "ysshellext_itscreduction.h"



YsArray <YsShell::Edge> YsShellExt_IntersectionReducingEdgeSwapping::GetIntersectingEdge(const YsShellExt &shl,const YsShellExt &toolShl,const YsShellLattice &toolShlLattice,const Option &opt) const
{
	YsArray <YsShell::Edge> itscEdge;
	YsShellEdgeEnumHandle edHd=nullptr;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		YsArray <YsVec3,16> itsc;
		YsArray <YsShellPolygonHandle,16> itscPlHd;
		auto edge=shl.GetEdge(edHd);
		if(YSOK==toolShlLattice.ShootFiniteRay(itsc,itscPlHd,shl.GetVertexPosition(edge[0]),shl.GetVertexPosition(edge[1])) &&
		   0<itsc.size())
		{
			itscEdge.push_back(edge);
		}
	}
	return itscEdge;
}
YSBOOL YsShellExt_IntersectionReducingEdgeSwapping::DoesReduceIntersection(const YsShellExt &shl,const YsShellExt &toolShl,const YsShellLattice &toolShlLattice,const Option &opt,const YsShell_SwapInfo &swapInfo) const
{
	double diagon[2],border[2][4];
	swapInfo.CalcualteDihedralAngleChange(shl,diagon,border);
	if(diagon[0]<opt.dhaThr)
	{
		YsArray <YsVec3,16> itsc;
		YsArray <YsShellPolygonHandle,16> itscPlHd;
		if(YSOK==toolShlLattice.ShootFiniteRay(itsc,itscPlHd,shl.GetVertexPosition(swapInfo.newDiagonal[0]),shl.GetVertexPosition(swapInfo.newDiagonal[1])))
		{
			if(0==itsc.size())
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}
