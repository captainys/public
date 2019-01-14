#ifndef D77FILESYSTEM_IS_INCLUDED
#define D77FILESYSTEM_IS_INCLUDED
/* { */

#include <vector>
#include "d77.h"

class D77FileSystem
{
public:
	enum 
	{
		STATE_UNUSED,
		STATE_USED,
		STATE_DELETED,
	};

	class FileInfo
	{
	public:
		std::string fName;
		int state;
		int fType;
		int firstCluster;

		std::string FormatInfo(void) const;
	};

private:
	std::vector <int> FAT;
	// File Allocation Table.
	// Do young people know FAT used to be a general term, not a file system from Microsoft?

	std::vector <FileInfo> directory;

public:
	D77FileSystem();
	~D77FileSystem();
	void CleanUp(void);

	static void ClusterToTrackSideSector(int &trk,int &sid,int &sec,int cluster);

	static FileInfo ByteDataToFileInfo(const unsigned char dat[32]);
	static void FileInfoToByteData(unsigned char dat[32],const FileInfo &fInfo);

	static FileInfo FMFileHeaderToFileInfo(const unsigned char fmHeader[16]);

	static void MoveToNextSector(int &trk,int &sid,int &sec);

	bool ReadDirectory(const D77File::D77Disk &dsk);
	bool WriteDirectory(D77File::D77Disk &dsk) const;

	/*! Returns a file index (zero or greater) if the file name exists.
	    Returns -1 if it doesn't.
	*/
	int FindFile(const D77File::D77Disk &dsk,const char fName[]) const;

	/*! Returns a file information.  If fileIdx is out of range, 
	    deleted flag will be set.
	*/
	FileInfo GetFileInfo(const D77File::D77Disk &dsk,int fileIdx) const;

	/*! Returns a file index to an unused file entry. */
	int GetUnusedFileIndex(const D77File::D77Disk &dsk) const;

	/*! Returns the cluster connection.
	    It is the next cluster if it is not the last cluster of a file.
	    Or, it is 0xff if the cluster is not used.
	*/
	int GetClusterConnection(const D77File::D77Disk &dsk,int cluster) const;

	/*! Delete a file in D77.
	*/
	bool Kill(D77File::D77Disk &dsk,const char filenameInD77[]);
	bool KillN(D77File::D77Disk &dsk,int n);


	/*! Returns a cached file allocation table.
	    This returns 152 cluster information starting at the 6th byte of Track 1 Side 0 Sector 1.
	*/
	std::vector <int> GetFAT(void) const;

	/*! Returns a dump of a directory.
	*/
	std::vector <unsigned char> GetDirDump(const D77File::D77Disk &dsk,int dirIdx) const;

	/*! Returns a list of files including unused entries (marked as deleted).
	*/
	std::vector <FileInfo> Files(const D77File::D77Disk &) const;

	/*! Returns a list of unused clusters.
	*/
	std::vector <int> FindUnusedCluster(const D77File::D77Disk &,int nCluster) const;

	/*! Get a chain of cluster for the file.
	*/
	std::vector <int> GetClusterChain(const D77File::D77Disk &,const FileInfo &fInfo) const;

	/*! Read raw file content from D77. */
	std::vector <unsigned char> RawFileRead(const D77File::D77Disk &dsk,const char filenameInD77[]) const;
	std::vector <unsigned char> RawFileRead(const D77File::D77Disk &dsk,int fileIdx) const;

	/*! Reserve a chain of clusters for a new file.
	*/
	std::vector <int> ReserveClusterChain(D77File::D77Disk &dsk,int nCluster,int lastClusterSectorUsage);

	/*! Write raw file content and updates file-allocation table.
	    Returns a first cluster.
	    If return value is negative, the file could not be written due to disk full or disk write protected.
	*/
	int RawFileWrite(D77File::D77Disk &dsk,const std::vector <unsigned char> &dat);

	/*! Save a FM-FILE format file to the disk.
	    If fileInD77 is "" or nullptr, file name is taken from the input file.
	*/
	bool SaveFMFile(D77File::D77Disk &dsk,const std::vector <unsigned char> &dat,const char filenameInD77[]);

	/*! Read into FM-data format.
	*/
	std::vector <unsigned char> EncodeFMFile(const D77File::D77Disk &dsk,const char filenameInD77[]) const;
	std::vector <unsigned char> EncodeFMFile(const D77File::D77Disk &dsk,int fileIdx) const;

	/*! Get FM-File format file extension.
	*/
	const char *GetFMFileExtensionForFile(const D77File::D77Disk &dsk,const char filenameInD77[]) const;
	const char *GetFMFileExtensionForFile(const D77File::D77Disk &dsk,int fileIdx) const;

	/*! Read from cluster up to nSector sectors.
	*/
	std::vector <unsigned char> ReadCluster(const D77File::D77Disk &dsk,int cluster,int nSector=8) const;

	/*! Write to the cluster up to 0x800 bytes and returns the actual bytes written.
	*/
	long long int WriteCluster(D77File::D77Disk &dsk,int cluster,long long int nByte,const unsigned char byteData[]);

	/*! Fill the entire cluster with 0x00.
	*/
	int ClearCluster(D77File::D77Disk &dsk,int cluster);
};


/* } */
#endif
