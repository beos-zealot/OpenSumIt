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

#include "HSVView.h"
#include "ColorSlider.h"

#include <Application.h>

const int kDiameter = 100;

CHSVView::CHSVView(BRect frame, const char *name)
	: BBox(frame, name)
{
	SetViewColor(kGray);
	SetLowColor(kGray);
	
	SetLabel("HSV");
	SetFont(be_plain_font);
	
	fWheelR.Set(0, 0, kDiameter, kDiameter);
	fBitmap = new BBitmap(fWheelR, B_RGB_32_BIT, true);
	if (!fBitmap) throw -108;
	fBitmap->AddChild(new BView(fWheelR, "bitmapview", 0, B_WILL_DRAW));

	font_height fi;
	be_plain_font->GetHeight(&fi);
	fWheelR.OffsetTo(6, fi.ascent + fi.descent);

	BRect vr = fWheelR;
	vr.left = fWheelR.right + 9;
	vr.right = vr.left + 12;

	rgb_color c = { 255, 255, 255, 0 };
	fValue = new CColorSlider(vr, "valueslider", c);
	AddChild(fValue);
	
	int bitcount = (kDiameter + 1) * (kDiameter + 1);
	
	fHues = (float *)malloc(bitcount * sizeof(float));
	fSaturations = (float *)malloc(bitcount * sizeof(float));
	
	if (!fHues || !fSaturations) throw -108;
	
	float x, y;
	float radius;
	int diameter;

	diameter = kDiameter;
	radius = diameter / 2;
	
	for (y = 0; y < diameter; y += 1)
	{
		int iy = (int)y;
		
		for (x = 0; x < diameter; x += 1)
		{
			int ix = (int)x, indx;
			float dx, dy;
			
			indx = iy * diameter + ix;
			
			dx = x - radius + 0.00001;
			dy = radius - y;
			
			fSaturations[indx] = sqrt(dx * dx + dy * dy) / radius;
			
			if (fSaturations[indx] <= 1.0)
			{
				if (dx > 0)
					fHues[indx] = (0.5 * M_PI - atan(dy/dx)) / (2 * M_PI);
				else
					fHues[indx] = (1.5 * M_PI - atan(dy/dx)) / (2 * M_PI);
			}
			else
				fSaturations[indx] = 2.0;
		}
	}
} /* CHSVView::CHSVView */

CHSVView::~CHSVView()
{
	if (fBitmap) delete fBitmap;
	if (fHues) free(fHues);
	if (fSaturations) free(fSaturations);
} /* CHSVView::~CHSVView */

void CHSVView::Draw(BRect updateRect)
{
	BBox::Draw(updateRect);
	DrawWheel();
} /* CHSVView::Draw */
	
void CHSVView::MessageReceived(BMessage *msg)
{
	if (msg->what == msg_SliderChanged)
	{
		UpdateBitmap();
		DrawWheel();
		fOwner->SetHSV(fHue, fSaturation, *fValue);
	}
	else
		BBox::MessageReceived(msg);
} /* CHSVView::MessageReceived */

void CHSVView::UpdateBitmap()
{
	int x, y;
	
	if (fBitmap->Lock())
	{
		BView *v = fBitmap->ChildAt(0);
		v->SetHighColor(kGray);
		v->FillRect(v->Bounds());
		v->Sync();
		
		for (y = 0; y < kDiameter; y++)
		{
			rgb_color *row =
				(rgb_color *)((char *)fBitmap->Bits() + y * fBitmap->BytesPerRow());
			
			for (x = 0; x < kDiameter; x++)
			{
				int indx = kDiameter * y + x;
	
				if (fSaturations[indx] != 2.0)
				{
					float r, g, b;
					hsv2rgb(fHues[indx], fSaturations[indx], *fValue, r, g, b);
					
					row[x].red = (int)(b * 255.0);
					row[x].green = (int)(g * 255.0);
					row[x].blue = (int)(r * 255.0);
				}
			}
		}
		
		BRect b = v->Bounds();
		if (IsFocus())
		{
			v->SetHighColor(keyboard_navigation_color());
			v->StrokeArc(b, 0, 360);
		}
		else
		{
			v->SetHighColor(kB_GrayTable[13]);
			v->StrokeArc(b, 60, 150);
			v->SetHighColor(kWhite);
			v->StrokeArc(b, 230, 150);
		}
		v->SetHighColor(kBlack);
		v->Sync();
		
		fBitmap->Unlock();
	}
} /* CHSVView::UpdateBitmap */

void CHSVView::MouseDown(BPoint where)
{
	if (!IsFocus())
		MakeFocus(true);
		
	float x, y, rad;
	rad = kDiameter / 2;
	x = fWheelR.left + rad + fSaturation * sin(fHue * 2 * M_PI) * rad;
	y = fWheelR.top + rad - fSaturation * cos(fHue * 2 * M_PI) * rad;
	
	BRect knob(x - 3.5, y - 3.5, x + 3.5, y + 3.5);

	if (knob.Contains(where))
	{
		BPoint oldPt(-1, -1);
		uint32 btns;
		
		do
		{
			float radius = kDiameter / 2;
			
			if (oldPt != where)
			{
				float dx, dy;
				
				dx = where.x - radius - fWheelR.left + 0.0001;
				dy = radius - where.y + fWheelR.top;
				
				fSaturation = sqrt(dx * dx + dy * dy) / radius;
				
				if (fSaturation > 1.0)
					fSaturation = 1.0;

				if (dx > 0)
					fHue = (0.5 * M_PI - atan(dy/dx)) / (2 * M_PI);
				else
					fHue = (1.5 * M_PI - atan(dy/dx)) / (2 * M_PI);

				DrawWheel();
				
				float r, g, b;
				hsv2rgb(fHue, fSaturation, 1.0, r, g, b);
				fValue->SetMax(f2rgb(r, g, b));

				fOwner->SetHSV(fHue, fSaturation, *fValue);

				oldPt = where;
			}
			GetMouse(&where, &btns);
		}
		while (btns);

		Window()->PostMessage(msg_EndTracking);
	}
	else
		BBox::MouseDown(where);
} /* CHSVView::MouseDown */

void CHSVView::DrawWheel()
{
	BRect r(fWheelR);

	r.InsetBy(-2, -2);
	
	SetPenSize(4);
	SetHighColor(kGray);
	
	StrokeLine(BPoint(r.left + 25, r.top), BPoint(r.left + 75, r.top));
	StrokeLine(BPoint(r.left + 25, r.bottom), BPoint(r.left + 75, r.bottom));
	StrokeLine(BPoint(r.left, r.top + 25), BPoint(r.left, r.top + 75));
	StrokeLine(BPoint(r.right, r.top + 25), BPoint(r.right, r.top + 75));
	
	SetPenSize(1);

	DrawBitmap(fBitmap, fWheelR);

	float x, y, rad;
	rad = kDiameter / 2;
	x = fWheelR.left + rad + fSaturation * sin(fHue * 2 * M_PI) * rad;
	y = fWheelR.top + rad - fSaturation * cos(fHue * 2 * M_PI) * rad;
	
	StrokeEllipse(BPoint(x, y), 2.5, 2.5);
	
	BRect knob(x - 3.5, y - 3.5, x + 3.5, y + 3.5);

	SetHighColor(kWhite);
	StrokeArc(knob, 45, 180);
	SetHighColor(kB_GrayTable[13]);
	StrokeArc(knob, 225, 180);
	
	SetHighColor(kBlack);
} /* CHSVView::DrawWheel */

void CHSVView::AttachedToWindow()
{
	fOwner = (CColorPicker *)Window();
} /* CHSVView::AttachedToWindow */

void CHSVView::SetColor(roSColor& newColor)
{
	fHue = newColor.m_Hue;

	float cmax, cmin, delta;
	cmax = std::max(newColor.m_Red, std::max(newColor.m_Green, newColor.m_Blue));
	cmin = std::min(newColor.m_Red, std::min(newColor.m_Green, newColor.m_Blue));
	delta = cmax - cmin;
	
	*fValue = cmax;
	fSaturation = cmax ? delta / cmax : 0.0;
	
	fValue->SetMax(f2rgb(newColor.m_Red, newColor.m_Green, newColor.m_Blue));
	
	UpdateBitmap();
	DrawWheel();
} /* CHSVView::SetColor */

void CHSVView::MakeFocus(bool focus)
{
	BBox::MakeFocus(focus);
	UpdateBitmap();
	DrawWheel();
} /* CHSVView::MakeFocus */

void CHSVView::KeyDown(const char *bytes, int32 numBytes)
{
	float x, nx, y, ny, rad;
	rad = kDiameter / 2;
	nx = x = fWheelR.left + rad + fSaturation * sin(fHue * 2 * M_PI) * rad;
	ny = y = fWheelR.top + rad - fSaturation * cos(fHue * 2 * M_PI) * rad;

	switch (bytes[0])
	{
		case B_UP_ARROW:
			ny = std::max(fWheelR.top, y - 1);
			break;
		
		case B_DOWN_ARROW:
			ny = std::min(fWheelR.bottom, y + 1);
			break;
		
		case B_LEFT_ARROW:
			nx = std::max(fWheelR.left, x - 1);
			break;
		
		case B_RIGHT_ARROW:
			nx = std::min(fWheelR.right, x + 1);
			break;
		
		default:
			BBox::KeyDown(bytes, numBytes);
	}
	
	if (nx != x || ny != y)
	{
		float dx, dy;
		
		dx = nx - rad - fWheelR.left + 0.0001;
		dy = rad - ny + fWheelR.top;
		
		fSaturation = sqrt(dx * dx + dy * dy) / rad;
		
		if (fSaturation > 1.0)
			fSaturation = 1.0;

		if (dx > 0)
			fHue = (0.5 * M_PI - atan(dy/dx)) / (2 * M_PI);
		else
			fHue = (1.5 * M_PI - atan(dy/dx)) / (2 * M_PI);

		DrawWheel();
		
		float r, g, b;
		hsv2rgb(fHue, fSaturation, 1.0, r, g, b);
		fValue->SetMax(f2rgb(r, g, b));

		fOwner->SetHSV(fHue, fSaturation, *fValue);
	}
} /* CHSVView::KeyDown */

void CHSVView::MouseMoved(BPoint, uint32, const BMessage *)
{
	be_app->SetCursor(B_HAND_CURSOR);
} /* CHSVView::MouseMoved */
