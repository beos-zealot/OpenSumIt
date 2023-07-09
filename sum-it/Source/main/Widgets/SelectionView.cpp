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
	SelectionView.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   SELECTIONVIEW_H
#include "SelectionView.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#ifndef   NAMETABLE_H
#include "NameTable.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif


#include <Message.h>

#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>

#include <Window.h>

CSelectionView::CSelectionView(CCellView* cellView, BRect frame)
	: BView(frame, "selView", B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,
		B_WILL_DRAW)
{
	fCellView = cellView;
	fText = NULL;
}

CSelectionView::~CSelectionView()
{
	if (fText)
		FREE(fText);
}

void
CSelectionView::AttachedToWindow()
{
	BView::AttachedToWindow();
	SetViewColor(kB_GrayTable[6]);
	fCellView->AttachSelectionView(this);
	ChooseFont();
} // AttachedToWindow

void
CSelectionView::Draw(BRect /*updateRect*/)
{
	BRect frame(Bounds());
		
	SetHighColor(kB_GrayTable[14]);
	StrokeRect(Bounds());
	SetHighColor(kB_GrayTable[0]);
	
	frame.InsetBy(1.0, 1.0);
	SetFontSize(9);
	FillRect3D(this, frame, FALSE, fWindowIsActive, fText);
}

void
CSelectionView::SetRange(range *r)
{
	char s[16];
	r->GetName(s);
	SetText(s);
}

void
CSelectionView::SetText(const char *s)
{
	if (fText)
		FREE(fText);
	
	fText = STRDUP(s);
	Draw(Bounds());
}
void CSelectionView::WindowActivated(bool active)
{
	BView::WindowActivated(active);
	fWindowIsActive = active;
	Draw(Bounds());
} /* CSelectionView::WindowActivated */

void CSelectionView::MouseDown(BPoint where)
{
	uint32 btns;
	BPoint cur;
	
	GetMouse(&cur, &btns);

	if (IsOptionalClick(Window()->CurrentMessage()))
	{
		BPopUpMenu popUp("gotomenu");
		popUp.SetFont(be_plain_font);
		CNameTable& names = *fCellView->GetNameTable();
		
		if (names.size() == 0)
			popUp.AddItem(new BMenuItem(GetMessage(msgNoNames), 0));
		else
		{
			namemap::iterator i;
			
			for (i = names.begin(); i != names.end(); i++)
				popUp.AddItem(new BMenuItem((*i).first, 0));
		}
		
		BMenuItem *item = popUp.Go(ConvertToScreen(where));
		
		if (item)
		{
			BMessage msg(msg_GoTo);
			msg.AddString("name", item->Label());
			Window()->PostMessage(&msg, fCellView);
		}
	}
	else
	{
		bool isIn = true;
		BRect frame(Bounds());
		
		frame.InsetBy(1.0, 1.0);
		FillRect3D(this, frame, true, true, fText);

		while (btns)
		{
			if (frame.Contains(cur))
			{
				if (!isIn)
				{
					isIn = true;
					FillRect3D(this, frame, true, true, fText);
				}
			}
			else
			{
				if (isIn)
				{
					isIn = false;
					FillRect3D(this, frame, false, true, fText);
				}
			}
			GetMouse(&cur, &btns);
		}
	
		FillRect3D(this, frame, false, true, fText);
		
		if (isIn)
			Window()->PostMessage(msg_GoToCmd, fCellView);
	}
} /* CSelectionView::MouseDown */

void CSelectionView::ChooseFont()
{
	CFontMetrics metrics = gFontSizeTable[fCellView->BorderFontID()];
	BFont myFont = metrics.Font();
	font_height fi;
	
	do
	{
		myFont.GetHeight(&fi);
		if (fi.ascent + fi.descent <= Bounds().Height())
			break;
		
		float size = myFont.Size();
		myFont.SetSize(size - 1);
		myFont.GetHeight(&fi);
		if (fi.ascent + fi.descent <= Bounds().Height())
			break;

		myFont.SetSize(size - 2);
		myFont.GetHeight(&fi);
		if (fi.ascent + fi.descent <= Bounds().Height())
			break;
		
		myFont = be_plain_font;
		myFont.GetHeight(&fi);
		if (fi.ascent + fi.descent <= Bounds().Height())
			break;
		
		// uh, oh, trouble...
	}
	while (false);

	SetFont(&myFont);
	Invalidate();
} /* CSelectionView::ChooseFont */	
