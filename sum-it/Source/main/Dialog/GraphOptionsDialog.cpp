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
	ColorPicker
	
	Copyright 1997, Hekkelman Programmatuur
*/

#include "Cell.h"
#include "Range.h"
#include "Graphic.h"
#include "GraphPlugIn.h"
#include "CellView.h"
#include "CellWindow.h"
#include "GraphOptionsDialog.h"
#include "TabSheet.h"
#include "DrawUtils.h"
#include "Utils.h"
#include "Command.h"
#include "GraphicCommands.h"
#include "messages.h"
#include "Formatter.h"
#include "MyError.h"

CGraphOptionsDialog::CGraphOptionsDialog(BRect frame, const char *name, CCellWindow *owner,
		window_type type, int flags)
	: CRDialog(frame, name, owner, type, flags)
{
} /* CGraphOptionsDialog::CGraphOptionsDialog */

CGraphOptionsDialog::~CGraphOptionsDialog()
{
	fOwner->PostMessage(msg_GraphOptionsDialogClosed, fTarget);
	delete fSettings;
} /* CGraphOptionsDialog::~CGraphOptionsDialog */

void CGraphOptionsDialog::PostInit(CGraphPlugIn *target, BMessage *settings)
{
	fTarget = target;
	fSettings = settings;
	fSettings->what = msg_ChartSettings;
	RestoreSettings();
	
	Show();
} /* CGraphOptionsDialog::PostInit */

bool CGraphOptionsDialog::OKClicked()
{
	RecordSettings();
	
	BMessage msg(msg_Command);
	msg.AddPointer("command", new CChartOptionsCommand(fOwner->GetCellView(), *fSettings));
	fOwner->PostMessage(&msg);
	return false;
} /* CGraphOptionsDialog::OKClicked */

bool CGraphOptionsDialog::CancelClicked()
{
	RestoreSettings();
	return false;
} /* CGraphOptionsDialog::CancelClicked */

void CGraphOptionsDialog::RecordSettings()
{
	int cnt = fSettings->CountNames(B_BOOL_TYPE);
	while (cnt--)
	{
		char *name;
		type_code type;
		
		fSettings->GetInfo(B_BOOL_TYPE, cnt, &name, &type);
		fSettings->ReplaceBool(name, IsOn(name));
	}
	
	cnt = fSettings->CountNames(B_STRING_TYPE);
	while (cnt--)
	{
		char *name;
		type_code type;
		
		fSettings->GetInfo(B_STRING_TYPE, cnt, &name, &type);
		fSettings->ReplaceString(name, GetText(name));
	}

	cnt = fSettings->CountNames(B_FLOAT_TYPE);
	while (cnt--)
	{
		char *name;
		type_code type;
		
		fSettings->GetInfo(B_FLOAT_TYPE, cnt, &name, &type);
		fSettings->ReplaceFloat(name, atof_i(GetText(name)));
	}
} /* CGraphOptionsDialog::RecordSettings */

void CGraphOptionsDialog::RestoreSettings()
{
	int cnt = fSettings->CountNames(B_BOOL_TYPE);
	while (cnt--)
	{
		char *name;
		type_code type;
		bool b;
		
		fSettings->GetInfo(B_BOOL_TYPE, cnt, &name, &type);
		fSettings->FindBool(name, &b);
		SetOn(name, b);
	}

	cnt = fSettings->CountNames(B_STRING_TYPE);
	while (cnt--)
	{
		char * name ;
		const char *text;
		type_code type;
		
		fSettings->GetInfo(B_STRING_TYPE, cnt, &name, &type);
		fSettings->FindString(name, &text);
		SetText(name, text);
	}

	cnt = fSettings->CountNames(B_FLOAT_TYPE);
	while (cnt--)
	{
		char *name, number[32];
		type_code type;
		float f;
		
		fSettings->GetInfo(B_FLOAT_TYPE, cnt, &name, &type);
		fSettings->FindFloat(name, &f);
		ftoa(f, number);
		SetText(name, number);
	}
} /* CGraphOptionsDialog::RestoreSettings */

CGraphOptionsDialog* CGraphOptionsDialog::Construct(CGraphPlugIn *target,
	BMallocIO& tmpl, BMessage *settings)
{
	BRect frame;
	char name[256];
	window_type type;
	int flags;
	CCellWindow *owner = (CCellWindow *)target->Parent()->Window();
	
	InitValues(-1, tmpl, frame, name, type, flags, NULL);

	CGraphOptionsDialog* result = new CGraphOptionsDialog(frame, name, owner, type, flags);
	if (result)
	{
		try
		{
			result->ConstructFromTemplate(tmpl);
			result->PostInit(target, settings);
		}
		catch (CErr& e)
		{
			e.DoError();
			delete result;
			result = NULL;
		}
	}
	
	return result;
} /* CGraphOptionsDialog::Construct */

