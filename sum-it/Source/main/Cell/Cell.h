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
	Cell.h

	Copyright 1997, Hekkelman Programmatuur

	Part of Sum-It for the BeBox version 1.1.

*/

#include <stdlib.h>
#include <SupportDefs.h>

#ifndef CELL_H
#define CELL_H

#if ! cell_defined
#define cell_defined 1

#ifndef   CONSTANTS_H
#include "Constants.h"
#endif

#define VFIXED	0x4000
#define HFIXED	0x2000
#define FMASK	(~(VFIXED | HFIXED))

struct cell {
	int16	v;
	int16	h;

	cell()
		{ v = 0; h = 0; };
	cell(int nh, int nv)
		{ v = nv; h = nh; };

	void Set(int nh, int nv)
		{ v = nv; h = nh; };
	void Set(const char *name)
		{ GetCell(name, *this); }

	int H() const
		{ return h < 0 ? h | ~FMASK : h & FMASK; }
	bool V_Fixed() const
		{ return (abs(h) & VFIXED) != 0; }
	bool H_Fixed() const
		{ return (abs(h) & HFIXED) != 0; }

	void GetName(char *) const;
	void GetRCName(char *) const;
	void GetFormulaName(char *, cell) const;

	cell GetFlatCell(cell loc) const;
	cell GetRefCell(cell loc) const;
	cell GetMacCell(cell loc) const;

	void OffsetRefBy(int x, int y);
	void Offset(cell inLocation, bool horizontal, int first, int count);
	void Offset(int h, int v);

	bool IsValid() const
		{ return v > 0 && v <= kRowCount && h > 0 && h <= kColCount; };

	static cell InvalidCell;

	bool operator==(const cell& c) const;
	bool operator!=(const cell& c) const;
	bool operator<(const cell& c) const;
	bool operator<=(const cell& c) const;

	static int GetCell(const char *inName, cell& outCell);
	static int GetFormulaCell(const char *inName, cell& inRef, cell& outCell);
};

inline bool cell::operator==(const cell& c) const
{
	return *((int32 *)&c) == *((int32 *)this);
} /* cell::operator== */

inline bool cell::operator!=(const cell& c) const
{
	return *((int32 *)&c) != *((int32 *)this);
} /* cell::operator != */

#if !__POWERPC__

inline bool cell::operator<(const cell& c) const
{
	return (v < c.v) || ((v == c.v) && (h < c.h));
} /* cell::operator< */

inline bool cell::operator<=(const cell& c) const
{
	return (v < c.v) || ((v == c.v) && (h <= c.h));
} /* cell::operator<= */

#else

inline bool cell::operator<(const cell& c) const
{
	return *((int32 *)this) < *((int32 *)&c);
} /* cell::operator< */

inline bool cell::operator<=(const cell& c) const
{
	return *((int32 *)this) <= *((int32 *)&c);
} /* cell::operator<= */

#endif

#endif

#endif
