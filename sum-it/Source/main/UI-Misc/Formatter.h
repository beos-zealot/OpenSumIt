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
	Formatter.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef FORMATTER_H
#define FORMATTER_H

class CFormatTable;

#ifndef   VALUE_H
#include "Value.h"
#endif

#include <map>

#include <Locker.h>

enum ENumberFormat {
	eGeneral,
	eCurrency,
	eScientific,
	eFixed,
	ePercent,
	eFormula,	// unused as of 1.1
	
		// commas << 9 | digits << 4 | format
	eFirstNewFormat = 0x0400	// 1024
};

enum DateOrder {
	doDMY,
	doMDY,
	doDYM,
	doMYD,
	doYMD,
	doYDM
};

extern DateOrder gDateOrder;

class CFormatter {
public:
	CFormatter();
	CFormatter(const char *inTemplate);
	CFormatter(int inOldID, int inDigits, bool inCommas);
	CFormatter(int inCodedID);
	
	float FormatValue(const Value& inValue, char *outString,
		int inFontSize = 0, float width = 1e6);

	int FormatID() const
		{ return fFormatID >= eFirstNewFormat ? fFormatID :
			fFormatID | (fDigits << 4) | (fCommas << 9); };
	
	int OldFormatID() const
		{ return fFormatID & 0x000F; }
	int Digits() const
		{ return fDigits; };
	bool Commas() const
		{ return fCommas; };
	
	bool operator== (const CFormatter& inOther) const;

private:
	void ParseTemplate(const char *inTemplate);
	float FormatDouble(double inValue, char *outString,
		int inFontSize, float inWidth);
	float FormatDate(const time_t inValue, char *outString,
		int inFontSize, float inWidth);

	int fFormatID;
	int fDigits;
	bool fCommas;
};

class CFormatTable : public BLocker {
public:
	CFormatTable();
	
	int GetFormatID(const CFormatter& inFormat);
	int GetFormatID(const char *inTemplate);

	float FormatValue(int inFormatID, const Value& inValue,
		char *outString, int inFontNr = 0,
		float inWidth = 1e6);
	
	const CFormatter& operator[](int indx);
	
	unsigned long Count() const
		{ return fFormats.size() + eFirstNewFormat; };

private:
	std::map<int,CFormatter> fFormats;
};

void InitFormatter(void);

_EXPORT float ftoa(double inValue, char *s); // versimpelde variant

extern CFormatTable gFormatTable;
extern char gTrueString[], gFalseString[];

#endif
