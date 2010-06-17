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
	CellView.mouse.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   CELLCOMMANDS_H
#include "CellCommands.h"
#endif

#ifndef   COPYCOMMANDS_H
#include "CopyCommands.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#include <support/Debug.h>
#include <ctype.h>
#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   TEXT_CELLS_H
#include "Text2Cells.h"
#endif

#ifndef   CELLSCROLLBAR_H
#include "CellScrollBar.h"
#endif

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
#endif

#ifndef   CELLPARSER_H
#include "CellParser.h"
#endif

#ifndef   FORMULA_H
#include "Formula.h"
#endif

#ifndef   MTHREAD_H
#include "MThread.h"
#endif

#ifndef   CALCULATEJOB_H
#include "CalculateJob.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   GRAPHIC_H
#include "Graphic.h"
#endif

#include <Beep.h>
#include <Message.h>

const pattern
	kResizeVPat = { {0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0}},
	kResizeHPat = { {0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0}};

void CCellView::MouseDown(BPoint where)
{
	StPenState save(this);
	
	try
	{
		bool optional = IsOptionalClick(Window()->CurrentMessage());
		
		if (!IsFocus() && !fEntering)
			MakeFocus();

		ResetOffset(-1);
	
		BRect myRect, bounds(Bounds());
		cell myCell = cell(fFrozen.h ? 1 : fPosition.h, fFrozen.v ? 1 : fPosition.v);

		if (fFirstGraphic)
		{
			if (fFirstGraphic->HandleMouseDown(where))
				return;
			else if (fFirstGraphic->HasFocus())
				fFirstGraphic->MakeFocus(false);
		}
	
		if (fCellBounds.Contains(where))
		{
			if ((PointIsInSelection(where) &&
				!fEntering) &&
				(optional ||
				WaitMouseMoved(where, false)))
			{
				StartDrag(where, optional);
			}
			else if (optional)
				GridMenu(where);
			else
				SelectCell(where);
		}
		
		if (!fShowBorders)
			return;
		
		myRect = bounds;
		myRect.top = fBorderHeight;
		myRect.left = 0.0;
		myRect.right = fBorderWidth;
	
		if (myRect.Contains(where))
		{
			if (optional)
				BorderMenu(where);
			else
			{
				do
				{
					myCell.v++;
					GetCellRect(myCell, myRect);
				}
				while (myRect.top + 2.0 <= where.y);
		
				if (where.y >= myRect.top - 2.0 && where.y <= myRect.top + 2.0)
					ResizeRow(where, myCell.v - 1);
				else
					SelectRow(where, myCell.v - 1);
			}
			return;
		}
	
		myRect.Set(fBorderWidth, 0.0, bounds.right, fBorderHeight);
	
		if (myRect.Contains(where))
		{
			if (optional)
				BorderMenu(where);
			else
			{
				do
				{
					myCell.h++;
					GetCellRect(myCell, myRect);
				}
				while (myRect.left + 2.0 <= where.x);
		
				if (where.x >= myRect.left - 2.0 && where.x <= myRect.left + 2.0)
					ResizeCol(where, myCell.h - 1);
				else
					SelectCol(where, myCell.h - 1);
			}
			return;
		}
	
		myRect.Set(0.0, 0.0, fBorderWidth, fBorderHeight);
	
		if (myRect.Contains(where))
		{
			if (!fEntering)
				SetSelection(range(1, 1, kColCount, kRowCount));
			else
				beep();
		}
	}
	
	catch(CErr& e)
	{
		CATCHED;
		e.DoError();
		return;
	}
	
	catch(...)
	{
		CATCHED;
		ASSERT(FALSE);
	}
}

void
CCellView::SelectRow(BPoint where, int rowNr)
{
	BPoint curPoint;
	cell oldAnts, curCurCell = fCurCell;
	range curSelection = fSelection, tmpSelection;
	long anchorRow, curRow, lastRow;
	float t, b;
	BRegion oldSel, newSel, oldClip;
	ulong modifiers = Window()->CurrentMessage()->FindInt32("modifiers");

	curPoint = where;
	curRow = rowNr;

	ClearAnts();

	if (!fEntering && (modifiers & B_SHIFT_KEY))
	{
		if (curRow > fSelection.bottom)
			anchorRow = fSelection.top;
		else
			anchorRow = fSelection.bottom;
	}
	else
		anchorRow = curRow;
	lastRow = curRow;

	if (!fEntering)
		SelectionToRegion(oldSel);

	if (anchorRow > curRow)
		fSelection.Set(1, curRow, kColCount, anchorRow);
	else
		fSelection.Set(1, anchorRow, kColCount, curRow);

	fCurCell = fSelection.TopLeft();
	DrawStatus();
	DrawBorders();
	
	if (!fEntering)
	{
		SelectionToRegion(newSel);
		ChangeSelection(&oldSel, &newSel);
	}

	ulong buttons;
	GetMouse(&curPoint, &buttons);
	while (buttons)
	{
		oldAnts = fCurCell;

		cell c;
		GetCellHitBy(curPoint, c);
		curRow = c.v;

		fCurCell.v = curRow;
		if (fEntering)
		{
			tmpSelection = fSelection;
			fSelection = curSelection;
		}
		ScrollToSelection();
		if (fEntering)
			fSelection = tmpSelection;
		fCurCell = oldAnts;

		if (lastRow != curRow)
		{

			if (fEntering)
				ClearAnts();
			else
				SelectionToRegion(oldSel);

			if (curRow > anchorRow)
				fSelection.Set(1, anchorRow, kColCount, curRow);
			else
				fSelection.Set(1, curRow, kColCount, anchorRow);
			
//			SetFontName("Emily");
//			SetFontSize(12);

			if (lastRow > curRow)
			{
				b = lastRow; t = curRow;
			}
			else
			{
				b = curRow; t = lastRow;
			}
			for (long x = static_cast<long>(t) ;
			     x <= static_cast<long>(b) ;
				 x++ )
			{
				BRect r;
				char s[10];
				
				c.v = x;
				GetCellRect(c, r);
				r.left = 0.0;
				r.right = fBorderWidth - 1.0;

				if (r.top != r.bottom)
				{
					sprintf(s, "%d", x);
					FillRect3D(this, r, x >= fSelection.top && x <= fSelection.bottom, true, s);
				}
			}
			
			if (!fEntering)
			{
				SelectionToRegion(newSel);
				ChangeSelection(&oldSel, &newSel);
			}

			if (!fEntering && fCurCell.v != fSelection.top)
				ClearAnts();

			fCurCell = fSelection.TopLeft();
			DrawStatus();
		}

		MarchAnts();
		lastRow = curRow;
		GetMouse(&curPoint, &buttons);
	}
	
	if (fEntering)
	{
		fEditBox->EnterRange(fSelection);
		ClearAnts();
		fSelection = curSelection;
		fCurCell = curCurCell;
		DrawStatus();
		DrawBorders();
	}
} /* SelectRow */

void
CCellView::SelectCol(BPoint where, int colNr)
{
	BPoint curPoint;
	cell oldAnts, curCurCell = fCurCell;
	range curSelection = fSelection, tmpSelection;
	long anchorCol, curCol, lastCol, y;
	float l, r;
	BRegion oldSel, newSel, oldClip;
	ulong modifiers = Window()->CurrentMessage()->FindInt32("modifiers");

	curPoint = where;

	curCol = colNr;

	ClearAnts();

	if (!fEntering && (modifiers & B_SHIFT_KEY))
	{
		if (curCol > fSelection.right)
			anchorCol = fSelection.left;
		else
			anchorCol = fSelection.right;
	}
	else
		anchorCol = curCol;
	lastCol = curCol;

	if (!fEntering)
		SelectionToRegion(oldSel);
	
	if (anchorCol > curCol)
		fSelection.Set(curCol, 1, anchorCol, kRowCount);
	else
		fSelection.Set(anchorCol, 1, curCol, kRowCount);
		
	fCurCell = fSelection.TopLeft();
	DrawStatus();
	DrawBorders();

	if (!fEntering)
	{
		SelectionToRegion(newSel);
		ChangeSelection(&oldSel, &newSel);
	}

	ulong buttons;
	GetMouse(&curPoint, &buttons);
	while (buttons)
	{
		oldAnts = fCurCell;

		cell c;
		GetCellHitBy(curPoint, c);
		curCol = std::max((int)c.h, 1);
	
		fCurCell.h = curCol;
		if (fEntering)
		{
			tmpSelection = fSelection;
			fSelection = curSelection;
		}
		ScrollToSelection();
		if (fEntering)
			fSelection = tmpSelection;
		fCurCell = oldAnts;

		if (lastCol != curCol)
		{
			if (fEntering)
				ClearAnts();
			else
				SelectionToRegion(oldSel);

			if (curCol > anchorCol)
				fSelection.Set(anchorCol, 1, curCol, kRowCount);
			else
				fSelection.Set(curCol, 1, anchorCol, kRowCount);
			
//			SetFontName("Emily");
//			SetFontSize(12);

			if (lastCol > curCol)
			{
				r = lastCol; l = curCol;
			}
			else
			{
				r = curCol; l = lastCol;
			}
			
			for (y =  static_cast<long>(l) ;
				 y <= static_cast<long>(r) ;
				 y++ )
			{
				c.h = y;
				BRect r;
				char s[10];
				
				GetCellRect(c, r);
				r.top = 1.0;
				r.bottom = fBorderHeight - 1.0;
				
				if (r.left != r.right)
				{
					NumToAString(y, s);
					FillRect3D(this, r, y >= fSelection.left && y <= fSelection.right, true, s);
				}
			}
				
			if (!fEntering)
			{
				SelectionToRegion(newSel);
				ChangeSelection(&oldSel, &newSel);
			}

			if (!fEntering && fCurCell.h != fSelection.left)
				ClearAnts();

			fCurCell = fSelection.TopLeft();
			DrawStatus();
		}

		MarchAnts();
		lastCol = curCol;
		GetMouse(&curPoint, &buttons);
	}

	if (fEntering)
	{
		fEditBox->EnterRange(fSelection);
		ClearAnts();
		fSelection = curSelection;
		fCurCell = curCurCell;
		DrawStatus();
		DrawBorders();
	}
} 

void CCellView::SelectCell(BPoint where)
{
	cell curCell, anchorCell, oldCurCell, tmpCell, prevCurCell;
	BPoint cPoint;
	range r, prevSel;
	BRegion lastSelectionRgn, newSelectionRgn;
	bool updateBorders = (fSelection.top == 1 && fSelection.bottom == kRowCount) ||
						 (fSelection.left == 1 && fSelection.right == kColCount);
	BMessage * current = Window()->CurrentMessage();
	int32 modifiers;
	FailOSErr(current->FindInt32("modifiers", &modifiers), errMessageMissing);

	prevSel = fSelection;
	prevCurCell = fCurCell;

	StClipCells clip(this);

	ClearAnts();

	cPoint = where;
	(void)GetCellHitBy(cPoint, curCell);

	if (!fEntering)
		HiliteSelection(false, false);

	if (!fEntering && modifiers & B_SHIFT_KEY)
	{
		if (curCell.v < fSelection.top)
			anchorCell.v = fSelection.bottom;
		else
			anchorCell.v = fSelection.top;
		if (curCell.h < fSelection.left)
			anchorCell.h = fSelection.right;
		else
			anchorCell.h = fSelection.left;

		fSelection.Set(std::min(anchorCell.h, curCell.h),
					   std::min(anchorCell.v, curCell.v),
					   std::max(anchorCell.h, curCell.h),
					   std::max(anchorCell.v, curCell.v));
	}
	else
	{
		anchorCell.h = r.right = r.left = curCell.h;
		anchorCell.v = r.bottom = r.top = curCell.v;
			
		fCurCell = curCell;
		fSelection = r;
	}

	if (!fEntering)
		HiliteSelection(false, true);

	DrawStatus();
	if (updateBorders)
		DrawBorders();

	oldCurCell = curCell;

	ulong buttons;
	GetMouse(&cPoint, &buttons);
	while (buttons)
	{
		(void)GetCellHitBy(cPoint, curCell);

		SelectionToRegion(lastSelectionRgn);

		if (curCell.v == anchorCell.v)
			r.bottom = r.top = curCell.v;
		else if (curCell.v < anchorCell.v)
		{
			r.top = curCell.v;
			r.bottom = anchorCell.v;
		}
		else
		{
			r.bottom = curCell.v;
			r.top = anchorCell.v;
		}

		if (curCell.h == anchorCell.h)
			r.right = r.left = curCell.h;
		else if (curCell.h < anchorCell.h)
		{
			r.left = curCell.h;
			r.right = anchorCell.h;
		}
		else
		{
			r.right = curCell.h;
			r.left = anchorCell.h;
		}

		if (fEntering && ((oldCurCell.v != curCell.v) || (oldCurCell.h != curCell.h)))
			ClearAnts();

		fSelection = r;
		SelectionToRegion(newSelectionRgn);

		if (!fEntering)
			ChangeSelection(&lastSelectionRgn, &newSelectionRgn);

		if (((fCurCell.v != fSelection.top) ||
			(fCurCell.h != fSelection.left)) && !fEntering)
			ClearAnts();
		fCurCell = fSelection.TopLeft();

		if (oldCurCell.v != curCell.v || oldCurCell.h != curCell.h)
		{
			DrawStatus();

			tmpCell = fCurCell;
			fCurCell = curCell;
			if (fEntering)
				fSelection = prevSel;
			AdjustScrollBars();
			ScrollToSelection();
			fSelection = r;
			fCurCell = tmpCell;
		}

		MarchAnts();

		oldCurCell = curCell;
		GetMouse(&cPoint, &buttons);
	}

	if (fEntering)
	{
		ClearAnts();
		fEditBox->EnterRange(fSelection);

		fSelection = prevSel;
		fCurCell = prevCurCell;
	}
	else
		fCurCell = fSelection.TopLeft();

	AdjustScrollBars();
	DrawStatus();
}
 // SelectCell

