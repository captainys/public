#include <math.h>

#include "ysshellext_primitiveshapeutil.h"



void YsShellExt_SphereGeneratorUtil::SetUp(const YsVec3 &cen,const double rad,int nLat,int nLng)
{
	vtPosTable.resize(nLat+1);

	this->nLat=nLat;
	this->nLng=nLng;

	for(int y=0; y<=nLat; ++y)
	{
		vtPosTable[y].resize(nLng);
		if(y!=0 && y!=nLat)
		{
			double lat=YsPi*(double)y/(double)nLat-YsPi/2.0;
			for(int x=0; x<nLng; ++x)
			{
				double lng=2.0*YsPi*(double)x/(double)nLng;

				double xx= rad*cos(lng)*cos(lat);
				double zz=-rad*sin(lng)*cos(lat);
				double yy= rad*sin(lat);

				YsVec3 v(xx,yy,zz);
				vtPosTable[y][x]=cen+v;
			}
		}
		else
		{
			YsVec3 v;
			if(0==y)
			{
				v.Set(0.0,-rad,0.0);
			}
			else
			{
				v.Set(0.0,rad,0.0);
			}
			for(int x=0; x<nLng; ++x)
			{
				vtPosTable[y][x]=cen+v;
			}
		}
	}
}



const int YsShellExt_BoxGeneratorUtil::boxVtIdx[6][4]=
{
	{0,1,2,3},
	{7,6,5,4},
	{1,0,4,5},
	{2,1,5,6},
	{3,2,6,7},
	{0,3,7,4}
};

void YsShellExt_BoxGeneratorUtil::SetUp(const YsVec3 &p1,const YsVec3 &p2)
{
	YsBoundingBoxMaker <YsVec3> bbx;
	bbx.Add(p1);
	bbx.Add(p2);
	const auto min=bbx.Min();
	const auto max=bbx.Max();

	boxVtPos[0]=min;
	boxVtPos[1].Set(max.x(),min.y(),min.z());
	boxVtPos[2].Set(max.x(),min.y(),max.z());
	boxVtPos[3].Set(min.x(),min.y(),max.z());

	boxVtPos[4].Set(min.x(),max.y(),min.z());
	boxVtPos[5].Set(max.x(),max.y(),min.z());
	boxVtPos[6].Set(max.x(),max.y(),max.z());
	boxVtPos[7].Set(min.x(),max.y(),max.z());
}
