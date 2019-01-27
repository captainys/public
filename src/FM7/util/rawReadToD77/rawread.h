#ifndef RAWREAD_IS_INCLUDED
#define RAWREAD_IS_INCLUDED
/* { */


#include <string>
#include <sstream>
#include <vector>

#include "d77.h"



class FM7RawDiskRead
{
public:
	enum
	{
		DISKERR_NOTREADY=10,
		DISKERR_WRITEPROTECTED=11,
		DISKERR_HARDERROR=12,	// SEEK ERROR, LOST DATA, RECORD NOT FOUND
		DISKERR_CRCERROR=13,
		DISKERR_DDMARK=14,
		DISKERR_TIMEOVER=15,
	};
	enum
	{
		FDCSTATE_BUSY=0x01,
		FDCSTATE_DRQ=0x02,
		FDCSTATE_LOSTDATA=0x04,
		FDCSTATE_CRCERROR=0x08,
		FDCSTATE_RECORDNOTFOUND=0x10,
		FDCSTATE_DELETEDDATA=0x20,
		FDCSTATE_WRITEPROTECTED=0x40,
		FDCSTATE_NOTREADY=0x80,
	};

	class Sector
	{
	public:
		int C,H,R,N;
		int errCode;
		int fdcState;
		std::vector <unsigned char> dat;

		/*! Returns the data size based on S of CHNS.
		    Which is (128<<s).
		*/
		unsigned long long int CalculateSize(void) const;

		/*! Returns a D77 library sector.
		    nSectorTrack and density cannot be calculated from sector info itself,
		    therefore need to be given as parameters.
			Double density=0x00,  Single density=0x40
		*/
		D77File::D77Disk::D77Sector MakeD77Sector(long long int nSectorTrack,int density) const;
	};


	class HasData
	{
	public:
		long long int mustBeSize;
		std::vector <unsigned char> dat;
		HasData()
		{
			mustBeSize=0;
		}
		unsigned int CalculateChekcSum(void) const;
		unsigned int CalculateXor(void) const;
	};
	class HasCheckSum : public HasData
	{
	public:
		unsigned int mustBeCheckSum,mustBeXor;
		HasCheckSum()
		{
			mustBeCheckSum=0;
			mustBeXor=0;
		}
		unsigned int CalculateCheckSum(void) const;
		unsigned int CalculateXor(void) const;
	};

	class RawRead : public HasCheckSum
	{
	};
	class SectorDump : public HasCheckSum
	{
	public:
		int version;
		SectorDump()
		{
			version=1;
		}
	};
	class SectorInfo : public HasData
	{
	};
	class Track
	{
	public:
		int trk,sid;
		unsigned int lastErr;

		std::vector <Sector> sec;

		std::vector <RawRead> rawRead;
		std::vector <SectorDump> sectorDump;
		std::vector <SectorInfo> sectorInfo;

		Track();
		void CleanUp(void);

		/*! Returns true if check sum, size, and xor matches actual.
		*/
		bool CheckSumTest(void) const;

		/*! Print check-sum, size, and xor status.
		*/
		void PrintStat(void) const;

		/*! Copy sector dump to each sector.
		    Returns false if calculated sector-size total does not match sectorDump size.
		*/
		bool SectorDumpToSector(void);
private:
		bool SectorDumpToSector(const SectorInfo &info,const SectorDump &dump);

public:
		/*! Make a D77 lib sector.
		*/
		D77File::D77Disk::D77Sector MakeD77Sector(int sectorId) const;
	};

	enum
	{
		MEDIA_2D=0,
		MEDIA_2DD=0x10
	};
	int mediaType=0;
	std::string diskName;
	std::vector <Track> trk;

	void CleanUp(void);
	void VerifyCheckSum(void) const;


	D77File::D77Disk MakeD77Disk(void) const;


	static std::vector <FM7RawDiskRead> Load(const char fn[]);
private:
	static FM7RawDiskRead LoadDisk(std::istream &ifp);
	static Track LoadTrack(std::istream &ifp,const std::string &firstLine);
	static void LoadAddrMark(std::istream &ifp);
	static RawRead LoadRawRead(std::istream &ifp);
	static SectorInfo LoadSectorInfo(std::istream &ifp);
	static SectorDump LoadSectorDump(std::istream &ifp);
	static std::string ShortFormat(const std::string &str);
};


/* } */
#endif
