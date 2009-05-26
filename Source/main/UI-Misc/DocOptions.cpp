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
	DocOptions.c
	
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

#ifndef   DOCOPTIONS_H
#include "DocOptions.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   CELLSTYLE_H
#include "CellStyle.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif


#include <Autolock.h>

#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <Button.h>

CDocOptions::CDocOptions(BRect frame, const char *name, CCellWindow *owner,
	window_type type, int flags)
	: CRDialog(frame, name, owner, type, flags)
{
} /* CDocOptions::CDocOptions */

void CDocOptions::PostInit(CCellWindow *inWindow)
{
	BAutolock lock(this);

	char s[256];
	sprintf(s, Title(), inWindow->Title());
	SetTitle(s);
	
	fView = inWindow->GetCellView();
	
	CellStyle cs;
	font_family fFamily;
	font_style fStyle;
	float fSize;

	fView->GetContainer()->GetDefaultCellStyle(cs);
	try {
		gFontSizeTable.GetFontInfo(cs.fFont, &fFamily, &fStyle, &fSize);
	}
	catch (CErr& e) {
		e.DoError();
	}

	BMenuField *mf = dynamic_cast<BMenuField*>(FindView("docfont"));
	fDocFont = mf->Menu();
	for (long i = 0; i < count_font_families(); i++)
	{
		font_family fontFamily;
		get_font_family(i, &fontFamily);
		fDocFont->AddItem(new BMenuItem(fontFamily, new BMessage(msg_Changed)));
		if (strcmp(fontFamily, fFamily) == 0)
			fDocFont->ItemAt(i)->SetMarked(true);
	}
	fDocFont->SetRadioMode(TRUE);

	ftoa(fSize, s);
	SetText("docsize", s);
	
	try
	{
		gFontSizeTable.GetFontInfo(fView->BorderFontID(),
			&fFamily, &fStyle, &fSize);
	}
	catch (CErr& e)
	{
		e.DoError();
	}

	mf = dynamic_cast<BMenuField*>(FindView("borderfont"));
	fBorderFont = mf->Menu();
	for (long i = 0; i < count_font_families(); i++)
	{
		font_family fontFamily;
		get_font_family(i, &fontFamily);
		fBorderFont->AddItem(new BMenuItem(fontFamily, new BMessage(msg_Changed)));
		if (strcmp(fontFamily, fFamily) == 0)
			fBorderFont->ItemAt(i)->SetMarked(true);
	}
	fBorderFont->SetRadioMode(TRUE);

	ftoa(fSize, s);
	SetText("bordersize", s);
	
	SetOn("autorecalc", fView->DoesAutoRecalc());
	SetOn("displayzero", fView->DoesDisplayZero());
	
	BButton *b = (BButton *)FindView("ok");
	b->SetEnabled(false);
	b = (BButton *)FindView("cancel");
	b->SetEnabled(false);

	Show();
} /* CDocOptions::CDocOptions */

bool CDocOptions::OKClicked()
{
	BMessage msg(msg_WindowOptions);

	const char *fn;
	float fs;
	
	fs = atof_i(GetText("docsize"));
	fn = fDocFont->FindMarked()->Label();
	
	msg.AddString("docfamily", fn);
	msg.AddString("docstyle", "Roman");
	msg.AddFloat("docsize", fs);

	fs = atof_i(GetText("bordersize"));
	fn = fBorderFont->FindMarked()->Label();
	
	msg.AddString("borderfamily", fn);
	msg.AddString("borderstyle", "Bold");
	msg.AddFloat("bordersize", fs);
	
	msg.AddBool("autorecalc", IsOn("autorecalc"));
	msg.AddBool("displayzero", IsOn("displayzero"));

	fOwner->PostMessage(&msg, fView);
	
	BButton *b = (BButton *)FindView("ok");
	b->SetEnabled(false);
	b = (BButton *)FindView("cancel");
	b->SetEnabled(false);

	return false;
} /* CDocOptions::OKClicked */

bool CDocOptions::CancelClicked()
{
	CellStyle cs;
	font_family fFamily, fontFamily;
	font_style fStyle;
	float fSize;
	char s[256];

	fView->GetContainer()->GetDefaultCellStyle(cs);
	gFontSizeTable.GetFontInfo(cs.fFont, &fFamily, &fStyle, &fSize);

	for (long i = 0; i < count_font_families(); i++)
	{
		get_font_family(i, &fontFamily);
		if (strcmp(fontFamily, fFamily) == 0)
			fDocFont->ItemAt(i)->SetMarked(true);
	}

	ftoa(fSize, s);
	SetText("docsize", s);
	
	gFontSizeTable.GetFontInfo(fView->BorderFontID(),
		&fFamily, &fStyle, &fSize);

	for (long i = 0; i < count_font_families(); i++)
	{
		get_font_family(i, &fontFamily);
		if (strcmp(fontFamily, fFamily) == 0)
			fBorderFont->ItemAt(i)->SetMarked(true);
	}

	ftoa(fSize, s);
	SetText("bordersize", s);
	
	SetOn("autorecalc", fView->DoesAutoRecalc());
	SetOn("displayzero", fView->DoesDisplayZero());

	BButton *b = (BButton *)FindView("ok");
	b->SetEnabled(false);
	b = (BButton *)FindView("cancel");
	b->SetEnabled(false);

	return false;
} /* CDocOptions::CancelClicked */

CDocOptions* CDocOptions::Construct(CCellWindow *inWindow)
{
	BMallocIO buf;
	BRect frame;
	char name[256];
	window_type type;
	int flags;
	
	InitValues(1, buf, frame, name, type, flags, inWindow);
	
	CDocOptions* result = new CDocOptions(frame, name, inWindow, type, flags);
	if (result)
	{
		result->ConstructFromTemplate(buf);
		result->PostInit(inWindow);
	}
	
	return result;
} /* CDocOption::Construct */

void CDocOptions::UpdateFields()
{
	BButton *b = (BButton *)FindView("ok");
	b->SetEnabled(true);
	b = (BButton *)FindView("cancel");
	b->SetEnabled(true);
} /* CDocOptions::UpdateFields */
