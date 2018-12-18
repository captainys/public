/* ////////////////////////////////////////////////////////////

File Name: macosxsample.m
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

/* gcc macosxsample.m ysglfontdata.c ysglusefontbitmap.c -DMACOSX -framework Cocoa -framework OpenGL */

#import <Cocoa/Cocoa.h>
#include <OpenGL/OpenGL.h>

#include "ysglfontdata.h"


@interface YsOpenGLWindow : NSWindow
{
}

@end

@implementation YsOpenGLWindow
- (id) initWithContentRect: (NSRect)rect styleMask:(NSUInteger)wndStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferFlg
{
	[super initWithContentRect:rect styleMask:wndStyle backing:bufferingType defer:deferFlg];

	[[NSNotificationCenter defaultCenter]
	  addObserver:self
	  selector:@selector(windowWillClose:)
	  name:NSWindowWillCloseNotification
	  object:self];

	return self;
}

- (void) windowWillClose: (NSNotification *)notification
{
	[NSApp terminate:nil];
}

@end


@interface YsOpenGLView : NSOpenGLView 
{
}
- (void) prepareOpenGL;
- (void) drawRect: (NSRect) bounds;
@end

@implementation YsOpenGLView
- (void) prepareOpenGL
{
	YsGlUseFontBitmap6x7(0x400);
	YsGlUseFontBitmap6x8(0x500);
	YsGlUseFontBitmap8x8(0x600);
	YsGlUseFontBitmap8x12(0x700);
	YsGlUseFontBitmap12x16(0x800);
	YsGlUseFontBitmap16x20(0x900);
	YsGlUseFontBitmap16x24(0xA00);
	YsGlUseFontBitmap20x28(0xB00);
	YsGlUseFontBitmap20x32(0xC00);
}

-(void) drawRect: (NSRect) bounds
{
	NSRect rect;
	char *str;

	rect=[self frame];

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);

	int wid,hei;
	wid=rect.size.width;
	hei=rect.size.height;
	printf("%d %d\n",wid,hei);
	glViewport(0,0,wid,hei);

	glDepthFunc(GL_ALWAYS);
	glDepthMask(0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5,(GLdouble)wid-0.5,(GLdouble)hei-0.5,-0.5,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);

	glColor3ub(255,255,255);

	glListBase(0x400);
	glRasterPos2i(0,7);
	str="6x7 Font ABCDEFG012345";
	glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

	glListBase(0x500);
	glRasterPos2i(0,15);
	str="6x8 Font ABCDEFG012345";
	glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

	glListBase(0x600);
	glRasterPos2i(0,23);
	str="8x8 Font ABCDEFG012345";
	glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

	glListBase(0x700);
	glRasterPos2i(0,35);
	str="8x12 Font ABCDEFG012345";
	glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

	glListBase(0x800);
	glRasterPos2i(0,51);
	str="12x16 Font ABCDEFG012345";
	glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

	glListBase(0x900);
	glRasterPos2i(0,71);
	str="16x20 Font ABCDEFG012345";
	glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

	glListBase(0xA00);
	glRasterPos2i(0,95);
	str="16x24 Font ABCDEFG012345";
	glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

	glListBase(0xB00);
	glRasterPos2i(0,123);
	str="20x28 Font ABCDEFG012345";
	glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

	glListBase(0xC00);
	glRasterPos2i(0,155);
	str="20x32 Font ABCDEFG012345";
	glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

	glFlush();
}
@end

static YsOpenGLWindow *ysWnd=nil;
static YsOpenGLView *ysView=nil;

static void YsOpenWindow(void)
{
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];


	[NSApplication sharedApplication];
	[NSBundle loadNibNamed:@"MainMenu" owner:NSApp];

	[NSApp finishLaunching];

	NSRect contRect;
	contRect=NSMakeRect(32, 32, 800, 600);
	
	unsigned int winStyle=
	  NSTitledWindowMask|
	  NSClosableWindowMask|
	  NSMiniaturizableWindowMask|
	  NSResizableWindowMask;
	
	ysWnd=[YsOpenGLWindow alloc];
	[ysWnd
		initWithContentRect:contRect
		styleMask:winStyle
		backing:NSBackingStoreBuffered 
		defer:NO];

	NSOpenGLPixelFormat *format;
	NSOpenGLPixelFormatAttribute formatAttrib[]=
	{
		NSOpenGLPFAWindow,
		NSOpenGLPFADepthSize,(NSOpenGLPixelFormatAttribute)32,
		/* NSOpenGLPFADoubleBuffer, */
		0
	};
	format=[NSOpenGLPixelFormat alloc];
	[format initWithAttributes: formatAttrib];
	
	ysView=[YsOpenGLView alloc];
	contRect=NSMakeRect(0,0,800,600);
	[ysView
		initWithFrame:contRect
		pixelFormat:format];
	
	[ysWnd setContentView:ysView];
	[ysWnd makeFirstResponder:ysView];

	[ysWnd makeKeyAndOrderFront:nil];
	[ysWnd makeMainWindow];

	[NSApp activateIgnoringOtherApps:YES];

	[pool release];
}

void YsAddMenu(void)
{
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];

	NSMenu *mainMenu;

	mainMenu=[NSMenu alloc];
	[mainMenu initWithTitle:@"Minimum"];

	NSMenuItem *fileMenu;
	fileMenu=[[NSMenuItem alloc] initWithTitle:@"File" action:NULL keyEquivalent:[NSString string]];
	[mainMenu addItem:fileMenu];

	NSMenu *fileSubMenu;
	fileSubMenu=[[NSMenu alloc] initWithTitle:@"FileMenu"];
	[fileMenu setSubmenu:fileSubMenu];

	NSMenuItem *fileMenu_Quit;
	fileMenu_Quit=[[NSMenuItem alloc] initWithTitle:@"Quit"  action:@selector(terminate:) keyEquivalent:@"q"];
	[fileMenu_Quit setTarget:NSApp];
	[fileSubMenu addItem:fileMenu_Quit];

	[NSApp setMainMenu:mainMenu];

	[pool release];
}

void YsSleep(int ms)
{
	double sec;
	sec=(double)ms/1000.0;
	[NSThread sleepForTimeInterval:0.02];
}

int main(int argc, char *argv[])
{
	YsOpenWindow();
	YsAddMenu();
	[NSApp run];
	return 0;
}
