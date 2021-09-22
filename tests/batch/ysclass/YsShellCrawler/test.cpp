/* ////////////////////////////////////////////////////////////

File Name: test.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include <ysclass.h>
#include <ysport.h>

YSRESULT TestFromPolygon(const YsShell &shl)
{
	YSRESULT res=YSOK;

	printf("%s %d\n",__FUNCTION__,__LINE__);

	auto plHd=shl.GetPolygonHandleFromId(10);
	if(nullptr==plHd)
	{
		fprintf(stderr,"Cannot get a polygon!\n");
		return YSERR;
	}

	const YsVec3 startPos(-5.0000000000, -3.5575352147, 0.0000000000);

	YsShellCrawler crawler;
	if(YSOK!=crawler.Start(shl,startPos,-YsYVec(),plHd))
	{
		fprintf(stderr,"Cannot start crawling!\n");
		return YSERR;
	}

	for(int i=0; i<10; ++i)
	{
		if(YSOK!=crawler.Crawl(shl,0.0))
		{
			fprintf(stderr,"Failed to crawl!\n");
			return YSERR;
		}

		if(YSTRUE==crawler.DeadEnd())
		{
			break;
		}
	}

	if(YSTRUE!=crawler.DeadEnd())
	{
		fprintf(stderr,"Crawler is supposed to hit a non-manifold edge!\n");
		return YSERR;
	}

	auto sta=crawler.GetCurrentState();
	if(YsVec3(2.3926750322, 5.0000000000, 0.0000000000)!=sta.pos ||
	   YsShellCrawler::STATE_ON_EDGE!=sta.state)
	{
		fprintf(stderr,"Last state is not right!\n");
		return YSERR;
	}

	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

class TestCanPassFunc : public YsShellCrawler::CanPassFunction
{
public:
	virtual YSBOOL CanPassPolygon(const YsShell &shl,const YsShellPolygonHandle plHd) const;
};

/* virtual */ YSBOOL TestCanPassFunc::CanPassPolygon(const YsShell &shl,const YsShellPolygonHandle plHd) const
{
	auto plId=shl.GetPolygonIdFromHandle(plHd);

	if(10==plId || 0==plId || 1==plId || 3==plId || 9==plId || 5==plId)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSRESULT TestFromPolygonWithCanPassFunc(const YsShell &shl)
{
	YSRESULT res=YSOK;

	printf("%s %d\n",__FUNCTION__,__LINE__);

	auto plHd=shl.GetPolygonHandleFromId(10);
	if(nullptr==plHd)
	{
		fprintf(stderr,"Cannot get a polygon!\n");
		return YSERR;
	}

	const YsVec3 startPos(-5.0000000000, -3.5575352147, 0.0000000000);

	YsShellCrawler crawler;
	TestCanPassFunc canPassFunc;
	crawler.SetCanPassFunc(&canPassFunc);

	if(YSOK!=crawler.Start(shl,startPos,-YsYVec(),plHd))
	{
		fprintf(stderr,"Cannot start crawling!\n");
		return YSERR;
	}

	for(int i=0; i<10; ++i)
	{
		if(YSOK!=crawler.Crawl(shl,0.0))
		{
			fprintf(stderr,"Failed to crawl!\n");
			return YSERR;
		}

		if(YSTRUE==crawler.DeadEnd())
		{
			break;
		}
	}

	if(YSTRUE!=crawler.DeadEnd())
	{
		fprintf(stderr,"Crawler is supposed to hit a non-manifold edge!\n");
		return YSERR;
	}

	auto sta=crawler.GetCurrentState();
	if(YsVec3(-9.8438920571, 1.0498776678, 0.0000000000)!=sta.pos ||
	   YsShellCrawler::STATE_ON_EDGE!=sta.state)
	{
		fprintf(stderr,"Last state is not right!\n");
		fprintf(stderr,"Pos=%s\n",sta.pos.Txt());
		return YSERR;
	}

	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}


YSRESULT TestPointToPoint(const YsShell &shl,const YsShellLattice &ltc,YsVec3 from,YsVec3 to)
{
	printf("Point to Point\n");

	YsShellCrawler crawler;
	crawler.SetMode(YsShellCrawler::CRAWL_A_TO_B);
	crawler.SetGoal(to);

	double dist;
	YsVec3 nearp;
	YsShellPolygonHandle nearPlHd=ltc.FindNearestPolygon(dist,nearp,from);
	if(nullptr!=nearPlHd)
	{
		if(YSOK!=crawler.Start(shl,from,YsVec3::Origin(),nearPlHd))
		{
			fprintf(stderr,"Cannot start.\n");
			return YSERR;
		}
	}
	else
	{
		return YSERR;
	}

	while(YSOK==crawler.Crawl(shl,0.0))
	{
		if(YSTRUE==crawler.ReachedNearGoal())
		{
			printf("Near Goal.\n");
			printf("%s\n",crawler.currentPos.Txt());
			break;
		}
		if(YSTRUE==crawler.DeadEnd())
		{
			printf("Dead End.\n");
			printf("%s\n",crawler.currentPos.Txt());
			break;
		}
		if(YSTRUE==crawler.ReachedGoal())
		{
			printf("Goal.\n");
			printf("%s\n",crawler.currentPos.Txt());
			break;
		}
	}

	return YSOK;
}

YSRESULT TestPointToPoint(const YsShell &shl)
{
	YsShellLattice ltc;
	ltc.SetDomain(shl,shl.GetNumPolygon()+1);

	if(YSOK!=TestPointToPoint(shl,ltc,YsVec3(2.6516504294, 3.9016504294, 0.9567085809),YsVec3(3.9429025374, 0.6764951252, -2.7244755339)))
	{
		return YSERR;
	}
	return YSOK;
}


int main(void)
{
	YsFileIO::ChDir(YsSpecialPath::GetProgramBaseDirW());

	YsShell shl;
	if(YSOK!=shl.LoadSrf("crawlertest.srf"))
	{
		fprintf(stderr,"Cannot load crawlertest.srf!\n");
		return 1;
	}
	shl.Encache();

	YsShellSearchTable search;
	shl.AttachSearchTable(&search);

	int nFail=0;
	if(YSOK!=TestFromPolygon(shl))
	{
		++nFail;
	}
	if(YSOK!=TestFromPolygonWithCanPassFunc(shl))
	{
		++nFail;
	}



	YsShell shl2;
	if(YSOK!=shl2.LoadSrf("crawlertest2.srf"))
	{
		fprintf(stderr,"Cannot load crawlertest2.srf!\n");
		return 1;
	}

	shl2.Encache();

	YsShellSearchTable search2;
	shl2.AttachSearchTable(&search2);

	if(YSOK!=TestPointToPoint(shl2))
	{
		++nFail;
	}



	printf("%d failed.\n",nFail);
	if(0<nFail)
	{
		return 1;
	}
	return 0;
}
