#include <ysdef.h>
#include <ysfileio.h>

//test scenario: test Fopen with "r" permissions
YSRESULT YsFileIO_Fopen_Test(char mode[])
{
	YsWString str;
	str.SetUTF8String("filename.txt");
	FILE* file = YsFileIO::Fopen(str, mode);

	if (file == NULL)
	{
		return YSERR;
	}
	return YSOK;
}

int main(void)
{
	int nFail = 0;
	if (YSOK != YsFileIO_Fopen_Test("r"))
	{
		++nFail;
	}

	if (YSOK != YsFileIO_Fopen_Test("w"))
	{
		++nFail;
	}

	if (YSOK != YsFileIO_Fopen_Test("a"))
	{
		++nFail;
	}

	if (YSOK != YsFileIO_Fopen_Test("r+"))
	{
		++nFail;
	}

	if (YSOK != YsFileIO_Fopen_Test("w+"))
	{
		++nFail;
	}

	if (YSOK != YsFileIO_Fopen_Test("a+"))
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