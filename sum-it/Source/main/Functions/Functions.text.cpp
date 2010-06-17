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
	Functions.text.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "Container.h"
#include "FunctionUtils.h"
#include "Functions.h"
#include "Formatter.h"
#if __BEOS__ || __HAIKU__
#include "utf-support.h"
#include <cstring>
#else
#	define mstrlen strlen
#	define mstrcpy strncpy
#	define moffset(s,i)	((s) + i)
#endif

void ASCFunction(Value *stack, int argCnt, CContainer *cells)
{
	char s[256];
	double d;
	
	if (GetTextArgument(stack, argCnt, 1, s) &&
		strlen(s) == 1)
		stack[0] = (double)s[0];
	else if (GetDoubleArgument(stack, argCnt, 1, &d) && isnan(d))
		stack[0] = d;
	else
		stack[0] = gValueNan;
}

void CHRFunction(Value *stack, int argCnt, CContainer *cells)
{
	double num;
	
	if (GetDoubleArgument(stack, argCnt, 1, &num) &&
		num > 0.0 && num <= 127.0)
	{
		if (isnan(num))
			stack[0] = num;
		else
		{
			char s[2];
			s[1] = 0;
			s[0] = static_cast<char>(rint(num));
			stack[0] = s;
		}
	}
	else
		stack[0] = gValueNan;
}

void LEFTFunction(Value *stack, int argCnt, CContainer *cells)
{
	char s[256];
	double count;
	
	if (CheckForNanParameters(stack, argCnt))
		return;

	if (GetTextArgument(stack, argCnt, 1, s) && 
		GetDoubleArgument(stack, argCnt, 2, &count))
	{
		char s2[512];
		mstrcpy(s2, s, static_cast<int>(count) );
		stack[0] = s2;
	}
	else
		stack[0] = gValueNan;
}

void LENGTHFunction(Value *stack, int argCnt, CContainer *cells)
{
	char s[256];
	double d;
	
	if (GetTextArgument(stack, argCnt, 1, s))
		stack[0] = (double)mstrlen(s);
	else if (GetDoubleArgument(stack, argCnt, 1, &d) && isnan(d))
		stack[0] = d;
	else
		stack[0] = gValueNan;
}

void MIDFunction(Value *stack, int argCnt, CContainer *cells)
{
	char s[256];
	double start, count;
	int starti, counti;
	
	if (CheckForNanParameters(stack, argCnt))
		return;

	if (GetTextArgument(stack, argCnt, 1, s) && 
		GetDoubleArgument(stack, argCnt, 2, &start) &&
		GetDoubleArgument(stack, argCnt, 3, &count))
	{
		starti = static_cast<int>(rint(start) - 1);
		counti = static_cast<int>(rint(count) );
		int sLen = mstrlen(s);
		
		if (sLen > starti + counti)
			mstrcpy(s, moffset(s, starti), counti);
		else if (sLen > starti)
			mstrcpy(s, moffset(s, starti), sLen - starti);
		else
			s[0] = 0;

		stack[0] = s;
	}
	else
		stack[0] = gValueNan;
}

void NUM2CFunction(Value *stack, int argCnt, CContainer *cells)
{
	double num;
	
	if (GetDoubleArgument(stack, argCnt, 1, &num))
	{
		char s[32];
		ftoa(num, s);
		stack[0] = s;
	}
	else
		stack[0] = gValueNan;
}

void RIGHTFunction(Value *stack, int argCnt, CContainer *cells)
{
	char s[256];
	double count;
	int counti;
	
	if (CheckForNanParameters(stack, argCnt))
		return;

	if (GetTextArgument(stack, argCnt, 1, s) && 
		GetDoubleArgument(stack, argCnt, 2, &count))
	{
		counti = static_cast<int32>(rint(count) );
		int sLen = mstrlen(s);
		
		if (sLen > counti)
			mstrcpy(s, moffset(s, sLen - counti), counti);

		stack[0] = s;
	}
	else
		stack[0] = gValueNan;
}

