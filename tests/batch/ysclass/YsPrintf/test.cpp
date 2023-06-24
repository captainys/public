#include <ysdef.h>
#include <ysprintf.h>
#include <stdio.h>

//placeholder test (want to make sure this compiles and doesn't throw any runtime errors)
YSRESULT YsPrintf_TestPrintf(void)
{
	YsPrintf("%d\n", 10);

	return YSOK;
}

int main(void)
{
	int nFail = 0;
	if (YSOK != YsPrintf_TestPrintf())
	{
		++nFail;
	}

	printf("%d failed.\n", nFail);
	if (0 < nFail)
	{
		return 1;
	}
	return 0;
}