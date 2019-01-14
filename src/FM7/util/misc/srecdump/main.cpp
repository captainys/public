#include <stdio.h>
#include "cpplib.h"
#include "fm7lib.h"


void Dump(FILE *ofp,const char ifn[])
{
	auto srec=FM7Lib::ReadTextFile(ifn);
	FM7BinaryFile binDat;
	if(0<srec.size() && true==binDat.DecodeSREC(srec))
	{
		int startAddr=(binDat.storeAddr&0xff00);
		int endAddr=((binDat.storeAddr+binDat.dat.size()-1)&0xff00);

		auto baseName=FM7Lib::GetBaseName(ifn);
		fprintf(ofp,"FILENAME=%s\n",baseName.c_str());
		for(int base=startAddr; base<=endAddr; base+=256)
		{
			unsigned int vCheckSum[16];
			for(auto &c : vCheckSum)
			{
				c=0;
			}
			fprintf(ofp,"Add   +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F Sum\n");
			for(int offset=0; offset<256; offset+=16)
			{
				if(binDat.dat.size()<=base+offset-startAddr)
				{
					break;
				}
				fprintf(ofp,"%04X ",base+offset);
				unsigned int hCheckSum=0;
				for(int i=0; i<16; ++i)
				{
					int index=(base+offset+i)-startAddr;
					if(0<=index && index<binDat.dat.size())
					{
						fprintf(ofp," %02X",binDat.dat[index]);
						hCheckSum+=binDat.dat[index];
						vCheckSum[i]+=binDat.dat[index];
					}
					else
					{
						fprintf(ofp,"   ");
					}
				}
				fprintf(ofp," :%02X |",(hCheckSum&0xff));
				for(int i=0; i<16; ++i)
				{
					int index=(base+offset+i)-startAddr;
					if(0<=index && index<binDat.dat.size() && 32<=binDat.dat[index] && binDat.dat[index]<128)
					{
						fprintf(ofp,"%c",binDat.dat[index]);
					}
					else
					{
						fprintf(ofp,".");
					}
				}
				fprintf(ofp,"\n");
			}
			fprintf(ofp,"---------------------------------------------------------\n");
			fprintf(ofp,"     ");
			for(int i=0; i<16; ++i)
			{
				fprintf(ofp," %02X",(vCheckSum[i]&0xff));
			}
			fprintf(ofp,"\n");
			fprintf(ofp,"\n");
		}
	}
	else
	{
		fprintf(stderr,"Cannot read file: %s\n",ifn);
	}
}

int main(int ac,char *av[])
{
	if(ac<2)
	{
		printf("Usage: srecdump filename.srec [output.txt]\n");
		return 0;
	}

	if(3<=ac)
	{
		FILE *ofp=fopen(av[2],"w");
		if(nullptr!=ofp)
		{
			Dump(ofp,av[1]);
			fclose(ofp);
		}
		else
		{
			return 1;
		}
	}
	else
	{
		Dump(stdout,av[1]);
	}
	return 0;
}
