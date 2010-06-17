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
	CellView.IO.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   NAMETABLE_H
#include "NameTable.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   TEXT_CELLS_H
#include "Text2Cells.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   FILEFORMAT_H
#include "FileFormat.h"
#endif

#ifndef   FUNCTIONUTILS_H
#include "FunctionUtils.h"
#endif

#ifndef   FORMULA_H
#include "Formula.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

#ifndef   CELLSTYLE_H
#include "CellStyle.h"
#endif

#ifndef   CELLITERATOR_H
#include "CellIterator.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#ifndef   FUNCTIONS_H
#include "Functions.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   PROGRESSVIEW_H
#include "ProgressView.h"
#endif

#include <support/Debug.h>
#include <Screen.h>
#include <Entry.h>

static void WriteChunk(BPositionIO& stream, ushort chunk, ushort size,
	const void *data)
{
	short t = htons(chunk);
	CHECKWRITE(stream, &t, 2);
	t = htons(size);
	CHECKWRITE(stream, &t, 2);
	if (data)
		CHECKWRITE(stream, data, size);
}

static int GetOffsetOf(int *list, int key, int max)
{
	int result = 0;
	while (result < max && list[result] != key)
		result++;
	return result < max ? result : -1;
}

static void WarnForMissingFunction(const char *name)
{
	char s[256];
	sprintf(s, GetMessage(msgFunctionNotAvailable), name);
	MAlert *a = new MInfoAlert(s);
	a->Go();
} 

void CCellView::Write(BPositionIO& stream)
{
// Collect the information needed to write the file, fonts first...
	int *fontList, usedFonts;
	fontList = (int *)MALLOC(gFontSizeTable.Count() * sizeof(int));
	FailNil(fontList);
	usedFonts = fContainer->CollectFontList(fontList);
	if (GetOffsetOf(fontList, fBorderFontID, usedFonts) == -1)
		fontList[usedFonts++] = fBorderFontID;

// ...then the styles
	int *styleList, usedStyles;
	styleList = (int *)CALLOC(gStyleTable.Count(), sizeof(int));
	FailNil(styleList);
	usedStyles = fContainer->CollectStyles(styleList);

// Write the version number
	scVersion vers;
	vers.major = 3;
	vers.minor = 0;
	WriteChunk(stream, kscVersion, sizeof(vers), &vers);

// Write some header info, global to the document
	scHeader head;
	head.defaultFormat = 0;
	head.flags = htonl(fAutoRecalc ? kscAutoRecalc : 0);
	head.functionCount = htons(gFuncCount);
	head.cellCount = htonl(fContainer->GetCellCount());
	WriteChunk(stream, kscHeader, sizeof(head), &head);

// Write a view 
	scView view;
	view.windowRect = Window()->Frame();			swap_order(view.windowRect);
	view.position = fPosition;								swap_order(view.position);
	view.frozen = fFrozen;								swap_order(view.frozen);
	view.curCell = fCurCell;								swap_order(view.curCell);
	view.selection = fSelection;							swap_order(view.selection);
	view.headingFont = htons(GetOffsetOf(fontList, fBorderFontID, usedFonts));
	if (fShowGrid) view.flags |= kscShowGrid;
	if (fShowBorders) view.flags |= kscShowHeadings;
	if (fDisplayZero) view.flags |= kscDisplayZero;	swap_order(view.flags);
	WriteChunk(stream, kscView, sizeof(view), &view);

	short size;

// Write the widths of the last view	
	size = fCellWidths.Count() * sizeof(short) * 2;
	void *p = MALLOC(size);
	FailNil(p);
	fCellWidths.Write(p);
	WriteChunk(stream, kscWidths, size, p);
	FREE(p);
	
// And the heights of course
	size = fCellHeights.Count() * sizeof(short) * 2;
	p = MALLOC(size);
	FailNil(p);
	fCellHeights.Write(p);
	WriteChunk(stream, kscHeights, size, p);
	FREE(p);

// Then write the styles for the columns
	size = fContainer->GetColumnStyles().Count() * sizeof(short) * 2;
	p = MALLOC(size);
	FailNil(p);
	scCSElement *sp = (scCSElement *)p;
	fContainer->GetColumnStyles().Write(p);
	for (int i = 0; i < fContainer->GetColumnStyles().Count(); i++)
	{
		swap_order(sp[i].index);
		sp[i].style = htons(GetOffsetOf(styleList, sp[i].style, usedStyles));
	}
	WriteChunk(stream, kscColStyles, size, p);
	FREE(p);

// Continue with the names
	namemap::iterator ni;
	for (ni = fNames->begin(); ni != fNames->end(); ni++)
	{
		scName name;
		char c;
		ushort k = htons(kscName);
		memset(name.name, 0, 32);
		
		CHECKWRITE(stream, &k, 2);
		
		strcpy(name.name, (*ni).first);
		
		if ((*ni).second.BotRight() == (*ni).second.TopLeft())
		{
			k = htons(32 + 6);
			CHECKWRITE(stream, &k, 2);
			CHECKWRITE(stream, name.name, 32);

			c = valCell;
			CHECKWRITE(stream, &c, sizeof(c));
			cell C = (*ni).second.BotRight();
			swap_order(C);
			CHECKWRITE(stream, &C, sizeof(cell));
		}
		else
		{
			k = htons(32 + 10);
			CHECKWRITE(stream, &k, 2);
			CHECKWRITE(stream, name.name, 32);

			c = valRange;
			CHECKWRITE(stream, &c, sizeof(c));
			range r = (*ni).second;
			swap_order(r);
			CHECKWRITE(stream, &r, sizeof(range));
		}
		c = opEnd;
		CHECKWRITE(stream, &c, sizeof(c));
	}

// Then there are the functions used in this document
	CSet funcs;
	fContainer->CollectFunctionNrs(funcs);
	for (int i = kFunctionCount; i < gFuncCount; i++)
		if (funcs[i])
		{
			scFunc func;
			memset(func.name, 0, 10);
			strcpy(func.name, gFuncArrayByNr[i].funcName);
			func.argCnt = htons(gFuncArrayByNr[i].argCnt);
			func.funcNr = htons(gFuncArrayByNr[i].funcNr);
			WriteChunk(stream, kscFunc, sizeof(func), &func);
		}

// Followed by the formatting information. Fonts first
	for (int i = 0; i < usedFonts; i++)
	{
		CFontMetrics fm = gFontSizeTable[fontList[i]];
		scFont *font;
		
		font_family fam;
		font_style sty;
		fm.Font().GetFamilyAndStyle(&fam, &sty);

		ulong size = sizeof(scFont) + strlen(fam) + strlen(sty) + 2;
		
		font = (scFont *)CALLOC(1, size);
		FailNil(font);
		
		font->size = B_HOST_TO_BENDIAN_FLOAT(fm.Font().Size());
		font->color = fm.FontColor();
		
		char *p = (char *)font + sizeof(scFont);
		strcpy(p, sty);
		p += strlen(sty) + 1;
		strcpy(p, fam);

		WriteChunk(stream, kscFont, size, font);
		FREE(font);
	}
	
// Then we get the number formats
	int *formatList, usedFormats;
	formatList = (int *)MALLOC(gFormatTable.Count() * sizeof(int));
	FailNil(formatList);
	usedFormats = fContainer->CollectFormats(formatList);
	for (int i = 0; i < usedFormats; i++)
	{
		CFormatter nf;
		if (formatList[i] < eFirstNewFormat)
			nf = CFormatter(formatList[i]);
		else
			nf = gFormatTable[formatList[i]];
		
		scFormat format;
		format.nr = htons(nf.FormatID());
		format.info[0] = 0;
		format.info[1] = 0;
		format.info[2] = 0;
		format.info[3] = 0;
		
		WriteChunk(stream, kscFormat, sizeof(format)-1, &format);
	}
	
// The style table
	for (int i = 0; i < usedStyles; i++)
	{
		CellStyle cs = gStyleTable[styleList[i]];

		scStyle style;
		memset(&style, 0, sizeof(style));
		style.font = htons(GetOffsetOf(fontList, cs.fFont, usedFonts));
		style.format = htons(GetOffsetOf(formatList, cs.fFormat, usedFormats));
		style.align = cs.fAlignment;
		style.lowColor = cs.fLowColor;
		
		WriteChunk(stream, kscStyle, sizeof(style), &style);
	}
	FREE(fontList);
	FREE(formatList);
	
	int *t = (int *)CALLOC(gStyleTable.Count(), sizeof(int));
	FailNil(t);
	for (int i = 0; i < usedStyles; i++)
		t[styleList[i]] = i;
	
	FREE(styleList);
	styleList = t;
	
// And now its time for some data
	StProgress progress(this, fContainer->GetCellCount(), pColorYellow, false);

	CCellIterator iter(fContainer);
	cell c;
	while (iter.NextExisting(c))
	{
		scCell cl;
		cl.loc = c;							swap_order(cl.loc);
		cl.style = htons(styleList[fContainer->GetCellStyleNr(c)]);
		Value val;
		fContainer->GetValue(c, val);
		
		switch (val.fType)
		{
			case eNoData:
				WriteChunk(stream, kscCellEmpty, kscCellSize, &cl);
				break;
			case eNumData:
			{
				double d = B_HOST_TO_BENDIAN_DOUBLE(val.fDouble);
				memcpy(cl.num, &d, sizeof(double));
				WriteChunk(stream, kscCellNumber, kscCellSize+sizeof(double), &cl);
				break;
			}
			case eBoolData:
				memcpy(cl.num, &val.fBool, sizeof(bool));
				WriteChunk(stream, kscCellBool, kscCellSize+sizeof(bool), &cl);
				break;
			case eTimeData:
			{
				time_t t = htonl(val.fTime);	
				memcpy(cl.num, &t, sizeof(time_t));
				WriteChunk(stream, kscCellDateTime, kscCellSize+sizeof(time_t), &cl);
				break;
			}
			case eTextData:
			{
				WriteChunk(stream, kscCellText, kscCellSize, &cl);
				const char *t = val;
				WriteChunk(stream, kscString, strlen(t) + 1, t);
				break;
			}
			default:
				// there was a warning about not all enum values handled in 
				// switch statement.
				break;
		}
		
		CFormula form = fContainer->GetCellFormula(c);
		if (form.IsFormula())
		{
			BMallocIO buf;
			form.Write(buf);
			
			WriteChunk(stream, kscFormula, buf.BufferLength(), buf.Buffer());
		}
		progress.Step();
	}
	
	WriteCharts(stream);
	
// cleanup the mess
	
	WriteChunk(stream, kscEnd, 0, NULL);

	FREE(styleList);
}


void CCellView::Read(BPositionIO& stream)
{
	CSwapStream str(stream);

	scChunk chunk;
	long offset;
	int *funcList, *styleList, *fontList, *formatList;
	int funcCount, styleCount, fontCount, formatCount, borderFont = fBorderFontID;
	bool warnForIncorrectFormula = true;
	scCell cl;

	styleCount = 0;
	styleList = (int *)MALLOC(0);
	fontCount = 0;
	fontList = (int *)MALLOC(0);
	formatCount = 0;
	formatList = (int *)MALLOC(0);
	funcList = NULL;
	
	scCSElement *colStyles = NULL;
	int colStyleCount = 0;

	offset = 0;

	StProgress progress(this, 1, pColorYellow, false);
	StWriteLock lock(fContainer);

	try
	{
		stream.Seek(offset, SEEK_SET);
		
		str >> chunk;

		if (chunk.type == kscVersion)
		{
			scVersion vers;
			str >> vers;
			if (vers.major != 3)
				THROW((errTooNewFileFormat));
		}
		else
			THROW((errUnknownFileFormat, ((CCellWindow *)Window())->EntryRef()->name));

		do
		{
			if (stream.Seek(offset, SEEK_SET) < offset)
			{
				MStopAlert("File is too short").Go();
				break;
			}
			
			str >> chunk;
			offset += 4 + chunk.size;
			
			switch (chunk.type)
			{
				case kscVersion:
					break;
				case kscHeader:
				{
					scHeader head;
					str >> head;
					
					funcList = (int *)CALLOC(head.functionCount, sizeof(int));
					FailNil(funcList);
					funcCount = head.functionCount;
					progress.NewMax(head.cellCount);
					break;
				}
				case kscView:
				{
					scView view;
					str >> view;
					
					if (view.windowRect.left > 0 && view.windowRect.top > 0)
					{
						BRect r;
						{
							r = BScreen().Frame();
						}

						r.InsetBy(4, 4);
						r = r & view.windowRect;
						
						if (r.IsValid() && r.Width() >= 300 && r.Height() >= 100)
						{
							Window()->MoveTo(r.left, r.top);
							Window()->ResizeTo(r.Width(), r.Height());
						}
					}
					if (view.position.h > 0 && view.position.v > 0)
						fPosition = view.position;
					fFrozen = view.frozen;
					if (view.selection.IsValid())
						fSelection = view.selection;
					if (fSelection.Contains(view.curCell))
						fCurCell = view.curCell;
					borderFont = view.headingFont;
					fShowGrid = (view.flags & kscShowGrid) != 0;
					fShowBorders = (view.flags & kscShowHeadings) != 0;
					if (!fShowBorders)
					{
						fCellBounds.left -= fBorderWidth;
						fCellBounds.top -= fBorderHeight;
					}

					Window()->Show();
					Window()->UpdateIfNeeded();
					break;
				}
				case kscPrint:
					break;
				case kscWidths:
				case kscHeights:
				{
					int count = chunk.size/(sizeof(short)*2);
					scWidthElement *elems = (scWidthElement *)MALLOC(chunk.size);
					FailNil(elems);
					
					int k = count * sizeof(scWidthElement);
					stream.Read(elems, k);
					
					if (chunk.type == kscWidths)
						fCellWidths.Read(count, elems);
					else
						fCellHeights.Read(count, elems);

					FREE(elems);
					break;
				}
				case kscColStyles:
				{
					colStyleCount = chunk.size/(sizeof(short)*2);
					colStyles = (scCSElement *)MALLOC(chunk.size);
					FailNil(colStyles);
					
					for (int i = 0; i < colStyleCount; i++)
						str >> colStyles[i];
					break;
				}
				case kscName:
				{
					char buf[50];
					scName *name = (scName *)buf;
					int l = std::min((int)chunk.size, 50);
					
					stream.Read(name, l);

					range r;
					
					memcpy(&r, name->reference+1, sizeof(cell));
					if (name->reference[0] == valRange)
						memcpy(&r.bottom, name->reference+5, sizeof(cell));
					else
						r.BotRight() = r.TopLeft();
					
					swap_order(r);
					
					if (fNames->count(name->name) == 0)
						(*fNames)[CName(name->name)] = r;
					break;
				}
				case kscFunc:
				{
					scFunc func;
					str >> func;
					
					if (!funcList) THROW((errCorruptedFile));
					if (func.funcNr >= funcCount)
						THROW((errCorruptedFile));
					
					funcList[func.funcNr] = GetFunctionNr(func.name);
					if (funcList[func.funcNr] == -1)
						WarnForMissingFunction(func.name);
					break;
				}
				case kscFont:
				{
					scFont font;
					str >> font;
					font_family fam;
					font_style sty;
					
					ReadCString(stream, sizeof(font_style), sty);
					ReadCString(stream, sizeof(font_family), fam);
				
					int *t = (int *)REALLOC(fontList, (fontCount+1)*sizeof(int));
					FailNil(t);
					fontList = t;
					fontList[fontCount] = gFontSizeTable.GetFontID(
						fam, sty, font.size, font.color);
					fontCount++;
					break;
				}
				case kscFormat:
				{
					scFormat format;
					str >> format;
					
					int *t = (int *)REALLOC(formatList, (formatCount+1)*sizeof(int));
					FailNil(t);
					formatList = t;
					
					if (format.nr < eFirstNewFormat)
						formatList[formatCount] = format.nr;
					else
					{
						char fs[256];
						ReadCString(stream, 255, fs);
						formatList[formatCount] = gFormatTable.GetFormatID(fs);
					}
						
					formatCount++;
					break;
				}
				case kscStyle:
				{
					scStyle style;
					str >> style;
					
					int *t = (int *)REALLOC(styleList, (styleCount+1)*sizeof(int));
					FailNil(t);
					styleList = t;
					
					CellStyle cs;

					if (style.font >= fontCount)
						THROW((errCorruptedFile));
					cs.fFont = fontList[style.font];

					if (style.format >= formatCount)
						THROW((errCorruptedFile));
					cs.fFormat = formatList[style.format];

					cs.fAlignment = style.align;
					cs.fLowColor = style.lowColor;
					cs.fLocked = (style.flags & kscLocked) != 0;
					cs.fHidden = (style.flags & kscHidden) != 0;
					
					styleList[styleCount] = gStyleTable.GetStyleID(cs);
					
					styleCount++;
					break;
				}
				case kscCellEmpty:
				case kscCellBool:
				case kscCellNumber:
				case kscCellDateTime:
				case kscCellText:
				{
					str >> cl;
					
					Value val;
					
					switch (chunk.type)
					{
						case kscCellBool:
						{
							bool b;
							str >> b;
							val = b;
							break;
						}
						case kscCellNumber:
						{
							double d;
							str >> d;
							val = d;
							break;
						}
						case kscCellDateTime:
						{
							time_t t;
							str >> t;
							val = t;
							break;
						}
						case kscCellText:
						{
							// do nothing yet...
						}
					}
	
					fContainer->NewCell(cl.loc, val, NULL);
					if (cl.style >= styleCount)
						/*THROW((errCorruptedFile))*/;
					else
						fContainer->SetCellStyleNr(cl.loc, styleList[cl.style]);
					
					progress.Step();
					break;
				}
				case kscString:
				{
					char s[256];
					Value val;
					stream.Read(s, std::min((int)chunk.size, 255));
					s[std::min((int)chunk.size, 255)] = 0;
					val = s;
					
					fContainer->SetValue(cl.loc, val);
					break;
				}
				case kscFormula:
				{
					CFormula form;
					try
					{
						if (!funcList) THROW((errCorruptedFile));
						form.Read(stream, funcList);
						Value v;
						form.Calculate(cl.loc, v, fContainer);
						fContainer->SetCellFormula(cl.loc, form.CopyString());
					}
					catch (CErr& e)
					{
						CATCHED;
						char s[32];
						cl.loc.GetName(s);
#if DEBUG
						printf("%s in formula of cell %s\n", (char *)e, s);
#endif
						if (warnForIncorrectFormula)
						{
							char m[256];
							sprintf(m, GetMessage(msgIncorrectFormula), s);
							MAlert *a = new MWarningAlert(m, GetMessage(msgOK),
								GetMessage(msgNoMoreWarnings));
							if (a->Go() == 2)
								warnForIncorrectFormula = false;
						}
					}
					form.Clear();
					break;
				}
				case kscChart:
					ReadChart(stream, chunk.size);
				case kscEnd:
					break;
				default:
					MStopAlert("File contains errors").Go();
					chunk.type = kscEnd;
					break;
			}
		}
		while (chunk.type != kscEnd);

// adjust the fields that couldn't be adjusted before
		if (fontCount && borderFont < fontCount)
			fBorderFontID = fontList[borderFont];
		
		if (colStyles && styleList)
		{
			for (int i = 0; i < colStyleCount; i++)
			{
				if (colStyles[i].style >= 0 && colStyles[i].style < styleCount)	
					colStyles[i].style = styleList[colStyles[i].style];
			}
			fContainer->GetColumnStyles().Read(colStyleCount, colStyles);
		}
	}
	
	catch (...)
	{
		CATCHED;
		if (fontList) FREE(fontList);
		if (formatList) FREE(formatList);
		if (styleList) FREE(styleList);
		if (funcList) FREE(funcList);
		if (colStyles) FREE(colStyles);
		throw;
	}
	
	if (fontList) FREE(fontList);
	if (formatList) FREE(formatList);
	if (styleList) FREE(styleList);
	if (funcList) FREE(funcList);
	if (colStyles) FREE(colStyles);

	RecalculateBorderWidths();
	AdjustScrollBars();
	Invalidate();
	Window()->UpdateIfNeeded();
	DrawStatus();

}
