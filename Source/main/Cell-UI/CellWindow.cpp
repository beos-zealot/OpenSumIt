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
	CellWindow.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLSCROLLBAR_H
#include "CellScrollBar.h"
#endif

#ifndef   SELECTIONVIEW_H
#include "SelectionView.h"
#endif

#ifndef   PROGRESSVIEW_H
#include "ProgressView.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
#endif

#ifndef   RESOURCEMANAGER_H
#include "ResourceManager.h"
#endif

#ifndef   COLORMENUITEM_H
#include "ColorMenuItem.h"
#endif

#ifndef   CELLPARSER_H
#include "CellParser.h"
#endif

#ifndef   ANIMATEGRAPHDIALOG_H
#include "AnimateGraphDialog.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   MTHREAD_H
#include "MThread.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   RES5_H
#include "res5.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif

#ifndef   CELL_H
#include "Cell.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#include <algo.h>

#include <support/Debug.h>

#include <Application.h>
#include <MessageFilter.h>

#include <MenuBar.h>
#include <ScrollView.h>
#include <PictureButton.h>
#include <Bitmap.h>
#include <Screen.h>
#include <FilePanel.h>

#include <Entry.h>

BList 	CCellWindow::sWindowList;
long	CCellWindow::sUntitledCount = 1;

class CCellWindowMessageFilter : public BMessageFilter {
public:
		CCellWindowMessageFilter(ulong inMessage);

virtual filter_result Filter(BMessage *message, BHandler **target);
};

void CCellWindow::GetUntitledName(char *newName)
{
	sprintf(newName, GetIndString(5, str_Untitled), atomic_add(&sUntitledCount, 1));
} /* CCellWindow::GetUntitledName */

CCellWindow::CCellWindow()
	: BWindow(GetFrameRect(), "sum-it window", B_DOCUMENT_WINDOW, 0)
	, fWindowModal(NULL)
	, fEntry(NULL)
	, fDirty(false)
	, fWaitForSave(false)
	, fFileMenu(NULL)
	, fEditMenu(NULL)
	, fFormatMenu(NULL)
	, fFontMenu(NULL)
	, fStyleMenu(NULL)
	, fSizeMenu(NULL)
	, fTextColorMenu(NULL)
	, fCellColorMenu(NULL)
	, fDataMenu(NULL)
	, fGraphMenu(NULL)
	, fWindowMenu(NULL)
	, fHelpMenu(NULL)
	, fEditBox(NULL)
	, fCells(NULL)
	, fShowingMenus(false)
	, fUndoMenuItem(NULL)
	, fRedoMenuItem(NULL)
	, fSavePanel(NULL)
{
	try
	{
		Lock();
	
		char title[32];
		GetUntitledName(title);
		SetTitle(title);

		InitWindow();
	
		Unlock();
		Show() ;
	}
	catch(CErr& e)
	{
		CATCHED;
		e.DoError();
		Lock();
		Quit();
	}
}

CCellWindow::CCellWindow(const entry_ref& inRef)
	: BWindow(GetFrameRect(), "Sum-It", B_DOCUMENT_WINDOW, 0)
{
	fWaitForSave = false;
	fWindowModal = NULL;
	fShowingMenus = false;
	fSavePanel = NULL;
	fEntry = new entry_ref(inRef);

	Lock();

	InitWindow();
		
	try
	{
		SetTitle(inRef.name);
		this->PostMessage(msg_Read);
	}
	catch (CErr& e)
	{
		CATCHED;
		e.DoError();
		PostMessage(B_QUIT_REQUESTED);
	}

	Run();
	Unlock();
} /* CCellWindow::CCellWindow */

CCellWindow::~CCellWindow()
{
	sWindowList.RemoveItem(this);

	if (fSavePanel)
		delete fSavePanel;
	
	for (int i = 0; i < fDialogs.CountItems(); i++)
		((BWindow *)fDialogs.ItemAt(i))->PostMessage(B_QUIT_REQUESTED);
	
	for (int i = 0; i < fGraphs.CountItems(); i++)
		((BWindow *)fGraphs.ItemAt(i))->PostMessage(B_QUIT_REQUESTED);
	
	while (fCommands.Top())
		delete (CCommand *)fCommands.Pop();
	
	while (fUndoneCommands.Top())
		delete (CCommand *)fUndoneCommands.Pop();
	
	be_app->PostMessage(msg_WindowRemoved);
}

void CCellWindow::InitWindow()
{
	BRect bounds(Bounds());
	sWindowList.AddItem(this);
	be_app->PostMessage(msg_WindowAdded);

	fDirty = FALSE;

	BRect mbarRect = bounds;
	mbarRect.bottom = mbarRect.top + 20.0;
	float mbarBottom = GetMBarHeight();

	font_height fi;
	be_plain_font->GetHeight(&fi);

	BRect editRect = bounds;
	editRect.left = 48.0;
	editRect.top = mbarBottom + 1;
	editRect.bottom = ceil(editRect.top + fi.ascent + fi.descent + fi.leading + 3);
	float editBoxBottom = max_c(editRect.bottom, mbarBottom + 15);
	
	C3DBoxje *b = new C3DBoxje(editRect);
	AddChild(b);
	
	editRect.OffsetTo(0.0, 0.0);
	editRect.InsetBy(1.0, 1.0);
	editRect.top += 1.0;

	fEditBox = new CTextControl(editRect);
	fEditBox->SetText("");

	BScrollView *scr = new BScrollView("scroller", fEditBox, 0,
		B_WILL_DRAW | B_NAVIGABLE, false, false, B_NO_BORDER);
	fEditBox->MakeResizable(scr);

	b->AddChild(scr);

	fEditBox->SetMaxBytes(255);

	BRect r(0, mbarBottom + 1, 15, mbarBottom + 15);
	BuildButton(r, "accept", msg_TextEntered, 400, 402);
	r.OffsetBy(16.0, 0.0);
	BuildButton(r, "reject", msg_TextCancelled, 401, 403);
	r.OffsetBy(16.0, 0);
	BuildButton(r, "sum", msg_SumButton, 404, 405);
	
	BRect cellsRect(Bounds());
	cellsRect.top += editBoxBottom + 1;
	cellsRect.right -= B_V_SCROLL_BAR_WIDTH;
	cellsRect.bottom -= B_H_SCROLL_BAR_HEIGHT;

	fCells = new CCellView(cellsRect, fEditBox, fEntry != NULL);
	AddChild(fCells);

	r = Bounds();
	r.top = r.bottom - B_H_SCROLL_BAR_HEIGHT + 1.0;
	r.left = -1.0;
	r.bottom++;
	r.right = 101.0;
	AddChild(new CSelectionView(fCells, r));

	r.OffsetBy(102.0, 0);
	AddChild(new CProgressView(fCells, r));
	
	r.left = r.right;
	r.right = Bounds().right - B_V_SCROLL_BAR_WIDTH + 1.0;
	CCellScrollBar *hBar = new CCellScrollBar(r, "hScroll",
		fCells, 0, kColCount, B_HORIZONTAL);
	AddChild(hBar);
	
	r = Bounds();
	r.top += editBoxBottom + 1;
	r.left = r.right - B_V_SCROLL_BAR_WIDTH + 1.0;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT - 1;
	r.right++;
	CCellScrollBar *vBar = new CCellScrollBar(r, "vScroll",
		fCells, 0, kRowCount, B_VERTICAL);
	AddChild(vBar);
	
	fCells->AdjustScrollBars();

	BMenuBar *mbar = BuildMenu(mbarRect, 1);
	FailNil(mbar);
	AddChild(mbar);
	
	mbar->SetTargetForItems(this);
	
	fFileMenu = mbar->SubmenuAt(0);
	fEditMenu = mbar->SubmenuAt(1);
	fFormatMenu = mbar->SubmenuAt(2);
	fDataMenu = mbar->SubmenuAt(3);
	fGraphMenu = mbar->SubmenuAt(4);
	fWindowMenu = mbar->SubmenuAt(5);
	fHelpMenu = mbar->SubmenuAt(6);
	
	fFontMenu = fFormatMenu->SubmenuAt(0);
	fStyleMenu = fFormatMenu->SubmenuAt(1);
	fSizeMenu = fFormatMenu->SubmenuAt(2);
	fTextColorMenu = fFormatMenu->SubmenuAt(3);
	fCellColorMenu = fFormatMenu->SubmenuAt(4);

	fFileMenu->FindItem(msg_NewWindow)->SetTarget(be_app);
	fFileMenu->FindItem(msg_OpenWindow)->SetTarget(be_app);
	
	fUndoMenuItem = fEditMenu->ItemAt(0);
	fUndoMenuItem->SetEnabled(FALSE);
	fRedoMenuItem = fEditMenu->ItemAt(1);
	fRedoMenuItem->SetEnabled(FALSE);
	fEditMenu->SetTargetForItems(fCells);

	for (long i = 0; i < count_font_families(); i++)
	{
		font_family family;
		get_font_family(i, &family);
		fFontMenu->AddItem(new BMenuItem(family, new BMessage(msg_FontSelected)));
	}
	fFontMenu->SetRadioMode(TRUE);
	fFontMenu->SetTargetForItems(fCells);
	
	fSizeMenu->SetRadioMode(TRUE);
	fSizeMenu->SetTargetForItems(fCells);
	
	fTextColorMenu->SetTargetForItems(fCells);
	fTextColorMenu->SetRadioMode(TRUE);
	
	fCellColorMenu->SetTargetForItems(fCells);
	fCellColorMenu->SetRadioMode(TRUE);
	
	fFormatMenu->SetTargetForItems(fCells);
	
	fDataMenu->SetTargetForItems(fCells);
	
	fGraphMenu->SetTargetForItems(fCells);

	fWindowMenu->SetTargetForItems(fCells);
	fHelpMenu->FindItem(B_ABOUT_REQUESTED)->SetTarget(be_app);
	fHelpMenu->FindItem(msg_Help)->SetTarget(be_app);
//	fWindowMenu->FindItem(msg_PrefsCmd)->SetTarget(be_app);
	fWindowMenu->FindItem(msg_ShowClipboard)->SetTarget(be_app);

	fEditBox->AddFilter(new CCellWindowMessageFilter(B_KEY_DOWN));
	fEditBox->AddFilter(new CCellWindowMessageFilter(B_MOUSE_DOWN));
	fEditBox->SetCellView(fCells);
	
	fCells->MakeFocus();

	BMessage *msg = new BMessage(msg_NavigateKey);
	msg->AddInt32("raw_char", B_UP_ARROW);
	msg->AddInt32("modifiers", B_COMMAND_KEY);
	AddShortcut(B_UP_ARROW, B_COMMAND_KEY, msg, fCells);

	msg = new BMessage(msg_NavigateKey);
	msg->AddInt32("raw_char", B_DOWN_ARROW);
	msg->AddInt32("modifiers", B_COMMAND_KEY);
	AddShortcut(B_DOWN_ARROW, B_COMMAND_KEY, msg, fCells);

	msg = new BMessage(msg_NavigateKey);
	msg->AddInt32("raw_char", B_LEFT_ARROW);
	msg->AddInt32("modifiers", B_COMMAND_KEY);
	AddShortcut(B_LEFT_ARROW, B_COMMAND_KEY, msg, fCells);

	msg = new BMessage(msg_NavigateKey);
	msg->AddInt32("raw_char", B_RIGHT_ARROW);
	msg->AddInt32("modifiers", B_COMMAND_KEY);
	AddShortcut(B_RIGHT_ARROW, B_COMMAND_KEY, msg, fCells);
	
	msg = new BMessage(msg_NavigateKey);
	msg->AddInt32("raw_char", B_UP_ARROW);
	msg->AddInt32("modifiers", B_COMMAND_KEY | B_SHIFT_KEY);
	AddShortcut(B_UP_ARROW, B_COMMAND_KEY | B_SHIFT_KEY, msg, fCells);

	msg = new BMessage(msg_NavigateKey);
	msg->AddInt32("raw_char", B_DOWN_ARROW);
	msg->AddInt32("modifiers", B_COMMAND_KEY | B_SHIFT_KEY);
	AddShortcut(B_DOWN_ARROW, B_COMMAND_KEY | B_SHIFT_KEY, msg, fCells);

	msg = new BMessage(msg_NavigateKey);
	msg->AddInt32("raw_char", B_LEFT_ARROW);
	msg->AddInt32("modifiers", B_COMMAND_KEY | B_SHIFT_KEY);
	AddShortcut(B_LEFT_ARROW, B_COMMAND_KEY | B_SHIFT_KEY, msg, fCells);

	msg = new BMessage(msg_NavigateKey);
	msg->AddInt32("raw_char", B_RIGHT_ARROW);
	msg->AddInt32("modifiers", B_COMMAND_KEY | B_SHIFT_KEY);
	AddShortcut(B_RIGHT_ARROW, B_COMMAND_KEY | B_SHIFT_KEY, msg, fCells);
	
	SetSizeLimits(300.0, 10000.0, 100.0, 100000.0);
}

void CCellWindow::BuildButton(BRect inFrame, const char *name,
	ulong message, int normalID, int pushedID)
{
	BRect b(0.0, 0.0, 15.0, 15.0);
	BBitmap *bmp = new BBitmap(b, B_COLOR_8_BIT);
	BView *tmp = new BView(b, NULL, 0, 0);
	
	AddChild(tmp);
	
	const void * data = gResourceManager.LoadMiniIcon(normalID);
	if (!data) THROW((errResNotFound));
	bmp->SetBits(data, 16 * 16, 0, B_COLOR_8_BIT);

	BPicture *on;
	tmp->BeginPicture(new BPicture);
	tmp->DrawBitmap(bmp);
	on = tmp->EndPicture();

	data = gResourceManager.LoadMiniIcon(pushedID);
	if (!data) THROW((errResNotFound));
	bmp->SetBits(data, 16 * 16, 0, B_COLOR_8_BIT);

	BPicture *off;
	tmp->BeginPicture(new BPicture);
	tmp->DrawBitmap(bmp);
	off = tmp->EndPicture();
	
	AddChild(new BPictureButton(inFrame, name, on, off, new BMessage(message)));
	
	RemoveChild(tmp);
	delete tmp;
} /* CCellWindow::BuildButton */

bool
CCellWindow::QuitRequested()
{
	bool result = true;
	fWaitForSave = false;
	
	if (fDirty)
	{
		char title[256];
		sprintf(title, GetMessage(msgSaveChanges), Title());
		
		MInfoAlert alert(title,
			GetMessage(msgSave),
			GetMessage(msgCancel),
			GetMessage(msgDontSave));
		
		switch (alert.Go())
		{
			case 3:
				break;
				
			case 2:
				result = FALSE;
				break;
				
			default:
				if (fEntry)
					Save();
				else
				{
					result = false;
					fWaitForSave = true;
					SaveAs();
				}
				break;
		}
	}
	 
	return (result);
}

BRect CCellWindow::GetFrameRect()
{
	BRect frame, r;
	{
		BScreen screen;
		frame = screen.Frame();
	}
	
	r.right = 0.75 * frame.Width();
	r.bottom = 0.75 * frame.Height();
	r.left = frame.left + 14;
	r.top = frame.top + 27;
	
	long	numWindows = sWindowList.CountItems();
	long t = (long)min(frame.Width() - 14, frame.Height() - 27) / 60;
	float offset = (numWindows % t) * 15;

	r.OffsetBy(offset, offset);

	return r;
} /* CCellWindow::GetFrameRect */

CCellWindow* CCellWindow::FindWindow(const entry_ref& inRef)
{
	CCellWindow *window = NULL;
	for ( long i = 0; 
		  (window = (CCellWindow *)sWindowList.ItemAt(i)) != NULL; 
		  i++)
	{
		if (window->fEntry && inRef == *window->fEntry)
			return (window);
	}
	
	return (NULL);
} /* CCellWindow::FindWindow */


void CCellWindow::SetDirty(bool inDirty)
{
	fDirty = inDirty;
}


void CCellWindow::WindowActivated(bool active)
{
	BWindow::WindowActivated(active);

	SetPulseRate(50000);
	
	if (active && fWindowModal)
		fWindowModal->Activate(true);
	
	if (active)
	{
		sWindowList.RemoveItem(this);
		sWindowList.AddItem(this, 0);
	}
} /* CCellWindow::WindowActivated */

// For Encapsulation porpoises
void * CCellWindow::GetCellFormula( const cell& cell )
{
	return fCells->GetContainer()->GetCellFormula(cell) ;
}

//#pragma mark -

CCellWindowMessageFilter::CCellWindowMessageFilter(ulong inMessage)
	: BMessageFilter(B_ANY_DELIVERY, B_ANY_SOURCE, inMessage)
{
} /* CCellWindowMessageFilter::CCellWindowMessageFilter */

filter_result CCellWindowMessageFilter::Filter(
	BMessage *message, BHandler **target)
{
	CTextControl* editBox = dynamic_cast<CTextControl*>(*target);
	CCellView* cellView = dynamic_cast<CCellView*>(*target);
	CCellWindow* window = dynamic_cast<CCellWindow*>(*target);

	filter_result result = B_DISPATCH_MESSAGE;

	long key, modifiers;
	message->FindInt32("raw_char", &key);
	message->FindInt32("modifiers", &modifiers);
	modifiers &= ~B_NUM_LOCK;
	
	if (editBox != NULL)
	{
		window = (CCellWindow *)editBox->Window();
		
		if (message->what == B_MOUSE_DOWN)
			window->GetCellView()->SetEntering(true);

		switch (key)
		{
			case B_ENTER:
			case B_TAB:
				message->what = msg_TextEntered;
				*target = window;
				break;
			
			case B_ESCAPE:
				message->what = msg_TextCancelled;
				*target = window;
				break;

			case B_UP_ARROW:
			case B_DOWN_ARROW:
			case B_LEFT_ARROW:
			case B_RIGHT_ARROW:
				if (gPrefs->GetPrefInt("Excel keys", 0) && modifiers == 0)
				{
					message->what = msg_TextEntered;
					*target = window;
				}
				break;
		}
	}
	else if (cellView != NULL || window != NULL)
	{
		if (!window)
			window = (CCellWindow *)cellView->Window();
		
		switch (key)
		{
			case B_UP_ARROW:
			case B_DOWN_ARROW:
			case B_LEFT_ARROW:
			case B_RIGHT_ARROW:
				if (message->FindInt32("modifiers") & B_COMMAND_KEY &&
					!cellView->IsEntering())
				{
					message->what = msg_NavigateKey;
					*target = cellView;
				}
				break;
		}
	}

	return result;
} /* CCellWindowMessageFilter::Filter */

