/* ////////////////////////////////////////////////////////////

File Name: ysshellext_smoothingutil.h
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

#ifndef YSSHELLEXT_SMOOTHINGUTIL_IS_INCLUDED
#define YSSHELLEXT_SMOOTHINGUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>

class YsShellExt_SmoothingUtil : public YsShellVertexPositionSource
{
public:
	class Preference
	{
	public:
	};

public:
};



class YsShellExt_FaceGroupSmoothingUtil : public YsShellExt_SmoothingUtil
{
public:
	class VertexState
	{
	public:
		YSBOOL hasCrawlingState;
		YsVec3 pos;
		YsShellCrawler::State sta;
	};

private:
	const YsShellExt *originalShl;
	YsShellExt cloneShl;
	YsShellVertexAttribTable <YsShellCrawler::State> crawlState;

	YsShell::VertexHandle OriginalVtHdToCloneVtHd(YsShell::VertexHandle orgVtHd) const;

	YsShellCrawler::CanPassFunction *canPassFunc;

	YSBOOL watchMode;

public:
	YsShellExt_FaceGroupSmoothingUtil();

	/*! Set pointer to YsShellCrawler::CanPassFunction.
	    If can-pass function is needed, this function must be called before Begin.
	*/
	void SetCanPassFunc(YsShellCrawler::CanPassFunction *func);

	/*! Call this function before starting smoothing.
	    This class creates a clone of the face group on which the smoothing is applied.
	    The changes can then be committed to the original shell by Commit() function.
	    While this class is performing smooting operations, the original shell must not be modified.
	*/
	YSRESULT Begin(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd);

	/*! Call this function before starting smoothing.
	    This class creates a clone of the shell which the smoothing is applied.
	    The changes can then be committed to the original shell by Commit() function.
	    While this class is performing smooting operations, the original shell must not be modified.
	*/
	YSRESULT Begin(const YsShellExt &shl);

	/*! This function returns a temporary vertex position of VtHd.
	    VtHd is a handle to the original shell, not internally-stored clone shell.
	    (The handle to the clone shell is not visible to the outside of the class anyway.)
	*/
	YsVec3 GetVertexPosition(YsShell::VertexHandle VtHd) const;

	/*! This function returns the current state of the vertex.
	    The vertex state can be saved before modifying the vertex position by SetVertexPosition.
	    If the smoothing result is not satisfactory, the change can be reverted by RestoreVertexState and give the return value of this function.
	    VtHd is the vertex handle to the original shell.
	*/
	VertexState SaveVertexState(YsShell::VertexHandle VtHd) const;

	/*! This function restores the state of the vertex.
	*/
	void RestoreVertexState(YsShell::VertexHandle vtHd,const VertexState &sta);

	/*! This function sets the vertex position of VtHd.
	    VtHd is a handle to the original shell, not internally-stored clone shell.
	    If Pos is not on the surface of the shell, it will be moved on the surface by YsShellCrawler class.
	*/
	YSRESULT SetVertexPosition(YsShell::VertexHandle VtHd,const YsVec3 &Pos);

	/*! This function sets the vertex position of VtHd.
	    VtHd is a handle to the original shell, not internally-stored clone shell.
	    Unlike SetVertexPosition, it forces the vertex to be at the given position, even if it is off the original surface mesh.
	    It also moves the crawler to the closest position to Pos.
	    Therefore, the vertex position and the crawler position may be different after this function.
	*/
	YSRESULT ForceSetVertexPosition(YsShell::VertexHandle VtHd,const YsVec3 &Pos);

	/*! Set watch mode.
	*/
	void SetWatchMode(YSBOOL watch);

	/*! This function returns all vertices in the smoothing target.
	    All handles are to the original shell.
	*/
	YsArray <YsShell::VertexHandle> AllVertex(void) const;

	/*! Returns YSTRUE if the vertex is included in the smoothing.  YSFALSE if not. */
	YSBOOL IsIncluded(YsShell::VertexHandle VtHd) const;

	/*! This function commits changes made in the clone shell back to the original shell.
	    Since the true identity of the original shell can be a sub-class of YsShellExt, 
	    and the behavior of the modifiers may be different, 
	    the l-value reference to the original shell must be given as the parameter.
	*/
	template <class SHLCLASS>
	void Commit(SHLCLASS &Shl)
	{
		typename SHLCLASS::StopIncUndo undoGuard(Shl);

		for(auto vtHd : cloneShl.AllVertex())
		{
			auto vtKey=cloneShl.GetSearchKey(vtHd);
			auto targetVtHd=Shl.FindVertex(vtKey);
			if(nullptr!=targetVtHd)
			{
				Shl.SetVertexPosition(targetVtHd,cloneShl.GetVertexPosition(vtHd));
			}
		}
	}
};



/*! Calculating one iteration of Laplacian smoothing using the current vertex-position source.

    The reason why Shl and VtPosSrc are needed is because temporary vertex position may not be final until the pre-defined termination condition is met.
    There is a situation that temporary vertex position must not be committed to Shl until the smoothing result is verified and confirmed satisfactory.
    VtPosSource can be a storage that stores temporary vertex positions until those are ready to be committed.

    For example, YsShellExt_FaceGroupSmoothingUtil can be used as VtPosSrc, in which case vertices are moved over the cloned face group stored in YsshellExt_FaceGrouopSmoothingUtil.
*/
YsResultAndVec3 YsShellExt_LaplacianSmoothing(const YsShellExt &Shl,const YsShellVertexPositionSource &VtPosSrc,YsShell::VertexHandle vtHd);


/* } */
#endif
