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
	Clipboard.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   MYCLIPBOARD_H
#include "MyClipboard.h"
#endif

#ifndef   TEXT2CELLS_H
#include "Text2Cells.h"
#endif


#include <Autolock.h>
#include <DataIO.h>
#include <Clipboard.h>
#include <View.h>
#include <Picture.h>
#include <ScrollView.h>


CClipboard *gClipboard;

class CClipboardView : public BView {
public:
	CClipboardView(BRect frame);

	void SetPicture(BPicture *inPic, BRect inRect);
	virtual void Draw(BRect updateRect);

	virtual void TargetedByScrollView(BScrollView *scroller);
	virtual void FrameResized(float w, float h);

	BPicture *fPic;
	BRect fRect;
	BScrollView *fScroller;
};


CClipboard::CClipboard()
	: BWindow(BRect(200.0, 400.0, 400.0, 500.0),
		"Clipboard", B_DOCUMENT_WINDOW, 0)
{
	fCopy = NULL;
	fPic = NULL;
	
	BRect r(Bounds());
	r.left--; r.top--;
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT;
	
	fView = new CClipboardView(r);

	AddChild(new BScrollView("scroller", fView, B_FOLLOW_ALL_SIDES,
		0, true, true, B_NO_BORDER));
} /* CClipboard::CClipboard */

CClipboard::~CClipboard()
{
	if (fCopy) fCopy->Release();
	
	if (fPic)
		delete fPic;
} /* CClipboard::~CClipboard */

bool CClipboard::QuitRequested()
{
	if (!IsHidden()) Hide();
	return CCellWindow::WindowCount() == 0;
} // CClipboard::QuitRequested
	
void CClipboard::PutCells(CContainer *inCopy, range *inRange,
	BPicture *inPic, BRect inRect, const char *inSender)
{
	try
	{
		be_clipboard->Lock();
		
		if (fCopy) fCopy->Release();
		
		if (fPic)
			delete fPic;
		
		fCopy = inCopy;
		fPic = inPic;
		fRange = *inRange;
	
		be_clipboard->Clear();
		
		be_clipboard->Data()->AddPointer("container", fCopy);
//		be_clipboard->Data()->AddData("picture", 'PICT', fPic->Data(), fPic->DataSize());
//		be_clipboard->Data()->AddData("rect of pict", 'RECT', &inRect, sizeof(BRect));
//		fView->SetPicture(fPic, inRect);
		
		BMallocIO stream;
		CTextConverter conv(stream, inCopy);
		conv.ConvertToText(NULL);
		
		be_clipboard->Data()->AddData("text/plain", B_MIME_DATA,
			stream.Buffer(), stream.BufferLength());
	
		be_clipboard->Commit();
		be_clipboard->Unlock();
	}
	catch(...)
	{
		be_clipboard->Unlock();
		throw;
	}
} /* CClipboard::PutCells */

bool CClipboard::GetCells(CContainer **outCopy, range *outRange)
{
	bool result = false;
	
	try
	{
		be_clipboard->Lock();

		if (be_clipboard->Data()->FindPointer("container", (void **)outCopy) != B_NO_ERROR)
			*outCopy = NULL;
			
		if (*outCopy && *outCopy == fCopy)
		{
			*outRange = fRange;
			result = true;
		}
		else
		{
			if (fCopy) fCopy->Release();
			fCopy = NULL;
			
			if (fPic)
				delete fPic;
			fPic = NULL;
			fView->SetPicture(NULL, BRect());
			
			const void *p;
			long l;
			if (be_clipboard->Data()->FindData("text/plain", B_MIME_DATA, &p, &l) != B_NO_ERROR)
				result = false;
	
			BMemoryIO stream(p, l);
			
			*outCopy = new CContainer(NULL);
			CTextConverter conv(stream, *outCopy);
			conv.ConvertFromText(fRange);
			
			*outRange = fRange;
			result = true;
		}
		be_clipboard->Unlock();
	}
	catch(...)
	{
		be_clipboard->Unlock();
		throw;
	}
	
	return result;
} /* CClipboard::GetCells */

bool CClipboard::ContainsData()
{
	bool result = false;
	
	if (be_clipboard->Lock())
	{
		long l;
		const void *p;
		
		if (be_clipboard->Data()->FindPointer("container", (void**)&p) == B_NO_ERROR)
			result = true;
		else
		{
			if (fCopy) fCopy->Release();
			fCopy = NULL;
				
			if (fPic)
				delete fPic;
			fPic = NULL;
	//		fView->SetPicture(NULL, BRect());
					
			if (be_clipboard->Data()->FindData("text/plain", B_MIME_DATA, &p, &l) == B_NO_ERROR)
				result = true;
		}
		
		be_clipboard->Unlock();
	}
	return result;
} /* CClipboard::ContainsData */

void CClipboard::PutGraphic(BPicture *pict, BRect r)
{
	if (fPic) delete fPic;
	fPic = pict;
	
	if (fCopy) fCopy->Release();
	fCopy = NULL;
	
	fView->SetPicture(pict, r);
	
	be_clipboard->Lock();
	be_clipboard->Clear();
//	be_clipboard->Data()->AddData("picture", 'PICT', pict->Data(), pict->DataSize());
//	be_clipboard->Data()->AddData("rect of pict", 'RECT', &r, sizeof(BRect));
	be_clipboard->Commit();
	be_clipboard->Unlock();
} /* CClipboard::PutGrapic */

CClipboardView::CClipboardView(BRect frame)
	: BView(frame, "view", 	B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS)
{
	fPic = NULL;
}

void CClipboardView::SetPicture(BPicture *inPic, BRect inRect)
{
	BAutolock lock(Window());

	if ((fPic = inPic) != NULL)
	{
		fRect = inRect;
		
		FrameResized(Bounds().Width(), Bounds().Height());
	}
	else
	{
		fScroller->ScrollBar(B_VERTICAL)->SetRange(0, 0);
		fScroller->ScrollBar(B_HORIZONTAL)->SetRange(0, 0);
	}
	Invalidate();
}

void CClipboardView::Draw(BRect updateRect)
{
	FillRect(Bounds(), B_SOLID_LOW);
	if (fPic)
		DrawPicture(fPic, BPoint(5, 5));
}

void CClipboardView::TargetedByScrollView(BScrollView *scroller)
{
	fScroller = scroller;
}

void CClipboardView::FrameResized(float w, float h)
{
	if (fPic != NULL)
	{
		float vh, vv;
		
		vv = static_cast<float>(std::max(static_cast<int>(fRect.Height() - h + B_H_SCROLL_BAR_HEIGHT), 0));
		vh = static_cast<float>(std::max(static_cast<int>(fRect.Width() - w + B_V_SCROLL_BAR_WIDTH), 0));
		
		fScroller->ScrollBar(B_VERTICAL)->SetRange(0, vv);
		fScroller->ScrollBar(B_HORIZONTAL)->SetRange(0, vh);
	}
	else
	{
		fScroller->ScrollBar(B_VERTICAL)->SetRange(0, 0);
		fScroller->ScrollBar(B_HORIZONTAL)->SetRange(0, 0);
	}
}
