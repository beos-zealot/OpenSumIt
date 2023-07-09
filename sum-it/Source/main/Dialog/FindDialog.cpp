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
	FindDialog.cpp

	Copyright 1997, Hekkelman Programmatuur

	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   CELLCOMMANDS_H
#include "CellCommands.h"
#endif

#ifndef   FINDDIALOG_H
#include "FindDialog.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif


#include <TextControl.h>
#include <Message.h>
#include <cstring>

CFindDialog::CFindDialog(BRect frame, const char *name, CCellWindow *owner,
		window_type type, int flags)
	: CRDialog(frame, name, owner, type, flags)
{
} /* CFindDialog::CFindDialog */

void CFindDialog::PostInit()
{
	if (gPrefs->GetPrefInt("Search In Formulas", 1))
		SetOn("form", true);
	else
		SetOn("val", true);

	BTextControl *sv = (BTextControl *)FindView("replace");
	float w = be_plain_font->StringWidth(sv->Label());
	sv = (BTextControl *)FindView("find");
	if (be_plain_font->StringWidth(sv->Label()) > w)
		w = be_plain_font->StringWidth(sv->Label());
	w += 2;
	sv->SetDivider(w);
	sv = (BTextControl *)FindView("replace");
	sv->SetDivider(w);

	SetOn("ign_case", gPrefs->GetPrefInt("Search Ignore Case", 1));
	SetOn("wrap", gPrefs->GetPrefInt("Search Wrap", 1));

	UpdateFields();
} /* CFindDialog::CFindDialog */

bool CFindDialog::QuitRequested()
{
	Hide();
	return CCellWindow::WindowCount() == 0;
} /* CFindDialog::QuitRequested */

void CFindDialog::DoFind(uint32 cmd)
{
	BMessage msg(cmd);

	msg.AddString("what", GetText("find"));
	msg.AddString("with", GetText("replace"));

	bool b = IsOn("form");
	msg.AddBool("where", b);
	gPrefs->SetPrefInt("Search In Formulas", b ? 1 : 0);

	b = IsOn("wrap");
	msg.AddBool("wrap", b);
	gPrefs->SetPrefInt("Search Wrap", b ? 1: 0);

	b = IsOn("ign_case");
	msg.AddBool("case", b);
	gPrefs->SetPrefInt("Search Ignore Case", b ? 1 : 0);

	CCellWindow *w = CCellWindow::TopWindow();

	if (w)
	{
		w->UpdateIfNeeded();
		w->PostMessage(&msg, w->GetCellView());
	}
} /* CFindDialog::DoFind */

void CFindDialog::WindowActivated(bool active)
{
	CDialog::WindowActivated(active);

	UpdateFields();
	FindView("find")->MakeFocus();
} /* CFindDialog::WindowActivated */

void CFindDialog::UpdateFields()
{
	CCellWindow *w = CCellWindow::TopWindow();

	if (IsOn("form") && w && strlen(GetText("find")))
	{
		BMessage query(msg_QueryCanReplace);
		query.AddString("what", GetText("find"));
		query.AddBool("case", IsOn("ign_case"));
		w->PostMessage(&query, w->GetCellView(), this);
	}
	else
	{
		SetEnabled("repl", false);
		SetEnabled("r&f", false);
	}

	SetEnabled("replace", IsOn("form"));
	SetEnabled("repl_all", IsOn("form"));
} /* CFindDialog::UpdateFields */

void CFindDialog::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case msg_Find:
		case msg_Replace:
		case msg_ReplaceAndFind:
		case msg_ReplaceAll:
			DoFind(msg->what);
			break;

		case msg_ReplyCanReplace:
		{
			bool canReplace;
			if (msg->FindBool("canreplace", &canReplace) == B_NO_ERROR)
			{
				SetEnabled("repl", canReplace);
				SetEnabled("r&f", canReplace);
			}
			break;
		}

		default:
			CDialog::MessageReceived(msg);
			break;
	}
} /* CFindDialog::MessageReceived */

CFindDialog* CFindDialog::Construct()
{
	BMallocIO buf;
	BRect frame;
	char name[256];
	window_type type;
	int flags;

	InitValues(8, buf, frame, name, type, flags, NULL);

	CFindDialog* result = new CFindDialog(frame, name, NULL, type, flags);
	if (result)
	{
		try
		{
			result->ConstructFromTemplate(buf);
			result->PostInit();
		}
		catch (CErr& e)
		{
			e.DoError();
			delete result;
			result = NULL;
		}
	}

	return result;
} /* CFindDialog::Construct */
