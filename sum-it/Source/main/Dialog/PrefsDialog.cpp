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
	PrefsDialog.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   RDIALOG_H
#include "RDialog.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   PREFSDIALOG_H
#include "PrefsDialog.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

#ifndef   TABSHEET_H
#include "TabSheet.h"
#endif

#ifndef   FORMULA_H
#include "Formula.h"
#endif

#ifndef   CELLSTYLE_H
#include "CellStyle.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif


#include <Autolock.h>
#include <Message.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <Box.h>
#include <Button.h>
#include <PrintJob.h>


// This looks like hacking, and in fact it is...
extern int gGridColor;

const ulong
	msg_ChangeTarget = 'ChTr',
	msg_DefPageSetup = 'Page';

CPrefsDialog::CPrefsDialog(BRect frame, const char *name, CCellWindow *owner,
	window_type type, int flags)
	: CRDialog(frame, name, owner, type, flags)
{
	fPageSetup = NULL;
} /* CPrefsDialog */

CPrefsDialog::~CPrefsDialog()
{
	if (fPageSetup) FREE(fPageSetup);
} /* CPrefsDialog::~CPrefsDialog */

void CPrefsDialog::PostInit()
{
	BAutolock lock(this);

	strcpy(fNewFontFamily, gPrefs->GetPrefString("defdoc font family", NULL));
	fNewFontSize = gPrefs->GetPrefDouble("defdoc font size", 0.0);
	strcpy(fNewBFontFamily, gPrefs->GetPrefString("border font family", NULL));
	fNewBFontSize = gPrefs->GetPrefDouble("border font size", 0.0);

	CellStyle cs;
	font_style style;

	fOwner->GetCellView()->GetContainer()->GetDefaultCellStyle(cs);
	try {
		gFontSizeTable.GetFontInfo(cs.fFont, &fDocFontFamily, &style, &fDocFontSize);
		gFontSizeTable.GetFontInfo(fOwner->GetCellView()->BorderFontID(),
			&fDocBFontFamily, &style, &fDocBFontSize);
	}
	catch (CErr& e) {
		e.DoError();
	}

	BMenuField *mf = dynamic_cast<BMenuField*>(FindView("docfont"));
	fDocFont = mf->Menu();
	if (fDocFont) 
	{
		for (long i = 0; i < count_font_families(); i++)
		{
			font_family ff;
			get_font_family(i, &ff);
			fDocFont->AddItem(new BMenuItem(ff, new BMessage(msg_Changed)));
		}
		fDocFont->SetRadioMode(TRUE);
	}

	mf = dynamic_cast<BMenuField*>(FindView("borderfont"));
	fBorderFont = mf->Menu();
	for (long i = 0; i < count_font_families(); i++)
	{
		font_family ff;
		get_font_family(i, &ff);
		fBorderFont->AddItem(new BMenuItem(ff, new BMessage(msg_Changed)));
	}
	fBorderFont->SetRadioMode(TRUE);

	mf = dynamic_cast<BMenuField*>(FindView("dateorder"));
	fDMY = mf->Menu();
	fDMY->AddItem(new BMenuItem("dmy", new BMessage(msg_Changed)));
	fDMY->AddItem(new BMenuItem("dym", new BMessage(msg_Changed)));
	fDMY->AddItem(new BMenuItem("mdy", new BMessage(msg_Changed)));
	fDMY->AddItem(new BMenuItem("myd", new BMessage(msg_Changed)));
	fDMY->AddItem(new BMenuItem("ydm", new BMessage(msg_Changed)));
	fDMY->AddItem(new BMenuItem("ymd", new BMessage(msg_Changed)));

	mf = dynamic_cast<BMenuField*>(FindView("neworcur"));
	if (mf)
	{
		fNewOrCur = mf->Menu();
		fNewOrCur->AddItem(new BMenuItem("New Documents", new BMessage(msg_ChangeTarget)));
		if (fOwner)
		{
			char s[32];
			fDocFont->FindItem(fDocFontFamily)->SetMarked(true);
			ftoa(fDocFontSize, s);
			SetText("docsize", s);
			fBorderFont->FindItem(fDocBFontFamily)->SetMarked(true);
			ftoa(fDocBFontSize, s);
			SetText("bordersize", s);

			fNewOrCur->AddItem(new BMenuItem(fOwner->Title(), new BMessage(msg_ChangeTarget)));
			fNewOrCur->ItemAt(1)->SetMarked(true);
		}
		else
			fNewOrCur->ItemAt(0)->SetMarked(true);
		mf->SetDivider(be_plain_font->StringWidth(mf->Label()) + 4);
	}

	BBox *b;
	BView *v;
	v = FindView("prgrid");
	b = dynamic_cast<BBox*>(v->Parent());
	if (b)
	{
		char t[256];
		sprintf(t, b->Label(), fOwner->Title());
		b->SetLabel(t);
	}
	
	fAutoRecalc = fOwner->GetCellView()->DoesAutoRecalc();
	fDisplayZero = fOwner->GetCellView()->DoesDisplayZero();
	fPrGrid = fOwner->GetCellView()->PrintsGrid();
	fPrBorders = fOwner->GetCellView()->PrintsBorders();
	fShGrid = fOwner->GetCellView()->ShowsGrid();
	fShBorders = fOwner->GetCellView()->ShowsBorders();

	CancelClicked();

	Show();
} /* CPrefsDialog::PostInit */

bool CPrefsDialog::OKClicked()
{
	font_family defFamily;
	font_style defStyle;

	be_plain_font->GetFamilyAndStyle(&defFamily, &defStyle);
	gPrefs->SetPrefString("defdoc font family", fNewFontFamily);
	gPrefs->SetPrefString("defdoc font style", defStyle);
	gPrefs->SetPrefDouble("defdoc font size", fNewFontSize);

	be_bold_font->GetFamilyAndStyle(&defFamily, &defStyle);
	gPrefs->SetPrefString("border font family", fNewBFontFamily);
	gPrefs->SetPrefString("border font style", defStyle);
	gPrefs->SetPrefDouble("border font size", fNewBFontSize);

	gPrefs->SetPrefInt("decimal point", *GetText("decsep"));
	gPrefs->SetPrefInt("thousands separator", *GetText("thoussep"));
	gPrefs->SetPrefInt("list separator", *GetText("listsep"));
	gPrefs->SetPrefInt("date separator", *GetText("datesep"));
	gPrefs->SetPrefInt("time separator", *GetText("timesep"));
	gPrefs->SetPrefString("date order", fDMY->FindMarked()->Label());	
	gPrefs->SetPrefInt("24 hours", IsOn("24 hours"));
	gPrefs->SetPrefInt("Excel keys", IsOn("excel"));
	gPrefs->SetPrefInt("Select Gray", IsOn("grayselect"));
	gPrefs->SetPrefInt("start with new", IsOn("donew"));
	gWithEqualSign = IsOn("formula starts with equal");
	gPrefs->SetPrefInt("formula starts with equal", gWithEqualSign);
	gPrefs->SetPrefInt("dark gridlines", 	IsOn("dark gridlines"));
	if (gPrefs->GetPrefInt("dark gridlines", 0))
		gGridColor = 10;
	else
		gGridColor = 6;

	gPrefs->SetPrefString("c_symbol", GetText("c_symbol"));
	gPrefs->SetPrefInt("c_before", IsOn("c_before"));
	gPrefs->SetPrefInt("c_neg_par", IsOn("c_neg_par"));
	gPrefs->SetPrefInt("c_digits", atoi(GetText("c_digits")));

	if (fPageSetup)
	{
		char *ps = Bin2Hex(fPageSetup, fPageSetupSize);
		gPrefs->SetPrefString("default page setup", ps);
		FREE(ps);
	}

	BMessage msg(msg_WindowOptions);

	msg.AddString("docfamily", fDocFontFamily);
	msg.AddString("docstyle", "Roman");
	msg.AddFloat("docsize", fDocFontSize);
	msg.AddString("borderfamily", fDocBFontFamily);
	msg.AddString("borderstyle", "Bold");
	msg.AddFloat("bordersize", fDocBFontSize);
	msg.AddBool("autorecalc", IsOn("autorecalc"));
	msg.AddBool("displayzero", IsOn("displayzero"));
	msg.AddBool("dispgrid", IsOn("dispgrid"));
	msg.AddBool("disphead", IsOn("disphead"));
	msg.AddBool("prgrid", IsOn("prgrid"));
	msg.AddBool("prhead", IsOn("prhead"));
	
	fOwner->PostMessage(&msg, fOwner->GetCellView());

	BButton *b;
	b = (BButton *)FindView("ok");
	b->SetEnabled(false);
	b = (BButton *)FindView("cancel");
	b->SetEnabled(false);

	InitFormatter();

	return false;
} /* CPrefsDialog::OKClicked */

bool CPrefsDialog::CancelClicked()
{
	char s[256];
	
	ResetFontSheet();
	
	SetOn("autorecalc", fAutoRecalc);
	SetOn("displayzero", fDisplayZero);
	
	s[1] = 0;
	s[0] = gDecimalPoint;
	SetText("decsep", s);
	s[0] = gThousandSeparator;
	SetText("thoussep", s);
	s[0] = gListSeparator;
	SetText("listsep", s);
	s[0] = gTimeSeparator;
	SetText("timesep", s);
	s[0] = gDateSeparator;
	SetText("datesep", s);
	fDMY->FindItem(gPrefs->GetPrefString("date order"))->SetMarked(true);
	SetOn("24 hours", g24Hours);
	SetOn("excel", gPrefs->GetPrefInt("Excel keys", 0));
	SetOn("grayselect", gPrefs->GetPrefInt("Select Gray", 0));
	SetOn(gPrefs->GetPrefInt("start with new", 1) ? "donew" : "doopen", B_CONTROL_ON);
	SetOn("formula starts with equal", gPrefs->GetPrefInt("formula starts with equal", 0));
	SetOn("dark gridlines", gPrefs->GetPrefInt("dark gridlines"));
	
	SetText("c_symbol", gPrefs->GetPrefString("c_symbol", "$"));
	SetOn(gPrefs->GetPrefInt("c_before", 1) ? "c_before" : "c_after", B_CONTROL_ON);
	SetOn("c_neg_par", gPrefs->GetPrefInt("c_neg_par", 1) ? B_CONTROL_ON : B_CONTROL_OFF);
	sprintf(s, "%d", gPrefs->GetPrefInt("c_digits"));
	SetText("c_digits", s);

	if (fPageSetup) FREE(fPageSetup);
	fPageSetup = (char *)Hex2Bin(gPrefs->GetPrefString("default page setup", ""), fPageSetupSize);
	
	SetOn("prgrid", fPrGrid);
	SetOn("prhead", fPrBorders);
	SetOn("dispgrid", fShGrid);
	SetOn("disphead", fShBorders);
	
	BButton *b;
	b = (BButton *)FindView("ok");
	b->SetEnabled(false);
	b = (BButton *)FindView("cancel");
	b->SetEnabled(false);
	
	return false;
} /* CPrefsDialog::CancelClicked */

void CPrefsDialog::UpdateFields()
{
	if (fNewOrCur->IndexOf(fNewOrCur->FindMarked()) == 1)
		GrabDocFont();
	else
		GrabNewFont();
	
	BButton *b;
	b = (BButton *)FindView("ok");
	b->SetEnabled(true);
	b = (BButton *)FindView("cancel");
	b->SetEnabled(true);
} /* CPrefsDialog::UpdateFields */

CPrefsDialog* CPrefsDialog::Construct(CCellWindow *owner)
{
	BMallocIO buf;
	BRect frame;
	char name[256];
	window_type type;
	int flags;
	
	InitValues(0, buf, frame, name, type, flags, NULL);
	
	CPrefsDialog* result = new CPrefsDialog(frame, name, owner, type, flags);
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
} /* CPrefsDialog::Construct */

void CPrefsDialog::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case msg_ChangeTarget:
			ResetFontSheet();
			break;

		case msg_DefPageSetup:
			GetDefPageSetup();
			break;
		
		default:
			CDialog::MessageReceived(msg);
			break;
	}
} /* CPrefsDialog::MessageReceived */

void CPrefsDialog::GrabDocFont()
{
	BMenuItem *item;
	
	item = fDocFont->FindMarked();
	if (item) strcpy(fDocFontFamily, item->Label());
	fDocFontSize = atof(GetText("docsize"));
	item = fBorderFont->FindMarked();
	if (item) strcpy(fDocBFontFamily, item->Label());
	fDocBFontSize = atof(GetText("bordersize"));
} /* CPrefsDialog::GrabDocFont */

void CPrefsDialog::GrabNewFont()
{
	BMenuItem *item;
	
	item = fDocFont->FindMarked();
	if (item) strcpy(fNewFontFamily, item->Label());
	fNewFontSize = atof(GetText("docsize"));
	item = fBorderFont->FindMarked();
	if (item) strcpy(fNewBFontFamily, item->Label());
	fNewBFontSize = atof(GetText("bordersize"));
} /* CPrefsDialog::GrabNewFont */

void CPrefsDialog::ResetFontSheet()
{
	char *ff, *bf;
	float fs, bs;
	
	if (fNewOrCur->IndexOf(fNewOrCur->FindMarked()) == 0)
	{
		ff = fNewFontFamily;	bf = fNewBFontFamily;
		fs = fNewFontSize;		bs = fNewBFontSize;
	}
	else
	{
		ff = fDocFontFamily;	bf = fDocBFontFamily;
		fs = fDocFontSize;		bs = fDocBFontSize;
	}

	char s[32];
	fDocFont->FindItem(ff)->SetMarked(true);
	ftoa(fs, s);
	SetText("docsize", s);
	fBorderFont->FindItem(bf)->SetMarked(true);
	ftoa(bs, s);
	SetText("bordersize", s);
} /* CPrefsDialog::ResetFontSheet */

void CPrefsDialog::GetDefPageSetup()
{
	BPrintJob prJob("a page setup job");
	
	if (fPageSetup)
	{
		BMessage *s = new BMessage;
		if (s && s->Unflatten(fPageSetup) == B_NO_ERROR)
			prJob.SetSettings(s);
	}
	
	int result = prJob.ConfigPage();
	
	if (result == B_NO_ERROR)
	{
		BMessage * s(prJob.Settings());

		fPageSetupSize = s->FlattenedSize();
		if (fPageSetup) FREE(fPageSetup);
		fPageSetup = (char *)MALLOC(fPageSetupSize);
		FailNil(fPageSetup);
		result = s->Flatten(fPageSetup, fPageSetupSize);
		FailOSErr(result, "error flattening (%d)");
		
		UpdateFields();
	}
} /* CPrefsDialog::GetDefPageSetup */
