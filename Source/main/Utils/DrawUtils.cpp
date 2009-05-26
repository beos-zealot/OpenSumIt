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
	DrawUtils.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#include <Point.h>
#include <Font.h>
#include <View.h>

const rgb_color
	kColorTable[] =
	{
		{ 0x00, 0x00, 0x00, 0x00 },		// Black
		{ 0x8F, 0x00, 0x00, 0x00 },		// Red
		{ 0x00, 0x8F, 0x00, 0x00 },		// Green
		{ 0x00, 0x00, 0x8F, 0x00 },		// Blue
		{ 0x00, 0x8F, 0x8F, 0x00 },		// Cyan
		{ 0x8F, 0x00, 0x8F, 0x00 },		// Magenta
		{ 0x8F, 0x8F, 0x00, 0x00 }		// Yellow
	};

const rgb_color
	kGrayTable[] =
	{
		{ 0, 	  0, 	0,	  0 }, // B
		{ 0xFF, 0xFF, 0xFF, 0 }, // W
		{ 0xEE, 0xEE, 0xEE, 0 }, // 1
		{ 0xDD, 0xDD, 0xDD, 0 }, // 2
		{ 0xCC, 0xCC, 0xCC, 0 }, // 3
		{ 0xBB, 0xBB, 0xBB, 0 }, // 4
		{ 0xAA, 0xAA, 0xAA, 0 }, // 5
		{ 0x99, 0x99, 0x99, 0 }, // 6
		{ 0x88, 0x88, 0x88, 0 }, // 7
		{ 0x77, 0x77, 0x77, 0 }, // 8
		{ 0x66, 0x66, 0x66, 0 }, // 9
		{ 0x55, 0x55, 0x55, 0 }, // 10
		{ 0x44, 0x44, 0x44, 0 }, // 11
		{ 0x33, 0x33, 0x33, 0 }  // 12
	};

const rgb_color
	kB_GrayTable[] = {
		{ 0,	  0,	0,	  0 }, // 0
		{ 0xFF, 0xFF, 0xFF, 0 }, // 1
		{ 0xF8, 0xF8, 0xF8, 0 }, // 1	2
		{ 0xF0, 0xF0, 0xF0, 0 }, // 1	3
		{ 0xE8, 0xE8, 0xE8, 0 }, // 2	4
		{ 0xE0, 0xE0, 0xE0, 0 }, // 3	5
		{ 0xD8, 0xD8, 0xD8, 0 }, // 4	6
		{ 0xD0, 0xD0, 0xD0, 0 }, // 5	7
		{ 0xC8, 0xC8, 0xC8, 0 }, // 6	8
		{ 0xC0, 0xC0, 0xC0, 0 }, // 7	9
		{ 0xB8, 0xB8, 0xB8, 0 }, // 8	10
		{ 0xB0, 0xB0, 0xB0, 0 }, // 9	11
		{ 0xA8, 0xA8, 0xA8, 0 }, // 10	12
		{ 0xA0, 0xA0, 0xA0, 0 }, // 11	13
		{ 0x98, 0x98, 0x98, 0 }, // 12	14
		{ 0x90, 0x90, 0x90, 0 }, // 13	15
		{ 0x88, 0x88, 0x88, 0 }, // 14	16
		{ 0x80, 0x80, 0x80, 0 }, // 15	17
		
		{ 0x60, 0x60, 0x60, 0 } // 16	18
	};

void FillRect3D(BView *inView, BRect r, bool pushed, bool enabled,
	const char *label)
{
	rgb_color high, low;
	high = inView->HighColor();
	low = inView->LowColor();
	
	if (!inView->IsPrinting())
	{
		if (pushed)
		{
			inView->SetHighColor(kB_GrayTable[0]);
			inView->SetLowColor(kB_GrayTable[8]);
		}
		else if (enabled)
		{
			inView->SetHighColor(kB_GrayTable[0]);
			inView->SetLowColor(kB_GrayTable[6]);
		}
		else
		{
			inView->SetHighColor(kB_GrayTable[8]);
			inView->SetLowColor(kB_GrayTable[4]);
		}
	}

	float w, h;
	
	w = r.Width();
	h = r.Height();

	if (w > 0 && h > 0)
		inView->FillRect(r, B_SOLID_LOW);

	if (w > 2 && h > 2)
	{
		if (label)
		{
			BFont currentFont;
			inView->GetFont(&currentFont);
			font_height fi;
			currentFont.GetHeight(&fi);
			
			float w = inView->StringWidth(label);
			
			if ((r.Height() >= (fi.ascent + fi.descent - 2.0)) &&
				((r.Width() - 1) > w))
			{
				inView->MovePenTo(r.left + (r.right - r.left - w) / 2 + 1,
					r.bottom - fi.descent + 1);
				inView->DrawString(label);
			}
		}
		
		if (enabled && !inView->IsPrinting())
		{
			inView->BeginLineArray(pushed ? 6 : 4);
			
			inView->AddLine(BPoint(r.left, r.bottom - 1),
					BPoint(r.left, r.top),
					kB_GrayTable[pushed ? 10 : 1]);
			inView->AddLine(BPoint(r.left, r.top),
					BPoint(r.right - 1, r.top),
					kB_GrayTable[pushed ? 10 : 1]);
	
			if (pushed)
			{
				inView->AddLine(BPoint(r.left + 1, r.bottom - 1),
						BPoint(r.left + 1, r.top + 1),
						kB_GrayTable[10]);
				inView->AddLine(BPoint(r.left + 1, r.top + 1),
						BPoint(r.right - 1, r.top + 1),
						kB_GrayTable[10]);
			}
	
			inView->AddLine(BPoint(r.left + 1, r.bottom),
					BPoint(r.right, r.bottom),
					kB_GrayTable[pushed ? 6 : 10]);
			inView->AddLine(BPoint(r.right, r.top + 1),
					BPoint(r.right, r.bottom),
					kB_GrayTable[pushed ? 6 : 10]);
			
			inView->EndLineArray();
		}
	}

	if (!inView->IsPrinting())
	{
		inView->SetHighColor(high);
		inView->SetLowColor(low);
	}
} // FillRect3D

StPenState::StPenState(BView *view, rgb_color low, rgb_color high, drawing_mode mode)
	: fView(view)
{
	fView->PushState();
	fView->SetHighColor(high);
	fView->SetLowColor(low);
	fView->SetDrawingMode(mode);
	fView->ConstrainClippingRegion(NULL);
} // StPenState::StPenState

StPenState::StPenState(BView *view)
	: fView(view)
{
	fView->PushState();
	fView->ConstrainClippingRegion(NULL);
} // StPenState::StPenState

StPenState::~StPenState()
{
	fView->ConstrainClippingRegion(NULL);
	fView->PopState();
} // StPenState::~StPenState
