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
	Dialog.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 ***/


#ifndef DIALOG_H
#define DIALOG_H

#include <SupportDefs.h>
#include <Window.h>
#include <Box.h>
#include <CheckBox.h>
#include <RadioButton.h>
#include <TextControl.h>
#include <StringView.h>
#include <Menu.h>
#include <MenuField.h>
#include <ListView.h>

const uint32
	msg_OK				= 'OK  ',
	msg_Cancel			= 'Cncl',
	msg_Changed			= 'Chng';

class CCellWindow;

class CDialog : public BWindow {
public:
	CDialog(BRect inFrame, const char *inName, CCellWindow *inOwner,
		BRect okR, BRect cancelR, bool modal = false);
	CDialog(BRect inFrame, const char *inName, CCellWindow *inOwner,
		window_type type, int flags);
	~CDialog();
	
	void SetOwner(CCellWindow *inWindow);
	
//protected:

	virtual bool OKClicked();
	virtual bool CancelClicked();
	virtual void UpdateFields();
	
	virtual void WindowActivated(bool active);
	
	BView* AddGrayView(BRect inFrame, const char *inName);
	BBox* AddBox(BRect inFrame, const char *inName, BView *inView = NULL);
	BCheckBox* AddCheckBox(BRect inFrame, const char *inName,
		const char *inLabel, int inValue, BView *inView = NULL);
	BRadioButton* AddRadioButton(BRect inFrame, const char *inName,
		const char *inLabel, int inValue, BView *inView = NULL);
	BTextControl* AddTextControl(BRect inFrame, const char *inName,
		const char *inLabel, const char *inText, const char *inAllowed = NULL,
		int maxLen = 0, BView *inView = NULL);
	BStringView* AddStringView(BRect inFrame, const char *inName,
		const char *inText, BView *inView = NULL);
	BTextView* AddTextView(BRect inFrame, const char *inName,
		BView *inView = NULL);
	BMenuField* AddMenuField(BRect inFrame, const char *inName,
		const char *inLabel, BMenu *inMenu, BView *inView = NULL);
	BMenu* AddPopUpMenu(BRect inFrame, const char *inName,
		const char *inLabel = NULL, BView *inView = NULL);
	BListView* AddListView(BRect inFrame, const char *inName,
		BView *inView = NULL);

	bool IsOn(const char *name);
	void SetOn(const char *name, bool on);

	void SetEnabled(const char *name, bool enabled);
	
	void SetText(const char *name, const char *text);
	const char* GetText(const char *name);
	bool GetDouble(const char *name, double *d);
	
	virtual void MessageReceived(BMessage *inMessage);
	
	void MakeModal();
	
	BView *fMainView;
	CCellWindow *fOwner;
};

#endif
