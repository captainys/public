#include "yspropertyfile.h"
#include "ystextstream.h"

void YsPropertyFileIO::Save(YsTextOutputStream &outStream)
{
	for(auto &p : prop)
	{
		auto str=p->FormatString();
		outStream.Printf("%s\n",str.data());
	}
}
YSRESULT YsPropertyFileIO::Load(YsTextInputStream &inStream)
{
	YSRESULT res=YSOK;
	errStr.clear();

	while(YSTRUE!=inStream.EndOfFile())
	{
		auto str=inStream.Fgets();
		auto argv=str.Argv();
		if(0<argv.size())
		{
			auto treeNdHd=tagToPropPtr.FindNode(argv[0]);
			if(treeNdHd!=nullptr)
			{
				auto propPtr=tagToPropPtr.GetValue(treeNdHd);
				if(YSOK!=propPtr->Recognize(argv))
				{
					YsString err;
					err="Cannot recognize:";
					err.Append(str);
					fprintf(stderr,"%s\n",err.data());
					res=YSERR;
				}
			}
		}
	}
	return res;
}
