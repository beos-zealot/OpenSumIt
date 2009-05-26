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
	InsertCommands.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

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

#ifndef   STLOCKER_H
#include "StLocker.h"
#endif

#ifndef   ROUND_H
#include "Round.h"
#endif


#include <Autolock.h>

#define kCellWouldFallOfDialogID	135

CInsertCommand::CInsertCommand(CCellView *inView, CContainer *inContainer)
	: CCellCommand(kInsertStrID, inView, inContainer),
		fSavedWidths(1, 0.0)
{
	inView->GetSelection(fAffected);
	fDoColumns = (fAffected.top == 1 && fAffected.bottom == kRowCount);
} /* CInsertCommand */

CInsertCommand::~CInsertCommand()
{
}

void CInsertCommand::DoCommand()
{
	cell c, newLoc;
	long v;
	float defWidth, defHeight;
	CFontMetrics *metrics;
	font_height fi;

	fCellView->SetBorderFont(&metrics);
	metrics->Font().GetHeight(&fi);

	StLocker<CContainer> lock(fSourceContainer);
	if (fDoColumns)
	{ // Insert Column
		defWidth = Round(6 * metrics->StringWidth("M"));

		fNumOfCols = fAffected.right - fAffected.left + 1;
		
		fCellView->GetNameTable()->Offset(true, fAffected.left, fNumOfCols);
		fCellView->OffsetGraphicReferences(true, fAffected.left, fNumOfCols);

		range r;
		r.Set(kColCount - fNumOfCols, 1, kColCount, kRowCount);
		CCellIterator iter(fSourceContainer, &r);
		if (iter.NextExisting(c))
			THROW((errCellsWouldFallOf));
		
		fSavedWidths = fCellView->GetWidths();
		fCellView->GetWidths().InsertValues(fAffected.left, fNumOfCols, defWidth);

		StProgress progress(fCellView, kRowCount, false);
	
		for (v = 1; v <= kRowCount; v++)
		{
			c.v = v; c.h = kColCount + 1;
			while (fSourceContainer->GetPreviousCellInRow(c, true))
			{
				newLoc = c;
				if (c.h >= fAffected.left)
					newLoc.h += fNumOfCols;
				fSourceContainer->MoveCell(fSourceContainer, c, newLoc, hSplit,
						 fAffected.left, fNumOfCols);
			}
			progress.Step();
		}
	}
	
	else
	{
		defHeight = Round(fi.ascent + fi.descent + fi.leading + 2);

		fNumOfRows = fAffected.bottom - fAffected.top + 1;
		
		fCellView->GetNameTable()->Offset(false, fAffected.top, fNumOfRows);
		fCellView->OffsetGraphicReferences(false, fAffected.top, fNumOfRows);

		range r;
		r.Set(1, kRowCount - fNumOfRows, kColCount, kRowCount);
		CCellIterator iter(fSourceContainer, &r);
		if (iter.NextExisting(c))
			THROW((errCellsWouldFallOf));

		fSavedWidths = fCellView->GetHeights();
		fCellView->GetHeights().InsertValues(fAffected.top, fNumOfRows, defHeight);

		StProgress progress(fCellView, kRowCount - fNumOfRows, false);

		for (v = kRowCount - fNumOfRows; v >= 1; v--)
		{
			c.v = v; c.h = 0;
			while (fSourceContainer->GetNextCellInRow(c, true))
			{
				newLoc = c;
				if (newLoc.v >= fAffected.top)
					newLoc.v += fNumOfRows;
				fSourceContainer->MoveCell(fSourceContainer, c, newLoc, vSplit,
						 fAffected.top, fNumOfRows);
			}
			progress.Step();
		}
	}

	UpdateView(true);
} /* CInsertCommand::DoCommand */

void CInsertCommand::UndoCommand()
{
	int v;
	cell c, newLoc;
	
	StLocker<CContainer> lock(fSourceContainer);
	if (fDoColumns)
	{
		fCellView->GetWidths() = fSavedWidths;
		
		fCellView->GetNameTable()->Offset(true, fAffected.left,
			-(fAffected.right - fAffected.left + 1));
		fCellView->OffsetGraphicReferences(true, fAffected.left,
			-(fAffected.right - fAffected.left + 1));

		StProgress progress(fCellView, kRowCount, false);
		
		for (v = 1; v <= kRowCount; v++)
		{
			c.v = v; c.h = 0;
			while (fSourceContainer->GetNextCellInRow(c, true))
			{
				newLoc = c;
				if (newLoc.h > fAffected.right)
					newLoc.h -= fNumOfCols;
				fSourceContainer->MoveCell(fSourceContainer, c, newLoc, hSplit, 
					fAffected.left, -fNumOfCols);
			}
			progress.Step();
		}
	}
	
	else
	{
		fCellView->GetHeights() = fSavedWidths;
			
		fCellView->GetNameTable()->Offset(false, fAffected.top,
			-(fAffected.bottom - fAffected.top + 1));
		fCellView->OffsetGraphicReferences(false, fAffected.top,
			-(fAffected.bottom - fAffected.top + 1));

		StProgress progress(fCellView, kRowCount, false);
	
		for (v = 1; v <= kRowCount; v++)
		{
			c.v = v; c.h = 0;
			while (fSourceContainer->GetNextCellInRow(c, true))
			{
				newLoc = c;
				if (newLoc.v > fAffected.bottom)
					newLoc.v -= fNumOfRows;
				fSourceContainer->MoveCell(fSourceContainer, c, newLoc, vSplit, 
					fAffected.top, -fNumOfRows);
			}
			progress.Step();
		}
	}

	UpdateView(true);
}

void CInsertCommand::RedoCommand()
{
	DoCommand();
} /* CInsertCommand::RedoCommand */

CDeleteCommand::CDeleteCommand(CCellView *inView, CContainer *inContainer)
	: CCellCommand(kDeleteStrID, inView, inContainer),
		fSavedWidths(1, 0.0)
{
	inView->GetSelection(fAffected);
	fSavedCells = new CContainer(NULL);
	fDoColumns = (fAffected.top == 1 && fAffected.bottom == kRowCount);
} /* CDeleteCommand */

CDeleteCommand::~CDeleteCommand()
{
	if (fSavedCells) fSavedCells->Release();
} /* ~CDeleteCommand */
	
void CDeleteCommand::DoCommand()
{
	int v;
	float defWidth, defHeight;
	cell c, newLoc;
	CFontMetrics *metrics;
	font_height fi;
	
	BAutolock slock(fSavedCells);
	BAutolock lock(fSourceContainer);
	fCellView->SetBorderFont(&metrics);
	metrics->Font().GetHeight(&fi);

	if (fDoColumns)
	{
		defWidth = Round(6 * fCellView->StringWidth("M"));
	
		fSavedWidths = fCellView->GetWidths();

		fNumOfCols = fAffected.right - fAffected.left + 1;

		fCellView->GetNameTable()->Offset(true, fAffected.left, -fNumOfCols);
		
		{
			CCellIterator iter(fSourceContainer, &fAffected);
			while (iter.NextExisting(c))
			{
				fSourceContainer->MoveCell(fSavedCells, c, c);
			}
		}
		
		fCellView->GetWidths().DeleteValues(fAffected.left, fNumOfCols);

		for (v = 1; v <= kRowCount; v++)
		{
			c.v = v; c.h = 0;
			while (fSourceContainer->GetNextCellInRow(c, true))
			{
				newLoc = c;
				if (newLoc.h > fAffected.right)
					newLoc.h -= fNumOfCols;
				fSourceContainer->MoveCell(fSourceContainer, c, newLoc, hSplit, 
					fAffected.left, -fNumOfCols);
			}
		}
	}
	
	else
	{
		defHeight = Round(fi.ascent + fi.descent + fi.leading + 2);

		fNumOfRows = fAffected.bottom - fAffected.top + 1;
	
		fCellView->GetNameTable()->Offset(false, fAffected.top, -fNumOfRows);
		
		fSavedWidths = fCellView->GetHeights();

		CCellIterator iter(fSourceContainer, &fAffected);
		while (iter.NextExisting(c))
		{
			fSourceContainer->MoveCell(fSavedCells, c, c);
		}
		
		fCellView->GetHeights().DeleteValues(fAffected.top, fNumOfRows);
		
		for (v = 1; v <= kRowCount; v++)
		{
			c.v = v; c.h = 0;
			while (fSourceContainer->GetNextCellInRow(c, true))
			{
				newLoc = c;
				if (newLoc.v > fAffected.bottom)
					newLoc.v -= fNumOfRows;
				fSourceContainer->MoveCell(fSourceContainer, c, newLoc, vSplit, 
					fAffected.top, -fNumOfRows);
			}
		}
	}

	UpdateView(true);
} /* CDeleteCommand::DoCommand */

void CDeleteCommand::UndoCommand()
{
	int v;
	cell c, newLoc;
	
	StLocker<CContainer> slock(fSavedCells);
	StLocker<CContainer> lock(fSourceContainer);
	if (fDoColumns)
	{
		fCellView->GetWidths() = fSavedWidths;
		
		fCellView->GetNameTable()->Offset(true, fAffected.left,
			fAffected.right - fAffected.left + 1);

		for (v = 1; v <= kRowCount; v++)
		{
			c.v = v; c.h = kColCount + 1;
			while (fSourceContainer->GetPreviousCellInRow(c, true))
			{
				newLoc = c;
				if (c.h >= fAffected.left)
					newLoc.h += fNumOfCols;
				fSourceContainer->MoveCell(fSourceContainer, c, newLoc, hSplit,
						 fAffected.left, fNumOfCols);
			}
		}
		
		c.v = 1; c.h = 0;
		while (fSavedCells->GetNextCell(c, true))
		{
			fSavedCells->CopyCell(fSourceContainer, c, c);
		}
	} 
	else
	{
		fCellView->GetHeights() = fSavedWidths;
		
		fCellView->GetNameTable()->Offset(false, fAffected.top,
			fAffected.bottom - fAffected.top + 1);

		for (v = kRowCount - fNumOfRows; v >= 1; v--)
		{
			c.v = v; c.h = 0;
			while (fSourceContainer->GetNextCellInRow(c, true))
			{
				newLoc = c;
				if (newLoc.v >= fAffected.top)
					newLoc.v += fNumOfRows;
				fSourceContainer->MoveCell(fSourceContainer, c, newLoc, vSplit,
						 fAffected.top, fNumOfRows);
			}
		}

		c.v = 1; c.h = 0;
		while (fSavedCells->GetNextCell(c, true))
		{
			fSavedCells->CopyCell(fSourceContainer, c, c);
		}
	}
	
	UpdateView(true);
} /* CDeleteCommand::UndoCommand */

void CDeleteCommand::RedoCommand()
{
	DoCommand();
} /* CDeleteCommand::RedoCommand */
