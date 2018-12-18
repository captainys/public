#include "filedialog.h"

#ifdef _WIN32

#include <windows.h>
#include <direct.h>

#pragma comment(lib,"comdlg32.lib")

static void BarToZero(char str[])
{
	if(NULL!=str)
	{
		for(int i=0; 0!=str[i]; ++i)
		{
			if(str[i]=='|')
			{
				str[i]=0;
			}
		}
	}
}

static void SearchTopLevelWindow(std::vector <HWND> &hWndAll,HWND hWnd,DWORD procId)
{
	DWORD windowProcId;
	GetWindowThreadProcessId(hWnd,&windowProcId);
	if(windowProcId==procId)
	{
		hWndAll.push_back(hWnd);
		return;
	}

	HWND hWndChild=NULL;
	while(NULL!=(hWndChild=FindWindowEx(hWnd,hWndChild,NULL,NULL))!=NULL)
	{
		SearchTopLevelWindow(hWndAll,hWndChild,procId);
	}
}

static HWND GetApplicationMainWindow(void)
{
	std::vector <HWND> hWndAll;
	SearchTopLevelWindow(hWndAll,NULL,GetCurrentProcessId());

	HWND hWndLargest=NULL;
	int largestArea=0;
	for(int i=0; i<hWndAll.size(); ++i)
	{
		char str[256];
		GetWindowTextA(hWndAll[i],str,255);
		// printf("hWnd=%08x Title=%s\n",(int)hWndAll[i],str);
		// printf("IsVisible=%d\n",IsWindowVisible(hWndAll[i]));

		RECT rc;
		GetWindowRect(hWndAll[i],&rc);

		int area=((rc.right-rc.left)*(rc.bottom-rc.top));
		if(area<0)
		{
			area=-area;
		}
		// printf("Area=%d square pixels. (%d x %d)\n",area,(rc.right-rc.left),(rc.bottom-rc.top));

		if(TRUE==IsWindowVisible(hWndAll[i]) && largestArea<area)
		{
			hWndLargest=hWndAll[i];
			largestArea=area;
		}
	}

	return hWndLargest;
}

std::string SelectFile(const FileDialogOption &opt)
{
	std::string fName;

	int result;
	char curpath[MAX_PATH];
	char selected[MAX_PATH]={0};
	char basename[MAX_PATH]={0};
	char ext[64]="(*.*)|*.*||";
	HINSTANCE module=GetModuleHandle(NULL);

	_getcwd(curpath,MAX_PATH);

	BarToZero(ext);

	OPENFILENAMEA para;
	ZeroMemory(&para,sizeof(para));

	para.lStructSize=sizeof(para);
	para.hwndOwner=GetApplicationMainWindow();
	para.hInstance=module;
	para.lpstrFilter=ext;
	para.lpstrDefExt="txt";
	para.lpstrCustomFilter=NULL;
	para.nFilterIndex=0;

	para.nMaxFile=sizeof(selected);
	para.lpstrFileTitle=basename;
	para.lpstrTitle="Select File";
	para.Flags=OFN_ENABLEHOOK|OFN_ENABLESIZING|OFN_EXPLORER;

	para.lpstrFile=selected;
	para.lpstrInitialDir=curpath;

	if(opt.MODE_OPEN==opt.mode)
	{
		para.Flags|=OFN_FILEMUSTEXIST;
	}
	if(opt.MODE_SAVE==opt.mode)
	{
		para.Flags|=OFN_OVERWRITEPROMPT;
		para.Flags|=OFN_HIDEREADONLY;
	}

	result=GetOpenFileNameA(&para);

	if(result!=0)
	{
		fName=selected;
	}
	_chdir(curpath);

	return fName;
}

#else   // #ifdef _WIN32

std::string SelectFile(const FileDialogOption &opt)
{
	return "";
}

#endif  // #ifdef _WIN32
