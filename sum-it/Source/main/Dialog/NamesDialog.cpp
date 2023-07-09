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
	NamesDialog.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   NAMESDIALOG_H
#include "NamesDialog.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   NAMETABLE_H
#include "NameTable.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   CONFIG_H
#include "Config.h"
#endif


#include <ctype.h>

#include <Debug.h>
#include <Autolock.h>
#include <Message.h>

#include <ListView.h>
#include <Button.h>
#include <TextControl.h>

const uint32
	msg_ListSelect = 'LstS',
	msg_ListInvoke = 'LstI';

CNamesDialog::CNamesDialog(CCellView *inView, range inRange)
	: CDialog(BRect(100.0, 100.0, 350.0, 210.0),
		"Names", (CCellWindow *)inView->Window(),
		BRect(175.0, 77.0, 239.0, 100.0),
		BRect(110.0, 77.0, 168.0, 100.0))
{
	BAutolock lock(this);
	if (!lock.IsLocked())
		return;
	
	fView = inView;
	
	char s[256];

	strcpy(s, "Named Ranges of ");
	strcat(s, fOwner->Title());
	SetTitle(s);

	inRange.GetName(s);
	
	BRect r;
	
	r.Set(10.0, 10.0, 90.0, 100.0);
	BListView *list = AddListView(r, "list");
	list->SetInvocationMessage(new BMessage(msg_ListInvoke));
	list->SetSelectionMessage(new BMessage(msg_ListSelect));

	r.Set(110.0, 10.0, 240.0, 30.0);
	AddTextControl(r, "name", "Name:", "");
	
	r.Set(110.0, 40.0, 240.0, 60.0);
	AddTextControl(r, "range", "Range:", s);
	
	BButton *btn = (BButton *)FindView("ok");
	btn->SetLabel("Add");
	btn = (BButton *)FindView("cancel");
	btn->SetLabel("Remove");
	btn->SetEnabled(false);
	
	FindView("name")->MakeFocus();
	
	Show();
} /* CNamesDialog::CNamesDialog */

bool CNamesDialog::OKClicked()
{
	char s[256];
	range r;
	bool result = true;
	
	strcpy(s, ((BTextControl *)FindView("range"))->Text());
	
	int l = cell::GetCell(s, r.TopLeft());
	if (l)
	{
		char *sp = s + l;
		if (*sp == ':')
			sp++;
		else if (*sp == '.' && sp[1] == '.')
			sp += 2;
		
		l = cell::GetCell(sp, r.BotRight());
		if (l == 0)
			r.BotRight() = r.TopLeft();
		sp += l;
		
		if (*sp != 0)
			result = false;
	}
	else
		result = false;
	
	if (!result)
	{
		MAlert *alrt = new MWarningAlert("Invalid range entered.");
		alrt->Go();
		return false;
	}
	
	strcpy(s, ((BTextControl *)FindView("name"))->Text());
	
	if (strlen(s) == 0)
	{
		MAlert *alrt = new MWarningAlert("No name entered.");
		alrt->Go();
		return false;
	}
	
	for (unsigned int i = 0; i < strlen(s); i++)
	{
		if (!isalnum(s[i]) && s[i] != '_')
		{
			MAlert *alrt = new MWarningAlert("Name may only contain alpha numeric characters.");
			alrt->Go();
			result = false;
		}
	}
	
	if (result)
	{
		BMessage msg(msg_AddName);
		msg.AddData("range", 'rang', &r, sizeof(range));
		msg.AddString("name", s);
		fOwner->PostMessage(&msg, fView);
	}
	
	return false;
} /* CNamesDialog::OKClicked */

bool CNamesDialog::CancelClicked()
{
	BListView *list = (BListView *)FindView("list");
	BStringItem *s = (BStringItem *)list->ItemAt(list->CurrentSelection());

	if (s)
	{
		BMessage msg(msg_RemoveName);
		msg.AddString("name", s->Text());
		fOwner->PostMessage(&msg, fView);
	}

	return false;
} /* CNamesDialog::CancelClicked */

void CNamesDialog::ReloadList()
{
	try
	{
		BListView *list = (BListView *)FindView("list");
		int i;
		
		for (i = 0; i < list->CountItems(); i++)
			delete list->ItemAt(i);
		list->MakeEmpty();
		
		CNameTable& tbl = *fView->GetNameTable();
		namemap::iterator it;
		
		i = 0;
		for (it = tbl.begin(); it != tbl.end(); it++)
			list->AddItem(new BStringItem((*it).first));
		
		list->Invalidate();
	}
	catch (CErr& e)
	{
		CATCHED;
		e.DoError();
	}
} /* CNamesDialog::ReloadList */

void CNamesDialog::SelectListItem()
{
	try
	{
		BListView *list = (BListView *)FindView("list");
		BStringItem *s = (BStringItem *)list->ItemAt(list->CurrentSelection());
		
		if (s)
		{
			CNameTable *tbl = fView->GetNameTable();
			
			BTextControl *ctrl = (BTextControl *)FindView("name");
			ctrl->SetText(s->Text());
			
			char range[64];
			CName name(s->Text());
			(*tbl)[name].GetName(range);
			
			ctrl = (BTextControl *)FindView("range");
			ctrl->SetText(range);
			
			((BControl *)FindView("cancel"))->SetEnabled(true);
		}
		else
		{
			BTextControl *ctrl = (BTextControl *)FindView("name");
			ctrl->SetText("");
			ctrl = (BTextControl *)FindView("range");
			ctrl->SetText("");
		}
	}
	catch(CErr& e)
	{
		CATCHED;
		if (e == errKeyNotFound)
		{
			MAlert *a = new MWarningAlert("That name does not exist anymore?!?!?!?!?!");
			a->Go();
		}
		else
			e.DoError();
	}		
}

void CNamesDialog::WindowActivated(bool active)
{
	if (active)
	{
		ReloadList();
		SetText("name", "");
		
		BMessenger m(fView);
		BMessage msg(msg_GetSelection), reply;
		
		char s[256];
		if (m.SendMessage(&msg, &reply) == B_NO_ERROR)
		{
			range r;
			ssize_t l;
			const void *p;
			if (reply.FindData("selection", 'rang', &p, &l) == B_NO_ERROR)
			{
				memcpy(&r, p, sizeof(range));
				r.GetName(s);
			}
			else
				ASSERT(false);
		}
		else	
			s[0] = 0;
		
		SetText("range", s);
		FindView("name")->MakeFocus();
	}
} /* CNamesDialog::WindowActivated */

void CNamesDialog::MessageReceived(BMessage *inMessage)
{
	switch (inMessage->what)
	{
		case msg_ListInvoke:
		case msg_ListSelect:
			SelectListItem();
			break;
		
		case msg_NameListChange:
			ReloadList();
			break;
		
		case msg_FillInRange:
		{
			char s[256];
			range r;
			ssize_t l;
			const void *p;
			
			if (inMessage->FindData("range", 'rang', &p, &l) == B_NO_ERROR)
				r = *(range *)p;
			else
				ASSERT(false);
			r.GetName(s);
			SetText("range", s);
			SetText("name", "");
			break;
		}
		
		default:
			CDialog::MessageReceived(inMessage);
	}
} /* CNamesDialog::MessageReceived */
