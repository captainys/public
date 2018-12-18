#include <ysshellext.h>
#include <ysshellextio.h>
#include <ysshellext_primitiveshapeutil.h>

void HighlightIntersection(YsShellExt &inOutShl,const YsShellExt &toolShl)
{
    YsShellLattice ltc;
    ltc.SetDomain(inOutShl.Conv(),1+inOutShl.GetNumPolygon());
    for(auto toolPlHd : toolShl.AllPolygon())
    {
        auto toolPlVtPos=toolShl.GetPolygonVertexPosition(toolPlHd);
        auto itscCheckCandidate=
            ltc.GetPolygonIntersectionCandidate(toolPlVtPos.size(),toolPlVtPos.data());
        for(auto checkPlHd : itscCheckCandidate)
        {
            auto checkPlVtPos=inOutShl.GetPolygonVertexPosition(checkPlHd);

            YsCollisionOfPolygon coll;
            coll.SetPolygon1(toolPlVtPos.size(),toolPlVtPos.data());
            coll.SetPolygon2(checkPlVtPos.size(),checkPlVtPos.data());
            if(YSTRUE==coll.CheckCollision())
            {
                inOutShl.SetPolygonColor(checkPlHd,YsRed());
            }
        }
    }
}

int main(int argc,char *argv[])
{
    if(3<=argc)
    {
        YsShellExt inOutShl;
        if(YSOK!=YsShellExt_LoadGeneral(inOutShl,argv[1]))
        {
            fprintf(stderr,"Failed to read the input.\n");
            return 1;
        }

        YsShellExt toolShl;
        YsShellExt_SphereGeneratorUtil util;
        util.SetUp(YsVec3::Origin(),4.0,16,32);
        util.Generate(toolShl);

        HighlightIntersection(inOutShl,toolShl);

        YsShellExt_SaveGeneral(argv[2],inOutShl);
    }
    return 0;
}
