/* ////////////////////////////////////////////////////////////

File Name: fslazywindow_viewcontroller.m
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

#import "fslazywindow_viewcontroller.h"
#import <OpenGLES/ES2/glext.h>


#include <fssimplewindowios.h>
#include "fslazywindowios.h"


@interface FsLazyWindowViewController () {
}
@property (strong, nonatomic) EAGLContext *context;

- (void) reportToucheEvent:(NSSet<UITouch *> *)touches;

@end

@implementation FsLazyWindowViewController

- (void) loadView
{
	// 2017/03/22 Programatically create GLKView.
	//            Unnecessitated the steps of changing UIView to GLKView in the storyboard.
	//            When starting from Single-View application.
	GLKView *glkView=[[GLKView alloc] initWithFrame:CGRectMake(0,0,256,256)];
	self.view=glkView;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    float scale=[self.view contentScaleFactor];
    
    FsIOSBeforeEverything();
    {
        int x0,y0,wid,hei;
        FsIOSGetWindowSizeAndPreference(&x0,&y0,&wid,&hei);
    }

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create OpenGL ES 2.0 context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.multipleTouchEnabled=true;
    
    [EAGLContext setCurrentContext:self.context];

    {
        CGRect rect=[view frame];
        int wid=(int)(rect.size.width*scale);
        int hei=(int)(rect.size.height*scale);
        FsIOSReportWindowSize(wid,hei);
    }

    glClearColor(1,1,1,0);
    FsIOSInitialize();
}

- (void)dealloc
{
    FsIOSBeforeTerminate();

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)update
{
    // 2015/12/01
    // In iOS Simulator, [self.view frame] gives correct dimension after rotation
    // In actual device, [self.view frame] does not change after rotation.  [self.view bounds] give correct dimension.
    
    float scale=[self.view contentScaleFactor];
    // CGRect rect=[self.view frame];
    CGRect bound=[self.view bounds];
    int wid=(int)(bound.size.width*scale);
    int hei=(int)(bound.size.height*scale);
    FsIOSReportWindowSize(wid,hei);
    FsIOSCallInterval();

	if(true!=FsIOSNeedRedraw() && true==FsIOSStayIdleUntilEvent())
	{
		self.paused=true;
	}
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	static int c=0;
	printf("Frame count=%d\n",c++);
    FsIOSDraw();
}

- (void) reportToucheEvent:(NSSet<UITouch *> *)touches
{
    const int maxNumTouch=16;
    int nTouch=0;
    int touchCoord[maxNumTouch*2];
    float scale=[self.view contentScaleFactor];

    printf("%lu\n",(unsigned long)touches.count);
    for(UITouch *t in touches)
    {
        CGPoint p=[t locationInView:self.view];
        bool touchIsActive=false;
       
        printf("%f %f ",p.x*scale,p.y*scale);
        switch([t phase])
        {
            case UITouchPhaseBegan:
                printf("Began\n");
                touchIsActive=true;
                break;
            case UITouchPhaseMoved:
                printf("Moved\n");
                touchIsActive=true;
                break;
            case UITouchPhaseStationary:
                printf("Stationary\n");
                touchIsActive=true;
                break;
            case UITouchPhaseEnded:
                printf("Ended\n");
                touchIsActive=false;
                break;
            case UITouchPhaseCancelled:
                printf("Cancelled\n");
                touchIsActive=false;
                break;
        }

        if(nTouch<maxNumTouch && true==touchIsActive)
        {
            touchCoord[nTouch*2  ]=(int)(p.x*scale);
            touchCoord[nTouch*2+1]=(int)(p.y*scale);
            ++nTouch;
        }
    }

    FsIOSReportCurrentTouch(nTouch,touchCoord);
    self.paused=false;
}

- (void) touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    printf("%s\n",__FUNCTION__);
    [self reportToucheEvent:[event allTouches]];
    self.paused=false;
}

- (void) touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    printf("%s\n",__FUNCTION__);
    [self reportToucheEvent:[event allTouches]];
    self.paused=false;
}

- (void) touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    printf("%s\n",__FUNCTION__);
    [self reportToucheEvent:[event allTouches]];
    self.paused=false;
}

- (void) touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    printf("%s\n",__FUNCTION__);
    [self reportToucheEvent:[event allTouches]];
    self.paused=false;
}

@end
