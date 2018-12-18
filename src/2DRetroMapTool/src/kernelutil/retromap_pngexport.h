#ifndef RETROMAP_PNGEXPORT_IS_INCLUDED
#define RETROMAP_PNGEXPORT_IS_INCLUDED
/* { */

#include <retromap_data.h>

class RetroMap_PngExportInfo
{
public:
	class FieldHandle_FileNamePair
	{
	public:
		RetroMap_World::FieldHandle fdHd;
		YsWString fn;
	};
private:
	YsArray <FieldHandle_FileNamePair> fdHdFn;
public:
	void CleanUp(void);
	YSRESULT MakeFIeldHandleFileNamePair(const YsWString &baseFn,const RetroMap_World &world);
	const YsConstArrayMask <FieldHandle_FileNamePair> GetFieldHandleFileNamePairArray(void) const;
	YSBOOL CheckOverwrite(void) const;
};

/* } */
#endif
