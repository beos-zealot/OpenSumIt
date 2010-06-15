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
	HSVView
	
	Copyright 1997, Hekkelman Programmatuur
*/

#include "RGBView.h"
#include "ColorSlider.h"

#include <Beep.h>
#include <Application.h>

#include <cstdlib>
#include <cstdio>

CRGBView::CRGBView(BRect frame, const char *name)
	: BBox(frame, name)
{
	SetFlags(Flags() & ~B_NAVIGABLE);
	
	SetViewColor(kGray);
	SetLowColor(kGray);
	
	SetLabel("RGB");
	SetFont(be_plain_font);

	rgb_color c = { 0, 0, 0, 0 };

	BRect r(Bounds()), tr;
	
	font_height fi;
	be_plain_font->GetHeight(&fi);

	r.InsetBy(10, 10);
	r.top += fi.ascent + fi.descent;
	r.right -= be_plain_font->StringWidth("255") + 18;

	float o = (r.Height() - 12) / 2;
	r.bottom = r.top + 12;
	
	tr.Set(r.right + 8, r.top - 2, r.right + 20 +
		be_plain_font->StringWidth("255"), r.top + 2 + fi.ascent + fi.descent);
	
	c.red = 255;
	fRedSlider = new CColorSlider(r, "redslider", c);
	AddChild(fRedSlider);
	
	AddChild(fRed = new BTextControl(tr, "red", NULL, "0", NULL));
	InitTextControl(fRed);
	
	r.OffsetBy(0, o);
	tr.OffsetBy(0, o);

	c.red = 0;
	c.green = 255;
	fGreenSlider = new CColorSlider(r, "greenslider", c);
	AddChild(fGreenSlider);
	
	AddChild(fGreen = new BTextControl(tr, "green", NULL, "0", NULL));
	InitTextControl(fGreen);

	r.OffsetBy(0, o);
	tr.OffsetBy(0, o);
	
	c.green = 0;
	c.blue = 255;
	fBlueSlider = new CColorSlider(r, "blueslider", c);
	AddChild(fBlueSlider);
	
	AddChild(fBlue = new BTextControl(tr, "blue", NULL, "0", NULL));
	InitTextControl(fBlue);
} /* CRGBView::CRGBView */

CRGBView::~CRGBView()
{
} /* CRGBView::~CRGBView */

void CRGBView::InitTextControl(BTextControl *ctl)
{
	BTextView *tv = ctl->TextView();
	tv->SetMaxBytes(3);
	
	for (int i = 0; i < 255; i++)
		tv->DisallowChar(i);
	
	tv->AllowChar('0');
	tv->AllowChar('1');
	tv->AllowChar('2');
	tv->AllowChar('3');
	tv->AllowChar('4');
	tv->AllowChar('5');
	tv->AllowChar('6');
	tv->AllowChar('7');
	tv->AllowChar('8');
	tv->AllowChar('9');
	
	tv->SetAlignment(B_ALIGN_RIGHT);
	tv->MakeResizable(false);
	
	BMessage *msg = new BMessage(msg_RawChange);
	msg->AddPointer("sender", ctl);
	ctl->SetModificationMessage(msg);
	ctl->SetTarget(ctl);
} /* CRGBView::InitTextControl */

void CRGBView::Draw(BRect updateRect)
{
	BBox::Draw(updateRect);
} /* CRGBView::Draw */
	
void CRGBView::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case msg_SliderChanged:
			UpdateTextFields();
			fOwner->SetRGB(*fRedSlider, *fGreenSlider, *fBlueSlider);
			break;
	
		case msg_RawChange:
		{
			BTextControl *sender;
			if (msg->FindPointer("sender", (void **)&sender) == B_NO_ERROR &&
				sender->TextView()->IsFocus())
			{
				if (atoi(sender->Text()) > 255)
				{
					beep();
					sender->SetText("255");
					sender->TextView()->SelectAll();
				}
				
				*fRedSlider = min(1.0, max(0.0, atof(fRed->Text()) / 255.0));
				*fGreenSlider = min(1.0, max(0.0, atof(fGreen->Text()) / 255.0));
				*fBlueSlider = min(1.0, max(0.0, atof(fBlue->Text()) / 255.0));

				fOwner->SetRGB(*fRedSlider, *fGreenSlider, *fBlueSlider);
				fOwner->PostMessage(msg_EndTracking);
			}
			break;
		}
			
		default:
			BBox::MessageReceived(msg);
	}
} /* CRGBView::MessageReceived */

void CRGBView::AttachedToWindow()
{
	fOwner = (CColorPicker *)Window();
} /* CRGBView::AttachedToWindow */

void CRGBView::SetColor(roSColor& newColor)
{
	*fRedSlider = newColor.m_Red;
	*fGreenSlider = newColor.m_Green;
	*fBlueSlider = newColor.m_Blue;

	UpdateTextFields();
} /* CRGBView::SetColor */

void CRGBView::UpdateTextFields()
{
	rgb_color c = f2rgb(*fRedSlider, *fGreenSlider, *fBlueSlider);

	char s[10];
	sprintf(s, "%d", c.red);
	if (strcmp(fRed->Text(), s))
		fRed->SetText(s);
	sprintf(s, "%d", c.green);
	if (strcmp(fGreen->Text(), s))
		fGreen->SetText(s);
	sprintf(s, "%d", c.blue);
	if (strcmp(fBlue->Text(), s))
		fBlue->SetText(s);
} /* CRGBColor::UpdateTextFields */

void CRGBView::MouseMoved(BPoint, uint32, const BMessage *)
{
	be_app->SetCursor(B_HAND_CURSOR);
} /* CRGBView::MouseMoved */
