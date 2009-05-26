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
	CellView.cells.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.
	
	routines voor berekeningen aan cellen en hun afmetingen
*/

#include "CellView.h"
#include "Container.h"
#include "CellScrollBar.h"
#include "FontMetrics.h"
#include <support/Debug.h>
#include "CellStyle.h"

void CCellView::GetCellTopLeft(cell c, BPoint& p)
{
	p.x = fCellBounds.left + fCellWidths[c.h] - fCellWidths[fPosition.h];
	p.y = fCellBounds.top + fCellHeights[c.v] - fCellHeights[fPosition.v];
} /* CCellView::GetCellTopLeft */

void CCellView::GetCellRect(cell c, BRect& r)
{
	ASSERT(c.v>0); ASSERT(c.v<=kRowCount);
	ASSERT(c.h>0); ASSERT(c.h<=kColCount);
	
	if (c.h <= fFrozen.h)
	{
		r.left = fCellWidths[c.h - 1] + fCellBounds.left + 1;
		r.right = fCellWidths[c.h] + fCellBounds.left - 1;
	}
	else if (fFrozen.h && c.h > fFrozen.h && c.h < fPosition.h)
	{
		r.left = r.right = fCellWidths[fFrozen.h] + fCellBounds.left;
	}
	else
	{
		r.left = fCellWidths[c.h - 1] - fCellWidths[fPosition.h - 1] +
			fCellWidths[fFrozen.h] + fCellBounds.left + 1;

		if (c.h > kColCount)
			r.right = fCellWidths[kColCount] - 1;
		else
			r.right = fCellWidths[c.h] - fCellWidths[c.h - 1] + r.left - 2;
	}

	if (c.v <= fFrozen.v)
	{
		r.top = fCellHeights[c.v - 1] + fCellBounds.top + 1;
		r.bottom = fCellHeights[c.v] + fCellBounds.top - 1;
	}
	else if (fFrozen.v && c.v > fFrozen.v && c.v < fPosition.v)
	{
		r.top = r.bottom = fCellHeights[fFrozen.v] + fCellBounds.top;
	}
	else
	{
	 	r.top = fCellHeights[c.v - 1] - fCellHeights[fPosition.v - 1] + 
	 		fCellHeights[fFrozen.v] + fCellBounds.top + 1;
	
		if (c.v > kRowCount)
			r.bottom = fCellHeights[kRowCount] - 1;
		else
			r.bottom = fCellHeights[c.v] - fCellHeights[c.v - 1] + r.top - 2;
	}
} /* GetCellRect */

void CCellView::GetCellBounds(cell c, BRect& r)
{
	BRect nextCellR;
	float sWid;
	EAlignment theAlignment;
	CellStyle cs;
	int vType = fContainer->GetType(c);

	fContainer->GetCellStyle(c, cs);
	GetCellRect(c, r);
	if (fFrozen.h && c.h > fFrozen.h && c.h < fPosition.h)
	{
		r.left = fCellBounds.left + fCellWidths[fFrozen.h] -
			(fCellWidths[fPosition.h - 1] - fCellWidths[c.h - 1]) + 1;
		r.right = r.left + fCellWidths[c.h] - fCellWidths[c.h - 1];
	}

	if (vType == eTextData)
	{
		char s[256];
		CFontMetrics *metrics;
		
		fContainer->GetCellResult(c, s, true);
		gFontSizeTable.SetFontID(this, cs.fFont, &metrics);
		
		sWid = metrics->StringWidth(s);
	}
	
	if (vType == eTextData && sWid > r.right - r.left)
	{
		if (cs.fAlignment == eAlignGeneral)
		{
			if (vType == eNumData && !isnan(fContainer->GetDouble(c)))
				theAlignment = eAlignRight;
			else
				theAlignment = eAlignLeft;
		}
		else
			theAlignment = (EAlignment)cs.fAlignment;

		switch (theAlignment)
		{

			case eAlignLeft:
			{
				int rb = c.h;
				float wb = fCellWidths[c.h - 1] + sWid + 2;
				
				while (rb < kColCount && wb > fCellWidths[rb])
					rb++;
				
				r.right += fCellWidths[rb] - fCellWidths[c.h];
				
				if (fContainer->GetNextCellInRow(c))
				{
					GetCellRect(c, nextCellR);
					r.right = min(nextCellR.left - 1, r.right);
				}
				break;
			}

			case eAlignCenter:
			{
				sWid = (sWid - r.right + r.left) / 2;
				float wr = fCellWidths[c.h] + sWid + 2,
					  wl = fCellWidths[c.h - 1] - sWid - 2;
				int rl = c.h, rr = c.h;

				while (rr < kColCount && wr > fCellWidths[rr])
					rr++;

				r.right += fCellWidths[rr] - fCellWidths[c.h];
				
				if (fContainer->GetNextCellInRow(c))
				{
					GetCellRect(c, nextCellR);
					r.right = min(nextCellR.left - 1, r.right);
				}

				while (rl > 1 && wl < fCellWidths[rl - 1])
					rl--;

				r.left -= fCellWidths[c.h - 1] - fCellWidths[rl - 1];

				if (fContainer->GetPreviousCellInRow(c))
				{
					GetCellRect(c, nextCellR);
					r.left = max(nextCellR.right + 1, r.left);
				}
				break;
			}

			case eAlignRight:
			{
				int rb = c.h;
				float wb = fCellWidths[rb] - sWid - 2;
				
				while (rb > 1 && wb < fCellWidths[rb - 1])
					rb--;

				r.left -= fCellWidths[c.h - 1] - fCellWidths[rb - 1];
				
				if (fContainer->GetPreviousCellInRow(c))
				{
					GetCellRect(c, nextCellR);
					r.left = max(nextCellR.right + 2, r.left);
				}
				break;
			}
			
			default:
				// there was a warning about not all enum values handled in 
				// switch statement.
				break;
		}
	}
} /* GetCellBounds */

long CCellView::GetCellSpan(orientation posture, bool upDirection)
{
	float x;
	long value, maxValue, span;
	BRect bounds(Bounds());

	if (upDirection)
	{
		if (posture == B_VERTICAL)
		{
			value = fPosition.v;
			x = fCellHeights[value];
			x -= bounds.bottom - fCellBounds.top - fCellHeights[fFrozen.h];
			if (x < 0.0) x = 0.0;
			do
			{
				value--;
			}
			while (fCellHeights[value - 1] >= x && value > 0);
			span = fPosition.v - value - 1;
		}
		else
		{
			value = fPosition.h;
			x = fCellWidths[value];
			x -= bounds.right - fCellBounds.left - fCellWidths[fFrozen.v];
			if (x < 0.0) x = 0.0;
			do
			{
				value--;
			}
			while (fCellWidths[value - 1] >= x && value > 0);
			span = fPosition.h - value - 1;
		}
	}
	else
	{
		if (posture == B_VERTICAL)
		{
			value = fPosition.v;
			maxValue = fMaxPosition.v;
			x = fCellHeights[value];
			x += bounds.bottom - fCellBounds.top - fCellHeights[fFrozen.v];
			do
			{
				value++;
			}
			while (fCellHeights[value + 1] <= x && value < maxValue + 1);
			span = fPosition.v - value + 1;
		}
		else
		{
			value = fPosition.h;
			maxValue = fMaxPosition.h;
			x = fCellWidths[value];
			x += bounds.right - fCellBounds.left - fCellWidths[fFrozen.h];
			do
			{
				value++;
			}
			while (fCellWidths[value + 1] <= x && value < maxValue + 1);
			span = fPosition.h - value + 1;
		}
	}
	
	return span;
} /* GetCellSpan */

void CCellView::SelectionToRect( BRect& r1 )
{
	BRect r2;

	GetCellRect(fSelection.TopLeft(), r1);
	GetCellRect(fSelection.BotRight(), r2);

	r1.SetRightBottom(r2.RightBottom());
}

void CCellView::SelectionToRegion(BRegion& rgn)
{
	BRect rect ;
	SelectionToRect( rect ) ;

	rgn.MakeEmpty();
	rgn.Include(rect & fCellBounds);
	
	BRegion clip;
	GetClip(clip);
	
	rgn.IntersectWith(&clip);
} /* SelectionToRegion */

float CCellView::GetColumnWidth(int colNr)
{
	return fCellWidths[colNr] - fCellWidths[colNr - 1] - 2;
} /* GetColumnWidth */

bool CCellView::GetCellHitBy(BPoint where, cell& c)
{
	BRect cellR;
	BPoint p;

	c = fPosition;

	p = where;
	p.x -= fCellBounds.left;
	p.y -= fCellBounds.top;
	
	if (fFrozen.h && p.x < fCellWidths[fFrozen.h])
	{
		c.h = 1;
		
		if (p.x > 0.0)
			while (c.h < kColCount && fCellWidths[c.h] < p.x)
				c.h++;
		else
			c.h = 1;
	}
	else
	{
		p.x -= fCellWidths[fFrozen.h];
		
		if (p.x > 0.0)
			while (c.h < kColCount && fCellWidths[c.h] - fCellWidths[fPosition.h - 1] < p.x)
				c.h++;
		else
			while (c.h > 1 && fCellWidths[c.h] - fCellWidths[fPosition.h - 1] > p.x)
				c.h--;
	}

	if (fFrozen.v && p.y < fCellHeights[fFrozen.v])
	{
		c.v = 1;
		
		if (p.y > 0.0)
			while (c.v < kRowCount && fCellHeights[c.v] < p.y)
				c.v++;
		else
			c.v = 1;
	}
	else
	{
		p.y -= fCellHeights[fFrozen.v];
		
		if (p.y > 0.0)
			while (c.v < kRowCount && fCellHeights[c.v] - fCellHeights[fPosition.v - 1] < p.y)
				c.v++;
		else
			while (c.v > 1 && fCellHeights[c.v] - fCellHeights[fPosition.v - 1] > p.y)
				c.v--;
	}
	
	if (c.h > 0 && c.h <= kColCount &&
		c.v > 0 && c.v <= kRowCount)
	{
		GetCellRect(c, cellR);
		return cellR.Contains(where);
	}
	else
		return false;
} /* GetCellHitBy */

bool CCellView::PointIsInSelection(BPoint where)
{
	BRegion rgn;
	SelectionToRegion(rgn);
	return rgn.Contains(where);
} /* CCellView::PointIsInSelection */

void CCellView::SetSylkColumnWidths(int start, int stop, int width)
{
	float w = width * be_plain_font->StringWidth("x");
	
	fCellWidths.SetValue(start, stop, w);
} /* CCellView::SetSylkColumnWidths */
