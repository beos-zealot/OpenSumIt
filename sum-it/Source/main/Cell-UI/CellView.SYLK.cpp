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
	SylkIO.cp
	
	
	
*/

#include "CellWindow.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "RunArray.h"
#include "CellView.h"
#include "CellStyle.h"
#include "errmsg.h"
#include "Formatter.h"
#include "CellParser.h"
#include "Container.h"
#include "CellIterator.h"
#include "StringTable.h"
#include "FileFormat.h"
#include <MAlert.h>

const int
	errUserAbort = -1;

class CSylkParser
{
  public:
	CSylkParser(BPositionIO& inStream, CCellView& inView)
		: fCellView(inView), fStream(inStream)
	{
		fShowError = true;
		fLineNr = 0;
		fColNr = fRowNr = 0;
		fCellView.GetContainer()->GetDefaultCellStyle(fStyle);
		fStreamLength = fStream.Seek(0, SEEK_END);
		fStream.Seek(0, SEEK_SET);
	}

	int fLineNr, fColNr, fRowNr;
	bool fShowError;
	CCellView& fCellView;
	BPositionIO& fStream;
	long fStreamLength;
	CellStyle fStyle;

	void SParseError(cell c);
	void SParseFields(char *s);
	void SParseCellFields(char *s);
	void SParseFormatFields(char *s);
	void SParseNames(char *s);

 	int SReadLine(char *out);
};

void CSylkParser::SParseError(cell c)
{
	if (fShowError)
	{
		char s[12], m[256], n[256];
		
		if (c.h > 26)
		{
			c.h--;
			s[0] = c.h / 26 + '@';
			s[1] = c.h % 26 + 'A';
			sprintf(s+2, "%d", c.v);
		}
		else
		{
			s[0] = c.h + '@';
			sprintf(s+1, "%d", c.v);
		}
		
		getindstring(n, 6, errSylkImportErr);
		sprintf(m, n, s);

		MWarningAlert a(m, GetIndString(6, msgGoOn),
			GetIndString(6, msgStop), GetIndString(6, msgShutUp));
		
		switch (a.Go())
		{
			case 1:
				break;
			case 2:
				throw (int)errUserAbort;
				break;
			case 3:
				fShowError = false;
				break;					
		}
	}
}; /* ParseError */

void CSylkParser::SParseFormatFields(char *s)
{
	char *ptr = s;
	CFormatter fm = gFormatTable[fStyle.fFormat];
	int oldStyle = fm.OldFormatID();
	int digits;
	bool commas = fm.Commas();

	while (*ptr)
	{
		switch (*ptr++)
		{
			case 'X':
				fColNr = strtoul(ptr, &ptr, 10);
				break;
			case 'Y':
				fRowNr = strtoul(ptr, &ptr, 10);
				break;
			case 'F':
				switch (*ptr++)
				{
					case 'D':
					case 'G':	oldStyle = eGeneral; break;
					case '$':	oldStyle = eCurrency; digits = 2; break;
					case 'E':	oldStyle = eScientific; break;
					case 'F':	oldStyle = eFixed; digits = 2; break;
					case '%':	oldStyle = ePercent; break;
					default:	break;
				}

				while (isspace(*ptr)) ptr++;

				char *t;
				digits = strtoul(ptr, &t, 10);
				if (t) ptr = t;
				
				while (isspace(*ptr)) ptr++;

				switch (*ptr++)
				{
					case 'C':	fStyle.fAlignment = eAlignGeneral; break;
					case 'L':	fStyle.fAlignment = eAlignLeft; break;
					case 'R':	fStyle.fAlignment = eAlignRight; break;
					default:	fStyle.fAlignment = eAlignGeneral; break;
				}
				break;
// 			case 'D':
// 				switch (*ptr++)
// 				{
// 					case 'D':
// 					case 'G':	oldStyle = eGeneral; break;
// 					case '$':	oldStyle = eCurrency; break;
// 					case 'E':	oldStyle = eScientific; break;
// 					case 'F':	oldStyle = eFixed; break;
// 					case '%':	oldStyle = ePercent; break;
// 					default:	break;
// 				}
// 				if (*ptr == 'F')
// 				{
// 					;// iets te doen?
// 					ptr++;
// 				}
// 				else
// 					style.format |= (strtoul(ptr, &ptr, 10) & 0x1F) << 4;
// 				switch (*ptr++)
// 				{
// 					case 'C':	style.fAlignment = kscAlignGeneral; break;
// 					case 'L':	style.fAlignment = kscAlignLeft; break;
// 					case 'R':	style.fAlignment = kscAlignRight; break;
// 					default:	style.fAlignment = kscAlignGeneral; break;
// 				}
// 				fCellView->GetContainer()->SetDefaultCellStyle(style);
// 				break;
			case 'W':
			{
				long a, b, w;
				a = strtoul(ptr, &ptr, 10);
				if (ptr) b = strtoul(ptr, &ptr, 10);
				if (ptr) w = strtoul(ptr, &ptr, 10);
				if (ptr && a && b)
					fCellView.SetSylkColumnWidths(a, b, w);
				break;
			}
			case 'K':
				commas = true;
				break;
			default:
				while (*ptr && *ptr != ';')
					ptr++;
				break;
		}
		if (*ptr == ';')
			ptr++;
	}
	
	fStyle.fFormat = gFormatTable.GetFormatID(CFormatter(oldStyle, digits, commas));
}; /* ParseFormatFields */

void CSylkParser::SParseCellFields(char *s)
{
	StWriteLock lock(fCellView.GetContainer());
	
	char *ptr = s, *cexp = 0, *cval = 0;
	bool quote = false;
	cell c;
	
	while (*ptr)
	{
		switch (*ptr++)
		{
			case 'X':
				fColNr = strtoul(ptr, &ptr, 10);
				break;
			case 'Y':
				fRowNr = strtoul(ptr, &ptr, 10);
				break;
			case 'K':
				cval = ptr;
				while (*ptr && (*ptr != ';' || quote))
					if (*ptr++ == '"') quote = !quote;
				break;
			case 'E':
				cexp = ptr;
				while (*ptr && (*ptr != ';' || quote))
					if (*ptr++ == '"') quote = !quote;
				break;
			default:
				while (*ptr && (*ptr != ';' || quote))
					if (*ptr++ == '"') quote = !quote;
				break;
		}
		if (*ptr == ';')
			*ptr++ = 0;
	}

	*ptr = 0;

	Value v;
	c.Set(fColNr, fRowNr);
	
	if (cval)
	{
		if (*cval == '"')
		{
			cval++;
			char *t = strrchr(cval, '"');
			if (t) *t = 0;
			v = cval;
		}
		else
		{
			try
				{ ParseToValue(cval, v, '.', ',', ':', '/'); }
			catch (...)
				{ v = cval; }
		}
	}
	
	if (cexp)
	{
		try
			{ TryToParseString(cexp, c, fCellView.GetContainer(), true,
				'.', ',', ':', '/'); }
		catch (...)
		{
			SParseError(c);
			v = cexp;
			fCellView.GetContainer()->NewCell(c, v, NULL);
		}
	}
	else
		fCellView.GetContainer()->NewCell(c, v, NULL);

	fCellView.GetContainer()->SetCellStyle(c, fStyle);
}; /* ParseCellFields */

void CSylkParser::SParseNames(char *s)
{
	char name[32];
	name[0] = 0;
	
	if (sscanf(s, "N%31[^;]s;", name) == 1)
		s += 2 + strlen(name);
	
	if (*name && *s == 'E')
	{
		unsigned int l, t, r, b;
		sscanf(s, "ER%uC%u:R%uC%u", &t, &l, &b, &r);
		
		range rng(l, t, r, b);
		fCellView.AddNamedRange(name, rng);
	}
}; /* ParseNames */

void CSylkParser::SParseFields(char *s)
{
	char *ptr = s;
	
	switch (*ptr++)
	{
		case 'I':
			if (*ptr++ != 'D') throw (int)errSylkErr;
			break;
		case 'P':
			break;
		case 'F':
			if (*ptr++ != ';') throw (int)errSylkErr;
			SParseFormatFields(ptr);
			break;
		case 'O':
			break;
		case 'C':
			if (*ptr++ != ';') throw (int)errSylkErr;
			SParseCellFields(ptr);
			break;
		case 'B':
			break;
		case 'N':
			switch (*ptr++)
			{
				case 'N':
					if (*ptr++ != ';') throw (int)errSylkErr;
					SParseNames(ptr);
					break;
				case 'E':
					break;
				case 'V':
					break;
				default:
					throw (int)errSylkErr;
			}
			break;
		case 'E':
			break;
		case 'W':
			break;
		default:
			throw (int)errSylkErr;
	}
}; /* ParseFields */

int CSylkParser::SReadLine(char *out)
{
	char *p = out, *m = p + 511;
	*p = 0;
	
	while (fStream.Position() < fStreamLength && p < m)
	{
		fStream.Read(p, 1);

		if (*p == '\r')
		{
			(void)fStream.Read(p, 1);
			if (*p != '\n')
				fStream.Seek(-1, SEEK_CUR);
			break;
		}
		else if (*p == '\n')
			break;
		else
			p++;
	}
	
	*p = 0;
	fLineNr++;

	return strlen(out);
} /* CSylkParser::SReadLine */

int stop = 0;

void CCellView::ReadSylk (BPositionIO& in)
{
	char s[512];
	CSylkParser p(in, *this);
	
	while (p.SReadLine(s))
	{
		try
		{
			if (p.fLineNr == 1593) stop = 1;
			p.SParseFields(s);
		}
//		catch (int e)
//		{
//			if (e == errSylkErr)
//			{
//				MWarningAlert a(GetIndString(6, errSylkErr), GetIndString(6, msgStop), GetIndString(6, msgGoOn));
//				switch (a.Go())
//				{
//					case 1:	THROW((B_OK));
//					case 2:	break;
//				}
//			}	
//			else
//				break;
//		}
		catch (...)
		{
			break;
		}
	}
} /* ReadSylkFile */

void CCellView::WriteSylk (FILE *f)
{
	cell c;
	short lastV = 0;
	range r;
	fContainer->GetBounds(r);
	CCellIterator iter(fContainer, &r);
	int i, firstCol, numCols;
	float colWidth = 0.0;
	
	fputs("ID;PSum-It for BeOS\n", f);
	fprintf(f, "B;X%d;Y%d\n", r.right - r.left + 1, r.bottom - r.top + 1);
	while (iter.NextExisting(c))
	{
		if (lastV != c.v)
		{
			fprintf(f, "C;Y%d;X%d;", c.v, c.h);
			lastV = c.v;
		}
		else
			fprintf(f, "C;X%d;", c.h);
		
		char s[256];
		Value val;
		
		if (fContainer->GetType(c) == eNumData)
		{
			Value v;
			fContainer->GetValue(c, v);
			fprintf(f, "K%g", v.fDouble);
		}
		else
		{
			fContainer->GetCellResult(c, s, true);
			fprintf(f, "K\"%s\"", s);
		}
		
		if (fContainer->CellHasFormula(c))
		{
			fContainer->GetCellFormula(c, s, true);
			fprintf(f, ";E%s", s);
		}
		
		fputc('\n', f);
	}
	numCols = (r.right - r.left + 1);
	colWidth = GetColumnWidth(1);
	for (i = 1, firstCol = 1; i <= numCols + 1; i++) {
		if ((i == numCols) || (colWidth != GetColumnWidth(i))) {
			/* the offset limits drifting */
			fprintf(f, "F;W%d %d %ld\n", firstCol, i-1, (long)(colWidth / be_plain_font->StringWidth("x") + 0.95));
			firstCol = i;
			if (i <= numCols)
				colWidth = GetColumnWidth(i);
		}	
	}
	fputs("E\n", f);
} /* WriteSylk */
