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
	CellView.draw.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/


#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   SELECTIONVIEW_H
#include "SelectionView.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   CELLSCROLLBAR_H
#include "CellScrollBar.h"
#endif

#ifndef   CELLSTYLE_H
#include "CellStyle.h"
#endif

#ifndef   GRAPHIC_H
#include "Graphic.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   STLOCKER_H
#include "StLocker.h"
#endif

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif

#include <Autolock.h>
#include <Screen.h>

const rgb_color
	kTransparent = { 255, 255, 255, 255 };

int gGridColor = 8;

void CCellView::Draw(BRect updateRect)
{
// workaround for bug in calling order
	if (fCellBounds.RightBottom() != Bounds().RightBottom())
	{
		fCellBounds = Bounds();
		if (fShowBorders)
		{
			fCellBounds.left += fBorderWidth;
			fCellBounds.top += fBorderHeight;
		}
		if (!IsPrinting())
			AdjustScrollBars();
	}

	StPenState save(this);

	try
	{
		int line;
		float cellTop;
		BRect r;
		
		SetClip(NULL);

		if (!fOnlyTheLines)
			DrawGrid();

		if (fFrozen.v)
			line = 1;
		else
			line = fPosition.v;
		
		BRegion hiliteClip, clip;

		GetClip(clip);
		
		do
		{
			cell c(fPosition.h, line);
			GetCellRect(c, r);
			r.right = fCellBounds.right;

			if ((!fOnlyTheLines || fDirtyLines[line]) &&
				 IsPrinting() ? fCellBounds.Intersects(r) : clip.Intersects(r))
			{
				DrawLine(clip, line);
				
				if (line >= fSelection.top && line <= fSelection.bottom)
					hiliteClip.Include(r);
			}
			
			if (line == fFrozen.v)
				line = fPosition.v;
			else
				line++;
				
			cellTop = r.top;
		}
		while (line <= kRowCount && cellTop < updateRect.bottom);
		
		if (fOnlyTheLines && !fSuppressHighlight)
		{
			hiliteClip.IntersectWith(&clip);
			SetClip(&hiliteClip);
			HiliteSelection(!fWindowActive);
		}
	
		if (!fOnlyTheLines && !IsPrinting() && !fSuppressHighlight)
			HiliteSelection(!fWindowActive);
		
		ConstrainClippingRegion(NULL);
		
		float min, max;
		fVScrollBar->GetRange(&min, &max);
		if (fPosition.v == max)
		{
			r = fCellBounds;
			r.top += fCellHeights[fFrozen.v];
			r.top += fCellHeights[kRowCount] -
				fCellHeights[fPosition.v - 1];
			r.top++;
			r.left++;
			FillRect(r, B_SOLID_LOW);
		}
		
		fHScrollBar->GetRange(&min, &max);
		if (fPosition.h == max)
		{
			r = fCellBounds;
			r.top++;
			r.left += fCellWidths[fFrozen.h];
			r.left += fCellWidths[kColCount] -
				fCellWidths[fPosition.h - 1];
			r.left++;
			FillRect(r, B_SOLID_LOW);
		}

		if (!fOnlyTheLines && !IsPrinting() && fBorderHeight > 0.0)
		{
			if (fShowBorders) DrawBorders();
			DrawStatus();
		}
		else if (!fOnlyTheLines && fShowBorders) {
			DrawBorders();
		}
		else if (fBorderHeight == 0.0)
		{		
			if (!IsPrinting()) SetHighColor(kB_GrayTable[13]);
			StrokeLine(r.LeftTop(), r.RightTop());
			if (!IsPrinting()) SetHighColor(kB_GrayTable[1]);
			StrokeLine(r.LeftTop(), r.LeftBottom());
			if (!IsPrinting()) SetHighColor(kB_GrayTable[0]);
		}

		ConstrainClippingRegion(NULL);

		if (!fOnlyTheLines && fFirstGraphic)
			fFirstGraphic->HandleUpdate(updateRect);
	}
	catch (CErr& e)
	{
		CATCHED;
		e.DoError();
	}
} /* CCellView::Draw */
	
void CCellView::SetBorderFont(CFontMetrics **metrics)
{
	gFontSizeTable.SetFontID(this, fBorderFontID, metrics);
} /* CCellView::SetBorderFont */

void CCellView::DrawBorders()
{
	float x, y, prFactor = IsPrinting() ? 0.5 : 0;
	BRect bounds(Bounds());
	BRegion clip;
	char s[10];
	BRect r;
	cell c;
	bool rowSelect = (fSelection.left == 1 && fSelection.right == kColCount),
		 colSelect = (fSelection.top == 1 && fSelection.bottom == kRowCount),
		 active = fWindowActive;

	StPenState save(this);

	SetBorderFont();

	if (IsPrinting())
		SetPenSize(0.5);
	else
		SetHighColor(kB_GrayTable[13]);
	
	if (IsPrinting())
		clip.Include(bounds);
	else
		GetClip(clip);

	if (IsPrinting()) 
		StrokeLine(BPoint(bounds.left + 0.25, bounds.top),
				   BPoint(bounds.left + 0.25, bounds.bottom - 0.50));

	StrokeLine(BPoint(bounds.left, bounds.top),
			   BPoint(bounds.right - prFactor, bounds.top));
	StrokeLine(BPoint(bounds.left, bounds.top + fBorderHeight),
			   BPoint(bounds.right - prFactor, bounds.top + fBorderHeight));
	StrokeLine(BPoint(bounds.left + fBorderWidth, bounds.top),
			   BPoint(bounds.left + fBorderWidth, bounds.bottom - prFactor));

	r.left = bounds.left;
	r.top = bounds.top + 1;
	r.right = r.left + fBorderWidth - 1.0;
	r.bottom = r.top + fBorderHeight - 2.0;
	if (IsPrinting()) r.left++;
	if (clip.Intersects(r))
//	if (bounds.Intersects(r))
		FillRect3D(this, r, rowSelect && colSelect, active, NULL);

	if (fFrozen.h || fFrozen.v)
	{
		c = cell(1, 1);
		r.top = bounds.top + fBorderHeight + 1.0;
		r.left = bounds.left;
		r.right = r.left + fBorderWidth - 1.0;
		r.bottom = r.top + fCellHeights[1] - 2.0;
		if (IsPrinting()) r.left++;
	
		while (c.v <= fFrozen.v)
		{
			if (clip.Intersects(r))
//			if (bounds.Intersects(r))
			{
				sprintf(s, "%d", c.v);
				
				FillRect3D(this, r,
					rowSelect && c.v >= fSelection.top && c.v <= fSelection.bottom,
					active || IsPrinting(), s);
			}
			
			r.top = r.bottom + 2.0;
			if (c.v < kRowCount)
				r.bottom += fCellHeights[c.v + 1] - fCellHeights[c.v];
			
			if (c.v == fFrozen.v && !IsPrinting())
				SetHighColor(kB_GrayTable[0]);

			StrokeLine(BPoint(bounds.left, r.top - 1.0),
				BPoint(bounds.left + fBorderWidth, r.top - 1.0));
			
			if (!IsPrinting()) SetHighColor(kB_GrayTable[13]);

			c.v++;
		}

		r.left = bounds.left + fBorderWidth + 1.0;
		r.top = bounds.top + 1.0;
		r.right = r.left + fCellWidths[1] - 2.0;
		r.bottom = r.top + fBorderHeight - 2.0;
	
		while (c.h <= fFrozen.h)
		{
			if (clip.Intersects(r))
//			if (bounds.Intersects(r))
			{
				NumToAString(c.h, s);
	
				FillRect3D(this, r, colSelect && c.h >= fSelection.left && c.h <= fSelection.right,
					active || IsPrinting(), s);
			}
			r.left = r.right + 2.0;
			if (c.h < kColCount)
				r.right += fCellWidths[c.h + 1] - fCellWidths[c.h];

			if (c.h == fFrozen.h && !IsPrinting())
				SetHighColor(kB_GrayTable[0]);
				
			StrokeLine(BPoint(r.left - 1.0, bounds.top),
				BPoint(r.left - 1.0, bounds.top + fBorderHeight));
			
			if (!IsPrinting()) SetHighColor(kB_GrayTable[13]);

			c.h++;
		}
	}

	y = fCellBounds.Height() + fCellHeights[fPosition.v];
	x = fCellBounds.Width() + fCellWidths[fPosition.h];

	c.v = fPosition.v;
	r.top = bounds.top + fBorderHeight + 1.0 + fCellHeights[fFrozen.v];
	r.left = bounds.left;
	r.right = r.left + fBorderWidth - 1.0;
	r.bottom = r.top + fCellHeights[c.v] - fCellHeights[c.v - 1] - 2.0;
	if (IsPrinting()) r.left++;

	while (c.v <= kRowCount && fCellHeights[c.v - 1] < y)
	{
		if (IsPrinting() && !bounds.Contains(r))
			break;
		else if (clip.Intersects(r))
//		else if (bounds.Intersects(r))
		{
			sprintf(s, "%d", c.v);
						
			FillRect3D(this, r, rowSelect && c.v >= fSelection.top && c.v <= fSelection.bottom,
				active || IsPrinting(), s);
		}

		r.top = r.bottom + 2.0;
		if (c.v < kRowCount)
			r.bottom += fCellHeights[c.v + 1] - fCellHeights[c.v];

		StrokeLine(BPoint(bounds.left, r.top - 1.0),
			BPoint(bounds.left + fBorderWidth, r.top - 1.0));
		c.v++;
	}
	
	if (c.v > kRowCount)
	{
		r.top = r.bottom + 2.0;
		r.bottom = bounds.bottom;
		FillRect(r, B_SOLID_LOW);
	}

	c.h = fPosition.h;
	r.left = bounds.left + fBorderWidth + 1.0 + fCellWidths[fFrozen.h];
	r.top = bounds.top + 1.0;
	r.right = r.left + fCellWidths[c.h] - fCellWidths[c.h - 1] - 2.0;
	r.bottom = r.top + fBorderHeight - 2.0;

	while (c.h <= kColCount && fCellWidths[c.h - 1] < x)
	{
		if (IsPrinting() && !bounds.Contains(r))
			break;
		else if (clip.Intersects(r))
//		else if (bounds.Intersects(r))
		{
			NumToAString(c.h, s);

			FillRect3D(this, r, colSelect && c.h >= fSelection.left && c.h <= fSelection.right,
				active || IsPrinting(), s);
		}

		r.left = r.right + 2.0;
		if (c.h < kColCount)
			r.right += fCellWidths[c.h + 1] - fCellWidths[c.h];

		StrokeLine(BPoint(r.left - 1.0, bounds.top),
			BPoint(r.left - 1.0, bounds.top + fBorderHeight));
		c.h++;
	}
	
	if (c.h > kColCount)
	{
		r.left = r.right + 2.0;
		r.right = bounds.right;
		FillRect(r, B_SOLID_LOW);
	}
	
	if (IsPrinting())
		SetPenSize(1);
	else
		SetHighColor(kB_GrayTable[0]);
} /* CCellView::DrawBorders */

void CCellView::DrawGrid()
{
	StPenState save(this);
	
	float relX, relY, frLeft, frTop;
	int x, y, lineCount;
	cell c;
	
	if (IsPrinting())
	{
		if (fShowGrid) {
			SetPenSize(0.5);
			BRect r(fCellBounds);
			r.InsetBy(0.5, 0.5);
			StrokeLine(fCellBounds.LeftTop(), fCellBounds.RightTop());
			StrokeLine(fCellBounds.LeftTop(), fCellBounds.LeftBottom());
		}
	}
	else
	{
		SetHighColor(kB_GrayTable[fSuppressHighlight ? gGridColor : 13]);
		StrokeLine(fCellBounds.LeftTop(), fCellBounds.RightTop());
		StrokeLine(fCellBounds.LeftTop(), fCellBounds.LeftBottom());
	}

	if (fFrozen.h || fFrozen.v)
	{
		BeginLineArray(fFrozen.h + fFrozen.v);
	
		c = cell(1, 1);
		
		while (c.h <= fFrozen.h)
		{
			BPoint start, finish;
			
			start.x = fCellBounds.left + fCellWidths[c.h];
			start.y = fCellBounds.top;
			
			int lineColor = gGridColor;

			if (IsPrinting() && fShowGrid)
				lineColor = 0;
			else if (!fShowGrid)
				lineColor = 1;
			else if (c.h == fFrozen.h)
				lineColor += 6;
	
			finish = start;
			finish.y = fBorderHeight + fCellBounds.Height();
			
			AddLine(start, finish, kB_GrayTable[lineColor]);
			c.h++;
		}
		
		while (c.v <= fFrozen.v)
		{
			BPoint start, finish;
			
			start.x = fCellBounds.left;
			start.y = fCellBounds.top + fCellHeights[c.v];

			int lineColor = gGridColor;

			if (IsPrinting() && fShowGrid)
				lineColor = 0;
			else if (!fShowGrid)
				lineColor = 1;
			else if (c.v == fFrozen.v)
				lineColor += 6;

			finish = start;
			finish.x = fBorderWidth + fCellBounds.Width();
			
			AddLine(start, finish, kB_GrayTable[lineColor]);
			c.v++;
		}
		
		EndLineArray();
	}

	frLeft = fCellBounds.left + fCellWidths[fFrozen.h];
	frTop = fCellBounds.top + fCellHeights[fFrozen.v];

	relX = fCellWidths[fPosition.h-1];
	relY = fCellHeights[fPosition.v-1];

	x = (int)(fCellBounds.Width()+relX);
	y = (int)(fCellBounds.Height()+relY);

	c = fPosition;
	lineCount = 0;
	while (c.h <= kColCount && fCellWidths[c.h++] <= x)
		lineCount++;
	while (c.v <= kRowCount && fCellHeights[c.v++] <= y)
		lineCount++;
	BeginLineArray(lineCount);

	c = fPosition;
	
	while (c.h <= kColCount && fCellWidths[c.h] <= x)
	{
		BPoint start, finish;
		
		finish.x = start.x = frLeft + fCellWidths[c.h]-relX;
		start.y = 0;
		finish.y = fCellBounds.bottom;
		
		int lineColor = gGridColor;

		if (IsPrinting() && fShowGrid)
			lineColor = 0;
		else if (!fShowGrid)
			lineColor = 1;
		else if (fHPageBreaks[c.h])
			lineColor += 8;

		finish = start;
		finish.y = fCellBounds.bottom - fCellBounds.top;
		
		AddLine(start, finish, kB_GrayTable[lineColor]);
		c.h++;
	}
	
	while (c.v <= kRowCount && fCellHeights[c.v] <= y)
	{
		BPoint start, finish;
		
		start.x = 0;
		finish.x = fCellBounds.right;
		start.y = finish.y = frTop + fCellHeights[c.v] - relY;

		int lineColor = gGridColor;

		if (IsPrinting() && fShowGrid)
			lineColor = 0;
		else if (!fShowGrid)
			lineColor = 1;
		else if (fVPageBreaks[c.v])
			lineColor += 8;

		AddLine(start, finish, kB_GrayTable[lineColor]);
		c.v++;
	}
	
	EndLineArray();

	if (IsPrinting())
		SetPenSize(1);
} /* CCellView::DrawGrid */

void CCellView::DrawAllLines(void)
{
	fOnlyTheLines = true;
	Draw(fCellBounds);
	fOnlyTheLines = false;
} /* CCellView::DrawAllLines */

void CCellView::TouchLine(int lineNr)
{
	fDirtyLines += lineNr;
} /* CCellView::TouchLine */

void CCellView::DrawLine(BRegion& updateRgn, int lineNr)
{
	cell c;
	BRect r;
	float right = Bounds().right, prFactor = IsPrinting() ? 0.5 : 0;
	CellStyle cs;

	rgb_color savedColor = HighColor();
	if (IsPrinting())
		SetPenSize(0.5);

	BAutolock lock(fContainer);

	if (fFrozen.h)
	{
		c.v = lineNr;
		c.h = 1;

		do
		{
			GetCellRect(c, r);
			
			fContainer->GetCellStyle(c, cs);
			if (cs.fLowColor.alpha == 0)
				SetLowColor(kWhite);
			else
				SetLowColor(cs.fLowColor);
			
			FillRect(r, B_SOLID_LOW);
			
			if (!fShowGrid)
				continue;
			else if (IsPrinting())
			{
				SetHighColor(kBlack);
				SetPenSize(0.5);
			}
			else if (c.h == fFrozen.h)
				SetHighColor(kB_GrayTable[gGridColor + 6]);
			else
				SetHighColor(kB_GrayTable[gGridColor]);
			
			if (fCellBounds.right > r.right + 1)
				StrokeLine(BPoint(r.right + 1.0, r.top - prFactor),
					BPoint(r.right + 1.0, r.bottom + prFactor));
			c.h++;

			if (IsPrinting())
				SetPenSize(1);
		}
		while (c.h <= fFrozen.h);
	
		c.h = 1;
	
		if (fContainer->GetType(c) != eNoData)
		{
			GetCellBounds(c, r);
			if (updateRgn.Intersects(r))
				DrawCellInRect(c, r);
		}

		while (fContainer->GetNextCellInRow(c) && c.h <= fFrozen.h)
		{
			GetCellBounds(c, r);
			if (r.right >= r.left && r.bottom >= r.top)
				if (updateRgn.Intersects(r))
					DrawCellInRect(c, r);
				else if (r.left > right)
					break;
		}
	}
	
	c.v = lineNr;
	c.h = fPosition.h;

	do
	{
		GetCellRect(c, r);

		fContainer->GetCellStyle(c, cs);
		if (cs.fLowColor.alpha == 0)
			SetLowColor(kWhite);
		else
			SetLowColor(cs.fLowColor);
			
		FillRect(r, B_SOLID_LOW);

		if (!fShowGrid)
			continue;
		else if (IsPrinting())
		{
			SetHighColor(kBlack);
			SetPenSize(0.5);
		}
		else if (c.h == fFrozen.h)
			SetHighColor(kB_GrayTable[gGridColor + 6]);
		else if (fHPageBreaks[c.h])
			SetHighColor(kB_GrayTable[gGridColor + 8]);
		else
			SetHighColor(kB_GrayTable[gGridColor]);

		if (fCellBounds.right >= r.right + 1.0)
			StrokeLine(BPoint(r.right + 1.0, r.top - prFactor),
				BPoint(r.right + 1.0, r.bottom + prFactor));
		
		if (IsPrinting())
			SetPenSize(1);
	}
	while (c.h++ < kColCount && r.right + 1.0 < right);

	c.h = fPosition.h;

	if (fContainer->GetType(c) != eNoData)
	{
		GetCellBounds(c, r);
		if (updateRgn.Intersects(r))
			DrawCellInRect(c, r);
	}
	else if (fContainer->GetPreviousCellInRow(c) && c.h > fFrozen.h)
	{
		GetCellBounds(c, r);
		if (updateRgn.Intersects(r))
			DrawCellInRect(c, r);
	}

	while (fContainer->GetNextCellInRow(c))
	{
		GetCellBounds(c, r);
		if (r.right >= r.left && r.bottom >= r.top)
			if (updateRgn.Intersects(r))
				DrawCellInRect(c, r);
			else if (r.left > right)
				break;
	}
	
	fDirtyLines -= lineNr;
	
	SetHighColor(savedColor);
} /* CCellView::DrawLine */

void CCellView::DrawCell(cell c)
{
	BRect r, br;
	cell c2 = c;

	BAutolock lock(fContainer);
	StPenState save(this);
	StClipCells clip(this);
	
	int type = fContainer->GetType(c);
	
	if (type == eNoData)
		GetCellRect(c, r);
	else
		GetCellBounds(c, r);
	
	BRegion rgn;
	rgn.Include(r);

	CellStyle cs;
	fContainer->GetCellStyle(c, cs);
	if (cs.fLowColor.alpha != 0)
		SetLowColor(cs.fLowColor);
	else
		SetLowColor(kWhite);
	FillRect(r, B_SOLID_LOW);
	SetLowColor(kWhite);

	if (type != eNoData)
	{
		DrawCellInRect(c, r);
	}
	else
	{
		bool drawn = false;
	
		if (fContainer->GetPreviousCellInRow(c2) && c.h > fFrozen.h)
		{
			GetCellBounds(c2, br);
			if (r.Intersects(br))
			{
				DrawCellInRect(c2, br);
				drawn = true;
				rgn.MakeEmpty();
				rgn.Include(br);
			}
		}
		if (!drawn && fContainer->GetNextCellInRow(c2))
		{
			GetCellBounds(c2, br);
			if (r.Intersects(br))
			{
				DrawCellInRect(c2, br);
				drawn = true;
				rgn.MakeEmpty();
				rgn.Include(br);
			}
		}
	}

	if (fSelection.Contains(c))
	{
		SetClip(&rgn);
		HiliteSelection(!fWindowActive);
	}
} /* CCellView::DrawCell */

void CCellView::DrawCellInRect(cell c, BRect r)
{
	CFontMetrics *metrics;
	font_height fi;
	char s[256];
	EAlignment alignment;
	float w = -1;
	CellStyle cs;
	bool numIsNan;
	BRect cr;

	BAutolock lock(fContainer);
	
	GetCellRect(c, cr);

	int/*ValueType*/ type = fContainer->GetType(c);
	rgb_color savedColor = HighColor();
	
	fContainer->GetCellStyle(c, cs);
	gFontSizeTable.SetFontID(this, cs.fFont, &metrics);
	metrics->Font().GetHeight(&fi);

	if (fShowFormulas)
		fContainer->GetCellFormula(c, s);
	else
		w = fContainer->GetCellResult(c, s, false);
	
	numIsNan = (type == eNumData) && isnan(fContainer->GetDouble(c));

	if (numIsNan || fShowFormulas)
		alignment = eAlignLeft;
	else
	{
		if (cs.fAlignment == eAlignGeneral)
		{
			if (type == eNumData && !numIsNan)
				alignment = eAlignRight;
			else
				alignment = eAlignLeft;
		}
		else
			alignment = (EAlignment)cs.fAlignment;
	}

	if (alignment != eAlignLeft || type == eTextData)
	{
		if (w == -1)
			w = metrics->StringWidth(s);
	}
	else
		w = 0.0;

	switch (alignment)
	{
		default:
		case eAlignLeft:
			if (fFrozen.h && c.h > fFrozen.h && c.h < fPosition.h)
				MovePenTo(r.left + 1.0,
					cr.bottom - fi.descent - fi.leading);
			else
				MovePenTo(cr.left + 1.0,
					cr.bottom - fi.descent - fi.leading);
			break;
		case eAlignCenter:
			if (fFrozen.h && c.h > fFrozen.h && c.h < fPosition.h)
				MovePenTo(r.left + 1.0,
					cr.bottom - fi.descent - fi.leading);
			else
				MovePenTo(cr.left + (cr.right - cr.left - w) / 2.0 + 1.0,
					cr.bottom - fi.descent - fi.leading);
			break;
		case eAlignRight:
			MovePenTo(cr.right - w + 1.0,
				cr.bottom - fi.descent - fi.leading);
			break;
	}

	if (w > cr.Width() || fShowFormulas ||
		r.Height() < fi.ascent + fi.descent + fi.leading)
	{
		BRect clipR;
		float left;
		
		left = 1.0;
		if (c.h > fFrozen.h)
			left += fCellWidths[fFrozen.h];

		if (IsPrinting())
			r.InsetBy(0, -0.5);

		clipR = r;
		if (r.left < left)
			clipR.left = left;
		
		if (c.h <= fFrozen.h && r.right > fCellWidths[fFrozen.h])
			clipR.right = fCellWidths[fFrozen.h] - 1.0;
		
		if (cs.fLowColor.alpha != 0)
			SetLowColor(cs.fLowColor);
		else
			SetLowColor(kWhite);

		BRegion clip;
		clipR.left = std::max(clipR.left, fCellBounds.left + 1);
		clip.Include(clipR);
		StClipCells clipit(this, &clip);

		FillRect(r, B_SOLID_LOW);
		DrawString(s);
	}
	else
	{
		if (cs.fLowColor.alpha != 0)
			SetLowColor(cs.fLowColor);

		DrawString(s);
	}

	SetHighColor(savedColor);
	SetLowColor(kWhite);
} /* CCellView::DrawCellInRect */

void CCellView::HiliteSelection(bool background, bool subtract)
{
	BRegion theSelection;

	SelectionToRegion(theSelection);

	StPenState save(this);
	SetClip(&theSelection);
	
	if (background)
	{
		SetDrawingMode(B_OP_INVERT);
		BRect r ;
		SelectionToRect(r) ;
		StrokeRect(r);
		SetDrawingMode(B_OP_COPY);
	}
	else if (gPrefs->GetPrefInt("Select Gray"))
	{
		SetHighColor(0x30, 0x30, 0x30);
		SetDrawingMode(subtract ? B_OP_SUBTRACT : B_OP_ADD);
		
		FillRect(theSelection.Frame());

		SetDrawingMode(B_OP_COPY);
		SetHighColor(kBlack);
	}
	else
		InvertRect(theSelection.Frame());
} /* CCellView::HiliteSelection */

void CCellView::ChangeSelection(BRegion *oldRgn, BRegion *newRgn)
{
	StClipCells clip(this);

	if (gPrefs->GetPrefInt("Select Gray"))
	{
		SetHighColor(0x30, 0x30, 0x30);
	
		BRegion tmp(*oldRgn);
		tmp.Exclude(newRgn);
		SetClip(&tmp);
		SetDrawingMode(B_OP_ADD);
		FillRect(tmp.Frame());
	
		tmp = *newRgn;
		tmp.Exclude(oldRgn);
		SetClip(&tmp);
		SetDrawingMode(B_OP_SUBTRACT);
		FillRect(tmp.Frame());
	
		SetDrawingMode(B_OP_COPY);
		SetHighColor(kBlack);
	}
	else
	{
		BRegion tmp(*oldRgn);
		tmp.Exclude(newRgn);
		SetClip(&tmp);
		InvertRect(tmp.Frame());
		
		tmp = *newRgn;
		tmp.Exclude(oldRgn);
		SetClip(&tmp);
		InvertRect(tmp.Frame());
	}
} // CCellView::ChangeSelection

void CCellView::DrawStatus(range *selection, bool updateEditField)
{
	if (!selection)
		selection = &fSelection;
	
	fSelView->SetRange(selection);
	
	if (updateEditField && !fEntering)
	{
		char s[256];
		BAutolock lock(fContainer);
		fContainer->GetCellFormula(fCurCell, s);
		fEditBox->SetText(s);
		fEditBox->Draw(fEditBox->Bounds());
	}
} /* CCellView::DrawStatus */

void CCellView::DrawBorderCell(int nr, bool horizontal)
{
	BRect r;
	char s[32];
	bool pushed;
	
	if (!fShowBorders)
		return;
	
	if (horizontal)
	{
		NumToAString(nr, s);
		GetCellRect(cell(nr, 1), r);
		r.top = 1;
		r.bottom = fBorderHeight-1;
		pushed = (nr >= fSelection.left && nr <= fSelection.right &&
			fSelection.top == 1 && fSelection.bottom == kRowCount);
	}
	else
	{
		sprintf(s, "%d", nr);
		GetCellRect(cell(1, nr), r);
		r.left = 0;
		r.right = fBorderWidth-1;
		pushed = (nr >= fSelection.top && nr <= fSelection.bottom &&
			fSelection.left == 1 && fSelection.right == kColCount);
	}
	
	SetBorderFont();
	FillRect3D(this, r, pushed, true, s);
	
	rgb_color color = HighColor();
	SetHighColor(kB_GrayTable[13]);
	r.InsetBy(-1,-1);
	StrokeRect(r);
	SetHighColor(color);
} // CCellView::DrawBorderCell

void CCellView::GetPictureOfSelection(BPicture *pic, BRect& r)
{
	cell pos = fPosition;
	cell frost = fFrozen;
	bool grid = fShowGrid;
	bool borders = fShowBorders;
	BRect cellBounds = fCellBounds;
	
//	fShowGrid = true;
	fShowBorders = false;
	fCellBounds = Bounds();
	
	BRect b = fCellBounds;

	fPosition.Set(fSelection.left, fSelection.top);
	fFrozen.Set(0, 0);
	fSuppressHighlight = true;
	
	try
	{
		BeginPicture(pic);
		
		GetCellRect(fSelection.BotRight(), r);
		r.left = r.top = 0;
		r.right += 1; r.bottom += 1;
		
		{
			r = BScreen().Frame() & r;
		}
		
		ResizeTo(r.Width(), r.Height());

		BRegion sel;
		sel.Include(r);
		ConstrainClippingRegion(&sel);
		
		Draw(r);
		
		// MV
		ConstrainClippingRegion(NULL);

		ResizeTo(b.Width(), b.Height());
	}
	catch(...) {}

	EndPicture();

	fPosition = pos;
	fFrozen = frost;
	fShowGrid = grid;
	fShowBorders = borders;
	fCellBounds = cellBounds;
	fSuppressHighlight = false;
} // CCellView::GetPictureOfSelection
