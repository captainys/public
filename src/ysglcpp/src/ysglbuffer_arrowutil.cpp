#include "ysglbuffer_arrowutil.h"



YSRESULT YsGLCreateArrowForGL_TRIANGLES(
    YsGLVertexBuffer &vtxBuf,YsGLNormalBuffer &nomBuf,
    const YsVec3 &p0,const YsVec3 &p1,YSBOOL forwardArrow,YSBOOL backwardArrow,
    const double inRad,const double outRad,const double headLng,int nDiv)
{
	vtxBuf.CleanUp();
	nomBuf.CleanUp();

	// Make a cross section.
	// backwardArrow==YSTRUE, forwardArrow==YSTRUE
	//    +        +
	//   /|        |\
	//  / +--------+ \
	// /              \
	// 
	// backwardArrow==YSFALSE, forwardArrow==YSTRUE
	//             +
	//             |\
	// +-----------+ \
	// |              \
	// 
	// 
	// backwardArrow==YSTRUE, forwardArrow==YSFALSE
	//    +
	//   /|
	//  / +-----------+
	// /              |

	YsVec2 section[6]; // Up to 6 points
	YsVec2 normal[5];  // Up to 5 normal vectors
	int nSection=0;
	const double L=(p1-p0).GetLength();

	section[nSection++].Set(0.0,0.0);
	if(YSTRUE==backwardArrow)
	{
		section[nSection++].Set(headLng,outRad);
		section[nSection++].Set(headLng,inRad);
	}
	else
	{
		section[nSection++].Set(0.0,inRad);
	}

	if(YSTRUE==forwardArrow)
	{
		section[nSection++].Set(L-headLng,inRad);
		section[nSection++].Set(L-headLng,outRad);
	}
	else
	{
		section[nSection++].Set(L,inRad);
	}
	section[nSection++].Set(L,0.0);

	for(int i=0; i<nSection-1; ++i)
	{
		auto v=YsUnitVector(section[i+1]-section[i]);
		normal[i].Set(-v.y(),v.x());
	}



	// Then create a solid of revolution.
	const auto xAxs=YsUnitVector(p1-p0);
	const auto yAxs=YsUnitVector(xAxs.GetArbitraryPerpendicularVector());
	const auto zAxs=xAxs^yAxs;
	YsVec3 sectionBuf[2][6],normalBuf[2][5];
	for(int i=0; i<nSection; ++i)
	{
		const auto wVec=yAxs;
		sectionBuf[0][i]=p0+xAxs*section[i].x()+wVec*section[i].y();
	}
	for(int i=0; i<nSection-1; ++i)
	{
		const auto wVec=yAxs;
		normalBuf[0][i]=xAxs*normal[i].x()+wVec*normal[i].y();
	}

	YsVec3 *curSection=sectionBuf[1],*prvSection=sectionBuf[0];
	YsVec3 *curNormal=normalBuf[1],*prvNormal=normalBuf[0];
	for(int r=1; r<=nDiv; ++r)
	{
		const double a=2.0*YsPi*(double)r/(double)nDiv;
		const double c=cos(a);
		const double s=sin(a);
		const auto wVec=c*yAxs+s*zAxs;

		for(int i=0; i<nSection; ++i)
		{
			curSection[i]=p0+xAxs*section[i].x()+wVec*section[i].y();
		}
		for(int i=0; i<nSection-1; ++i)
		{
			curNormal[i]=xAxs*normal[i].x()+wVec*normal[i].y();
		}

		vtxBuf.Add(curSection[0]);
		vtxBuf.Add(curSection[1]);
		vtxBuf.Add(prvSection[1]);
		nomBuf.Add(curNormal[0]);
		nomBuf.Add(curNormal[0]);
		nomBuf.Add(prvNormal[0]);

		for(int i=1; i<nSection-2; ++i)
		{
			vtxBuf.Add(curSection[i]);
			vtxBuf.Add(curSection[i+1]);
			vtxBuf.Add(prvSection[i+1]);
			nomBuf.Add(curNormal[i]);
			nomBuf.Add(curNormal[i]);
			nomBuf.Add(prvNormal[i]);

			vtxBuf.Add(prvSection[i]);
			vtxBuf.Add(curSection[i]);
			vtxBuf.Add(prvSection[i+1]);
			nomBuf.Add(prvNormal[i]);
			nomBuf.Add(curNormal[i]);
			nomBuf.Add(prvNormal[i]);
		}

		vtxBuf.Add(prvSection[nSection-2]);
		vtxBuf.Add(curSection[nSection-2]);
		vtxBuf.Add(prvSection[nSection-1]);
		nomBuf.Add(prvNormal[nSection-2]);
		nomBuf.Add(curNormal[nSection-2]);
		nomBuf.Add(prvNormal[nSection-2]);

		YsSwapSomething(curSection,prvSection);
		YsSwapSomething(curNormal,prvNormal);
	}

	return YSOK;
}
