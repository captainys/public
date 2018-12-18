/* ////////////////////////////////////////////////////////////

File Name: test.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <ysclass.h>
#include <ysport.h>
#include <ysshellext.h>
#include <ysshellextio.h>
#include <ysshellextedit.h>


template <class SHLCLASS>
static bool TestConsistency(const SHLCLASS &shl,YsShell::MetaDataHandle mdHd)
{
	auto md=shl.GetMetaData(mdHd);
	if(md.key!=shl.GetMetaDataKey(mdHd))
	{
		printf("GetMetaDataKey is returning inconsistent data with GetMetaData\n");
		return false;
	}
	if(md.value!=shl.GetMetaDataValue(mdHd))
	{
		printf("GetMetaDataValue is returning inconsistent data with GetMetaData\n");
		return false;
	}

	auto key=shl.GetSearchKey(mdHd);
	if(shl.FindMetaData(key)!=mdHd)
	{
		printf("FindMetaData does not return handle for GetSearchKey.\n");
		return false;
	}

	auto mdHdStorePtr=shl.FindMetaData(md.key);
	if(nullptr==mdHdStorePtr)
	{
		printf("FindMetaData(from string) does not return handle.\n");
		return false;
	}
	else
	{
		bool included=false;
		for(auto hd : *mdHdStorePtr)
		{
			if(hd==mdHd)
			{
				included=true;
				break;
			}
		}
		if(true!=included)
		{
			printf("FindMetaData(from string) does not return handle that uses a key.\n");
			return false;
		}
	}

	return true;
}

template <class SHLCLASS>
static bool Test(void)
{
	const char *const tag[]=
	{
		"tag0","tag1","tag2","tag3"
	};
	const char *const value[]=
	{
		"value0adjfa","value1asdfjad","value2cz,mv/","value3aopi;a"
	};



	YsArray <YsShell::MetaDataHandle> mdHd;
	SHLCLASS shl,shl2,shl3;
	for(int i=0; i<4; ++i)
	{
		mdHd.push_back(shl.AddMetaData(tag[i],value[i]));
	}
	for(int i=0; i<4; ++i)
	{
		auto md=shl.GetMetaData(mdHd[i]);
		if(0!=md.key.Strcmp(tag[i]))
		{
			printf("Tag %s!=%s\n",tag[i],md.key.c_str());
			return false;
		}
		if(0!=md.value.Strcmp(value[i]))
		{
			printf("Value %s!=%s\n",value[i],md.value.c_str());
			return false;
		}
		if(true!=TestConsistency(shl,mdHd[i]))
		{
			return false;
		}
		printf("Pass: %s, %s\n",tag[i],value[i]);
	}



	shl2=shl;
	if(sizeof(tag)/sizeof(tag[0])!=shl2.GetNumMetaData())
	{
		printf("Meta Data not copied.\n");
		printf("There supposed to be %d meta data.\n",(int)sizeof(tag)/sizeof(tag[0]));
		printf("Actually %d\n",(int)shl2.GetNumMetaData());
		return false;
	}



	YsShellExtWriter writer;
	writer.SaveSrf("metadata.srf",shl.Conv());

	shl.CleanUp();
	for(auto mdHd : shl.AllMetaData())
	{
		printf("Meta Data not cleaned up.\n");
		return false;
	}



	YsFileIO::File fp("metadata.srf","r");
	auto inStream=fp.InStream();

	shl3.LoadSrf(inStream);
	if(sizeof(tag)/sizeof(tag[0])!=shl3.GetNumMetaData())
	{
		printf("Meta Data not read from file (or not written to file).\n");
		printf("There supposed to be %d meta data.\n",(int)sizeof(tag)/sizeof(tag[0]));
		printf("Actually read %d\n",(int)shl3.GetNumMetaData());
		return false;
	}
	for(auto mdHd : shl3.AllMetaData())
	{
		TestConsistency(shl3,mdHd);
		auto md=shl3.GetMetaData(mdHd);
		int found=-1;
		for(int i=0; i<4; ++i)
		{
			if(0==md.key.Strcmp(tag[i]))
			{
				found=i;
				break;
			}
		}
		if(found<0)
		{
			printf("Key %s not supposed to exist.\n",md.key.c_str());
			return false;
		}
		if(0!=md.value.Strcmp(value[found]))
		{
			printf("Value for key %s is supposed to be %s.\n",tag[found],value[found]);
			printf("Actually got %s\n",md.value.c_str());
			return false;
		}
		printf("Pass: %s,%s\n",md.key.c_str(),md.value.c_str());
	}


	int i=0;
	for(auto mdHd : shl3.AllMetaData())
	{
		if(0==i%2)
		{
			shl3.DeleteMetaData(mdHd);
		}
		++i;
	}
	if(2!=shl3.GetNumMetaData())
	{
		printf("Meta Data not deleted.\n");
		return false;
	}

	{
		auto mdHd=shl.AddMetaData("key","value");
		if(YSOK!=shl.SetMetaDataValue(mdHd,"newvalue"))
		{
			printf("Failed to update a value.\n");
			return false;
		}
		auto md=shl.GetMetaData(mdHd);
		if(md.value!=YsString("newvalue"))
		{
			printf("Value not updated.\n");
			return false;
		}
		printf("Pass: Value updated ok\n");
	}

	return true;
}

int main(void)
{
	YsFileIO::ChDir(YsSpecialPath::GetProgramBaseDirW());

	printf("@@@ TESTING: YsShell Meta Data\n");


	if(true!=Test<YsShellExt>())
	{
		goto BADEND;
	}
	if(true!=Test<YsShellExtEdit>())
	{
		goto BADEND;
	}


	printf("@@@ TEST RESULT: OK\n");
	return 0;

BADEND:
	printf("@@@ TEST RESULT: ERROR\n");
	return 1;
}

