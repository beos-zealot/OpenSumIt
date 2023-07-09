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
	Dialog.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   DIALOG_H
#include "Dialog.h"
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

#ifndef   MALERT_H
#include "MAlert.h"
#endif


#include <typeinfo>
#include <cstring>

#include <Debug.h>
#include <Application.h>
#include <Autolock.h>

// We use pretty much everything here
#include <InterfaceKit.h>

class CView : public BView {
public:
	CView(BRect frame) : BView(frame, NULL, 0, B_WILL_DRAW) {};
	void MouseMoved(BPoint point, uint32 transit, const BMessage *msg)
	{
		BView::MouseMoved(point, transit, msg);
		be_app->SetCursor(B_HAND_CURSOR);
	} /* CView::MouseMoved */
};

CDialog::CDialog(BRect inFrame, const char *inName,
	CCellWindow *inOwner, BRect okRect, BRect cancelRect,
	bool modal)
	: BWindow(inFrame, inName,
		inName ? B_TITLED_WINDOW : B_MODAL_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	BAutolock lock(this);

	be_app->SetCursor(B_HAND_CURSOR);

	inFrame.OffsetTo(0.0, 0.0);
	fMainView = new CView(inFrame);
	fMainView->SetViewColor(kB_GrayTable[6]);
	AddChild(fMainView);
	
	BButton *btn = new BButton(okRect, "ok", "OK", new BMessage(msg_OK));
	fMainView->AddChild(btn);
	SetDefaultButton(btn);

	btn = new BButton(cancelRect, "cancel", "Cancel", new BMessage(msg_Cancel));
	fMainView->AddChild(btn);
	
	fOwner = inOwner;
	if (fOwner)
		fOwner->AddDialog(this, modal);
} /* CDialog::CDialog */

CDialog::CDialog(BRect inFrame, const char *inName,
	CCellWindow *inOwner, window_type type, int flags)
	: BWindow(inFrame, inName, type, flags)
{
	BAutolock lock(this);

	be_app->SetCursor(B_HAND_CURSOR);

	inFrame.OffsetTo(0.0, 0.0);
	fMainView = new CView(inFrame);
	fMainView->SetViewColor(kB_GrayTable[6]);
	AddChild(fMainView);
	
	fOwner = inOwner;
	if (fOwner)
		fOwner->AddDialog(this, false);
} /* CDialog::CDialog */

CDialog::~CDialog()
{
	if (fOwner && fOwner->Lock())
	{
		fOwner->RemoveDialog(this);
		fOwner->Unlock();
	}
} /* CDialog::~CDialog */
	
BView* CDialog::AddGrayView(BRect inFrame, const char *inName)
{
	BView *result;
	
	result = new BView(inFrame, inName, 0, B_WILL_DRAW | B_NAVIGABLE);
	result->SetViewColor(kB_GrayTable[6]);
	
	fMainView->AddChild(result);
	
	return result;
} /* CDialog::AddGrayView */

BBox* CDialog::AddBox(BRect inFrame, const char *inName, BView *inView)
{
	if (!inView)
		inView = fMainView;

	BBox *box;
	
	box = new BBox(inFrame, inName);
	inView->AddChild(box);
	box->SetFont(be_plain_font);
	
	if (inName)
		box->SetLabel(inName);

	box->SetViewColor(kB_GrayTable[6]);
	box->SetLowColor(kB_GrayTable[6]);
	
	return box;
} /* CDialog::AddBox */

BCheckBox* CDialog::AddCheckBox(BRect inFrame, const char *inName,
	const char *inLabel, int inValue, BView *inView)
{
	if (!inView)
		inView = fMainView;

	BCheckBox *ctrl = new BCheckBox(inFrame, inName, inLabel,
		new BMessage(msg_Changed));
	inView->AddChild(ctrl);
	ctrl->SetValue(inValue);
	
	return ctrl;
} /* CDialog::AddCheckBox */

BRadioButton* CDialog::AddRadioButton(BRect inFrame,
	const char *inName, const char *inLabel,
	int inValue, BView *inView)
{
	if (!inView)
		inView = fMainView;

	BRadioButton *ctrl = new BRadioButton(inFrame, inName, inLabel,
		new BMessage(msg_Changed));
	inView->AddChild(ctrl);
	ctrl->SetValue(inValue);
	
	return ctrl;
} /* CDialog::AddRadioButton */

BTextControl* CDialog::AddTextControl(BRect inFrame,
	const char *inName, const char *inLabel,
	const char *inText, const char *inAllowed,
	int maxLen, BView *inView)
{
	if (!inView)
		inView = fMainView;

	BTextControl *ctrl = new BTextControl(inFrame, inName, inLabel,
		inText, new BMessage(msg_Changed));
	inView->AddChild(ctrl);
	
	BTextView *tv = (BTextView *)ctrl->ChildAt(0);
	if (inAllowed)
	{
		for (int i = 32; i < 256; i++)
			if (strchr(inAllowed, i))
				tv->AllowChar(i);
			else
				tv->DisallowChar(i);
	}
	
	if (maxLen)
		tv->SetMaxBytes(maxLen);
	
	ctrl->SetModificationMessage(new BMessage(msg_Changed));
	
	return ctrl;
} /* CDialog::AddTextControl */

BStringView* CDialog::AddStringView(BRect inFrame,
	const char *inName, const char *inText, BView *inView)
{
	if (!inView)
		inView = fMainView;

	BStringView *ctrl = new BStringView(inFrame, inName, inText);
	inView->AddChild(ctrl);
	ctrl->SetViewColor(inView->ViewColor());
	
	return ctrl;
} /* CDialog::AddStringView */

BTextView* CDialog::AddTextView(BRect inFrame,
	const char *inName, BView *inView)
{
	if (!inView)
		inView = fMainView;

	BRect txtFrame = inFrame;
	txtFrame.top += 2;
	txtFrame.bottom -= 2;
	txtFrame.left += 2;
	txtFrame.right -= 2;

	BTextView *ctrl = new BTextView(inFrame, inName, txtFrame, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW);
	inView->AddChild(ctrl);
	ctrl->SetViewColor(inView->ViewColor());
	ctrl->MakeSelectable(false);
	ctrl->MakeEditable(false);
	
	return ctrl;
} /* CDialog::AddTextView */

BMenuField* CDialog::AddMenuField(BRect inFrame,
	const char *inName, const char *inLabel,
	BMenu *inMenu, BView *inView)
{
	if (!inView)
		inView = fMainView;

	BMenuField *ctrl = new BMenuField(inFrame, inName, inLabel,
		inMenu);
	inView->AddChild(ctrl);
	
	return ctrl;
} /* CDialog::AddMenuField */

BListView* CDialog::AddListView(BRect inFrame,
	const char *inName, BView *inView)
{
	if (!inView)
		inView = fMainView;
	
	BListView *ctrl = new BListView(inFrame, inName);
	ctrl->SetInvocationMessage(new BMessage(msg_OK));
	
	BScrollView *scroller = new BScrollView(NULL, ctrl,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true);
	
	inView->AddChild(scroller);
	
	return ctrl;
} /* CDialog::AddListView */

BMenu* CDialog::AddPopUpMenu(BRect inFrame, const char *inName,
	const char *inLabel, BView *inView)
{
	if (!inView)
		inView = fMainView;
	
	BMenu *menu = new BPopUpMenu(inName);
	
	BView *v = new BMenuField(inFrame, inName, inLabel, menu);
	inView->AddChild(v);
	
	return menu;
} /* CDialog::AddPopUpMenu */

bool CDialog::IsOn(const char *name)
{
	BControl *ctrl;
	ctrl = dynamic_cast<BControl *>(FindView(name));
	if (ctrl)
		return ctrl->Value() == B_CONTROL_ON;
	else
		return false;
} /* CDialog::IsOn */

void CDialog::SetOn(const char *name, bool on)
{
	BControl *ctrl;
	ctrl = dynamic_cast<BControl *>(FindView(name));
	if (ctrl)
	{
		ctrl->SetValue(on ? B_CONTROL_ON : B_CONTROL_OFF);
	}
} /* CDialog::SetOn */
	
void CDialog::MessageReceived(BMessage *inMessage)
{
	switch (inMessage->what)
	{
		case msg_OK:
			if (OKClicked())
				Close();
			break;
		
		case msg_Changed:
			UpdateFields();
			break;
			
		case msg_Cancel:
			if (CancelClicked())
				Close();
			break;
			
		default:
			BWindow::MessageReceived(inMessage);
	}
} /* CDialog::MessageReceived */

bool CDialog::OKClicked()
{
	return true;
} /* CDialog::OKClicked */

bool CDialog::CancelClicked()
{
	return true;
} /* CDialog::CancelClicked */

void CDialog::UpdateFields()
{

} /* CDialog::UpdateFields */

void CDialog::WindowActivated(bool active)
{
	BWindow::WindowActivated(active);
	if (active)
		be_app->SetCursor(B_HAND_CURSOR);
} /* CDialog::WindowActivated */

void CDialog::SetText(const char *name, const char *text)
{
	BView *c = FindView(name);
	if (!c)
	{
		ASSERT(false);
		return;
	}

	if (typeid(*c) == typeid(BTextControl))
	{
		BTextControl *tc = dynamic_cast<BTextControl*>(c);
		if (strcmp(tc->Text(), text))
		{
			tc->SetText(text);
			if (c->IsFocus())
				tc->TextView()->SelectAll();
		}
	}
	else if (typeid(*c) == typeid(BStringView))
		((BStringView *)c)->SetText(text);

} /* CDialog::SetText */

const char* CDialog::GetText(const char *name)
{
	BTextControl *t = dynamic_cast<BTextControl *>(FindView(name));
	if (t)
	{
		return t->Text();
	}
	else
		return NULL;
} /* CDialog::GetText */

void CDialog::SetEnabled(const char *name, bool enabled)
{
	BControl *ctl;
	ctl = dynamic_cast<BControl*>(FindView(name));
	if (ctl)
		ctl->SetEnabled(enabled);
} /* CDialog::SetEnabled */

bool CDialog::GetDouble(const char *name, double *d)
{
	*d = atof_i(GetText(name));
	
	if (isnan(*d))
	{
		MAlert *a = new MWarningAlert("Invalid number entered");
		a->Go();
		FindView(name)->MakeFocus(true);
		return false;
	}

	return true;
} /* CDialog::GetDouble */

void CDialog::MakeModal()
{
	ASSERT(fOwner);
	fOwner->MakeModal(this);
} /* CDialog::MakeModal */
