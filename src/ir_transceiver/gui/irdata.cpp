#include <stdlib.h>

#include "irdata.h"

#include "cheaputil.h"

IRData::IRData()
{
	CleanUp();
}

void IRData::CleanUp(void)
{
	label="";
	sample.clear();
	vizTop=0;
	vizZoom=0.25;
	selSample=0;
}

/* static */ std::vector <unsigned int> IRData::PWMtoModulated(const std::vector <unsigned int> &src)
{
	std::vector <unsigned int> mod;

	mod.push_back(0);
	for(long long int i=0; i<src.size(); i+=2)
	{
		if(i+1<src.size())
		{
			if(src[i+1]<26) // If Off-cycle is less than 26us, take it as all On.
			{
				mod.back()+=src[i];
				mod.back()+=src[i+1];
			}
			else
			{
				mod.back()+=src[i];
				mod.push_back(src[i+1]);
				mod.push_back(0);
			}
		}
		else
		{
			mod.back()+=src[i];
		}
	}

	if(0==mod.back()) // mod must have at least one number in it.
	{
		mod.pop_back();
	}

	return mod;
}



////////////////////////////////////////////////////////////



void IRDataSet::CleanUp(void)
{
	dataSet.clear();
}
void IRDataSet::MakeUpTestData(void)
{
	this->CleanUp();

	IRData dat;

	dat.CleanUp();
	dat.label="Data1";
	for(int k=0; k<3; ++k)
	{
		dat.sample.push_back(std::vector <unsigned int>());
		for(int i=0; i<10; ++i)
		{
			dat.sample.back().push_back(15);
		}
	}
	dataSet.push_back((IRData &&)dat);
}

bool IRDataSet::Save(FILE *fp) const
{
	for(auto &dat : dataSet)
	{
		fprintf(fp,"DATA \"%s\"\n",dat.label.c_str());
		for(auto &sam : dat.sample)
		{
			fprintf(fp,"SAMPLE");
			for(auto us : sam)
			{
				fprintf(fp," %d",us);
			}
			fprintf(fp,"\n");
		}
		fprintf(fp,"ENDDATA\n");
	}
	return true;
}
bool IRDataSet::Load(FILE *fp)
{
	CleanUp();
	std::string str;
	while(nullptr!=CheapFGets(str,fp))
	{
		auto argv=CheapParser(str.c_str());
		if(0<argv.size())
		{
			if("DATA"==argv[0])
			{
				dataSet.push_back(IRData());
				if(2<=argv.size())
				{
					dataSet.back().label=argv[1];
				}
			}
			else if("SAMPLE"==argv[0])
			{
				if(0<dataSet.size())
				{
					std::vector <unsigned int> microsPulse;
					for(int i=1; i<argv.size(); ++i)
					{
						microsPulse.push_back(atoi(argv[i].c_str()));
					}
					dataSet.back().sample.push_back((std::vector <unsigned int> &&)microsPulse);
				}
			}
			else if("IRTOY_0_17"==argv[0])
			{
				if(0<dataSet.size())
				{
					std::vector <unsigned int> microsPulse;
					for(int i=1; i+1<argv.size(); i+=2)
					{
						unsigned int low=atoi(argv[i+1].c_str());
						unsigned int high=atoi(argv[i].c_str());
						unsigned int mix=high*256+low;
						mix*=17;
						mix/=100;
						microsPulse.push_back(mix);
					}
					dataSet.back().sample.push_back((std::vector <unsigned int> &&)microsPulse);
				}
			}
			else if("ENDDATA"==argv[0])
			{
			}
		}
	}
	return true;
}
