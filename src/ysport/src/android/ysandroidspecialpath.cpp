/* ////////////////////////////////////////////////////////////

File Name: ysunixspecialpath.cpp
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

#include <unistd.h>
#include <sys/stat.h>

#include <ysclass.h>
#include "../ysspecialpath.h"

#include <jni.h>
#include <android/log.h>

// Assume the project includes YsAndroidConnector.java and YsAndroidConnector class instance
// is created in the constructor of MainActivity.

// Also Android JNI doesn't give a pointer to the running Java VM.
// Therefore it needs to get an environment pointer from FsLazyWindow library.
// Stupid, isnt' it?  Or maybe intentionallly making it difficult to create an independent library.
#include "android/fslazywindow_android.h"


YSRESULT YsSpecialPath::GetProgramFileName(YsString &path)
{
	int n;
	char buf[MaxPath];
	n=readlink("/proc/self/exe",buf,MaxPath-1);
	if(0<=n)
	{
		buf[n]=0;
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetProgramFileName(YsWString &wpath)
{
	YsString path;
	if(YSOK==GetProgramFileName(path))
	{
		wpath.SetUTF8String(path);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetProgramBaseDir(YsString &path)
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

	auto env=FsLazyWindow_GetCurrentJNIEnv();
    jclass viewClass=env->FindClass("lib/ysflight/YsAndroidConnector");

    jmethodID methodID=env->GetStaticMethodID(viewClass,"GetFilesDir","()Ljava/lang/String;");
    jstring dir=(jstring)env->CallStaticObjectMethod(viewClass,methodID);

    auto dirUTF=env->GetStringUTFChars(dir,nullptr);

	path=dirUTF;

    env->ReleaseStringUTFChars(dir,dirUTF);
    env->DeleteLocalRef(dir);

    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

	return YSOK;
}

YSRESULT YsSpecialPath::GetProgramBaseDir(YsWString &wpath)
{
	YsString path;
	if(YSOK==GetProgramBaseDir(path))
	{
		wpath.SetUTF8String(path);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserDir(YsString &path)
{
    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

	auto env=FsLazyWindow_GetCurrentJNIEnv();
    jclass viewClass=env->FindClass("lib/ysflight/YsAndroidConnector");

    jmethodID methodID=env->GetStaticMethodID(viewClass,"GetExternalFilesDir","()Ljava/lang/String;");
    jstring dir=(jstring)env->CallStaticObjectMethod(viewClass,methodID);

    auto dirUTF=env->GetStringUTFChars(dir,nullptr);

	path=dirUTF;

    env->ReleaseStringUTFChars(dir,dirUTF);
    env->DeleteLocalRef(dir);

    __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);

	return YSOK;
}

YSRESULT YsSpecialPath::GetUserDir(YsWString &wpath)
{
	YsString userDir;
	GetUserDir(userDir);
	wpath.SetUTF8String(userDir);
	return YSOK;
}

YSRESULT YsSpecialPath::GetUserDocDir(YsString &path)
{
	YsString homeDir;
	GetUserDir(homeDir);
	path.MakeFullPathName(homeDir,"Documents");
	mkdir(path,493); // 0755
	return YSOK;
}

YSRESULT YsSpecialPath::GetUserDocDir(YsWString &wpath)
{
	YsString path;
	if(YSOK==GetUserDocDir(path))
	{
		wpath.SetUTF8String(path);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserAppDataDir(YsString &path)
{
	YsString homeDir;
	GetUserDir(homeDir);
	path.MakeFullPathName(homeDir,"ApplicationData");
	mkdir(path,493); // 0755
	return YSOK;
}

YSRESULT YsSpecialPath::GetUserAppDataDir(YsWString &wpath)
{
	YsString path;
	if(YSOK==GetUserAppDataDir(path))
	{
		wpath.SetUTF8String(path);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetAllUsersDir(YsString &path)
{
	return YSERR;
}

YSRESULT YsSpecialPath::GetAllUsersDir(YsWString &path)
{
	return YSERR;
}

