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
	ColorPicker
	
	Copyright 1997, Hekkelman Programmatuur
*/

#include "ColorPicker.h"
#include "HSVView.h"
#include "ONView.h"
#include "RGBView.h"

#ifndef   KEYFILTER_H
#include "KeyFilter.h"
#endif

const uint32
	msg_OK = 'OK  ',
	msg_Cancel = 'Cncl',
	msg_ManualChange = 'MnCh';

CColorPicker::CColorPicker(BRect frame, const char *name, bool stayOpen)
	: BWindow(frame, name, stayOpen ? B_TITLED_WINDOW : B_MODAL_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	fStayOpen = stayOpen;
	fCaller = NULL;
	
	BView *main = new BView(Bounds(), "main", 0, B_WILL_DRAW);
	AddChild(main);
	main->SetViewColor(kGray);
	
	BRect r(10, 10, 145, 130);
	fHSV = new CHSVView(r, "hsvview");
	main->AddChild(fHSV);
	
	r.Set(155, 10, 205, 130);
	fON = new CONView(r, "o&n");
	main->AddChild(fON);

	r.Set(215, 10, 380, 130);
	fRGB = new CRGBView(r, "rgbview");
	main->AddChild(fRGB);

	if (fStayOpen)
	{
		BRect b(Bounds());
		
		ResizeTo(b.Width(), b.Height() - 30);
	}
	else
	{
		r.Set(320, 140, 380, 160);
		BButton * ok = new BButton(r, "ok", "OK", new BMessage(msg_OK));
		main->AddChild(ok);
		ok->MakeDefault(true);
		
		r.Set(250, 140, 310, 160);
		main->AddChild(new BButton(r, "cancel", "Cancel", new BMessage(msg_Cancel)));

		// TPV 00-02-27
        KeyFilter * kf = new KeyFilter(B_ESCAPE, msg_Cancel) ;
        AddCommonFilter( kf ) ;
	}
} /* CColorPicker::CColorPicker */

CColorPicker::~CColorPicker()
{
} /* CColorPicker::~CColorPicker */

bool CColorPicker::QuitRequested()
{
	if (fStayOpen)
	{
		Hide();
		return false;
	}
	else
		return true;
} /* CColorPicker::QuitRequested */

void CColorPicker::Connect(BMessage *msg, BHandler *caller)
{
	fCaller = caller;
	fMessage = *msg;

	rgb_color *c;
	ssize_t l;
	
	if (msg && msg->FindData("color", 'RGBC', (const void **)&c, &l) == B_NO_ERROR)
	{
		rgb2ro(*c, fNewColor);
		fOldColor = fNewColor;
		
		fHSV->SetColor(fNewColor);
		fRGB->SetColor(fNewColor);

		fON->SetOldColor(*c);
		fON->SetNewColor(*c);
	}

	Show();
} /* CColorPicker::Connect */

void CColorPicker::MessageReceived(BMessage *msg)
{
	roSColor *c;
	ssize_t l;
	
	if (msg->WasDropped() && msg->FindData("roColour", 'roCr', (const void **)&c, &l) == B_NO_ERROR)
	{
		fNewColor = *c;
		fHSV->SetColor(fNewColor);
		fRGB->SetColor(fNewColor);
		fON->SetNewColor(f2rgb(fNewColor.m_Red, fNewColor.m_Green,
			fNewColor.m_Blue, fNewColor.m_Alpha));
		return;
	}

	switch (msg->what)
	{
		case msg_EndTracking:
			if (fStayOpen)
				PostNewColor();
			break;
		
		case msg_RawChange:
			fRGB->MessageReceived(msg);
			break;
	
		case msg_ManualChange:
			fHSV->SetColor(fNewColor);
			fON->SetOldColor(f2rgb(fNewColor.m_Red, fNewColor.m_Green,
				fNewColor.m_Blue, fNewColor.m_Alpha));
			fON->SetNewColor(f2rgb(fNewColor.m_Red, fNewColor.m_Green,
				fNewColor.m_Blue, fNewColor.m_Alpha));
			fRGB->SetColor(fNewColor);
			break;
	
		case msg_OK:
			PostNewColor();
		// fall through
		case msg_Cancel:
			Quit();
			break;
		
		default:
			BWindow::MessageReceived(msg);
	}
} /* CColorPicker::MessageReceived */

void CColorPicker::SetColor(rgb_color oldColor)
{
	rgb2ro(oldColor, fOldColor);
	fNewColor = fOldColor;
	
	PostMessage(msg_ManualChange);
} /* CColorPicker::SetColor */

rgb_color CColorPicker::operator= (rgb_color oldColor)
{
	SetColor(oldColor);
	return oldColor;
} /* CColorPicker::SetColor */

//void CColorPicker::

void CColorPicker::SetHSV(float h, float s, float v)
{
	fNewColor.m_Hue = h;
	hsv2rgb(h, s, v, fNewColor.m_Red, fNewColor.m_Green, fNewColor.m_Blue);
	
	fON->SetNewColor(f2rgb(fNewColor.m_Red, fNewColor.m_Green,
		fNewColor.m_Blue, fNewColor.m_Alpha));
	
	fRGB->SetColor(fNewColor);
} /* CColorPicker::SetHSV */

void CColorPicker::SetRGB(float r, float g, float b)
{
	fNewColor.m_Red = r;
	fNewColor.m_Green = g;
	fNewColor.m_Blue = b;
	
	float s, v;
	
	rgb2hsv(r, g, b, fNewColor.m_Hue, s, v);
	
	fON->SetNewColor(f2rgb(r, g, b, fNewColor.m_Alpha));
	
	fHSV->SetColor(fNewColor);
} /* CColorPicker::SetRGB */

BRect CColorPicker::NicePosition()
{
	BRect p(0, 0, 390, 170);
	BScreen s;
	
	p.OffsetBy((s.Frame().Width() - p.Width()) / 2, (s.Frame().Height() - p.Height()) / 3);
	
	return p;
} /* CColorPicker::NicePosition */

CColorPicker::operator rgb_color() const
{
	return f2rgb(fNewColor.m_Red, fNewColor.m_Green, fNewColor.m_Blue, fNewColor.m_Alpha);
} /* CColorPicker::operator rgb_color */

CColorPicker::operator roSColor() const
{
	return fNewColor;
} /* CColorPicker::operator rgb_color */

void CColorPicker::PostNewColor()
{
	rgb_color c = f2rgb(fNewColor.m_Red, fNewColor.m_Green, fNewColor.m_Blue, 1.0);
	fMessage.ReplaceData("color", B_RGB_COLOR_TYPE, &c, 4);
	fCaller->Looper()->PostMessage(&fMessage, fCaller);
} /* CColorPicker::PostNewColor */

//////////////////////////////////////////////////////
//
//   Color conversion routines
//

void rgb2f(rgb_color rgb, float& r, float& g, float& b, float& a)
{
	r = rgb.red / 255.0;
	g = rgb.green / 255.0;
	b = rgb.blue / 255.0;
	a = rgb.alpha / 255.0;
} /* rgb2f */

rgb_color f2rgb(float r, float g, float b, float a)
{
	rgb_color rgb;

	rgb.red   = std::max(0, std::min(255, static_cast<int>(r * 255) ));
	rgb.green = std::max(0, std::min(255, static_cast<int>(g * 255) ));
	rgb.blue  = std::max(0, std::min(255, static_cast<int>(b * 255) ));
	rgb.alpha = std::max(0, std::min(255, static_cast<int>(a * 255) ));

	return rgb;
} /* f2rgb */

void rgb2hsv(float r, float g, float b, float& h, float& s, float& v)
{
	float cmin, cmax, delta;
	
	cmax = std::max(r, std::max(g, b));
	cmin = std::min(r, std::min(g, b));
	delta = cmax - cmin;
	
	v = cmax;
	s = cmax ? delta / cmax : 0.0;

	if (s == 0.0)
		h = -1;
	else
	{
		if (r == cmax)
			h = (g - b) / delta;
		else if (g == cmax)
			h = 2 + (b - r) / delta;
		else if (b == cmax)
			h = 4 + (r - g) / delta;
		h /= 6.0;
	}
} /* rgb2hsv */

void hsv2rgb(float h, float s, float v, float& r, float& g, float& b)
{
	float A, B, C, F;
	int i;
	
	if (s == 0.0)
		r = g = b = v;
	else
	{
		if (h >= 1.0 || h < 0.0)
			h = 0.0;
		h *= 6.0;
		i = static_cast<int>(floor(h)) ;
		F = h - i;
		A = v * (1 - s);
		B = v * (1 - (s * F));
		C = v * (1 - (s * (1 - F)));
		switch (i)
		{
			case 0:	r = v; g = C; b = A; break;
			case 1:	r = B; g = v; b = A; break;
			case 2:	r = A; g = v; b = C; break;
			case 3:	r = A; g = B; b = v; break;
			case 4:	r = C; g = A; b = v; break;
			case 5:	r = v; g = A; b = B; break;
		}
	}
} /* hsv2rgb */

void rgb2ro(rgb_color rgb, roSColor& ro)
{
	rgb2f(rgb, ro.m_Red, ro.m_Green, ro.m_Blue, ro.m_Alpha);
	
	float sat, val;
	rgb2hsv(ro.m_Red, ro.m_Green, ro.m_Blue, ro.m_Hue, sat, val);
} /* rgb2ro */

rgb_color ro2rgb(roSColor& ro)
{
	return f2rgb(ro.m_Red, ro.m_Green, ro.m_Blue);
} /* ro2rgb */

