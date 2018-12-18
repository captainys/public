/* ////////////////////////////////////////////////////////////

File Name: yspositivevector.h
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

#ifndef YSPOSITIVEVECTOR_IS_INCLUDED
#define YSPOSITIVEVECTOR_IS_INCLUDED
/* { */

#include "ysshell.h"
#include "ysshellsearch.h"



/*! Base class for YsPositiveVectorCalculator and YsPositiveVolumeCalculator
*/
class YsVolumeSlasher
{
protected:
	YsShell shl;
	YsShellSearchTable search;

public:
	YsVolumeSlasher();
	~YsVolumeSlasher();
protected:
	void CalculateVertexOnEdge(
	    YsArray <YsShell::VertexHandle,16> &vtHdInsert,
	    YsArray <YsShell::VertexHandle,16> &vtHdOnPlane, // Not inserted, but already on plane.
	    const YsPlane &pln);
	void InsertVertexOnEdge(const YsConstArrayMask <YsShellVertexHandle> &vtHdInsert);
	void SlashPolygon(const YsPlane &pln);
	void DeleteNegativePolygon(const YsPlane &pln);
	void CloseHole(
	    const YsPlane &pln,
	    const YsConstArrayMask <YsShellVertexHandle> &vtHdInsert,
	    const YsConstArrayMask <YsShellVertexHandle> &vtHdOnPlane);
	YsShell::Edge FindOpenEdgeFrom(YsShell::VertexHandle vtHd0) const;

public:
	/*! Saves the slashed octahedron/cube for debugging purpose. */
	YSRESULT SaveSrf(const char fn[]) const;
};



/*! A class that calculates a vector v that satisfies n[i]*v>0
*/
class YsPositiveVectorCalculator : public YsVolumeSlasher
{
private:
	int nVec;
	YsVec3 vec[3];

public:
	/*! Default constructor.  Initializes the octahedron. */
	YsPositiveVectorCalculator();
	/*! Default destructor. */
	~YsPositiveVectorCalculator();
	/*! Initializes the octahedron used for finding the positive vector. */
	void Initialize(void);

	/*! Add a reference vector.  This class will find a vector that 
	    the dot (inner) product is positive for all reference vectors. */
	void AddVector(YsVec3 n); // Memo: n will be normalized.  Copied once anyway.  No point to make it a reference.

	/*! Alias for Add function. */
	inline void Add(const YsVec3 &n)
	{
		AddVector(n);
	}

public:
	/*! Returns a vector that the dot (inner) product is always positive for 
	    all reference vectors.  If no such vector exists, this function returns
	    YSERR and the content of vec will be undefined.  */
	YSRESULT GetPositiveVector(YsVec3 &vec) const;

	/*! After finding a positive vector, this function tries to maximize the minimum of vecOut*refVec[i].  
	    All refVec[i] need to be normalized before given to this function.  Or the result may be meaningless.
	    Constraint vectors will limit the direction so that vecOut*constVec[i] will be zero or greater.  */
	static YSRESULT Optimize(YsVec3 &vecOut,const YsVec3 &vecIn,const YSSIZE_T nRefVec,const YsVec3 refVec[],int nConst=0,const YsVec3 constVec[]=NULL);
private:
	static YSRESULT OptimizeByBiSection(double &newMinVec,YsVec3 &vecInOut,const YsVec3 &goal,const YSSIZE_T nRefVec,const YsVec3 refVec[],int nConst,const YsVec3 constVec[],const double currentMinimum); 
	static YSRESULT ApplyConstraint(YsVec3 &vecInOut,int nConst,const YsVec3 constVec[]);
};



/*! A class that calculates a volume that is positive side of pln[] 
    The class initializes a cube, and then chop off negative side of pln[i] one by one.
    Finally, the volume that is positive side of all pln[i] is obtained as a YsShell.
*/
class YsPositiveVolumeCalculator : public YsVolumeSlasher
{
public:
	/*! Initialize a cube.
	*/
	void Initialize(const YsVec3 &min,const YsVec3 &max);

	/*! Cuts the cube by the plane and drops the negative side of the plane.
	*/
	void AddPlane(const YsPlane &pln);

	/*! Test if dropping the negative side of the plane will wipe the remaining volume.
	    If the volume will remain after dropping the negative side, it returns YSOK.
	    If the volume will be zero after dropping the negative side, it reutns YSERR.
	*/
	YSRESULT TestPlane(const YsPlane &pln) const;

	/*! Returns the shell of the remaining volume.
	*/
	const YsShell &GetVolume(void) const;

	/*! Returns the center of gravity. */
	YsVec3 GetCenterOfGravity(void) const;
};


/* } */
#endif
