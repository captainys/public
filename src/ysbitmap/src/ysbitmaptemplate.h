/* ////////////////////////////////////////////////////////////

File Name: ysbitmaptemplate.h
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

#ifndef YSBITMAPTEMPLATE_IS_INCLUDED
#define YSBITMAPTEMPLATE_IS_INCLUDED
/* { */

#ifndef YSRESULT_IS_DEFINED
#define YSRESULT_IS_DEFINED
typedef enum
{
	YSERR,
	YSOK
} YSRESULT;
#endif

#ifndef YSBOOL_IS_DEFINED
#define YSBOOL_IS_DEFINED
/*! Enum for boolearn. */
typedef enum
{
	YSFALSE,     /*!< False */
	YSTRUE,      /*!< True */
	YSTFUNKNOWN  /*!< Unable to tell true or false. */
} YSBOOL;
#endif

template <class ComponentType,int NumComponentPerPixel>
class YsBitmapTemplate
{
private:
	YsBitmapTemplate <ComponentType,NumComponentPerPixel> &operator=(const YsBitmapTemplate <ComponentType,NumComponentPerPixel> &);
	YsBitmapTemplate <ComponentType,NumComponentPerPixel> &operator=(const YsBitmapTemplate <ComponentType,NumComponentPerPixel> &&);
	YsBitmapTemplate(const YsBitmapTemplate <ComponentType,NumComponentPerPixel> &);
	YsBitmapTemplate(const YsBitmapTemplate <ComponentType,NumComponentPerPixel> &&);

	int nx,ny;
	ComponentType *bmpPtr;
	ComponentType **lineTop;

protected:
	ComponentType *LineTop(int y)
	{
		return lineTop[y];
	}
	const ComponentType *LineTop(int y) const
	{
		return lineTop[y];
	}

public:
	YsBitmapTemplate()
	{
		nx=0;
		ny=0;
		bmpPtr=nullptr;
		lineTop=nullptr;
	}
	~YsBitmapTemplate()
	{
		CleanUp();
	}

	/*! Clears the bitmap.  This bitmap will become empty. */
	void CleanUp(void)
	{
		nx=0;
		ny=0;
		if(nullptr!=bmpPtr)
		{
			delete [] bmpPtr;
			bmpPtr=nullptr;
		}
		if(nullptr!=lineTop)
		{
			delete [] lineTop;
			lineTop=nullptr;
		}
	}

	/*! Cuts out a rectangular area of this bitmap, (thisX0,thisY0)-(thisX0+w-1,thisY0+h-1) ,into destination bitmap.
	    The pixels outside of this bitmap will be filled by the clearColor values.
	*/
	YSRESULT CutOut(YsBitmapTemplate <ComponentType,NumComponentPerPixel> &destination,int thisX0,int thisY0,int wid,int hei,ComponentType clearColor) const
	{
		destination.Create(wid,hei);

		int destY=0;
		int thisY=thisY0;
		if(thisY<0)
		{
			int fillHeight=-thisY;
			if(hei<fillHeight)
			{
				fillHeight=hei;
			}
			for(int i=0; i<fillHeight*destination.GetNumComponentPerLine(); ++i)
			{
				destination.bmpPtr[i]=clearColor;
			}
			destY+=fillHeight;
			thisY+=fillHeight;
		}

		while(thisY<this->GetHeight() && destY<hei)
		{
			auto thisPtr=this->LineTop(thisY);
			auto destPtr=destination.LineTop(destY);

			int destX=0;
			int thisX=thisX0;
			if(thisX<0)
			{
				int fillWidth=-thisX;
				if(wid<fillWidth)
				{
					fillWidth=wid;
				}
				for(int i=0; i<fillWidth*NumComponentPerPixel; ++i)
				{
					destPtr[i]=clearColor;
				}
				thisX+=fillWidth;
				destX+=fillWidth;
			}

			if(thisX<this->GetWidth() && destX<wid)
			{
				auto thisWidRemain=GetWidth()-thisX;
				auto destWidRemain=wid-destX;
				auto copyWid=(thisWidRemain<destWidRemain ? thisWidRemain : destWidRemain);
				for(int i=0; i<copyWid*NumComponentPerPixel; ++i)
				{
					destPtr[destX*NumComponentPerPixel+i]=thisPtr[thisX*NumComponentPerPixel+i];
				}
				destX+=copyWid;
				thisX+=copyWid;
			}

			if(destX<wid)
			{
				int fillWidth=wid-destX;
				for(int i=0; i<fillWidth*NumComponentPerPixel; ++i)
				{
					destPtr[destX*NumComponentPerPixel+i]=clearColor;
				}
				thisX+=fillWidth;
				destX+=fillWidth;
			}

			++destY;
			++thisY;
		}

		if(destY<hei)
		{
			int fillHeight=hei-destY;
			auto fillPtr=destination.LineTop(destY);
			for(int i=0; i<fillHeight*destination.GetNumComponentPerLine(); ++i)
			{
				fillPtr[i]=clearColor;
			}
			destY+=fillHeight;
			thisY+=fillHeight;
		}
		return YSOK;
	}

	/*! Returns the total number of components in the bitmap of given width and height.
	    If, the number of components per pixel is 4, (100x100) bitmap has 4x100x100 total number of components.
	*/
	int GetTotalNumComponent(int wid,int hei) const
	{
		return wid*hei*NumComponentPerPixel;
	}
	/*! Returns the total number of components in this bitmap.
	    If, the number of components per pixel is 4, (100x100) bitmap has 4x100x100 total number of components.
	*/
	int GetTotalNumComponent(void) const
	{
		return GetTotalNumComponent(nx,ny);
	}
	/*! Returns the total number of components in one single line for the given width.
	    If the width of the bitmap is 100, and the total number of components per pixel is 4, 4x100 components per line.
	*/
	int GetNumComponentPerLine(int wid) const
	{
		return wid*NumComponentPerPixel;
	}
	/*! Returns the total number of components in one single line for this bitmap.
	    If the width of the bitmap is 100, and the total number of components per pixel is 4, 4x100 components per line.
	*/
	int GetNumComponentPerLine(void) const
	{
		return GetNumComponentPerLine(nx);
	}
	/*! Make a copy of the incoming bitmap.
	*/
	YsBitmapTemplate <ComponentType,NumComponentPerPixel> &CopyFrom(const YsBitmapTemplate <ComponentType,NumComponentPerPixel> &incoming)
	{
		if(&incoming!=this)
		{
			this->CleanUp();
			Create(incoming.nx,incoming.ny);
			auto totalNumComponent=GetTotalNumComponent();
			for(decltype(totalNumComponent) ptr=0; ptr<totalNumComponent; ++ptr)
			{
				// Hope that the optimzer recognizes it's a bulk memory copy.
				this->bmpPtr[ptr]=incoming.bmpPtr[ptr];
			}
		}
		return *this;
	}
	/*! Moves the incoming bitmap to this bitmap.
	*/
	YsBitmapTemplate <ComponentType,NumComponentPerPixel> &MoveFrom(YsBitmapTemplate <ComponentType,NumComponentPerPixel> &incoming)
	{
		if(&incoming!=this)
		{
			this->nx=incoming.nx;
			this->ny=incoming.ny;
			this->bmpPtr=incoming.bmpPtr;
			this->lineTop=incoming.lineTop;
			incoming.nx=0;
			incoming.ny=0;
			incoming.bmpPtr=nullptr;
			incoming.lineTop=nullptr;
		}
		return *this;
	}

	/*! Creates a bitmap.
	*/
	YSRESULT Create(int wid,int hei)
	{
		if(this->nx==wid && this->ny==hei)
		{
			// Doesn't have to do anything.
			return YSOK;
		}

		CleanUp();
		if(0<wid && 0<hei)
		{
			nx=wid;
			ny=hei;
			bmpPtr=new ComponentType [GetTotalNumComponent(wid,hei)];
			lineTop=new ComponentType *[hei];
			for(int y=0; y<hei; ++y)
			{
				lineTop[y]=&bmpPtr[y*GetNumComponentPerLine(wid)];
			}
			return YSOK;
		}
		// Should I make it an error?
		return YSOK;
	}
	/*! Returns the width in the number of pixels.
	*/
	int GetWidth(void) const
	{
		return nx;
	}
	/*! Returns the height in the number of pixels.
	*/
	int GetHeight(void) const
	{
		return ny;
	}
	/*! Returns the pointer to the bitmap.
	*/
	const ComponentType *GetBitmapPointer(void) const
	{
		return bmpPtr;
	}
	/*! Returns the pointer to the pixel.
	*/
	const ComponentType *GetPixelPointer(int x,int y) const
	{
		return lineTop[y]+x*NumComponentPerPixel;
	}
	/*! Returns the mutable pointer to the bitmap.
	*/
	ComponentType *GetEditableBitmapPointer(void)
	{
		return bmpPtr;
	}
	/*! Returns the mutable pointer to the pixel.
	*/
	ComponentType *GetEditablePixelPointer(int x,int y)
	{
		return lineTop[y]+x*NumComponentPerPixel;
	}

	/*! Flips the image upside down.
	*/
	void Invert(void)
	{
		int x,y;
		ComponentType a;
		for(y=0; y<(int)GetHeight()/2; y++)
		{
			for(x=0; x<(int)GetWidth()*NumComponentPerPixel; x++)
			{
				a=lineTop[y][x];
				lineTop[y][x]=lineTop[GetHeight()-1-y][x];
				lineTop[GetHeight()-1-y][x]=a;
			}
		}
	}

	/*! Sets an array and width and height dirctory.
	    After this function, this class takes an ownership of incomingBmpPtr.
	    It must not be deleted outside of this class.
	*/
	void SetDirect(int wid,int hei,ComponentType *incomingBmpPtr)
	{
		if(incomingBmpPtr!=this->bmpPtr)
		{
			CleanUp();
			this->nx=wid;
			this->ny=hei;
			bmpPtr=incomingBmpPtr;
			lineTop=new ComponentType *[hei];
			for(int y=0; y<hei; ++y)
			{
				lineTop[y]=&bmpPtr[y*GetNumComponentPerLine(wid)];
			}
		}
	}

	/*! Returns YSTRUE if (x,y) is inside the bitmap.
	*/
	YSBOOL IsInRange(int x,int y) const
	{
		if(0<=x && x<nx && 0<=y && y<ny)
		{
			return YSTRUE;
		}
		return YSFALSE;
	}
	YSBOOL IsXInRange(int x) const
	{
		if(0<=x && x<nx)
		{
			return YSTRUE;
		}
		return YSFALSE;
	}
	YSBOOL IsYInRange(int y) const
	{
		if(0<=y && y<ny)
		{
			return YSTRUE;
		}
		return YSFALSE;
	}
};

/* } */
#endif
