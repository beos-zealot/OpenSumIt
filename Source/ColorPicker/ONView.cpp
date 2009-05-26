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
	CONView
	
	Copyright 1997, Hekkelman Programmatuur
*/

#include "ONView.h"
#include "ColorPicker.h"

#include <Application.h>

CONView::CONView(BRect frame, const char *name)
	: BBox(frame, name)
{
	SetFlags(Flags() & ~B_NAVIGABLE);
	
	SetFont(be_plain_font);
	SetLabel("Result");
	SetViewColor(kGray);
	
	fRect = Bounds();
	fRect.InsetBy(5, 5);
	
	font_height fi;
	be_plain_font->GetHeight(&fi);
	fRect.top += fi.ascent + fi.descent;

	fOldRect = fRect;
	fOldRect.InsetBy(1, 1);
	fOldRect.bottom -= fRect.Height() / 2 - 1;
	
	fNewRect = fRect;
	fNewRect.InsetBy(1, 1);
	fNewRect.top += fRect.Height() / 2 - 1;
} /* CONView::CONView */

CONView::~CONView()
{
} /* CONView::~CONView */
	
void CONView::Draw(BRect updateRect)
{
	SetHighColor(kBlack);
	BBox::Draw(updateRect);

	BRect r = fRect;	
	SetHighColor(kB_GrayTable[13]);
	StrokeLine(r.LeftTop(), r.LeftBottom());
	StrokeLine(r.LeftTop(), r.RightTop());
	SetHighColor(kWhite);
	StrokeLine(r.LeftBottom(), r.RightBottom());
	StrokeLine(r.RightTop(), r.RightBottom());
	
	SetHighColor(fOldColor);
	FillRect(fOldRect);
	SetHighColor(fNewColor);
	FillRect(fNewRect);
	SetHighColor(kBlack);
} /* CONView::Draw */

void CONView::SetOldColor(rgb_color oldColor)
{
	fOldColor = oldColor;
	SetHighColor(oldColor);
	FillRect(fOldRect);
} /* CONView::SetOldColor */

void CONView::SetNewColor(rgb_color newColor)
{
	fNewColor = newColor;
	SetHighColor(newColor);
	FillRect(fNewRect);
} /* CONView::SetNewColor */

void CONView::MouseDown(BPoint where)
{
	if (fNewRect.Contains(where))
	{
		BMessage msg(B_PASTE);
		
		rgb_color c = *fOwner;
		msg.AddData("RGBColor", 'RGBC', &c, 4);
		
		roSColor ro = *fOwner;
		msg.AddData("roColour", 'roCr', &ro, sizeof(ro));

		BRect r(0, 0, 9, 9);
		BBitmap *image = new BBitmap(r, B_RGB_32_BIT);
		
		uchar data[300], *dp = data;
		for (int i = 0; i < 100; i++)
		{
			*dp++ = c.red;
			*dp++ = c.green;
			*dp++ = c.blue;
		}
		image->SetBits(data, 300, 0, B_RGB_32_BIT);
		
		DragMessage(&msg, image, BPoint(4, 4));
	}
} /* CONView::MouseDown */

void CONView::AttachedToWindow()
{
	fOwner = (CColorPicker *)Window();
} /* CONView::AttachedToWindow */

void CONView::MouseMoved(BPoint, uint32, const BMessage *)
{
	be_app->SetCursor(B_HAND_CURSOR);
} /* CONView::MouseMoved */
