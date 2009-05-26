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
	CellCommands.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CONFIG_H
#include "Config.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLITERATOR_H
#include "CellIterator.h"
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

#ifndef   PROGRESSVIEW_H
#include "ProgressView.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   CELLPARSER_H
#include "CellParser.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   NAMETABLE_H
#include "NameTable.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

#ifndef   UNDOSTRS_H
#include "undostrs.h"
#endif

#include <Autolock.h>

#define kCellWouldFallOfDialogID	135

//#pragma mark --- CCellCommand ---

CCellCommand::CCellCommand(int inStrID, CCellView *inView,
	CContainer *inCells, bool inUndoable)
	: CCommand(inStrID, inUndoable)
{
	fCellView = inView;
	fSourceContainer = inCells;
} /* CCellCommand */

void CCellCommand::UpdateView(bool redrawAll, bool recalculate)
{
	if (redrawAll)
		fCellView->Invalidate();
	else
		fCellView->DrawAllLines();

	fCellView->DrawStatus();

	if (recalculate)
		fCellView->CalculateAllCells();
} /* UpdateView */

//#pragma mark -
//#pragma mark --- CFillDowmCommand ---

CFillDownCommand::CFillDownCommand(CCellView *inView, CContainer *inContainer)
	: CCellCommand(kFillDownStrID, inView, inContainer)
{
	fSavedCells = new CContainer(NULL);
	inView->GetSelection(fAffected);
} /* CFillDownCommand */

CFillDownCommand::~CFillDownCommand()
{
	fSavedCells->Release();
} /* ~CFillDownCommand */

void CFillDownCommand::DoCommand()
{
	int v, h;
	cell c, srcCell;
	
	StProgress progress(fCellView,
		(fAffected.right - fAffected.left + 1) * (fAffected.bottom - fAffected.top + 1),
		false);
	
	BAutolock slock(fSavedCells);
	
	srcCell.v = fAffected.top;
	for (h = fAffected.left; h <= fAffected.right; h++)
	{
		c.h = h;
		srcCell.h = h;
		for (v = fAffected.top + 1; v <= fAffected.bottom; v++)
		{
			c.v = v;
			BAutolock lock(fSourceContainer);
			fSourceContainer->MoveCell(fSavedCells, c, c);
			fSourceContainer->CopyCell(fSourceContainer, srcCell, c);
			fCellView->TouchLine(v);
			progress.Step();
		}
	}

	UpdateView();	
} /* CFillDownCommand::DoCommand */

void CFillDownCommand::UndoCommand()
{
	int v, h;
	cell c;
	
	StProgress progress(fCellView,
		(fAffected.right - fAffected.left + 1) * (fAffected.bottom - fAffected.top + 1),
		false);
	BAutolock slock(fSavedCells);
	
	for (h = fAffected.left; h <= fAffected.right; h++)
	{
		c.h = h;
		for (v = fAffected.top + 1; v <= fAffected.bottom; v++)
		{
			c.v = v;
			BAutolock lock(fSourceContainer);
			fSourceContainer->DisposeCell(c);
			fSavedCells->CopyCell(fSourceContainer, c, c);
			fCellView->TouchLine(v);
			progress.Step();
		}
	}

	UpdateView();
} /* CFillDownCommand::UndoCommand */

void CFillDownCommand::RedoCommand()
{
	int v, h;
	cell c, srcCell;
	
	StProgress progress(fCellView,
		(fAffected.right - fAffected.left + 1) * (fAffected.bottom - fAffected.top + 1),
		false);
	BAutolock slock(fSavedCells);
	
	srcCell.v = fAffected.top;
	for (h = fAffected.left; h <= fAffected.right; h++)
	{
		c.h = h;
		srcCell.h = h;
		for (v = fAffected.top + 1; v <= fAffected.bottom; v++)
		{
			c.v = v;
			BAutolock lock(fSourceContainer);
			fSourceContainer->DisposeCell(c);
			fSourceContainer->CopyCell(fSourceContainer, srcCell, c);
			fCellView->TouchLine(v);
			progress.Step();
		}
	}

	UpdateView();
} /* CFillDownCommand::RedoCommand */


//#pragma mark -
//#pragma mark --- CFillRightCommand ---

CFillRightCommand::CFillRightCommand(CCellView *inView, CContainer *inContainer)
	: CCellCommand(kFillRightStrID, inView, inContainer)
{
	fSavedCells = new CContainer(NULL);
	inView->GetSelection(fAffected);
} /* CFillRightCommand */

CFillRightCommand::~CFillRightCommand()
{
	fSavedCells->Release();
} /* ~CFillRightCommand */
	
void CFillRightCommand::DoCommand()
{
	int v, h;
	cell c, srcCell;
	
	StProgress progress(fCellView,
		(fAffected.right - fAffected.left + 1) * (fAffected.bottom - fAffected.top + 1),
		false);
	BAutolock slock(fSavedCells);
	
	srcCell.h = fAffected.left;
	for (v = fAffected.top; v <= fAffected.bottom; v++)
	{
		c.v = srcCell.v = v;
		for (h = fAffected.left + 1; h <= fAffected.right; h++)
		{
			c.h = h;
			BAutolock lock(fSourceContainer);
			fSourceContainer->MoveCell(fSavedCells, c, c);
			fSourceContainer->CopyCell(fSourceContainer, srcCell, c);
			progress.Step();
		}
		fCellView->TouchLine(v);
	}
	
	UpdateView();
} /* CFillRightCommand::DoCommand */

void CFillRightCommand::UndoCommand()
{
	int v, h;
	cell c;
	
	StProgress progress(fCellView,
		(fAffected.right - fAffected.left + 1) * (fAffected.bottom - fAffected.top + 1),
		false);
	BAutolock slock(fSavedCells);
	
	for (v = fAffected.top; v <= fAffected.bottom; v++)
	{
		c.v = v;
		for (h = fAffected.left + 1; h <= fAffected.right; h++)
		{
			c.h = h;
			BAutolock lock(fSourceContainer);
			fSourceContainer->DisposeCell(c);
			fSavedCells->CopyCell(fSourceContainer, c, c);
			progress.Step();
		}
		fCellView->TouchLine(v);
	}

	UpdateView();
} /* CFillRightCommand::UndoCommand */

void CFillRightCommand::RedoCommand()
{
	int v, h;
	cell c, srcCell;
	
	StProgress progress(fCellView,
		(fAffected.right - fAffected.left + 1) * (fAffected.bottom - fAffected.top + 1),
		false);
	BAutolock slock(fSavedCells);
	
	srcCell.h = fAffected.left;
	for (v = fAffected.top; v <= fAffected.bottom; v++)
	{
		c.v = srcCell.v = v;
		for (h = fAffected.left + 1; h <= fAffected.right; h++)
		{
			c.h = h;
			BAutolock lock(fSourceContainer);
			fSourceContainer->DisposeCell(c);
			fSourceContainer->CopyCell(fSourceContainer, srcCell, c);
			progress.Step();
		}
		fCellView->TouchLine(v);
	}

	UpdateView();
} /* CFillRightCommand::RedoCommand */


//#pragma mark -
//#pragma mark --- CEnterCellCommand ---

CEnterCellCommand::CEnterCellCommand(CCellView *inView, CContainer *inContainer,	
	const char *inFormula, cell inCell)
		: CCellCommand(kEnterCellStrID, inView, inContainer)
{
	fCell = inCell;
	fFormula = STRDUP(inFormula);
	FailNil(fFormula);
	BAutolock lock(fSourceContainer);
	fSourceContainer->GetCellStyle(fCell, fStyle);
	
	Value v;
	fSourceContainer->GetValue(fCell, v);
	fType = v.fType;
} /* CEnterCellCommand */

CEnterCellCommand::~CEnterCellCommand()
{
	if (fFormula)
		FREE(fFormula);
} /* ~CEnterCellCommand */
	
void CEnterCellCommand::DoCommand()
{
	char s[256];
	
	BAutolock lock(fSourceContainer);
	fSourceContainer->GetCellFormula(fCell, s);

	int pResult = TryToParseString(fFormula, fCell, fSourceContainer, true);
	
	if (pResult)
	{
		if (fFormula)
			FREE(fFormula);
		fFormula = STRDUP(s);
		FailNil(fFormula);
		
		Value v;
		fSourceContainer->GetValue(fCell, v);

		CellStyle cs = fStyle;

		if (v.fType != fType && (fType == eNumData || fType == eTimeData/* || fType == eNoData*/))
			cs.fFormat = pResult == 2 ? ePercent : 0;

		fSourceContainer->SetCellStyle(fCell, cs);
	}
	else
		THROW((B_NO_ERROR));

	fCellView->TouchLine(fCell.v);
	UpdateView();
	fCellView->DrawStatus();
} /* CEnterCellCommand::DoCommand */

void CEnterCellCommand::UndoCommand()
{
	char s[256];
	
	BAutolock lock(fSourceContainer);
	fSourceContainer->GetCellFormula(fCell, s);

	int pResult = TryToParseString(fFormula, fCell, fSourceContainer, true);
	
	if (pResult)
	{
		if (fFormula)
			FREE(fFormula);
		fFormula = STRDUP(s);
		FailNil(fFormula);
		
		fSourceContainer->SetCellStyle(fCell, fStyle);
	}
	else
		THROW((B_NO_ERROR));

	fCellView->TouchLine(fCell.v);
	UpdateView();
	fCellView->DrawStatus();
} /* CEnterCellCommand::UndoCommand */


//#pragma mark -
//#pragma mark --- CEraseCommand ---

CEraseCommand::CEraseCommand(CCellView *inView, CContainer *inCells)
	: CCellCommand (kEraseStrID, inView, inCells)
{
	inView->GetSelection(fAffected);
	fSavedCells = new CContainer(NULL);
	FailNil(fSavedCells);
} /* CEraseCommand:: */

CEraseCommand::~CEraseCommand()
{
	fSavedCells->Release();
} /* ~CEraseCommand */
	
void CEraseCommand::DoCommand()
{
	Value v;
	cell c;

	StProgress progress(fCellView,
		fSourceContainer->CountCells(&fAffected), false);
	BAutolock slock(fSavedCells);
	
	CCellIterator iter(fSourceContainer, &fAffected);	

	while (iter.NextExisting(c))
	{
		BAutolock lock(fSourceContainer);
		fSourceContainer->CopyCell(fSavedCells, c, c);
		fSourceContainer->SetCellFormula(c, NULL);
		fSourceContainer->SetValue(c, v);
		fCellView->TouchLine(c.v);
		progress.Step();
	}

	UpdateView();
} /* DoCommand */

void CEraseCommand::UndoCommand()
{
	cell c;

	StProgress progress(fCellView,
		fSavedCells->CountCells(&fAffected), false);
	
	BAutolock slock(fSavedCells);
	CCellIterator iter(fSavedCells, &fAffected);	

	while (iter.NextExisting(c))
	{
		BAutolock lock(fSourceContainer);
		fSavedCells->CopyCell(fSourceContainer, c, c);
		fCellView->TouchLine(c.v);
		progress.Step();
	}
	
	fCellView->SetSelection(fAffected);
	UpdateView();
} /* UndoCommand */

void CEraseCommand::RedoCommand()
{
	cell c;	
	Value v;

	StProgress progress(fCellView,
		fSourceContainer->CountCells(&fAffected), false);
	
	CCellIterator iter(fSourceContainer, &fAffected);

	while (iter.NextExisting(c))
	{
		BAutolock lock(fSourceContainer);
		fSourceContainer->SetCellFormula(c, NULL);
		fSourceContainer->SetValue(c, v);
		fCellView->TouchLine(c.v);
		progress.Step();
	}
	
	fCellView->SetSelection(fAffected);
	UpdateView();
} /* RedoCommand */
