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
	MyTextControl.c

	Copyright 1997, Hekkelman Programmatuur

	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   SUM_IT_H
#include "Sum-It.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   PASTEFUNCTIONDIALOG_H
#include "PasteFunctionDialog.h"
#endif


#include <ctype.h>
#include <support/Debug.h>

#include <MenuItem.h>
#include <PopUpMenu.h>
#include <Clipboard.h>

CTextControl::CTextControl(BRect inFrame)
	: BTextView(inFrame, "editbox",
		BRect(0, 0, inFrame.Width(), inFrame.Height()),
		0, B_WILL_DRAW | B_NAVIGABLE)
{
	fCellEntered = false;
	fIsNewFormula = false;
} /* CTextControl::CTextControl */

void CTextControl::MakeFocus(bool isFocus)
{
	BTextView::MakeFocus(isFocus);

	fIsNewFormula = (TextLength() == 0);

	if (isFocus)
	{
		CCellView *cv = dynamic_cast<CCellView *>(Window()->FindView("cells"));
		ASSERT(cv);
		if (!cv->IsEntering())
		{
			cv->SetEntering(isFocus);
			SelectAll();
		}
		fCellEntered = false;
	}
} /* CTextControl::MakeFocus */

void CTextControl::MessageReceived(BMessage *inMessage)
{
	switch (inMessage->what)
	{
		case msg_FunctionChosen:
		{
			MakeFocus();
			EnterString(inMessage->FindString("string"),
				inMessage->FindInt32("start"),
				inMessage->FindInt32("stop"));
			break;
		}

		default:
			BTextView::MessageReceived(inMessage);
			break;
	}
} /* CTextControl::MessageReceived */

void CTextControl::EnterString(const char *s, long selStart, long selLen)
{
	int32 sStart, sEnd;

	if (!fCellView->IsEntering())
		fCellView->SetEntering(true);

	GetSelection(&sStart, &sEnd);

	if (fCellEntered)
	{
		Select(fCellEnteredAt, sStart);
		sStart = fCellEnteredAt;
	}

	Delete();

	if (!selLen)
		selLen = strlen(s);

	sStart += selStart;
	sEnd = sStart + selLen;

	Insert(s);

	if (!selStart)
		sStart = sEnd;

	Select(sStart, sEnd);

	fCellEntered = false;
} /* CTextControl::EnterString */

void CTextControl::EnterRange(range inRange)
{
	char s[20];

	int32 sStart, sEnd;
	GetSelection(&sStart, &sEnd);

	if (!fCellEntered)
		fCellEnteredAt = sStart;

	inRange.GetName(s);
	EnterString(s);

	fCellEntered = true;
} /* CTextControl::EnterRange */

void CTextControl::MouseDown(BPoint where)
{
	fCellEntered = false;

	if (IsFocus())
	{
		if (IsOptionalClick(Window()->CurrentMessage()))
		{
			DoEntryContextMenu(where);
			return;
		}
	}

	BTextView::MouseDown(where);
} /* CTextControl::MouseDown */

void CTextControl::KeyDown(const char *bytes, int32 numOfBytes)
{
	fCellEntered = false;

//	uint32 modifiers;
//	Window()->CurrentMessage()->FindInt32("modifiers", &modifiers);
//
//	if (modifiers & B_CONTROL)
//	{
//		switch (*bytes)
//		{
//			case B_UP:
//
//				break;
//			default:
//				BTextView::KeyDown(bytes, numOfBytes);
//		}
//	}
//	else
		BTextView::KeyDown(bytes, numOfBytes);
} /* CTextControl::KeyDown */

C3DBoxje::C3DBoxje(BRect r)
	: BView(r, "boxje", B_FOLLOW_LEFT_RIGHT,
		B_WILL_DRAW)
{
	fRight = r.right;
} /* C3DBoxje::C3DBoxje */

void C3DBoxje::Draw(BRect updateRect)
{
	BView::Draw(updateRect);

	BRect b(Bounds()), r;

	b = r;
	r.right = fRight;

	BeginLineArray(4);
	AddLine(r.LeftBottom(), r.LeftTop(), kB_GrayTable[17]);
	AddLine(r.LeftTop(), r.RightTop(), kB_GrayTable[17]);
	AddLine(r.RightTop(), r.RightBottom(), kB_GrayTable[4]);
	AddLine(r.RightBottom(), r.LeftBottom(), kB_GrayTable[4]);
	EndLineArray();
} /* C3DBoxje::Draw */

void CTextControl::DoEntryContextMenu(BPoint where)
{
	BPopUpMenu popup("entrypopup", false);
	popup.SetFont(be_plain_font);
	popup.AddItem(new BMenuItem(GetMessage(msgCut), NULL));
	popup.AddItem(new BMenuItem(GetMessage(msgCopy), NULL));
	popup.AddItem(new BMenuItem(GetMessage(msgPaste), NULL));
	popup.AddItem(new BMenuItem(GetMessage(msgClear), NULL));
	popup.AddSeparatorItem();
	popup.AddItem(new BMenuItem(GetMessage(msgSelectAll), NULL));
	popup.AddSeparatorItem();
	popup.AddItem(new BMenuItem(GetMessage(msgInsertFunction), NULL));

	BMenuItem *item = popup.Go(ConvertToScreen(where));

	if (item)
	{
		switch (popup.IndexOf(item))
		{
			case 0:
				Cut(be_clipboard);
				break;
			case 1:
				Copy(be_clipboard);
				break;
			case 2:
				Paste(be_clipboard);
				break;
			case 3:
				Delete();
				break;
			case 5:
				SelectAll();
				break;
			case 7:
				CPasteFunctionDialog::Construct((CCellWindow *)Window());
				break;
		}
	}
} /* CTextControl::DoEntryContextMenu */

bool CTextControl::AcceptsDrop(const BMessage *inMessage)
{
	const char *p;
	long l;

	if (inMessage->FindData("text/plain", B_MIME_DATA, (const void **)&p, &l))
		return false;

	while (l--)
	{
		if (iscntrl(p[l])) return false;
		if (p[l] == B_RETURN || p[l] == B_TAB) return false;
	}

	return true;
} /* CTextControl::AcceptsDrop */

bool CTextControl::AcceptsPaste(BClipboard *clipboard)
{
	const char *p;
	long l;

	if (be_clipboard->Data()->FindData("text/plain", B_MIME_DATA, (const void **)&p, &l) != B_NO_ERROR)
		return false;

	while (l--)
	{
		if (iscntrl(p[l])) return false;
		if (p[l] == B_RETURN || p[l] == B_TAB) return false;
	}

	return true;
} /* CTextControl::AcceptsPaste */
