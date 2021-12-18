#include <iostream>
#include "ysclass.h"



int main(void)
{
	YsShell shl;
	YsArray <YsShell::TexCoordHandle> tcHd;
	YsArray <YsShell::VertexHandle> vtHd;
	const YsVec2 tc[]=
	{
		YsVec2(0.0,0.0),
        YsVec2(1.0,0.0),
        YsVec2(1.0,1.0),
        YsVec2(0.0,1.0),
	};
	const YsVec3 plg[]=
	{
		YsVec3(0.0,0.0,0.0),
        YsVec3(1.0,0.0,0.0),
        YsVec3(1.0,1.0,0.0),
        YsVec3(0.0,1.0,0.0),
	};

	int len=sizeof(tc)/sizeof(tc[0]);

	for(auto T : tc)
	{
		tcHd.push_back(shl.AddTexCoord(T));
	}
	for(auto P : plg)
	{
		vtHd.push_back(shl.AddVertexH(P));
	}

	printf("Adding and finding.\n");
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

	YsShell::VertexHandle plVtHd[2][3]=
	{
		{vtHd[0],vtHd[1],vtHd[2]},
		{vtHd[2],vtHd[3],vtHd[0]},
	};
	YsShell::TexCoordHandle plTcHd[2][3]=
	{
		{tcHd[0],tcHd[1],tcHd[2]},
		{tcHd[2],tcHd[3],tcHd[0]},
	};
	YsShell::PolygonHandle plHd[2]=
	{
		shl.AddPolygonH(3,plVtHd[0]),
		shl.AddPolygonH(3,plVtHd[1]),
	};
	shl.SetPolygonTexCoord(plHd[0],3,plTcHd[0]);
	shl.SetPolygonTexCoord(plHd[1],3,plTcHd[1]);

	printf("Used tex coord undeletable.\n");
	for(auto hd : tcHd)
	{
		if(YSOK==shl.DeleteTexCoord(hd))
		{
			printf("Used texCoord is supposed to be undeletable.\n");
			return 1;
		}
	}

	printf("Iterator.\n");
	{
		int i=0;
		for(auto hd : shl.AllTexCoord())
		{
			if(hd!=tcHd[i])
			{
				printf("TexCoord iterator error.\n");
				return 1;
			}
			++i;
		}
		if(i!=len)
		{
			printf("Length error.\n");
		}
		if(shl.GetNumTexCoord()!=len)
		{
			printf("Length error.\n");
		}
	}

	printf("Tex coord to polygon table.\n");
	YsShellSearchTable search;
	shl.AttachSearchTable(&search);
	for(auto hd : tcHd)
	{
		auto found=search.FindPolygonFromTexCoord(hd);
		if(1!=found.size() && 2!=found.size())
		{
			printf("TexCoord is supposed to be used by one or two polygons.\n");
			return 1;
		}
	}

	printf("Freeze polygon\n");
	shl.FreezePolygon(plHd[0]);
	shl.FreezePolygon(plHd[1]);
	for(auto hd : tcHd)
	{
		auto found=search.FindPolygonFromTexCoord(hd);
		if(0!=found.size())
		{
			printf("TexCoord is supposed to zero.\n");
			return 1;
		}
	}

	for(auto hd : tcHd)
	{
		if(YSOK==shl.DeleteTexCoord(hd))
		{
			printf("Used texCoord (by a frozen polygon) is supposed to be undeletable.\n");
			return 1;
		}
	}


	printf("Melt polygon\n");
	shl.MeltPolygon(plHd[0]);
	shl.MeltPolygon(plHd[1]);
	for(auto hd : tcHd)
	{
		if(YSOK==shl.DeleteTexCoord(hd))
		{
			printf("Used texCoord is supposed to be undeletable.\n");
			return 1;
		}
	}




	printf("Delete tex coord.\n");
	shl.DeletePolygon(plHd[0]);
	shl.DeletePolygon(plHd[1]);
	for(auto hd : tcHd)
	{
		if(YSOK!=shl.DeleteTexCoord(hd))
		{
			printf("TexCoord is supposed to be deletable.\n");
			return 1;
		}
	}


	printf("TexCoord test done.\n");

	return 0;
}
