/* ////////////////////////////////////////////////////////////

File Name: ysrect.h
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

#ifndef YSRECT_IS_INCLUDED
#define YSRECT_IS_INCLUDED
/* { */

#include "ysgeometry.h"
#include "ysveci.h"
#include "ysmath.h"



/*! Template for the rectangle class.
*/
template <class VecType>
class YsRectTemplate
{
private:
	VecType minmax[2];
public:
	/*! Default constructor.
	*/
	YsRectTemplate()
	{
	}
	/*! Makes an empty rectangle.
	*/
	YsRectTemplate(std::nullptr_t)
	{
		minmax[0]=VecType::Origin();
		minmax[1]=VecType::Origin();
	}
	/*! Constructor that constructs a rectangle from two corners.
	*/
	YsRectTemplate(const VecType &min,const VecType &max)
	{
		Set(min,max);
	}

	/*! Returns the minimum.
	*/
	const VecType &Min(void) const
	{
		return minmax[0];
	}
	/*! Returns the maximum.
	*/
	const VecType &Max(void) const
	{
		return minmax[1];
	}
	/*! Returns the center.
	*/
	VecType GetCenter(void) const
	{
		return (minmax[0]+minmax[1])/(typename VecType::COMPONENT_TYPE)2;
	}

	/*! Returns min and max as an array.
	*/
	const VecType *MinMax(void) const
	{
		return minmax;
	}

	/*! Returns the size of the rectangle.  The value returned is same as Max()-Min().
	    When VecType is an integral vector, the rectangle is defined as from Min() to Max() excluding Max().
	    For example, to define a rectnagular area of an 800x600 window, min should be the origin, and the
	    max should be (800,600).  Not (799-599).
	*/
	VecType GetSize(void) const
	{
		return minmax[1]-minmax[0];
	}

	/*! Alias for GetSize.  Some old code is using GetDimension of YsBoundingBoxMaker2 or YsBoundingBoxMaker3.
	    To replace the legacy YsBoundingBoxMaker?, this alias might help.
	*/
	VecType GetDimension(void) const
	{
		return GetSize();
	}

	/*! Returns the length of minimum to maximum. */
	const double GetDiagonalLength(void) const
	{
		return GetSize().GetLength();
	}

	/*! Sets two corners.  Component of min can be greater than the corresponding component of max, in which
	    case the component from max and min will be stored as min and max respectively.
	*/
	void Set(const VecType &min,const VecType &max)
	{
		for(int i=0; i<VecType::NDIMENSION; ++i)
		{
			minmax[0][i]=YsSmaller(min[i],max[i]);
			minmax[1][i]=YsGreater(min[i],max[i]);
		}
	}

	/*! Allows access to the min and max as an array.
	*/
	const VecType &operator[](YSSIZE_T idx) const
	{
		return minmax[idx];
	}

	/*! Returns YSTRUE if the point p is inside of the rect.  YSFALSE otherwise.
	    Boundary along each component min is considered inside.
	    Boundary along Each component max is not considered inside.
	*/
	YSBOOL IsInside(const VecType &p) const
	{
		for(int i=0; i<VecType::NDIMENSION; ++i)
		{
			if(p[i]<minmax[0][i] || minmax[1][i]<=p[i])
			{
				return YSFALSE;
			}
		}
		return YSTRUE;
	}

	/*! Returns YSTRUE if incoming overlaps with this rect.
	*/
	YSBOOL DoesOverlap(const YsRectTemplate<VecType> &incoming) const
	{
		for(int i=0; i<VecType::NDIMENSION; ++i)
		{
			if(Max()[i]<incoming.Min()[i] || incoming.Max()[i]<Min()[i])
			{
				return YSFALSE;
			}
		}
		return YSTRUE;
	}

	/*! Calculates overlapping rect and returns in overlapRect */
	void GetOverlap(YsRectTemplate <VecType> &overlapRect,const YsRectTemplate <VecType> &withThisRect) const
	{
		auto thisMin=Min();
		auto thisMax=Max();
		auto incomingMin=withThisRect.Min();
		auto incomingMax=withThisRect.Max();
		VecType newMin,newMax;

		for(int i=0; i<VecType::NDIMENSION; ++i)
		{
			newMin[i]=YsGreater(thisMin[i],incomingMin[i]);
			newMax[i]=YsSmaller(thisMax[i],incomingMax[i]);
			if(newMax[i]<newMin[i]) // Means no overlap
			{
				if(thisMax[i]<incomingMin[i])
				{
					newMin[i]=thisMax[i];
					newMax[i]=thisMax[i];
				}
				else
				{
					newMin[i]=thisMin[i];
					newMax[i]=thisMin[i];
				}
			}
		}
		overlapRect.Set(newMin,newMax);
	}
};


class YsRect2i : public YsRectTemplate <YsVec2i>
{
public:
	YsRect2i()
	{
	}
	YsRect2i(std::nullptr_t)
	{
		Set(YsVec2i::Origin(),YsVec2i::Origin());
	}
	YsRect2i(const YsVec2i &min,const YsVec2i &max)
	{
		Set(min,max);
	}

	int GetWidth(void) const
	{
		return Max().x()-Min().x();
	}
	int GetHeight(void) const
	{
		return Max().y()-Min().y();
	}

};

typedef YsRectTemplate <YsVec3i> YsRect3i;
typedef YsRectTemplate <YsVec2> YsRect2;
typedef YsRectTemplate <YsVec3> YsRect3;


/* } */
#endif
