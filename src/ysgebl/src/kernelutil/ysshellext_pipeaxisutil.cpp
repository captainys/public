#include "ysshellext_pipeaxisutil.h"
#include "ysshellext_trackingutil.h"
#include "ysshellext_boundaryutil.h"



void YsShellExt_PipeAxisUtil::CleanUp(void)
{
	path.clear();
	reachedEnd=YSFALSE;
}
void YsShellExt_PipeAxisUtil::Begin(const YsShellExt &shl,YsConstArrayMask <YsShell::PolygonHandle> startPlg,YsConstArrayMask <YsShell::PolygonHandle> endPlg)
{
	CleanUp();
	ltc.SetDomain(shl.Conv(),shl.GetNumPolygon()+1);
	this->startPlg=startPlg;
	this->endPlg=endPlg;
	CalculateInitialPosition(shl,startPlg);
}
void YsShellExt_PipeAxisUtil::CalculateInitialPosition(const YsShellExt &shl,YsConstArrayMask <YsShell::PolygonHandle> startPlg)
{
	auto cen=GetContourCenter(shl,startPlg);

	YsVec3 nom=YsVec3::Origin();
	for(auto plHd : startPlg)
	{
		nom+=shl.GetNormal(plHd);
	}
	nom.Normalize();

	reachedEnd=YSFALSE;

	Photon current;
	current.pos=cen;
	current.vec=-nom; // Normal should point outward.
	path.push_back(current);
}
YsVec3 YsShellExt_PipeAxisUtil::GetContourCenter(const YsShellExt &shl,YsConstArrayMask <YsShell::PolygonHandle> plg) const
{
	YsShellExt_BoundaryInfo boundaryInfo;
	boundaryInfo.MakeInfo(shl.Conv(),plg.size(),plg.data());
	boundaryInfo.CacheContour(shl.Conv());

	int nSample=0;
	YsVec3 cen=YsVec3::Origin();
	for(int i=0; i<boundaryInfo.GetNumContour(); ++i)
	{
		auto contour=boundaryInfo.GetContour(i);
		auto N=contour.size();
		if(0<N)
		{
			if(contour.front()==contour.back())
			{
				--N;
			}
			for(int j=0; j<N; ++j)
			{
				cen+=shl.GetVertexPosition(contour[j]);
			}
			nSample+=N;
		}
	}
	if(0<nSample)
	{
		cen/=(double)nSample;
	}
	return cen;
}
YSRESULT YsShellExt_PipeAxisUtil::ShootOneStep(const YsShellExt &shl)
{
	if(YSTRUE==reachedEnd)
	{
		return YSOK;
	}

	YsShell::PolygonHandle iPlHd=nullptr;
	YsVec3 itscPos;
	YSRESULT res;
	if(1==path.size())
	{
		res=ltc.FindFirstIntersection(iPlHd,itscPos,path.back().pos,path.back().vec,startPlg.size(),startPlg.data());
	}
	else
	{
		res=ltc.FindFirstIntersection(iPlHd,itscPos,path.back().pos,path.back().vec);
	}
	if(YSOK!=res || nullptr==iPlHd)
	{
		return YSERR;
	}

	if(YSTRUE==endPlg.IsIncluded(iPlHd))
	{
		reachedEnd=YSTRUE;

		Photon p;
		p.pos=GetContourCenter(shl,endPlg);
		p.vec=YsVec3::Origin();
		path.push_back(p);

		return YSOK;
	}

	double segmentRadius=EstimateRadius(shl,(itscPos+path.back().pos)/2.0,path.back().vec);
	YsVec3 wallNom=shl.GetNormal(iPlHd);
	// Pull back by the estimated radius.
	YsPlane offsetPln;
	offsetPln.Set(itscPos-wallNom*segmentRadius,wallNom);
	YsVec3 offsetItsc;
	if(YSOK==offsetPln.GetIntersection(offsetItsc,path.back().pos,path.back().vec))
	{
		auto side=ltc.CheckInsideSolid(offsetItsc);
		if(YSINSIDE!=side)
		{
			return YSERR;
		}

		auto searchDir=ShootRadial(shl,offsetItsc,wallNom);
		int longestIdx=-1;
		double longest=0.0;
		for(int i=0; i<searchDir.size(); ++i)
		{
			double d=(searchDir[i]-offsetItsc).GetLength();
			if(longest<d)
			{
				longestIdx=i;
				longest=d;
			}
		}

		if(0<=longestIdx)
		{
			YsVec3 v1=searchDir[longestIdx]-offsetItsc,v2;
			double dPrev=(searchDir.GetCyclic(longestIdx-1)-offsetItsc).GetSquareLength();
			double dNext=(searchDir.GetCyclic(longestIdx+1)-offsetItsc).GetSquareLength();
			if(dPrev<dNext)
			{
				v2=searchDir.GetCyclic(longestIdx+1)-offsetItsc;
			}
			else
			{
				v2=searchDir.GetCyclic(longestIdx-1)-offsetItsc;
			}

			double d1=v1.GetLength();
			double d2=v2.GetLength();
			v1/=d1;
			v2/=d2;
			YsMakeGreater(longest,d1);
			YsMakeGreater(longest,d2);
			for(int iter=0; iter<nBisection; ++iter)
			{
				auto vMid=(v1+v2)/2.0;
				vMid.Normalize();
				YsShell::PolygonHandle iPlHd;
				YsVec3 itscPos;
				if(YSOK!=ltc.FindFirstIntersection(iPlHd,itscPos,offsetItsc,vMid) ||
				   nullptr==iPlHd)
				{
					return YSERR;
				}
				auto dMid=(itscPos-offsetItsc).GetLength();
				YsMakeGreater(longest,dMid);
				if(d1>d2)
				{
					d2=dMid;
					v2=vMid;
				}
				else
				{
					d1=dMid;
					v1=vMid;
				}
			}


			// Tentatively make up a next pos.
			Photon nextPos;
			nextPos.pos=offsetItsc;
			nextPos.vec=(v1+v2)/2.0;

			// If the pipe is really thin, more adjustment is needed.
			// (1) Try to find a vector that is parallel to the walls
			{
				YsShell::PolygonHandle iPlHd;
				YsVec3 itscPos;
				if(YSOK==ltc.FindFirstIntersection(iPlHd,itscPos,offsetItsc,nextPos.vec) &&
				   nullptr!=iPlHd)
				{
					YsVec3 mid=(itscPos+offsetItsc)/2.0;
					auto radialNom=ShootRadialGetNormal(shl,mid,nextPos.vec);
					YsVec3 vNew;
					if(YSOK==YsFindLeastSquareFittingPlaneNormal(vNew,radialNom.size(),radialNom.data()))
					{
						if(vNew*nextPos.vec<0.0)
						{
							vNew=-vNew;
						}
						if(YSOK==ltc.FindFirstIntersection(iPlHd,itscPos,offsetItsc,vNew) &&
						   nullptr!=iPlHd)
						{
							auto dist=(itscPos-offsetItsc).GetSquareLength();
							if(longest<dist)
							{
printf("%s %d\n",__FUNCTION__,__LINE__);
								longest=dist;
								nextPos.vec=vNew;
							}
						}
					}
				}
			}

			// (2) Push it
			for(;;)
			{
				bool moved=false;
				YsShell::PolygonHandle iPlHd;
				YsVec3 itscPos;
				if(YSOK==ltc.FindFirstIntersection(iPlHd,itscPos,offsetItsc,nextPos.vec) &&
				   nullptr!=iPlHd)
				{
					YsVec3 mid=(itscPos+offsetItsc)/2.0;
					auto radial=ShootRadial(shl,mid,nextPos.vec);
					auto cen=YsGetCenter(radial.size(),radial.data());
					auto vNew=cen-offsetItsc;
					if(YSOK==vNew.Normalize() &&
					   YSOK==ltc.FindFirstIntersection(iPlHd,itscPos,offsetItsc,vNew) &&
					   nullptr!=iPlHd)
					{
						auto dist=(itscPos-offsetItsc).GetSquareLength();
						if(longest<dist)
						{
							longest=dist;
							nextPos.vec=vNew;
							moved=true;
						}
					}
				}
				if(true!=moved)
				{
					break;
				}
			}

			path.push_back(nextPos);
			return YSOK;
		}
	}
	return YSERR;
}
double YsShellExt_PipeAxisUtil::EstimateRadius(const YsShellExt &shl,const YsVec3 &pos,const YsVec3 &vec) const
{
	auto radialItsc=ShootRadial(shl,pos,vec);
	auto cen=YsGetCenter(radialItsc.size(),radialItsc.data());
	double avg=0.0;
	for(auto itsc : radialItsc)
	{
		avg+=(itsc-cen).GetLength();
	}
	if(0<radialItsc.size())
	{
		avg/=(double)radialItsc.size();
	}
	return avg;
}
YsArray <YsVec3> YsShellExt_PipeAxisUtil::ShootRadial(const YsShellExt &shl,const YsVec3 &pos,const YsVec3 &vec) const
{
	auto U=vec.GetArbitraryPerpendicularVector();
	U.Normalize();
	auto V=vec^U;
	V.Normalize();

	YsArray <YsVec3> radialItsc;
	for(int i=0; i<nSearchDir; ++i)
	{
		double a=(double)i*YsPi*2.0/(double)nSearchDir;
		auto ray=U*cos(a)+V*sin(a);

		YsShell::PolygonHandle iPlHd;
		YsVec3 itscPos;
		if(YSOK==ltc.FindFirstIntersection(iPlHd,itscPos,pos,ray) && nullptr!=iPlHd)
		{
			radialItsc.push_back(itscPos);
		}
	}

	return radialItsc;
}
YsArray <YsVec3> YsShellExt_PipeAxisUtil::ShootRadialGetNormal(const YsShellExt &shl,const YsVec3 &pos,const YsVec3 &vec) const
{
	auto U=vec.GetArbitraryPerpendicularVector();
	U.Normalize();
	auto V=vec^U;
	V.Normalize();

	YsArray <YsVec3> radialItsc;
	for(int i=0; i<nSearchDir; ++i)
	{
		double a=(double)i*YsPi*2.0/(double)nSearchDir;
		auto ray=U*cos(a)+V*sin(a);

		YsShell::PolygonHandle iPlHd;
		YsVec3 itscPos;
		if(YSOK==ltc.FindFirstIntersection(iPlHd,itscPos,pos,ray) && nullptr!=iPlHd)
		{
			radialItsc.push_back(shl.GetNormal(iPlHd));
		}
	}

	return radialItsc;
}
