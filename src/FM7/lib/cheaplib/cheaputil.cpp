#include <string.h>

#include "cheaputil.h"



std::vector <std::string> CheapParser(const char str[])
{
	int state=0;
	std::string curStr;
	std::vector <std::string> argv;
	for(int i=0; 0!=str[i]; ++i)
	{
		if(0==state)
		{
			if(' '!=str[i] && '\t'!=str[i])
			{
				curStr.push_back(str[i]);
				state=1;
			}
		}
		else if(1==state)
		{
			if(' '==str[i] || '\t'==str[i] || 0==str[i+1])
			{
				if(' '!=str[i] && '\t'!=str[i])
				{
					curStr.push_back(str[i]);
				}
				argv.push_back((std::string &&)curStr);
				state=0;
			}
			else
			{
				curStr.push_back(str[i]);
			}
		}
	}

	return argv;
}

const char *CheapFGets(std::string &str,FILE *fp)
{
	str.clear();
	bool alreadyEof=true;

	const int L=16;
	char buf[L];
	while(nullptr!=fgets(buf,L-1,fp))
	{
		alreadyEof=false;
		auto l=strlen(buf);
		if(buf[l-1]=='\n' || buf[l-1]=='\r')
		{
			buf[l-1]=0;
			str.append(buf);
			break;
		}
		else
		{
			str.append(buf);
		}
	}

	if(true==alreadyEof)
	{
		return nullptr;
	}
	else
	{
		return str.c_str();
	}
}

