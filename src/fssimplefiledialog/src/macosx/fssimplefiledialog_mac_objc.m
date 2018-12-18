/* ////////////////////////////////////////////////////////////

File Name: fssimplefiledialog_mac_objc.m
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

#import <Cocoa/Cocoa.h>
// #import AppKit?


// NSAutoReleasePool doesn't seem to exist if Auto Reference Count is on.
// It needs to be checked by __has_feature(objc_arc)
// Can't Apple just make NSAutoReleasePool a dummy class in that case?  
// Ahgh!  Objective-C is inferior to C++.
#ifndef __has_feature
#define __has_feature(x) 0
#endif


#include "fssimplefiledialog_mac.h"
#include <string.h>

static struct FsSimpleFileDialog_Mac_StringList *AddString(const char str[],struct FsSimpleFileDialog_Mac_StringList *currentList)
{
	struct FsSimpleFileDialog_Mac_StringList *newStr;
	newStr=(struct FsSimpleFileDialog_Mac_StringList *)malloc(sizeof(struct FsSimpleFileDialog_Mac_StringList));

	if(NULL!=newStr)
	{
		int len;
		len=strlen(str);

		newStr->fn=(char *)malloc(sizeof(char)*(len+1));
		if(NULL!=newStr->fn)
		{
			strcpy(newStr->fn,str);
			newStr->next=currentList;
			return newStr;
		}
		else
		{
			/* Something went wrong.  Don't change. */
			free(newStr);
			return currentList;
		}
	}
	return NULL;
}

static void DeleteStringList(struct FsSimpleFileDialog_Mac_StringList *list)
{
	struct FsSimpleFileDialog_Mac_StringList *next;
	while(NULL!=list)
	{
		next=list->next;
		free(list->fn);
		free(list);
		list=next;
	}
}

void FsSimpleFileDialog_Mac_InitDialog(struct FsSimpleFileDialog_Mac *dlg)
{
	dlg->in_multiSelect=0;
	dlg->in_mode=FSSIMPLEFILEDIALOG_MAC_MODE_OPEN;
	dlg->in_fileExtensionList=NULL;
	dlg->defaultPath="";
	dlg->defaultFile="";
	dlg->title="";

	dlg->out_result=0;
	dlg->out_selected=NULL;
}

void FsSimpleFileDialog_Mac_AddExtension(struct FsSimpleFileDialog_Mac *dlg,const char *ext)
{
	if(NULL!=ext)
	{
		dlg->in_fileExtensionList=AddString(ext,dlg->in_fileExtensionList);
	}
}

void FsSimpleFileDialog_Mac_OpenDialog(struct FsSimpleFileDialog_Mac *dlg)
{
#if !__has_feature(objc_arc)
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif


	NSPanel *panel=(FSSIMPLEFILEDIALOG_MAC_MODE_SAVE==dlg->in_mode ? [NSSavePanel savePanel] : [NSOpenPanel openPanel]);
	NSMutableArray *extArray=NULL;


	[panel setTitle:[NSString stringWithUTF8String:dlg->title]];
	[panel setTreatsFilePackagesAsDirectories:YES];

	if(FSSIMPLEFILEDIALOG_MAC_MODE_SAVE==dlg->in_mode)
	{
		[panel setCanCreateDirectories:YES];
		// [panel setExtensionHidden:NO];  // How can I do it before OSX 10.9?
		[panel setCanSelectHiddenExtension:YES];
	}
	else
	{
		[panel setCanCreateDirectories:NO];
		if(0!=dlg->in_multiSelect)
		{
			[panel setAllowsMultipleSelection:YES];
		}
		else
		{
			[panel setAllowsMultipleSelection:NO];
		}
	}

	if(NULL!=dlg->in_fileExtensionList)
	{
		struct FsSimpleFileDialog_Mac_StringList *ptr=dlg->in_fileExtensionList,*last=NULL;

		extArray=[[NSMutableArray alloc] init];

		while(NULL!=ptr)
		{
			const char *ext=ptr->fn;
			if('*'==ext[0])
			{
				++ext;
			}
			if('.'==ext[0])
			{
				++ext;
			}
			[extArray addObject:[[NSString alloc] initWithUTF8String:ext]];
			last=ptr;
			ptr=ptr->next;
		}

		[panel setAllowedFileTypes:extArray];
		[panel setAllowsOtherFileTypes:NO];
	}



	[panel setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:dlg->defaultPath]]];
	if(0!=dlg->defaultFile[0])
	{
		[panel setNameFieldStringValue:[NSString stringWithUTF8String:dlg->defaultFile]];
	}

	if(NSFileHandlingPanelOKButton==[panel runModal])
	{
		if(FSSIMPLEFILEDIALOG_MAC_MODE_SAVE==dlg->in_mode)
		{
			printf("[Selected]\n");
			printf("%s\n",[[[panel URL] path] UTF8String]);

			dlg->out_selected=AddString([[[panel URL] path] UTF8String],dlg->out_selected);
			dlg->out_result=1;
		}
		else
		{
			NSArray *fileArray=[panel URLs];
			if(nil!=fileArray)
			{
				printf("[Selected]\n");
				int idx;
				for(idx=0; idx<[fileArray count]; ++idx)
				{
					NSURL *url=[fileArray objectAtIndex:idx];
					printf("%s\n",[[url path] UTF8String]);
					dlg->out_selected=AddString([[url path] UTF8String],dlg->out_selected);
				}
				dlg->out_result=1;
			}
		}
	}
	else
	{
		dlg->out_result=0;
	}

	[[NSApp mainWindow] makeKeyAndOrderFront:nil];

#if !__has_feature(objc_arc)
	[pool release];
#endif
}

void FsSimpleFileDialog_Mac_CleanUp(struct FsSimpleFileDialog_Mac *dlg)
{
	DeleteStringList(dlg->in_fileExtensionList);
	DeleteStringList(dlg->out_selected);
}


