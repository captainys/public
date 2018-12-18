/* ////////////////////////////////////////////////////////////

File Name: ysglbuffer.h
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

#ifndef YSGLDRAWBUFFER_IS_INCLUDED
#define YSGLDRAWBUFFER_IS_INCLUDED
/* { */

#include <ysclass.h>

typedef float YSGLFLOAT;

class YsGLBuffer
{
public:
};


////////////////////////////////////////////////////////////

// 2015/08/29
//   Function name MoveFrom was confusing.  I replaced MoveFrom with AddAndCleanUpIncoming.

template <typename T,const int D>
class YsGLBufferTemplate : public YsGLBuffer
{
protected:
	YsArray <T> dat;
public:
	inline YSSIZE_T GetN(void) const
	{
		return dat.GetN()/D;
	}
	inline int GetNi(void) const
	{
		return (int)(dat.GetN()/D);
	}
	inline YSSIZE_T GetNPerElem(void) const
	{
		return D;
	}
	inline YSSIZE_T GetNByte(void) const
	{
		return sizeof(T)*dat.GetN();
	}
	inline YSSIZE_T GetNBytePerElem(void) const
	{
		return sizeof(T)*D;
	}
	inline const T *GetArrayPointer(void) const
	{
		return dat.GetArray();
	}
	inline const T *GetElemPointer(YSSIZE_T elemIdx) const
	{
		return dat.GetArray()+elemIdx*D;
	}
	inline const T *operator[](YSSIZE_T elemIdx) const
	{
		return GetElemPointer(elemIdx);
	}
	inline operator const T *() const
	{
		return dat;
	}
	inline void CleanUp(void)
	{
		dat.CleanUp();
	}
	inline void AddAndCleanUpIncoming(YsGLBufferTemplate <T,D> &incoming)
	{
		this->dat.Add(incoming.dat);
		incoming.dat.CleanUp();
	}
	inline void MoveFrom(YsGLBufferTemplate <T,D> &incoming)
	{
		this->dat.MoveFrom(incoming.dat);
	}

	inline YSSIZE_T size(void) const
	{
		return dat.GetN()/D;
	}
	inline const T *data(void) const
	{
		return dat.data();
	}
};


////////////////////////////////////////////////////////////

template <typename T>
class YsGLBufferTemplate1D : public YsGLBufferTemplate <T,1>
{
public:
	using YsGLBufferTemplate <T,1>::GetN;
	using YsGLBufferTemplate <T,1>::GetNi;
	using YsGLBufferTemplate <T,1>::GetNPerElem;
	using YsGLBufferTemplate <T,1>::CleanUp;
	using YsGLBufferTemplate <T,1>::AddAndCleanUpIncoming;
	inline void Add(T x)
	{
		this->dat.Add(x);
	}
};

template <typename T>
class YsGLBufferTemplate2D : public YsGLBufferTemplate <T,2>
{
public:
	using YsGLBufferTemplate <T,2>::GetN;
	using YsGLBufferTemplate <T,2>::GetNi;
	using YsGLBufferTemplate <T,2>::GetNPerElem;
	using YsGLBufferTemplate <T,2>::CleanUp;
	using YsGLBufferTemplate <T,2>::AddAndCleanUpIncoming;
	inline void Add(T x,T y)
	{
		this->dat.Add(x);
		this->dat.Add(y);
	}
};

template <typename T>
class YsGLBufferTemplate3D : public YsGLBufferTemplate <T,3>
{
public:
	using YsGLBufferTemplate <T,3>::GetN;
	using YsGLBufferTemplate <T,3>::GetNi;
	using YsGLBufferTemplate <T,3>::GetNPerElem;
	using YsGLBufferTemplate <T,3>::CleanUp;
	using YsGLBufferTemplate <T,3>::AddAndCleanUpIncoming;
	inline void Add(T x,T y,T z)
	{
		this->dat.Add(x);
		this->dat.Add(y);
		this->dat.Add(z);
	}
};

template <typename T>
class YsGLBufferTemplate4D : public YsGLBufferTemplate <T,4>
{
public:
	using YsGLBufferTemplate <T,4>::GetN;
	using YsGLBufferTemplate <T,4>::GetNi;
	using YsGLBufferTemplate <T,4>::GetNPerElem;
	using YsGLBufferTemplate <T,4>::CleanUp;
	using YsGLBufferTemplate <T,4>::AddAndCleanUpIncoming;
	inline void Add(T x,T y,T z,T w)
	{
		this->dat.Add(x);
		this->dat.Add(y);
		this->dat.Add(z);
		this->dat.Add(w);
	}
};


////////////////////////////////////////////////////////////


class YsGLVertexBuffer : public YsGLBufferTemplate3D <YSGLFLOAT>
{
public:
	typedef YsGLBufferTemplate3D <YSGLFLOAT> BASECLASS;

	using BASECLASS::GetN;
	using BASECLASS::GetNi;
	using BASECLASS::GetNPerElem;
	using BASECLASS::CleanUp;
	using BASECLASS::AddAndCleanUpIncoming;


	inline void Add(const YsVec3 &pos);

	template <class T>
	inline void Add(T x,T y,T z);

	/*! Old naming convention.  Use Add instead. */
	inline void AddVertex(const YsVec3 &pos);
	/*! Old naming convention.  Use Add instead. */
	template <class T>
	inline void AddVertex(T x,T y,T z);
	/*! Old naming convention.  Use GetN instead. */
	inline YSSIZE_T GetNumVertex(void) const;

	YSRESULT AddArrowHeadTriangle(const YsVec3 &p0,const YsVec3 &p1,const double arrowSize);
	YSRESULT AddArrowHeadTriangle(const YsVec3 p[2],const double arrowSize);
};

inline void YsGLVertexBuffer::Add(const YsVec3 &pos)
{
	BASECLASS::Add(pos.xf(),pos.yf(),pos.zf());
}

inline void YsGLVertexBuffer::AddVertex(const YsVec3 &pos)
{
	BASECLASS::Add(pos.xf(),pos.yf(),pos.zf());
}

template <class T>
inline void YsGLVertexBuffer::Add(T x,T y,T z)
{
	BASECLASS::Add((YSGLFLOAT)x,(YSGLFLOAT)y,(YSGLFLOAT)z);
}

template <class T>
inline void YsGLVertexBuffer::AddVertex(T x,T y,T z)
{
	BASECLASS::Add((YSGLFLOAT)x,(YSGLFLOAT)y,(YSGLFLOAT)z);
}

inline YSSIZE_T YsGLVertexBuffer::GetNumVertex(void) const
{
	return GetN();
}

////////////////////////////////////////////////////////////

class YsGLVertexBuffer2D : public YsGLBufferTemplate2D <YSGLFLOAT>
{
public:
	typedef YsGLBufferTemplate2D <YSGLFLOAT> BASECLASS;

	using BASECLASS::GetN;
	using BASECLASS::GetNi;
	using BASECLASS::GetNPerElem;
	using BASECLASS::CleanUp;
	using BASECLASS::AddAndCleanUpIncoming;

	inline void Add(const YsVec2 &pos);

	template <class T>
	inline void Add(T x,T y);
};

inline void YsGLVertexBuffer2D::Add(const YsVec2 &pos)
{
	BASECLASS::Add(pos.xf(),pos.yf());
}

template <class T>
inline void YsGLVertexBuffer2D::Add(T x,T y)
{
	BASECLASS::Add((YSGLFLOAT)x,(YSGLFLOAT)y);
}

////////////////////////////////////////////////////////////


class YsGLPixOffsetBuffer : public YsGLBufferTemplate2D <YSGLFLOAT>
{
public:
	typedef YsGLBufferTemplate2D <YSGLFLOAT> BASECLASS;

	using BASECLASS::GetN;
	using BASECLASS::GetNi;
	using BASECLASS::GetNPerElem;
	using BASECLASS::CleanUp;
	using BASECLASS::AddAndCleanUpIncoming;

	inline void Add(const YsVec2 &pos);

	template <class T>
	inline void Add(T x,T y);

	/*! Old naming convention.  Use Add instead. */
	template <class T>
	inline void AddPixOffset(T x,T y);
};

inline void YsGLPixOffsetBuffer::Add(const YsVec2 &pos)
{
	BASECLASS::Add(pos.xf(),pos.yf());	
}

template <class T>
inline void YsGLPixOffsetBuffer::Add(T x,T y)
{
	BASECLASS::Add((YSGLFLOAT)x,(YSGLFLOAT)y);
}

template <class T>
inline void YsGLPixOffsetBuffer::AddPixOffset(T x,T y)
{
	BASECLASS::Add((YSGLFLOAT)x,(YSGLFLOAT)y);
}


////////////////////////////////////////////////////////////


class YsGLColorBuffer : public YsGLBufferTemplate4D <YSGLFLOAT>
{
public:
	typedef YsGLBufferTemplate4D <YSGLFLOAT> BASECLASS;

	using BASECLASS::GetN;
	using BASECLASS::GetNi;
	using BASECLASS::GetNPerElem;
	using BASECLASS::CleanUp;
	using BASECLASS::AddAndCleanUpIncoming;

	inline void Add(YsColor col);
	template <class T>
	inline void Add(YsColor col,T a);
	template <class T>
	inline void Add(T r,T g,T b,T a);
	template <class T>
	inline void Add(T r,T g,T b);

	/*! Old naming convention.  Use Add instead. */
	inline void AddColor(YsColor col);

	/*! Old naming convention.  Use Add instead. */
	template <class T>
	inline void AddColor(YsColor col,T a);

	/*! Old naming convention.  Use Add instead. */
	template <class T>
	inline void AddColor(T r,T g,T b,T a);

	/*! Old naming convention.  Use Add instead. */
	template <class T>
	inline void AddColor(T r,T g,T b);
};

inline void YsGLColorBuffer::Add(YsColor col)
{
	BASECLASS::Add(col.Rf(),col.Gf(),col.Bf(),col.Af());
}

template <class T>
inline void YsGLColorBuffer::Add(YsColor col,T a)
{
	BASECLASS::Add(col.Rf(),col.Gf(),col.Bf(),a);
}

template <class T>
inline void YsGLColorBuffer::Add(T r,T g,T b,T a)
{
	BASECLASS::Add((YSGLFLOAT)r,(YSGLFLOAT)g,(YSGLFLOAT)b,(YSGLFLOAT)a);
}

template <class T>
inline void YsGLColorBuffer::Add(T r,T g,T b)
{
	BASECLASS::Add((YSGLFLOAT)r,(YSGLFLOAT)g,(YSGLFLOAT)b,1.0f);
}

inline void YsGLColorBuffer::AddColor(YsColor col)
{
	BASECLASS::Add(col.Rf(),col.Gf(),col.Bf(),col.Af());
}

template <class T>
inline void YsGLColorBuffer::AddColor(YsColor col,T a)
{
	BASECLASS::Add(col.Rf(),col.Gf(),col.Bf(),a);
}

template <class T>
inline void YsGLColorBuffer::AddColor(T r,T g,T b,T a)
{
	BASECLASS::Add((YSGLFLOAT)r,(YSGLFLOAT)g,(YSGLFLOAT)b,(YSGLFLOAT)a);
}

template <class T>
inline void YsGLColorBuffer::AddColor(T r,T g,T b)
{
	BASECLASS::Add((YSGLFLOAT)r,(YSGLFLOAT)g,(YSGLFLOAT)b,1.0f);
}


////////////////////////////////////////////////////////////


class YsGLNormalBuffer : public YsGLBufferTemplate3D <YSGLFLOAT>
{
public:
	typedef YsGLBufferTemplate3D <YSGLFLOAT> BASECLASS;

	using BASECLASS::GetN;
	using BASECLASS::GetNi;
	using BASECLASS::GetNPerElem;
	using BASECLASS::CleanUp;
	using BASECLASS::AddAndCleanUpIncoming;

	inline void Add(const YsVec3 &pos);

	template <class T>
	inline void Add(T x,T y,T z);

	/*! Old naming convention.  Use Add instead. */
	inline void AddNormal(const YsVec3 &nom);

	/*! Old naming convention.  Use Add instead. */
	template <class T>
	inline void AddNormal(T x,T y,T z);
};

inline void YsGLNormalBuffer::Add(const YsVec3 &pos)
{
	BASECLASS::Add(pos.xf(),pos.yf(),pos.zf());
}

template <class T>
inline void YsGLNormalBuffer::Add(T x,T y,T z)
{
	BASECLASS::Add((YSGLFLOAT)x,(YSGLFLOAT)y,(YSGLFLOAT)z);
}

inline void YsGLNormalBuffer::AddNormal(const YsVec3 &nom)
{
	BASECLASS::Add(nom.xf(),nom.yf(),nom.zf());
}

template <class T>
inline void YsGLNormalBuffer::AddNormal(T x,T y,T z)
{
	BASECLASS::Add((YSGLFLOAT)x,(YSGLFLOAT)y,(YSGLFLOAT)z);
}


////////////////////////////////////////////////////////////

class YsGLPointSizeBuffer : public YsGLBufferTemplate1D <YSGLFLOAT>
{
public:
	typedef YsGLBufferTemplate1D <YSGLFLOAT> BASECLASS;

	using BASECLASS::Add;
	using BASECLASS::GetN;
	using BASECLASS::GetNi;
	using BASECLASS::GetNPerElem;
	using BASECLASS::CleanUp;
	using BASECLASS::AddAndCleanUpIncoming;
};

////////////////////////////////////////////////////////////

template <class ComponentType>
class YsGLPrimitiveSequence : public YsGLBuffer
{
public:
	/*! primTypeAndNumVtx[i*2  ] is YsGL::PTIMITIVE_TYPE.
	    primTypeAndNumVtx[i*2+1] is the number of vertices for the ith primitive.
	*/
	YsArray <int> primTypeAndNumVtx;

	/*! XYZ Coordinates. */
	YsArray <ComponentType> coord;

	/*! Color */
	YsArray <float> color;

	void CleanUp(void);
};

typedef YsGLPrimitiveSequence <float> YsGLPrimitiveSequencef;

////////////////////////////////////////////////////////////

/* } */
#endif
