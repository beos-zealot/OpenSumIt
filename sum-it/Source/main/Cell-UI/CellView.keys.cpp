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
	CellView.keys.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
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

#ifndef   CELLSCROLLBAR_H
#include "CellScrollBar.h"
#endif

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
#endif

#ifndef   GRAPHICCOMMANDS_H
#include "GraphicCommands.h"
#endif

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#include <ctype.h>
#include <cstring>
#include <Autolock.h>

void CCellView::Step(StepDirection step)
{
	if (fEntering)
		return;

	range r;
	bool rangeSelection;
	BMessage * current = Window()->CurrentMessage();
	uint32 modifiers = current->FindInt32("modifiers");

	StClipCells clip(this);
	ClearAnts();
	ResetOffset(-1);

	rangeSelection = ((fSelection.top != fSelection.bottom) ||
					  (fSelection.left != fSelection.right));

	if (rangeSelection && !(modifiers & B_SHIFT_KEY))
	{
		r = fSelection;

		switch (step)
		{
			case stepUp:
				if (fCurCell.v > r.top)
					fCurCell.v--;
				else
				{
					fCurCell.v = r.bottom;
					if (fCurCell.h > r.left)
						fCurCell.h--;
					else
						fCurCell.h = r.right;
				}
				break;
			case stepDown:
				if (fCurCell.v < r.bottom)
					fCurCell.v++;
				else
				{
					fCurCell.v = r.top;
					if (fCurCell.h < r.right)
						fCurCell.h++;
					else
						fCurCell.h = r.left;
				}
				break;
			case stepLeft:
				if (fCurCell.h > r.left)
					fCurCell.h--;
				else
				{
					fCurCell.h = r.right;
					if (fCurCell.v > r.top)
						fCurCell.v--;
					else
						fCurCell.v = r.bottom;
				}
				break;
			case stepRight:
				if (fCurCell.h < r.right)
					fCurCell.h++;
				else
				{
					fCurCell.h = r.left;
					if (fCurCell.v < r.bottom)
						fCurCell.v++;
					else
						fCurCell.v = r.top;
				}
				break;
		}
	}
	else
	{
		if (!(modifiers & B_SHIFT_KEY))
			HiliteSelection(false, false);
		long span;
		bool wasEmpty = TRUE;
		cell c, anchor;

		if (fCurCell.v == fSelection.bottom)
			anchor.v = fSelection.top;
		else if (fCurCell.v == fSelection.top)
			anchor.v = fSelection.bottom;
		else
			anchor.v = -1;

		if (fCurCell.h == fSelection.right)
			anchor.h = fSelection.left;
		else if (fCurCell.h == fSelection.left)
			anchor.h = fSelection.right;
		else
			anchor.h = -1;

		switch (step)
		{
			case stepUp:
				if (fCurCell.v == 1)
					{}
				else if (modifiers & B_CONTROL_KEY)
				{
					BAutolock lock(fContainer);
					wasEmpty = fContainer->GetType(fCurCell) == eNoData;
					
					c = fCurCell;
					c.v--;
					if (wasEmpty != (fContainer->GetType(c) == eNoData))
						wasEmpty = !wasEmpty;
					
					while (c.v >= 1 &&
						   wasEmpty == (fContainer->GetType(c) == eNoData))
					{
						c.v--;
					} 
					
					if (c.v < fCurCell.v - 1)
						c.v++;

					fCurCell = c;
				}
				else if (modifiers & B_COMMAND_KEY)
				{
					if (fCurCell.v > fPosition.v)
						fCurCell.v = fPosition.v;
					else
					{
						ScrollToPosition(fPosition.h,
							fPosition.v - GetCellSpan(B_VERTICAL, true));
						fCurCell.v = fPosition.v;
					}
					if (fCurCell.v < 1) fCurCell.v = 1;
				}
				else if (fCurCell.v > 1)
					fCurCell.v--;
				break;
			case stepDown:
				if (fCurCell.v == kRowCount)
					{}
				else if (modifiers & B_CONTROL_KEY)
				{
					BAutolock lock(fContainer);
					wasEmpty = fContainer->GetType(fCurCell) == eNoData;
					
					c = fCurCell;
					c.v++;
					if (wasEmpty != (fContainer->GetType(c) == eNoData))
						wasEmpty = !wasEmpty;
					
					while (c.v <= kRowCount &&
						   wasEmpty == (fContainer->GetType(c) == eNoData))
					{
						c.v++;
					} 
					
					if (c.v > fCurCell.v + 1)
						c.v--;

					fCurCell = c;
				}
				else if (modifiers & B_COMMAND_KEY)
				{
					long max;
					
					max = fMaxPosition.v;
					
					span = GetCellSpan(B_VERTICAL, false);
					if (fCurCell.v < fPosition.v - span)
						fCurCell.v = fPosition.v - span;
					else if (fPosition.v - span == max)
						fCurCell.v = kRowCount;
					else
					{
						ScrollToPosition(fPosition.h, fPosition.v - span);
						fCurCell.v -= GetCellSpan(B_VERTICAL, false);
					}
					if (fCurCell.v > kRowCount) fCurCell.v = kRowCount;
				}
				else if (fCurCell.v < kRowCount)
					fCurCell.v++;
				break;
			case stepLeft:
				if (fCurCell.h == 1)
					{}
				else if (modifiers & B_CONTROL_KEY)
				{
					BAutolock lock(fContainer);
					wasEmpty = fContainer->GetType(fCurCell) == eNoData;
					
					c = fCurCell;
					c.h--;
					if (wasEmpty != (fContainer->GetType(c) == eNoData))
						wasEmpty = !wasEmpty;
					
					while (c.h >= 1 &&
						   wasEmpty == (fContainer->GetType(c) == eNoData))
					{
						c.h--;
					} 
					
					if (c.h < fCurCell.h - 1)
						c.h++;

					fCurCell = c;
				}
				else if (modifiers & B_COMMAND_KEY)
				{
					if (fCurCell.h > fPosition.h)
						fCurCell.h = fPosition.h;
					else
					{
						ScrollToPosition(fPosition.h - GetCellSpan(B_HORIZONTAL, true), 
							fPosition.v);
						fCurCell.h = fPosition.h;
					}
					if (fCurCell.h < 1) fCurCell.h = 1;
				}
				else if (fCurCell.h > 1)
					fCurCell.h--;
				break;
			case stepRight:
				if (fCurCell.h == kColCount)
					{}
				else if (modifiers & B_CONTROL_KEY)
				{
					BAutolock lock(fContainer);
					wasEmpty = fContainer->GetType(fCurCell) == eNoData;
					
					c = fCurCell;
					c.h++;
					if (wasEmpty != (fContainer->GetType(c) == eNoData))
						wasEmpty = !wasEmpty;
					
					while (c.h <= kColCount &&
						   wasEmpty == (fContainer->GetType(c) == eNoData))
					{
						c.h++;
					} 
					
					if (c.h > fCurCell.h + 1)
						c.h--;

					fCurCell = c;
				}
				else if (modifiers & B_COMMAND_KEY)
				{
					long max;
					
					max = fMaxPosition.h;
					
					span = GetCellSpan(B_HORIZONTAL, false);
					if (fCurCell.h < fPosition.h - span)
						fCurCell.h = fPosition.h - span;
					else if (fPosition.h - span == max)
						fCurCell.h = kColCount;
					else
					{
						ScrollToPosition(fPosition.h - span, fPosition.v);
						fCurCell.h -= GetCellSpan(B_HORIZONTAL, false);
					}
					if (fCurCell.h > kColCount) fCurCell.h = kColCount;
				}
				else if (fCurCell.h < kColCount)
					fCurCell.h++;
				break;
		}
		
		if (modifiers & B_SHIFT_KEY)
		{
			BRegion oldSel, newSel;
			
			SelectionToRegion(oldSel);
			
			if (anchor.v == -1 && anchor.h == -1)
			{
				fSelection.left = std::min(fSelection.left, fCurCell.h);
				fSelection.right = std::max(fSelection.right, fCurCell.h);
				fSelection.top = std::min(fSelection.top, fCurCell.v);
				fSelection.bottom = std::max(fSelection.bottom, fCurCell.v);
			}
			else if (anchor.v != -1 && anchor.h != -1)
			{
				fSelection.top = std::min(fCurCell.v, anchor.v);
				fSelection.bottom = std::max(fCurCell.v, anchor.v);
				fSelection.left = std::min(fCurCell.h, anchor.h);
				fSelection.right = std::max(fCurCell.h, anchor.h);
			}
			else if (anchor.v == -1)
			{
				fSelection.left = std::min(fCurCell.h, anchor.h);
				fSelection.right = std::max(fCurCell.h, anchor.h);
				fSelection.top = std::min(fSelection.top, fCurCell.v);
				fSelection.bottom = std::max(fSelection.bottom, fCurCell.v);
			}
			else
			{
				fSelection.top = std::min(fCurCell.v, anchor.v);
				fSelection.bottom = std::max(fCurCell.v, anchor.v);
				fSelection.left = std::min(fSelection.left, fCurCell.h);
				fSelection.right = std::max(fSelection.right, fCurCell.h);
			}

			SelectionToRegion(newSel);
			ChangeSelection(&oldSel, &newSel);
		}
		else
		{
			fSelection.Set(fCurCell.h, fCurCell.v, fCurCell.h, fCurCell.v);
			HiliteSelection();
		}
	}

	AdjustScrollBars();
	ScrollToSelection();
	MarchAnts();
	DrawStatus();
} /* Step */

bool CCellView::EnterCell(const char *s, BMessage *inMessage)
{
	bool result = true;

	CancelCalculation();

	if (((CCellWindow *)Window())->
		RegisterCommand(new CEnterCellCommand(this, fContainer, s, fCurCell)))
	{
		MakeFocus();

		SetEntering(false);
		
		uint32 modifiers = inMessage->FindInt32("modifiers");
		
		switch (inMessage->FindInt32("raw_char"))
		{
			case B_TAB:
				if (gPrefs->GetPrefInt("Excel keys") == 0)
				{
					if (modifiers & B_OPTION_KEY)
						Step(stepLeft);
					else
						Step(stepRight);
				}
				break;
				
			case B_ENTER:
				if (gPrefs->GetPrefInt("Excel keys") == 0)
				{
					if (modifiers & B_OPTION_KEY)
						Step(stepUp);
					else
						Step(stepDown);
				}
				break;

			case B_UP_ARROW:
				Step(stepUp);
				break;

			case B_DOWN_ARROW:
				Step(stepDown);
				break;

			case B_LEFT_ARROW:
				Step(stepLeft);
				break;

			case B_RIGHT_ARROW:
				Step(stepRight);
				break;
		}

		((CCellWindow *)Window())->SetDirty(true);
	}
	else
		result = false;

	return result;
} /* CCellView::EnterCell */

void CCellView::RejectCell()
{
	MakeFocus();
	SetEntering(false);
	DrawStatus();
} /* CCellView::RejectCell */

void
CCellView::KeyDown(const char *bytes, int32 numOfBytes)
{
	try
	{
		switch (*bytes)
		{
			case B_ESCAPE:
			{
				range r(fCurCell.h, fCurCell.v, fCurCell.h, fCurCell.v);
				SetSelection(r);
				break;
			}
		
			case B_HOME:
				ScrollToPosition(1, 1);
				break;
		
			case B_LEFT_ARROW:
				Step(stepLeft);
				break;
			
			case B_PAGE_UP:
				ScrollToPosition(fPosition.h,
					fPosition.v - GetCellSpan(B_VERTICAL, true));
				break;
			
			case B_PAGE_DOWN:
				ScrollToPosition(fPosition.h,
					fPosition.v - GetCellSpan(B_VERTICAL, false));
				break;
			
			case B_END:
			{
				float min, max;
				fHScrollBar->GetRange(&min, &max);
				ScrollToPosition(max, fPosition.v);
				break;
			}
			
			case B_TAB:		
			case B_RIGHT_ARROW:
				Step(stepRight);
				break;
			
			case B_UP_ARROW:
				Step(stepUp);
				break;
			
			case B_ENTER:
				fEditBox->MakeFocus();
				break;
				
			case B_DOWN_ARROW:
				Step(stepDown);
				break;
			
			case B_DELETE:
			case B_BACKSPACE:
				if (GraphicHasFocus())
					((CCellWindow *)Window())->RegisterCommand(
						new CDeleteGraphicCommand(this));
				else
					((CCellWindow *)Window())->RegisterCommand(
						new CEraseCommand(this, fContainer));
				((CCellWindow *)Window())->SetDirty(true);
				break;
	
			default:
				if ((*bytes & 0x80) || isprint(*bytes))
				{
					char s[6];
					strncpy(s, bytes, numOfBytes);
					s[numOfBytes] = 0;
					fEditBox->MakeFocus();
					fEditBox->SetText(s);
					fEditBox->Select(numOfBytes, numOfBytes);
				}
		}
	}
	
	catch(CErr& e)
	{
		CATCHED;
		e.DoError();
	}
	
	catch(...)
	{
		CATCHED;
	}
} /* CCellView::KeyDown */
