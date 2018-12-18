#include <math.h>
#include <ysshellext.h>
#include <ysshellextio.h>
#include <ysshellext_orientationutil.h>

void MakeMesh(YsShellExt &shl)
{
    // YsArray works just like std::vector
    YsArray <YsShell::VertexHandle> vtHdMatrix;
    for(int j=0; j<=100; ++j)
    {
        double y=(double)j/20;
        for(int i=0; i<=100; ++i)
        {
            double x=(double)i/20;
            double z=sin(x*2.0*YsPi/5.0)*cos(y*2.0*YsPi/5.0);
            vtHdMatrix.push_back(shl.AddVertex(YsVec3(x,y,z)));
        }
    }
    for(int j=0; j<100; ++j)
    {
        for(int i=0; i<100; ++i)
        {
            YsShell::VertexHandle quadVtHd[4]=
            {
                vtHdMatrix[ j*101   +i],
                vtHdMatrix[ j*101   +i+1],
                vtHdMatrix[(j+1)*101+i+1],
                vtHdMatrix[(j+1)*101+i],
            };
            auto plHd=shl.AddPolygon(4,quadVtHd);
            shl.SetPolygonColor(plHd,YsBlue());
        }
    }
}

int main(void)
{
    YsShellExt shl;
    MakeMesh(shl);

	YsShellExt_OrientationUtil util;
    util.RecalculateNormalFromCurrentOrientation(shl.Conv());
    util.Commit(shl);

    YsShellExt_SaveGeneral("sample.srf",shl);
    return 0;
}
