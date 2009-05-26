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
	SortDialog.c
	
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

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   SORTDIALOG_H
#include "SortDialog.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   CELLCOMMANDS_H
#include "CellCommands.h"
#endif

#ifndef   SORTCOMMAND_H
#include "SortCommand.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   RES5_H
#include "res5.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif


#include <Autolock.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>


const ulong
	msg_DoSort = 'DSrt',
	msg_OrderChanged = 'Ordr',
	msg_KeyChanged = 'Kchg';

CSortDialog::CSortDialog(BRect frame, const char *name, CCellWindow *owner,
		window_type type, int flags)
	: CRDialog(frame, name, owner, type, flags)
{
} /* CSortDialog::CSortDialog */

void CSortDialog::PostInit()
{
	fView = fOwner->GetCellView();
	fCells = fView->GetContainer();
	fRows = true;
	
	BAutolock lock(this);
	if (!lock.IsLocked())
		THROW((errLockWindow));

	FindView("range")->SetFont(be_bold_font);

	BMenuField *mf = dynamic_cast<BMenuField*>(FindView("Key 1"));
	fKey1 = mf->Menu();
	
	mf = dynamic_cast<BMenuField*>(FindView("Order 1"));
	fAsc1 = mf->Menu();
	fAsc1->AddItem(new BMenuItem(GetIndString(5, dlg_Asc), new BMessage(msg_OrderChanged)));
	fAsc1->AddItem(new BMenuItem(GetIndString(5, dlg_Des), new BMessage(msg_OrderChanged)));
	fAsc1->ItemAt(0)->SetMarked(true);

	mf = dynamic_cast<BMenuField*>(FindView("Key 2"));
	fKey2 = mf->Menu();
	
	mf = dynamic_cast<BMenuField*>(FindView("Order 2"));
	fAsc2 = mf->Menu();
	fAsc2->AddItem(new BMenuItem(GetIndString(5, dlg_Asc), new BMessage(msg_OrderChanged)));
	fAsc2->AddItem(new BMenuItem(GetIndString(5, dlg_Des), new BMessage(msg_OrderChanged)));
	fAsc2->ItemAt(0)->SetMarked(true);

	mf = dynamic_cast<BMenuField*>(FindView("Key 3"));
	fKey3 = mf->Menu();
	
	mf = dynamic_cast<BMenuField*>(FindView("Order 3"));
	fAsc3 = mf->Menu();
	fAsc3->AddItem(new BMenuItem(GetIndString(5, dlg_Asc), new BMessage(msg_OrderChanged)));
	fAsc3->AddItem(new BMenuItem(GetIndString(5, dlg_Des), new BMessage(msg_OrderChanged)));
	fAsc3->ItemAt(0)->SetMarked(true);

	SetOn("byrow", fRows);
	SetOn("bycol", !fRows);

	UpdateFields();

	Show();
} /* CSortDialog::CSortDialog */

void CSortDialog::AddToMenu(BMenu *menu, int from, int to, bool number)
{
	int i;
	char s[64];
	
	for (i = menu->CountItems(); i >= 0; --i)
	{
		BMenuItem *item = menu->RemoveItem(i);
		if (item)
			delete item;
	}
	
	menu->AddItem(new BMenuItem(GetIndString(5, dlg_NoKey), new BMessage(msg_KeyChanged)));
	
	for (i = from; i <= to; i++)
	{
		if (number)
			sprintf(s, "%d", i);
		else
			NumToAString(i, s);
		menu->AddItem(new BMenuItem(s, new BMessage(msg_KeyChanged)));
	}
	
	menu->FindItem(GetIndString(5, dlg_NoKey))->SetMarked(true);
} /* CSortDialog::AddToMenu */

void CSortDialog::UpdateFields()
{
	range r;
	fView->GetSelection(r);
	char s[256], name[64];
	r.GetName(name);
	sprintf(s, GetIndString(5, dlg_SortRange), name);
	SetText("range", s);
	
	if (IsOn("bycol"))
	{
		fRows = false;
		AddToMenu(fKey1, r.top, r.bottom, true);
		AddToMenu(fKey2, r.top, r.bottom, true);
		AddToMenu(fKey3, r.top, r.bottom, true);
		sprintf(s, GetIndString(5, dlg_Key), GetIndString(5, dlg_Col));
		SetText("keydesc", s);
	}
	else
	{
		fRows = true;
		AddToMenu(fKey1, r.left, r.right, false);
		AddToMenu(fKey2, r.left, r.right, false);
		AddToMenu(fKey3, r.left, r.right, false);
		sprintf(s, GetIndString(5, dlg_Key), GetIndString(5, dlg_Row));
		SetText("keydesc", s);
	}
} /* CSortDialog::UpdateFields */

bool CSortDialog::OKClicked()
{
	BMessage msg(msg_Sort);
	int key1, key2, key3;
	bool asc1, asc2, asc3;
	
	range r;
	fView->GetSelection(r);
	
	key1 = fKey1->IndexOf(fKey1->FindMarked());
	if (key1 > 0)
		key1 += (fRows ? r.left : r.top) - 1;
	key2 = fKey2->IndexOf(fKey2->FindMarked());
	if (key2 > 0)
		key2 += (fRows ? r.left : r.top) - 1;
	key3 = fKey3->IndexOf(fKey3->FindMarked());
	if (key3 > 0)
		key3 += (fRows ? r.left : r.top) - 1;
	
	asc1 = fAsc1->IndexOf(fAsc1->FindMarked()) == 0;
	asc2 = fAsc2->IndexOf(fAsc2->FindMarked()) == 0;
	asc3 = fAsc3->IndexOf(fAsc3->FindMarked()) == 0;
	
	msg.AddInt32("key1", key1);
	msg.AddInt32("key2", key2);
	msg.AddInt32("key3", key3);
	msg.AddBool("asc1", asc1);
	msg.AddBool("asc2", asc2);
	msg.AddBool("asc3", asc3);
	msg.AddBool("rows", fRows);
	
	fOwner->PostMessage(&msg, fView);

	return true;
} /* CSortDialog::OKClicked */

CSortDialog* CSortDialog::Construct(CCellWindow *owner)
{
	BMallocIO buf;
	BRect frame;
	char name[256];
	window_type type;
	int flags;
	
	InitValues(10, buf, frame, name, type, flags, owner);
	
	CSortDialog* result = new CSortDialog(frame, name, owner, type, flags);
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
} /* CSortDialog::Construct */

void CSortDialog::WindowActivated(bool state)
{
	BWindow::WindowActivated(state);
	if (state)
		UpdateFields();
} /* CSortDialog::WindowActivated */
