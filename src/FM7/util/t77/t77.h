#ifndef T77_IS_INCLUDED
#define T77_IS_INCLUDED
/* { */

#include <stdio.h>
#include <vector>
#include <string>

#include "../lib/fm7lib.h"

class T77File
{
public:
	std::vector <unsigned char> t77;

	void CleanUp(void);

	/*! This function is for appending a file to an existing .T77 file.
	    Read binary data of the existing .T77 file and give to this function.
	    This function cleans the data, and then copies (or moves) dat to its own array.
	*/
	void DumpT77(const std::vector <unsigned char> &dat);
	void DumpT77(std::vector <unsigned char> &&dat);

	class WAVOutOption
	{
	public:
		unsigned int Hz;
		unsigned int levelLow,levelHigh;
		unsigned int firstSilence;		// In Milliseconds
		unsigned int silenceAfterFile;	// In Milliseconds

		WAVOutOption();
	};
protected:
	class WAVOutDataBuffer
	{
	public:
		FILE *fp;
		long long int totalLength;
		long long int bufUsed;
		std::vector <unsigned char> buf;
		WAVOutDataBuffer();
		void CleanUp(void);
		bool Pump(unsigned char dat);
		void Rewind(void);
		bool Flush(void);
	};

public:
	/*! This function writes .WAV file of this T77 format data.
	*/
	bool WriteWav(const char fName[],const WAVOutOption &opt) const;
	bool WriteWav(FILE *fp,const WAVOutOption &opt) const;
protected:
	/*! Writes data to the file stream.  If fp==nullptr, it just counts number of bytes.
	*/
	long long int WriteData(WAVOutDataBuffer &dataBuf,long long int &ptr,const WAVOutOption &opt) const;

	long long int CalculateNumSampleForMillisecond(long long int milliSec,const WAVOutOption &opt) const;
};

class T77Encoder : public T77File
{
public:
	bool debug_makeIntentionalCheckSumError_headerBlock;
	bool debug_makeIntentionalCheckSumError_dataBlock;
	bool debug_mixNonFFinDataGap;

	T77Encoder();
	~T77Encoder();
	void CleanUp(void);

	unsigned char CalculateCheckSum(const std::vector <unsigned char> &blk);
	std::vector <unsigned char> MakeHeaderBlock(const char t77fName[],int fType);
	std::vector <unsigned char> MakeDataBlock(const std::vector <unsigned char> &dat,long long int ptr,long long int nByte);
	std::vector <unsigned char> MakeTerminalBlock(void);
	void StartT77Header(void);
	void AddBit(unsigned int bitData);
	void AddByte(unsigned char byteData);
	void AddGapBetweenFile(void);

	bool Encode(int fType,const char t77fName[],const std::vector <unsigned char> &dat);
	bool Encode(int fType,const char t77fName[],const FM7BinaryFile &dat);
	bool EncodeFromFMFile(const char t77fName[],const std::vector <unsigned char> &dat);
};

class T77Decoder : public T77File
{
public:
	unsigned int OneZeroThr;
	// By default, 0x1A+0x1A=0x34 (Off bit), 0x30+0x30=0x60 (On bit).
	// To take in the middle, it should be somewhere like 0x48.
	std::vector <long long int> filePtr;  // Index to T77File::t77.
	std::vector <std::vector <unsigned char> > fileDump;

public:
	T77Decoder();

	long long int SkipLead(long long int ptr) const;
	int GetBit(long long int &ptr) const;
	bool FindFirstFFFFFF(long long int &ptr) const;
	bool Decode(void);



	class RawDecodingInfo
	{
	public:
		bool endOfFile;
		long long int ptr;
		long long int byteCtr;
		int byteData;
	};
	RawDecodingInfo BeginRawDecoding(void) const;
	RawDecodingInfo RawReadByte(RawDecodingInfo info) const;


	static long long int GetHeaderBlockPointer(const std::vector <unsigned char> &dump);
	static std::string GetDumpFileName(const std::vector <unsigned char> &dump);
	static std::vector <unsigned char> DumpToFMFormat(const std::vector <unsigned char> &dump);
};

/* } */
#endif
