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
	CellWindow.dialogs.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "CellView.h"
#include "CellWindow.h"
#include "SortDialog.h"
#include "FillDialog.h"
#include "NamesDialog.h"
#include "GotoDialog.h"
#include "PasteFunctionDialog.h"
#include "DigitsDialog.h"

#include <typeinfo>

CDialog* CCellWindow::GetSortDialog()
{
	CDialog *d;
	int i;
	
	for (i = 0; i < fDialogs.CountItems(); i++)
	{
		d = (CDialog *)fDialogs.ItemAt(i);
		if (typeid(*d) == typeid(CSortDialog))
		{
			d->Activate();
			return d;
		}
	}
	
	return CSortDialog::Construct(this);
} /* CCellWindow::GetSortDialog */
 
CDialog* CCellWindow::GetFillDialog()
{
	CDialog *d;
	int i;
	
	for (i = 0; i < fDialogs.CountItems(); i++)
	{
		d = (CDialog *)fDialogs.ItemAt(i);
		if (typeid(*d) == typeid(CFillDialog))
		{
			d->Activate();
			return d;
		}
	}
	
	return CFillDialog::Construct(this);
} /* CCellWindow::GetFillDialog */

CDialog* CCellWindow::GetNamesDialog()
{
	CDialog *d;
	int i;
	
	for (i = 0; i < fDialogs.CountItems(); i++)
	{
		d = (CDialog *)fDialogs.ItemAt(i);
		if (typeid(*d) == typeid(CNamesDialog))
		{
			d->Activate();
			return d;
		}
	}
	
	range r;
	fCells->GetSelection(r);
	return new CNamesDialog(fCells, r);
} /* CCellWindow::GetNamesDialog */

CDialog* CCellWindow::GetGoToDialog()
{
	CDialog *d;
	int i;
	
	for (i = 0; i < fDialogs.CountItems(); i++)
	{
		d = (CDialog *)fDialogs.ItemAt(i);
		if (typeid(*d) == typeid(CGoToDialog))
		{
			d->Activate();
			return d;
		}
	}
	
	return CGoToDialog::Construct(this);
} /* CCellWindow::GetGoToDialog */

CDialog* CCellWindow::GetPasteFunctionDialog()
{
	CDialog *d;
	int i;
	
	for (i = 0; i < fDialogs.CountItems(); i++)
	{
		d = (CDialog *)fDialogs.ItemAt(i);
		if (typeid(*d) == typeid(CPasteFunctionDialog))
		{
			d->Activate();
			return d;
		}
	}
	
	return CPasteFunctionDialog::Construct(this);
} /* CCellWindow::GetPasteFunctionDialog */

CDialog* CCellWindow::GetDigitsDialog(int current)
{
	CDialog *d;
	int i;
	
	for (i = 0; i < fDialogs.CountItems(); i++)
	{
		d = (CDialog *)fDialogs.ItemAt(i);
		if (typeid(*d) == typeid(CDigitsDialog))
		{
			d->Activate();
			return d;
		}
	}
	
	return CDigitsDialog::Construct(this, current);
} /* CCellWindow::GetDigitsDialog */


void CCellWindow::AddDialog(CDialog *inDialog, bool inWindowModal)
{
	fDialogs.AddItem(inDialog);
	if (inWindowModal)
		fWindowModal = inDialog;
} /* CCellWindow::AddDialog */

void CCellWindow::RemoveDialog(CDialog *inDialog)
{
	fDialogs.RemoveItem(inDialog);
	if (fWindowModal == inDialog)
		fWindowModal = NULL;
} /* CCellWindow::RemoveDialog */

void CCellWindow::MakeModal(CDialog *inDialog)
{
	if (!fWindowModal)
		fWindowModal = inDialog;
} /* CCellWindow::MakeModal */
