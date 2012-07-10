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
	PasteFunctionDialog.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "CellView.h"
#include "CellWindow.h"
#include "DrawUtils.h"
#include "PasteFunctionDialog.h"
#include "FunctionUtils.h"
#include "StringTable.h"
#include "Utils.h"
#include "MyTextControl.h"
#include "messages.h"
#include "namestrs.h"
#include "MyError.h"
#include "Globals.h"

#include <cstring>

#include <Autolock.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <ListView.h>


const ulong
	msg_MenuChanged = 'Mchg',
	msg_FuncChanged = 'Fchg';

CPasteFunctionDialog::CPasteFunctionDialog(BRect frame, const char *name, 
		CCellWindow *inWindow, window_type type, int flags)
	: CRDialog(frame, name, inWindow, type, flags)
{
} /* CPasteFunctionDialog::CPasteFunctionDialog */

void CPasteFunctionDialog::PostInit()
{
	fCellView = fOwner->GetCellView();
	
	BAutolock lock(this);
	if (!lock.IsLocked())
		THROW((errLockWindow));

	fList = (BListView *)FindView("list");
	fList->SetInvocationMessage(new BMessage(msg_OK));
	fList->SetSelectionMessage(new BMessage(msg_FuncChanged));
//	fList->SetFont(be_plain_font);

	fFuncDescription = (BTextView *)FindView("desc");

	BMenuField *mf = dynamic_cast<BMenuField *>(FindView("group"));
	fMenu = mf->Menu();
	fMenu->AddItem(new BMenuItem("All", new BMessage(msg_MenuChanged)));
	fMenu->AddSeparatorItem();
	fMenu->AddItem(new BMenuItem("Cell", new BMessage(msg_MenuChanged)));
	fMenu->AddItem(new BMenuItem("Date-Time", new BMessage(msg_MenuChanged)));
	fMenu->AddItem(new BMenuItem("Financial", new BMessage(msg_MenuChanged)));
	fMenu->AddItem(new BMenuItem("Math", new BMessage(msg_MenuChanged)));
	fMenu->AddItem(new BMenuItem("Statistical", new BMessage(msg_MenuChanged)));
	fMenu->AddItem(new BMenuItem("Text", new BMessage(msg_MenuChanged)));
	fMenu->AddItem(new BMenuItem("Logical", new BMessage(msg_MenuChanged)));
	fMenu->AddItem(new BMenuItem("Engineering", new BMessage(msg_MenuChanged)));
	fMenu->AddItem(new BMenuItem("Plugins", new BMessage(msg_MenuChanged)));

	fMenu->FindItem("All")->SetMarked(true);

	FillList();
	
	Show();
} /* CPasteFunctionDialog::CPasteFunctionDialog */

bool CPasteFunctionDialog::OKClicked()
{
	bool result = true;
	BStringItem *item = (BStringItem *)fList->ItemAt(fList->CurrentSelection());
	char *cp;
	
	if (item)
	{
		BMessage msg(msg_FunctionChosen);
		const char *s = item->Text();
		
		msg.AddString("string", item->Text());
		
		if (strstr(s, "()") != NULL)
			;
		else if ((cp = strchr(s, '(')) != NULL)
		{
			int start;
			
			start = cp - s + 1;
			
			int end = 0;
			char *sp;
			
			if ((sp = strchr(cp, gListSeparator)) != NULL)
			{
				end = sp - cp - 1;
			}
			
			if (!end && (sp = strchr(cp, ')')) != NULL)
			{
				end = sp - cp - 1;
			}
			
			if (!end)
				end = strlen(s) - start - 1;
			
			msg.AddInt32("start", start);
			msg.AddInt32("stop", end);
		}

		fOwner->PostMessage(&msg, fOwner->GetEditBox());
	}
	else
		result = false;

	return result;
} /* CPasteFunctionDialog::OKClicked */

void CPasteFunctionDialog::FillList()
{
	int i, grp;
	BListItem *item;

	grp = fMenu->IndexOf(fMenu->FindMarked()) - 1;

	item = fList->LastItem();
	while (item)
	{
		fList->RemoveItem(item);
		delete item;
		item = fList->LastItem();
	}

	for (i = 0; i < gFuncCount; i++)
		if (grp == -1 || gFuncArrayByName[i].groupNr == grp)
		{
			const char *s = gFuncPasteStrings[gFuncArrayByName[i].funcNr];
			
			char *sp = strchr(s, ',');
			while (sp)
			{
				*sp = gListSeparator;
				sp = strchr(sp + 1, ',');
			}
	
			fList->AddItem(new BStringItem(s));
		}
	
	UpdateDesc();
} /* CPasteFunctionDialog::FillList */

void CPasteFunctionDialog::UpdateDesc()
{
	int i;
	
	if ((i = fList->CurrentSelection()) >= 0)
	{
		char s[256], *sp;
		
		strcpy(s, ((BStringItem *)fList->ItemAt(i))->Text());
		sp = strchr(s, '(');
		if (sp) *sp = 0;
		
		i = GetFunctionNr(s);
		strcpy(s, gFuncArrayByNr[i].funcName);
		fFuncDescription->SetText(gFuncDescriptions[i]);
	}
	else
		fFuncDescription->SetText("");
		
} /* CPasteFunctionDialog::UpdateDesc */

void CPasteFunctionDialog::MessageReceived(BMessage *inMessage)
{
	switch (inMessage->what)
	{
		case msg_MenuChanged:
			FillList();
			break;
		
		case msg_FuncChanged:
			UpdateDesc();
			break;
		
		default:
			CDialog::MessageReceived(inMessage);
	}
} /* CPasteFunctionDialog::MessageReceived */

CPasteFunctionDialog* CPasteFunctionDialog::Construct(CCellWindow *inWindow)
{
	BMallocIO buf;
	BRect frame;
	char name[256];
	window_type type;
	int flags;
	
	InitValues(5, buf, frame, name, type, flags, inWindow);
	
	CPasteFunctionDialog* result = new CPasteFunctionDialog(frame, name, inWindow, type, flags);
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
} /* CPasteFunctionDialog::Construct */

