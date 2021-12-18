#include <iostream>
#include "ysclass.h"



int main(void)
{
	YsShell shl;
	YsArray <YsShell::TexCoordHandle> tcHd;
	const YsVec2 tc[]=
	{
		YsVec2(0.0,0.0),
        YsVec2(1.0,0.0),
        YsVec2(1.0,1.0),
        YsVec2(0.0,1.0),
	};

	int len=sizeof(tc)/sizeof(tc[0]);

	for(auto T : tc)
	{
		tcHd.push_back(shl.AddTexCoord(T));
	}

	for(int i=0; i<len; ++i)
	{
		auto uv=shl.GetTexCoordUV(tcHd[i]);
		if(uv!=tc[i])
		{
			printf("Coord error! %d\n",i);
			return 1;
		}

		auto verifyTcHd=shl.FindTexCoord(shl.GetSearchKey(tcHd[i]));
		if(verifyTcHd!=tcHd[i])
		{
			printf("TexCoord search error! %d\n",i);
			return 1;
		}
	}

	return 0;
}
