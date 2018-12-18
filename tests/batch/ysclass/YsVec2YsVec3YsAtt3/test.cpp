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

#include "../src/ysclass.h"


#define PrintEntryLabel YsPrintf("** %s (Line:%d) **\n",__FUNCTION__,__LINE__);


void Test(int &nFail,bool mustBeTrue,const char testTitle[])
{
	if(true==mustBeTrue)
	{
		printf("OK...%s\n",testTitle);
	}
	else
	{
		printf("!!!! ERROR !!!!....%s\n",testTitle);
	}
}


////////////////////////////////////////////////////////////
// Geometry

int TestGeometry_YsVec2(void)
{
	int nFail=0;
	PrintEntryLabel;

	const double v1value[2]={1.0,2.0};
	YsVec2 v1(v1value);
	YsVec2 v2;
	v2.Set(-100.0,-100.0);
	v2.Set(v1value);
	Test(nFail,v1==v2,"Substitution");

	const double v3value[3]={2,4};
	YsVec2 v3(v3value);
	v3/=2.0;
	Test(nFail,v1==v3,"Substitution");


	YsVec2 v4,v5;
	v4.Set(YsCos30deg,YsSin30deg);
	v4.Rotate(YsPi/6.0);
	v5.Set(YsCos60deg,YsSin60deg);
	Test(nFail,v4==v5,"Rotation");

	v4*=123.0;
	Test(nFail,(YSTRUE==YsEqual(v4.GetLength(),123.0) && YSTRUE==YsEqual(v4.GetSquareLength(),YsSqr(123.0))),"Length");

	v1.Set(1,2);
	v2.Set(3,4);
	v3=v1+v2;
	v4.Set(4,6);
	Test(nFail,(v3==v4),"Addition");

	v3=v2-v1;
	v4.Set(2,2);
	Test(nFail,(v3==v4),"Subtraction");

	v1.Set(1,2);
	v2.Set(3,4);
	const double dot=v1*v2;
	Test(nFail,(YSTRUE==YsEqual(dot,11.0)),"Dot product");


	return nFail;
}

int TestGeometry_YsVec3(void)
{
	int nFail=0;
	PrintEntryLabel;

	const double v1value[3]={1.0,2.0,3.0};
	YsVec3 v1(v1value);
	YsVec3 v2;
	v2.Set(-100.0,-100.0,-100.0);
	v2.Set(v1value);
	Test(nFail,(v1==v2),"Substitution");

	const double v3value[3]={2,4,6};
	YsVec3 v3(v3value);
	v3/=2.0;
	Test(nFail,(v1==v3),"Substitution");


	v1.Set(100,200,300);
	v2.Set(0.0,0.0,0.0);
	Test(nFail,(YSOK==v1.Normalize() && YSTRUE==YsEqual(v1.GetLength(),1.0) && YSOK!=v2.Normalize()),"Normalize");


	return nFail;
}

int TestGeometry_YsAtt3(void)
{
	YSCOORDSYSMODEL save=YsCoordSysModel;

	PrintEntryLabel;

	int nFail=0;
	for(int coordSysModel=0; coordSysModel<2; ++coordSysModel)
	{
		YsCoordSysModel=(0==coordSysModel ? YSOPENGL : YSBLUEIMPULSE);
		const YsVec3 fwdVec=(0==coordSysModel ? YsVec3(0.0,0.0,-1.0) : YsVec3(0.0,0.0,1.0));

		for(int ipch=-90; ipch<=90; ipch+=45)
		{
			const double pch=YsPi*(double)ipch/180.0;
			for(int ihdg=-180; ihdg<=180; ihdg+=45)
			{
				const double hdg=YsPi*(double)ihdg/180.0;

				for(int ibnk=-180; ibnk<=180; ibnk+=45)
				{
					const double bnk=YsPi*(double)ibnk/180.0;

					YsAtt3 att(hdg,pch,bnk);

					YsVec3 fwdVecByMul;
					att.Mul(fwdVecByMul,fwdVec);

					YsMatrix3x3 mat;
					YsVec3 fwdVecByMat3;
					fwdVecByMat3=att.GetMatrix3x3(mat)*fwdVec;

					YsVec3 fwdVecDirect=att.GetForwardVector();

					YsString msg;
					msg.Printf("CoordSys %d h=%d p=%d b=%d",coordSysModel,ihdg,ipch,ibnk);
					Test(nFail,(fwdVecByMul==fwdVecDirect && fwdVecByMat3==fwdVecDirect),msg);
				}
			}
		}
	}


	YsCoordSysModel=save;

	return nFail;
}

YSRESULT TestGeometry_PositiveVector(YsPositiveVectorCalculator &posiVec,YSSIZE_T nVec,const YsVec3 vec[])
{
	printf("Enter TestGeometry_PositiveVector\n");

	posiVec.Initialize(); // Initialize function must work.

	for(int i=0; i<nVec; i++)
	{
		posiVec.AddVector(vec[i]);
	}

	YsVec3 res;
	if(YSOK==posiVec.GetPositiveVector(res))
	{
		for(int i=0; i<nVec; i++)
		{
			printf("vec[%d]*res=%lf\n",i,vec[i]*res);
			if(0.0>vec[i]*res)
			{
				printf("Calculation Error!\n");
				return YSERR;
			}
		}
	}
	return YSOK;
}

int TestGeometry_PositiveVector(void)
{
	int nFail=0;

	printf("@@@ TESTING: YsPositiveVectorCalculator\n");

	YsPositiveVectorCalculator posiVec;
	YsVec3 nom[10];

	nom[0].Set(1.0,1.0,1.0);
	nom[1].Set(-1.0,1.0,1.0);
	nom[2].Set(0.0,1.0,0.0);
	if(YSOK!=TestGeometry_PositiveVector(posiVec,3,nom))
	{
		printf("@@@ TEST RESULT: ERROR\n");
		++nFail;
	}

	nom[3].Set(1.0,0.0,0.0);
	if(YSOK!=TestGeometry_PositiveVector(posiVec,4,nom))
	{
		printf("@@@ TEST RESULT: ERROR\n");
		++nFail;
	}

	nom[4].Set(0.0,0.0,1.0);
	if(YSOK!=TestGeometry_PositiveVector(posiVec,5,nom))
	{
		printf("@@@ TEST RESULT: ERROR\n");
		++nFail;
	}

	nom[5].Set(0.0,1.0,1.0);
	if(YSOK!=TestGeometry_PositiveVector(posiVec,6,nom))
	{
		printf("@@@ TEST RESULT: ERROR\n");
		++nFail;
	}

	if(0==nFail)
	{
		printf("@@@ TEST RESULT: OK\n");
	}
	return nFail;
}

int TestGeometry_ForwardVector(void)
{
	int err=0;

	for(int i=0; i<2; ++i)
	{
		YsCoordSysModel=(i==0 ? YSOPENGL : YSBLUEIMPULSE);

		printf("Coord Sys Model %d\n",YsCoordSysModel);

		const YsVec3 locFwdVec=(i==0 ? -YsZVec() : YsZVec());
		const YsVec3 locUpVec=YsYVec();


		{
			const double h=YsPi*2.0*(double)rand()/(double)RAND_MAX;
			const double p=0.5*YsPi-YsPi*(double)rand()/(double)RAND_MAX;
			const double b=YsPi-YsPi*2.0*(double)rand()/(double)RAND_MAX;

			YsAtt3 att;
			att.Set(h,p,b);

			YsMatrix3x3 mat;
			mat.Rotate(att);

			YsVec3 fwd=att.GetForwardVector();
			mat.MulInverse(fwd,fwd);

			printf("ForwardVector must becom Z vector: %s\n",fwd.Txt());
			printf("Error=%lf\n",(fwd-locFwdVec).GetLength());
			if(YsTolerance<(fwd-locFwdVec).GetLength())
			{
				err=1;
				printf("!!!!!!!! ERROR !!!!!!!!\n");
			}
		}

		{
			const double vx=((double)rand()/(double)RAND_MAX)-0.5;
			const double vy=((double)rand()/(double)RAND_MAX)-0.5;
			const double vz=((double)rand()/(double)RAND_MAX)-0.5;
			YsVec3 vec=YsUnitVector(YsVec3(vx,vy,vz));
			YsAtt3 reverseAtt(0.0,0.0,0.0);
			reverseAtt.SetForwardVector(vec);

			YsVec3 vec2=locFwdVec;
			reverseAtt.Mul(vec2,vec2);

			printf("ForwardVectors must match:\n");
			printf("  (%s) (%s)\n",vec.Txt(),vec2.Txt());
			printf("Error=%lf\n",(vec-vec2).GetLength());
			printf("H,P,B=%lf %lf %lf\n",reverseAtt.h(),reverseAtt.p(),reverseAtt.b());
			if(YsTolerance<(vec-vec2).GetLength())
			{
				err=1;
				printf("!!!!!!!! ERROR !!!!!!!!\n");
			}
		}

		{
			YsAtt3 att(0.0,0.0,0.0);
			att.NoseUp(YsPi/4.0);

			double pitch45degreeUp;
			if(0==i && 20140530<=YsAtt3::compatibilityLevel)
			{
				pitch45degreeUp=-YsPi/4.0;
			}
			else
			{
				pitch45degreeUp=YsPi/4.0;
			}

			printf("Pitch must be %lf=%lf\n",pitch45degreeUp,att.p());
			printf("Error=%lf\n",pitch45degreeUp-att.p());
			if(YsTolerance<fabs(pitch45degreeUp-att.p()))
			{
				err=1;
				printf("!!!!!!!! ERROR !!!!!!!!\n");
			}
		}


		{
			// Inverted
			YsAtt3 att(0.0,0.0,YsPi);
			att.NoseUp(YsPi/4.0);

			double pitch45degreeUp;
			if(0==i && 20140530<=YsAtt3::compatibilityLevel)
			{
				pitch45degreeUp=YsPi/4.0;
			}
			else
			{
				pitch45degreeUp=-YsPi/4.0;
			}

			printf("Pitch must be %lf=%lf\n",pitch45degreeUp,att.p());
			printf("Error=%lf\n",pitch45degreeUp-att.p());
			if(YsTolerance<fabs(pitch45degreeUp-att.p()))
			{
				err=1;
				printf("!!!!!!!! ERROR !!!!!!!!\n");
			}
		}


		{
			const double vx=((double)rand()/(double)RAND_MAX)-0.5;
			const double vy=((double)rand()/(double)RAND_MAX)-0.5;
			const double vz=((double)rand()/(double)RAND_MAX)-0.5;

			const double sx=((double)rand()/(double)RAND_MAX)-0.5;
			const double sy=((double)rand()/(double)RAND_MAX)-0.5;
			const double sz=((double)rand()/(double)RAND_MAX)-0.5;

			YsVec3 fwd=YsUnitVector(YsVec3(vx,vy,vz));
			YsVec3 sideVec=YsUnitVector(YsVec3(sx,sy,sz));

			YsVec3 upv=YsUnitVector(fwd^sideVec);

			YsAtt3 att;
			att.SetTwoVector(fwd,upv);



			YsVec3 reverseFwd,reverseUpv;
			att.Mul(reverseFwd,locFwdVec);
			att.Mul(reverseUpv,locUpVec);

			printf("Forward vectors must match: Error=%lf\n",(reverseFwd-fwd).GetLength());
			printf("Up vectors must match: Error=%lf\n",(reverseUpv-upv).GetLength());

			if(YsTolerance<(reverseFwd-fwd).GetLength() || YsTolerance<(reverseUpv-upv).GetLength())
			{
				err=1;
				printf("!!!!!!!! ERROR !!!!!!!!\n");
			}
		}
	}

	return err;
}

int TestGeometry(void)
{
	int nFail=0;
	nFail+=TestGeometry_YsVec2();
	nFail+=TestGeometry_YsVec3();
	nFail+=TestGeometry_YsAtt3();
	nFail+=TestGeometry_PositiveVector();
	nFail+=TestGeometry_ForwardVector();
	return nFail;
}


////////////////////////////////////////////////////////////

int main(void)
{
	auto nFail=TestGeometry();
	printf("%d failed\n",nFail);
	if(0<nFail)
	{
		return 1;
	}
	return 0;
}
