#ifndef YSPOSITIVEAREA_IS_INCLUDED
#define YSPOSITIVEAREA_IS_INCLUDED
/* { */

#include "ysgeometry.h"



class YsAreaSlasher
{
protected:
	YsArray <YsVec2,4> plg;
public:
	YsAreaSlasher();
	~YsAreaSlasher();

protected:
	void SetInitialPolygon(YSSIZE_T np,const YsVec2 p[]);

	void SetInitialPolygon(const YsConstArrayMask <YsVec2> &p);

	void InsertVertexOnEdge(const YsVec2 o,const YsVec2 v);

	// This preserves edges on the n side of the cutting.
	void DeleteNegativeEdge(const YsVec2 o,const YsVec2 n);

	void CloseHole(void);

public:
	/*! Saves the slashed octahedron/cube for debugging purpose. */
	YSRESULT SaveSrf(const char fn[]) const;
};



class YsPositiveAreaCalculator : public YsAreaSlasher
{
public:
	/*! Initialize a cube.
	*/
	void Initialize(const YsVec2 &min,const YsVec2 &max);

	/*! Cuts the polygon by the line that passes through o and perpendicular to n,
	    and then drops the part that is negative side of the (o,n).

	    When finding a convex area from which every part of the polygon is visible (is there a term for this?),
	    the normal must point inward to the polygon.
	*/
	void AddLine(const YsVec2 o,const YsVec2 n);

	/*! Returns the shell of the remaining volume.
	*/
	const YsConstArrayMask <YsVec2> GetPolygon(void) const;
};


/* } */
#endif
