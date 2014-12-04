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
	Copyright 1997, Hekkelman Programmatuur
	
*/

#include "CellView.h"
#include "Container.h"
#include "CellIterator.h"
#include "Command.h"
#include "CellCommands.h"
#include "MyError.h"
#include "Formula.h"
#include "ProgressView.h"
#include "MAlert.h"
#include "StringTable.h"
#include "CellParser.h"
#include "FontMetrics.h"
#include "NameTable.h"
#include "Formatter.h"
#include "undostrs.h"
#include "StyleCommand.h"
#include "StLocker.h"
#include <cstring>

CStyleCommand::CStyleCommand(CCellView *inView, CContainer *inContainer)
	: CCellCommand(kStyleStrID, inView, inContainer)
	, fSavedHeights(inView->GetHeights())
	, fSavedColumnStyles(inContainer->GetColumnStyles())
	, fSavedRowStyles(inContainer->GetRowStyles())
{
	StLocker<CContainer> lock(inContainer);

	fType = eNoData;
	fCellView->GetSelection(fAffected);
	fRowSelected = fAffected.left == 1 && fAffected.right == kColCount;
	fColSelected = fAffected.top == 1 && fAffected.bottom == kRowCount;
} /* CStyleCommand::CStyleCommand */

CStyleCommand::~CStyleCommand()
{
	delete[] fSavedStyles;
} /* CStyleCommand::CStyleCommand */
	
void CStyleCommand::DoCommand()
{
	long numOfCells = 0, i;
	CellStyle cs;
	int sn;
	cell c;
	bool aRC = fRowSelected || fColSelected;
	float savedH, height;
	
	if (fRowSelected && fColSelected)
		fSourceContainer->SetDefaultCellStyle(fNewStyle);

	if (!aRC)
		numOfCells = (fAffected.right - fAffected.left + 1) *
					 (fAffected.bottom - fAffected.top + 1);
	else
	{
		StLocker<CContainer> lock(fSourceContainer);
		numOfCells = fSourceContainer->CountCells(&fAffected);
	}
	
	StProgress progress(fCellView, numOfCells, false);
	
	fSavedStyles = new int[numOfCells];
	FailNil(fSavedStyles);
	
	i = 0;
	StLocker<CContainer> lock(fSourceContainer);
	CCellIterator iter(fSourceContainer, &fAffected);
	while (aRC ? iter.NextExisting(c) : iter.Next(c))
	{
		CRunArray& heights = fCellView->GetHeights();
		savedH = height = heights[c.v] - heights[c.v - 1];

		sn = fSourceContainer->GetCellStyleNr(c);
		cs = gStyleTable[sn];
		
		if (fType == eNoData)
			ChangeStyle(cs, height);
		else
		{
			int cType = fSourceContainer->GetType(c);
			if (cType == fType || cType == eNoData)
				ChangeStyle(cs, height);
		}
		
		if (!fSourceContainer->Exists(c))
			fSavedStyles[i++] = -1;
		else
			fSavedStyles[i++] = sn;
		
		fSourceContainer->SetCellStyle(c, cs);
		fCellView->TouchLine(c.v);
		
		if (height > savedH)
			heights.SetValue(c.v, ceil(height));
		
		progress.Step();
	}
	
	if (fColSelected)
		for (int i = fAffected.left; i <= fAffected.right; i++)
		{
			fSourceContainer->GetColumnStyle(i, cs);
			ChangeStyle(cs, height);
			fSourceContainer->SetColumnStyle(i, cs);
		}
	if (fRowSelected)
		for (int i = fAffected.top; i <= fAffected.bottom; i++)
		{
			fSourceContainer->GetRowStyle(i, cs);
			ChangeStyle(cs, height);
			fSourceContainer->SetRowStyle(i, cs);
		}

	UpdateView(true, false);
} /* CStyleCommand::DoCommand */

void CStyleCommand::UndoCommand()
{
	cell c;
	int cs;
	long numOfCells = 0, i;
	bool aRC = fRowSelected || fColSelected;

	if (fRowSelected && fColSelected)
		fSourceContainer->SetDefaultCellStyle(fNewStyle);
	
	if (!aRC)
		numOfCells = (fAffected.right - fAffected.left + 1) *
					 (fAffected.bottom - fAffected.top + 1);
	else
	{
		StLocker<CContainer> lock(fSourceContainer);
		numOfCells = fSourceContainer->CountCells(&fAffected);
	}
	
	StProgress progress(fCellView, numOfCells, false);

	StLocker<CContainer> lock(fSourceContainer);
	CCellIterator iter(fSourceContainer, &fAffected);
	i = 0;
	while (aRC ? iter.NextExisting(c) : iter.Next(c))
	{
		if (fSourceContainer->Exists(c))
			cs = fSourceContainer->GetCellStyleNr(c);
		else
			cs = -1;

		if (fSavedStyles[i] == -1)
			fSourceContainer->DisposeCell(c);
		else
			fSourceContainer->SetCellStyleNr(c, fSavedStyles[i]);

		fSavedStyles[i] = cs;
		i++;
		fCellView->TouchLine(c.v);
		progress.Step();
	}

	CRunArray temp(fSavedHeights);
	fSavedHeights = fCellView->GetHeights();	
	fCellView->GetHeights() = temp;

	if (fColSelected)
	{
		StLocker<CContainer> lock(fSourceContainer);
		CRunArray2 tmp = fSourceContainer->GetColumnStyles();
		fSourceContainer->GetColumnStyles() = fSavedColumnStyles;
		fSavedColumnStyles = tmp;
	}

	if (fRowSelected)
	{
		StLocker<CContainer> lock(fSourceContainer);
		CRunArray2 tmp = fSourceContainer->GetRowStyles();
		fSourceContainer->GetRowStyles() = fSavedRowStyles;
		fSavedRowStyles = tmp;
	}

	UpdateView(true, false);
} /* CStyleCommand::UndoCommand */

void CStyleCommand::RedoCommand()
{
	UndoCommand();
} /* CStyleCommand::RedoCommand */


CFontStyleCommand::CFontStyleCommand(CCellView *inView, CContainer *inContainer,
		const char *family)
	: CStyleCommand(inView, inContainer)
{
	strcpy(fNewFamily, family);
} /* CFontStyleCommand */

void CFontStyleCommand::ChangeStyle(CellStyle& ioStyle, float& ioHeight)
{
	const CFontMetrics *fm;
	fm = &gFontSizeTable[ioStyle.fFont];
	
	ioStyle.fFont = gFontSizeTable.GetFontID(fNewFamily, fm->Style(),
		fm->Size(), fm->FontColor());
	
	font_height fh;
	BFont font(gFontSizeTable[ioStyle.fFont].Font());
	font.GetHeight(&fh);
	ioHeight = fh.ascent + fh.descent + fh.leading + 1;
} /* CFontStyleCommand::ChangeStyle */


CFaceStyleCommand::CFaceStyleCommand(CCellView *inView, CContainer *inContainer,
		const char *face)
	: CStyleCommand(inView, inContainer)
{
	strcpy(fNewFace, face);
} /* CStyleStyleCommand::CStyleStyleCommand */

void CFaceStyleCommand::ChangeStyle(CellStyle& ioStyle, float& ioHeight)
{
	const CFontMetrics *fm;
	fm = &gFontSizeTable[ioStyle.fFont];
	
	ioStyle.fFont = gFontSizeTable.GetFontID(fm->Family(), fNewFace,
		fm->Size(), fm->FontColor());
	
	font_height fh;
	BFont font(gFontSizeTable[ioStyle.fFont].Font());
	font.GetHeight(&fh);
	ioHeight = fh.ascent + fh.descent + fh.leading + 1;
} /* CStyleStyleCommand::ChangeStyle */

CSizeStyleCommand::CSizeStyleCommand(CCellView *inView, CContainer *inContainer,
		float newSize)
	: CStyleCommand(inView, inContainer)
{
	fNewSize = newSize;
} /* CSizeStyleCommand::CSizeStyleCommand */

void CSizeStyleCommand::ChangeStyle(CellStyle& ioStyle, float& ioHeight)
{
	const CFontMetrics *fm;
	fm = &gFontSizeTable[ioStyle.fFont];
	
	ioStyle.fFont = gFontSizeTable.GetFontID(fm->Family(), fm->Style(),
		fNewSize, fm->FontColor());
	
	font_height fh;
	BFont font(gFontSizeTable[ioStyle.fFont].Font());
	font.GetHeight(&fh);
	ioHeight = fh.ascent + fh.descent + fh.leading + 1;
} /* CSizeStyleCommand::ChangeStyle */

CTextColorStyleCommand::CTextColorStyleCommand(CCellView *inView, CContainer *inContainer,
		rgb_color newColor)
	: CStyleCommand(inView, inContainer)
{
	fNewColor = newColor;
} /* CTextColorStyleCommand::.. */

void CTextColorStyleCommand::ChangeStyle(CellStyle& ioStyle, float& ioHeight)
{
	const CFontMetrics *fm;
	fm = &gFontSizeTable[ioStyle.fFont];
	
	ioStyle.fFont = gFontSizeTable.GetFontID(fm->Family(), fm->Style(),
		fm->Size(), fNewColor);
} /* CTextColorStyleCommand::ChangeStyle */

CLowColorStyleCommand::CLowColorStyleCommand(CCellView *inView, CContainer *inContainer,
		rgb_color newColor)
	: CStyleCommand(inView, inContainer)
{
	fNewColor = newColor;
} /* CLowColorStyleCommand */

void CLowColorStyleCommand::ChangeStyle(CellStyle& ioStyle, float& ioHeight)
{
	ioStyle.fLowColor = fNewColor;
} /* CLowColorStyleCommand::ChangeStyle */

CAlignmentStyleCommand::CAlignmentStyleCommand(CCellView *inView, CContainer *inContainer,
		int newAlignment)
	: CStyleCommand(inView, inContainer)
{
	fNewAlignment = newAlignment;
} /* CAlignmentStyleCommand */

void CAlignmentStyleCommand::ChangeStyle(CellStyle& ioStyle, float& ioHeight)
{
	ioStyle.fAlignment = fNewAlignment;
} /* CAlignmentStyleCommand::ChangeStyle */

CFormatStyleCommand::CFormatStyleCommand(CCellView *inView, CContainer *inContainer,
		int newFormat)
	: CStyleCommand(inView, inContainer)
{
	fNewFormat = newFormat;
	fType = eNumData;
} /* CFormatStyleCommand::CFormatStyleCommand */

void CFormatStyleCommand::ChangeStyle(CellStyle& ioStyle, float& ioHeight)
{
	CFormatter of, nf;
	
	of = gFormatTable[ioStyle.fFormat];
	
	if (fNewFormat == eCurrency && of.OldFormatID() == eGeneral /*&&
		of.Digits() == 0 && of.Commas() == false*/)
		nf = CFormatter(eCurrency, gCurrencyDigits, true);
	else
		nf = CFormatter(fNewFormat, of.Digits(), of.Commas());
	
	ioStyle.fFormat = gFormatTable.GetFormatID(nf);
} /* CFormatStyleCommand::ChangeStyle */

CTimeFormatStyleCommand::CTimeFormatStyleCommand(CCellView *inView, CContainer *inContainer,
		int newFormat)
	: CStyleCommand(inView, inContainer)
{
	fNewFormat = newFormat;
	fType = eTimeData;
} /* CTimeFormatStyleCommand::CTimeFormatStyleCommand */

void CTimeFormatStyleCommand::ChangeStyle(CellStyle& ioStyle, float& ioHeight)
{
	ioStyle.fFormat = fNewFormat;
} /* CTimeFormatStyleCommand::ChangeStyle */

CDigitsStyleCommand::CDigitsStyleCommand(CCellView *inView, CContainer *inContainer,
		int newDigits)
	: CStyleCommand(inView, inContainer)
{
	fNewDigits = newDigits;
	fType = eNumData;
} /* CDigitsStyleCommand::CDigitsStyleCommand */

void CDigitsStyleCommand::ChangeStyle(CellStyle& ioStyle, float& ioHeight)
{
	CFormatter of, nf;
	
	of = gFormatTable[ioStyle.fFormat];
	if (of.OldFormatID() == eGeneral)
		nf = CFormatter(eFixed, fNewDigits, of.Commas());
	else
		nf = CFormatter(of.OldFormatID(), fNewDigits, of.Commas());	
	
	ioStyle.fFormat = gFormatTable.GetFormatID(nf);
} /* CDigitsStyleCommand::ChangeStyle */

CCommasStyleCommand::CCommasStyleCommand(CCellView *inView, CContainer *inContainer,
		bool newCommas)
	: CStyleCommand(inView, inContainer)
{
	fNewCommas = newCommas;
	fType = eNumData;
} /* CCommasStyleCommand::CCommasStyleCommand */

void CCommasStyleCommand::ChangeStyle(CellStyle& ioStyle, float& ioHeight)
{
	CFormatter of, nf;
	
	of = gFormatTable[ioStyle.fFormat];
	nf = CFormatter(of.OldFormatID(), of.Digits(), fNewCommas);
	
	ioStyle.fFormat = gFormatTable.GetFormatID(nf);
} /* CCommasStyleCommand::ChangeStyle */
