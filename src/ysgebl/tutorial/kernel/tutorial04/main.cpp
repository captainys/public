#include <ysshellext.h>
#include <ysshellextio.h>

void HighlightIntersection(YsShellExt &shl)
{
    YsShellLattice ltc;
    ltc.SetDomain(shl.Conv(),1+shl.GetNumPolygon());
	for(auto toolPlHd : shl.AllPolygon())
    {
		shl.SetPolygonColor(toolPlHd,YsWhite());
	}
	for(auto toolPlHd : shl.AllPolygon())
    {
		auto toolPlVtHd=shl.GetPolygonVertex(toolPlHd);
        auto toolPlVtPos=shl.GetPolygonVertexPosition(toolPlHd);
        auto itscCheckCandidate=
            ltc.GetPolygonIntersectionCandidate(toolPlVtPos.size(),toolPlVtPos.data());
        for(auto checkPlHd : itscCheckCandidate)
        {
			auto checkPlVtHd=shl.GetPolygonVertex(checkPlHd);
			if(YSTRUE==toolPlVtHd.HasCommonItem(checkPlVtHd))
			{
				continue;
			}

            auto checkPlVtPos=shl.GetPolygonVertexPosition(checkPlHd);
            YsCollisionOfPolygon coll;
            coll.SetPolygon1(toolPlVtPos.size(),toolPlVtPos.data());
            coll.SetPolygon2(checkPlVtPos.size(),checkPlVtPos.data());
            if(YSTRUE==coll.CheckCollision())
            {
                shl.SetPolygonColor(checkPlHd,YsRed());
                shl.SetPolygonColor(toolPlHd,YsRed());
            }
        }
    }
}

int main(int argc,char *argv[])
{
    if(3<=argc)
    {
        YsShellExt shl;
        if(YSOK!=YsShellExt_LoadGeneral(shl,argv[1]))
        {
            fprintf(stderr,"Failed to read the input.\n");
            return 1;
        }
        HighlightIntersection(shl);
        YsShellExt_SaveGeneral(argv[2],shl);
    }
    return 0;
}
