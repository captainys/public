/* ////////////////////////////////////////////////////////////

File Name: ysshellextio.h
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

#ifndef YSSHELLEXTIO_IS_INCLUDED
#define YSSHELLEXTIO_IS_INCLUDED
/* { */

#include "ysshellext.h"
#include "ysshellextedit.h"
#include "ysshellextentityset.h"

/*! This class can read a .SRF format text stream into a YsShellExt object. */
class YsShellExtReader
{
private:
	YsShellExtEntitySet readEntity;

	class ReadSrfVariable
	{
	public:
		enum READSTATE
		{
			NORMAL,
			IN_POLYGON
		};

		READSTATE state;
		YsArray <YsShellVertexHandle,4> currentPlVtHd;
		YSBOOL currentPlNoShad;
		YsColor currentPlCol;
		YsVec3 currentPlNom;
		YsKeyStore alreadyUsedCeIdent,alreadyUsedFgIdent,alreadyUsedVlIdent;
		YsKeyStore plKeyAlreadyPartOfFg;

		YsHashTable <YsShellExt::ConstEdgeHandle> ceIdentToCeHd;
		YsHashTable <YsShellExt::FaceGroupHandle> fgIdentToFgHd;
		YsHashTable <YsShellExt::VolumeHandle> vlIdentToVlHd;
	};

	ReadSrfVariable srf;

	mutable YsString lastErrorMessage;
	mutable YsString lastErrorLine;

public:
	/*! This function merges the .SRF format data to the output shell. */
	YSRESULT MergeSrf(YsShellExt &shl,YsTextInputStream &inStream);

	/*! This function returns an entity set that has all newly read entities. */
	const YsShellExtEntitySet &GetEntitySet(void) const;

public:
	/*! This function start merging a text input stream. */
	void StartMergeSrf(const YsShellExt &shl);

	/*! This function processes one line input from the text stream. */
	YSRESULT ReadSrfOneLine(YsShellExt &shl,const YsString &str);
private:
	YSRESULT ReadSrfOneLineNormal(YsShellExt &shl,const YsString &str);
	YSRESULT ReadSrfOneLineInPolygon(YsShellExt &shl,const YsString &str);
	void StartPolygon(void);
	void EndPolygon(void);

	YsShellExt::ConstEdgeHandle FindConstEdgeByIdentCreateIfNotExist(YsShellExt &shl,int ceIdent);
	YsShellExt::FaceGroupHandle FindFaceGroupByIdentCreateIfNotExist(YsShellExt &shl,int fgIdent);
	YsShellExt::VolumeHandle FindVolumeByIdentCreateIfNotExist(YsShellExt &shl,int vlIdent);

	template <const int N>
	YSRESULT GetVtHdArray(YsArray <YsShellVertexHandle,N> &plVtHd,YSSIZE_T n,const YsString args[],const char src[]) const;
	template <const int N>
	YSRESULT GetPlHdArray(YsArray <YsShellPolygonHandle,N> &fgPlHd,YSSIZE_T n,const YsString args[],const char src[]) const;
	template <const int N>
	YSRESULT GetFgHdArray(YsArray <YsShellExt::FaceGroupHandle,N> &fgHd,YSSIZE_T n,const YsString args[],const char src[]) const;

	YsShellVertexHandle GetVertexHandle(const char arg[],const char src[]) const;
	YsShellPolygonHandle GetPolygonHandle(const char arg[],const char src[]) const;

public:
	/*! This function terminates merging a text input stream. */
	void EndMergeSrf(void);

public:
	/*! This function returns the last error message. */
	const char *GetLastErrorMessage(void) const;

	/*! This function returns the content of the line in which the last error took place. */
	const char *GetLastErrorLine(void) const;
};



/*! This class is for writing YsShellExt class to a stream. */
class YsShellExtWriter
{
private:
	YsHashTable <YSSIZE_T> ceKeyToCeIdentMap;
	YsHashTable <YSSIZE_T> fgKeyToFgIdentMap;

public:
	/*! This function writes shl to the file. */
	YSRESULT SaveSrf(const char fn[],const YsShellExt &shl);

	/*! This function writes shl to outStream. */
	YSRESULT SaveSrf(YsTextOutputStream &outStream,const YsShellExt &shl);

	void MakeCeKeyToCeIdentMap(const YsShellExt &shl);
	void MakeFgKeyToFgIdentMap(const YsShellExt &shl);
};



/*! This class reads a Wavefront .OBJ file into a YsShellExt class. */
class YsShellExtObjReader
{
public:
	class ReadOption
	{
	public:
		YSBOOL useSmoothShadingGroupAsFaceGroup;
		YSBOOL usePolygonGroupAsFaceGroup;

		ReadOption()
		{
			usePolygonGroupAsFaceGroup=YSTRUE;
			useSmoothShadingGroupAsFaceGroup=YSTRUE;
		};
	};

	ReadOption option;
	YsArray <YsShellVertexHandle> vtHdArray;
	YsArray <YsVec3> nomArray;  // Stores normals from "vn" tag.  For future versions.
	YsArray <YsVec2> texCoordArray;  // Stores normals from "vt" tag.  For future versions.

	YSBOOL inPlGrp;
	YsString currentPlGrpName;
	YsString currentSmthShadGrpName;
	YsArray <YsShellPolygonHandle> currentPlGrp;

public:
	/*! Begins reading .OBJ file. */
	void BeginReadObj(const ReadOption &option);

	/*! Read one line of .OBJ file. */
	YSRESULT ReadObjOneLine(YsShellExt &shl,YsString &str);

	/*! End reading .OBJ file. */
	void EndReadObj(YsShellExt &shl);

	/*! This function reads a .OBJ file from a text-input stream.  Internally this uses BeginReadObj, EndReadObj, and ReadObnOneLine. */
	YSRESULT ReadObj(YsShellExt &shl,YsTextInputStream &inStream,const ReadOption &option);

private:
	void DecodePlVtId(int &vtIdx,int &texCoordIdx,int &nomIdx,const YsString &str) const;
};

class YsShellExtObjWriter
{
public:
	class WriteOption
	{
	};

private:
	YsHashTable <YSSIZE_T> plKeyToObjPlIdx;
	YsArray <YsShellPolygonHandle> plHdArray;
	YsArray <YSHASHKEY> fgKeyArray;

	YsHashTable <int> vtKeyToNomIdx;
	YsArray <YsVec3> nomArray;

	void BeginWriteObj(const WriteOption &option);
	void WriteVertex(YsTextOutputStream &outStream,const YsShellExt &shl);
	void MakeNormalArray(const YsShellExt &shl);
	void WriteNormal(YsTextOutputStream &outStream);
	void SortPolygonByFaceGroupKey(const YsShellExt &shl);
	void WritePolygon(YsTextOutputStream &outStream,const YsShellExt &shl);
	void WriteConstEdge(YsTextOutputStream &outStream,const YsShellExt &shl);
	void EndWriteObj(YsTextOutputStream &outStream,const YsShellExt &shl);

public:
	YSRESULT WriteObj(YsTextOutputStream &outStream,const YsShellExt &shl,const WriteOption &option);
};

class YsShellExtFMTOWNSWriter
{
public:
	class WriteOption
	{
	};

public:
	YSRESULT WriteFMTOWNST3D(FILE *ofp,const YsShellExt &shl,const WriteOption &opt);
};

class YsShellExtOffReader
{
private:
	enum
	{
		STATE_SKIPPING_OFF,
		STATE_GETTING_NUMBERS,
		STATE_READING_VERTEX,
		STATE_READING_POLYGON,
		STATE_NONE
	};
	YsArray <YsShellVertexHandle> vtHdArray;
	int state;
	YSSIZE_T nVtx,nPlg,nVtxRead,nPlgRead;

private:

public:
	/*! Begins reading .OFF file. */
	void BeginReadOff(void);

	/*! Read one line of .OFF file. */
	YSRESULT ReadOffOneLine(YsShellExt &shl,YsString &str);

	/*! End reading .OFF file. */
	void EndReadOff(YsShellExt &shl);

	/*! This function reads a .OFF file from a text-input stream.  Internally this uses BeginReadOff, EndReadOff, and ReadObnOneLine. */
	YSRESULT ReadOff(YsShellExt &shl,YsTextInputStream &inStream);
};



class YsShellExtOffWriter
{
public:
	YSRESULT WriteOff(YsTextOutputStream &outStream,const YsShellExt &shl);
};



class YsShellExtDxfReader
{
private:
	enum
	{
		STATE_NONE,
		STATE_3DFACE,
	};
	int state;
	int dataId;
	YSBOOL faceFilled[12];
	YsVec3 faceCache[4];
public:
	// Only support 3DFACE tags.  Text DXFs only.  Does binary DXF exist?

	/*! Begins reading .Dxf file. */
	void BeginReadDxf(void);

	/*! Read one line of .Dxf file. */
	YSRESULT ReadDxfOneLine(YsShellExt &shl,const YsString &str);

	/*! End reading .Dxf file. */
	void EndReadDxf(YsShellExt &shl);

	/*! This function reads a .Dxf file from a text-input stream.  Internally this uses BeginReadDxf, EndReadDxf, and ReadObnOneLine. */
	YSRESULT ReadDxf(YsShellExt &shl,YsTextInputStream &inStream);
};



/*! Convenience function for reading a file into shl.
    The file type will be identified by the file extension.
*/
YSRESULT YsShellExt_LoadGeneral(YsShellExt &SHL,const char FN[]);

/*! Convenience function for reading a file into shl.
    The file type will be identified by the file extension.
*/
YSRESULT YsShellExtEdit_LoadGeneral(YsShellExtEdit &SHL,const char FN[]);

/*! Convenience function for writing an shl to a file.
    The file type will be identified by the file extension.
*/
YSRESULT YsShellExt_SaveGeneral(const char FN[],const YsShellExt &SHL);


/* } */
#endif
