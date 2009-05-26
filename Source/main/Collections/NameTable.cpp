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
	NameTable.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "CellView.h"
#include "NameTable.h"
#include "StLocker.h"

CName::CName()
{
	fName[0] = 0;
} /* CName::CName */

CName::CName(const char *inName)
{
	strncpy(fName, inName, 31);
	fName[31] = 0;
} /* CName::CName */

bool CName::operator<(const CName& inName) const
{
	return strcasecmp(fName, inName.fName) < 0;
} /* CName::operator< */

bool CName::operator==(const CName& inName) const
{
	return strcasecmp(fName, inName.fName) == 0;
} /* CName::operator== */

void CName::operator=(CName& inName)
{
	strcpy(fName, inName.fName);
} /* CName::operator= */

void CName::operator=(const char *inName)
{
	strncpy(fName, inName, 31);
	fName[31] = 0;
} /* CName::operator= */

CName::operator const char*() const
{
	return fName;
} /* CName::operator char * */

CNameTable::CNameTable()
{
} /* CNameTable::CNameTable */

void CNameTable::Offset(bool horizontal, int first, int count)
{
	namemap::iterator i;
	
	for (i = begin(); i != end(); i++)
	{
		range& r = (*i).second;
		
		if (count > 0)
		{
			if (horizontal)
			{
				if (r.left >= first)
					r.OffsetBy(count, 0);
				else if (r.left < first && r.right >= first)
					r.right += count;
			}
			else
			{
				if (r.top >= first)
					r.OffsetBy(0, count);
				else if (r.top < first && r.bottom >= first)
					r.bottom += count;
			}
		}
		else
		{
			if (horizontal)
			{
				if (r.left >= first)
					r.OffsetBy(count, 0);
				else if (r.left < first && r.right >= first)
					r.right += count;
			}
			else
			{
				if (r.top >= first)
					r.OffsetBy(0, count);
				else if (r.top < first && r.bottom >= first)
					r.bottom += count;
			}
		}
	}
} /* CNameTable::Offset */
