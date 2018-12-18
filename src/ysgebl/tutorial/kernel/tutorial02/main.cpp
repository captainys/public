#include <ysshellext.h>
#include <ysshellextio.h>

void ResampleMesh(YsShellExt &outShl,const YsShellExt &inShl)
{
    YsShellLattice ltc;
    ltc.SetDomain(inShl.Conv(),1+inShl.GetNumPolygon());
 
	YsArray <YsShell::VertexHandle> vtHdMatrix;
    for(int j=0; j<=20; ++j)
    {
        printf("%d\n",j);
        double y=5.0*(double)j/20.0;
        for(int i=0; i<=20; ++i)
        {
            double x=5.0*(double)i/20.0;
            YsVec3 from(x,y,10.0);
            YsVec3 dir(0.0,0.0,-1.0);

            YsShell::PolygonHandle itscPlHd;
            YsVec3 itscPos;
            if(YSOK==ltc.FindFirstIntersection(itscPlHd,itscPos,from,dir) && nullptr!=itscPlHd)
            {
                vtHdMatrix.push_back(outShl.AddVertex(itscPos));
            }
            else
            {
                vtHdMatrix.push_back(nullptr);
            }
        }
    }

	for(int j=0; j<20; ++j)
    {
        for(int i=0; i<20; ++i)
        {
            YsShell::VertexHandle quadVtHd[4]=
            {
                vtHdMatrix[ j*21   +i],
                vtHdMatrix[ j*21   +i+1],
                vtHdMatrix[(j+1)*21+i+1],
                vtHdMatrix[(j+1)*21+i],
            };
            if(nullptr!=quadVtHd[0] && nullptr!=quadVtHd[1] &&
               nullptr!=quadVtHd[2] && nullptr!=quadVtHd[3])
            {
                auto plHd=outShl.AddPolygon(4,quadVtHd);
                outShl.SetPolygonColor(plHd,YsGreen());
            }
        }
    }

}

int main(int argc,char *argv[])
{
    if(3<=argc)
    {
        YsShellExt inShl,outShl;
        if(YSOK!=YsShellExt_LoadGeneral(inShl,argv[1]))
        {
            fprintf(stderr,"Failed to read the input.\n");
            return 1;
        }
        ResampleMesh(outShl,inShl);
        YsShellExt_SaveGeneral(argv[2],outShl);
    }
    return 0;
}

