/* ////////////////////////////////////////////////////////////

File Name: ysshellext_protrusionfinder.h
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

#ifndef YSSHELLEXT_PROTRUSIONFINDER_IS_INCLUDED
#define YSSHELLEXT_PROTRUSIONFINDER_IS_INCLUDED
/* { */

#include <ysshellext.h>

/*! A class for finding a protrusions and through holes.
*/
class YsShellExt_ProtrusionFinder
{
public:
	class Protrusion
	{
	public:
		class Boundary
		{
		public:
			YsArray <YsShell::VertexHandle> vtHd;
			YsArray <YSSIZE_T> constraintIndex;

			YsShell::VertexHandle midVertex;
			YsArray <YsShell::Triangle> tri;
			YsArray <YsArray <YsShell::VertexHandle> > newCeVtHd;
			YSBOOL canRemoveSurroundingConstEdge;

			void CleanUp(void);
		};

		YsShell::PolygonHandle seedPlHd;
		YsArray <YSHASHKEY> plKey;
		YsArray <Boundary> boundary;
		YsArray <YsVec3> path;
		YsVec3 axis,cen;
		double volume; // Positive->Protrusion   Negative->Hole

		void CleanUp(void);

		YsArray <YsShell::PolygonHandle> PlHd(const YsShellExt &shl) const;

	public:
		void MakeBoundaryConstraint(const YsShellExt &shl);
	private:
		void MakeBoundaryConstraint(YsArray <YSSIZE_T> &boundaryConstIndex,const YsShellExt &shl,const YsConstArrayMask <YsShell::VertexHandle> &boundary,const YsShellPolygonStore &plToDel);

	public:
		template <class SHLCLASS>
		void AddBoundaryMidVertex(SHLCLASS &shl)
		{
			for(auto &b : boundary)
			{
				if(2<=b.constraintIndex.GetN())
				{
					auto cen=shl.GetCenter(b.vtHd);
					b.midVertex=shl.AddVertex(cen);
				}
				else
				{
					b.midVertex=nullptr;
				}
			}
		}

	public:
		YSRESULT TessellateBoundary(const YsShellExt &shl);
	private:
		YsArray <YsShell::Triangle> TessellateBoundary(Boundary &b,const YsShellExt &shl);
	};

	class SearchCondition
	{
	public:
		/*! If YSTRUE, it searches from each polygon by cutting with a plane. 
		    Default value is YSTRUE.*/
		YSBOOL fromPolygon;

		/*! If YSTRUE, it searches for face groups that has two or more boundaries that satisfies the conditions. 
		    Default value is YSFALSE. */
		YSBOOL fromFaceGroup;

		/*! The protrusion can be identified only if  it is reasonably prismatic.
		    If it is perfectly prismatic, the normal vectors of the polygons forming the hole/protrusion
		    should be parallel to the axis.
		    This threshold specifies how much deviation of the axial-direction from a polygon 
		    of the hole/protrusion is acceptable.
		    The threshold must be reasonably small.
		    If it is zero, only perfectly prismatic holes/protrusions are identified.
		    Default value is 10 degrees.
		*/
		double taperAngleThr;

		/*! Only holes/protrusions that fits within a cylinder of this diameter will be identified.
		    Default value is 1.0.
		*/
		double diameter;       // Must make one round without going out of this diameter

		/*! If true, search will be per face group.
		    Default value is YSFALSE.
		*/
		YSBOOL perFaceGroup;

		/*! At least nMinimumSamplePerFaceGroup polygons (or as many polygons as in a face group) will be sampled from each face group. */
		int nMinimumSamplePerFaceGroup;

		/*! Roughly nSample polygons will be sampled. */
		int nSample;

		SearchCondition();
	};

	class Aftermath
	{
	public:
		class SubChunk
		{
		public:
			double area;
			YsArray <YsShell::PolygonHandle> plHd;
		};

		int chunkIdx;
		int largestSubChunkIdx;
		YsArray <SubChunk> subChunk;
	};

	class UngroupedPolygonEnclosedByConstEdge;

private:
	YsShellPolygonStore partOfProtrusion;
	YsArray <Protrusion> protrusion;

	YsShellPolygonAttribTable <YSSIZE_T> plHdToChunkIdx;
	YsArray <YsArray <YSHASHKEY> > chunkPlKey;

	YsArray <Aftermath> aftermath;


public:
	YsShellExt_ProtrusionFinder();
	~YsShellExt_ProtrusionFinder();
	void CleanUp(void);

	void Begin(const YsShellExt &shl);
	void Find(const YsShellExt &shl,SearchCondition cond);
protected:
	void FindFromPolygon(const YsShellExt &shl,SearchCondition cond);
	void FindFromFaceGroup(const YsShellExt &shl,SearchCondition cond);
private:
	YsShellPolygonStore MakeSamplePolygon(const YsShellExt &shl,SearchCondition cond) const;

	YSRESULT FindFromPolygon(Protrusion &prt,const YsShellExt &shl,YsShell::PolygonHandle plHd,SearchCondition cond);
	YSRESULT FindFromPolygonOneDirection(Protrusion &prt,const YsShellExt &shl,YsShell::PolygonHandle plHd,SearchCondition cond,const YsVec3 &axis,const YsVec3 &dir);

	YSRESULT OptimizeAxis(Protrusion &prt,const YsShellExt &shl,SearchCondition cond);

	double CalculateAxisNormalError(const Protrusion &prt,const YsShellExt &shl) const;

	void AugmentByFaceGroup(Protrusion &prt,const YsShellExt &shl) const;

	// When polygons are removed, there must be two open holes.
	void CacheBoundary(Protrusion &prt,const YsShellExt &shl) const;

	void CacheVolume(Protrusion &prt,const YsShellExt &shl) const;

	YSRESULT RecheckDiameter(const Protrusion &prt,const YsShellExt &shl,SearchCondition cond) const;
	YSRESULT CheckTaper(const Protrusion &prt,const YsShellExt &shl,SearchCondition cond) const;
	YSRESULT CheckAngularCoverageAroundAxis(const Protrusion &prt,const YsShellExt &shl) const;

public:
	/*! Call this function before actually deleting holes/protrusions.
	*/
	void IdentifyConstVertexOnBoundary(const YsShellExt &shl);

public:
	const YsConstArrayMask <Protrusion> GetProtrusion(void) const;

public:
	/*! Use this function after removing holes/protrusions and patching the open loops.
	    This will populate internal aftermath array, which can later be used for deleting hanging polygons.
	*/
	void CalculateAftermath(const YsShellExt &shl);


	const YsConstArrayMask <Aftermath> GetAftermath(void) const;


	/*! Use this function after calculating aftermath.
	    This function will fill the open polygons created by the deletion of the feature.
	*/
	template <class SHLCLASS>
	void FillHole(SHLCLASS &shl)
	{
		typename SHLCLASS::StopIncUndo undoGuard(shl);

		for(auto &prt : protrusion)
		{
			auto allPlHd=prt.PlHd(shl.Conv());
			if(allPlHd.GetN()!=prt.plKey.GetN())
			{
				continue;
			}

			prt.AddBoundaryMidVertex(shl);
			if(YSOK==prt.TessellateBoundary(shl.Conv()))
			{
				shl.DeleteFaceGroupPolygonMulti(allPlHd);
				shl.DeleteMultiPolygon(allPlHd);

				for(auto &b : prt.boundary)
				{
					YsArray <YsShell::PolygonHandle> fgPlHd;
					for(auto t : b.tri)
					{
						const YsShell::VertexHandle plVtHd[3]=
						{
							t[2],t[1],t[0]
						};
						const YsVec3 plVtPos[3]=
						{
							shl.GetVertexPosition(plVtHd[0]),
							shl.GetVertexPosition(plVtHd[1]),
							shl.GetVertexPosition(plVtHd[2]),
						};
						auto plNom=YsGetAverageNormalVector(3,plVtPos);
						auto plHd=shl.AddPolygon(3,plVtHd);
						fgPlHd.Add(plHd);
						shl.SetPolygonNormal(plHd,plNom);
					}

					for(auto ceVtHd : b.newCeVtHd)
					{
						shl.AddConstEdge(ceVtHd,YSFALSE);
					}

					if(YSTRUE==b.canRemoveSurroundingConstEdge)
					{
						for(auto edIdx : b.vtHd.AllIndex())
						{
							for(auto ceHd : shl.FindConstEdgeFromEdgePiece(b.vtHd[edIdx],b.vtHd.GetCyclic(edIdx+1)))
							{
								shl.DeleteConstEdge(ceHd);
							}
							auto edPlHd=shl.FindPolygonFromEdgePiece(b.vtHd[edIdx],b.vtHd.GetCyclic(edIdx+1));
							YsShellExt::FaceGroupHandle fgHd=nullptr;
							for(auto plHd : edPlHd)
							{
								fgHd=shl.FindFaceGroupFromPolygon(plHd);
								if(nullptr!=fgHd)
								{
									break;
								}
							}

							for(auto plHd : edPlHd)
							{
								if(nullptr==shl.FindFaceGroupFromPolygon(plHd))
								{
									auto plHdToFill=FindUngroupedPolygonSeparatedByConstEdge(shl.Conv(),plHd);
									if(0<plHdToFill.GetN())
									{
										shl.AddFaceGroupPolygon(fgHd,plHdToFill);
									}
								}
							}
						}
					}
					else
					{
						shl.AddFaceGroup(fgPlHd);
					}
				}
			}
		}
	}

	/*! Call this function to delete islands after calling CalculateAftermath().
	*/
	template <class SHLCLASS>
	void DeleteIsland(SHLCLASS &shl)
	{
		YsShellVertexStore vtHdToDel(shl.Conv());

		for(auto &m : aftermath)
		{
			if(1<m.subChunk.GetN())
			{
				for(auto i : m.subChunk.AllIndex())
				{
					if(i!=m.largestSubChunkIdx)
					{
						for(auto plHd : m.subChunk[i].plHd)
						{
							vtHdToDel.Add(shl.GetPolygonVertex(plHd));
						}
						shl.ForceDeleteMultiPolygon(m.subChunk[i].plHd);
					}
				}
			}
		}

		YsShellExt::ConstEdgeStore ceHdToCheck(shl.Conv());
		for(auto vtHd : vtHdToDel)
		{
			ceHdToCheck.Add(shl.FindConstEdgeFromVertex(vtHd));
		}
		for(auto ceHd : ceHdToCheck)
		{
			YSBOOL stillUsed=YSFALSE;
			for(auto vtHd : shl.GetConstEdgeVertex(ceHd))
			{
				if(YSTRUE!=vtHdToDel.IsIncluded(vtHd))
				{
					stillUsed=YSTRUE;
					break;
				}
			}
			if(YSTRUE!=stillUsed)
			{
				shl.DeleteConstEdge(ceHd);
			}
		}
		for(auto vtHd : vtHdToDel)
		{
			shl.DeleteVertex(vtHd);
		}
	}

	
private:
	YsArray <YsShell::PolygonHandle> FindUngroupedPolygonSeparatedByConstEdge(const YsShellExt &shl,YsShell::PolygonHandle seedPlHd) const;

};


/* } */
#endif
