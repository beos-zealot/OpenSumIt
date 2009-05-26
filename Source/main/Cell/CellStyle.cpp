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
	CellStyle.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELLSTYLE_H
#include "CellStyle.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif


#include <Autolock.h>

CStyleTable gStyleTable;
BLocker gStyleTableLock( "style_table_lock", true ) ;


CellStyle::CellStyle()
{
	memset(this, 0, sizeof(CellStyle));
	fFormat = CFormatter(eGeneral, 2, false).OldFormatID();
	fLowColor.red = fLowColor.green = fLowColor.blue = 255;
	fLowColor.alpha = 255;
} // CellStyle::CellStyle

bool CellStyle::operator==(const CellStyle& cs) const
{
	int result = memcmp(this, &cs, sizeof(CellStyle));
	return result == 0;
} // CellStyle::operator==

CStyleTable::CStyleTable()
{
	fNextStyleID = 0;
	
	CellStyle cs;
	GetStyleID(cs);
} // CStyleTable::CStyleTable

int CStyleTable::GetStyleID(const CellStyle& cs)
{
	map<int,CellStyle>::iterator mi;
	
	for (mi = fStyles.begin(); mi != fStyles.end(); mi++)
	{
		if ((*mi).second == cs)
			return (*mi).first;
	}
	
	fStyles[fNextStyleID] = cs;
	return fNextStyleID++;
} // CStyleTable::GetStyleID

const CellStyle& CStyleTable::operator[](int indx)
{
	BAutolock lock(gStyleTableLock);
	try
	{
		return fStyles[indx];
	}
	catch (...)
	{
//
	}
	static CellStyle defstyle;
	return defstyle;
} /* CStyleTable::operator[] */
