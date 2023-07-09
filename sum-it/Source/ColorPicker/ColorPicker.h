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

#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <InterfaceKit.h>
#include <algorithm>

const rgb_color
	kGray = { 0xd8, 0xd8, 0xd8, 0},
	kDGray = { 0x90, 0x90, 0x90, 0},
	kWhite = { 0xff, 0xff, 0xff, 0},
	kBlack = { 0x00, 0x00, 0x00, 0};

const rgb_color
	kB_GrayTable[] = {
		{ 0,    0,	  0,    0, }, // 0
		{ 0xFF, 0xFF, 0xFF, 0, }, // 1
		{ 0xF8, 0xF8, 0xF8, 0, }, // 1	2
		{ 0xF0, 0xF0, 0xF0, 0, }, // 1	3
		{ 0xE8, 0xE8, 0xE8, 0, }, // 2	4
		{ 0xE0, 0xE0, 0xE0, 0, }, // 3	5
		{ 0xD8, 0xD8, 0xD8, 0, }, // 4	6
		{ 0xD0, 0xD0, 0xD0, 0, }, // 5	7
		{ 0xC8, 0xC8, 0xC8, 0, }, // 6	8
		{ 0xC0, 0xC0, 0xC0, 0, }, // 7	9
		{ 0xB8, 0xB8, 0xB8, 0, }, // 8	10
		{ 0xB0, 0xB0, 0xB0, 0, }, // 9	11
		{ 0xA8, 0xA8, 0xA8, 0, }, // 10	12
		{ 0xA0, 0xA0, 0xA0, 0, }, // 11	13
		{ 0x98, 0x98, 0x98, 0, }, // 12	14
		{ 0x90, 0x90, 0x90, 0, }, // 13	15
		{ 0x88, 0x88, 0x88, 0, }, // 14	16
		{ 0x80, 0x80, 0x80, 0, }, // 15	17
		
		{ 0x60, 0x60, 0x60, 0, }, // 16	18
	};

const uint32
	msg_EndTracking = 'EndT';

struct roSColor
{ 
    float  m_Red; 
    float  m_Green; 
    float  m_Blue; 
    float  m_Alpha; 
    float  m_Hue; 
};

class CHSVView;
class CONView;
class CRGBView;

class CColorPicker : public BWindow {
public:
	CColorPicker(BRect frame, const char *name, bool stayOpen = false);
	~CColorPicker();
	
	void Connect(BMessage *msg, BHandler *caller);
	void MessageReceived(BMessage *msg);
	bool QuitRequested();
	
	void SetColor(rgb_color oldColor);
	rgb_color operator= (rgb_color oldColor);
	operator rgb_color() const;
	operator roSColor() const;
	
	static BRect NicePosition();
	
	void SetHSV(float h, float s, float v);
	void SetRGB(float r, float g, float b);
	
private:
	
	void PostNewColor();
	
	BHandler *fCaller;
	BMessage fMessage;
	roSColor fNewColor, fOldColor;
	CHSVView *fHSV;
	CRGBView *fRGB;
	CONView *fON;
	bool fStayOpen;
};

void rgb2ro(rgb_color rgb, roSColor& ro);
rgb_color ro2rgb(roSColor& ro);
void rgb2hsv(float r, float g, float b, float& h, float& s, float& v);
void hsv2rgb(float h, float s, float v, float& r, float& g, float& b);
void rgb2f(rgb_color rgb, float& r, float& g, float& b, float& a);
rgb_color f2rgb(float r, float g, float b, float a = 1.0);

#endif
