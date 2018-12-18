#include <stdio.h>
#include <string>

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <fssimplewindow.h>
#include <fslazywindow.h>
#include "fslazywindowconnector.h"


////////////////////////////////////////////////////////////
#include <jni.h>
#include <android/log.h>
#include "fslazywindow_android.h"

static JNIEnv *jniEnv=nullptr;

extern "C" JNIEnv *FsLazyWindow_GetCurrentJNIEnv(void)
{
	return jniEnv;
}

/*! Called from ysgles2view.cpp
*/
extern "C" void FsLazyWindow_SetCurrentJNIEnv(JNIEnv *env)
{
	jniEnv=env;
}

////////////////////////////////////////////////////////////



void FsLazyWindowConnector_BeforeEverything(int argc,char *argv[])
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

	auto app=FsLazyWindowApplicationBase::GetApplication();
	app->BeforeEverything(argc,argv);

    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
}

void FsLazyWindowConnector_GetOpenWindowOption(FsOpenWindowOption &OPT)
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

	auto app=FsLazyWindowApplicationBase::GetApplication();
	app->GetOpenWindowOption(OPT);

    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
}

void FsLazyWindowConnector_Initialize(int argc,char *argv[])
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

	auto app=FsLazyWindowApplicationBase::GetApplication();
	app->Initialize(argc,argv);

    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
}

void FsLazyWindowConnector_Interval(void)
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

	auto app=FsLazyWindowApplicationBase::GetApplication();
	app->Interval();

    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
}

void FsLazyWindowConnector_BeforeTerminate(void)
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

	auto app=FsLazyWindowApplicationBase::GetApplication();
	app->BeforeTerminate();

    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
}

void FsLazyWindowConnector_Draw(void)
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

	auto app=FsLazyWindowApplicationBase::GetApplication();
	app->Draw();

    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
}

bool FsLazyWindowConnector_UserWantToCloseProgram(void)
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
	auto app=FsLazyWindowApplicationBase::GetApplication();
	auto res=app->UserWantToCloseProgram();
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
    return res;
}

bool FsLazyWindowConnector_MustTerminate(void)
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
	auto app=FsLazyWindowApplicationBase::GetApplication();
	auto res=app->MustTerminate();
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
    return res;
}

long long int FsLazyWindowConnector_GetMinimumSleepPerInterval(void)
{
	auto app=FsLazyWindowApplicationBase::GetApplication();
	auto res=app->GetMinimumSleepPerInterval();
	return res;
}

bool FsLazyWindowConnector_NeedRedraw(void)
{
	auto app=FsLazyWindowApplicationBase::GetApplication();
	auto res=app->NeedRedraw();
    return res;
}

bool FsLazyWindowConnector_StayIdleUntilEvent(void)
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
	auto app=FsLazyWindowApplicationBase::GetApplication();
	auto res=app->StayIdleUntilEvent();
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
    return res;
}

void FsLazyWindowConnector_ContextLostAndRecreated(void)
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
	auto app=FsLazyWindowApplicationBase::GetApplication();
	app->ContextLostAndRecreated();
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
}

