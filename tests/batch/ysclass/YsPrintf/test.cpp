#include <ysdef.h>
#include <ysprintf.h>
#include <stdio.h>

//placeholder test (want to make sure this compiles and doesn't throw any runtime errors)
YSRESULT Test(void)
{
	YsPrintf("%d\n", 10);

	return YSOK;
}

int main(void)
{
	int nFail = 0;
	if (YSOK != Test())
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