#ifndef FSLAZYWINDOW_ANDROID_IS_INCLUDED
#define FSLAZYWINDOW_ANDROID_IS_INCLUDED
/* { */

#include <jni.h>

extern "C" JNIEnv *FsLazyWindow_GetCurrentJNIEnv(void);

/*! Called from ysgles2view.cpp
*/
extern "C" void FsLazyWindow_SetCurrentJNIEnv(JNIEnv *env);

/* } */
#endif
