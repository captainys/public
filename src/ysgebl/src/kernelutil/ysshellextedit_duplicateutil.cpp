/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_duplicateutil.cpp
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

#include "ysshellextedit_duplicateutil.h"
#include <ysshellextedit.h>



YSRESULT YsShellExtEdit_DuplicateUtil::MakeAxisymmetric(class YsShellExtEdit &shl,const int component,YSBOOL dontMakeCopyOnSymmPlane,YSBOOL deletePolygonOnSymmPlane)
{
	YsShellExtEdit::StopIncUndo incUndo(&shl);

	for(auto &vtx : vertexArray)
	{
		auto newPos=vtx.pos;
		if(YSTRUE==dontMakeCopyOnSymmPlane && YSTRUE==YsEqual(newPos[component],0.0) && (const YsShellExt *)&shl==srcShl)
		{
			vtx.newVtHd=vtx.srcVtHd;
		}
		else
		{
			newPos[component]=-newPos[component];
			vtx.newVtHd=shl.AddVertex(newPos);
			shl.SetVertexAttrib(vtx.newVtHd,vtx.attrib);
		}
	}

	for(auto &plg : polygonArray)
	{
		YSBOOL outOfSymmPlane=YSFALSE;
		YsArray <YsShellVertexHandle,4> newPlVtHd;
		newPlVtHd.Set(plg.vertexArray.GetN(),NULL);
		for(YSSIZE_T idx=0; idx<plg.vertexArray.GetN(); ++idx)
		{
			auto vtIdx=plg.vertexArray[idx];
			auto &vtx=vertexArray[vtIdx];
			if(vtx.srcVtHd!=vtx.newVtHd)
			{
				outOfSymmPlane=YSTRUE;
			}
			newPlVtHd[newPlVtHd.GetN()-1-idx]=vtx.newVtHd;
		}

		if(YSTRUE==outOfSymmPlane || (const YsShellExt *)&shl!=srcShl)
		{
			auto newPlHd=shl.AddPolygon(newPlVtHd);
			shl.SetPolygonAttrib(newPlHd,plg.attrib);
			shl.SetPolygonColor(newPlHd,plg.col);

			YsVec3 newNom=plg.nom;
			newNom[component]=-newNom[component];
			shl.SetPolygonNormal(newPlHd,newNom);

			plg.newPlHd=newPlHd;
		}
		else
		{
			if(YSTRUE==deletePolygonOnSymmPlane)
			{
				shl.DeletePolygon(plg.srcPlHd);
			}
			plg.newPlHd=NULL;
		}
	}

	PasteConstEdgeForMirrorImage(shl);
	PasteFaceGroup(shl);

	return YSOK;
}



YSRESULT YsShellExtEdit_DuplicateUtil::MakeBySymmetryPlane(class YsShellExtEdit &shl,const YsPlane &pln,YSBOOL dontMakeCopyOnSymmPlane,YSBOOL deletePolygonOnSymmPlane)
{
	YsShellExtEdit::StopIncUndo incUndo(&shl);

	for(auto &vtx : vertexArray)
	{
		auto newPos=vtx.pos;
		if(YSTRUE==dontMakeCopyOnSymmPlane && YSTRUE==pln.CheckOnPlane(newPos) && (const YsShellExt *)&shl==srcShl)
		{
			vtx.newVtHd=vtx.srcVtHd;
		}
		else
		{
			const YsVec3 prj=pln.GetNormal()*(pln.GetNormal()*(newPos-pln.GetOrigin()));
			newPos=newPos-prj*2.0;
			vtx.newVtHd=shl.AddVertex(newPos);
			shl.SetVertexAttrib(vtx.newVtHd,vtx.attrib);
		}
	}

	for(auto &plg : polygonArray)
	{
		YSBOOL outOfSymmPlane=YSFALSE;
		YsArray <YsShellVertexHandle,4> newPlVtHd;
		newPlVtHd.Set(plg.vertexArray.GetN(),NULL);
		for(YSSIZE_T idx=0; idx<plg.vertexArray.GetN(); ++idx)
		{
			auto vtIdx=plg.vertexArray[idx];
			auto &vtx=vertexArray[vtIdx];
			if(vtx.srcVtHd!=vtx.newVtHd)
			{
				outOfSymmPlane=YSTRUE;
			}
			newPlVtHd[newPlVtHd.GetN()-1-idx]=vtx.newVtHd;
		}

		if(YSTRUE==outOfSymmPlane || (const YsShellExt *)&shl!=srcShl)
		{
			auto newPlHd=shl.AddPolygon(newPlVtHd);
			shl.SetPolygonAttrib(newPlHd,plg.attrib);
			shl.SetPolygonColor(newPlHd,plg.col);

			const YsVec3 prj=pln.GetNormal()*(plg.nom*pln.GetNormal());
			const YsVec3 newNom=plg.nom-prj*2.0;
			shl.SetPolygonNormal(newPlHd,newNom);

			plg.newPlHd=newPlHd;
		}
		else
		{
			if(YSTRUE==deletePolygonOnSymmPlane)
			{
				shl.DeletePolygon(plg.srcPlHd);
			}
			plg.newPlHd=NULL;
		}
	}

	PasteConstEdgeForMirrorImage(shl);
	PasteFaceGroup(shl);

	return YSOK;
}



void YsShellExtEdit_DuplicateUtil::PasteConstEdgeForMirrorImage(class YsShellExtEdit &dstShl)
{
	for(auto &ce : constEdgeArray)
	{
		YSBOOL outOfSymmPlane=YSFALSE;
		YsArray <YsShellVertexHandle,4> newVtHd;
		newVtHd.Set(ce.vertexArray.GetN(),NULL);
		for(YSSIZE_T idx=0; idx<ce.vertexArray.GetN(); ++idx)
		{
			auto vtIdx=ce.vertexArray[idx];
			auto &vtx=vertexArray[vtIdx];
			if(vtx.srcVtHd!=vtx.newVtHd)
			{
				outOfSymmPlane=YSTRUE;
			}
			newVtHd[newVtHd.GetN()-1-idx]=vtx.newVtHd;
		}

		if(YSTRUE==outOfSymmPlane || (const YsShellExt *)&dstShl!=srcShl)
		{
			YsShellExt::ConstEdgeHandle newCeHd=dstShl.AddConstEdge(newVtHd,ce.isLoop);
			dstShl.SetConstEdgeAttrib(newCeHd,ce.attrib);
		}
	}
}

