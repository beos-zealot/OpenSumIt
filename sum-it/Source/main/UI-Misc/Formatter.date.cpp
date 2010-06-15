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
	Formatter.date.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif


#include <ctime>
#include <cstdio>

DateOrder gDateOrder;

float CFormatter::FormatDate(const time_t t, char *s,
	int inFont, float inWidth)
{
	struct tm *lt;
	time_t t2 = t;
	lt = localtime(&t2);
	char f[24];
	int formatID = FormatID();

	int day, month, year, hour, minute, second;
	
	day = formatID & 1;
	month = formatID >> 1 & 3;
	year = formatID >> 3 & 3;
	hour = formatID >> 5 & 1;
	minute = formatID >> 6 & 1;
	second = formatID >> 7 & 1;
	
	if (formatID == 0)
	{
		if (t >= (24 * 60 * 60))
		{
			day = month = 1;
			year = 2;
		}
		else
		{
			hour = minute = 1;
			second = lt->tm_sec;
		}
	}

	if (day || month || year)
	{
		char *d, *m, *y, ds[2], *p[3];
		ds[1] = 0;
		
		if (day)
			d = "%d";
		else
			d = "";
		
		switch (month)
		{
			case 1: m = "%m"; ds[0] = gDateSeparator;break;
			case 2: m = "%b"; ds[0] = ' '; break;
			case 3: m = "%B"; ds[0] = ' '; break;
			default: m = ""; ds[0] = ' '; break;
		}
		
		switch (year)
		{
			case 1: y = "%y"; lt->tm_year %= 100; break;
			case 2: y = "%Y"; break;
			default: y = ""; break;
		}

		switch (gDateOrder)
		{
			case doDMY:	p[0] = d; p[1] = m; p[2] = y; break;
			case doMDY:	p[0] = m; p[1] = d; p[2] = y; break;
			case doDYM:	p[0] = d; p[1] = y; p[2] = m; break;
			case doMYD:	p[0] = m; p[1] = y; p[2] = d; break;
			case doYMD:	p[0] = y; p[1] = m; p[2] = d; break;
			case doYDM:	p[0] = y; p[1] = d; p[2] = m; break;
		}

		if (*p[0])
		{
			if (*p[1])
			{
				if (*p[2])
					sprintf(f, "%s%s%s%s%s", p[0], ds, p[1], ds, p[2]);
				else
					sprintf(f, "%s%s%s", p[0], ds, p[1]);
			}
			else if (*p[2])
				sprintf(f, "%s%s%s", p[0], ds, p[2]);
			else
				strcpy(f, p[0]);
		}
		else
		{
			if (*p[1])
			{
				if (*p[2])
					sprintf(f, "%s%s%s", p[1], ds, p[2]);
				else
					strcpy(f, p[1]);
			}
			else
				strcpy(f, p[2]);
		}

		strftime(s, 255, f, lt);
	}
	else
		*s = 0;

	if ((day || month || year) && (hour || minute || second))
		strcat(s, " ");
	
	if (hour || minute || second)
	{
		if (hour)
			strcpy(f, g24Hours ? "%H" : "%I");
		else
			strcpy(f, "");
		
		if (hour && minute)
		{
			f[2] = gTimeSeparator;
			f[3] = 0;
		}

		if (minute)
			strcat(f, "%M");
		
		if (minute && second)
		{
			int i = hour ? 5 : 2;
			f[i] = gTimeSeparator;
			f[i + 1] = 0;
		}
		
		if (second)
			strcat(f, "%S");
		
		if (hour && !g24Hours)
			strcat(f, "%p");

		strftime(s + strlen(s), 255 - strlen(s), f, lt);
	}
	
	CFontMetrics fm(gFontSizeTable[inFont]);
	float w = fm.StringWidth(s);
	if (w > inWidth)
	{
		int c = max(static_cast<int>(inWidth / fm['#']) , (int)0 ) ;
		
		s[c] = 0;

		while (c > 0)
			s[--c] = '#';

		w = c * fm['#'];
	}

	return w;
} /* FormatDate */
