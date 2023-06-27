#include "ysadvgeometry.h"
#include "yspositivearea.h"

YsAreaSlasher::YsAreaSlasher()
{
}
YsAreaSlasher::~YsAreaSlasher()
{
}

void YsAreaSlasher::SetInitialPolygon(YSSIZE_T np,const YsVec2 p[])
{
	plg.Set(np,p);
}

void YsAreaSlasher::SetInitialPolygon(const YsConstArrayMask <YsVec2> &p)
{
	plg=p;
}

void YsAreaSlasher::InsertVertexOnEdge(const YsVec2 o,const YsVec2 v)
{
	for(YSSIZE_T idx=0; idx<plg.size(); ++idx)
	{
		const YsVec2 edVtPos[2]=
		{
			plg[idx],
			plg.GetCyclic(idx+1)
		};

		YsVec2 itsc;
		if(YSOK==YsGetLineIntersection2(itsc,o,o+v,edVtPos[0],edVtPos[1]) &&
		   itsc!=edVtPos[0] &&
		   itsc!=edVtPos[1] &&
		   YSTRUE==YsCheckInBetween2(itsc,edVtPos))
		{
			plg.Insert(idx+1,itsc);
			++idx;
		}
	}
}
void YsAreaSlasher::DeleteNegativeEdge(const YsVec2 o,const YsVec2 n)
{
	for(YSSIZE_T idx=plg.size()-1; 0<=idx; --idx)
	{
		auto d=(plg[idx]-o)*n;
		if(d<-YsTolerance)
		{
			plg.Delete(idx);
		}
	}
}
YSRESULT YsAreaSlasher::SaveSrf(const char fn[]) const
{
	if(0<plg.size())
	{
		FILE *fp=fopen(fn,"w");
		if(nullptr!=fp)
		{
			fprintf(fp,"Surf\n");

			for(auto p : plg)
			{
				fprintf(fp,"V %lf %lf 0\n",p.x(),p.y());
			}

			fprintf(fp,"F\n");
			fprintf(fp,"V");
			for(auto idx : plg.AllIndex())
			{
				fprintf(fp," %d",(int)idx);
			}
			fprintf(fp,"\n");
			fprintf(fp,"E\n");

			fclose(fp);
		}
	}

	return YSERR;
}



////////////////////////////////////////////////////////////



void YsPositiveAreaCalculator::Initialize(const YsVec2 &min,const YsVec2 &max)
{
	const YsVec2 plg[4]=
	{
		min,
		YsVec2(max.x(),min.y()),
		max,
		YsVec2(min.x(),max.y()),
	};
	SetInitialPolygon(4,plg);
}

void YsPositiveAreaCalculator::AddLine(const YsVec2 o,const YsVec2 n)
{
	const YsVec2 v(-n.y(),n.x());
	InsertVertexOnEdge(o,v);
	DeleteNegativeEdge(o,n);
}

const YsConstArrayMask <YsVec2> YsPositiveAreaCalculator::GetPolygon(void) const
{
	return plg;
}
