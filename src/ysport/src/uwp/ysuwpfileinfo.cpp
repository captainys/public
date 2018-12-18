/* ////////////////////////////////////////////////////////////

File Name: ysuwpfileinfo.cpp
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

#include <chrono>

#include "../ysfileinfo.h"
#include "../ysencoding.h"
#include "../ysfileio.h"

#include <ppltasks.h> // To use Concurrency namespace and create_task

using namespace Windows::Storage;
using namespace Concurrency; // To use create_task
using namespace Windows::Foundation::Collections; // To use IVectorView

YSRESULT YsFileList::FindFileList(const char path[],const char prefix[],const char postfix[])
{
	YsWString wPath,wPrefix,wPostfix;
	if(NULL!=path)
	{
		YsSystemEncodingToUnicode(wPath,path);
	}
	if(NULL!=prefix)
	{
		YsSystemEncodingToUnicode(wPrefix,prefix);
	}
	if(NULL!=postfix)
	{
		YsSystemEncodingToUnicode(wPostfix,postfix);
	}
	return FindFileList(wPath,wPrefix,wPostfix);
}



class YsUwpFileList
{
public:
	class File
	{
	public:
		YsWString nm;
		YSBOOL isDir;
	};

	YSRESULT res;
	YsArray <File> fileList;

	YSRESULT MakeFileList(const YsWString &fulPath);
private:
	void RunInThread(const wchar_t fulPath[]);
	void ConstrainFileName(YsWString &fn);
};

YSRESULT YsUwpFileList::MakeFileList(const YsWString &fulPath)
{
	// task.wait() fails in the UI thread.  Run it in a separate thread.
	std::thread t(&YsUwpFileList::RunInThread,this,fulPath.Txt());
	t.join();
	return res;
}

void YsUwpFileList::RunInThread(const wchar_t fulPath[])
{
	YsWString ful=fulPath;
	ConstrainFileName(ful);

	printf("Looking into: %ls\n",fulPath);
	printf("Boo\n");

	this->fileList.CleanUp();
	this->res=YSOK;
	Platform::String^ uwpPath=ref new Platform::String(ful);

	try
	{
		printf("Bow\n");
		auto getFolderTask=create_task(Windows::Storage::StorageFolder::GetFolderFromPathAsync(uwpPath));
		getFolderTask.wait();
		auto folder=getFolderTask.get();
		auto getItemsTask=create_task(folder->GetItemsAsync());
		auto itemList=getItemsTask.get();
		for(int i=0; i<itemList->Size; ++i)
		{
			auto itm=itemList->GetAt(i);
			YSBOOL isDir=YSFALSE;
			if((itm->Attributes&FileAttributes::Directory)==FileAttributes::Directory)
			{
				isDir=YSTRUE;
			}
			YsWString fn=itm->Name->Data();
			fileList.Increment();
			fileList.Last().nm=fn;
			fileList.Last().isDir=isDir;
		}
	}
	catch(Platform::COMException^)
	{
		// Not found.  Nothing to do :-P
		printf("Mew\n");
		this->res=YSERR;
	}

	printf("Beh\n");
}

void YsUwpFileList::ConstrainFileName(YsWString &path)
{
	path.SimplifyPath();

	for(int i=0; i<path.Strlen(); ++i)
	{
		if(path[i]=='/')
		{
			path.Set(i,'\\');
		}
	}

	if(YSTRUE==path.DoesEndWith(L"\\."))
	{
		path.BackSpace();
		path.BackSpace();
	}
}

YSRESULT YsFileList::FindFileList(const wchar_t path[],const wchar_t prefix[],const wchar_t postfix[])
{
	Initialize();


	YsWString fn,fulDir,tst;


	if(NULL!=prefix)
	{
		uPrefix.Set(prefix);
	}
	else
	{
		uPrefix.Set(L"");
	}
	if(NULL!=postfix)
	{
		uPostfix.Set(postfix);
	}
	else
	{
		uPostfix.Set(L"");
	}
	uPrefix.Uncapitalize();
	uPostfix.Uncapitalize();
	this->path.Set(path);



	fulDir.Set(path);
	if(0<fulDir.Strlen() && '\\'!=fulDir.LastChar() && '/'!=fulDir.LastChar())
	{
		fulDir.Append(L"\\");
	}
	fulDir.Append(uPrefix);
	fulDir.Append(L"*");
	fulDir.Append(uPostfix);



	YSRESULT res=YSERR;
	YsUwpFileList fList;
	if(YSOK==fList.MakeFileList(path))
	{
		res=YSOK;
	}
	else
	{
		YsWString ful;
		ful.MakeFullPathName(YsFileIO::Getcwd(),path);
		if(YSOK==fList.MakeFileList(ful))
		{
			res=YSOK;
		}
	}
	for(auto &fi : fList.fileList)
	{
		TestAndAddFileInfo(fi.nm,fi.isDir,0,1);
	}


	return res;
}


