//
// Created by Soji Yamakawa on 2018/05/13.
//

#include <jni.h>
#include <ysgl.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>
#include <android/log.h>
#include <android/fssimplewindowconnector.h>
#include <android/fslazywindowconnector.h>
#include <android/fslazywindow_android.h>
#include <android/fssimplewindow_android.h>

#include <thread>
#include <mutex>

#define PACKAGE_NAME lib_ysflight

// Android GLSurfaceView creates UI thread and Graphics thread.  I didn't ask for it.
// The program can easily crash if UI-thread function enters while Graphics-thread function is still doing something.
// For preventing crash.
std::mutex reentranceLock;

extern "C" JNIEXPORT void JNICALL Java_lib_ysflight_YsGLES2View_YsInitialize(JNIEnv *env,jobject thisPtr)
{
	std::lock_guard <std::mutex> lock(reentranceLock);

    FsLazyWindow_SetCurrentJNIEnv(env);

    static bool first=true;
    if(true==first)
    {
        first=false;
        char *argv[1],dummyCmd[256]="argv0";
        argv[0]=dummyCmd;

        __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

        jclass viewClass=env->GetObjectClass(thisPtr);

        jmethodID getFilesDirMethod=env->GetMethodID(viewClass,"GetFilesDir","()Ljava/lang/String;");
        jstring filesDir=(jstring)env->CallObjectMethod(thisPtr,getFilesDirMethod);

        jmethodID getCacheDirMethod=env->GetMethodID(viewClass,"GetCacheDir","()Ljava/lang/String;");
        jstring cacheDir=(jstring)env->CallObjectMethod(thisPtr,getFilesDirMethod);

        auto filesDirUTF=env->GetStringUTFChars(filesDir,nullptr);
        auto cacheDirUTF=env->GetStringUTFChars(cacheDir,nullptr);
        FsSimpleWindowConnector_NotifyFilesDirAndCacheDir(filesDirUTF,cacheDirUTF);
        env->ReleaseStringUTFChars(filesDir,filesDirUTF);
        env->ReleaseStringUTFChars(cacheDir,cacheDirUTF);

        __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

        FsLazyWindowConnector_BeforeEverything(1,argv);
        FsOpenWindowOption opt;
        FsLazyWindowConnector_GetOpenWindowOption(opt);
        FsLazyWindowConnector_Initialize(1,argv);
    }
    else
    {
        FsLazyWindowConnector_ContextLostAndRecreated();
    }
}

extern "C" JNIEXPORT jboolean JNICALL Java_lib_ysflight_YsGLES2View_YsInterval(JNIEnv *env,jobject /* this */)
{
	std::lock_guard <std::mutex> lock(reentranceLock);

    FsLazyWindow_SetCurrentJNIEnv(env);
    FsLazyWindowConnector_Interval();
    return (jboolean)FsLazyWindowConnector_NeedRedraw();
}

static int wid=1,hei=1;

extern "C" JNIEXPORT void JNICALL Java_lib_ysflight_YsGLES2View_YsNotifyWindowSize(JNIEnv *env,jobject /* this */,jint widIn,jint heiIn)
{
	std::lock_guard <std::mutex> lock(reentranceLock);

    wid=widIn;
    hei=heiIn;
    FsLazyWindow_SetCurrentJNIEnv(env);
    FsSimpleWindowConnector_NotifyWindowSize(widIn,heiIn);
}

extern "C" JNIEXPORT void JNICALL Java_lib_ysflight_YsGLES2View_YsDraw(JNIEnv *env,jobject /* this */)
{
	std::lock_guard <std::mutex> lock(reentranceLock);

/*    glClearColor(0,0,1,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0,0,wid,hei);

    YsGLSL2DRenderer renderer;
    float col[4]={0,0,0,1};
    renderer.SetUniformColor(col);
    renderer.UseWindowCoordinateTopLeftAsOrigin();
    for(int x=0; x<wid; x+=80)
    {
        float vtx[4] = {(float)x,0,(float)x,(float)hei};
        renderer.DrawVtx(GL_LINES, 2, vtx);
    }
    for(int y=0; y<hei; y+=80)
    {
        float vtx[4] = {0, (float)y,(float)wid,(float)y};
        renderer.DrawVtx(GL_LINES, 2, vtx);
    } */
    FsLazyWindow_SetCurrentJNIEnv(env);
    FsLazyWindowConnector_Draw();
}

extern "C" JNIEXPORT void JNICALL Java_lib_ysflight_YsGLES2View_YsNotifyTouch(JNIEnv *env,jobject /* this */,jintArray xy)
{
	std::lock_guard <std::mutex> lock(reentranceLock);

    jsize len=env->GetArrayLength(xy);
    jint *data=env->GetIntArrayElements(xy,0);

    FsLazyWindow_SetCurrentJNIEnv(env);
    FsSimpleWindowConnector_NotifyTouchState(len/2,(int *)data);

    env->ReleaseIntArrayElements(xy,data,0);
}

extern "C" JNIEXPORT void JNICALL Java_lib_ysflight_YsGLES2View_YsOnPause(JNIEnv *env,jobject /* this */)
{
	std::lock_guard <std::mutex> lock(reentranceLock);
    FsLazyWindow_SetCurrentJNIEnv(env);
}

extern "C" JNIEXPORT void JNICALL Java_lib_ysflight_YsGLES2View_YsOnResume(JNIEnv *env,jobject /* this */)
{
	std::lock_guard <std::mutex> lock(reentranceLock);
    FsLazyWindow_SetCurrentJNIEnv(env);
}
