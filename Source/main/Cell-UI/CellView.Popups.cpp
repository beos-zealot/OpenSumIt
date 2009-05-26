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
	CellView.Popups.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-05) Made to compile
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   MTHREAD_H
#include "MThread.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
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

#include <Menu.h>
#include <MenuItem.h>
#include <PopUpMenu.h>

class CBorderPopup : public MThread {
public:
	CBorderPopup(BPoint where, CCellView *owner);
	virtual long Execute();

private:
	BPoint fWhere;
	CCellView *fOwner;
};

CBorderPopup::CBorderPopup(BPoint where, CCellView *owner)
{
	fWhere = where;
	fOwner = owner;
} /* CBorderPopup::CBorderPopup */

long CBorderPopup::Execute()
{
	BPopUpMenu popup("popupmenu", false);

	popup.SetFont(be_plain_font);
	popup.AddItem(new BMenuItem("Hide", new BMessage(msg_Head)));
	popup.AddSeparatorItem();
	
	BMenu *fontMenu = new BMenu("Font");
	fontMenu->SetFont(be_plain_font);
	for (long i = 0; i < count_font_families(); i++)
	{
		font_family fontFamily;
		get_font_family(i, &fontFamily);
		fontMenu->AddItem(new BMenuItem(fontFamily,
			new BMessage(msg_ChangeBorderFontFamily)));
	}
	fontMenu->SetRadioMode(TRUE);
	popup.AddItem(fontMenu);

	BMenu *sizeMenu = new BMenu("Size");
	sizeMenu->SetFont(be_plain_font);
	sizeMenu->AddItem(new BMenuItem("9", new BMessage(msg_ChangeBorderFontSize)));
	sizeMenu->AddItem(new BMenuItem("10", new BMessage(msg_ChangeBorderFontSize)));	
	sizeMenu->AddItem(new BMenuItem("12", new BMessage(msg_ChangeBorderFontSize)));
	sizeMenu->AddItem(new BMenuItem("14", new BMessage(msg_ChangeBorderFontSize)));
	sizeMenu->AddItem(new BMenuItem("18", new BMessage(msg_ChangeBorderFontSize)));
	sizeMenu->AddItem(new BMenuItem("24", new BMessage(msg_ChangeBorderFontSize)));
	sizeMenu->SetRadioMode(TRUE);
	popup.AddItem(sizeMenu);

	font_family bFamily;
	font_style bStyle;
	float bSize;
	char bSizeStr[32];
	int borderFontID;
	
	borderFontID = fOwner->BorderFontID();
	gFontSizeTable.GetFontInfo(borderFontID, &bFamily, &bStyle, &bSize);
	ftoa(bSize, bSizeStr);

	BMenuItem *item = sizeMenu->FindItem(bSizeStr);
	if (item) item->SetMarked(true);

	item = fontMenu->FindItem(bFamily);
	if (item) item->SetMarked(true);
	
	BMenuItem *selected = popup.Go(fWhere);
	
	if (selected)
	{
		BMessage msg( *selected->Message() );
		switch (msg.what)
		{
			case msg_ChangeBorderFontFamily:
				msg.AddString("family", selected->Label());
				break;
			case msg_ChangeBorderFontSize:
				msg.AddFloat("size", atof_i(selected->Label()));
				break;
		}
		fOwner->Window()->PostMessage(&msg, fOwner);
	}
	return 0 ;
}

void CCellView::BorderMenu(BPoint where)
{
	ConvertToScreen(&where);
	CBorderPopup *m = new CBorderPopup(where, this);
	m->Run();
}
 /* CCellView::BorderMenu */

void CCellView::GridMenu(BPoint where)
{
	BPopUpMenu popup("gridpopup", false);
	popup.SetFont(be_plain_font);
	popup.AddItem(new BMenuItem(GetMessage(msgCut), NULL));
	popup.AddItem(new BMenuItem(GetMessage(msgCopy), NULL));
	popup.AddItem(new BMenuItem(GetMessage(msgPaste), NULL));
	popup.AddItem(new BMenuItem(GetMessage(msgPasteSpecial), NULL));
	popup.AddItem(new BMenuItem(GetMessage(msgClear), NULL));
	popup.AddSeparatorItem();
	popup.AddItem(new BMenuItem(GetMessage(msgSelectAll), NULL));
	popup.AddSeparatorItem();
	popup.AddItem(new BMenuItem(GetMessage(fShowGrid ? msgHideGrid : msgShowGrid), NULL));
	popup.AddItem(new BMenuItem(GetMessage(fShowBorders ? msgHideHead : msgShowHead), NULL));
	
	BMenuItem *item = popup.Go(ConvertToScreen(where));
	
	if (item)
	{
		CCellWindow *w = (CCellWindow *)Window();
		
		switch (popup.IndexOf(item))
		{
			case 0:
				w->RegisterCommand(new CCutCommand(this, fContainer));
				w->SetDirty(true);
				break;
			case 1:
				w->RegisterCommand(new CCopyCommand(this, fContainer));
				break;
			case 2:
				w->RegisterCommand(new CPasteCommand(this, fContainer,
						true, true,
						fSelection.top != fSelection.bottom ||
						fSelection.left != fSelection.right));
				w->SetDirty(true);
				break;
			case 3:
				new CPasteSpecialDialog(this, fContainer);
				break;
			case 4:
				w->RegisterCommand(new CClearCommand(this, fContainer));
				w->SetDirty(true);
				break;
			case 6:
			{
				range r(1, 1, kColCount, kRowCount);
				SetSelection(r);
				break;
			}
			case 8:
				fShowGrid = !fShowGrid;
				Invalidate(fCellBounds);
				break;
			case 9:
				fShowBorders = !fShowBorders;
				RecalculateBorderWidths();
				Invalidate();
				break;
		}
	}
	return ;
}
	
