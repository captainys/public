#include <ysport.h>
#include "retromap_pngexport.h"

void RetroMap_PngExportInfo::CleanUp(void)
{
	fdHdFn.CleanUp();
}
YSRESULT RetroMap_PngExportInfo::MakeFIeldHandleFileNamePair(const YsWString &baseFn,const RetroMap_World &world)
{
	CleanUp();

	auto baseWithoutExt=baseFn;
	baseWithoutExt.RemoveExtension();
	baseWithoutExt.Append(L"_");

	int num=01;
	for(auto fdHd : world.AllField())
	{
		fdHdFn.Increment();

		fdHdFn.Last().fdHd=fdHd;
		fdHdFn.Last().fn=baseWithoutExt;

		YsString numStr;
		numStr.Printf("%03d",num);
		YsWString numWStr;
		numWStr.SetUTF8String(numStr);
		fdHdFn.Last().fn.Append(numWStr);

		auto fldName=world.GetFieldName(fdHd);
		if(0<fldName.Strlen())
		{
			fdHdFn.Last().fn.Append(L"_");
			fdHdFn.Last().fn.Append(fldName);
		}

		fdHdFn.Last().fn.ReplaceExtension(L".png");
		++num;
	}

	return YSOK;
}
const YsConstArrayMask <RetroMap_PngExportInfo::FieldHandle_FileNamePair> RetroMap_PngExportInfo::GetFieldHandleFileNamePairArray(void) const
{
	YsConstArrayMask <FieldHandle_FileNamePair> mask(fdHdFn.GetN(),fdHdFn);
	return mask;
}
YSBOOL RetroMap_PngExportInfo::CheckOverwrite(void) const
{
	for(auto &f : fdHdFn)
	{
		YsFileIO::File fp(f.fn,"rb");
		if(nullptr!=fp)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

