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
	ColorSlider
	
	Copyright 1997, Hekkelman Programmatuur
*/

#include "ColorSlider.h"
#include "ColorPicker.h"

CColorSlider::CColorSlider(BRect frame, const char *name, rgb_color max)
	: BView(frame, name, 0, B_WILL_DRAW | B_NAVIGABLE)
{
	fMax = max;
	fValue = 0;
	fHorizontal = frame.Width() > frame.Height();
} /* CColorSlider::CColorSlider */

CColorSlider::~CColorSlider()
{
} /* CColorSlider::~CColorSlider */

void CColorSlider::SetMax(rgb_color max)
{
	fMax = max;
	Draw(Bounds());
} /* CColorSlider::SetMax */

void CColorSlider::Draw(BRect /* updateRect */)
{
	BRect r = Bounds();
	
	if (IsFocus())
	{
		SetHighColor(keyboard_navigation_color());
		StrokeRect(r);
	}
	else
	{
		SetHighColor(kB_GrayTable[13]);
		StrokeLine(r.LeftTop(), r.LeftBottom());
		StrokeLine(r.LeftTop(), r.RightTop());
		SetHighColor(kWhite);
		StrokeLine(r.LeftBottom(), r.RightBottom());
		StrokeLine(r.RightTop(), r.RightBottom());
	}

	r.InsetBy(1, 1);

	BRect knob;
	float m = std::min(r.Width(), r.Height()), a;
	knob.Set(1, 1, m + 1, m + 1);
	
	if (fHorizontal)
	{
		a = r.Width() - m;
		knob.OffsetBy(a * fValue, 0);
		
		BeginLineArray((int)a + 1);
		
		float x = r.left;
		for (;x <= r.right; x += 1)
		{
			float v = (x - 1) / a;
			rgb_color c = fMax;
			c.red = (int)((float)c.red * v);
			c.green = (int)((float)c.green * v);
			c.blue = (int)((float)c.blue * v);
			
			float X = x < knob.left ? x : x + knob.Width();
			
			AddLine(BPoint(X, r.top), BPoint(X, r.bottom), c);
		}
		
		EndLineArray();
	}
	else
	{
		a = r.Height() - m;
		knob.OffsetBy(0, a * (1.0 - fValue));
		
		BeginLineArray((int)a + 1);
		
		float y = r.top;
		for (;y <= r.bottom; y += 1)
		{
			float v = 1.0 - (y - 1) / a;
			rgb_color c = fMax;
			c.red = (int)((float)c.red * v);
			c.green = (int)((float)c.green * v);
			c.blue = (int)((float)c.blue * v);
			
			float Y = y < knob.top ? y : y + knob.Height();
			
			AddLine(BPoint(r.left, Y), BPoint(r.right, Y), c);
		}
		
		EndLineArray();
	}
	
	SetHighColor(kWhite);
	StrokeLine(knob.LeftTop(), knob.LeftBottom());
	StrokeLine(knob.LeftTop(), knob.RightTop());
	SetHighColor(kB_GrayTable[13]);
	StrokeLine(knob.LeftBottom(), knob.RightBottom());
	StrokeLine(knob.RightTop(), knob.RightBottom());
	
	SetHighColor(kGray);
	knob.InsetBy(1, 1);
	StrokeRect(knob);
	
	knob.InsetBy(1, 1);
	rgb_color c = fMax;
	c.red = (int)((float)c.red * fValue);
	c.green = (int)((float)c.green * fValue);
	c.blue = (int)((float)c.blue * fValue);
	SetHighColor(c);
	FillRect(knob);

	SetHighColor(kBlack);
	
	Sync();
} /* CColorSlider::Draw */

void CColorSlider::MouseDown(BPoint where)
{
	if (!IsFocus())
		MakeFocus(true);
		
	BRect b(Bounds()), knob;
	
	b.InsetBy(1, 1);
	
	float a, s;
	
	knob = b;
	
	if (fHorizontal)
	{
		knob.right = b.left + b.Height();
		a = b.Width() - knob.Width();
		knob.OffsetBy(fValue * a, 0);
		s = where.x - knob.left;
	}
	else
	{
		knob.bottom = b.top + b.Width();
		a = b.Height() - knob.Height();
		knob.OffsetBy(0, (1 - fValue) * a);
		s = where.y - knob.top;
	}
	
	if (knob.Contains(where))
	{
		BPoint oldPt(-1, -1);
		uint32 btns;
		BMessage msg(msg_SliderChanged);
		msg.AddFloat("value", fValue);
		msg.AddPointer("sender", this);
		
		do
		{
			if (oldPt != where)
			{
				if (fHorizontal)
					fValue = std::min(1.0F, std::max(0.0F, (where.x - s) / a));
				else
					fValue = 1 - std::min(1.0F, std::max(0.0F, (where.y - s) / a));
				
				msg.ReplaceFloat("value", fValue);
				
				Draw(Bounds());
				MessageReceived(&msg);
				
				oldPt = where;
			}
			
			GetMouse(&where, &btns);
		}
		while (btns);
		
		Window()->PostMessage(msg_EndTracking);
	}
	else // TPV 00-02-27
	{
		float pos , range ;
		if (fHorizontal)
		{
			pos = where.x - knob.Width()/2;
			range = a ;
		}
		else
		{
			pos = a - ( where.y - knob.Width()/2 ) ;
			range = a ;
		}
		if( pos < 0 ) pos = 0 ;
		else if( pos > range ) pos = range ;

		fValue = pos/range ;
		ColorChanged() ;
	}		
} /* CColorSlider::MouseDown */

void CColorSlider::ColorChanged()
{
	Draw(Bounds()) ;
	BMessage msg(msg_SliderChanged);
	msg.AddFloat("value", fValue);
	msg.AddPointer("sender", this);
	MessageReceived(&msg);
}	

float CColorSlider::Value() const
{
	return fValue;
} /* CColorSlider::Value */

void CColorSlider::SetValue(float v)
{
	fValue = v;
	Draw(Bounds());
} /* CColorSlider::SetValue */

void CColorSlider::MakeFocus(bool focus)
{
	BView::MakeFocus(focus);
	Draw(Bounds());
} /* CColorSlider::MakeFocus */

void CColorSlider::KeyDown(const char *bytes, int32 numBytes)
{
	BRect r(Bounds());
	
	r.InsetBy(1, 1);
	float a, nv = fValue;
	
	if (fHorizontal)
		a = r.Width() - r.Height();
	else
		a = r.Height() - r.Width();
	
	switch (bytes[0])
	{
		case B_UP_ARROW:
			if (!fHorizontal)
				nv = std::min(1.0F, (fValue * a + 1) / a);
			break;
			
		case B_LEFT_ARROW:
			if (fHorizontal)
				nv = std::max(0.0F, (fValue * a - 1) / a);
			break;
		
		case B_DOWN_ARROW:
			if (!fHorizontal)
				nv = std::max(0.0F, (fValue * a - 1) / a);
			break;
		
		case B_RIGHT_ARROW:
			if (fHorizontal)
				nv = std::min(1.0F, (fValue * a + 1) / a);
			break;
		
		default:
			BView::KeyDown(bytes, numBytes);
	}
	
	if (nv != fValue)
	{
		fValue = nv;
		ColorChanged() ;
	}
} /* CColorSlider::KeyDown */
