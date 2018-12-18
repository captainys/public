/* ////////////////////////////////////////////////////////////

File Name: ysequation.h
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

#ifndef YSEQUATION_IS_INCLUDED
#define YSEQUATION_IS_INCLUDED
/* { */

#include "ysdef.h"

/* ax+by+c=0 */
/* px+qy+r=0 */
YSRESULT YsSolveTwoLinearEquation
    (double *x,double *y,
     const double &a,const double &b,const double &c,
     const double &p,const double &q,const double &r);

/* a1 x+b1 y+c1 z+d1=0 */
/* a2 x+b2 y+c2 z+d2=0 */
/* a3 x+b3 y+c3 z+d3=0 */
YSRESULT YsSolveThreeLinearEquation
    (double *x,double *y,double *z,
     const double &a1,const double &b1,const double &c1,const double &d1,
     const double &a2,const double &b2,const double &c2,const double &d2,
     const double &a3,const double &b3,const double &c3,const double &d3);
/* } */
#endif
