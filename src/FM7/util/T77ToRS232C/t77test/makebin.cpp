#include <stdio.h>
#include <stdlib.h>


unsigned char buf[0x7000-0x2000];

void Save(char fn[])
{
	FILE *fp=fopen(fn,"wb");
	fwrite(buf,1,sizeof(buf),fp);
	fclose(fp);
}

int main(void)
{
	for(auto &c : buf)
	{
		c=0x4e;
	}
	Save("all4e.bin");

	int i=0;
	for(auto &c : buf)
	{
		c=i;
		i=(i+1)&255;
	}
	Save("inc01.bin");

	i=0;
	for(auto &c : buf)
	{
		c=i;
		i=(i+0x10)&255;
	}
	Save("inc10.bin");

	for(auto &c : buf)
	{
		c=rand()&255;
	}
	Save("random.bin");
	return 0;
}

