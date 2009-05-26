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
	Set.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   SET_H
#include "Set.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   CONFIG_H
#include "Config.h"
#endif

#include <Debug.h>
#include <Beep.h>

CSet::CSet(int nrOfElems)
{
	fSize = nrOfElems / 8;
	fData = (char *)MALLOC(fSize * 8 + 1);
	FailNil(fData);

	int i;
	for (i = 0; i < fSize; i++)
		fData[i] = 0;
} /* CSet::CSet */

CSet::~CSet()
{
	FREE(fData);
}

bool CSet::operator[](int index) const
{
	if (!(index > 0 && index / 8 <= fSize))
		beep();
	int byte, bit;
	
	index--;
	byte = index >> 3;
	bit = index & 0x00000007;
	
	return (fData[byte] & 1 << bit) != 0;
} /* CSet::operator[] */

void CSet::operator += (int index)
{
	ASSERT(index > 0 && index / 8 <= fSize);
	int byte, bit;
	
	index--;
	byte = index >> 3;
	bit = index & 0x00000007;
	
	fData[byte] |= 1 << bit;
} /* CSet::operator+= */

void CSet::operator -= (int index)
{
	ASSERT(index > 0 && index / 8 <= fSize);
	int byte, bit;
	
	index--;
	byte = index >> 3;
	bit = index & 0x00000007;
	
	fData[byte] &= ~(1 << bit);
} /* CSet::operator-= */

void CSet::operator|= (const CSet& set)
{
	ASSERT(set.fSize == fSize);
	int i;
	for (i = 0; i < fSize; i++)
		fData[i] |= set.fData[i];
} /* CSet::operator|= */

void CSet::Clear()
{
	memset(fData, 0, fSize);
}
