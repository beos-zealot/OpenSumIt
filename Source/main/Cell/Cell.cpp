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
	Cell.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/


#ifndef   CELL_H
#include "Cell.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif

#include <cstring>

#include <ctype.h>
#include <support/Debug.h>

cell cell::InvalidCell(~FMASK, 0);

int cell::GetCell(const char *inCellName, cell& outCell)
{
	const char *p = inCellName;

	outCell.h = 0;
	outCell.v = 0;

	while (*inCellName && isalpha(*inCellName))
	{
		outCell.h *= 26;
		outCell.h += *inCellName & 0xDF - '@';
		inCellName++;
	}

	if (outCell.h <= 0 || outCell.h > kColCount)
		return 0;

	while (*inCellName && isdigit(*inCellName))
	{
		outCell.v *= 10;
		outCell.v += *inCellName - '0';
		inCellName++;
	}

	if (outCell.v <= 0 || outCell.v > kRowCount)
		return 0;

	return inCellName - p;
} /* GetCell */

int cell::GetFormulaCell(const char *inName, cell& inRef, cell& outCell)
{
	bool vAbs = false, hAbs = false;
	const char *p = inName;

	outCell.h = 0;
	outCell.v = 0;

	if (*inName == '$')
	{
		hAbs = true;
		inName++;
	}

	while (*inName && isalpha(*inName))
	{
		outCell.h *= 26;
		outCell.h += *inName & 0xDF - '@';
		inName++;
	}

	if (outCell.h <= 0 || outCell.h > kColCount)
		return 0;

	if (!hAbs)
		outCell.h -= inRef.h;
		
	if (*inName == '$')
	{
		vAbs = true;
		inName++;
	}

	while (*inName && isdigit(*inName))
	{
		outCell.v *= 10;
		outCell.v += *inName - '0';
		inName++;
	}

	if (outCell.v <= 0 || outCell.v > kRowCount)
		return 0;

	if (!vAbs)
		outCell.v -= inRef.v;

	if (vAbs)
		outCell.h ^= VFIXED;
	if (hAbs)
		outCell.h ^= HFIXED;

	return inName - p;
} /* GetFormulaCell */

void cell::GetName(char *name) const
{
	ASSERT(v > 0 && h > 0);

	char s[10];
	
	bool hFixed, vFixed;
	hFixed = H_Fixed();
	vFixed = V_Fixed();
	
	int H = h & FMASK, V = v;
	
	if (H == 0 || V == 0)
	{
		getindstring(name, kNanStrings, 18);
		return;
	}
	
	if (hFixed)
		strcpy(name, "$");
	else
		name[0] = 0;
	NumToAString(H, s);
	strcat(name, s);
	
	if (vFixed)
		strcat(name, "$");
	
	sprintf(s, "%d", V);
	strcat(name, s);
} /* GetName */

void cell::GetFormulaName(char *name, cell loc) const
{
	cell t;
	
	int H = abs(h);
	
	if (H & HFIXED)
		t.h = h & FMASK;
	else if (h < 0)
		t.h = loc.h - (H & FMASK);
	else
		t.h = loc.h + (h & FMASK);
	
	if (H & VFIXED)
		t.v = v;
	else
		t.v = loc.v + v;
	
	t.h ^= (H & ~FMASK);
	
	if (t.v == 0 && t.h == ~FMASK)
		getindstring(name, 1, 18);
	else if (t.v < 0 || t.h < 0)
		getindstring(name, 1, 18);
	else
		t.GetName(name);
} /* GetFormulaName */


cell cell::GetFlatCell(cell loc) const
{
	if (*this == cell::InvalidCell)
		return *this;

	bool hAbs, vAbs;
	int H;
	
	hAbs = H_Fixed();
	vAbs = V_Fixed();
	
	H = h;
	
	if (vAbs)
	{
		loc.v = v;
		H ^= VFIXED;
	}
	else
		loc.v += v;
	
	if (hAbs)
		loc.h = H & FMASK;
	else
		loc.h += H;
		
	return loc;
} /* cell::GetFlatCell */

cell cell::GetRefCell(cell loc) const
{
	cell result;
	
	if (h < 0)
		result.h = HFIXED | -h;
	else
		result.h = h - loc.h;

	if (v < 0)
	{
		result.h |= VFIXED;
		result.v = -v;
	}
	else
		result.v = v - loc.v;

	return result;
} /* cell::GetRefCell */

cell cell::GetMacCell(cell loc) const
{
	bool hAbs, vAbs;
	int H;
	
	hAbs = H_Fixed();
	vAbs = V_Fixed();
	
	H = h;
	
	if (vAbs)
	{
		loc.v = v;
		H ^= VFIXED;
	}
	else
		loc.v += v;
	
	if (hAbs)
		loc.h = H & FMASK;
	else
		loc.h += H;
	
	if (vAbs)
		loc.v = -loc.v;
	if (hAbs)
		loc.h = -loc.h;
	
	return loc;
} /* cell::GetMacCell */

void cell::OffsetRefBy(int x, int y)
{
	int F = abs(h) & ~FMASK;
	
	if (x)
	{
		h ^= F;
		h += x;
		h ^= F;
	}
	
	if (y)
	{
		v += y;
	}
} /* cell::OffsetBy */

void cell::Offset(cell inLocation, bool horizontal,
	int first, int count)
{
	cell cf = GetFlatCell(inLocation);
	
	if (horizontal && !H_Fixed())
	{
		if (count < 0)
		{
			if (inLocation.h <= first + count && cf.h > first)
				OffsetRefBy(count, 0);
			else if (inLocation.h > first && cf.h <= first + count)
				OffsetRefBy(-count, 0);
			else if (cf.h > first + count && cf.h <= first)
				*this = cell::InvalidCell;
		}
		else
		{
			if (inLocation.h < first && cf.h >= first)
				OffsetRefBy(count, 0);
			else if (inLocation.h >= first && cf.h < first)
				OffsetRefBy(-count, 0);
		}
	}
	else if (!horizontal && !V_Fixed())
	{
		if (count < 0)
		{
			if (inLocation.v <= first + count && cf.v > first)
				OffsetRefBy(0, count);
			else if (inLocation.v > first && cf.v <= first + count)
				OffsetRefBy(0, -count);
			else if (cf.v > first + count && cf.v <= first)
				*this = cell::InvalidCell;
		}
		else
		{
			if (inLocation.v < first && cf.v >= first)
				OffsetRefBy(0, count);
			else if (inLocation.v >= first && cf.v < first)
				OffsetRefBy(0, -count);
		}
	}
} /* cell::Offset */

void cell::Offset(int h, int v)
{
	if (h && H_Fixed())
		h = 0;

	if (v && V_Fixed())
		v = 0;
	
	OffsetRefBy(h, v);
} /* cell::Offset */

void cell::GetRCName(char *name) const
{
	int x, y;
	x = H();
	y = v;
	
	if (x && y)
		sprintf(name, "R[%d]C[%d]", y, x);
	else if (x)
		sprintf(name, "RC[%d]", x);
	else if (y)
		sprintf(name, "R[%d]C", y);
	else
		sprintf(name, "RC"); // invalid ??
} // cell::GetRCName
