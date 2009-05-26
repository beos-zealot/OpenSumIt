/*
	Copyright 1996, 1997, 1998, 2000
	        Hekkelman Programmatuur B.V.  All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this software
	   must display the following acknowledgement:
	   
	    This product includes software developed by Hekkelman Programmatuur B.V.
	
	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
	   promote products derived from this software without specific prior
	   written permission.
	
	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/
/*
	MyMath.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

	uitbreidingen op de POSIX math library
	schaamteloze kopie van de SUN sources

*/

#include "MyMath.h"

double Nan(int nannr)
{
	double x;
	__HI(x) = 0x7FF00000 | ((nannr & 0x000000FF) << 5);
	__LO(x) = 0;
	return x;
}

//int isnan (double x)
//{
//	int hx,lx;
//	hx = (__HI(x)&0x7fffffff);
//	lx = __LO(x);
//	hx |= (unsigned)(lx|(-lx))>>31;	
//	hx = 0x7ff00000 - hx;
//	return ((unsigned)(hx))>>31;
//}

//int finite (double x)
//{
//	int hx; 
//	hx = __HI(x);
//	return  (unsigned)((hx&0x7fffffff)-0x7ff00000)>>31;
//}


//double copysign (double x, double y)
//{
//	__HI(x) = (__HI(x)&0x7fffffff)|(__HI(y)&0x80000000);
//        return x;
//}

static const double TWO52[2]={
  4.50359962737049600000e+15, /* 0x43300000, 0x00000000 */
 -4.50359962737049600000e+15, /* 0xC3300000, 0x00000000 */
};

//double rint (double x)
//{
//	int i0,j0,sx;
//	unsigned i,i1;
//	double w,t;
//	i0 =  __HI(x);
//	sx = (i0>>31)&1;
//	i1 =  __LO(x);
//	j0 = ((i0>>20)&0x7ff)-0x3ff;
//	if(j0<20)
//	{
//	    if(j0<0)
//{ 	
//		if(((i0&0x7fffffff)|i1)==0) return x;
//		i1 |= (i0&0x0fffff);
//		i0 &= 0xfffe0000;
//		i0 |= ((i1|-i1)>>12)&0x80000;
//		__HI(x)=i0;
//	        w = TWO52[sx]+x;
//	        t =  w-TWO52[sx];
//	        i0 = __HI(t);
//	        __HI(t) = (i0&0x7fffffff)|(sx<<31);
//	        return t;
//	    } else
//	{
//		i = (0x000fffff)>>j0;
//		if(((i0&i)|i1)==0) return x; /* x is integral */
//		i>>=1;
//		if(((i0&i)|i1)!=0)
//		{
//		    if(j0==19) i1 = 0x40000000; else
//		    i0 = (i0&(~i))|((0x20000)>>j0);
//		}
//	    }
//	} else if (j0>51)
//	{
//	    if(j0==0x400) return x+x;	/* inf or NaN */
//	    else return x;		/* x is integral */
//	} else
//	{
//	    i = ((unsigned)(0xffffffff))>>(j0-20);
//	    if((i1&i)==0) return x;	/* x is integral */
//	    i>>=1;
//	    if((i1&i)!=0) i1 = (i1&(~i))|((0x40000000)>>(j0-20));
//	}
//	__HI(x) = i0;
//	__LO(x) = i1;
//	w = TWO52[sx]+x;
//	return w-TWO52[sx];
//}
