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
	CellIterator.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLITERATOR_H
#include "CellIterator.h"
#endif

#ifndef   CELLDATA_H
#include "CellData.h"
#endif

#ifndef   CELLUTILS_H
#include "CellUtils.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#include <Autolock.h>
#include <cstring>

CCellIterator::CCellIterator(CContainer *inContainer, range *sRange)
	: fCellData(inContainer->fCellData)
{
	if (sRange)
		fRange = *sRange;
	else
		fRange = range(1, 1, kColCount, kRowCount);
	fBeforeStart = true;
} /* CCellIterator */

void CCellIterator::Reset()
{
	fBeforeStart = true;
} /* Reset */

bool CCellIterator::GoTo(cell c)
{
	fBeforeStart = false;
	
	cellmap::iterator i = fCellData.upper_bound(c);
	
	return i != fCellData.end() && (*i).first == c;
} /* CCellIterator::GoTo */

bool CCellIterator::Next(cell& c)
{
	if (fBeforeStart)
	{
		fBeforeStart = false;
		c = fRange.TopLeft();
		return true;
	}
	
	c.h++;
	if (c.h > fRange.right)
	{
		c.h = fRange.left;
		c.v++;
		return c.v <= fRange.bottom;
	}
	else
		return true;
} /* CCellIterator::Next */

bool CCellIterator::NextExisting(cell& c)
{
	if (fBeforeStart)
	{
		fBeforeStart = false;
		c = fRange.TopLeft();
		
		if (fCellData.count(c))
			return true;
		
		c.h--;
	}

	while (true)
	{
		if (++c.h > fRange.right)
		{
			c.h = fRange.left;
			if (++c.v > fRange.bottom)
				break;
		}
		
		cellmap::iterator i = fCellData.lower_bound(c);
		
		if (i == fCellData.end())
			return false;
		
		c = (*i).first;
		if (CellInRange(c, fRange))
			return true;
	}

	return false;
} /* CCellIterator::NextExisting */

CInterface::CInterface(CContainer *container, range *sRange)
{
	fIterator = new CCellIterator(container, sRange);
} /* CInterface::CInterface */

CInterface::~CInterface()
{
	delete fIterator;
} /* CInterface::~CInterface */

bool CInterface::Next()
{
	try
	{
		return fIterator->NextExisting(fCurrent);
	}
	catch(...)
	{
		CATCHED;
		return false;
	}
} /* CInterface::Next */

bool CInterface::GetDouble(double& d)
{
	bool result = false;
	try
	{
		if (fIterator->fCellData[fCurrent].mType == eNumData)
		{
			d = fIterator->fCellData[fCurrent].mDouble;
			result = true;
		}
	}
	catch(...)
	{
		CATCHED;
	}
	return result;
}  /* CInterface::GetDouble */

bool CInterface::GetText(char *s, int maxLen)
{
	bool result = false;
	try
	{
		if (fIterator->fCellData[fCurrent].mType == eTextData)
		{
			maxLen = std::min((ulong)maxLen - 1, strlen(fIterator->fCellData[fCurrent].mText));
			strncpy(s, fIterator->fCellData[fCurrent].mText, maxLen);
			s[maxLen] = 0;
			result = true;
		}
	}
	catch(...)
	{
		CATCHED;
	}
	return result;
} /* CInterface::GetText */

bool CInterface::GetTime(time_t& t)
{
	bool result = false;
	try
	{
		if (fIterator->fCellData[fCurrent].mType == eTimeData)
		{
			t = fIterator->fCellData[fCurrent].mTime;
			result = true;
		}
	}
	catch(...)
	{
		CATCHED;
	}
	return result;
} /* CInterface::GetTime */

bool CInterface::GetBool(bool& b)
{
	bool result = false;
	try
	{
		if (fIterator->fCellData[fCurrent].mType == eBoolData)
		{
			b= fIterator->fCellData[fCurrent].mBool;
			result = true;
		}
	}
	catch(...)
	{
		CATCHED;
	}
	return result;
} /* CInterface::GetBool */


bool GetCellBool(void *cookie, cell c, bool *b)
{
//	CContainer *cc = dynamic_cast<CContainer *>(cookie);
//	if (!cc)
//		return false;
	CContainer *cc = (CContainer *)cookie;

	try
	{
		BAutolock lock(cc);
		Value v;
		if (cc->GetValue(c, v) && v.fType == eBoolData)
		{
			*b = v.fBool;
			return true;
		}
	}
	catch(...)
	{
		CATCHED;
	}
	return false;
} /* GetCellBool */

bool GetCellText(void *cookie, cell c, char *s)
{
//	CContainer *cc = dynamic_cast<CContainer *>(cookie);
//	if (!cc)
//		return false;
	CContainer *cc = (CContainer *)cookie;

	try
	{
		BAutolock lock(cc);
		Value v;
		if (cc->GetValue(c, v) && v.fType == eTextData)
		{
			strcpy(s, v.fText);
			return true;
		}
	}
	catch(...)
	{
		CATCHED;
	}
	return false;
} /* GetCellText */

bool GetCellDouble(void *cookie, cell c, double *d)
{
//	CContainer *cc = dynamic_cast<CContainer *>(cookie);
//	if (!cc)
//		return false;
	CContainer *cc = (CContainer *)cookie;

	try
	{
		BAutolock lock(cc);
		Value v;
		if (cc->GetValue(c, v) && v.fType == eNumData)
		{
			*d = v.fDouble;
			return true;
		}
	}
	catch(...)
	{
		CATCHED;
	}
	return false;
} /* GetCellDouble */

bool GetCellTime(void *cookie, cell c, time_t *t)
{
//	CContainer *cc = dynamic_cast<CContainer *>(cookie);
//	if (!cc)
//		return false;
	CContainer *cc = (CContainer *)cookie;

	try
	{
		BAutolock lock(cc);
		Value v;
		if (cc->GetValue(c, v) && v.fType == eTimeData)
		{
			*t = v.fTime;
			return true;
		}
	}
	catch (...) { }

	return false;
} /* GetCellTime */
