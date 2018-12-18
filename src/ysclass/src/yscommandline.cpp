#include "yscommandline.h"


template <class STRTYPE> 
static bool NeedDoubleQuote(const STRTYPE &str)
{
	for(auto c : str)
	{
		if(' '==c || '\t'==c)
		{
			return true;
		}
	}
	return false;
}

template <class STRTYPE>
STRTYPE YsAssembleCommandLineTemplate(YsConstArrayMask <STRTYPE> argv)
{
	STRTYPE cmdLine;
	for(auto &s : argv)
	{
		if(0<cmdLine.size())
		{
			cmdLine.push_back(' ');
		}

		if(true==NeedDoubleQuote(s))
		{
			cmdLine.push_back('\"');
			cmdLine+=s;
			cmdLine.push_back('\"');
		}
		else
		{
			cmdLine+=s;
		}
	}
	return cmdLine;
}

YsString YsAssembleCommandLine(YsConstArrayMask <YsString> argv)
{
	return YsAssembleCommandLineTemplate<YsString>(argv);
}

YsWString YsAssembleCommandLine(YsConstArrayMask <YsWString> argv)
{
	return YsAssembleCommandLineTemplate<YsWString>(argv);
}
