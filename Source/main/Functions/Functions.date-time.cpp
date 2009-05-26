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
	Functions.date-time.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "Container.h"
#include "FunctionUtils.h"
#include "Functions.h"
#include "Formatter.h"
#include "Globals.h"

void DATEFunction(Value *stack, int argCnt, CContainer *cells)
{
	double day, month, year;
	bool validDate = false;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &year) &&
		GetDoubleArgument(stack, argCnt, 2, &month) &&
		GetDoubleArgument(stack, argCnt, 3, &day))
	{
		int d, m, y;
		
		d = static_cast<int>(day);
		m = static_cast<int>(month) ;
		y = static_cast<int>(year) ;
		if( y > 1900 )
			y -= 1900 ;
		
		if (d < 1 || d > 31 || m < 1 || m > 12 || y < 0 || y > 200)
			validDate = false;
		else
		{
			struct tm time, *lt;
			memset(&time, 0, sizeof(time));
			time.tm_year = y;
			time.tm_mon = m - 1;
			time.tm_mday = d;
			
			time_t t = mktime(&time);
			stack[0] = t;
			
			lt = localtime(&t);
			
			validDate =
				lt->tm_year == y &&
				lt->tm_mon == m - 1 &&
				lt->tm_mday == d;
		}
	}

	if (!validDate)
		stack[0] = gDateNan;
}

void DAYFunction(Value *stack, int argCnt, CContainer *cells)
{
	time_t theDate;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetTimeArgument(stack, argCnt, 1, &theDate))
	{
		struct tm *theDateRec;
		theDateRec = localtime(&theDate);
		
		stack[0] = (double)theDateRec->tm_mday;
	}
	else
		stack[0] = gDateNan;
}

void DOWFunction(Value *stack, int argCnt, CContainer *cells)
{
	time_t theDate;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetTimeArgument(stack, argCnt, 1, &theDate))
	{
		struct tm *theDateRec;
		theDateRec = localtime(&theDate);
		
		stack[0] = (double)theDateRec->tm_wday + 1;
	}
	else
		stack[0] = gDateNan;
}

void HOURFunction(Value *stack, int argCnt, CContainer *cells)
{
	time_t theDate;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetTimeArgument(stack, argCnt, 1, &theDate))
	{
		struct tm *theDateRec;
		theDateRec = localtime(&theDate);
		
		stack[0] = (double)theDateRec->tm_hour;
	}
	else
		stack[0] = gDateNan;
}

void MINUTEFunction(Value *stack, int argCnt, CContainer *cells)
{
	time_t theDate;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetTimeArgument(stack, argCnt, 1, &theDate))
	{
		struct tm *theDateRec;
		theDateRec = localtime(&theDate);
		
		stack[0] = (double)theDateRec->tm_min;
	}
	else
		stack[0] = gDateNan;
}

void MONTHFunction(Value *stack, int argCnt, CContainer *cells)
{
	time_t theDate;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetTimeArgument(stack, argCnt, 1, &theDate))
	{
		struct tm *theDateRec;
		theDateRec = localtime(&theDate);
		
		stack[0] = (double)(theDateRec->tm_mon + 1);
	}
	else
		stack[0] = gDateNan;
}

void NOWFunction(Value *stack, int , CContainer *)
{
	time_t now;
	time(&now);
	stack[0] = now;
} /* NOWFunction */

void SECONDFunction(Value *stack, int argCnt, CContainer *cells)
{
	time_t theDate;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetTimeArgument(stack, argCnt, 1, &theDate))
	{
		struct tm *theDateRec;
		theDateRec = localtime(&theDate);
		
		stack[0] = (double)theDateRec->tm_sec;
	}
	else
		stack[0] = gDateNan;
}

void TIMEFunction(Value *stack, int argCnt, CContainer *cells)
{
	double hour, minute, second;
	bool validTime = false;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &hour) &&
		GetDoubleArgument(stack, argCnt, 2, &minute) &&
		GetDoubleArgument(stack, argCnt, 3, &second))
	{
		int h, m, s;
		
		h = static_cast<int>(hour) ;
		m = static_cast<int>(minute) ;
		s = static_cast<int>(second) ;

		validTime = (h >= 0 && h < 24) &&
					(m >= 0 && m < 60) &&
					(s >= 0 && s < 60);

		stack[0] = (time_t)(((h * 60) + m) * 60 + s);
	}

	if (!validTime)
		stack[0] = gTimeNan;
}

void TIME2CFunction(Value *stack, int argCnt, CContainer* cells)
{
	time_t t;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetTimeArgument(stack, argCnt, 1, &t))
	{
		Value v = t;
		char s[64];
		gFormatTable.FormatValue(0, v, s);
		stack[0] = s;
	}
	else
		stack[0] = gTimeNan;
}

void YEARFunction(Value *stack, int argCnt, CContainer *cells)
{
	time_t theDate;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetTimeArgument(stack, argCnt, 1, &theDate))
	{
		struct tm *theDateRec;
		theDateRec = localtime(&theDate);
		
		stack[0] = (double)(theDateRec->tm_year + 1900);
	}
	else
		stack[0] = gDateNan;
}

