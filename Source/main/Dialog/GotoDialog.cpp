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
	GotoDialog.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   GOTODIALOG_H
#include "GotoDialog.h"
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

#ifndef   BTREE_T
#include "BTree.t"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif


#include <ctype.h>
#include <Message.h>

CGoToDialog::CGoToDialog(BRect frame, const char *name, CCellWindow *owner,
		window_type type, int flags)
	: CRDialog(frame, name, owner, type, flags)
{
} /* CGoToDialog::CGoToDialog */

void CGoToDialog::PostInit()
{
	FindView("goto")->MakeFocus();
	
	Show();
} /* CGoToDialog::CGoToDialog */

bool CGoToDialog::OKClicked()
{
	char s[256];
	range r;
	bool result = false, isRect = true;
	CCellView *v = fOwner->GetCellView();
	
	strcpy(s, GetText("goto"));
	
	if (v->IsNamedRange(s))
	{
		result = true;
		isRect = false;
	}
	
	if (!result)
	{
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
			
			if (*sp == 0)
				result = true;
		}
	}
	
	if (!result)
	{
		MAlert *alrt = new MWarningAlert("Invalid range entered.");
		alrt->Go();
		return false;
	}
	
	if (result)
	{
		BMessage msg(msg_GoTo);
		
		if (isRect)
			msg.AddData("range", 'data', &r, sizeof(r));
		else
			msg.AddString("name", s);
		
		fOwner->PostMessage(&msg, v);
	}
	
	return true;
} /* CGoToDialog::OKClicked */

CGoToDialog* CGoToDialog::Construct(CCellWindow *inWindow)
{
	BMallocIO buf;
	BRect frame;
	char name[256];
	window_type type;
	int flags;
	
	InitValues(7, buf, frame, name, type, flags, inWindow);

	CGoToDialog* result = new CGoToDialog(frame, name, inWindow, type, flags);
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
} /* CGoToDialog::Construct */
