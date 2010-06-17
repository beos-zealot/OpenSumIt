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
	CellUtils.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include <ctype.h>
//#include <stdlib.h>
#include "Container.h"
#include "CellUtils.h"
#include "CellView.h"
#include "CellWindow.h"
#include "Utils.h"
//#include "RekenbladConstants.h"

void GetCellName(cell c, char *name)
{
	c.GetName(name);
} /* GetCellName */

void GetRangeName(range& r, char *name)
{
	r.GetName(name);
} /* GetRangeName */

bool GetCellNum(char *inCellName, cell& outCell)
{
	bool isAbsolute = false;

	outCell.h = 0;
	outCell.v = 0;

	if (*inCellName == '$')
	{
		isAbsolute = true;
		inCellName++;
	}

	while (*inCellName && isalpha(*inCellName))
	{
		outCell.h *= 26;
		outCell.h += *inCellName & 0xDF - '@';
		inCellName++;
	}

	if (outCell.h <= 0 || outCell.h > kColCount)
		return false;

	if (isAbsolute)
		outCell.h = -outCell.h;
	isAbsolute = false;

	if (*inCellName == '$')
	{
		isAbsolute = true;
		inCellName++;
	}

	while (*inCellName && isdigit(*inCellName))
	{
		outCell.v *= 10;
		outCell.v += *inCellName - '0';
		inCellName++;
	}

	if (outCell.v <= 0 || outCell.v > kRowCount)
		return false;

	if (isAbsolute)
		outCell.v = -outCell.v;

	return true;
} /* GetCellNum */

void OffsetCell(cell& ioCell, cell inOffset, SplitType what, int split, int shift)
{
	bool wasAbsolute = false;

	if (what == hSplit)
	{
		if (abs(ioCell.h) >= split)
		{

			if (ioCell.h < 0)
			{
				wasAbsolute = true;
				ioCell.h = -ioCell.h;
			}
			else
				wasAbsolute = false;

			if (shift < 0 && ioCell.h < split - shift)
				ioCell.v = ioCell.h = 0;
			else
				ioCell.h += shift;

			if (wasAbsolute)
				ioCell.h = -ioCell.h;
		}
	}
	else if (ioCell.h > 0)
		ioCell.h = std::max(ioCell.h + inOffset.h, 0);
	
	if (what == vSplit)
	{
		if (abs(ioCell.v) >= split)
		{

			if (ioCell.v < 0)
			{
				wasAbsolute = true;
				ioCell.v = -ioCell.v;
			}
			else
				wasAbsolute = false;

			if (shift < 0 && ioCell.v < split - shift)
				ioCell.h = ioCell.v = 0;
			else
				ioCell.v += shift;

			if (wasAbsolute)
				ioCell.v = -ioCell.v;
		}
	}
	else if (ioCell.v > 0)
		ioCell.v = std::max(ioCell.v + inOffset.v, 0);

	if (abs(ioCell.v) > kRowCount || abs(ioCell.h) > kColCount)
		ioCell.h = ioCell.v = 0;
} /* OffsetCell */

void OffsetRange(range& ioRange, cell inOffset, SplitType what, int split, int shift)
{
	bool wasSpecial = false;

	if (shift < 0 && what != noSplit)
		if (what == hSplit)
		{

			if (abs(ioRange.left) >= split && abs(ioRange.right) <= split - shift)
				ioRange.Set(0, 0, 0, 0);
			else if (abs(ioRange.left) >= split && abs(ioRange.left) <= split - shift)
			{
				if (ioRange.left < 0)
					ioRange.left = -split;
				else
					ioRange.left = split;
				OffsetCell(ioRange.BotRight(), inOffset, what, split, shift);
				wasSpecial = true;
			}
			else if (abs(ioRange.right) >= split && abs(ioRange.right) <= split - shift)
			{
				if (ioRange.right < 0)
					ioRange.right = -split + 1;
				else
					ioRange.right = split - 1;
				OffsetCell(ioRange.TopLeft(), inOffset, what, split, shift);
				wasSpecial = true;
			}
		}
		else if (what == vSplit)
		{

			if (abs(ioRange.top) >= split && abs(ioRange.bottom) <= split - shift)
				ioRange.Set(0, 0, 0, 0);
			else if (abs(ioRange.top) >= split && abs(ioRange.top) <= split - shift)
			{
				if (ioRange.top < 0)
					ioRange.top = -split;
				else
					ioRange.top = split;
				OffsetCell(ioRange.BotRight(), inOffset, what, split, shift);
				wasSpecial = true;
			}
			else if (abs(ioRange.bottom) >= split && abs(ioRange.bottom) <= split - shift)
			{
				if (ioRange.bottom < 0)
					ioRange.bottom = -split + 1;
				else
					ioRange.bottom = split - 1;
				OffsetCell(ioRange.TopLeft(), inOffset, what, split, shift);
				wasSpecial = true;
			}
		}

	if (!wasSpecial)
	{
		OffsetCell(ioRange.TopLeft(), inOffset, what, split, shift);
		OffsetCell(ioRange.BotRight(), inOffset, what, split, shift);
	}
} /* OffsetRange */

bool CellInRange(cell c, range& r)
{
	return (c.v >= r.top && c.v <= r.bottom && c.h >= r.left && c.h <= r.right);
} /* CellInRange */
