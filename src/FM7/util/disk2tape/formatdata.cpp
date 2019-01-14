#include "formatdata.h"



void FormatData::CleanUp(void)
{
	formatData.clear();
}

bool FormatData::MakeFormatData(const D77File::D77Disk &dsk)
{
	CleanUp();

	int nTrk=0;
	for(auto trkLoc : dsk.AllTrack())
	{
		if(true==dsk.CheckDuplicateSector(trkLoc.track,trkLoc.side))
		{
			fprintf(stderr,"Duplicate Sectors.\n");
			fprintf(stderr,"Use -deldupsec option.\n");
			return false;
		}

		auto trkPtr=dsk.FindTrack(trkLoc.track,trkLoc.side);
		if(nullptr!=trkPtr && 0<trkPtr->sector.size())
		{
			formatData.push_back((unsigned char)(trkPtr->sector.size()));
			for(auto &s : trkPtr->sector)
			{
				int sectorId=s.sector;
				if(sectorId==0xf5 || sectorId==0xf6 || sectorId==0xf7)
				{
					printf("0x%02x sector found.\n",sectorId);
					printf("You can force writing these sector contents by re-numbering with\n");
					printf("-renumfx option.  However, this program does not automatically\n");
					printf("break copy protection.\n");
					return false;
				}
				formatData.push_back((unsigned char)sectorId);
				formatData.push_back((unsigned char)s.sizeShift);
				formatData.push_back(0);
			}
		}
		else
		{
			// Unformatted track?  
			// I don't know how to unformat a track.
			// I just make it a standard 16-sector track.
			AddStandardTrack();
		}

		++nTrk;
		if(80<=nTrk)
		{
			break;
		}
	}

	formatData.push_back(0);
	return true;
}

void FormatData::AddStandardTrack(void)
{
	formatData.push_back(16);
	for(int i=1; i<=16; ++i)
	{
		formatData.push_back(i);
		formatData.push_back(1);
		formatData.push_back(0);
	}
}
