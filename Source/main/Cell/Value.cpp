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
	Value.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELL_H
#include "Cell.h"
#endif

#ifndef   RANGE_H
#include "Range.h"
#endif

#ifndef   FORMULA_H
#include "Formula.h"
#endif

#ifndef   VALUE_H
#include "Value.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif


#include <cstdio>
#include <cstdlib>

Value::Value()
{
	fType = eNoData;
}

Value::Value(double d)
{
	fType = eNumData;
	fDouble = d;
}

Value::Value(time_t t)
{
	fType = eTimeData;
	fTime = t;
}

Value::Value(const char *inString, bool inCopy)
{
	fType = eTextData;
	if ((fTextIsCopy = inCopy) == true)
	{
		fText = STRDUP(inString);
		FailNil(fText);
	}
	else
		fText = const_cast<char *>(inString) ;
}

Value::Value(bool b)
{
	fType = eBoolData;
	fBool = b;
}

Value::Value(CellData& cd)
{
	*this = cd;
}

Value::~Value()
{
	if (fType == eTextData && fText && fTextIsCopy)
		FREE(fText);
}

void Value::operator+=(Value &v)
{
	if (fType == v.fType)
		switch (fType)
		{
			case eNumData:
				fDouble += v.fDouble;
				break;
			
			case eTextData:
			{
				char *t = fText;
				int tlen = strlen(fText) + strlen(v.fText);
				fText = (char *)MALLOC(tlen + 1);
				FailNil(fText);
				strcpy(fText, t);
				strcat(fText, v.fText);
				if (fTextIsCopy)
					FREE(t);
				fTextIsCopy = true;
				break;
			}
			case eNoData:
				break;
			case eTimeData:
				if (abs((long)v.fTime) < (24 * 60 * 60))
					fTime += v.fTime;
				else
					*this = gValueNan;
				break;
			default:
				*this = gValueNan;
		}
	else if (fType == eNoData)
		this->operator=(v);
	else if (fType == eTimeData && v.fType == eNumData)
		fTime += static_cast<time_t>( v.fDouble ) ;
	else if (v.fType != eNoData)
		*this = gValueNan;
}

void Value::operator-=(Value &v)
{
	if (fType == eNumData && v.fType == eNumData)
		fDouble -= v.fDouble;
	else if (fType == eNoData && v.fType == eNumData)
	{
		fType = eNumData;
		fDouble = -v.fDouble;
	}
	else if (fType == eTimeData && v.fType == eTimeData)
	{
		fType = eNumData;
		fDouble = fTime - v.fTime;
	}
	else if (fType == eTimeData && v.fType == eNumData)
	{
		fTime -= static_cast<time_t>( v.fDouble ) ;
	}
	else if (v.fType != eNoData)
		*this = gValueNan;
}

void Value::operator*=(Value &v)
{
	if (fType == eNumData && v.fType == eNumData)
		this->operator=(fDouble * v.fDouble);
	else if (fType == eNoData || v.fType == eNoData)
		*this = 0.0;
	else
		*this = gValueNan;
}

void Value::operator/=(Value &v)
{
	if (fType == eNumData && v.fType == eNumData)
	{
		if (v.fDouble != 0.0)
			fDouble /= v.fDouble;
		else
			*this = gDivNan;
	}
	else if (fType == eNoData && v.fType == eNumData)
		*this = 0.0;
	else if (fType == eNumData && v.fType == eNoData)
		*this = gDivNan;
	else
		*this = gValueNan;
}

void Value::operator^=(Value &v)
{
	if (fType == eNumData || v.fType == eNumData)
		this->operator=(pow(fDouble, v.fDouble));
	else
		*this = gValueNan;
}

void Value::operator&=(Value &v)
{
	if (fType == eBoolData && v.fType == eBoolData)
		fBool = fBool && v.fBool;
	else
		*this = gValueNan;
}

void Value::operator|=(Value &v)
{
	if (fType == eBoolData && v.fType == eBoolData)
		fBool = fBool || v.fBool;
	else
		*this = gValueNan;
}

//Value& Value::Error(int err)
//{
//	Value result(Nan(err));
//	return result;
//}

static const char kCompareTypes[7][7] = 
{
	{ 0, 1, 1, 1, 1, 1, 1 },
	{ -1, 0, -1, -1, -1, -1, -1 },
	{ -1, 1, 0, -1, -1, -1, -1 },
	{ -1, 1, 1, 0, -1, -1, -1 },
	{ -1, 1, 1, 1, 0, -1, -1 },
	{ -1, 1, 1, 1, 1, 0, -1 },
	{ -1, 1, 1, 1, 1, 1, 0 }
};

bool Value::operator<(Value &v)
{
	if (fType == eNumData && v.fType == eNumData)
		return fDouble < v.fDouble;
	else if (fType == eTextData && v.fType == eTextData)
		return strcasecmp(fText, v.fText) < 0;
	else if (fType == eTimeData && v.fType == eTimeData)
		return fTime < v.fTime;
	else if (fType == eBoolData && v.fType == eBoolData)
		return fBool < v.fBool;
	else
		return kCompareTypes[fType][v.fType] < 0;
}

bool Value::operator<=(Value &v)
{
	if (fType == eNumData && v.fType == eNumData)
		return fDouble <= v.fDouble;
	else if (fType == eTextData && v.fType == eTextData)
		return strcasecmp(fText, v.fText) <= 0;
	else if (fType == eTimeData && v.fType == eTimeData)
		return fTime <= v.fTime;
	else if (fType == eBoolData && v.fType == eBoolData)
		return fBool <= v.fBool;
	else
		return kCompareTypes[fType][v.fType] <= 0;
}

bool Value::operator>(Value &v)
{
	if (fType == eNumData && v.fType == eNumData)
		return fDouble > v.fDouble;
	else if (fType == eTextData && v.fType == eTextData)
		return strcasecmp(fText, v.fText) > 0;
	else if (fType == eTimeData && v.fType == eTimeData)
		return fTime > v.fTime;
	else if (fType == eBoolData && v.fType == eBoolData)
		return fBool > v.fBool;
	else
		return kCompareTypes[fType][v.fType] > 0;
}

bool Value::operator>=(Value &v)
{
	if (fType == eNumData && v.fType == eNumData)
		return fDouble >= v.fDouble;
	else if (fType == eTextData && v.fType == eTextData)
		return strcasecmp(fText, v.fText) >= 0;
	else if (fType == eTimeData && v.fType == eTimeData)
		return fTime >= v.fTime;
	else if (fType == eBoolData && v.fType == eBoolData)
		return fBool >= v.fBool;
	else
		return kCompareTypes[fType][v.fType] >= 0;
}

bool Value::operator==(Value &v)
{
	if (fType == eNumData && v.fType == eNumData)
		return fDouble == v.fDouble;
	else if (fType == eTextData && v.fType == eTextData)
		return strcasecmp(fText, v.fText) == 0;
	else if (fType == eTimeData && v.fType == eTimeData)
		return fTime == v.fTime;
	else if (fType == eBoolData && v.fType == eBoolData)
		return fBool == v.fBool;
	else
		return kCompareTypes[fType][v.fType] == 0;
}

bool Value::operator!=(Value &v)
{
	if (fType == eNumData && v.fType == eNumData)
		return fDouble != v.fDouble;
	else if (fType == eTextData && v.fType == eTextData)
		return strcasecmp(fText, v.fText) != 0;
	else if (fType == eTimeData && v.fType == eTimeData)
		return fTime != v.fTime;
	else if (fType == eBoolData && v.fType == eBoolData)
		return fBool != v.fBool;
	else
		return kCompareTypes[fType][v.fType] != 0;
}

void Value::operator=(const char *inString)
{
	Clear();
	fType = eTextData;
	fText = STRDUP(inString);
	fTextIsCopy = true;
}

void Value::operator=(const Value &inValue)
{
	Clear();
	if ((fType = inValue.fType) == eTextData)
	{
		fText = STRDUP(inValue.fText);
		fTextIsCopy = true;
	}
	else
		fDouble = inValue.fDouble;
}

void Value::operator=(const double d)
{
	Clear();
	fType = eNumData;
	fDouble = d;
}

void Value::operator=(const bool b)
{
	Clear();
	fType = eBoolData;
	fBool = b;
}

void Value::operator=(const time_t& t)
{
	Clear();
	fType = eTimeData;
	fTime = t;
}

void Value::operator=(const range& r)
{
	Clear();
	fType = eRangeData;
	fRange = r;
}

void Value::Clear()
{
	if (fType == eTextData && fText && fTextIsCopy)
	{
		FREE(fText);
		fText = NULL;
	}
	fTextIsCopy = false;
}
