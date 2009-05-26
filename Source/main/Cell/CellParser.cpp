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
	CellParser.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include <support/Debug.h>
#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   CELLCOMMANDS_H
#include "CellCommands.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   FORMULA_H
#include "Formula.h"
#endif

#ifndef   CELLPARSER_H
#include "CellParser.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   PARSER_H
#include "parser.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif

#include <stdarg.h>

#include <Autolock.h>

extern int yyparse(void *ioResult);

CParseErr::CParseErr(int offset, int len, int err, ...)
{
	fOffset = offset;
	fLen = len;
	fErr = err;
	
	const char *r;
	r = GetIndString(6, err < 0 ? 0 : err);
	if (!r)
		sprintf(fMessage, "Syntax error?");
	else
	{
		va_list vl;
		va_start(vl, err);
		vsprintf(fMessage, r, vl);
		va_end(vl);
	}
} /* CParseErr::CParseErr */

int TryToParseString(const char *inString, cell inLocation,
	CContainer *inContainer, bool inWarnIfError,
	char decSep, char listSep, char timeSep, char dateSep)
{
	int tResult = 1;
	BAutolock lock(inContainer);
	
	if (strlen(inString) > 0)
	{
		Value v;
		CParser p(inContainer,
			(listSep ? listSep : gListSeparator),
			(decSep ? decSep : gDecimalPoint),
			(dateSep ? dateSep : gDateSeparator),
			(timeSep ? timeSep : gTimeSeparator));
		
		try
		{
			if (!p.Parse(inString, inLocation))
			{
				v = inString;
				inContainer->NewCell(inLocation, v, NULL);
			}
			else if (p.Formula().ReduceToValue(v, inContainer))
			{
				inContainer->NewCell(inLocation, v, NULL);
				if (p.Formula()[0] == valPerc)
					tResult = 2;
			}
			else
			{
				if (p.Formula().IsConstant())
					p.Formula().Calculate(inLocation, v, inContainer);

				inContainer->NewCell(inLocation, v, p.Formula().CopyString());
			}
		}
		catch (CErr& e)
		{
			CATCHED;

			if (inWarnIfError)
				throw;

			v = inString;
			inContainer->NewCell(inLocation, v, NULL);
			tResult = false;
		}
	}
	else
	{
		inContainer->DisposeCell(inLocation);
	}
	
	return tResult;
} /* TryToParseString */

void ParseToValue(const char *inString, Value& v,
		char decSep, char listSep, char timeSep, char dateSep)
{
	cell c;
	
	// fprintf( stderr, "ParseToValue( \"%s\", %p , '%c' , '%c' , '%c' , '%c'\n",
	// 	inString, &v, decSep, listSep, timeSep, dateSep ) ;
	
	if (strlen(inString) > 0)
	{
		CParser p(NULL,
			(listSep ? listSep : gListSeparator),
			(decSep ? decSep : gDecimalPoint),
			(dateSep ? dateSep : gDateSeparator),
			(timeSep ? timeSep : gTimeSeparator));
		
		p.Parse(inString, c);
		if (!p.Formula().ReduceToValue(v, NULL) && p.Formula().IsConstant())
			p.Formula().Calculate(c, v, NULL);
	}
	else
		v.fType = eNoData;
} /* ParseToValue */
