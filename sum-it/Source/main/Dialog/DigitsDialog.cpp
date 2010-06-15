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
	DigitsDialog.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

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

#ifndef   DIGITSDIALOG_H
#include "DigitsDialog.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif


#include <Message.h>
#include <Beep.h>

CDigitsDialog::CDigitsDialog(BRect frame, const char *name, CCellWindow *owner,
		window_type type, int flags)
	: CRDialog(frame, name, owner, type, flags)
{
} /* CDigitsDialog::CDigitsDialog */

void CDigitsDialog::PostInit(int inCurrent)
{
	char s[32];
	sprintf(s, "%d", inCurrent);
	
	SetText("digits", s);
	FindView("digits")->MakeFocus();
	
	Show();
} /* CDigitsDialog::CDigitsDialog */

bool CDigitsDialog::OKClicked()
{
	int digits;
	bool result = true;
	const char *s = GetText("digits");
	
	if (s && sscanf(s, "%d", &digits))
	{
		BMessage msg(msg_FormatDigits);
		msg.AddInt32("digits", digits);
		fOwner->PostMessage(&msg, fOwner->GetCellView());
	}
	else
	{
		beep();
		result = false;
	}
	
	return result;
} /* CDigitsDialog::OKClicked */

CDigitsDialog* CDigitsDialog::Construct(CCellWindow *inWindow, int current)
{
	BMallocIO buf;
	BRect frame;
	char name[256];
	window_type type;
	int flags;
	
	InitValues(6, buf, frame, name, type, flags, inWindow);

	CDigitsDialog* result = new CDigitsDialog(frame, name, inWindow, type, flags);
	if (result)
	{
		try
		{
			result->ConstructFromTemplate(buf);
			result->PostInit(current);
			result->MakeModal();
		}
		catch (CErr& e)
		{
			e.DoError();
			delete result;
			result = NULL;
		}
	}
	
	return result;
} /* CDigitsDialog::Construct */
