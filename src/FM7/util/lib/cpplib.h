#ifndef CPPLIB_IS_INCLUDED
#define CPPLIB_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include <stdio.h>

namespace FM7Lib
{

/*! Convert a hexa-decimal number described in str to an integer.
*/
int Xtoi(const char str[]);

/*! Convert a number described in str to an integer.
    If the number starts with "0x", "$", or "&H" it is taken as a hexa-decimal number.
    If the number ends with "H" it is taken as a hexa-decimal number.
*/
int Atoi(const char str[]);


/*! Convert an unsigned integer to Intel-byteorder binary.
    I hope people didn't come up with a confusing name called Little Endian and Big Endian.
    Just say MSB last or MSB first!!!!!
*/
void UIntToIntelByteOrder(unsigned char bin[4],unsigned int i);
void UShortToIntelByteOrder(unsigned char bin[2],unsigned short i);


std::vector <unsigned char> ReadBinaryFile(const char fn[]);
std::vector <unsigned char> ReadBinaryFile(FILE *fp);

bool WriteBinaryFile(const char fn[],const std::vector <unsigned char> &dat);
bool WriteBinaryFile(FILE *fp,const std::vector <unsigned char> &dat);

std::vector <std::string> ReadTextFile(const char fn[]);
std::vector <std::string> ReadTextFile(FILE *fp);

bool WriteTextFile(const char fn[],const std::vector <std::string> &text);
bool WriteTextFile(FILE *fp,const std::vector <std::string> &text);

std::vector <unsigned char> TextToByteArray(const std::vector <std::string> &text,const char lineBreak[]);


/*! Splits string with ' ' and '\t' and makes an array of arguments.
*/
std::vector <std::string> QuickParser(const char str[]);


/*! Turn small letters into capital letters.
*/
void Capitalize(std::string &s);


/*!  Convert '0' through '9' to 0 through 9
             'A' through 'F' to 10 to 15
             'a' through 'f' to 10 to 15
*/
int LetterToNum(char c);


/*! Returns an extension of the file name.
    The returned string will include '.' if it has an extension.
*/
std::string GetExtension(const char fName[]);


/*! Changes an extension of a file name.
*/
std::string ChangeExtension(const char orgFName[],const char newExt[]);



/*! Removes an extension of a file name. 
*/
std::string RemoveExtension(const char orgFName[]);



/*! Returns a base name.
    If input is "c:\users\soji\nicefile.dat", "nicefile.dat" will be returned.
*/
std::string GetBaseName(const char path[]);



/*! Check if the file exists.
*/
bool FileExist(const char fn[]);



/*! Check if the string starts with the pattern. 
*/
bool StrStartsWith(const std::string &str,const char ptn[]);



/*! Skip the given pattern and return the pointer if there is a match.  Or, it returns nullptr otherwise. 
*/
const char *StrSkip(const char str[],const char ptn[]);



/*! Skip space and tab and return the pointer.  This function will not return nullptr.
    If it reaches the end of the string, it returns a pointer to an empty string.
*/
const char *StrSkipSpace(const char str[]);



/*! Skip non-space (not space and not tab) and return the pointer.  This function will not return nullptr.
    If it reaches the end of the string, it returns a pointer to an empty string.
*/
const char *StrSkipNonSpace(const char str[]);



/*! Make a FM-FILE header. */
std::vector <unsigned char> MakeFMFileHeader(const char filenameInD77[],unsigned char byte10,unsigned char byte11);



/*! Create FM-FILE (0A0) format from a text.
*/
std::vector <unsigned char> TextTo0A0(const std::vector <std::string> &text,const char filenameInD77[]);



/*! Create FM-FILE (0A0) format from a text.
    Text must be nullptr-terminated.
*/
std::vector <unsigned char> TextTo0A0(const char * const text[],const char filenameInD77[]);



/*! Create FM-FILE (2B0) format from a binary.
*/
std::vector <unsigned char> BinaryTo2B0(const std::vector <unsigned char> &dat,const char filenameInD77[],int storeAddr,int execAddr);



/*! Create a image (raw data stored in the media) from a binary.
*/
std::vector <unsigned char> BinaryToStoreImage(const std::vector <unsigned char> &dat,int storeAddr,int execAddr);



/*! Extract file-name part from a FM-FILE.
*/
std::string GetFileNameFromFMFile(const std::vector <unsigned char> &dat);



/*! Interpret raw text of hexadecimal numbers into array of unsigned chars.
    This function ignores spaces, tabs, and non-hexadecimal characters.
*/
std::vector <unsigned char> RawHexToByteData(const std::vector <std::string> &rawHexTxt);


const char *BoolToStr(bool b);


/*! Convert a string to a std::vector of unsigned chars.
    If the string starts with ' or ", it is taken as an ASCII string.
    Otherwise it is taken as a hexa-decimal numbers.

	818A9B9C -> {0x81,0x8A,0x9B,0x9C}
	"ABCDEF" -> {'A','B','C','D','E','F'}
*/
std::vector <unsigned char> StrToByteArray(const char str[]);


} // namespace FM7Lib

/* } */
#endif
