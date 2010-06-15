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
	Value.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef VALUE_H
#define VALUE_H

#ifndef   CELL_H
#include "Cell.h"
#endif

#ifndef   RANGE_H
#include "Range.h"
#endif

#include <cmath>
#include <ctime>

struct CellData;

extern double gValueNan;

enum ValueType {
	eNoData,
	eBoolData,
	eNumData,
	eTimeData,
	eUnused,			// was ooit datedata
	eTextData,
	eRangeData			// Niet echt een mogelijk type, maar wel handig bij berekeningen.
};

struct Value {
	Value();
	Value(double d);
	Value(const char *s, bool inCopy = true);
	Value(bool b);
	Value(time_t t);
	Value(CellData& cd);
	~Value();

	void operator+=(Value &);
	void operator-=(Value &);
	void operator*=(Value &);
	void operator/=(Value &);
	void operator^=(Value &);
	void operator&=(Value &);
	void operator|=(Value &);
	
	bool operator<(Value &v);
	bool operator<=(Value &v);
	bool operator>(Value &v);
	bool operator>=(Value &v);
	bool operator==(Value &v);
	bool operator!=(Value &v);

	void operator=(const Value &inValue);
	void operator=(const double d);
	void operator=(const char *inString);
	void operator=(const bool b);
	void operator=(const range& r);
	void operator=(const time_t& t);
	
	inline operator double() const;
	inline operator const char *() const;
	inline operator bool() const;
	inline operator range() const;
	inline operator time_t() const;
	
	inline bool IsNan();
	
	void operator=(const CellData&);

//	static Value& Error(int errno);

	void Clear();

	union {
		double		fDouble;
		char*			fText;
		bool			fBool;
		time_t		fTime;
		_range		fRange;
	};
	ValueType 	fType;
	bool 		fTextIsCopy;
//	ValueType 	fType			: 8;
//	bool 		fTextIsCopy		: 8;
};

/*
	Inlines:
*/

inline Value::operator double() const
{
	if (fType == eNumData)
		return fDouble;
	else
		return gValueNan;
}

inline Value::operator const char *() const
{
	if (fType == eTextData)
		return fText;
	else
		return "Error";
}

inline Value::operator bool() const
{
	if (fType == eBoolData)
		return fBool;
	else
		return false;
}

inline Value::operator time_t() const
{
	if (fType == eTimeData)
		return fTime;
	else
		return 0;
}

inline Value::operator range() const
{
	if (fType == eRangeData)
		return fRange;
	else
		return range();
}

inline bool Value::IsNan()
{
	return (fType == eNumData && isnan(fDouble));
}

#endif
