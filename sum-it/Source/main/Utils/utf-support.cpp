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
	utf-support
	
	Copyright 1997, Hekkelman Programmatuur
	
*/

#include "utf-support.h"
#include <Debug.h>
#include <cstring>

int mstrlen(const char *s)
{
	int result = 0, i;
	
	while (*s)
	{
		if ((*s & 0x80) == 0)
			i = 0;
		else if ((*s & 0xC0) == 0x80)
			i = 0;	// this is a following char ????
		else if ((*s & 0xE0) == 0xC0)
			i = 1;
		else if ((*s & 0xF0) == 0xE0)
			i = 2;
		else if ((*s & 0xF8) == 0xF0)
			i = 3;
		else if ((*s & 0xFC) == 0xF8)
			i = 4;
		else if ((*s & 0xFE) == 0xFC)
			i = 5;
		
		result++;
		s++;
		
		while (i-- && (*s & 0xC0) == 0x80)
			s++;
	}
	
	return result;
} /* mstrlen */

void mstrcpy(char *dst, const char *src, int count)
{
	const char *max = src + strlen(src);
	
	ASSERT(src);
	
	while (*src && count-- && src < max)
	{
		int i = 0;
		
		if ((*src & 0x80) == 0)
			i = 0;
		else if ((*src & 0xE0) == 0xC0)
			i = 1;
		else if ((*src & 0xF0) == 0xE0)
			i = 2;
		else if ((*src & 0xF8) == 0xF0)
			i = 3;
		else if ((*src & 0xFC) == 0xF8)
			i = 4;
		else if ((*src & 0xFE) == 0xFC)
			i = 5;
		
		*dst++ = *src++;
		while (i-- && (*src & 0xC0) == 0x80)
			*dst++ = *src++;
	}

	*dst = 0;
} /* mstrcpy */

int mcharlen(const char *src)
{
	int result, i;
	
	if ((*src & 0x80) == 0)
		return 1;
	else if ((*src & 0xE0) == 0xC0)
		i = 1;
	else if ((*src & 0xF0) == 0xE0)
		i = 2;
	else if ((*src & 0xF8) == 0xF0)
		i = 3;
	else if ((*src & 0xFC) == 0xF8)
		i = 4;
	else if ((*src & 0xFE) == 0xFC)
		i = 5;
	
	result = 1;
	src++;

	while (i-- && (*src++ & 0xC0) == 0x80)
		result++;
	
	return result;
} /* mcharlen */

char *moffset(char *s, int count)
{
	char *r = s;
	
	while (count--)
	{
		int l = mcharlen(r);
		if (!l) return r;
		r += l;
	}
	
	return r;
} /* moffset */

int municode(const char *s) {
	const uchar *us = (uchar *)s;
	int unicode = 0;

	switch (mcharlen(s))
	{
		case 1:
			unicode = *us;
			break;
		case 2:
			unicode = ((us[0] << 6) & 0x07C0) | (us[1] & 0x003F);
			break;
		case 3:
			unicode = ((us[0] << 12) & 0x0F000) | ((us[1] << 6) & 0x0FC0) | (us[2] & 0x003F);
			break;
		case 4:
			unicode = ((us[0] << 18) & 0x01C0000) | ((us[1] << 12) & 0x03F000) |
				((us[2] << 6) & 0x0FC0) | (us[3] & 0x003F);
			break;
		case 5:
			unicode = ((us[0] << 24) & 0x03000000) | ((us[1] << 18) & 0x0FC0000) |
				((us[2] << 12) & 0x03F000) | ((us[3] << 6) & 0x0FC0) | (us[4] & 0x003F);
			break;
		default:
			// help!
			printf("char (%s) is %d bytes long!\n", s, mcharlen(s));
			ASSERT(false);
			break;
	}
	
	return unicode;
} /* municode */
