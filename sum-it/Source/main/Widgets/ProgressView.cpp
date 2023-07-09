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
	ProgressView.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   PROGRESSVIEW_H
#include "ProgressView.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#include <algorithm>

#include <Message.h>
#include <Window.h>

const double
	kUpdateDelay = 300000.0,
	kStartDelay = 500000.0;

const rgb_color
	kProgressBarColor[][3] =
	{
		{ { 0x32, 0x96, 0xFF, 0 },
		  { 0x84, 0xC0, 0xFF, 0 },
		  { 0x1E, 0x5A, 0x99, 0 } },
		
		{ { 0xFF, 0x96, 0x32, 0 },
		  { 0xFF, 0xC0, 0x84, 0 },
		  { 0x99, 0x5A, 0x1E, 0 } },
		
		{ { 0xFF, 0x32, 0x96, 0 },
		  { 0xFF, 0x84, 0xC0, 0 },
		  { 0x99, 0x1E, 0x5A, 0 } }
	};

CProgressView::CProgressView(CCellView* inCellView, BRect inFrame)
	: BView(inFrame, "selView", B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,
		B_WILL_DRAW)
{
	
	fCellView = inCellView;
	fVisible = false;
	fMax = 100;
	fCurrent = 50;
	fColor = 0;
} /* CProgressView::CProgressView */

void CProgressView::AttachedToWindow()
{
	SetViewColor(kB_GrayTable[6]);
	fCellView->AttachProgressView(this);
} /* CProgressView::AttachedToWindow */

void CProgressView::Draw(BRect updateRect)
{
	BRect frame(Bounds());
		
	SetHighColor(kB_GrayTable[14]);
	StrokeRect(Bounds());
	SetHighColor(kB_GrayTable[0]);
	
	frame.InsetBy(1.0, 1.0);
	FillRect3D(this, frame, FALSE, fWindowIsActive, NULL);

	if (fVisible)
		DrawBar();
} /* CProgressView::Draw */
	
void CProgressView::DrawBar()
{
	BRect frame(Bounds());
	
	frame.InsetBy(4.0, 4.0);
	StrokeRect(frame);	

	float right = frame.right;
	frame.right = frame.left + (frame.right - frame.left) * fCurrent / fMax;

	frame.InsetBy(1.0, 1.0);
	SetHighColor(kProgressBarColor[fColor][0]);
	FillRect(frame);
	float r2 = frame.right;
	frame.right = right - 1.0;

	frame.InsetBy(-2.0, -2.0);
	
	BeginLineArray(8);

	AddLine(BPoint(frame.left, frame.bottom - 1.0),
		frame.LeftTop(), kB_GrayTable[10]);
	AddLine(frame.LeftTop(), BPoint(frame.right - 1.0, frame.top),
		kB_GrayTable[10]);
	AddLine(BPoint(frame.left + 1.0, frame.bottom),
		frame.RightBottom(), kB_GrayTable[1]);
	AddLine(frame.RightBottom(), BPoint(frame.right, frame.top + 1.0),
		kB_GrayTable[1]);
	
	frame.InsetBy(2.0, 2.0);
	BRect r = frame;
	frame.right = r2;
	r.left = r2 + 1.0;

	AddLine(BPoint(frame.left, frame.bottom - 1.0),
		frame.LeftTop(), kProgressBarColor[fColor][1]);
	AddLine(frame.LeftTop(), BPoint(frame.right - 1.0, frame.top),
		kProgressBarColor[fColor][1]);

	AddLine(BPoint(frame.left + 1.0, frame.bottom),
		frame.RightBottom(), kProgressBarColor[fColor][2]);
	AddLine(frame.RightBottom(), BPoint(frame.right, frame.top + 1.0),
		kProgressBarColor[fColor][2]);

	EndLineArray();
	
	FillRect(r, B_SOLID_LOW);
	
	SetHighColor(0, 0, 0, 0);
} /* CProgressView::DrawBar */

void CProgressView::Start(int inMax, int inColor)
{
	fVisible = true;
	fMax = inMax;
	fColor = inColor;
	fCurrent = 0;
	DrawBar();
} /* CProgressView::Start */

void CProgressView::Step()
{
	fCurrent++;
	DrawBar();
} /* CProgressView::Step */

void CProgressView::Stop()
{
	fVisible = false;
	Invalidate(Bounds());
} /* CProgressView::Stop */

void CProgressView::WindowActivated(bool active)
{
	BView::WindowActivated(active);
	fWindowIsActive = active;
	Draw(Bounds());
} /* CProgressView::WindowActivated */

void CProgressView::MessageReceived(BMessage *inMessage)
{
	int32 l;
	switch (inMessage->what)
	{
		case msg_ProgressInfo:
			if (inMessage->FindInt32("start", &l) == B_NO_ERROR)
			{
				fVisible = true;
				inMessage->FindInt32("max", &fMax);
				inMessage->FindInt32("current", &fCurrent);
				inMessage->FindInt32("color", &fColor);
				DrawBar();
			}
			else if (inMessage->FindInt32("step", &l) == B_NO_ERROR)
			{
				inMessage->FindInt32("current", &fCurrent);
				DrawBar();
				Sync();
			}
			else if (inMessage->FindInt32("stop", &l) == B_NO_ERROR)
			{
				Stop();
			}
			break;
			
		default:
			BView::MessageReceived(inMessage);
			break;
	}
} /* CProgressView::MessageReceived */


//
//
//	StProgress
//

StProgress::StProgress(CCellView* inView, int inMax, bool inPost)
{
	fShowing = false;
	fPost = inPost;
	fProgress = inView->GetProgressView();
	fMax = inMax;
	fCurrent = 0;
	fNextAction = system_time() + kStartDelay;
	fColor = pColorBlue;
} /* StProgress::StProgress */

StProgress::StProgress(CCellView* inView, int inMax, int inColor, bool inPost)
{
	fShowing = false;
	fPost = inPost;
	fProgress = inView->GetProgressView();
	fMax = inMax;
	fCurrent = 0;
	fNextAction = system_time() + kStartDelay;
	fColor = inColor;
} /* StProgress::StProgress */

StProgress::~StProgress()
{
	if (fShowing)
		Stop();
} /* StProgress::~StProgress */

void StProgress::Start()
{
	if (fPost)
	{
		BMessage msg(msg_ProgressInfo);
		msg.AddInt32("start", 1);
		msg.AddInt32("max", fMax);
		msg.AddInt32("current", fCurrent);
		msg.AddInt32("color", fColor);
		fProgress->Window()->PostMessage(&msg, fProgress);
	}
	else
	{
		fProgress->fVisible = true;
		fProgress->fCurrent = fCurrent;
		fProgress->fMax = fMax;
		fProgress->fColor = fColor;
		fProgress->DrawBar();
	}
	
	fNextAction = system_time() + kUpdateDelay;
	fShowing = true;
} /* StProgress::Start */

void StProgress::Stop()
{
	if (fPost)
	{
		BMessage msg(msg_ProgressInfo);
		msg.AddInt32("stop", 1);
		fProgress->Window()->PostMessage(&msg, fProgress);
	}
	else
	{
		fProgress->fVisible = false;
		fProgress->Invalidate(fProgress->Bounds());
	}
	fShowing = false;
} /* StProgress::Stop */

void StProgress::DoStep()
{
	if (fPost)
	{
		BMessage msg(msg_ProgressInfo);
		msg.AddInt32("step", 1);
		msg.AddInt32("current", fCurrent);
		fProgress->Window()->PostMessage(&msg, fProgress);
	}
	else
	{
		fProgress->fCurrent = fCurrent;
		fProgress->DrawBar();
	}

	fNextAction = system_time() + kUpdateDelay;
} /* StProgress::DoStep */

void StProgress::NewMax(int inMax)
{
	fMax = fProgress->fMax = inMax;
	fCurrent = fProgress->fCurrent = std::min( static_cast<int32>(inMax), fCurrent);
	if (fProgress->fVisible)
		fProgress->DrawBar();
} /* StProgress::NewMax */
