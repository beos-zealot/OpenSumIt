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
	range.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   RANGE_H
#include "Range.h"
#endif

#include <cstring>

void range::GetName(char *outString) const
{
	TopLeft().GetName(outString);
	if (!(TopLeft() == BotRight()))
	{
		char t[12];
		BotRight().GetName(t);
		strcat(outString, "..");
		strcat(outString, t);
	}
} /* range::GetName */

void range::GetFormulaName(char *outString, cell inLocation) const
{
	TopLeft().GetFormulaName(outString, inLocation);
	if (!(TopLeft() == BotRight()))
	{
		char t[12];
		BotRight().GetFormulaName(t, inLocation);
		strcat(outString, "..");
		strcat(outString, t);
	}
} /* GetFormulaName */

range range::GetFlatRange(cell loc) const
{
	range result;
	result.TopLeft() = TopLeft().GetFlatCell(loc);
	result.BotRight() = BotRight().GetFlatCell(loc);
	return result;
} /* range::GetFlatRange */

range range::GetRefRange(cell loc) const
{
	range result;
	result.TopLeft() = TopLeft().GetRefCell(loc);
	result.BotRight() = BotRight().GetRefCell(loc);
	return result;
} /* range::GetRefRange */

void range::Offset(cell inLocation, bool horizontal, int first, int count)
{
	bool wasSpecial = false;
	range rf = GetFlatRange(inLocation);
	
	if (horizontal)
		if (count < 0)
		{
			if (rf.left > first + count && rf.right <= first)
			{
				TopLeft() = cell::InvalidCell;
				BotRight() = cell::InvalidCell;
				wasSpecial = true;
			}
			else if (rf.left > first + count && rf.left <= first)
			{
				int dx = first - rf.left + 1;
				if (inLocation.h < first)
					dx += count;
				TopLeft().OffsetRefBy(dx, 0);
				BotRight().Offset(inLocation, true, first, count);
				wasSpecial = true;
			}
			else if (rf.right > first + count && rf.right <= first)
			{
				int dx = first - rf.right;
				if (inLocation.h < first)
					dx += count;
				BotRight().OffsetRefBy(dx, 0);
				TopLeft().Offset(inLocation, true, first, count);
				wasSpecial = true;
			}
		}
		else
		{
			if (rf.left < first && rf.right >= first)
			{
				if (inLocation.h < first)
					BotRight().OffsetRefBy(count, 0);
				else
					TopLeft().OffsetRefBy(-count, 0);
				wasSpecial = true;
			}
		}
	else
		if (count < 0)
		{
			if (rf.top > first + count && rf.bottom <= first)
			{
				TopLeft() = cell::InvalidCell;
				BotRight() = cell::InvalidCell;
				wasSpecial = true;
			}
			else if (rf.top > first + count && rf.top <= first)
			{
				int dy = first - rf.top + 1;
				if (inLocation.v < first)
					dy += count;
				TopLeft().OffsetRefBy(0, dy);
				BotRight().Offset(inLocation, true, first, count);
				wasSpecial = true;
			}
			else if (rf.bottom > first + count && rf.bottom <= first)
			{
				int dy = first - rf.bottom;
				if (inLocation.v < first)
					dy += count;
				BotRight().OffsetRefBy(0, dy);
				TopLeft().Offset(inLocation, true, first, count);
				wasSpecial = true;
			}
		}
		else
		{
			if (rf.top < first && rf.bottom >= first)
			{
				if (inLocation.v < first)
					BotRight().OffsetRefBy(0, count);
				else
					TopLeft().OffsetRefBy(0, -count);
				wasSpecial = true;
			}
		}

	if (!wasSpecial)
	{
		TopLeft().Offset(inLocation, horizontal, first, count);
		BotRight().Offset(inLocation, horizontal, first, count);
	}
} /* range::Offset */

void range::GetRCName(char *name) const
{
	TopLeft().GetRCName(name);
	if (!(TopLeft() == BotRight()))
	{
		char t[12];
		BotRight().GetRCName(t);
		strcat(name, ":");
		strcat(name, t);
	}
} // range::GetRCName
