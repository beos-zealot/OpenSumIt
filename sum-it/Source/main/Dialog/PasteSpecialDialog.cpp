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
	PasteSpecialDialog.c
	
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

#ifndef   COPYCOMMANDS_H
#include "CopyCommands.h"
#endif

#ifndef   PASTESPECIALDIALOG_H
#include "PasteSpecialDialog.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif


#include <Autolock.h>
#include <Message.h>
#include <Control.h>

CPasteSpecialDialog::CPasteSpecialDialog(CCellView *inView,
	CContainer *inContainer)
	: CDialog(BRect(100.0, 100.0, 380.0, 306.0), 
		"Paste Special",
		(CCellWindow *)inView->Window(),
		BRect(209.0, 175.0, 267.0, 195.0),
		BRect(135.0, 175.0, 193.0, 195.0),
		true)
{
	fCaller = inView;
	fCells = inContainer;
	range s;
	fCaller->GetSelection(s);
	
	BAutolock lock(this);
	if (!lock.IsLocked())
		THROW((errLockWindow));
	
	BRect r;
	
	r.Set(23.0, 13.0, 148.0, 31.0);
	AddCheckBox(r, "paste format", "Paste Format", B_CONTROL_ON);

	r.Set(23.0, 33.0, 148.0, 51.0);
	AddCheckBox(r, "paste content", "Paste Content", B_CONTROL_ON);

	r.Set(42.0, 53.0, 148.0, 111.0);
	BView *gv = AddGrayView(r, NULL);

	r.Set(0.0, 0.0, 106.0, 18.0);
	AddRadioButton(r, "values", "Values", B_CONTROL_OFF, gv);

	r.Set(0.0, 20.0, 106.0, 38.0);
	AddRadioButton(r, "formulas", "Formulas", B_CONTROL_ON, gv);

	r.Set(0.0, 40.0, 106.0, 58.0);
	AddRadioButton(r, "references", "References", B_CONTROL_OFF, gv);

	r.Set(161.0, 13.0, 267.0, 111.0);
	gv = AddGrayView(r, NULL);

	r.Set(0.0, 0.0, 106.0, 18.0);
	AddRadioButton(r, "overwrite", "Overwrite", B_CONTROL_ON, gv);

	r.Set(0.0, 20.0, 106.0, 38.0);
	AddRadioButton(r, "add", "Add", B_CONTROL_OFF, gv);

	r.Set(0.0, 40.0, 106.0, 58.0);
	AddRadioButton(r, "substract", "Substract", B_CONTROL_OFF, gv);

	r.Set(0.0, 60.0, 106.0, 78.0);
	AddRadioButton(r, "multiply", "Multiply", B_CONTROL_OFF, gv);

	r.Set(0.0, 80.0, 106.0, 98.0);
	AddRadioButton(r, "divide", "Divide", B_CONTROL_OFF, gv);

	r.Set(23.0, 121.0, 251.0, 139.0);
	AddCheckBox(r, "original", "Original Cell References", B_CONTROL_OFF);

	r.Set(23.0, 141.0, 251.0, 159.0);
	AddCheckBox(r, "selection only", "To Selection Only", 
		s.top != s.bottom || s.left != s.right);

	r.Set(23.0, 161.0, 251.0, 179.0);
	AddCheckBox(r, "transpose", "Transpose", B_CONTROL_OFF);

	UpdateFields();
	
	FindView("paste format")->MakeFocus();
	
	Show();
} /* CPasteSpecialDialog::CPasteSpecialDialog */

void CPasteSpecialDialog::UpdateFields()
{
	fPasteFormat = IsOn("paste format");
	fPasteContent = IsOn("paste content");
	fFillSelection = IsOn("selection only");
	fTranspose = IsOn("transpose");
	fOriginalRefs = IsOn("original");
	
	if (IsOn("values"))
		fPasteWhat = pValues;
	else if (IsOn("formulas"))
		fPasteWhat = pFormulas;
	else
		fPasteWhat = pReferences;
	
	if (IsOn("overwrite"))
		fPasteHow = pOverwrite;
	else if (IsOn("add"))
		fPasteHow = pAdd;
	else if (IsOn("substract"))
		fPasteHow = pSubtract;
	else if (IsOn("multiply"))
		fPasteHow = pMultiply;
	else
		fPasteHow = pDivide;
	
	((BControl *)FindView("values"))->SetEnabled(fPasteContent);
	((BControl *)FindView("formulas"))->SetEnabled(fPasteContent);
	((BControl *)FindView("references"))->SetEnabled(fPasteContent);
	((BControl *)FindView("overwrite"))->SetEnabled(fPasteContent);
	((BControl *)FindView("add"))->SetEnabled(fPasteContent);
	((BControl *)FindView("substract"))->SetEnabled(fPasteContent);
	((BControl *)FindView("multiply"))->SetEnabled(fPasteContent);
	((BControl *)FindView("divide"))->SetEnabled(fPasteContent);
	((BControl *)FindView("original"))->SetEnabled(fPasteContent);
	
} /* CPasteSpecialDialog::UpdateFields */

bool CPasteSpecialDialog::OKClicked()
{
	BMessage msg(msg_Command);

	msg.AddPointer("command", new CPasteCommand(
		fCaller, fCells, fPasteFormat, fPasteContent,
		fFillSelection, fTranspose, fOriginalRefs,
		fPasteWhat, fPasteHow));

	fOwner->PostMessage(&msg);
	return true;
} /* CPasteSpecialDialog::OKClicked */
