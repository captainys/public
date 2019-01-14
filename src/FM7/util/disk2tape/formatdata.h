#ifndef FORMATDATA_IS_INCLUDED
#define FORMATDATA_IS_INCLUDED
/* { */

#include <vector>
#include "../d77/d77.h"

class FormatData
{
public:
	std::vector <unsigned char> formatData;

	void CleanUp(void);
	bool MakeFormatData(const D77File::D77Disk &dsk);
private:
	void AddStandardTrack(void);
};


/* } */
#endif
