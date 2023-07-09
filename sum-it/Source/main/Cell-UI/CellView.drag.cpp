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
	CellView.drag.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   DRAGCOMMAND_H
#include "DragCommand.h"
#endif

#ifndef   TEXT_CELLS_H
#include "Text2Cells.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#ifndef   GRAPHIC_H
#include "Graphic.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   RESOURCEMANAGER_H
#include "ResourceManager.h"
#endif

#include <AppDefs.h>
#include <Application.h>
#include <Message.h>
#include <Beep.h>
#include <DataIO.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <Picture.h>


void CCellView::StartDrag(BPoint where, bool copy)
{
	ClearAnts();
	BMessage msg(B_SIMPLE_DATA);
	msg.AddPointer("container", fContainer);
	msg.AddPointer("cellview", this);
	msg.AddData("range", 'rang', &fSelection, sizeof(range));
	msg.AddBool("dragacopy", copy);
	
	BRegion rgn;
	SelectionToRegion(rgn);
	cell c;
	GetCellHitBy(where, c);
	BRect dragRect = rgn.Frame();
	dragRect.OffsetBy(fCellWidths[c.h] - fCellWidths[fSelection.left],
		fCellHeights[c.v] - fCellHeights[fSelection.top]);
	
	BMallocIO buf;
	CTextConverter conv(buf, fContainer);
	conv.ConvertToText(&fSelection);
	msg.AddData("text/plain", B_MIME_DATA,
		buf.Buffer(), buf.BufferLength());

// and add a nice picture
	BPicture pic;
	BRect r;
	GetPictureOfSelection(&pic, r);
	pic.Archive(&msg);
//	msg.AddData("picture", 'PICT', pic.Data(), pic.DataSize());
	msg.AddData("rect of pict", 'RECT', &r, sizeof(BRect));
	
	fDragIsAcceptable = true;
	DragMessage(&msg, dragRect, this);
} /* CCellView::StartDrag */

void CCellView::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	if (message &&
		(message->what == B_SIMPLE_DATA || message->what == 'MIME'))
	{
		ssize_t l;
		const void *p;

		switch (transit)
		{
			case B_ENTERED_VIEW:
				fDragIsAcceptable =
					!fEntering && (
					message->FindData("text/plain", B_MIME_DATA, &p, &l) == B_NO_ERROR ||
					message->FindPointer("container", (void **)&p ) == B_NO_ERROR);
			case B_INSIDE_VIEW:
				if (fDragIsAcceptable)
				{
					ClearAnts();
					GetCellHitBy(point, fCurCell);
					MarchAnts();
				}
				break;

			case B_EXITED_VIEW:
				ClearAnts();
				fDragIsAcceptable = false;
				fCurCell = fSelection.TopLeft();
				break;
		}
	}
	else
	{
		if (!fWindowActive)
			return;
		
		if (fFirstGraphic && fFirstGraphic->Contains(point))
		{
			be_app->SetCursor(B_HAND_CURSOR);
			fCurrentCursor = curHand;
			return;
		}
		
		if (PointIsInSelection(point) && !fEntering)
		{
			if (fCurrentCursor != curDrag)
			{
				be_app->SetCursor(gResourceManager.LoadCursor(1));
				fCurrentCursor = curDrag;
			}
			return;
		}
		
		if (fCellBounds.Contains(point))
		{
			if (fCurrentCursor != curPlus)
			{
				be_app->SetCursor(gResourceManager.LoadCursor(0));
				fCurrentCursor = curPlus;
			}
		}
		else if (fCurrentCursor != curHand)
		{
			be_app->SetCursor(B_HAND_CURSOR);
			fCurrentCursor = curHand;
		}
	}
} /* CCellView::MouseMoved */

void CCellView::HandleDrop(BMessage *inMessage)
{
	ssize_t l;
	void * p;
	BPoint dp = inMessage->DropPoint();
	
	fDragIsAcceptable = false;
	
	if (inMessage->FindPointer("container", &p) == B_NO_ERROR)
	{
		key_info ki;
		get_key_info(&ki);
		
		CContainer *srcContainer;
		range srcRange, dstRange;
		CCellView *srcView;
		int action;
		
		srcContainer = (CContainer *)p;
		FailOSErr(inMessage->FindData("range", 'rang', (const void**)&p, &l), errMessageMissing);
		srcRange = *(range *)p;
		FailOSErr(inMessage->FindPointer("cellview", &p), errMessageMissing);
		srcView = (CCellView *)p;
		
		if (srcView != this)
			action = dragCopy;
		else
			action = dragMove;
		
		inMessage->FindBool("dragacopy", &fDragACopy);
		
		if (srcView == this &&
		    (ki.modifiers & B_CONTROL_KEY ||
			fDragACopy))
		{
			BPopUpMenu popup("dragpopup", false);
			popup.SetFont(be_plain_font);
			popup.AddItem(new BMenuItem(GetMessage(msgLinkHere), NULL));
			popup.AddItem(new BMenuItem(GetMessage(msgMoveHere), NULL));
			popup.AddItem(new BMenuItem(GetMessage(msgCopyHere), NULL));
			popup.AddSeparatorItem();
			popup.AddItem(new BMenuItem(GetMessage(msgCancel), NULL));
			
			BMenuItem *item = popup.Go(dp, false, true);
			int result = item ? popup.IndexOf(item) : -1;
			switch (result)
			{
				case 0: action = dragLink; break;
				case 1: action = dragMove; break;
				case 2: action = dragCopy; break;
				default:
					ClearAnts();
					fCurCell = fSelection.TopLeft();
					return;
			}
		}
	
		dstRange = srcRange;
		dstRange.OffsetBy(fCurCell.h - dstRange.left,
			fCurCell.v - dstRange.top);
		
		((CCellWindow *)Window())->
			RegisterCommand(new CDragCommand(this, srcContainer,
				fContainer, &srcRange, &dstRange, action));
	}
	else if (inMessage->FindData("text/plain", B_MIME_DATA, (const void **)&p, &l) == B_NO_ERROR)
	{
		cell dc;
		
		ConvertFromScreen(&dp);
		GetCellHitBy(dp, dc);
		
		BMemoryIO buf(p, l);
		CContainer *srcContainer = new CContainer;
		range srcRange, dstRange;
		
		CTextConverter conv(buf, srcContainer);
		conv.ConvertFromText(srcRange);
		
		dstRange = srcRange;
		dstRange.OffsetBy(dc.h - srcRange.left, dc.v - srcRange.top);
		
		((CCellWindow *)Window())->
			RegisterCommand(new CDragCommand(this, srcContainer,
				fContainer, &srcRange, &dstRange, dragMove));

		srcContainer->Release();
	}
	else
	{
		beep();
		ClearAnts();
		fCurCell = fSelection.TopLeft();
	}
} /* CCellView::HandleDrop */

