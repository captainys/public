#include <ysshellext.h>
#include <ysshellextio.h>
#include <ysshellext_triangulationutil.h>

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

		YsShellExt_TriangulationInfo util;
		util.MakeInfo(shl);
		util.Commit(shl);

        YsShellExt_SaveGeneral(argv[2],shl);
    }
    return 0;
}
