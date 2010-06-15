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
	Copyright 1997, Hekkelman Programmatuur

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   TABSHEET_H
#include "TabSheet.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif


#include <Window.h>

CTabSheet::CTabSheet(BRect frame, const char *name)
	: BView(frame, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_NAVIGABLE)
{
	SetViewColor(kB_GrayTable[6]);
	SetLowColor(kB_GrayTable[6]);
	SetFont(be_plain_font);
	
	font_height fi;
	be_plain_font->GetHeight(&fi);
	fTabHeight = ceil(fi.ascent + fi.descent + fi.leading) + 5;
	fBaseLine = ceil(fi.ascent + 3);
	fTabSpace = StringWidth("MM");
	fCurrent = 0;
} /* CTabSheet::CTabSheet */

CTabSheet::~CTabSheet()
{
} /* CTabSheet::~CTabSheet */
	
void CTabSheet::MouseDown(BPoint where)
{
	int i;
	BRect t;
	
	t.Set(0, 0, 0, fTabHeight);
	
	for (i = 0; i < CountChildren(); i++)
	{
		BView *v = ChildAt(i);
		float w = StringWidth(v->Name()) + fTabSpace;
		
		t.left = t.right;
		t.right += w;
		
		if (t.Contains(where))
		{
			if (fCurrent != i)
			{
				ChildAt(fCurrent)->Hide();
				fCurrent = i;
				ChildAt(fCurrent)->Show();
				Draw(Bounds());
				Window()->PostMessage(msg_Flip);
			}
			break;
		}
	}
} /* CTabSheet::MouseDown */

void CTabSheet::Draw(BRect update)
{
	int i;
	float l = 0;
	
	BRect b(Bounds());
	
	FillRect(b, B_SOLID_LOW);
	
	for (i = 0; i < CountChildren(); i++)
	{
		BView *v = ChildAt(i);
		float w = StringWidth(v->Name()) + fTabSpace;
		float r = l + w;
		
		SetHighColor(kBlack);
		DrawString(v->Name(), BPoint(l + (fTabSpace / 2), fBaseLine));
		
		if (i == fCurrent)
		{
			SetHighColor(kWhite);
			StrokeLine(BPoint(l, 0), BPoint(r - 2, 0));
			StrokeLine(BPoint(l, 0), BPoint(l, fTabHeight - 1));
			
			SetHighColor(kB_GrayTable[13]);
			StrokeLine(BPoint(r - 2, 1), BPoint(r - 2, fTabHeight - 1));
			
			SetHighColor(kB_GrayTable[18]);
			StrokeLine(BPoint(r - 1, 1), BPoint(r - 1, fTabHeight - 1));
		}
		else
		{
			SetHighColor(kWhite);
			StrokeLine(BPoint(l, 2), BPoint(r - 2, 2));
			StrokeLine(BPoint(l, 2), BPoint(l, fTabHeight - 1));
			StrokeLine(BPoint(l, fTabHeight), BPoint(r, fTabHeight));
			
			SetHighColor(kB_GrayTable[13]);
			StrokeLine(BPoint(r - 2, 3), BPoint(r - 2, fTabHeight - 1));
			
			SetHighColor(kB_GrayTable[18]);
			StrokeLine(BPoint(r - 1, 3), BPoint(r - 1, fTabHeight - 1));
		}
		
		l += w;
	}
	
	SetHighColor(kB_GrayTable[13]);
	StrokeLine(b.LeftBottom(), b.RightBottom());
	StrokeLine(b.RightBottom(), BPoint(b.right, b.top + fTabHeight));
	
	SetHighColor(kWhite);
	StrokeLine(BPoint(b.left, fTabHeight), b.LeftBottom());
	StrokeLine(BPoint(l, fTabHeight), BPoint(b.right, fTabHeight));
	
	SetHighColor(kBlack);
} /* CTabSheet::Draw */
	
BRect CTabSheet::ClientArea()
{
	BRect area = Bounds();
	area.top += fTabHeight;
	area.InsetBy(4, 4);
	return area;
} /* CTabSheet::ClientArea */

BView* CTabSheet::AddSheet(const char *name)
{
	BView *r = new BView(ClientArea(), name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	r->SetViewColor(kB_GrayTable[6]);
	AddChild(r);
	if (CountChildren() > 1)
		r->Hide();
	return r;
} /* CTabSheet::AddSheet */
