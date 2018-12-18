#include <ysshellext.h>
#include <ysshellextio.h>
#include <ysshellext_trackingutil.h>
void PrintNumVertexNumPolygon(const YsShellExt &shl)
{
	printf("Number of Vertices: %d\n",(int)shl.GetNumVertex());
	printf("Number of Polygons: %d\n",(int)shl.GetNumPolygon());
}
void PrintMinMaxAverageValence(const YsShellExt &shl)
{
	YSSIZE_T min,max,avg=0,nSample=0;
	for(auto vtHd : shl.AllVertex())
	{
		// You can also use GetNumPoygonUsingVertex
		auto vtPlHd=shl.FindPolygonFromVertex(vtHd);
		if(0==nSample)
		{
			min=vtPlHd.size();
			max=vtPlHd.size();
		}
		else
		{
			YsMakeSmaller(min,vtPlHd.size());
			YsMakeGreater(max,vtPlHd.size());
		}
		avg+=vtPlHd.size();
		++nSample;
	}
	if(0<nSample)
	{
		avg/=nSample;
	}
	printf("Valence Min:%lld Max:%lld Avg:%lld\n",min,max,avg);
}
void PrintMeshType(const YsShellExt &shl)
{
	bool isOpen=false,isNonManifold=false;
	YsShellEdgeEnumHandle edHd=nullptr;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		// You can shortcut by GetNumPolygonUsingEdge
		// or you can also directly give edHd to FindPolygonFromEdge
		auto edgePiece=shl.GetEdge(edHd);
		auto edPlHd=shl.FindPolygonFromEdgePiece(edgePiece);
		if(2!=edPlHd.size())
		{
			isNonManifold=true;
		}
		if(1==edPlHd.size())
		{
			isOpen=true;
		}
	}
	// In fact, the following loop also captures an edge used by more than
	// two polygons.
	for(auto vtHd : shl.AllVertex())
	{
		YsShellExt::PassAll cond; // <- Consider all polygons.
		auto vtPlHd=shl.FindPolygonFromVertex(vtHd);
		if(0<vtPlHd.size())
		{
			// The idea is to make a disk or fan around a vertex
			// starting from one of the polygons using the vertex.
			// If the number of polygons in the fan is same as the
			// number of polygons using the vertex, it is a topological
			// disk.  If not, it is a point connection or an edge is 
			// used by more than two polygons.
			auto plgFan=
			   YsShellExt_TrackingUtil::MakePolygonFanAroundVertex(
			      shl,vtHd,vtPlHd[0],cond);
			if(plgFan.size()!=vtPlHd.size())
			{
				isNonManifold=true;
			}
		}
	}
	if(true==isNonManifold)
	{
		printf("Is a non-manifold mesh.\n");
	}
	if(true==isOpen)
	{
		printf("Is an open mesh.\n");
	}
	if(true!=isNonManifold && true!=isOpen)
	{
		printf("Is a closed mesh.\n");
	}
}
void PrintVolume(const YsShellExt &shl)
{
	printf("Volume is inaccurate if it is not a closed manifold mesh.\n");
	printf("Volume: %lf\n",shl.ComputeVolume());
}
void PrintTotalArea(const YsShellExt &shl)
{
	double area=0.0;
	for(auto plHd : shl.AllPolygon())
	{
		area+=shl.GetPolygonArea(plHd);
	}
	printf("Total Area: %lf\n",area);
}
int main(int argc,char *argv[])
{
    if(2<=argc)
    {
        YsShellExt shl;
        if(YSOK!=YsShellExt_LoadGeneral(shl,argv[1]))
        {
            fprintf(stderr,"Failed to read the input.\n");
            return 1;
        }
        shl.EnableSearch();
		PrintNumVertexNumPolygon(shl);
		PrintMinMaxAverageValence(shl);
		PrintMeshType(shl);
		PrintVolume(shl);
		PrintTotalArea(shl);
    }
    return 0;
}
