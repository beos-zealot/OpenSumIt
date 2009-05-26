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
	range.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef RANGE_H
#define RANGE_H

#include "Cell.h"

// our love for egcs has no limits... puke

struct _range
{
	short top, left, bottom, right;
};

class range : public _range {
public:
	range()
	{
		top = left = bottom = right = 0;
	}
	range(int inLeft, int inTop, int inRight, int inBottom)
	{
		top = inTop;
		left = inLeft;
		bottom = inBottom;
		right = inRight;
	}
	range(const _range& r)
	{
		top = r.top;
		left = r.left;
		bottom = r.bottom;
		right = r.right;
	}
	
	void GetName(char *outString) const;
	void GetRCName(char *outString) const;
	void GetFormulaName(char *outString, cell inLocation) const;
	
	cell& TopLeft() const
		{ return *((cell *)&top); };
	cell& BotRight() const
		{ return *((cell *)&bottom); };
	
	range GetFlatRange(cell loc) const;
	range GetRefRange(cell loc) const;
	
	void Offset(cell inLocation, bool horizontal,
		int first, int count);
	
	void Set(int l, int t, int r, int b)
		{ top = t, left = l, bottom = b, right = r; };
	
	void OffsetBy(int x, int y)
		{ top += y; left += x; bottom += y; right += x; };
		
	bool operator== (const range& r)
		{ return TopLeft() == r.TopLeft() && BotRight() == r.BotRight(); }
	
	inline bool Contains(cell c) const;
	inline bool IsValid() const;
};

inline bool range::Contains(cell c) const
{
	return c.h >= left && c.h <= right && c.v >= top && c.v <= bottom;
} /* range::Contains */

inline bool range::IsValid() const
{
	return left >= 1 && left <= right && left <= kColCount &&
		top >= 1 && top <= bottom && bottom <= kRowCount &&
		TopLeft() != cell::InvalidCell &&
		BotRight() != cell::InvalidCell;
} /* range::IsValid */

#endif
