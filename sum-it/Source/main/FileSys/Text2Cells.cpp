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
	Text2Cells.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.
	
	Converteer tekst in een handle naar cel inhoud en vice versa

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

/* Includes */
#ifndef   RANGE_H
#include "Range.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   TEXT2CELLS_H
#include "Text2Cells.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLITERATOR_H
#include "CellIterator.h"
#endif

#ifndef   CELLPARSER_H
#include "CellParser.h"
#endif

#ifndef   PROGRESSVIEW_H
#include "ProgressView.h"
#endif

#ifndef   PARSER_H
#include "parser.h"
#endif

#ifndef   STLOCKER_H
#include "StLocker.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif


#include <DataIO.h>

/* Defines */
#define kEndIndicator	0x00
#define kEndState		-1

/* Implementatie */

CTextConverter::CTextConverter(BPositionIO& text, CContainer *container)
	: fText (text)
{
	fContainer = container;
	fQuoteTextFields = false;
	fFieldSeparator = '\t';
#if __BEOS__
	fRecordSeparator = '\n';
#else
	fRecordSeparator = '\r';
#endif
} /* CTextConverter */

int CTextConverter::GetNextChar()
{
	char c, d;
	
#if __BEOS__
	if (fText.Read(&c, 1) != 1)
		c = kEndIndicator;
	
	if (c == '\r')
	{
		int read = fText.Read(&d, 1);

		if (d != '\n' && read == 1)
			Retract();
	}

#else
	if (fText.AtEnd())
		c = kEndIndicator;
	else
	{
		fText >> c;
		
		if (c == '\r' && !fText.AtEnd () )
		{
			fText >> d;

			if (d != '\n')
				Retract();
		}
	}
#endif
		
	if (c == '\n' || c == '\r')
		c = fRecordSeparator;
	
	return c;
} /* GetNextChar */

void CTextConverter::Retract()
{
#if __BEOS__
	fText.Seek(-1, SEEK_CUR);
#else
	fText.SetMarker(-1, streamFrom_Marker);
#endif
} /* Retract() */

bool CTextConverter::GetNextField (char *s)
{
	int state = 1, result = 0;
	char ch, *sp = s;

	s[0] = 0;

	while (state != kEndState && sp < (s + 255))
	{
		switch (state)
		{
			case 1:
				ch = GetNextChar();
				if (ch == fFieldSeparator)
				{
					if (++fCol > fMaxRight)
						fMaxRight = fCol;
				}
				else if (ch == fRecordSeparator)
				{
					fRow++;
					fCol = 1;
				}
				else if (ch == kEndIndicator)
					state = kEndState;
				else
					state = 2;
				break;

			case 2:
				result = true;
				if (ch == '"')
					state = 5;
				else
				{
					*sp++ = ch;
					state = 3;
				}
				break;

			case 3:
				ch = GetNextChar();
				if (ch == fRecordSeparator || ch == fFieldSeparator)
					state = 4;
				else if (ch == kEndIndicator)
					state = kEndState;
				else if (ch == '"')
					state = 5;
				else
					*sp++ = ch;
				break;

			case 4:
				Retract();
				state = kEndState;
				break;

			case 5:
				ch = GetNextChar();
				if (ch == '"')
					state = 3;
				else if (ch == kEndIndicator)
					state = kEndState;
				else
					*sp++ = ch;
				break;

			default:
				THROW((errConvertingText));
				break;
		}
	}
	
	*sp = 0;
	
	return result;
}

void CTextConverter::ConvertFromText(range& range, CCellView *inView,
	bool inUseDef)
{
	cell c;
	StProgress *progress = NULL;
	char s[512];
	int row = 0;
	
	fCol = 1;
	fRow = 1;
	fMaxRight = 1;
	
	char s1, s2, s3, s4;
	if (inUseDef)
	{
		s1 = '.'; s2 = ','; s3 = '/'; s4 = ':';
	}
	else
	{
		s1 = gDecimalPoint; s2 = gListSeparator;
		s3 = gDateSeparator; s4 = gTimeSeparator;
	}

	try
	{
		StWriteLock lock(fContainer);
		size_t size;
		
#if __BEOS__
		fText.Seek(0, SEEK_END);
		size = fText.Position();
		fText.Seek(0, SEEK_SET);
#else
		size = fText.GetLength();
#endif
		
		if (inView)
			progress = new StProgress(inView, size, pColorYellow, false);
		
		CParser p(fContainer, s2, s1, s3, s4);
	
		Value v;
		
		while (GetNextField(s))
		{
			if (fCol > kColCount)
				if (fRow > kRowCount)
					break;
				else
					continue;
	
			c.h = fCol;
			c.v = fRow;
			
			if (inView && row != fRow)
			{
				row = fRow;
				inView->TouchLine(row);
			}
			
			const char *t;
			int result = p.ScanFirstToken(s, &t, c);
			if (*t) result = -1;
			
			Value v;
			
			switch (result)
			{
				case NUMBER:	v = (double)p; break;
				case BOOL:		v = (bool)p; break;
				case TIME:		v = (time_t)p; break;
				default:		v = s;
			}

			try
			{
				fContainer->SetValue(c, v);
			}
			catch (CErr& e)
			{
				CATCHED;
				throw;
			}

			if (progress)
#if __BEOS__
				progress->Step(fText.Position());
#else
				progress->Step(fText.GetMarker());
#endif
		}
		
		if (progress) delete progress;
	}
	catch(...)
	{
		CATCHED;
		if (progress) delete progress;
	}

	if (inView)
		inView->DrawAllLines();

	range.Set(1, 1, min(fMaxRight, (int)kColCount), min(fRow, (int)kRowCount));
} /* Text2Cells */

void CTextConverter::ConvertToText(range *sRange, CCellView *inView)
{
	cell c, prevLoc;
	char s[256];
	char buffer[400], *bp, *sp;
	range r;
	
	if (!sRange)
	{
		fContainer->GetBounds(r);
		sRange = &r;
	}
	
	prevLoc = sRange->TopLeft();
	
	CCellIterator iter(fContainer, sRange);
	while (iter.NextExisting(c))
	{
		Value v;
		
		bp = buffer;
		fContainer->Lock();
		fContainer->GetValue(c, v);
		fContainer->GetCellResult(c, s, true);
		fContainer->Unlock();

		if (prevLoc.v < c.v)
		{
			while (prevLoc.v++ < c.v)
				*bp++ = fRecordSeparator;
			prevLoc.h = sRange->left;
		}

		if (prevLoc.h < c.h)
		{
			while (prevLoc.h++ < c.h)
				*bp++ = fFieldSeparator;
		}
		
		if (fQuoteTextFields)
		{
			if (v.fType == eTextData)
				*bp++ = '"';
		}
		
		sp = s;
		while (*sp)
			*bp++ = *sp++;

		if (fQuoteTextFields && v.fType == eTextData)
			*bp++ = '"';
		
#if __BEOS__
		CHECKWRITE(fText, buffer, bp - buffer);
#else
		fText.WriteBlock(buffer, bp - buffer);
#endif
		prevLoc = c;
	}
} /* Cells2Text */

bool DataIsText(char *data, unsigned long size)
{
	// stolen from Jon Watte's datatype lib texthandler example
	while (size-- > 0) {
		int ch = *(unsigned char *)(data++);
//		if ((ch < 9) || (ch == 11) || ((ch > 13) && (ch < 32)))
		if ((ch < 32) && (ch != '\n') && (ch != '\r'))
			return FALSE;
	}
	return TRUE;
} // DataIsText
