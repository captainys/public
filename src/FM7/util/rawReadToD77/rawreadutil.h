#ifndef RAWREADUTIL_IS_INCLUDED
#define RAWREADUTIL_IS_INCLUDED
/* { */

#include "rawread.h"

class FM7RawDiskReadUtil
{
public:
	class CommandParameterInfo
	{
	public:
		std::string inFName,d77OutFName;
		bool verifyRawRead;
		bool makeD77;

		CommandParameterInfo();
		void CleanUp(void);
		bool RecognizeCommandParameter(int ac,char *av[]);
		static void ShowHelp(void);
	};

private:
	std::vector <FM7RawDiskRead> rawDisk;

public:
	FM7RawDiskReadUtil();
	void CleanUp(void);
	int RunBatchMode(const CommandParameterInfo &cpmi);

	bool Load(const char fn[]);
	bool SaveAsD77(const char fn[]) const;
};

/* } */
#endif
