#ifndef IRDATA_IS_INCLUDED
#define IRDATA_IS_INCLUDED
/* { */


#include <stdio.h>

#include <vector>


class IRData
{
public:
	std::string label;
	std::vector <std::vector <unsigned int> > sample;

	int vizTop;
	float vizZoom;
	int selSample;

	IRData();
	void CleanUp(void);
	static std::vector <unsigned int> PWMtoModulated(const std::vector <unsigned int> &dat);
};


class IRDataSet
{
public:
	std::vector <IRData> dataSet;

	void CleanUp(void);
	void MakeUpTestData(void);

	bool Save(FILE *fp) const;
	bool Load(FILE *fp);
};


/* } */
#endif
