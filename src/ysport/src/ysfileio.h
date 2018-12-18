/* ////////////////////////////////////////////////////////////

File Name: ysfileio.h
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

#ifndef YSFILEMISC_IS_INCLUDED
#define YSFILEMISC_IS_INCLUDED
/* { */

#include <cstddef>
#include <ysclass.h>
#include <stdio.h>


/*
  2016/01/08
  YsFileIO as C-File abstraction was totally my bad.  I added YsFileIO::File class for that purpose.
  Hope it doesn't affect too many users.  
*/

/*! This class provides a cover for file-IO functions. 
    Main purpose is to cover the difference of fopen between Windows and Unix-based systems.
    Windows' fopen takes a enconding that depends on the regional settings, while 
    Unix-based systems a UTF-8 enconding (apparently.)  Windows opens a Unicode file name with 
    wfopen instead. 

    This difference causes a problem when I want to write a portable program that can open 
    a Unicode file name.  YsFileIO::Fopen opens a file with a UTF-8 file name or a 
    Unicode file name in wchar_t string.

    Also getcwd, chdir, and mkdir functions are for whatever reason defined in a different 
    header file in Windows and Unix-based systems.  Also Windows' directory functions are 
    influenced by regional settings.  The member functions hide of these differences as well.

	In Windows, an incoming 8-bit (char) file name is considered as UTF-8 string, and converted 
	to wchar_t string and then given to the Windows-only functions.

    In Unix-based systems, an incoming 16-bit (wchar_t) file name is converted to UTF-8 string
    and then given to regular C functions.
*/
class YsFileIO
{
public:
	/*! Returns the current working directory.
	*/
	static const char *Getcwd(YsString &cwd);
	static const wchar_t *Getcwd(YsWString &cwd);
	static YsWString Getcwd(void);

	/*! Change the current working directory.
	*/
	static YSRESULT ChDir(const char dirName[]);
	static YSRESULT ChDir(const wchar_t dirName[]);


	/*! Class to preserve the current working directory in a control block.
	*/
	class Pushd
	{
	private:
		YsWString cwd;
	public:
		Pushd()
		{
			cwd=Getcwd();
		}
		Pushd(const wchar_t dir[])
		{
			cwd=Getcwd();
			ChDir(dir);
		}
		Pushd(const char dir[])
		{
			cwd=Getcwd();
			ChDir(dir);
		}
		~Pushd()
		{
			Popd();
		}
		void Popd(void)
		{
			ChDir(cwd.data());
		}
	};


	/*! Make a directory.
	*/
	static YSRESULT MkDir(const char dirName[]);
	static YSRESULT MkDir(const wchar_t dirName[]);

	/*! Open a file.
	*/
	static FILE *Fopen(const wchar_t wfn[],const char mode[]);
	static FILE *Fopen(const char fn[],const char mode[]);

	/*! Remove a file.
	*/
	static YSRESULT Remove(const wchar_t fn[]);
	static YSRESULT Remove(const char fn[]);

	/*! Returns YSTRUE if the file exists, YSFALSE otherwise.
	*/
	static YSBOOL CheckFileExist(const wchar_t fn[]);
	static YSBOOL CheckFileExist(const char fn[]);

	/*! Returns the real full-path name of the file.
	    In Unix-based systems it uses realpath() function.
	    In Windows it uses GetFullPathName() function.
		If there is an error, it returns the same string as the fn.
	*/
	static YsWString GetRealPath(const wchar_t fn[]);

	/*! This class covers FILE pointer of C-standard library.
	    This class owns a file pointer, which is closed in the destructor.
	    Therefore, copy is not allowed.  However, moving is allowed.

	    A variable of this class can be given in place for FILE *.

	    The using program is not required to explicitly close a file, but it is possible by Fclose() function.

		The integrity of this class can be broken if it is given to fclose.  Don't do it.
	 */
	class File
	{
	private:
		FILE *fp;
		YsString mode;

		File(const File &);
		File &operator=(const File &);
		void MoveFrom(File &incoming);
	public:
		/*! Default constructor. */
		File();

		/*! A constructor that opens a file.  The second parameter, mode, is compatible with fopen of C-standard library.
		 */
		File(const wchar_t fn[],const char mode[]);

		/*! A constructor that opens a file.  The second parameter, mode, is compatible with fopen of C-standard library.
		 */
		File(const char fn[],const char mode[]);

		/*! Move constructor.
		 */
		File(File &&incoming);

		/*! Move operator.
		 */
		File &operator=(File &&incoming);

		/*! Destructor.  It automatically close a file if one is open.
		 */
		virtual ~File();

		/*! Cast operator that allows a variable of this type be casted in place for FILE *.
		 */
		inline operator FILE *() const
		{
			return fp;
		}
		/*! Returns a text-input stream.
		 */
		inline YsTextFileInputStream InStream(void) const
		{
			return YsTextFileInputStream(fp);
		}

		/*! Returns a text-output stream.
		 */
		inline YsTextFileOutputStream OutStream(void) const
		{
			return YsTextFileOutputStream(fp);
		}
		/*! This object can be compared against nullptr to check if the file is successfully opened.
		 */
		inline bool operator==(std::nullptr_t)
		{
			return (fp==nullptr);
		}
		/*! This object can be compared against nullptr to check if the file is successfully opened.
		 */
		inline bool operator!=(std::nullptr_t)
		{
			return (fp!=nullptr);
		}

		/*! Opens a file.  If the file is already open, it does nothing and returns nullptr.
		    If successful it returns this pointer. */
		File *Fopen(const wchar_t fn[],const char mode[]);
		File *Fopen(const char fn[],const char mode[]);

		/*! Explicitly closes a file.
		 */
		void Fclose(void);

		/*! Alias for Fclose() 
		 */
		inline void Close(void)
		{
			Fclose();
		}

		/*! Returns an internally stored file pointer.
		 */
		FILE *Fp(void) const;


		/*! Returns a raw read from file.  Must be opened as "rb" */
		YsArray <unsigned char> Fread(void);

		/*! Writes a byte-array to the file.  Must be opened as "wb" */
		YSRESULT Fwrite(YsConstArrayMask <unsigned char> byteArray);

		/*! Reads a text in an array of YsStrings. */
		YsArray <YsString> ReadText(void);

		/*! Writes an array of YsStrings. */
		YSRESULT WriteText(const YsConstArrayMask <YsString> txt);
	};
};

inline bool operator==(std::nullptr_t,const YsFileIO::File &fp)
{
	return (fp==nullptr);
}
inline bool operator!=(std::nullptr_t,const YsFileIO::File &fp)
{
	return (fp!=nullptr);
}

/* } */
#endif
