/* ////////////////////////////////////////////////////////////

File Name: ysveci.h
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

#ifndef YSVECI_IS_INCLUDED
#define YSVECI_IS_INCLUDED
/* { */

#include "ysmath.h"

class YsVec2i : protected YsFixedLengthArray <int,2>
{
public:
	enum
	{
		NDIMENSION=2
	};
	typedef int COMPONENT_TYPE;

	inline YsVec2i()
	{
	}
	inline YsVec2i(int x,int y)
	{
		dat[0]=x;
		dat[1]=y;
	}
	YsVec2i(const char *x,const char *y);

	inline const int &operator[](int idx) const
	{
		return dat[idx];
	}
	inline int &operator[](int idx)
	{
		return dat[idx];
	}
	operator const int *() const
	{
		return dat;
	}
	const char *Text(char buf[],const char *fmt="%d %d") const;

	YsVec2i operator/(int n) const
	{
		auto a=*this;
		a.dat[0]/=n;
		a.dat[1]/=n;
		return a;
	}

	inline void AddX(const int &x)
	{
		dat[0]+=x;
	}
	inline void AddY(const int &y)
	{
		dat[1]+=y;
	}

	inline void SubX(const int &x)
	{
		dat[0]-=x;
	}
	inline void SubY(const int &y)
	{
		dat[1]-=y;
	}

	inline void MulX(const int &x)
	{
		dat[0]*=x;
	}
	inline void MulY(const int &y)
	{
		dat[1]*=y;
	}

	inline void DivX(const int &x)
	{
		dat[0]/=x;
	}
	inline void DivY(const int &y)
	{
		dat[1]/=y;
	}

	inline int x(void) const
	{
		return dat[0];
	}
	inline int y(void) const
	{
		return dat[1];
	}

	inline int xi(void) const
	{
		return dat[0];
	}
	inline int yi(void) const
	{
		return dat[1];
	}

	inline float xf(void) const
	{
		return (float)dat[0];
	}
	inline float yf(void) const
	{
		return (float)dat[1];
	}

	inline double xd(void) const
	{
		return (double)dat[0];
	}
	inline double yd(void) const
	{
		return (double)dat[1];
	}

	inline void Set(int x,int y)
	{
		dat[0]=x;
		dat[1]=y;
	}

	inline void SetX(const int &x)
	{
		dat[0]=x;
	}
	inline void SetY(const int &y)
	{
		dat[1]=y;
	}

	void RotateLeft90(void)
	{
		int x,y;
		x=dat[0];
		y=dat[1];
		dat[0]=-y;
		dat[1]=x;
	}
	void RotateRight90(void)
	{
		int x,y;
		x=dat[0];
		y=dat[1];
		dat[0]=y;
		dat[1]=-x;
	}

	YsVec2i operator+=(const YsVec2i incoming)
	{
		dat[0]+=incoming.dat[0];
		dat[1]+=incoming.dat[1];
		return *this;
	}
	YsVec2i operator-=(const YsVec2i incoming)
	{
		dat[0]-=incoming.dat[0];
		dat[1]-=incoming.dat[1];
		return *this;
	}

	/*! Returns an index to the axis of the longest-dimension */
	inline int GetLongestAxis(void) const
	{
		if(YsAbs(dat[0])>YsAbs(dat[1]))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}

	inline static YsVec2i Origin(void)
	{
		return YsVec2i(0,0);
	};

	double GetLength(void) const;
};

inline const YsVec2i operator-(const YsVec2i &a)
{
	return YsVec2i(-a.x(),-a.y());
}

inline const YsVec2i operator-(const YsVec2i &a,const YsVec2i &b)
{
	return YsVec2i(a.x()-b.x(),a.y()-b.y());
}

inline const YsVec2i operator+(const YsVec2i &a,const YsVec2i &b)
{
	return YsVec2i(a.x()+b.x(),a.y()+b.y());
}

inline int operator^(const YsVec2i &a,const YsVec2i &b)
{
	return a.x()*b.y()-a.y()*b.x();
}

inline bool operator==(const YsVec2i &a,const YsVec2i &b)
{
	return (a.x()==b.x() && a.y()==b.y());
}

inline bool operator!=(const YsVec2i &a,const YsVec2i &b)
{
	return (a.x()!=b.x() || a.y()!=b.y());
}


////////////////////////////////////////////////////////////

class YsVec2iRange
{
private:
	YsVec2i min,max;

public:
	inline YsVec2iRange(const YsVec2i &min,const YsVec2i &max)
	{
		this->min.Set(YsSmaller(min.x(),max.x()),YsSmaller(min.y(),max.y()));
		this->max.Set(YsGreater(min.x(),max.x()),YsGreater(min.y(),max.y()));
	}

	class iterator
	{
	public:
		YsVec2i min,max;
		YsVec2i idx;
		int dir;
	private:
		inline void Move(int dir)
		{
			if(0<dir)
			{
				idx.AddX(1);
				if(max.x()<idx.x())
				{
					idx.SetX(min.x());
					idx.AddY(1);
				}
			}
			else
			{
				idx.SubX(1);
				if(idx.x()<0)
				{
					idx.SetX(max.x());
					idx.SubY(1);
				}
			}
		}
		inline void Forward(void)
		{
			Move(dir);
		}
		inline void Backward(void)
		{
			Move(-dir);
		}
	public:
		inline iterator &operator++()
		{
			Forward();
			return *this;
		}
		inline iterator operator++(int)
		{
			iterator copy=*this;
			Forward();
			return copy;
		}
		inline iterator &operator--()
		{
			Backward();
			return *this;
		}
		inline iterator operator--(int)
		{
			iterator copy=*this;
			Backward();
			return copy;
		}
		inline bool operator==(const iterator &from)
		{
			return (idx==from.idx);
		}
		inline bool operator!=(const iterator &from)
		{
			return (idx!=from.idx);
		}
		inline YsVec2i &operator*()
		{
			return idx;
		}
	};

	/*! Support for STL-like iterator */
	inline iterator begin() const
	{
		iterator iter;
		iter.min=min;
		iter.max=max;
		iter.idx=min;
		iter.dir=1;
		return iter;
	}

	/*! Support for STL-like iterator */
	inline iterator end() const
	{
		iterator iter;
		iter.min=min;
		iter.max=max;
		iter.idx.SetX(min.x());
		iter.idx.SetY(max.y()+1);
		iter.dir=1;
		return iter;
	}

	/*! Support for STL-like iterator */
	inline iterator rbegin() const
	{
		iterator iter;
		iter.min=min;
		iter.max=max;
		iter.idx=max;
		iter.dir=-1;
		return iter;
	}

	/*! Support for STL-like iterator */
	inline iterator rend() const
	{
		iterator iter;
		iter.min=min;
		iter.max=max;
		iter.idx.SetX(max.x());
		iter.idx.SetY(min.y()-1);
		iter.dir=-1;
		return iter;
	}
};

inline YsVec2iRange::iterator begin(const YsVec2iRange &allIndex)
{
	return allIndex.begin();
}

inline YsVec2iRange::iterator rbegin(const YsVec2iRange &allIndex)
{
	return allIndex.rbegin();
}

inline YsVec2iRange::iterator end(const YsVec2iRange &allIndex)
{
	return allIndex.end();
}

inline YsVec2iRange::iterator rend(const YsVec2iRange &allIndex)
{
	return allIndex.rend();
}


////////////////////////////////////////////////////////////


class YsVec3i : protected YsFixedLengthArray <int,3>
{
public:
	enum
	{
		NDIMENSION=3
	};
	typedef int COMPONENT_TYPE;

	inline YsVec3i()
	{
	}
	inline YsVec3i(int x,int y,int z)
	{
		dat[0]=x;
		dat[1]=y;
		dat[2]=z;
	}
	YsVec3i(const char *x,const char *y,const char *z);

	inline const int &operator[](int idx) const
	{
		return dat[idx];
	}
	inline int &operator[](int idx)
	{
		return dat[idx];
	}
	operator const int *() const
	{
		return dat;
	}
	const char *Text(char buf[],const char *fmt="%d %d %d") const;

	YsVec3i operator/(int n) const
	{
		auto a=*this;
		a.dat[0]/=n;
		a.dat[1]/=n;
		a.dat[2]/=n;
		return a;
	}

	inline void AddX(const int &x)
	{
		dat[0]+=x;
	}
	inline void AddY(const int &y)
	{
		dat[1]+=y;
	}
	inline void AddZ(const int &z)
	{
		dat[2]+=z;
	}

	inline void SubX(const int &x)
	{
		dat[0]-=x;
	}
	inline void SubY(const int &y)
	{
		dat[1]-=y;
	}
	inline void SubZ(const int &z)
	{
		dat[2]-=z;
	}

	inline void MulX(const int &x)
	{
		dat[0]*=x;
	}
	inline void MulY(const int &y)
	{
		dat[1]*=y;
	}
	inline void MulZ(const int &z)
	{
		dat[2]*=z;
	}

	inline void DivX(const int &x)
	{
		dat[0]/=x;
	}
	inline void DivY(const int &y)
	{
		dat[1]/=y;
	}
	inline void DivZ(const int &z)
	{
		dat[2]/=z;
	}

	inline int x(void) const
	{
		return dat[0];
	}
	inline int y(void) const
	{
		return dat[1];
	}
	inline int z(void) const
	{
		return dat[2];
	}

	inline void Set(int x,int y,int z)
	{
		dat[0]=x;
		dat[1]=y;
		dat[2]=z;
	}

	inline void SetX(const int &x)
	{
		dat[0]=x;
	}
	inline void SetY(const int &y)
	{
		dat[1]=y;
	}
	inline void SetZ(const int &z)
	{
		dat[2]=z;
	}

	/*! Returns an index to the axis of the longest-dimension */
	inline int GetLongestAxis(void) const
	{
		if(YsAbs(dat[0])>YsAbs(dat[1]) && YsAbs(dat[0])>YsAbs(dat[2]))
		{
			return 0;
		}
		else if(YsAbs(dat[1])>YsAbs(dat[2]))
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}

	inline static YsVec3i Origin(void)
	{
		return YsVec3i(0,0,0);
	};

	double GetLength(void) const;
};

inline const YsVec3i operator-(const YsVec3i &a)
{
	return YsVec3i(-a.x(),-a.y(),-a.z());
}

inline const YsVec3i operator-(const YsVec3i &a,const YsVec3i &b)
{
	return YsVec3i(a.x()-b.x(),a.y()-b.y(),a.z()-b.z());
}

inline const YsVec3i operator+(const YsVec3i &a,const YsVec3i &b)
{
	return YsVec3i(a.x()+b.x(),a.y()+b.y(),a.z()+b.z());
}

inline bool operator==(const YsVec3i &a,const YsVec3i &b)
{
	return (a.x()==b.x() && a.y()==b.y() && a.z()==b.z());
}

inline bool operator!=(const YsVec3i &a,const YsVec3i &b)
{
	return (a.x()!=b.x() || a.y()!=b.y() || a.z()!=b.z());
}

////////////////////////////////////////////////////////////

class YsVec3iRange
{
private:
	YsVec3i min,max;

public:
	inline YsVec3iRange(const YsVec3i &max)
	{
		this->min.Set(0,0,0);
		this->max.Set(YsGreater(min.x(),max.x()),YsGreater(min.y(),max.y()),YsGreater(min.z(),max.z()));
	}
	inline YsVec3iRange(const YsVec3i &min,const YsVec3i &max)
	{
		this->min.Set(YsSmaller(min.x(),max.x()),YsSmaller(min.y(),max.y()),YsSmaller(min.z(),max.z()));
		this->max.Set(YsGreater(min.x(),max.x()),YsGreater(min.y(),max.y()),YsGreater(min.z(),max.z()));
	}

	class iterator
	{
	public:
		YsVec3i min,max;
		YsVec3i idx;
		int dir;
	private:
		inline void Move(int dir)
		{
			if(0<dir)
			{
				idx.AddX(1);
				if(max.x()<idx.x())
				{
					idx.SetX(min.x());
					idx.AddY(1);
					if(max.y()<idx.y())
					{
						idx.SetY(min.y());
						idx.AddZ(1);
					}
				}
			}
			else
			{
				idx.SubX(1);
				if(idx.x()<0)
				{
					idx.SetX(max.x());
					idx.SubY(1);
					if(idx.y()<0)
					{
						idx.SetY(max.y());
						idx.SubZ(1);
					}
				}
			}
		}
		inline void Forward(void)
		{
			Move(dir);
		}
		inline void Backward(void)
		{
			Move(-dir);
		}
	public:
		inline iterator &operator++()
		{
			Forward();
			return *this;
		}
		inline iterator operator++(int)
		{
			iterator copy=*this;
			Forward();
			return copy;
		}
		inline iterator &operator--()
		{
			Backward();
			return *this;
		}
		inline iterator operator--(int)
		{
			iterator copy=*this;
			Backward();
			return copy;
		}
		inline bool operator==(const iterator &from)
		{
			return (idx==from.idx);
		}
		inline bool operator!=(const iterator &from)
		{
			return (idx!=from.idx);
		}
		inline YsVec3i &operator*()
		{
			return idx;
		}
	};

	/*! Support for STL-like iterator */
	inline iterator begin() const
	{
		iterator iter;
		iter.min=min;
		iter.max=max;
		iter.idx=min;
		iter.dir=1;
		return iter;
	}

	/*! Support for STL-like iterator */
	inline iterator end() const
	{
		iterator iter;
		iter.min=min;
		iter.max=max;
		iter.idx.SetX(min.x());
		iter.idx.SetY(min.y());
		iter.idx.SetZ(max.z()+1);
		iter.dir=1;
		return iter;
	}

	/*! Support for STL-like iterator */
	inline iterator rbegin() const
	{
		iterator iter;
		iter.min=min;
		iter.max=max;
		iter.idx=max;
		iter.dir=-1;
		return iter;
	}

	/*! Support for STL-like iterator */
	inline iterator rend() const
	{
		iterator iter;
		iter.min=min;
		iter.max=max;
		iter.idx.SetX(max.x());
		iter.idx.SetY(max.y());
		iter.idx.SetZ(min.z()-1);
		iter.dir=-1;
		return iter;
	}
};

inline YsVec3iRange::iterator begin(const YsVec3iRange &allIndex)
{
	return allIndex.begin();
}

inline YsVec3iRange::iterator rbegin(const YsVec3iRange &allIndex)
{
	return allIndex.rbegin();
}

inline YsVec3iRange::iterator end(const YsVec3iRange &allIndex)
{
	return allIndex.end();
}

inline YsVec3iRange::iterator rend(const YsVec3iRange &allIndex)
{
	return allIndex.rend();
}

////////////////////////////////////////////////////////////

/* } */
#endif
