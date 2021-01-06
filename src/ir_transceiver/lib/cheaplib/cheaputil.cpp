#include <string.h>

#include "cheaputil.h"



std::vector <std::string> CheapParser(const char str[])
{
	int state=0;
	std::vector <std::string> argv;
	for(int i=0; 0!=str[i]; ++i)
	{
		if(0==state) // Nowhere
		{
			if('\"'==str[i])
			{
				std::string s;
				argv.push_back(s);
				state=2;
			}
			else if(' '!=str[i] && '\t'!=str[i])
			{
				std::string s;
				s.push_back(str[i]);
				argv.push_back(s);
				state=1;
			}
		}
		else if(1==state) // In a word
		{
			if(' '==str[i] || '\t'==str[i] || 0==str[i+1])
			{
				state=0;
			}
			else if('\"'==str[i])
			{
				std::string s;
				argv.push_back(s);
				state=2;
			}
			else
			{
				argv.back().push_back(str[i]);
			}
		}
		else if(2==state) // In a string
		{
			if('\"'==str[i])
			{
				state=0;
			}
			else
			{
				argv.back().push_back(str[i]);
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

