#include "clipboard.h"

#ifdef _WIN32
#include <windows.h>
#endif

std::vector <char> ReadFromClipboard(void)
{
	std::vector <char> clipboard;

#ifdef _WIN32
	// UNICODE Version
	if(TRUE==IsClipboardFormatAvailable(CF_UNICODETEXT) &&
	   TRUE==OpenClipboard(NULL))
	{
		HANDLE hClipBoard=GetClipboardData(CF_UNICODETEXT);
		if(NULL!=hClipBoard)
		{
			const wchar_t *wStrPtr=(const wchar_t *)GlobalLock(hClipBoard);
			if(NULL!=wStrPtr)
			{
				// It's FM-7.  Ignore all non-ASCII chars anyway.
				for(int i=0; 0!=wStrPtr[i]; ++i)
				{
					if(0<wStrPtr[i] && wStrPtr[i]<=0x7f)
					{
						clipboard.push_back((char)wStrPtr[i]);
					}
					else
					{
						clipboard.push_back(' ');
					}
				}
			}
			GlobalUnlock(hClipBoard);
		}
		CloseClipboard();
	}
	// ASCII Version
	else if(TRUE==IsClipboardFormatAvailable(CF_TEXT) &&
	   TRUE==OpenClipboard(NULL))
	{
		HANDLE hClipBoard=GetClipboardData(CF_TEXT);
		if(NULL!=hClipBoard)
		{
			const char *strPtr=(const char *)GlobalLock(hClipBoard);
			if(NULL!=strPtr)
			{
				for(int i=0; 0!=strPtr[i]; ++i)
				{
					clipboard.push_back(strPtr[i]);
				}
			}
			GlobalUnlock(hClipBoard);
		}
		CloseClipboard();
	}
#endif
	clipboard.push_back(0);
	return clipboard;
}
