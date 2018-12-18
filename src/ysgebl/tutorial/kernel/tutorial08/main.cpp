#include <ysshellext.h>
#include <ysshellextio.h>
#include <ysshellext_trackingutil.h>



void LaplacianSmoothing(YsShellExt &shl)
{
    for(auto vtHd : shl.AllVertex())
    {
        auto avg=YsVec3::Origin();
        YSSIZE_T nSample=0;
        for(auto connVtHd : shl.GetConnectedVertex(vtHd))
        {
            avg+=shl.GetVertexPosition(connVtHd);
            ++nSample;
        }
        if(0<nSample)
        {
            avg/=(double)nSample;
            shl.SetVertexPosition(vtHd,avg);
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

        shl.EnableSearch();
        for(int i=0; i<100; ++i)
        {
            LaplacianSmoothing(shl);
        }

        YsShellExt_SaveGeneral(argv[2],shl);
    }
    return 0;
}

