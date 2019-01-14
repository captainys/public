#ifndef FM7LIB_IS_INCLUDED
#define FM7LIB_IS_INCLUDED
/* { */

#include <vector>
#include <string>

class FM7File
{
public:
	enum
	{
		FTYPE_BASIC_BINARY,
		FTYPE_BASIC_ASCII,
		FTYPE_BINARY,
		FTYPE_DATA_BINARY, // I don't know if such a data type exists.  This constant is for more like a reservation in the future.  If the future exists!!
		FTYPE_DATA_ASCII,
		FTYPE_UNKNOWN
	};

	std::string fName;

	void CleanUp(void);
	static int DecodeFMHeaderFileType(unsigned char byte10,unsigned char byte11);
	static const char *FileTypeToString(int fType);

	/*! Returns a default FM-File extension for file type.
	*/
	static const char *GetDefaultFMFileExtensionForType(int fType);
};

class FM7BinaryFile : public FM7File
{
public:
	std::string fName;
	unsigned int storeAddr,execAddr;
	std::vector <unsigned char> dat;

	FM7BinaryFile();
	~FM7BinaryFile();
	void CleanUp(void);

	bool DecodeSREC(const std::vector <std::string> &srec);
	bool DecodeSREC(const char *const srec[]);
	bool Decode2B0(const std::vector <unsigned char> &dat,bool verbose=false);


	/*! Create a byte array for T77.
	    The differencefrom a byte array for D77 is it doesn't have a 0x1A in the end.
	*/
	std::vector <unsigned char> MakeByteArrayForT77(void) const;
};

////////////////////////////////////////////////////////////

class SRECDecoder
{
private:
	unsigned char dat[65536],used[65536];

public:
	unsigned int storeAddr,length,execAddr;

	SRECDecoder();
	bool Decode(const std::vector <std::string> &fileDat);
	bool Decode(const char *const fileDat[]);
	std::vector <unsigned char> data(void) const;

private:
	bool DecodeOneLine(const char str[]);
	bool PostProc(void);
	std::vector <unsigned char> GetByteData(const char str[]) const;
};

/* } */
#endif
