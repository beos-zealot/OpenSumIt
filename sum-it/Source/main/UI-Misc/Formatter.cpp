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
	Formatter.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   BTREE_T
#include "BTree.t"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif


#include <support/Debug.h>
#include <locale.h>

#include <Autolock.h>

CFormatTable gFormatTable;

static int32 gNextFormatNr = 1024;

char gTrueString[32];
char gFalseString[32];

char gCurrencySymbol[32];
bool gCurrencyBefore;
bool gCurrencyParens;
int gCurrencyDigits;

void InitFormatter(void)
{
	struct lconv *locales = localeconv();
	
	if (gPrefs->GetPrefInt("use locales", 0))
	{
		gDecimalPoint = locales->decimal_point[0];
		gThousandSeparator = locales->thousands_sep[0];
		if (gDecimalPoint == '.')
			gListSeparator = ',';
		else
			gListSeparator = ';';
	}
	else
	{
		gDecimalPoint = gPrefs->GetPrefInt("decimal point", '.');
		gThousandSeparator = gPrefs->GetPrefInt("thousands separator", ',');
		gListSeparator = gPrefs->GetPrefInt("list separator", ',');
	}
		
	gTimeSeparator = gPrefs->GetPrefInt("time separator", ':');
	gDateSeparator = gPrefs->GetPrefInt("date separator", '/');
	
	char dfm[4];
	strncpy(dfm, gPrefs->GetPrefString("date order", "mdy"), 3);
	dfm[3] = 0;
	if (strcasecmp(dfm, "dmy") == 0)
		gDateOrder = doDMY;
	else if (strcasecmp(dfm, "dym") == 0)
		gDateOrder = doDYM;
	else if (strcasecmp(dfm, "mdy") == 0)
		gDateOrder = doMDY;
	else if (strcasecmp(dfm, "myd") == 0)
		gDateOrder = doMYD;
	else if (strcasecmp(dfm, "ydm") == 0)
		gDateOrder = doYDM;
	else if (strcasecmp(dfm, "ymd") == 0)
		gDateOrder = doYMD;
	else
		gDateOrder = doMDY;
	
	g24Hours = gPrefs->GetPrefInt("24 hours", true);
	
	strcpy(gCurrencySymbol, gPrefs->GetPrefString("c_symbol", "$"));
	gCurrencyBefore = gPrefs->GetPrefInt("c_before", 1);
	gCurrencyParens = gPrefs->GetPrefInt("c_neg_par", 1);
	gCurrencyDigits = gPrefs->GetPrefInt("c_digits", 2);
	
	getindstring(gTrueString, kFormattingStrings, kTrueIndex);
	getindstring(gFalseString, kFormattingStrings, kFalseIndex);
} /* InitFormatter */

CFormatTable::CFormatTable()
{
} // CFormatTable::CFormatTable

int CFormatTable::GetFormatID(const CFormatter& nf)
{
	BAutolock lock(this);

	int id = nf.FormatID();
	if (id < eFirstNewFormat)
		return id;
	
	if (fFormats.find(id) != fFormats.end())
		return id;

	fFormats[id] = nf;
	return id;
} // CFormatTable::GetNrFormatID

int CFormatTable::GetFormatID(const char *inTemplate)
{
	CFormatter nf(inTemplate);
	return GetFormatID(nf);
} // CFormatTable::GetFormatID

float CFormatTable::FormatValue(int inFormatID, const Value& inValue,
	char *outString, int inFontNr, float inWidth)
{
	if (inFormatID >= eFirstNewFormat)
	{
		CFormatter nf = fFormats[inFormatID];
		return nf.FormatValue(inValue, outString, inFontNr, inWidth);
	}
	else
	{
		CFormatter nf(inFormatID);
		return nf.FormatValue(inValue, outString, inFontNr, inWidth);
	}
} // CFormatTable::FormatValue

const CFormatter& CFormatTable::operator[](int indx)
{
	if (indx < eFirstNewFormat)
	{
		static CFormatter result;
		result = CFormatter(indx);
		return result;
	}
	else
	{
		BAutolock lock(this);
		return fFormats[indx];
	}
} /* CFormattable::operator[] */

CFormatter::CFormatter()
{
	fFormatID = fCommas = 0;
	fDigits = 2;
} // CFormatter::CFormatter

CFormatter::CFormatter(int inOldID, int inDigits, bool inCommas)
{
	fFormatID = inOldID;
	fDigits = inDigits;
	fCommas = inCommas;
} // CFormatter::CFormatter

CFormatter::CFormatter(int inCodedID)
{
	if (inCodedID < eFirstNewFormat)
	{
		fFormatID = inCodedID & 0x000F;
		fDigits = (inCodedID >> 4) & 0x001F;
		fCommas = inCodedID >> 9;
	}
	else
		fFormatID = -1;
} // CFormatter::CFormatter

CFormatter::CFormatter(const char *inTemplate)
{
	fFormatID = atomic_add(&gNextFormatNr, 1);
	ParseTemplate(inTemplate);
} // CFormatter::CFormatter

float CFormatter::FormatValue(const Value& inValue, char *outString,
	int inFontNr, float inWidth)
{
	float result = -1;
	
	switch (inValue.fType)
	{
		case eNoData:
			outString[0] = 0;
			result = 0;
			break;
		case eTextData:
			strcpy(outString, inValue.fText);
			if (outString[0] == '\'')
			{
				int sLen = strlen(outString);
				memmove(outString, outString + 1, sLen);
				sLen--;
				if (outString[sLen - 1] == '\'')
					outString[sLen - 1] = 0;
			}
			else if (outString[0] == '"')
			{
				int sLen = strlen(outString);
				memmove(outString, outString + 1, sLen);
				sLen--;
				if (outString[sLen - 1] == '"')
					outString[sLen - 1] = 0;
			}
			break;
		case eNumData:
			result = FormatDouble(inValue.fDouble, outString, inFontNr, inWidth);
			break;
		case eBoolData:
			if (inValue.fBool)
				strcpy(outString, gTrueString);
			else
				strcpy(outString, gFalseString);
			break;
		case eTimeData:
			result = FormatDate(inValue.fTime, outString, inFontNr, inWidth);
			break;
		default:
			ASSERT(false);
			break;
	}
	
	return result;
} // CFormatter::FormatValue

// rounds at 6 digits
float ftoa(double inValue, char *s)
{
	float w;
	w = gFontSizeTable[0].Font().StringWidth("0.000000");
	
	CFormatter nf(eGeneral);
	return nf.FormatValue(Value(inValue), s, 0, w);
} // ftoa

 
