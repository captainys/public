#include "rawread.h"
#include "rawreadutil.h"

int main(int ac,char *av[])
{
	FM7RawDiskReadUtil::CommandParameterInfo cpmi;

	if(true!=cpmi.RecognizeCommandParameter(ac,av))
	{
		printf("Usage:\n");
		printf("  rawRead2D77 -?\n");
		printf("    Show help.\n");
		printf("  rawRead2D77 inputFileName.txt outputFileName.d77 <options>\n");
		printf("    Read from inputFileName.txt and convert to .D77 file.\n");
		printf("  rawRead2D77 inputFileName.txt <options>\n");
		printf("    Read from inputFileName.txt and run specified tasks, but don't write to .D77 file.\n");
		return 0;
	}


	FM7RawDiskReadUtil util;
	return util.RunBatchMode(cpmi);
}
