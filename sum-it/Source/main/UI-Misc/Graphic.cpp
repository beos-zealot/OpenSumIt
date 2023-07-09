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
	Graphic
	
	Copyright 1997, Hekkelman Programmatuur
	
*/

#ifndef   CELL_H
#include "Cell.h"
#endif

#ifndef   RANGE_H
#include "Range.h"
#endif

#ifndef   GRAPHIC_H
#include "Graphic.h"
#endif

#ifndef   GRAPHPLUGIN_H
#include "GraphPlugIn.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   GRAPHICCOMMANDS_H
#include "GraphicCommands.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif


#include <algorithm>

#include <Beep.h>
#include <Picture.h>
#include <Autolock.h>

CGraphic::CGraphic(cell anchor, BRect frame, const char *name)
	: BHandler(name)
{
	fNext = NULL;
	fPrevious = NULL;
	fParent = NULL;
	fAnchor = anchor;
	fWidth = frame.Width();
	fHeight = frame.Height();
	fOffset.x = frame.left;
	fOffset.y = frame.top;
	fHasFocus = false;
	fMakingCopy = false;
} /* CGraphic::CGraphic */

CGraphic::~CGraphic()
{
} /* CGraphic::~CGraphic */
	
BRect CGraphic::Bounds()
{
	BRect f(Frame());
	f.InsetBy(5, 5);
	return f;
} /* CGraphic::Bounds */

BRect CGraphic::Frame()
{
	BRect r(0, 0, fWidth, fHeight);
	
	if (fParent && !fMakingCopy)
	{
		BPoint p;
		fParent->GetCellTopLeft(fAnchor, p);

		r.OffsetTo(p.x + fOffset.x, p.y + fOffset.y);
	}

	return r;
} /* CGraphic::Frame */

bool CGraphic::IsVisible()
{
	return true;	
} /*  CGraphic::IsVisible */

void CGraphic::Draw(BRect updateRect)
{
	BRect r(Bounds());
	BPoint p = r.LeftTop();
	
	p.x += 2;
	p.y += 20;
	
	fParent->SetFont(be_plain_font);
	fParent->DrawString("Hello world!", p);
} /* CGraphic::Draw */

void CGraphic::MouseDown(BPoint where)
{
//	beep();
} /* CGraphic::MouseDown */

void CGraphic::MakeFocus(bool active)
{
	fHasFocus = active;
	
	if (fHasFocus)
		DrawBorders();
	else
	{
		BRegion rgn;
		rgn.Include(Frame());
		BRect r = Bounds();
		r.InsetBy(1, 1);
		rgn.Exclude(r);

		for (int i = 0; i < rgn.CountRects(); i++)
			fParent->Invalidate(rgn.RectAt(i));
		
		fParent->Window()->UpdateIfNeeded();
	}
} /* CGraphic::MakeFocus */

bool CGraphic::HasFocus()
{
	return fHasFocus;
} /* CGraphic::HasFocus */

CGraphic* CGraphic::Contains(BPoint where)
{
	CGraphic *g = this;
	
	while (g)
	{
		BRect r(g->Frame());
		
		if (r.Contains(where))
			return g;

		g = g->fNext;
	}
	
	return NULL;
} /* CGraphic::Contains */

bool CGraphic::HandleMouseDown(BPoint where)
{
	bool result = false;
	CGraphic *g = Contains(where);

	if (g)
	{
		BRect r(g->Frame()), b;
		b = r;
		b.InsetBy(5, 5);
		
		if (!g->HasFocus())
		{
			if (fParent->IsEntering())
				beep();
			else
			{
				fParent->MoveToFront(g);
				g->MakeFocus(true);
				fParent->ClearAnts();
			}
		}
		else if (where.x >= r.right - 5 && where.y >= r.bottom - 5)
			g->TrackResize(where);
		else if (!b.Contains(where))
			g->TrackDrag(where);
		else
			g->MouseDown(where);

		result = true;
	}	

	return result;
} /* CGraphic::HandleMouseDown */

void CGraphic::HandleUpdate(BRect updateRect)
{
	if (!fParent) return;
	CGraphic *g = this;
	BRect cb(fParent->CellBounds());
	cb.left += 1;
	cb.top += 1;

	StPenState save(fParent);
	
	while (g->fNext)
		g = g->fNext;
	
	while (g)
	{
		BRect r(g->Frame());
	
		if (updateRect.Intersects(r))
		{
			BRegion clip;
			clip.Include(r & cb);
			g->ClipAbove(clip);
			
			// MV
//			fParent->ConstrainClippingRegion(&clip);
			
			fParent->SetDrawingMode(B_OP_COPY);
			fParent->SetLowColor(kWhite);
			g->DrawBorders();
			g->Draw(updateRect & r);
		}
		
		g = g->fPrevious;
	}
} /* CGraphic::HandleUpdate */

void CGraphic::CreateClip(BRegion& clip)
{
	CGraphic *g = this;
	
	while (g)
	{
		BRect r(g->Frame());
		
		if (clip.Intersects(r))
			clip.Exclude(r);
		
		g = g->fNext;
	}
} /* CGraphic::CreateClip */

void CGraphic::ClipAbove(BRegion& rgn)
{
	CGraphic *a = fPrevious;
	
	while (a)
	{
		rgn.Exclude(a->Frame());
		a = a->fPrevious;
	}
} /* CGraphic::ClipAbove */

void CGraphic::AttachedToCellView(CCellView *parent)
{
//	ASSERT(fParent == NULL);
	fParent = parent;
	BAutolock lock(fParent->Window());
	fParent->Window()->AddHandler(this);
} /* CGraphic::AttachedToCellView */

void CGraphic::RemovedFromCellView()
{
	fParent->Window()->RemoveHandler(this);
} /* CGraphic::RemovedFromCellView */

CGraphic* CGraphic::Next()
{
	return fNext;
} /* CGraphic::Next */

void CGraphic::SetNext(CGraphic* next)
{
	fPrevious = next ? next->fPrevious : NULL;
	fNext = next;
	if (fNext) fNext->fPrevious = this;
	if (fPrevious) fPrevious->fNext = this;
} /* CGraphic::SetNext */

void CGraphic::Remove(CGraphic *g)
{
	CGraphic *n = this;

	while (n && n != g)
		n = n->Next();

	if (n && n == g)
	{
		if (n->fNext)		n->fNext->fPrevious = n->fPrevious;
		if (n->fPrevious)	n->fPrevious->fNext = n->fNext;
		n->fNext = NULL;
		n->fPrevious = NULL;
	}
} /* CGraphic::Remove */

void CGraphic::DrawBorders()
{
	BRect r = Frame();
	
	r.InsetBy(2, 2);
	fParent->SetPenSize(5);
	fParent->SetHighColor(kWhite);
	fParent->StrokeRect(r);
	fParent->SetPenSize(1);
	r.InsetBy(-2, -2);
	
	if (fHasFocus)
	{
		fParent->SetHighColor(keyboard_navigation_color());
		fParent->StrokeRect(r);
		
		BRect c(r);
		c.left = r.right - 4;
		c.top = c.bottom - 4;
		fParent->FillRect(c);
	}
	else
	{
		fParent->SetHighColor(kWhite);
		BRect c(r);
		c.left = r.right - 4;
		c.top = c.bottom - 4;
		fParent->FillRect(c);

		fParent->SetHighColor(kB_GrayTable[6]);
		fParent->StrokeRect(r);
	}

	fParent->SetHighColor(kBlack);
} /* CGraphic::DrawBorders */

void CGraphic::TrackResize(BPoint where)
{
	BPoint loc = where, oldSize, newSize;
	uint32 btns;
	float w, h;
	
	oldSize.x = w = fWidth;
	oldSize.y = h = fHeight;
	
	do
	{
		if (loc != where)
		{
			float dx, dy;
			
			dx = loc.x - where.x;
			dy = loc.y - where.y;

			w += dx;
			h += dy;
			
			ResizeTo( std::max(w, static_cast<float>(100)) , std::max(h, static_cast<float>(100)) ) ;
			
			where = loc;
		}
		fParent->GetMouse(&loc, &btns);
	}
	while (btns);

	newSize.x = fWidth;
	newSize.y = fHeight;

	CCellWindow *wind = (CCellWindow *)fParent->Window();
	wind->RegisterCommand(new CResizeGraphicCommand(fParent, oldSize, newSize));
} /* CGraphic::TrackResize */

void CGraphic::ResizeTo(float w, float h)
{
	BRect r(Frame());

	fWidth = w;
	fHeight = h;

	if (fParent)
	{
		fParent->Invalidate(r);
		fParent->Invalidate(Frame());
		FrameResized(w, h);
		fParent->Window()->UpdateIfNeeded();
		((CCellWindow *)fParent->Window())->SetDirty(true);
	}
} /* CGraphic::ResizeTo */

void CGraphic::MoveBy(float dx, float dy)
{
	cell c;
	BRect f(Frame()), r, upd;
	BPoint p, k;
	p.x = std::max(f.left + dx, fParent->CellBounds().left);
	p.y = std::max(f.top + dy, fParent->CellBounds().top);
	
	dx = p.x - f.left;
	dy = p.y - f.top;
	
	if (!fParent->GetCellHitBy(p, c))
	{
		c = cell(1, 1);
	}
	
	fAnchor = c;

	FrameMoved(p.x, p.y);
	
	upd = f & fParent->CellBounds();
	
	fParent->GetCellTopLeft(c, k);
	fOffset.x = p.x - k.x;
	fOffset.y = p.y - k.y;
	
	r = upd;
	r.OffsetBy(dx, dy);
	fParent->Sync();
	fParent->CopyBits(upd, r);
	
	BRegion updR;
	updR.Include(f);
	f.OffsetBy(dx, dy);
	updR.Include(f);
	updR.Exclude(r);

	for (int i = 0; i < updR.CountRects(); i++)
		fParent->Invalidate(updR.RectAt(i));

	fParent->Window()->UpdateIfNeeded();
	((CCellWindow *)fParent->Window())->SetDirty(true);
} /* CGraphic::MoveBy */

void CGraphic::MoveTo(float x, float y)
{
	BRect f(Frame());
	MoveBy(x - f.left, y - f.top);
} /* CGraphic::MoveTo */

void CGraphic::TrackDrag(BPoint where)
{
	BPoint loc = where, ftl, d;
	uint32 btns;
	cell savedAnchor;
	BRect savedSize;

	ftl = Frame().LeftTop();
	d.x = where.x - ftl.x;
	d.y = where.y - ftl.y;
	
	savedAnchor = fAnchor;
	savedSize.Set(fOffset.x, fOffset.y, fOffset.x + fWidth, fOffset.y + fHeight);
	
	do
	{
		if (loc != where)
		{
			float dx, dy;
			
			dx = loc.x - where.x;
			dy = loc.y - where.y;
			
			MoveBy(dx, dy);
			
			ftl = Frame().LeftTop();
			where.x = ftl.x + d.x;
			where.y = ftl.y + d.y;
		}
		fParent->GetMouse(&loc, &btns);
	}
	while (btns);

	CCellWindow *w = (CCellWindow *)fParent->Window();
	w->RegisterCommand(new CMoveGraphicCommand(fParent, savedAnchor, savedSize,
		fAnchor, BRect(fOffset.x, fOffset.y, fOffset.x + fWidth, fOffset.y + fHeight)));
} /* CGraphic::TrackDrag */

void CGraphic::FrameResized(float w, float h)
{
} /* CGraphic::FrameResized */

void CGraphic::FrameMoved(float x, float y)
{
} /* CGraphic::FrameMoved */

void CGraphic::MessageReceived(BMessage *msg)
{
	BHandler::MessageReceived(msg);
} /* CGraphic::MessageReceived */

void CGraphic::GetPosition(cell& anchor, BRect& size)
{
	anchor = fAnchor;
	size.left = fOffset.x;
	size.top = fOffset.y;
	size.right = size.left + fWidth;
	size.bottom = size.top + fHeight;
} /* CGraphic::GetPosition */

void CGraphic::SetPosition(cell anchor, BRect size)
{
	fParent->Invalidate(Frame());
	fAnchor = anchor;
	fOffset.x = size.left;
	fOffset.y = size.top;
	fWidth = size.Width();
	fHeight = size.Height();
	fParent->Invalidate(Frame());
	fParent->Window()->UpdateIfNeeded();
} /* CGraphic::SetPosition */

void CGraphic::Copy(BPicture*& pict, BRect& r)
{
	fMakingCopy = true;
	try
	{
		r = Bounds();
		r.OffsetTo(0, 0);
		
		Parent()->BeginPicture(new BPicture());
		Draw(Bounds());
		pict = Parent()->EndPicture();
	}
	catch (...)
	{
		fMakingCopy = false;
		throw;
	}
	fMakingCopy = false;
} /* CGraphic::Copy */
