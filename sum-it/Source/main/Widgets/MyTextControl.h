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
	MyTextControl.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef MYTEXTCONTROL_H
#define MYTEXTCONTROL_H

#ifndef   RANGE_H
#include "Range.h"
#endif

#include <TextView.h>

class CCellView;

class CTextControl : public BTextView {
public:
	CTextControl(BRect inFrame);
	
	virtual void MakeFocus(bool focused = true);

	void EnterString(const char *inString, long inStart = 0,
		long inLen = 0);
	void EnterRange(range inRange);
	void ResetCellEntered();
	
	void SetCellView(CCellView *inView)
		{ fCellView = inView; };

	bool IsNewFormula() { return fIsNewFormula; };

	virtual bool AcceptsDrop(const BMessage *inMessage);
	virtual bool AcceptsPaste(BClipboard *clipboard);

private:
	virtual void MouseDown(BPoint where);
	virtual void KeyDown(const char *bytes, int32 numOfBytes);
	virtual void MessageReceived(BMessage *inMessage);

	void DoEntryContextMenu(BPoint where);
	
	CCellView *fCellView;
	long fCellEnteredAt, fLastPosition;
	long fSelStart, fSelEnd;
	bool fCellEntered;
	bool fIsNewFormula;
};

class C3DBoxje : public BView {
public:
	C3DBoxje(BRect r);
	
	virtual void Draw(BRect updateRect);
private:
	float fRight;
};

#endif
