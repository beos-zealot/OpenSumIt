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
	RunArray.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   RUNARRAY_H
#include "RunArray.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   ROUND_H
#include "Round.h"
#endif

#ifndef   CONFIG_H
#include "Config.h"
#endif

#include <cstring>

#include <support/Debug.h>
#include <ByteOrder.h>
#ifndef htonl
#	define htonl(x) B_HOST_TO_BENDIAN_INT32(x)
#	define ntohl(x) B_BENDIAN_TO_HOST_INT32(x)
#	define htons(x) B_HOST_TO_BENDIAN_INT16(x)
#	define ntohs(x) B_BENDIAN_TO_HOST_INT16(x)
#endif

CRunArray::CRunArray(int inMax, int inValue)
{
	fData = (RAElement *)CALLOC(2, sizeof(RAElement));
	FailNil(fData);
	fData[1].raIndex = 1;
	fData[1].raValue = inValue;
	fMax = inMax;
	fCount = 2;
} /* CRunArray::CRunArray */

CRunArray::CRunArray(CRunArray& inArray)
{
	fCount = inArray.fCount;
	fMax = inArray.fMax;
	fData = (RAElement *)MALLOC(fCount * sizeof(RAElement));
	FailNil(fData);
	memcpy(fData, inArray.fData, fCount * sizeof(RAElement));
} /* CRunArray::CRunArray */

CRunArray::~CRunArray()
{
	FREE(fData);
} /* CRunArray::~CRunArray */

float CRunArray::operator[] (const int inIndex) const
{
//	ASSERT(inIndex >= 0);
//	ASSERT(inIndex <= fMax);

	int offset = GetOffset(inIndex);
	return fData[offset].raCumul +
		fData[offset].raValue * (inIndex - fData[offset].raIndex + 1);
} /* CRunArray::operator[] */

void CRunArray::SetValue(int inIndex, float inValue)
{
	ASSERT(inIndex > 0);
	ASSERT(inIndex <= fMax);
	
	if (inIndex < 1 || inIndex > fMax)
		return;
	
	int offset = GetOffset(inIndex);
	
	if (fData[offset].raValue == inValue)
		return;
	
	if (inIndex == fMax)
	{
		if (fData[offset].raIndex == inIndex)
		{
			fData[offset].raValue = inValue;
		}
		else
		{
			RAElement *t = (RAElement *)
				REALLOC(fData, sizeof(RAElement) * (fCount + 1));
			FailNil(t);
			fData = t;
			
			fData[fCount].raValue = inValue;
			fData[fCount].raIndex = inIndex;

			fCount++;
		}
	}

	else if (fData[offset].raIndex == inIndex)
	{
		if (offset < fCount - 1 &&
			fData[offset + 1].raIndex == inIndex + 1)
		{
			fData[offset].raValue = inValue;
		}
		else
		{
			RAElement *t = (RAElement *)
				REALLOC(fData, sizeof(RAElement) * (fCount + 1));
			FailNil(t);
			fData = t;
			
			memmove(fData + offset + 1, fData + offset,
				(fCount - offset) * sizeof(RAElement));
			fCount++;
			
			fData[offset].raValue = inValue;
			fData[offset + 1].raIndex = inIndex + 1;
		}
	}

	else if (offset < fCount - 1 &&
			fData[offset + 1].raIndex == inIndex + 1)
	{
		RAElement *t = (RAElement *)
			REALLOC(fData, sizeof(RAElement) * (fCount + 1));
		FailNil(t);
		fData = t;
		
		memmove(fData + offset + 1, fData + offset,
			(fCount - offset) * sizeof(RAElement));
		fCount++;
		
		offset++;
		fData[offset].raValue = inValue;
		fData[offset].raIndex = inIndex;
	}

	else
	{
		RAElement *t = (RAElement *)
			REALLOC(fData, sizeof(RAElement) * (fCount + 2));
		FailNil(t);
		fData = t;
		
		memmove(fData + offset + 2, fData + offset,
			(fCount - offset) * sizeof(RAElement));
		fCount += 2;
		
		offset++;
		fData[offset].raValue = inValue;
		fData[offset].raIndex = inIndex;
		
		offset++;
		fData[offset].raIndex = inIndex + 1;
	}
	
	for (int i = 0; i < fCount - 1; i++)
		if (fData[i].raValue == fData[i + 1].raValue)
		{
			memmove(fData + i + 1, fData + i + 2,
				(fCount - i - 2) * sizeof(RAElement));
			fCount--;

			RAElement *t = (RAElement *)
				REALLOC(fData, sizeof(RAElement) * fCount);
			FailNil(t);
			fData = t;
			i--;
		}
	
	RecalcCumulatives();
} /* CRunArray::SetValue */

void CRunArray::SetValue(int inStart, int inStop, float inValue)
{
	ASSERT(inStart <= inStop);
	
	SetValue(inStart, inValue);
	
	if (inStart != inStop)
	{
		SetValue(inStop, inValue);
		
		int start = GetOffset(inStart);
		int stop = GetOffset(inStop);
		
		if (start != stop)
		{
			int n = stop - start;
			
			memmove(fData + start + 1, fData + stop + 1,
				sizeof(RAElement) * (fCount - stop));
			fCount -= n;
			
			RAElement *t = (RAElement *)
				REALLOC(fData, fCount * sizeof(RAElement));
			FailNil(t);
			fData = t;

			RecalcCumulatives();
		}
	}
} /* CRunArray::SetValue */

void CRunArray::InsertValues(int inIndex, int inCount, float inValue)
{
	int offset = GetOffset(inIndex);
	
	if (fData[offset].raValue != inValue)
	{
		if (fData[offset].raIndex == inIndex)
		{
			RAElement *t = (RAElement *)
				REALLOC(fData, sizeof(RAElement) * (fCount + 1));
			FailNil(t);
			fData = t;
			
			memmove(fData + offset + 1, fData + offset,
				(fCount - offset) * sizeof(RAElement));
			fCount++;
			
			fData[offset].raValue = inValue;
		}
		else
		{
			RAElement *t = (RAElement *)
				REALLOC(fData, sizeof(RAElement) * (fCount + 2));
			FailNil(t);
			fData = t;
			
			memmove(fData + offset + 2, fData + offset,
				(fCount - offset) * sizeof(RAElement));
			fCount += 2;
			
			offset++;
			fData[offset].raIndex = inIndex;
			fData[offset].raValue = inValue;
			
			fData[offset + 1].raIndex = inIndex;
		}
	}	

	for (int i = offset + 1; i < fCount; i++)
	{
		fData[i].raIndex += inCount;
		if (fData[i].raIndex > fMax)
		{
			fCount = i;
			RAElement *t = (RAElement *)
				REALLOC(fData, sizeof(RAElement) * fCount);
			FailNil(t);
			fData = t;
			break;
		}
	}

	RecalcCumulatives();
	
} /* CRunArray::InsertValues */

void CRunArray::DeleteValues(int inIndex, int inCount)
{
	float v = fData[GetOffset(inIndex + inCount)].raValue;
	
	SetValue(inIndex, inIndex + inCount, v);
	
	int offset = GetOffset(inIndex);
	
	for (int i = offset + 1; i < fCount; i++)
		fData[i].raIndex -= inCount;

	RecalcCumulatives();

} /* CRunArray::DeleteValues */

int CRunArray::GetOffset(int inIndex) const
{
	int L = 0, R = fCount - 1, i;
	
	while (L <= R)
	{
		i = (L + R) / 2;
		if (inIndex < fData[i].raIndex)
			R = i - 1;
		else if (inIndex > fData[i].raIndex)
			L = i + 1;
		else
			return i;
	}
	
	return R;
} /* CRunArray::GetOffset */

void CRunArray::RecalcCumulatives()
{
	for (int i = 1; i < fCount; i++)
	{
		ASSERT(fData[i].raIndex > fData[i - 1].raIndex);
		fData[i].raCumul = fData[i - 1].raCumul +
			(fData[i].raIndex - fData[i - 1].raIndex) *
			fData[i - 1].raValue;
	}
} /* CRunArray::RecalcCumulatives */

CRunArray& CRunArray::operator= (const CRunArray& inRunArray)
{
	fCount = inRunArray.fCount;
	fMax = inRunArray.fMax;
	fData = (RAElement *)REALLOC(fData, fCount * sizeof(RAElement));
	FailNil(fData);
	memcpy(fData, inRunArray.fData, fCount * sizeof(RAElement));
	return *this;
} /* CRunArray::operator= */


long CRunArray::Count() const
{
	return fCount;
} /* CRunArray::IOSize */

void CRunArray::Write(void *p)
{
	int i;
	short *sp = (short *)p;
	
	for (i = 0; i < fCount; i++)
	{
		*sp++ = htons((short)fData[i].raIndex);
		*sp++ = htons((short)fData[i].raValue);
	}
} /* CRunArray::Write */

void CRunArray::Read(int count, void *p)
{
	short i, *sp = (short *)p;
	RAElement *t = (RAElement *)MALLOC(count * sizeof(RAElement));
	FailNil(t);
	
	fCount = count;
	if (fData) FREE(fData);
	fData = t;
	
	for (i = 0; i < count; i++)
	{
		fData[i].raIndex = ntohs(*sp++);
		fData[i].raValue = ntohs(*sp++);
		fData[i].raCumul = 0;
	}
		
	RecalcCumulatives();
} /* CRunArray::Read */

bool CRunArray::operator==(const CRunArray& other)
{
	return fCount == other.fCount &&
		fMax == other.fMax &&
		memcmp(fData,other.fData,fCount*sizeof(RAElement)) == 0;
} // CRunArray::operator==
