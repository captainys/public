#ifndef YSTEXTDATA_IS_INCLUDED
#define YSTEXTDATA_IS_INCLUDED

#include <stdio.h>
#include "ysdef.h"
#include "ysarray.h"

template <class STRTYPE>
class YsTextDataTemplate
{
private:
	YSEOLTYPE eolType;
	YsArray <STRTYPE> textData;
public:
	YsTextDataTemplate()
	{
		eolType=YSEOLTYPE_DEFAULT;
	}

	/*! Identify EOL-code type based on the input byte array.
	    YsArray <unsigned char> or std::vector <unsigned char> can be given as
	    byteArray.
	*/
	static YSEOLTYPE IdentifyEOLType(const YsConstArrayMask <unsigned char> &byteArray)
	{
		bool found0d=false,found0a=false;

		for(YSSIZE_T idx=0; idx<byteArray.size()-1; ++idx)
		{
			if(0x0d==byteArray[idx] && 0x0a==byteArray[idx+1])
			{
				printf("LINEBREAK==0x0d,0x0a\n");
				return YSEOLTYPE_CRLF;
			}
			if(0x0d==byteArray[idx])
			{
				found0d=true;
			}
			if(0x0a==byteArray[idx])
			{
				found0a=true;
			}
		}
		if(true==found0a)
		{
			printf("LINEBREAK==0x0a\n");
			return YSEOLTYPE_LF;
		}
		if(true==found0d)
		{
			printf("LINEBREAK==0x0d\n");
			return YSEOLTYPE_CR;
		}
		return YSEOLTYPE_DEFAULT;
	}

	/*! Identify EOL-code type based on the file pointer and the number of bytes
	    taken from the file.  The file must be opened with "rb" mode.
	    After this function, the file position will be moved forward by the number of bytes read.
	*/
	static YSEOLTYPE IdentifyEOLType(FILE *fp,YSSIZE_T numByte)
	{
		YSSIZE_T byteRead=0;
		YsArray <unsigned char> byteData;
		byteData.resize(numByte);
		byteRead=fread(byteData.data(),1,numByte,fp);
		byteData.resize(byteRead);
		return IdentifyEOLType(byteData);
	}

	/*! Identify EOL-code type based on the input byte array, and
	    store as the EOL-code for this text data.
	*/
	YSEOLTYPE SetEOLType(const YsConstArrayMask <unsigned char> &byteArray)
	{
		auto et=IdentifyEOLType(byteArray);
		SetEOLType(et);
		return et;
	}

	/*! Identify EOL-code type based on the input FILE pointer and the number of bytes 
	    taken from the file.  The file must be opened with "rb" mode.
	    After this function, the file position will be moved forward by the number of bytes read.
	*/
	YSEOLTYPE SetEOLType(FILE *fp,YSSIZE_T numByte)
	{
		auto et=IdentifyEOLType(fp,numByte);
		SetEOLType(et);
		return et;
	}

	/*! Set EOL-code type */
	void SetEOLType(YSEOLTYPE eolType)
	{
		this->eolType=eolType;
	}
};

#endif
