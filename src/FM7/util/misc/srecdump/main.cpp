#include <stdio.h>
#include "cpplib.h"
#include "fm7lib.h"

class Option
{
public:
	bool rawDump;
	Option()
	{
		rawDump=false;
	}
	int Recognize(int ac,char *av[])
	{
		int taken=0;
		for(int i=2; i<ac; ++i)
		{
			std::string argv=av[i];
			FM7Lib::Capitalize(argv);
			if(argv=="-RAWDUMP")
			{
				rawDump=true;
				++taken;
			}
		}
		return taken;
	}
};

void Dump(FILE *ofp,const char ifn[],Option opt)
{
	auto srec=FM7Lib::ReadTextFile(ifn);
	FM7BinaryFile binDat;
	if(0<srec.size() && true==binDat.DecodeSREC(srec))
	{
		if(true==opt.rawDump)
		{
			int x=0;
			for(auto c : binDat.dat)
			{
				fprintf(ofp,"%02x",c);
				++x;
				if(16==x)
				{
					fprintf(ofp,"\n");
					x=0;
				}
			}
			if(0<x)
			{
				fprintf(ofp,"\n");
			}
		}
		else
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
		printf("Usage: srecdump filename.srec [output.txt] -rawdump\n");
		return 0;
	}

	Option opt;
	auto taken=opt.Recognize(ac,av);

	if(3<=ac-taken)
	{
		FILE *ofp=fopen(av[2],"w");
		if(nullptr!=ofp)
		{
			Dump(ofp,av[1],opt);
			fclose(ofp);
		}
		else
		{
			return 1;
		}
	}
	else
	{
		Dump(stdout,av[1],opt);
	}
	return 0;
}
