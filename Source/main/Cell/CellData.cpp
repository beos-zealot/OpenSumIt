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
	CellData.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "MyError.h"
#include "Formula.h"
#include "CellData.h"
#include "Value.h" 
#include "Utils.h"

#include <cstring>
#include <cstdlib>

/*
	union
	{
		double	mDouble;
		char*	mText;
		bool	mBool;
		time_t	mTime;
	};
	void*				mFormula;
	unsigned short		mStyle;
	unsigned char		mStatus;
	unsigned char		mConstant	: 1;
	unsigned char		mType		: 7;
*/
CellData::CellData( )
:	mFormula(NULL)
,	mStyle(0)
,	mStatus(0)
,	mConstant(0)
,	mType(eNoData)
{	
	memset( &mDouble, sizeof(mDouble), 0 ) ;
};

CellData::CellData(const CellData& d)
{
	mDouble = d.mDouble;
	mConstant = d.mConstant;
	mStatus = d.mStatus;
	mType = d.mType;
	mStyle = d.mStyle;
	mFormula = d.mFormula;
} /* CellData::CellData */

const CellData& CellData::operator=(const CellData& d)
{
//	double *a = &mDouble;
//	const double *b = &d.mDouble;
//	*a++ = *b++;
//	*a++ = *b++;
	mDouble = d.mDouble;
	mConstant = d.mConstant;
	mStatus = d.mStatus;
	mType = d.mType;
	mStyle = d.mStyle;
	mFormula = d.mFormula;
	return *this;
} /* CellData::operator= */

void CellData::Clear()
{
	if (mType == eTextData && mText)
		FREE(mText);
	if (mFormula)
		FREE(mFormula);
} /* CellData::Clear */

void CellData::operator=(const Value& inValue)
{
	if (mType == eTextData && mText)
		FREE(mText);

	mType = inValue.fType;

	if (mType == eTextData)
	{
		mText = STRDUP(inValue.fText);
		FailNil(mText);
	}
	else
		mDouble = inValue.fDouble;
} /* CellData::operator= */

void CellData::Copy()
{
	if (mType == eTextData && mText)
	{
		mText = STRDUP(mText);
		FailNil(mText);
	}
	
	if (mFormula)
	{
		CFormula form(mFormula);
		mFormula = form.CopyString();
	}
} /* CellData::Copy */

//
//	Value routine die hier pas gedefinieerd kan worden
//	omdat CellData niet bekend is in Value.h
//
void Value::operator=(const CellData &cd)
{
	switch (cd.mType)
	{
		case eTextData:	*this = cd.mText; break;
		case eNumData:	*this = cd.mDouble; break;
		case eBoolData:	*this = cd.mBool; break;
		case eTimeData:	*this = cd.mTime; break;
		default: fType = eNoData;
	}
} /* Value::operator= */
