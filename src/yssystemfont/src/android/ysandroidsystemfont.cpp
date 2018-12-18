#include <jni.h>
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_INFO,"Ys",__VA_ARGS__);


#include "../yssystemfont.h"


#include <android/fslazywindow_android.h>


// Requires FsLazyWindow for Android
// Requires lib.ysflight.YsFontRenderer.java


class YsSystemFontCache::InternalData
{
private:
	int fontHeight;

public:
	InternalData();
	~InternalData();
	void Initialize(void);
	void CleanUp(void);
	void RequestFontHeight(int fontHeight);

	unsigned char *RenderTextToBitmap(int &w,int &h,const wchar_t msg[],const unsigned char fgCol[3],const unsigned char bgCol[3]);
	YSRESULT GetTightBitmapSize(int &wid,int &hei,const wchar_t str[]) const;
};
YsSystemFontCache::InternalData::InternalData()
{
	fontHeight=64;
}
YsSystemFontCache::InternalData::~InternalData()
{
	CleanUp();
}
void YsSystemFontCache::InternalData::Initialize(void)
{
	CleanUp();
}
void YsSystemFontCache::InternalData::CleanUp(void)
{
}
void YsSystemFontCache::InternalData::RequestFontHeight(int fontHeight)
{
	this->fontHeight=fontHeight;
}
unsigned char *YsSystemFontCache::InternalData::RenderTextToBitmap(int &wid,int &hei,const wchar_t wMsg[],const unsigned char fgCol[3],const unsigned char bgCol[3])
{
	/* Want to do this, but NDK apparently does not support JNI_GetCreatedJavaVMs
	JavaVM *vm[16]={nullptr};
	jsize nVm=0;
	if(JNI_OK!=JNI_GetCreatedJavaVMs(vm,16,&nVm) || 0==nVm)
	{
        __android_log_write(ANDROID_LOG_ERROR,"Error","VM not running.");
		return nullptr;
	}
	JNIEnv *env=nullptr;
	vm[0]->GetEnv((void **)&env,JNI_VERSION_1_6); */

	auto env=FsLazyWindow_GetCurrentJNIEnv();

    jclass cls=env->FindClass("lib/ysflight/YsFontRenderer");
    // Function signature: http://journals.ecs.soton.ac.uk/java/tutorial/native1.1/implementing/method.html
    jmethodID mid=env->GetStaticMethodID(cls,"RenderFont","(IILjava/lang/String;)[I");
    if(nullptr==mid)
    {
        __android_log_write(ANDROID_LOG_ERROR,"Error","RenderFont method not found.");
        return nullptr;
    }

	int len=0;
	for(len=0; 0!=wMsg[len]; ++len)
	{
	}
	// Should len be number of bytes or number of letters?
	// No documentation.  Is it undefined officially?  Indeed.  Java is an inferior language.
	// I wish Java extincts and C++ dominates.
    jstring str=env->NewString((const jchar *)wMsg,len*2);
	unsigned int argb=0xff000000+(fgCol[0]<<16)+(fgCol[1]<<8)+fgCol[2];
    jobject intArrayObj=env->CallStaticObjectMethod(cls,mid,(jint)fontHeight,(jint)argb,str);
    env->DeleteLocalRef(str);
    jintArray intArray=(jintArray)intArrayObj;

    jint *intPtr=env->GetIntArrayElements(intArray,nullptr);
    jsize length=env->GetArrayLength(intArray);

	wid=intPtr[0];
	hei=intPtr[1];

	auto rgba=new unsigned char [wid*hei*4];
	for(int y=0; y<hei; ++y)
	{
		auto dstTop=rgba+4*wid*y;
		auto srcTop=intPtr+wid*y;
		for(int x=0; x<wid; ++x)
		{
			auto col=srcTop[x];
			dstTop[x*4  ]=(col>>16)&255;
			dstTop[x*4+1]=(col>> 8)&255;
			dstTop[x*4+2]=(col    )&255;
			dstTop[x*4+3]=(col>>24)&255;
		}
	}

    env->ReleaseIntArrayElements(intArray,intPtr,0);
    env->DeleteLocalRef(intArrayObj);
	return rgba;
}

YSRESULT YsSystemFontCache::InternalData::GetTightBitmapSize(int &wid,int &hei,const wchar_t wMsg[]) const
{
	/* Want to do this, but JNI_GetCreatedJavaVMs not supported in NDK apparently.
	JavaVM *vm[16]={nullptr};
	jsize nVm=0;
	if(JNI_OK!=JNI_GetCreatedJavaVMs(vm,16,&nVm) || 0==nVm)
	{
        __android_log_write(ANDROID_LOG_ERROR,"Error","VM not running.");
		return YSERR;
	}
	JNIEnv *env=nullptr;
	vm[0]->GetEnv((void **)&env,JNI_VERSION_1_6); */
	auto env=FsLazyWindow_GetCurrentJNIEnv();


    jclass cls=env->FindClass("lib/ysflight/YsFontRenderer");
    // Function signature: http://journals.ecs.soton.ac.uk/java/tutorial/native1.1/implementing/method.html
    jmethodID mid=env->GetStaticMethodID(cls,"RenderFont","(IILjava/lang/String;)[I");
    if(nullptr==mid)
    {
        __android_log_write(ANDROID_LOG_ERROR,"Error","RenderFont method not found.");
        return YSERR;
    }

	int len=0;
	for(len=0; 0!=wMsg[len]; ++len)
	{
	}
    jstring str=env->NewString((const jchar *)wMsg,len*2);
	unsigned int argb=0xffffffff;
    jobject intArrayObj=env->CallStaticObjectMethod(cls,mid,(jint)fontHeight,(jint)argb,str);
    env->DeleteLocalRef(str);
    jintArray intArray=(jintArray)intArrayObj;

    jint *intPtr=env->GetIntArrayElements(intArray,nullptr);
    jsize length=env->GetArrayLength(intArray);

    wid=intPtr[0];
    hei=intPtr[1];

    env->ReleaseIntArrayElements(intArray,intPtr,0);
    env->DeleteLocalRef(intArrayObj);
	return YSOK;
}

////////////////////////////////////////////////////////////

YsSystemFontCache::YsSystemFontCache()
{
	internal=new YsSystemFontCache::InternalData;
	internal->Initialize();
}

YsSystemFontCache::~YsSystemFontCache()
{
	delete internal;
}

YSRESULT YsSystemFontCache::RequestDefaultFont(void)
{
	internal->Initialize();
	return YSERR;
}

YSRESULT YsSystemFontCache::RequestDefaultFontWithHeight(int height)
{
	internal->RequestFontHeight(height);
	return YSOK;
}

YSRESULT YsSystemFontCache::MakeSingleBitBitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],YSBOOL reverse) const
{
	const unsigned char fgCol[3]={255,255,255},bgCol[3]={0,0,0};
	int w,h;
	auto rgba=internal->RenderTextToBitmap(w,h,wStr,fgCol,bgCol);

	const int bitPerPixel=1;
	int bytePerLine=(w+7)&(~7);
	auto *bitArray=new unsigned char [bytePerLine*h];
	for(int y=0; y<h; ++y)
	{
		auto rgbaTop=(YSTRUE!=reverse ? rgba+y*w*4 : rgba+(h-1-y)*4);
		auto bitTop=bitArray+y*bytePerLine;
		unsigned char orBit=0x80;
		for(int x=0; x<w; ++x)
		{
			if(0==x%8)
			{
				bitArray[x/8]=0;
			}
			if(255==rgbaTop[x*4])
			{
				bitTop[x/8]|=(0x80>>(x%8));
			}
		}
	}
	delete [] rgba;
	bmp.SetBitmap(w,h,bytePerLine,bitPerPixel,bitArray);
	return YSOK;
}

YSRESULT YsSystemFontCache::MakeRGBABitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],const unsigned char fgCol[3],const unsigned char bgCol[3],YSBOOL reverse) const
{
	int w,h;
	auto rgba=internal->RenderTextToBitmap(w,h,wStr,fgCol,bgCol);

	if(YSTRUE==reverse)
	{
		for(int y=0; y<h/2; ++y)
		{
			auto lineTop0=rgba+y*w*4;
			auto lineTop1=rgba+(h-1-y)*w*4;
			for(int i=0; i<w*4; ++i)
			{
				auto c=lineTop0[i];
				lineTop0[i]=lineTop1[i];
				lineTop1[i]=c;
			}
		}
	}

	auto bytePerLine=w*4;
	auto bitPerPixel=32;
	bmp.SetBitmap(w,h,bytePerLine,bitPerPixel,rgba);
	return YSOK;
}

YSRESULT YsSystemFontCache::MakeGrayScaleAndAlphaBitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],unsigned char fgColS,unsigned char bgColS,YSBOOL reverse) const
{
	const unsigned char fgCol[3]={fgColS,fgColS,fgColS},bgCol[3]={bgColS,bgColS,bgColS};
	int w,h;
	auto rgba=internal->RenderTextToBitmap(w,h,wStr,fgCol,bgCol);

	const int bytePerLine=2*w;
	const int bitPerPixel=16;

	unsigned char *ra=new unsigned char [bytePerLine*h];
	for(int y=0; y<h; ++y)
	{
		auto rgbaTop=(YSTRUE!=reverse ? rgba+y*4 : rgba+(h-1-y)*4);
		auto raTop=ra+bytePerLine*y;
		for(int x=0; x<w; ++x)
		{
			raTop[x*2  ]=rgbaTop[x*4];
			raTop[x*2+1]=rgbaTop[x*4+3];
		}
	}
	delete [] rgba;

	bmp.SetBitmap(w,h,bytePerLine,bitPerPixel,ra);
	return YSOK;
}

YSRESULT YsSystemFontCache::GetTightBitmapSize(int &wid,int &hei,const wchar_t str[]) const
{
	return internal->GetTightBitmapSize(wid,hei,str);
}
