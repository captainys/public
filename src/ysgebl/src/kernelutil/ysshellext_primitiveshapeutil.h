#ifndef YSSHELLEXT_PRIMITIVESHAPEUTIL_IS_INCLUDED
#define YSSHELLEXT_PRIMITIVESHAPEUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>

/*! Base class for primitive-shape generator.
*/
class YsShellExt_PrimitiveShapeUtil
{
protected:
	/*! An array for storing generated vertex handles.
	*/
	YsArray <YsShell::VertexHandle> generatedVtHd;

	/*! An array for storing generated polygon handles.
	*/
	YsArray <YsShell::PolygonHandle> generatedPlHd;
};

/*! A class for generating a sphere in YsShellExt or YsShellExtEdit
*/
class YsShellExt_SphereGeneratorUtil : public YsShellExt_PrimitiveShapeUtil
{
private:
	int nLat,nLng;
	YsArray <YsArray <YsVec3> > vtPosTable;
public:
	/*! Set up a sphere for generation.
	*/
	void SetUp(const YsVec3 &cen,const double rad,int nLat,int nLng);

	/*! Generates a sphere in a YsShellExt or YsshellExtEdit.
	*/
	template <class SHLCLASS>
	void Generate(SHLCLASS &shl)
	{
		typename SHLCLASS::StopIncUndo undoGuard(shl);

		YsArray <YsArray <YsShell::VertexHandle> > vtHdTable;
		vtHdTable.resize(nLat+1);
		for(int y=0; y<=nLat; ++y)
		{
			vtHdTable[y].resize(nLng);
			if(y!=0 && y!=nLat)
			{
				for(int x=0; x<nLng; ++x)
				{
					auto vtHd=shl.AddVertex(vtPosTable[y][x]);
					vtHdTable[y][x]=vtHd;
					generatedVtHd.push_back(vtHd);
				}
			}
			else
			{
				auto vtHd=shl.AddVertex(vtPosTable[y][0]);
				for(int x=0; x<nLng; ++x)
				{
					vtHdTable[y][x]=vtHd;
				}
				generatedVtHd.push_back(vtHd);
			}
		}

		for(int y=0; y<nLat; ++y)
		{
			for(int x=0; x<nLng; ++x)
			{
				YsVec3 quadVtPos[4]=
				{
					vtPosTable[y][x],
					vtPosTable[y].GetCyclic(x+1),
					vtPosTable[y+1].GetCyclic(x+1),
					vtPosTable[y+1][x]
				};
				YsShell::VertexHandle quadVtHd[4]=
				{
					vtHdTable[y][x],
					vtHdTable[y].GetCyclic(x+1),
					vtHdTable[y+1].GetCyclic(x+1),
					vtHdTable[y+1][x]
				};
				int nVt=4;
				if(0==y)
				{
					quadVtPos[1]=quadVtPos[2];
					quadVtPos[2]=quadVtPos[3];
					quadVtHd[1]=quadVtHd[2];
					quadVtHd[2]=quadVtHd[3];
					nVt=3;
				}
				else if(nLat-1==y)
				{
					nVt=3;
				}

				auto plHd=shl.AddPolygon(nVt,quadVtHd);
				auto nom=YsGetAverageNormalVector(nVt,quadVtPos);
				shl.SetPolygonNormal(plHd,nom);
				generatedPlHd.push_back(plHd);
			}
		}
	}
};


/*! A class for generating a cylinder in YsShellExt or YsShellExtEdit.
*/
class YsShellExt_BoxGeneratorUtil : public YsShellExt_PrimitiveShapeUtil
{
private:
	YsVec3 boxVtPos[8];
	static const int boxVtIdx[6][4];
public:
	/*! Set up a cube for generation.
	    A diagonal of the box is defined by two points p1 and p2.
	*/
	void SetUp(const YsVec3 &p1,const YsVec3 &p2);

	/*! Generates a box in a YsShellExt or YsshellExtEdit.
	*/
	template <class SHLCLASS>
	void Generate(SHLCLASS &shl)
	{
		typename SHLCLASS::StopIncUndo undoGuard(shl);

		generatedVtHd.resize(8);
		for(int i=0; i<8; ++i)
		{
			generatedVtHd[i]=shl.AddVertex(boxVtPos[i]);
		}

		generatedPlHd.resize(6);
		for(int i=0; i<6; ++i)
		{
			YsShell::VertexHandle quadVtHd[4]=
			{
				generatedVtHd[boxVtIdx[i][0]],
				generatedVtHd[boxVtIdx[i][1]],
				generatedVtHd[boxVtIdx[i][2]],
				generatedVtHd[boxVtIdx[i][3]],
			};
			generatedPlHd[i]=shl.AddPolygon(4,quadVtHd);
		}
	}
};

/* } */
#endif
