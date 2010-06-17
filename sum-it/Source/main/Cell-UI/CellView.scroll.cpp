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
	CellView.scroll.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLSCROLLBAR_H
#include "CellScrollBar.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#include <Autolock.h>
#include <Window.h>

void CCellView::ScrollToPosition(long x, long y)
{
	int dv, dh;
	float dx, dy;
	BRect r, bounds(Bounds());
	
	StPenState save(this);

	dh = (x ? x - fPosition.h : 0);
	dv = (y ? y - fPosition.v : 0);

	if (dv + fPosition.v > kRowCount)
		dv = kRowCount - fPosition.v;
	else if (dv + fPosition.v < fFrozen.v + 1)
		dv = 1 - fPosition.v;

	if (dv)
	{
		dy = fCellHeights[fPosition.v - 1] - fCellHeights[fPosition.v + dv - 1];
		fPosition.v += dv;
	}
	else
		dy = 0.0;

	if (dh + fPosition.h > kColCount)
		dh = kColCount - fPosition.h;
	else if (dh + fPosition.h < fFrozen.h + 1)
		dh = 1 - fPosition.h;

	if (dh)
	{
		dx = fCellWidths[fPosition.h - 1] - fCellWidths[fPosition.h + dh - 1];
		fPosition.h += dh;
	}
	else
		dx = 0.0;

	if (dx || dy)
	{
		r.Set(fCellBounds.left + fCellWidths[fFrozen.h] + 1.0,
			fCellBounds.top + fCellHeights[fFrozen.v] + 1.0,
			bounds.right, bounds.bottom);
		ScrollRect(r, dx, dy);
		
		r.Set(fCellBounds.left + fCellWidths[fFrozen.h] + 1.0, bounds.top + 1.0,
			bounds.right, fCellBounds.top + fCellHeights[fFrozen.v]);
		ScrollRect(r, dx, 0);
	
		r.Set(bounds.left, fCellBounds.top + fCellHeights[fFrozen.v] + 1.0,
			fCellBounds.left + fCellWidths[fFrozen.h], bounds.bottom);
		ScrollRect(r, 0, dy);
	
		if (dx) fHScrollBar->SetValue(fPosition.h);
		if (dy) fVScrollBar->SetValue(fPosition.v);

		Window()->UpdateIfNeeded();
	}
} /* CCellView::ScrollToPosition */

void CCellView::ScrollToNewValues(long x, long y)
{
	long L, R, v = 0, h = 0;
	
	if (x)
	{
		L = 1, R = kColCount;
		
		while (L <= R)
		{
			h = (L + R) / 2;
			if (x > fCellWidths[h])
				L = h + 1;
			else if (x < fCellWidths[h])
				R = h - 1;
			else
				break;
		}
	}
	
	if (y)
	{
		L = 1, R = kRowCount;
		
		while (L <= R)
		{
			v = (L + R) / 2;
			if (x > fCellHeights[v])
				L = v + 1;
			else if (x < fCellHeights[v])
				R = v - 1;
			else
				break;
		}
	}
	
	if (v || h)
		ScrollToPosition(h, v);
} /* CCellView::ScrollToNewValues */

void CCellView::ScrollToSelection()
{
	long dh = 0, dv = 0, vMax, hMax;
	BRect frame, r;

	hMax = fMaxPosition.h;
	vMax = fMaxPosition.v;

	GetCellRect(fCurCell, r);
	frame = fCellBounds;
	frame.top += fCellHeights[fFrozen.v];
	frame.left += fCellWidths[fFrozen.h];

	if (fCurCell.v < fPosition.v && fCurCell.v > fFrozen.v)
	{
		dv = fCurCell.v - fPosition.v;
	}
	else if (fCurCell.v > fFrozen.v)
	{
		while (fPosition.v + dv < vMax && r.bottom >= frame.bottom)
		{
			dv++;
			r.bottom -= (fCellHeights[fPosition.v + dv] - fCellHeights[fPosition.v + dv - 1]);
		}
		while (fPosition.v + dv > fFrozen.v + 1 && r.top <= frame.top)
		{
			dv--;
			r.top += (fCellHeights[fPosition.v + dv + 1] - fCellHeights[fPosition.v + dv]);
		}
	}
	
	if (fCurCell.h < fPosition.h && fCurCell.h > fFrozen.h)
	{
		dh = fCurCell.h - fPosition.h;
	}
	else if (fCurCell.h > fFrozen.h)
	{
		while (fPosition.h + dh < hMax && r.right >= frame.right)
		{
			dh++;
			r.right -= (fCellWidths[fPosition.h + dh] - fCellWidths[fPosition.h + dh - 1]);
		}
		while (fPosition.h + dh > fFrozen.h + 1 && r.left <= frame.left)
		{
			dh--;
			r.left += (fCellWidths[fPosition.h + dh + 1] - fCellWidths[fPosition.h + dh]);
		}
	}

	if (dh || dv)
		ScrollToPosition(fPosition.h + dh, fPosition.v + dv);
} /* ScrollToSelection */

void CCellView::AdjustScrollBars()
{
	BAutolock lock(Window());
	
	if (!lock.IsLocked())
		return;

	if (!fHScrollBar)
		fHScrollBar = (CCellScrollBar *)ScrollBar(B_HORIZONTAL);
	
	if (!fVScrollBar)
		fVScrollBar = (CCellScrollBar *)ScrollBar(B_VERTICAL);

	float width, height;
	
	width = fCellBounds.Width() - fCellWidths[fFrozen.h];
	height = fCellBounds.Height() - fCellHeights[fFrozen.v];
	
	fMaxPosition.Set(kColCount, kRowCount);
	while (fMaxPosition.h > 0 &&
		fCellWidths[kColCount] - fCellWidths[fMaxPosition.h - 2] < width)
		fMaxPosition.h--;
	
	while (fMaxPosition.v > 0 &&
		fCellHeights[kRowCount] - fCellHeights[fMaxPosition.v - 2] < height)
		fMaxPosition.v--;

	cell c;
	range r;

	fContainer->Lock();
	fContainer->GetBounds(r);
	fContainer->Unlock();

	c.h = r.right;
	if (fCurCell.h > c.h) c.h = fCurCell.h;
	if (fSelection.right > c.h) c.h = fSelection.right;
	c.h = ((c.h + 1) / 16 + 1) * 16;
	if (c.h > fMaxPosition.h) c.h = fMaxPosition.h;

	c.v = r.bottom;
	if (fCurCell.v > c.v) c.v = fCurCell.v;
	if (fSelection.bottom > c.v) c.v = fSelection.bottom;
	c.v = ((c.v + 1) / 64 + 1) * 64;
	if (c.v > fMaxPosition.v) c.v = fMaxPosition.v;

	if (fHScrollBar)
	{
		fHScrollBar->SetRange(fFrozen.h + 1, c.h + 1);
		fHScrollBar->SetValue(fPosition.h);
		fHScrollBar->SetSteps(1,
			std::max(-GetCellSpan(B_HORIZONTAL, false), GetCellSpan(B_HORIZONTAL, true)));
	}
	if (fVScrollBar)
	{
		fVScrollBar->SetRange(fFrozen.v + 1, c.v + 1);
		fVScrollBar->SetValue(fPosition.v);
		fVScrollBar->SetSteps(1,
			std::max(-GetCellSpan(B_VERTICAL, false), GetCellSpan(B_HORIZONTAL, false)));
	}
} /* CCellView::AdjustScrollBars */

void CCellView::ScrollRect(BRect r, float x, float y)
{
	BRect src, dst;
	
	dst = r;
	dst.OffsetBy(x, y);
	dst = r & dst;
	
	src = dst;
	src.OffsetBy(-x, -y);
	
	CopyBits(src, dst);
	
	if (x)
	{
		BRect tmp(r);
		if (x < 0)
			tmp.left = dst.right;
		else
			tmp.right = dst.left;
		Invalidate(tmp);
	}
	
	if (y)
	{
		BRect tmp(r);
		if (y < 0)
			tmp.top = dst.bottom;
		else
			tmp.bottom = dst.top;
		Invalidate(tmp);
	}
} /* CCellView::ScrollRect */
