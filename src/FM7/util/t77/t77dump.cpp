#include <vector>
#include <stdio.h>


std::vector <unsigned char> ReadFile(const char fn[])
{
	const int bufSize=1024*1024;
	std::vector <unsigned char> readbuf;
	std::vector <unsigned char> all;
	readbuf.resize(bufSize);

	FILE *fp=fopen(fn,"rb");
	if(nullptr!=fp)
	{
		long long int rs;
		while(0<(rs=fread(readbuf.data(),1,bufSize,fp)))
		{
			for(int i=0; i<rs; ++i)
			{
				all.push_back(readbuf[i]);
			}
		}
		fclose(fp);
	}
	return all;
}

// Short 0x1A+0x1A=0x34
// Long  0x30+0x30=0x60
const int OneZeroThr=0x48;

long long int SkipLead(const std::vector <unsigned char> &dat,long long int ptr)
{
	int state=0;  // 0:Not yet in the lead.  1:In the lead.
	int longCtr=0;
	int leadCtr=0;
	while(ptr+4<dat.size())
	{
		if(0==state)
		{
			if(0x40<dat[ptr] && dat[ptr+2]<0x40)
			{
				if((unsigned int)dat[ptr+1]+(unsigned int)dat[ptr+3]<OneZeroThr)
				{
					longCtr=0;
				}
				else
				{
					++longCtr;
					if(10==longCtr)
					{
						state=1;
						longCtr=0;
						leadCtr=1;
					}
				}
				ptr+=4;
			}
			else
			{
				ptr+=2;
			}
		}
		else
		{
			if(0x40<dat[ptr] && dat[ptr+2]<0x40)
			{
				if((unsigned int)dat[ptr+1]+(unsigned int)dat[ptr+3]<OneZeroThr)
				{
					for(int i=0; i<10; ++i)
					{
						if(dat[ptr+4+i*4+1]+dat[ptr+4+i*4+3]<OneZeroThr)
						{
							printf("%d repeats.\n",leadCtr);
							return ptr;
						}
					}
				}
				ptr+=44;
				leadCtr++;
			}
			else
			{
				ptr+=2;
			}
		}
	}
	return ptr;
}

int GetBit(const std::vector <unsigned char> &dat,long long int &ptr)
{
	int nLow=0,nHigh=0;
	while(ptr+2<dat.size() && (0==nLow || 0==nHigh))
	{
		if(dat[ptr]<0x40)
		{
			nLow+=dat[ptr+1];
		}
		else
		{
			nHigh+=dat[ptr+1];
		}
		ptr+=2;
	}
	if(OneZeroThr<nLow+nHigh)
	{
		return 1;
	}
	return 0;
}

bool FindFirstFFFFFF(const std::vector <unsigned char> &dat,long long int &ptr)
{
	int bit[33];

	while(ptr<dat.size())
	{
		auto searchPtr=ptr;

		for(int i=0; i<33; ++i)
		{
			bit[i]=GetBit(dat,searchPtr);
		}

		for(int i=0; i<33; ++i)
		{
			printf("%d ",bit[i]);
		}
		printf("\n");
		//printf(">");
		//getchar();


		bool isFf=false;
		if(0==bit[0] && 0==bit[11] && 0==bit[22])
		{
			for(int i=1; i<33; ++i)
			{
				if(0!=(i%11) && 1!=bit[i])
				{
					goto NEXTPTR;
				}
			}

			printf("FFFFFF found at ptr=%d\n",(int)ptr);
			return true;
		}
	NEXTPTR:
		ptr+=2;
	}

	return false;
}

std::vector <unsigned char> DecodeT77(const std::vector <unsigned char> &dat)
{
	long long int ptr=0x10;  // Skip 16-byte header.
	std::vector <unsigned char> dump;

	int state=0; // 0 Outside   1 Inside
	int nFile=0;

	while(ptr+4<dat.size())
	{
		if(0==state)
		{
			if(true==FindFirstFFFFFF(dat,ptr))
			{
				if(0<nFile)
				{
					for(int i=0; i<256; ++i)
					{
						dump.push_back(0);
					}
				}
				++nFile;
				state=1;
			}
			else
			{
				break;
			}
		}
		else
		{
			if(0x7f==dat[ptr] && 0xff==dat[ptr+1])
			{
				// XM7 seems to write 0x7f, 0xff between files.
				state=0;
			}

			auto ptr0=ptr;
			int bit[11];
			for(int i=0; i<11; ++i)
			{
				bit[i]=GetBit(dat,ptr);
			}


			// printf("[%5d]",(int)dump.size());
			// for(int i=0; i<11; ++i)
			// {
			// 	printf("%d ",bit[i]);
			// }
			// printf("\n");
			// printf(">");
			// getchar();


			if(0!=bit[0] || 0==bit[9] || 0==bit[10])
			{
				printf("Device I/O Error at ptr=%d (0x%08x)\n",(int)ptr0,(int)ptr0);
				printf("May just be an end of file.\n");
				state=0;
			}

			unsigned int add=1;
			unsigned int sum=0;
			for(int i=1; i<9; ++i)
			{
				if(0!=bit[i])
				{
					sum+=add;
				}
				add<<=1;
			}

			dump.push_back(sum);
		}
	}
	return dump;
}

std::vector <unsigned char> ExtractBlock(const std::vector <unsigned char> &dat,long long int &ptr)
{
	// Block starts with: 0x01 0x3c
	// Followed by 0 or 1 or 0xff
	//             0 maybe header
	//             1 maybe data
	//             0xff is terminator.

	std::vector <unsigned char> block;
	while(ptr+5<dat.size())
	{
		if(dat[ptr]==0xff && dat[ptr+1]==0x01 && dat[ptr+2]==0x3c)
		{
			if(0x00==dat[ptr+3] || 0x01==dat[ptr+3])
			{
			}
			else if(0xff==dat[ptr+3])
			{
			}
			int nType=dat[ptr+3];
			int nByte=dat[ptr+4];
			++ptr;
			for(int i=0; i<4 && ptr<dat.size(); ++i)
			{
				block.push_back(dat[ptr]);
				++ptr;
			}
			for(int i=0; i<nByte+1 && ptr<dat.size(); ++i)
			{
				block.push_back(dat[ptr]);
				++ptr;
			}

			printf("nByte=%d (0x%02x)\n",nByte,nByte);

			if(0<nByte)
			{
				unsigned int sum=0;
				for(int i=0; i<nByte+2; ++i)
				{
					sum+=block[i+2];
				}
				// Total of the block except 0x01 0x3c and the last Check-sum byte.
				printf("Check sum:%08x\n",sum);
				printf("Last Byte:%02x\n",block[4+nByte]);
			}
			else
			{
				printf("Must be the terminator.\n");
				printf("No check sum.\n");
			}
			return block;
		}
		else if(0xff==dat[ptr] || 0x00==dat[ptr])
		{
			++ptr;
		}
		else
		{
			printf("Error at %d!\n",(int)ptr);
			printf("%02x %02x %02x\n",dat[ptr],dat[ptr+1],dat[ptr+2]);
			break;
		}
	}
	return block;
}

int main(int ac,char *av[])
{
	if(ac<3)
	{
		printf("Usage: t77dump input.t77 output.bin\n");
		return 1;
	}

	auto dat=ReadFile(av[1]);
	auto dump=DecodeT77(dat);
	if(0==dump.size())
	{
		printf("Did not find a lead.\n");
	}
	else
	{
		printf("Read %d bytes.\n",(int)dump.size());
		const char *fn=av[2];
		FILE *fp=fopen(fn,"wb");
		fwrite(dump.data(),1,dump.size(),fp);
		fclose(fp);
		printf("Dump written to %s\n",fn);

		long long int ptr=0;
		while(ptr<dump.size()-5)
		{
			auto prevPtr=ptr;
			auto blk=ExtractBlock(dump,ptr);
			if(prevPtr==ptr)
			{
				break;
			}
		}
	}
	return 0;
}


