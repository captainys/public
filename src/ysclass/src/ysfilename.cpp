/* ////////////////////////////////////////////////////////////

File Name: ysfilename.cpp
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

#include <stdio.h>
#include <string.h>

#include "ysdef.h"
#include "ysfilename.h"
#include "yslist.h"

#include "ysbase.h"
#include "ysclassmgr.h"

#include "ysmath.h"
#include "ysarray.h"
#include "yslist2.h"
#include "ysstring.h"

#include "ysexception.h"

YSFTYPE YsIdentifyFileType(const char fn[])
{
	int i;
	YsString ext;

	for(i=(int)strlen(fn)-1; i>=0; i--)
	{
		if(fn[i]=='/' || fn[i]=='\\' || fn[i]==':')
		{
			return YSFTYPE_UNKNOWN;
		}
		else if(fn[i]=='.')
		{
			ext.Set(fn+i+1);
			break;
		}
	}

	ext.Capitalize();

	if(strcmp(ext,"SRF")==0)
	{
		return YSFTYPE_SRF;
	}
	else if(strcmp(ext,"SRM")==0)
	{
		return YSFTYPE_SRM;
	}
	else if(strcmp(ext,"DNM")==0)
	{
		return YSFTYPE_DNM;
	}
	else if(strcmp(ext,"FLD")==0)
	{
		return YSFTYPE_FLD;
	}
	else if(strcmp(ext,"STP")==0)
	{
		return YSFTYPE_STP;
	}
	else if(strcmp(ext,"YFS")==0)
	{
		return YSFTYPE_YFS;
	}
	else if(strcmp(ext,"MSH")==0)
	{
		return YSFTYPE_MSH;
	}
	else if(strcmp(ext,"MS3")==0)
	{
		return YSFTYPE_MS3;
	}
	else if(strcmp(ext,"STL")==0)
	{
		return YSFTYPE_STL;
	}
	else if(strcmp(ext,"BMP")==0)
	{
		return YSFTYPE_BMP;
	}
	else if(strcmp(ext,"PNG")==0)
	{
		return YSFTYPE_PNG;
	}
	else if(strcmp(ext,"S2D")==0)
	{
		return YSFTYPE_S2D;
	}
	else if(strcmp(ext,"PAR")==0)
	{
		return YSFTYPE_PAR;
	}
	else if(strcmp(ext,"NAS")==0)
	{
		return YSFTYPE_NAS;
	}
	else if(strcmp(ext,"HGC")==0)
	{
		return YSFTYPE_HGC;
	}
	else if(strcmp(ext,"HBS")==0)
	{
		return YSFTYPE_HBS;
	}
	else if(strcmp(ext,"TSHL")==0)
	{
		return YSFTYPE_TSHL;
	}
	else if(strcmp(ext,"SHL")==0)
	{
		return YSFTYPE_SHL;
	}
	else if(strcmp(ext,"OBJ")==0)
	{
		return YSFTYPE_OBJ;
	}
	else if(strcmp(ext,"OFF")==0)
	{
		return YSFTYPE_OFF;
	}

	return YSFTYPE_UNKNOWN;
}

YSFTYPE YsIdentifyFileType(const wchar_t fn[])
{
	YsString str;
	str.EncodeUTF8(fn);
	return YsIdentifyFileType(str.Txt());
}

void YsPutExt(char *fname,const char *ext)
{
	int i;

	for(i=0; fname[i]!=0; i++)
	{
		if(fname[i]=='.')
		{
			return;
		}
	}

	if(*ext == '*')
	{
		ext++;
	}
	if(*ext == '.')
	{
		ext++;
	}

	sprintf(fname,"%s.%s",fname,ext);
}

unsigned long YsFileSize(const char *fname)
{
	FILE *fp;
    unsigned long edptr;

	fp=fopen(fname,"rb");
	if(fp==NULL)
	{
		return 0;
	}

    fseek(fp,0,2 /* SEEK_END */);
    edptr=ftell(fp);

	fclose(fp);

    return edptr;
}

YSBOOL YsFileExist(const char *fn)
{
	FILE *fp;
	fp=fopen(fn,"rb");

	if(fp!=NULL)
	{
		fclose(fp);
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

YSRESULT YsSeparatePathFile(char *pth,char *fil,const char *org)
{
	char tmp[256],*seekPtr,*cutPtr;

	strcpy(tmp,org);
	cutPtr=tmp;

	/* Ysip Drive Name */
	if(tmp[1]==':')
	{
		cutPtr+=2;
	}

	/* scan until nul Stopper */
	for(seekPtr=cutPtr; *seekPtr!=0; seekPtr++)
	{
		if(*seekPtr=='\\' || *seekPtr=='/' || *seekPtr==':')
		{
			cutPtr=seekPtr+1;
		}
	}

	/* cutPtr points *tmp  or after ':' or after last '\\' */
	strcpy(fil,cutPtr);

	*cutPtr = 0;
	strcpy(pth,tmp);
	return YSOK;
}

void YsDeleteExtension(char def[])
{
	int i;
	for(i=(int)strlen(def)-1; i>=0; i--)
	{
		if(def[i]=='/' || def[i]=='\\')
		{
			break;
		}
		else if(def[i]=='.')
		{
			def[i]=0;
			break;
		}
	}
}

void YsReplaceExtension(char fn[],const char ext[])  // ext can be "*.???",".???" or "???"
{
	int offset;
	offset=0;
	while(ext[offset]=='.' || ext[offset]=='*')
	{
		offset++;  // Skip first "*.", if it is given.
	}

	YsDeleteExtension(fn);

	if(fn[0]!=0)
	{
		strcat(fn,".");
		strcat(fn,ext+offset);
	}
}


////////////////////////////////////////////////////////////

YsBufferedFileReader::YsBufferedFileReader(YSSIZE_T size)
{
	fp=NULL;
	nBufFilled=0;
	nBufUsed=0;
	buf=new unsigned char [size];
	if(buf!=NULL)
	{
		bufSize=size;

		filePtrLWord=0;
		filePtrHWord=0;
	}
	else
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsBufferedFileReader::YsBufferedFileReader()");
	}
}

YsBufferedFileReader::~YsBufferedFileReader()
{
	if(fp!=NULL)
	{
		fclose(fp);
	}
	if(buf!=NULL)
	{
		delete [] buf;
	}
}

YSRESULT YsBufferedFileReader::SetBufferSize(YSSIZE_T size)
{
	if(nBufFilled==0)
	{
		if(buf!=NULL)
		{
			delete [] buf;
		}
		buf=new unsigned char [size];
		if(buf!=NULL)
		{
			bufSize=size;
			return YSOK;
		}
		else
		{
			YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsBufferedFileReader::SetBufferSize()");
		}
	}
	return YSERR;
}

YSRESULT YsBufferedFileReader::Open(const char fn[])
{
	fp=fopen(fn,"rb");
	if(fp!=NULL)
	{
		nBufFilled=0;
		nBufUsed=0;

		filePtrHWord=0;
		filePtrLWord=0;

		return YSOK;
	}
	return YSERR;
}

YSSIZE_T YsBufferedFileReader::Read(void *readBufPtr,YSSIZE_T readSize)
{
	YSSIZE_T nReturned;
	unsigned char *readBuf;
	readBuf=(unsigned char *)readBufPtr;
	nReturned=0;
	while(nReturned<readSize)
	{
		if(nBufFilled<=nBufUsed)
		{
			nBufFilled=fread(buf,1,bufSize,fp);
			nBufUsed=0;
			if(nBufFilled==0)
			{
				return nReturned;
			}
		}

		while(nBufUsed<nBufFilled && nReturned<readSize)
		{
			readBuf[nReturned++]=buf[nBufUsed++];
			filePtrLWord++;
			if(filePtrLWord==0)
			{
				filePtrHWord++;
			}
		}
	}
	return nReturned;
}

YSRESULT YsBufferedFileReader::Close(void)
{
	if(fp!=NULL)
	{
		fclose(fp);
		nBufFilled=0;
		nBufUsed=0;
		return YSOK;
	}
	return YSERR;
}

YSSIZE_T YsBufferedFileReader::GetFilePtrKB(void) const
{
	return (filePtrLWord>>10)+(filePtrHWord<<22);

}

////////////////////////////////////////////////////////////

YsBufferedFileWriter::YsBufferedFileWriter(YSSIZE_T size)
{
	fp=NULL;
	bufSize=size;
	nBufFilled=0;
	buf=new unsigned char [size];
	if(buf==NULL)
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsBufferedFileWriter::YsBufferedFileWriter()");
	}
}

YsBufferedFileWriter::~YsBufferedFileWriter()
{
	if(fp!=NULL)
	{
		Close();
	}
	if(buf!=NULL)
	{
		delete [] buf;
	}
}

YSRESULT YsBufferedFileWriter::SetBufferSize(YSSIZE_T size)
{
	if(nBufFilled==0)
	{
		delete [] buf;
		buf=new unsigned char [size];
		if(buf!=NULL)
		{
			bufSize=size;
			return YSOK;
		}
		else
		{
			YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsBufferedFileWriter::SetBufferSize()");
		}
	}
	return YSERR;
}

YSRESULT YsBufferedFileWriter::Open(const char fn[])
{
	if(fp!=NULL)
	{
		Close();
	}

	fp=fopen(fn,"wb");
	if(fp!=NULL)
	{
		nBufFilled=0;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsBufferedFileWriter::Write(void *datPtr,YSSIZE_T writeSize)
{
	YSSIZE_T i,actualWriteSize;
	unsigned char *dat;
	dat=(unsigned char *)datPtr;

	for(i=0; i<writeSize; i++)
	{
		buf[nBufFilled++]=dat[i];
		if(bufSize<=nBufFilled)
		{
			actualWriteSize=fwrite(buf,1,nBufFilled,fp);
			if(actualWriteSize!=nBufFilled)
			{
				return YSERR;
			}
			nBufFilled=0;
		}
	}
	return YSOK;
}

YSRESULT YsBufferedFileWriter::Close(void)
{
	if(fp!=NULL)
	{
		if(0<nBufFilled)
		{
			fwrite(buf,1,nBufFilled,fp);
		}
		fclose(fp);
		fp=NULL;
		nBufFilled=0;
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

const char *YsGetExtensionFromFileType(YSFTYPE fType)
{
	switch(fType)
	{
	default:
	case YSFTYPE_UNKNOWN:
		return "";
	case YSFTYPE_SRF:
	case YSFTYPE_SRM:
		return ".srf";
	case YSFTYPE_DNM:
		return ".dnm";
	case YSFTYPE_FLD:
		return ".fld";
	case YSFTYPE_STP:
		return ".stp";
	case YSFTYPE_YFS:
		return ".yfs";
	case YSFTYPE_MSH:
		return ".msh";
	case YSFTYPE_MS3:
		return ".ms3";
	case YSFTYPE_STL:
		return ".stl";
	case YSFTYPE_BMP:
		return ".bmp";
	case YSFTYPE_PNG:
		return ".png";
	case YSFTYPE_S2D:
		return ".s2d";
	case YSFTYPE_PAR:
		return ".par";
	case YSFTYPE_NAS:
		return ".nas";
	case YSFTYPE_ABAQUSINP:
	case YSFTYPE_STARCDINP:
		return ".inp";
	case YSFTYPE_HGC:
		return ".hgc";
	case YSFTYPE_HBS:
		return ".hbs";
	case YSFTYPE_SHL:
		return ".shl";
	case YSFTYPE_TSHL:
		return ".tshl";
	case YSFTYPE_OBJ:
		return ".obj";
	}
}

const wchar_t *YsGetWExtensionFromFileType(YSFTYPE fType)
{
	switch(fType)
	{
	default:
	case YSFTYPE_UNKNOWN:
		return L"";
	case YSFTYPE_SRF:
	case YSFTYPE_SRM:
		return L".srf";
	case YSFTYPE_DNM:
		return L".dnm";
	case YSFTYPE_FLD:
		return L".fld";
	case YSFTYPE_STP:
		return L".stp";
	case YSFTYPE_YFS:
		return L".yfs";
	case YSFTYPE_MSH:
		return L".msh";
	case YSFTYPE_MS3:
		return L".ms3";
	case YSFTYPE_STL:
		return L".stl";
	case YSFTYPE_BMP:
		return L".bmp";
	case YSFTYPE_PNG:
		return L".png";
	case YSFTYPE_S2D:
		return L".s2d";
	case YSFTYPE_PAR:
		return L".par";
	case YSFTYPE_NAS:
		return L".nas";
	case YSFTYPE_ABAQUSINP:
	case YSFTYPE_STARCDINP:
		return L".inp";
	case YSFTYPE_HGC:
		return L".hgc";
	case YSFTYPE_HBS:
		return L".hbs";
	case YSFTYPE_SHL:
		return L".shl";
	case YSFTYPE_TSHL:
		return L".tshl";
	case YSFTYPE_OBJ:
		return L".obj";
	}
}

YSFTYPE YsGetFileTypeFromExtension(const char ext[])
{
	YsString capExt(ext);
	if('.'==capExt[0])
	{
		capExt.Delete(0);
	}
	capExt.Uncapitalize();

	if(0==strcmp(capExt,"srf"))
	{
		return YSFTYPE_SRF;
	}
	else if(0==strcmp(capExt,"dnm"))
	{
		return YSFTYPE_DNM;
	}
	else if(0==strcmp(capExt,"fld"))
	{
		return YSFTYPE_FLD;
	}
	else if(0==strcmp(capExt,"stp"))
	{
		return YSFTYPE_STP;
	}
	else if(0==strcmp(capExt,"yfs"))
	{
		return YSFTYPE_YFS;
	}
	else if(0==strcmp(capExt,"msh"))
	{
		return YSFTYPE_MSH;
	}
	else if(0==strcmp(capExt,"ms3"))
	{
		return YSFTYPE_MS3;
	}
	else if(0==strcmp(capExt,"stl"))
	{
		return YSFTYPE_STL;
	}
	else if(0==strcmp(capExt,"bmp"))
	{
		return YSFTYPE_BMP;
	}
	else if(0==strcmp(capExt,"png"))
	{
		return YSFTYPE_PNG;
	}
	else if(0==strcmp(capExt,"s2d"))
	{
		return YSFTYPE_S2D;
	}
	else if(0==strcmp(capExt,"par"))
	{
		return YSFTYPE_PAR;
	}
	else if(0==strcmp(capExt,"nas"))
	{
		return YSFTYPE_NAS;
	}
	else if(0==strcmp(capExt,"inp"))
	{
		return YSFTYPE_ABAQUSINP;
	}
	else if(0==strcmp(capExt,"hgc"))
	{
		return YSFTYPE_HGC;
	}
	else if(0==strcmp(capExt,"hbs"))
	{
		return YSFTYPE_HBS;
	}
	else if(0==strcmp(capExt,"shl"))
	{
		return YSFTYPE_SHL;
	}
	else if(0==strcmp(capExt,"tshl"))
	{
		return YSFTYPE_TSHL;
	}
	else if(0==strcmp(capExt,"obj"))
	{
		return YSFTYPE_OBJ;
	}
	return YSFTYPE_UNKNOWN;
}

YSFTYPE YsGetFileTypeFromExtension(const wchar_t ext[])
{
	YsString str;
	str.EncodeUTF8(ext);
	return YsGetFileTypeFromExtension(str);
}


