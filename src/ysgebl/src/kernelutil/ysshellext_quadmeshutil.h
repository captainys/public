#ifndef YSSHELLEXT_QUADMESHUTIL_IS_INCLUDED
#define YSSHELLEXT_QUADMESHUTIL_IS_INCLUDED
/* { */



#include "ysshellext.h"
#include "ysshellext_geomcalc.h"

class YsShellExt_QuadMeshUtil
{
public:
	class Candidate
	{
	public:
		YsShell::VertexHandle quadVtHd[4];
		YsShell::PolygonHandle quadPlHd;
		YSHASHKEY srcPlKey[2];
	};

	class ScoreFunc
	{
	public:
		/*! Calculate quad score.  Smaller is better.
		*/
		virtual double Calculate(const YsShellExt &shl,const Candidate &can) const=0;

		/*! The candidate will be dropped if the score is greater than this threshold.
		*/
		virtual double Threshold(const YsShellExt &shl) const=0;

		/*! If this function returns YSTRUE, the candidate will be excluded regardless of the score.
		*/
		virtual YSBOOL DontMerge(const YsShellExt &shl,YsShellEdgeEnumHandle edHd) const=0;
	};
	class ScoreFunc_Orthogonality : public ScoreFunc
	{
		virtual double Calculate(const YsShellExt &shl,const Candidate &can) const;
		virtual double Threshold(const YsShellExt &shl) const;
		virtual YSBOOL DontMerge(const YsShellExt &shl,YsShellEdgeEnumHandle edHd) const;
	};


	class ThreadParam
	{
	public:
		YsArray <YsShellEdgeEnumHandle> toTest;
		const ScoreFunc *scoreFuncPtr;
		const YsShellExt *shlPtr;

		YsArray <Candidate> candidate;
		YsArray <double> score;

		void Run(void);
	};

	YsArray <Candidate> candidate;
	YsArray <double> score;

public:
	YsShellExt_QuadMeshUtil();
	~YsShellExt_QuadMeshUtil();
	void CleanUp(void);

	void MakeCandidate(const YsShellExt &shl,const ScoreFunc &scoreFunc,int nThread=8);

	template <class SHLCLASS>
	void Convert(SHLCLASS &shl)
	{
		for(auto &c : candidate)
		{
			YsShell::PolygonHandle plHd[2]=
			{
				shl.FindPolygon(c.srcPlKey[0]),
				shl.FindPolygon(c.srcPlKey[1]),
			};
			if(nullptr!=plHd[0] && nullptr!=plHd[1])
			{
				YsVec3 nom0=YsVec3::UnitVector(shl.GetNormal(plHd[0])+shl.GetNormal(plHd[1]));
				auto col=shl.GetColor(plHd[0]);
				auto fgHd=shl.FindFaceGroupFromPolygon(plHd[0]);

				shl.ForceDeletePolygon(plHd[0]);
				shl.ForceDeletePolygon(plHd[1]);
				c.quadPlHd=shl.AddPolygon(4,c.quadVtHd);

				auto n=YsShell_CalculateNormal(shl.Conv(),c.quadPlHd);
				if(n*nom0<0.0)
				{
					n=-n;
				}
				shl.SetPolygonNormal(c.quadPlHd,n);
				shl.SetPolygonColor(c.quadPlHd,col);
				if(nullptr!=fgHd)
				{
					YsShell::PolygonHandle fgPlHd[1]={c.quadPlHd};
					shl.AddFaceGroupPolygon(fgHd,1,fgPlHd);
				}
			}
		}
	}
};



/* } */
#endif
