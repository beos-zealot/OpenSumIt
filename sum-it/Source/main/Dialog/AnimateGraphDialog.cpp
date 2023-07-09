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
	AnimateGraphDialog.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

//#ifndef   CELLVIEW_H
//#include "CellView.h"
//#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   ANIMATEGRAPHDIALOG_H
#include "AnimateGraphDialog.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#include <View.h>
#include <Control.h>
#include <Message.h>

class CPulsingView : public BView {
public:
	CPulsingView(CAnimateGraphDialog *d)
		: BView(BRect(-1, -1, 0, 0), "pulseview", 0, B_PULSE_NEEDED)
			{ fDialog = d; };

	void Pulse()
		{ fDialog->Pulse(); };
	
	CAnimateGraphDialog *fDialog;
};

CAnimateGraphDialog::CAnimateGraphDialog(CCellWindow *w)
	: CDialog(BRect(100, 100, 300, 250),
		"Animate Graphs", w,
		BRect(142, 120, 190, 140),
		BRect(74, 120, 132, 140))
{
	fAnimating = false;

	char number[] = "0123456789.eE-+";
	number[10] = gDecimalPoint;

	BRect r;

	r.Set(10, 10, 190, 26);
	AddTextControl(r, "cell", "Cell:", NULL, NULL, 6);

	r.Set(10, 31, 190, 47);
	AddTextControl(r, "start", "Start:", NULL, number, 10);

	r.Set(10, 52, 190, 68);
	AddTextControl(r, "stop", "Stop:", NULL, number, 10);

	r.Set(10, 73, 190, 89);
	AddTextControl(r, "step", "Step:", NULL, number, 10);

	r.Set(10, 94, 190, 110);
	AddTextControl(r, "interval", "Interval:", NULL, number, 5);

	FindView("cell")->MakeFocus();

	BControl *c = (BControl *)FindView("ok");
	c->SetLabel("Start");

	c = (BControl *)FindView("cancel");
	c->SetLabel("Stop");

	AddChild(new CPulsingView(this));
	
	Show();
} /* CAnimateGraphDialog::CAnimateGraphDialog */

bool CAnimateGraphDialog::OKClicked()
{
	double interval;
	
	if (!cell::GetCell(GetText("cell"), fCell))
		return false;

	if ( fOwner->GetCellFormula(fCell) )
	{
		MAlert *a = new MWarningAlert(GetMessage(msgHasFormula));
		a->Go();
		return false;
	}
	
	if (!GetDouble("start", &fStart) ||
		!GetDouble("stop", &fStop) ||
		!GetDouble("step", &fStep) ||
		!GetDouble("interval", &interval))
		return false;
	
	SetPulseRate(interval * 1e6);
	fCurrent = fStart;
	fAnimating = true;
	return false;
} /* CAnimateGraphDialog::OKClicked */

bool CAnimateGraphDialog::CancelClicked()
{
	fAnimating = false;
	return false;
} /* CAnimateGraphDialog::CancelClicked */

void CAnimateGraphDialog::Pulse()
{
	if (fAnimating)
	{
		BHandler * h = fOwner->GetCellHandler();
		BMessage msg(msg_AnimateCell);
		msg.AddInt32("cell", *(int32 *)&fCell);
		msg.AddDouble("value", fCurrent);
		fOwner->PostMessage(&msg, h);

		fCurrent += fStep;
		if (fCurrent > fStop)
			fCurrent = fStart;
	}
} /* CAnimateGraphDialog::Pulse */
