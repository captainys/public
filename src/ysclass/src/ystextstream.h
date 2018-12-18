/* ////////////////////////////////////////////////////////////

File Name: ystextstream.h
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

#ifndef YSTEXTSTREAM_IS_INCLUDED
#define YSTEXTSTREAM_IS_INCLUDED
/* { */

#include "ysstring.h"
#include "yslist2.h"
#include "yssegarray.h"

/*! Defines a base class for text input/output stream classes.
*/
class YsTextStream
{
};

/*! Defines a base class for text-input stream classes.

    Gets and Fgets are seemingly read-only functions, and may feel like they must 
    be const, but these actually moves the file pointer.  
    It is correct to make these non-const function, I think.

    After using this class a while, I realized that I always mix up Fgets and Gets.
    I have decided to add an alias function Fgets to reduce waste of time for
    hunting my mix-up.
*/
class YsTextInputStream : public YsTextStream
{
protected:
	/*! This variable is set to YSFALSE in the constructor.
	    A sub-class must set this to YSTRUE when the stream reaches the end.
	*/
	YSBOOL endOfFile;
public:
	YsTextInputStream();

	/*! This function returns YSTRUE if the stream reaches the end.
	    Usage can be:
	        YsTextInputStream inStream(fp);
	        while(auto s=inStream.Gets())
	        {
	            if(YSTRUE==inStream.EndOfFile())
	            {
	                break;
	            }
	            // Do something with the string.
	        }
	*/
	YSBOOL EndOfFile(void) const;

	/*! Read one line from he stream.
	*/
	YsString Gets(void);

	/*! Read one line from he stream.  (An alias for Gets)
	*/
	YsString Fgets(void);

	/*! Read one line from the stream and returns it in str.
	    This function must be implemented in a sub-class.
	    When the stream reaches the end, this function must return nullptr.
	    Otherwise, this function must (const char *)str.
	*/
	virtual const char *Gets(YsString &str)=0;
};

/*! Defines a base class for text-output stream classes.
*/
class YsTextOutputStream : public YsTextStream
{
public:
	/*! The sub-class must implement this function so that it writes a string to the stream.
	*/
	virtual YSRESULT Puts(const char str[])=0;
	/*! The sub-class must implement this function so that it writes a string to the stream.
	*/
	virtual YSRESULT Puts(const YsString &str)=0;
	/*! The sub-class must implement this function so that it writes a string to the stream.
	*/
	virtual YSRESULT Printf(const char fmt[],...)=0;
};


/*! Defines a text-input stream from a file.
    It takes a file pointer as a constructor parameter.
    This class does not automatically close the file pointer in the destructor.
    The file pointer must be closed outside this class.

*/
class YsTextFileInputStream : public YsTextInputStream
{
private:
	FILE *fp;
public:
	/*! Constructor.  It takes a file pointer as an input.
	    It is not allowed to construct a class without a file pointer that is open with text-read mode ("r").
	*/
	YsTextFileInputStream(FILE *fp);

	/*! Copy constructor.  
	    Since this class does not own a file pointer, it is safe to copy.
	    The user must make sure that fp outlives this object. */
	YsTextFileInputStream(const YsTextFileInputStream &incoming);

	/*! Copy operator. 
	    Since this class does not own a file pointer, it is safe to copy.
	    The user must make sure that fp outlives this object. */
	YsTextFileInputStream &operator=(const YsTextFileInputStream &);

public:
	/*! Implementation of YsTextInputStream::Gets.
	*/
	const char *Gets(YsString &str);
};

/*! Defines a text-output stream from a file.
    It takes a file pointer as a constructor parameter.
    This class does not automatically close the file pointer in the destructor.
    The file pointer must be closed outside this class.

    2016/10/08  It's a shame that I was forgetting to add Move-constructor and Move-operator.
    That explains why I could not give a file stream as a parameter to some functions.
*/
class YsTextFileOutputStream : public YsTextOutputStream
{
private:
	FILE *fp;
public:
	/*! Constructor.  It takes a file pointer as an input.
	    It is not allowed to construct a class without a file pointer that is open with text-write mode ("w").
	*/
	YsTextFileOutputStream(FILE *fp);

	/*! Copy constructor.
	    Since this class does not own a file pointer, it is safe to copy.
	    The user must make sure that fp outlives this object. */
	YsTextFileOutputStream(const YsTextFileOutputStream &);

	/*! Copy operator.
	    Since this class does not own a file pointer, it is safe to copy.
	    The user must make sure that fp outlives this object. */
	YsTextFileOutputStream &operator=(const YsTextFileOutputStream &);

public:
	/*! Write a string to the stream.
	*/
	YSRESULT Puts(const char str[]);

	/*! Write a string to the stream.
	*/
	YSRESULT Puts(const YsString &str);

	/*! Printf to the stream.
	*/
	YSRESULT Printf(const char fmt[],...);
};




class YsTextMemoryOutputStream : public YsTextOutputStream, public YsSegmentedArray <YsString,8>
{
private:
	YSSIZE_T curLineIdx;

	YsString &GetCurrentLine(void);
	YsString &NewLine(void);

public:
	YsTextMemoryOutputStream(void);
	void CleanUp(void);
	YSRESULT Puts(const char str[]);
	YSRESULT Puts(const YsString &str);
	YSRESULT Printf(const char fmt[],...);

	const YSSIZE_T GetNumLine(void) const;
};


/*! Text-input stream from an array of strings.  The array of strings must be in a form of:
       const char *const inStr[]=
       {
           "First line",
           "Second line",
           "Third line",
               :
           nullptr
       };
	The array must be terminated by a nullptr.

	Or, it can be an array of YsStrings.

	In either case, this class does not take an ownership of the array of strings.  Therefore,
	this class must not outlive the incoming strings.  The using program must make sure the
	strings are valid until this object is deleted.
*/
class YsTextMemoryInputStream : public YsTextInputStream
{
private:
	enum MODE
	{
		MODE_NONE,
		MODE_CSTRING_ARRAY,
		MODE_YSSTRING_ARRAY
	};

	MODE mode;
	YSSIZE_T ptr;
	const char *const *inStr;

	YSSIZE_T nYsStr;
	const YsString *inYsStr;

public:
	/*! Create from a NULL-terminated array of C-strings. */
	YsTextMemoryInputStream(const char *const inStr[]);

	/*! Create from an array of YsString s. */
	YsTextMemoryInputStream(const YsConstArrayMask <YsString> &inStr);

	/*! Implementation of YsTextInputStream::Gets.
	*/
	virtual const char *Gets(YsString &str);
};


/* } */
#endif
