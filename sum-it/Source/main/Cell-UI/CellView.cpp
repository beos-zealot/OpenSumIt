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
	CellView.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   BTREE_T
#include "BTree.t"
#endif

#ifndef   NAMETABLE_H
#include "NameTable.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   SELECTIONVIEW_H
#include "SelectionView.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLSCROLLBAR_H
#include "CellScrollBar.h"
#endif

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
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

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   CELLCOMMANDS_H
#include "CellCommands.h"
#endif

#ifndef   DRAGCOMMAND_H
#include "DragCommand.h"
#endif

#ifndef   TEXT_CELLS_H
#include "Text2Cells.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#ifndef   GRAPHIC_H
#include "Graphic.h"
#endif

#include <Debug.h>
#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   ROUND_H
#include "Round.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif


#include <MenuBar.h>
#include <MenuItem.h>
#include <Message.h>
#include <PrintJob.h>
#include <Autolock.h>

CCellView::CCellView(BRect frame, CTextControl* editBox, bool fromFile)
	: BView(frame, "cells", B_FOLLOW_ALL_SIDES,
		B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS | B_PULSE_NEEDED
//		| B_FULL_UPDATE_ON_RESIZE
		)
	, fSelection(1, 1, 1, 1)
	, fCurCell(1,1)
	, fFrozen(0,0)
	, fBorderWidth(1)
	, fBorderHeight(1)
	, fBorderFontID(0)
	, fCellWidths(kColCount, 10.0)
	, fCellHeights(kRowCount, 10.0)
	, fPosition(2,2)
	, fEditBox(editBox)
	, fHScrollBar(NULL)
	, fVScrollBar(NULL)
	, fCurrentCursor(curNoCursor)
	, fPageSetup(NULL)
	, fPageSetupSize(0)
	, fVPageCount(0)
	, fHPageCount(0)
	, fHPageBreaks(kColCount)
	, fShowGrid(true)
	, fShowBorders(true)
	, fEntering(false)
	, fPrGrid (true)
	, fPrBorders(true)
	, fOnlyTheLines(false)
	, fWindowActive(false)
	, fDragIsAcceptable(false)
	, fFromFile(fromFile)
	, fFindInFormulas(false)
	, fIgnoreCase(true)
	, fWrap(false)
	, fAutoRecalc(true)
	, fShowFormulas(false)
	, fDragACopy(false)
	, fSuppressHighlight(false)
	, fDisplayZero(false)
	, fNew(true)
	, fWhatToFind(NULL)
	, fReplaceWith(NULL)
	, fFOffset(0)
	, fJob(NULL)
	, fFirstGraphic(NULL)
{
	fNames = new CNameTable;
	fContainer = new CContainer(this, fNames);
	fDisplayZero = gPrefs->GetPrefInt("Display Zero", 0);
	fPageSetup = (char *)Hex2Bin(gPrefs->GetPrefString("default page setup", ""), fPageSetupSize);

	if (fPageSetup)
	{
		BPrintJob prJob("setup CellView job");
		BMessage *msg = new BMessage;
		msg->Unflatten(fPageSetup);
		if (msg->what != 0) {
			prJob.SetSettings(msg);
		}
		else {
			fPageSetup = NULL;
		}
		fPaperRect = prJob.PaperRect();
		fMargins = prJob.PrintableRect();
	}
	else
		PRINT(("Dit gaat mis!"));

	Repaginate();
} /* CCellView::CCellView */

CCellView::~CCellView()
{
	CancelCalculation();
	delete fNames;
	fContainer->Release();
	if (fPageSetup) FREE(fPageSetup);
	
	CGraphic *g = fFirstGraphic;
	while (g)
	{
		CGraphic *n = g->Next();
		delete g;
		g = n;
	}
} /* CCellView::~CCellView */

void CCellView::ReInit()
{
	CancelCalculation();
	fNames->clear();
	fContainer->Release();
	fContainer = new CContainer(this, fNames);
} /* CCellView::ReInit */

void CCellView::AttachedToWindow()
{
	font_family bFamily;
	font_style bStyle;
	float bSize;

	be_bold_font->GetFamilyAndStyle(&bFamily, &bStyle);
	bSize = be_bold_font->Size();
	
	fBorderFontID = gFontSizeTable.GetFontID(
		gPrefs->GetPrefString("border font family", bFamily),
		gPrefs->GetPrefString("border font style", bStyle),
		gPrefs->GetPrefDouble("border font size", bSize));

	CFontMetrics *metrics;
	gFontSizeTable.SetFontID(this, fBorderFontID, &metrics);
	
	font_height fi;
	metrics->Font().GetHeight(&fi);
	
	float x = Round(fi.ascent + fi.descent + fi.leading + 2);
	
	BFont f = gFontSizeTable[gFontSizeTable.GetFontID(
		gPrefs->GetPrefString("defdoc font family", bFamily),
		gPrefs->GetPrefString("defdoc font style", bStyle),
		gPrefs->GetPrefDouble("defdoc font size", bSize))].Font();
	f.GetHeight(&fi);
	x = std::max(x, (float)Round(fi.ascent + fi.descent + fi.leading + 1));
	
	fCellHeights.SetValue(1, kRowCount, x);
	
	x = Round(6 * metrics->StringWidth("M"));
	fCellWidths.SetValue(1, kColCount, x);
	
	RecalculateBorderWidths();
	
//	SetViewColor(B_TRANSPARENT_32_BIT);
} /* CCellView::AttachedToWindow */

void CCellView::RecalculateBorderWidths()
{
	CFontMetrics *metrics;
	gFontSizeTable.SetFontID(this, fBorderFontID, &metrics);
	
	font_height fi;
	metrics->Font().GetHeight(&fi);
	
	fBorderWidth = Round(5 * metrics->StringWidth("0") + 9);
	fBorderHeight = Round(fi.ascent + fi.descent + fi.leading + 2);
	
	fCellBounds = Bounds();
	if (fShowBorders)
	{
		fCellBounds.left += fBorderWidth;
		fCellBounds.top += fBorderHeight;
	}
} /* CCellView::RecalculateBorderWidths */

void CCellView::MakeFocus(bool focused)
{
	StPenState save(this);
	
	BView::MakeFocus();

	if (!focused)
	{
		if (GraphicHasFocus())
			fFirstGraphic->MakeFocus(false);
		else
			ClearAnts();
	}
} /* CCellView::MakeFocus */

void CCellView::WindowActivated(bool active)
{
	BView::WindowActivated(active);

	StPenState save(this);

	if (fNew)
	{
		fNew = false;
		Invalidate();
	}

	if (active != fWindowActive)
	{
		ClearAnts();
		fWindowActive = active;
		if (!fEntering)
		{
			SetClip(NULL);
			HiliteSelection(active, false);
			HiliteSelection(!active, true);
		}

		if (fShowBorders)
			DrawBorders();
	}
} /* CCellView::WindowActivated */

void CCellView::AttachSelectionView(CSelectionView* selView)
{
	fSelView = selView;
	selView->SetRange(&fSelection);
} /* CCellView::AttachSelectionView */

void CCellView::AttachProgressView(CProgressView* inProgressView)
{
	fProgressView = inProgressView;
} /* CCellView::AttachProgressView */

bool CCellView::WaitMouseMoved(BPoint where, bool moveIsOK)
{
	BPoint cur;
	uint32 btns;
	bigtime_t breakAt;
	
	get_click_speed(&breakAt);
	breakAt += system_time();
	
	GetMouse(&cur, &btns);
	while (btns)
	{
		if (fabs(cur.x - where.x) > 2.0 ||
			fabs(cur.y - where.y) > 2.0)
			return moveIsOK;
		
		if (system_time() >= breakAt)
			return true;
			
		GetMouse(&cur, &btns);
	}
	
	return false;
} /* CCellView::WaitMouseMoved */

bool CCellView::IsEntering() const
{
	return fEntering;//fEditBox->ChildAt(0)->IsFocus();
} /* CCellView::IsEntering */

void CCellView::SetEntering(bool flag)
{
	fEntering = flag;
	
	if (!flag)
	{
		if (GraphicHasFocus())
			fFirstGraphic->MakeFocus(false);
		else
			ClearAnts();
	}

	BMenuBar *mbar = (BMenuBar *)Window()->FindView("mbar");

	mbar->FindItem(B_COPY)->SetTarget(flag ? (BHandler *)fEditBox : (BHandler *)this);
	mbar->FindItem(B_CUT)->SetTarget(flag ? (BHandler *)fEditBox : (BHandler *)this);
	mbar->FindItem(B_PASTE)->SetTarget(flag ? (BHandler *)fEditBox : (BHandler *)this);

	mbar->FindItem(msg_Clear)->SetEnabled(!flag);
	mbar->FindItem(msg_PasteSpecial)->SetEnabled(!flag);
//	mbar->FindItem(msg_PasteFunction)->SetEnabled(flag);
	mbar->FindItem(msg_InsertRC)->SetEnabled(!flag);
	mbar->FindItem(msg_DeleteRC)->SetEnabled(!flag);
	
	mbar->SubmenuAt(2)->SetEnabled(!flag);

	mbar->FindItem(msg_Recalculate)->SetEnabled(!flag);
	mbar->FindItem(msg_SortCmd)->SetEnabled(!flag);
	mbar->FindItem(msg_FillDown)->SetEnabled(!flag);
	mbar->FindItem(msg_FillRight)->SetEnabled(!flag);
	mbar->FindItem(msg_FillCmd)->SetEnabled(!flag);
	mbar->FindItem(msg_FindCmd)->SetEnabled(!flag);
	mbar->FindItem(msg_FindAgainCmd)->SetEnabled(!flag);
	
	mbar->FindItem(msg_Freeze)->SetEnabled(!flag);
	mbar->FindItem(msg_UnFreeze)->SetEnabled(!flag);
} /* CCellView::SetEntering */

range CCellView::GetNamedRange(const char *inName)
{
	CName name(inName);
	range result = (*fNames)[name];
	return result;
} /* CCellView::GetNamedRange */

void CCellView::AddNamedRange(const char *inName, range inRange)
{
	try
	{
		CName name(inName);
		(*fNames)[name] = inRange;
	}
	catch (CErr& e)
	{
		CATCHED;
		if (e == errDuplicateKey)
		{
			MAlert *alrt = new MWarningAlert("Name already exists.");
			alrt->Go();
			THROW((B_NO_ERROR));
		}
		else
			throw;
	}
} /* CCellView::AddNamedRange */

void CCellView::RemoveNamedRange(const char *inName)
{
	BAutolock lock(fContainer);
	if (fContainer->RefersToNamedRange(inName))
	{
		MAlert *a = new MWarningAlert("This name is in use!");
		a->Go();
		THROW((B_NO_ERROR));
	}
	
	CName name(inName);
	fNames->erase(name);
} /* CCellView::RemoveNamedRange */

bool CCellView::IsNamedRange(const char *inName)
{
	CName name(inName);
	return fNames->count(name);
} /* CCellView::IsNamedRange */

void CCellView::CalculateAllCells(bool override)
{
	if (!fJob && (override || fAutoRecalc))
	{
		fJob = new CCalculateJob(this, fContainer);
		fJob->Run();
	}
} /* CCellView::CalculateAllCells */

void CCellView::CancelCalculation()
{
	if (fJob)
	{
		fJob->Cancel();
		status_t l;
		wait_for_thread(fJob->Thread(), &l);
		fJob = NULL;
	}
} /* CCellView::CancelCalculation */

void CCellView::SetSelection(range r)
{
	StPenState save(this);

	HiliteSelection(!fWindowActive, false);
	
	bool updBorders = fWindowActive &&
		 ((fSelection.top == 1 && fSelection.bottom == kRowCount) ||
		(fSelection.left == 1 && fSelection.right == kColCount) ||
		(r.top == 1 && r.bottom == kRowCount) ||
		(r.left == 1 && r.right == kColCount));

	fSelection = r;

	if (!r.Contains(fCurCell))
	{
		ClearAnts();
		fCurCell = r.TopLeft();
	}
	
	AdjustScrollBars();
	HiliteSelection(!fWindowActive, true);

	if (updBorders && fShowBorders)
		DrawBorders();
	
	DrawStatus();
	ResetOffset(-1);
} /* CCellView::SetSelection */

