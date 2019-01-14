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
	while(0==nLow || 0==nHigh)
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

int main(int ac,char *av[])
{
	auto dat=ReadFile(av[1]);
	long long int ptr=0x10;

	ptr=SkipLead(dat,ptr);
	printf("%08x\n",(int)ptr);
	auto lastLead=ptr;

	for(;;)
	{
		for(int i=0; i<256; ++i)
		{
			if(i<dat.size())
			{
				printf("%02x ",dat[ptr+i]);
			}
			if(15==i%16)
			{
				printf("\n");
			}
		}

		{
			int nLow=0,nHigh=0;
			int add=0x1,sum=0;

			int i=0;
			auto ptrCpy=ptr;
			while(i<11 && ptrCpy+1<dat.size())
			{
				int bit=GetBit(dat,ptrCpy);
				if(0!=bit)
				{
					printf("1 ");
					sum+=(add>>1);
				}
				else
				{
					printf("0 ");
				}
				add<<=1;
				if(0x200==add)
				{
					add=0;
				}
				i++;
			}
			printf("\n");
			printf("%02x\n",sum);
			if(' '<=sum && sum<128)
			{
				printf("[%c]\n",sum);
			}
		}
		printf("Bit count from the lead %d\n",(int)(ptr-lastLead)/4);

		if(0==((ptr-lastLead)/4)%11)
		{
			printf("11-bit border");
		}

		printf("(s:Skip Leader  b:Move 11 bits)>");

		char cmdstr[256];
		fgets(cmdstr,255,stdin);
		auto cmd=cmdstr[0];
		if(cmd=='s' || cmd=='S')
		{
			ptr=SkipLead(dat,ptr);
		}
		else if(cmd=='b' || cmd=='B')
		{
			for(int i=0; i<11; ++i)
			{
				GetBit(dat,ptr);
			}
		}
		else
		{
			GetBit(dat,ptr);
		}
	}

	return 0;
}


