/* ////////////////////////////////////////////////////////////

File Name: ystexturemanager.h
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

#ifndef YSTEXTUREMANAGER_IS_INCLUDED
#define YSTEXTUREMANAGER_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysbase64.h>
#include <yseditarray.h>
#include <ysbitmap.h>

class YsTextureManager
{
friend class Unit;

private:
	YsTextureManager(const YsTextureManager &);
	YsTextureManager &operator=(const YsTextureManager &);

public:
	enum DATA_FORMAT
	{
		FOM_UNKNOWN,
		FOM_RAW_RGBA,
		FOM_PNG,
		FOM_RAW_Z,
	};
	static DATA_FORMAT GetFormatFromString(const char str[]);
	static const char *GetStringFromFormat(DATA_FORMAT fom);

	class TextureGenerationOption
	{
	public:
		/*! OpenGL ES, poor and incapable, can only accept 2^N x 2^N resolution textures.
		    In my opinion, OpenGL ES should be gone.  Portable devices can and should 
		    support full-scale OpenGL instead of cheap and bad variant called OpenGL ES.

	        If you are unforunate and need to support worst kind of implementation of
	        OpenGL ES, which only accept 2^N x 2^N resolution textures, turn on this option, 
	        and scale your texture coordinates by ScaleTexCoord function in the Unit.

		    OpenGL ES was supposed to be a temporary fix.  It should be thrown away by now.
		*/
		YSBOOL needUpScaling;
		TextureGenerationOption()
		{
			needUpScaling=YSFALSE;
		}
	};

	class ActualTexture; // API-specific.  Hope DirectX didn't exist :-P
	class Unit
	{
	friend class YsTextureManager;

	public:
		enum FILTERTYPE
		{
			FILTERTYPE_NEAREST,
			FILTERTYPE_LINEAR
		};

		enum
		{
			FLAG_RENDERTARGET=1,
			FLAG_DONTSAVE=2
		};

		/*!
		EFFECT_RANDOMNOISE         Add random noise
		EFFECT_ALPHAMASK_MAXRGB    Take alpha from max of RGB, and make RGB all 1.0
		EFFECT_ALPHA_FROM_MAXRGB   Take alpha from max of RGB, and do not change RGB
		*/
		enum EFFECTTYPE
		{
			EFFECT_NONE,
			EFFECT_RANDOMNOISE,
			EFFECT_ALPHAMASK_MAXRGB,
			EFFECT_ALPHA_FROM_MAXRGB
		};

	private:
		class YsTextureManager *owner;

		unsigned int flags;
		mutable ActualTexture *texPtr;
		YsString label;
		FILTERTYPE filterType;
		EFFECTTYPE effectType;
		DATA_FORMAT fType;
		double randomNoiseLevel;
		YsWString fn; // For refreshing in the editor.
		mutable int wid,hei; // It is more like a cache.
		mutable int upScaleWid,upScaleHei; // Poor OpenGL ES requires a texture to be 2^N x 2^N.
		mutable YsVec2 texCoordScale;      // Texture may need to be up-scaled for poor OpenGL ES.
		YsArray <unsigned char> dat;

		// IsTextureReady is used for checking if the texture on GPU has been erased outside.
		// bitmapLoaded is used for checking if the RGBA bitmap has already been transferred.
		mutable YSBOOL bitmapLoaded;

		Unit(const Unit &incoming);
		Unit &operator=(const Unit &incoming);
	public:
		Unit();
		~Unit();
		void CleanUp(void);
	public:
		void CleanTextureCache(void) const;
	private:
		/*! Copy texture data from incoming unit.
		    The actual texture that was originally stored in this unit will be cleared.
		    It does not make texture ready.
		    The label is copied from incoming texture.  However, it must be changed to a unique texture when added to the texture manager.
		*/
		Unit &CopyFrom(Unit &incoming);

		/*!  Set texture data.  
		     Parameters w and h are ignored unless fType is FOM_RAW_RGBA.
		*/
		YSRESULT SetData(DATA_FORMAT fType,int w,int h,YSSIZE_T length,const unsigned char dat[]);

		/*!  Set texture data.  
		     Parameters w and h are ignored unless fType is FOM_RAW_RGBA.
		*/
		YSRESULT SetData(DATA_FORMAT fType,int w,int h,YsConstArrayMask <unsigned char> dat);

		/*!  Make a texture for a shadow-map.
		     If w or h is zero, size will be the maximum texture size allowed.
		*/
		YSRESULT MakeShadowMap(int w,int h);

		YSRESULT SetLabel(const YsString &str);
	public:
		YSRESULT SetFileName(const YsWString &fn);
		YSBOOL IsActualTextureReady(void) const;

		/*! Returns if the texture has already been transferred to the GPU.
		*/
		YSBOOL IsBitmapLoaded(void) const;
	private:
		void ApplyRandomNoise(int wid,int hei,unsigned char rgba[]) const;
		void MakeAlphaMaskMaxValue(int wid,int hei,unsigned char rgba[]) const;
		void AlphaFromMaxRGB(int wid,int hei,unsigned char rgba[]) const;

	public:
		/*! If you need to support poor & incapable OpenGL ES, I feel your pain.
		    I hate it, too.  It should have been a temporary fix.  It should be gone by now.
		    Portable devices can and should support full-scale OpenGL instead of cheap and bad OpenGL ES.
		    But, if your OpenGL ES implementation is of the worst, and requires a texture to be 2^N x 2^N,
		    turn on needUpScaling option on, and scale your texture coordinate with this function.
		*/
		YsVec2 ScaleTexCoord(double u,double v) const;
		YsVec2 ScaleTexCoord(YsVec2 uv) const;

		/*! Transfers the bitmap to GPU and make it ready to use.
		    This function uses the default TextureGenerationOption.
		*/
		YSRESULT MakeActualTexture(void) const;

		/*! Transfers the bitmap to GPU with the given options and make it ready to use.
		*/
		YSRESULT MakeActualTexture(TextureGenerationOption opt) const;

	private:
		YSRESULT SetUpRGBABitmap(ActualTexture *texPtr,int wid,int hei,const unsigned char rgba[],const TextureGenerationOption &opt) const;
		int UpscaleDimension(int d) const;

	public:
		YSRESULT Bind(int texIdent=0) const; // API dependent.  Written in ystexturemanagerXX.cpp
		YSRESULT BindFrameBuffer(void) const; // API dependent.  Written in ystexturemanagerXX.cpp

		/*! Returns a bitmap of this texture unit.
		    It will be an empty bitmap if it is a render target.
		*/
		YsBitmap GetBitmap(void) const;

		/*! Returns the width.
		*/
		int GetWidth(void) const;
		
		/*! Returns the height.
		*/
		int GetHeight(void) const;
		/*! Returns the size.
		*/
		YsVec2i GetSize(void) const;

		/*! Turn on/off DONTSAVE flag. */
		void SetDontSave(YSBOOL dontSave);


		DATA_FORMAT GetFileType(void) const;
		ActualTexture *GetActualTexture(void) const;
		const YsString &GetLabel(void) const;
		FILTERTYPE GetFilterType(void) const;
		const YsWString &GetFileName(void) const;
		const YsArray <unsigned char> &GetFileData(void) const;

		const double GetRandomNoiseLevel(void) const;
		void SetRandomNoiseLevel(const double randomNoiseLevel);

		EFFECTTYPE GetEffectType(void) const;
		void SetEffectType(EFFECTTYPE effecttype);

		/*! Returns YSTRUE if the unit is a render target, such as depth map.
		    A render target will not be saved by YsTextureManager::Save function.
		*/
		YSBOOL IsRenderTarget(void) const;

		/*! Returns YSTRUE if the unit must not be saved to a file.
		    If it is a render target, this function returns YSTRUE.
		    Or, DONTSAVE flag is set, this function returns YSTRUE.
		*/
		YSBOOL MustNotBeSavedToFile(void) const;
	};
private:
	YsEditArray <Unit,8> bmpArray;
	YsDictionary <YsString,YsEditArrayObjectHandle <Unit,8> > bmpDict;
	TextureGenerationOption genOpt;

	static ActualTexture *Alloc(void);         // Written in ystexturemanagergl.h/cpp or ystexturemanager_d3d9.h/cpp
	static void Delete(ActualTexture *texPtr); // Written in ystexturemanagergl.h/cpp or ystexturemanager_d3d9.h/cpp
	static YSRESULT TransferRGBABitmap(ActualTexture *texPtr,int wid,int hei,const unsigned char rgba[]); // Written in // Written in ystexturemanagergl.h/cpp or ystexturemanager_d3d9.h/cpp
	static YSRESULT CreateDepthMapRenderTarget(ActualTexture *texPtr,int wid,int hei); // Written in // Written in ystexturemanagergl.h/cpp or ystexturemanager_d3d9.h/cpp
	static YSBOOL IsTextureReady(ActualTexture *texPtr); // Written in ystexturemanagergl.h/cpp or ystexturemanager_d3d9.h/cpp
public:
	YsTextureManager();

	void CleanUp(void);

	/*! Call this function when graphics context is lost due to an irresponsible implementation of the driver.
	    This will keep all handles valid, but internally deletes the texture.
	    Whoever uses the handle can re-create textures next time.
	*/
	void ContextLost(void);

	/*! Set texture-generation option.
	*/
	void SetTextureGenerationOption(TextureGenerationOption opt);

	/*! Get texture-generation option.
	*/
	TextureGenerationOption GetTextureGenerationOption(void) const;


	typedef YsEditArrayObjectHandle <Unit,8> TexHandle;

	YSBOOL IsLabelUsed(const YsString &str) const;

	/*! Creates a label that is not used by any texture. */
	YsString MakeUniqueLabel(void) const;

	/*! Adds a texture.  The content of the texture is empty, and it cannot be bound until the file data is set.
	    It cannot be changed to the Shadow-Map texture.
	    The label must be unique.
	    If the given label is an empty string, it creates a unique label using MakeUniqueLabel function.
	    If the given label is already used, this function will fail and return nullptr.
	*/
	TexHandle AddTexture(const YsString &label);


	/*! Adds a texture.
	    The label must be unique.
	    If the given label is an empty string, it creates a unique label using MakeUniqueLabel function.
	    If the given label is already used, this function will fail and return nullptr.
	    Parameters w and h are ignored unless fType is FOM_RAW_RGBA.
	*/
	TexHandle AddTexture(const YsString &label,DATA_FORMAT fType,int w,int h,YSSIZE_T length,const unsigned char fileData[]);

	/*! Adds a texture.
	    The label must be unique.
	    If the given label is an empty string, it creates a unique label using MakeUniqueLabel function.
	    If the given label is already used, this function will fail and return nullptr.
	    Parameters w and h are ignored unless fType is FOM_RAW_RGBA.
	*/
	TexHandle AddTexture(const YsString &label,DATA_FORMAT fType,int w,int h,YsConstArrayMask <unsigned char> dat);

	/*! Add a texture for a Shadow-Map.
	    The label must be unique.
	    If the given label is an empty string, it creates a unique label using MakeUniqueLabel function.
	    If the given label is already used, this function will fail and return nullptr.
	    If parameters w or h is zero, the size is the maximum texture size allowed.
	*/
	TexHandle AddShadowMapTexture(const YsString &label,DATA_FORMAT fType,int w,int h);

	/*! Deletes a texture. */
	void Delete(TexHandle texHd);

	/*! Duplicates a texture.
	    New texture will not be ready until calling MakeActualTexture, regardless of the state of the incoming texture.
	    The label must be unique.
	    If the given label is an empty string, it creates a unique label using MakeUniqueLabel function.
	    If the given label is already used, this function will fail and return nullptr.
	*/
	TexHandle Duplicate(TexHandle texHd,const YsString &label);

	/*! Finds a texture handle that matches the label. */
	TexHandle FindTextureFromLabel(const YsString &label) const;

	YsString GetLabel(TexHandle texHd) const;

	YSRESULT FreezeTexture(TexHandle texHd);
	YSRESULT MeltTexture(TexHandle texHd);

	YsEditArray <Unit,8>::HandleEnumerator AllTexture(void) const;
	DATA_FORMAT GetTextureFormat(TexHandle texHd) const;

	/*! This function returns the texture size.
	    The size is valid only for FOM_RAW_RGBA.
	    If the file type is not FOM_RAW_RGBA, the return value is zero.
	*/
	YsVec2i GetTextureSize(TexHandle texHd) const;

	YSRESULT SetTextureFileName(TexHandle texHd,const YsWString &fn);
	YsWString GetTextureFileName(TexHandle texHd) const;

	YSRESULT SetTextureFilterType(TexHandle texHd,Unit::FILTERTYPE filterType);
	const Unit::FILTERTYPE GetTextureFilterType(TexHandle texHd) const;

	const YsArray <unsigned char> &GetTextureFileData(TexHandle texHd) const;

	/*!  Set texture data.  
	     Parameters w and h are ignored unless fType is FOM_RAW_RGBA.
	*/
	YSRESULT SetTextureFileData(TexHandle texHd,DATA_FORMAT fType,int w,int h,YSSIZE_T length,const unsigned char fileData[]);

	/*! Turn on/off DONTSAVE flag. */
	YSRESULT SetDontSave(TexHandle texHd,YSBOOL dontSave);

	/*!  Set texture data.  
	     Parameters w and h are ignored unless fType is FOM_RAW_RGBA.
	*/
	YSRESULT SetTextureFileData(TexHandle texHd,DATA_FORMAT fType,int w,int h,YsConstArrayMask <unsigned char> fileData);

	/*! Sets the effect type.
	*/
	YSRESULT SetTextureEffectType(TexHandle texHd,Unit::EFFECTTYPE effectType);

	/*! Returns the effect type.
	*/
	Unit::EFFECTTYPE GetTextureEffectType(TexHandle texHd) const;

	const double GetRandomNoiseLevel(TexHandle texHd) const;
	YSRESULT SetRandomNoiseLevel(TexHandle texHd,const double randomNoiseLevel);

	const Unit *GetTexture(TexHandle texHd) const;

	/*! Make the texture ready and return the pointer to the texture unit.
	    Returns nullptr if the texHd is nullptr or the texture could not be made ready.
	*/
	const Unit *GetTextureReady(TexHandle texHd) const;


	/*! Returns YSTRUE if texture texHd is ready to use.
	    A texture may be invalidated outside the application's control.
	    Direct X9 didn't want to be responsible for maintaining the context when the window is resized.
	    ANGLE library also does seem to forget everything upon power management event.
	    In those cases, this function may return YSFALSE for a texture that was perfectly good while drawing the previous frame.
	    If texHd is nullptr, it returns YSFALSE.
	*/
	YSBOOL IsReady(TexHandle texHd) const;


	/*! Unbinds a texture.  In OpenGL, some graphics drivers seem to require shadow-map texture be unbound before writing to it.
	    This function is for that purpose.
	*/
	void Unbind(int texIdent=0) const; // Written in an API-specific source.


	YSRESULT Save(YsTextOutputStream &outStream) const;
	void BeginLoadTexMan(void);
	YSRESULT LoadTexManOneLine(const YsString &str,YSSIZE_T argc,const YsString args[]);
	void EndLoadTexMan(void);
private:
	enum LOADINGSTATE
	{
		LOADINGSTATE_NORMAL,
		LOADINGSTATE_TEXTUREDATA,
	};

	LOADINGSTATE loadingState;
	YsString loadingTexFileName;
	DATA_FORMAT loadingTexFType;
	YsVec2i loadingTexSize;
	double loadingTexRandomNoiseLevel;
	YsString loadingTexLabel;
	YsArray <unsigned char> loadingTexData;
	int nLoadingTexDataLine;
	YsBase64Decoder base64decoder;
	Unit::FILTERTYPE loadingFilterType;
	Unit::EFFECTTYPE loadingEffectType;

	void ClearLoadingInfo(void);
};

/* } */
#endif
