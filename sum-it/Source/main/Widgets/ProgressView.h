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
	ProgressView.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 ***/

#ifndef PROGRESSVIEW_H
#define PROGRESSVIEW_H

#include <Debug.h>

#include <View.h>

class CCellView;
class CProgressView;

enum {
	pColorBlue,
	pColorRed,
	pColorYellow
};

class StProgress {
public:
	StProgress(CCellView* inView, int inMax, bool post);
	StProgress(CCellView* inView, int inMax, int inColor, bool post);
	~StProgress();
	
	inline void Step();
	inline void Step(int newValue);
	void NewMax(int inMax);

private:

	void Start();
	void Stop();
	void DoStep();

	double fNextAction;
	bool fShowing;
	bool fPost;
	int32 fColor;
	int32 fCurrent, fMax;
	CProgressView* fProgress;
};

class CProgressView : public BView {
	friend class StProgress;
public:
	CProgressView(CCellView* cellView, BRect frame);

	void Start(int inMax, int inColor = pColorBlue);
	void Step();
	void Stop();

protected:

	virtual void MessageReceived(BMessage *inMessage);

	virtual void AttachedToWindow();
	virtual void Draw(BRect updateRect);
	virtual void WindowActivated(bool active);
	void DrawBar();

private:
	CCellView* fCellView;
	bool fVisible;
	bool fWindowIsActive;
	int32 fMax, fCurrent;
	int32 fColor;
};

inline void StProgress::Step()
{
	double now = system_time();

	fCurrent++;
	ASSERT(fCurrent <= fMax);

	if (now > fNextAction)
	{
		if (!fShowing)
			Start();
		else
			DoStep();
	}
} /* StProgress::Step */

inline void StProgress::Step(int newValue)
{
	double now = system_time();

	fCurrent = newValue;
	ASSERT(fCurrent >= 0);
	ASSERT(fCurrent <= fMax);

	if (now > fNextAction)
	{
		if (!fShowing)
			Start();
		else
			DoStep();
	}
} /* StProgress::Step */
#endif
