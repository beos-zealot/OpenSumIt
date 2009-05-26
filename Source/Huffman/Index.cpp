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
	32-bit to indexed color conversion
*/

#include "Index.h"

#ifndef CALLOC
#define CALLOC calloc
#endif

#ifndef MALLOC
#define MALLOC malloc
#endif

#ifndef FREE
#define FREE free
#endif

#include <GraphicsDefs.h>
#include <support/Debug.h>

const uchar RUNCHAR = 0xFF;

static long LookupColor(rgb_color lookup, rgb_color *clut, long count)
{
	long i;
	
	for (i = 0; i < count; i++)
	{
		if (*(long *)&lookup == *(long *)&clut[i])
			return i;
	}
	
	return count;
} /* LookupColor */

void Bitmap2Index(const void *bitmap32, long bitmap32length,
	int width, int height, void **indices, long *indexLength)
{
	rgb_color *clut, lookup = { 0, 0, 0, 0};
	long count = 0, i, pixels;
	uchar *p;
	
	*indexLength = 0;
	
	clut = (rgb_color *)MALLOC(256 * sizeof(rgb_color));
	
	pixels = bitmap32length / 3;
	p = (uchar *)bitmap32;
	
	for (i = 0; i < pixels; i++)
	{
		lookup.red = *p++;
		lookup.green = *p++;
		lookup.blue = *p++;
		if (LookupColor(lookup, clut, count) == count)
		{
			clut[count] = lookup;
			count++;
		}
		if (count >= 254)
		{
			printf("Too many colors\n");
			exit(1);
		}
	}
	
	uchar *rp;
	long rl;
	rl = bitmap32length;
	rp = (uchar *)MALLOC(rl);
	if (!rp) return;
	*indices = rp;
	
	*rp++ = width >> 8;
	*rp++ = width & 0xFF;
	*rp++ = height >> 8;
	*rp++ = height & 0xFF;

	*rp++ = count;
	
	for (i = 0; i < count; i++)
	{
		*rp++ = clut[i].red;
		*rp++ = clut[i].green;
		*rp++ = clut[i].blue;
	}
	
	p = (uchar *)bitmap32;
	uchar last, cur, run, *sp;
	last = RUNCHAR;
	run = 0;
	sp = rp;
	
	for (i = 0; i < pixels; i++)
	{
		lookup.red = *p++;
		lookup.green = *p++;
		lookup.blue = *p++;
		
		cur = LookupColor(lookup, clut, count);
		if (cur == last)
		{
			if (run >= 254)
			{
				*rp++ = RUNCHAR;
				*rp++ = run;
				*rp++ = cur;
				run = 0;
			}
			else
				run++;
		}
		else
		{
			if (run)
			{
				*rp++ = RUNCHAR;
				*rp++ = run;
				run = 0;
			}
			*rp++ = cur;
		}

		last = cur;
	}

	if (run)
	{
		*rp++ = RUNCHAR;
		*rp++ = run;
	}

	*indexLength = 5 + count * 3 + (rp - sp);
} // Bitmap2Index

void Index2Bitmap(const void *indexed, long indexedLength,
	int *width, int *height, void **bitmap, long *bitmapLength)
{
	int colors, indices, i;
	rgb_color *clut;
	uchar *bp, *ip;
	
	*bitmapLength = 0;
	
	ip = (uchar *)indexed;
	
	*width = (*ip++) << 8;
	*width |= *ip++;
	*height = (*ip++) << 8;
	*height |= *ip++;
	
	colors = *ip++;

	clut = (rgb_color *)CALLOC(colors, sizeof(rgb_color));
	if (!clut) return;
	
	for (i = 0; i < colors; i++)
	{
		clut[i].red = *ip++;
		clut[i].green = *ip++;
		clut[i].blue = *ip++;
	}
	
	indices = indexedLength - 5 - 3 * colors;
	*bitmap = MALLOC(*width * *height * 3);
	if (!*bitmap) return;
	bp = (uchar *)*bitmap;
	*bitmapLength = *width * *height * 3;
	
	uchar *max = ip + indices, cur; //, *maxbp = bp + *bitmapLength;

	while (ip < max)
	{
		cur = *ip++;
		*bp++ = clut[cur].red;
		*bp++ = clut[cur].green;
		*bp++ = clut[cur].blue;
		ASSERT(bp <= maxbp);
		
		if (*ip == RUNCHAR)
		{
			ip++;
			uchar cnt = *ip++;
			
			while (cnt--)
			{
				*bp++ = clut[cur].red;
				*bp++ = clut[cur].green;
				*bp++ = clut[cur].blue;
				ASSERT(bp <= maxbp);
			}
		}
	}
} /* Index2Bitmap */
