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
	CellView.resizing.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "CellView.h"
#include "Command.h"
#include "CellCommands.h"
#include "ResizeCommands.h"
#include "DrawUtils.h"
#include "Utils.h"
#include "CellWindow.h"
#include <support/Debug.h>
#include "Globals.h"
#include "Round.h"
#include "Graphic.h"

const pattern
	kColResizePattern = {{ 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 }},
	kRowResizePattern = {{ 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0 }};

void CCellView::ResizeRow(BPoint where, int rowNr)
{
	float y, minY;
	BPoint newP, lastP;
	BRect bounds(Bounds()), r, b;
	BRegion rgn;
	uint32 buttons, cnt;
	bool multi;
	int mRow = rowNr;
	CRunArray backup(fCellHeights);
	
	StPenState save(this);
	ClearAnts();
	
	rgn.Include(bounds);

	cnt = fSelection.bottom-fSelection.top+1;

	while (where.y > fCellHeights[rowNr+1]+2)
		rowNr++;

	multi = fSelection.top != fSelection.bottom
					&& rowNr >= fSelection.top
					&& rowNr <= fSelection.bottom;

	if (rowNr <= fFrozen.v)
	{
		y = fBorderHeight+fCellHeights[rowNr];
		minY = fBorderHeight+fCellHeights[multi?fSelection.top-1:rowNr-1];
	}
	else
	{
		y = fBorderHeight+fCellHeights[rowNr]+fCellHeights[fFrozen.v]-
			fCellHeights[fPosition.v-1];
		minY = fBorderHeight+fCellHeights[fFrozen.v]-fCellHeights[fPosition.v-1]+
			fCellHeights[multi?fSelection.top-1:rowNr-1];
	}
	
	r.left = 0;
	r.right = fBorderWidth-1;
	r.top = minY+1;
	if (multi)
		r.bottom = minY+fCellHeights[fSelection.bottom]-
			fCellHeights[fSelection.top-1]-1;
	else
		r.bottom = minY+fCellHeights[rowNr]-fCellHeights[rowNr-1]-1;

	b = bounds;
	b.top = r.bottom;
	
	lastP = where;
	newP = where;
	do
	{
		if (newP.y != lastP.y)
		{
			float dy;
			int k = rowNr;
			
			dy = newP.y - lastP.y;

			if (lastP.y + dy < bounds.top + fBorderHeight)
				dy = std::min(lastP.y - bounds.top - fBorderHeight, (float)0);
			if (lastP.y + dy > bounds.bottom)
				dy = std::max(bounds.bottom - lastP.y, (float)0);
			
			if (multi && y + dy < minY)
				dy = minY - y;

			if (multi)
			{
				float h, t;
				
				h = y + dy - minY;
				if (h > 0)
					h = Round(h/(rowNr-fSelection.top+1));
				
				t = fCellHeights[fSelection.bottom];				
				fCellHeights.SetValue(fSelection.top, fSelection.bottom, h);
				dy = fCellHeights[fSelection.bottom] - t;
			}
			else if (y + dy < minY)
			{
				int t = k;
				
				fCellHeights.SetValue(rowNr, 0);

				while (k > 1 && backup[k - 1] + fBorderHeight >= y + dy)
					k--;

				ASSERT(k<rowNr);

				if (k <= mRow)
					while (mRow > k)
						fCellHeights.SetValue(mRow--, 0);
				else
				{
					t = mRow;
					for (; mRow < k; mRow++)
						fCellHeights.SetValue(mRow, backup[mRow] - backup[mRow - 1]);
				}

				float kh = y + dy - fBorderHeight;
				kh -= backup[k - 1];
				fCellHeights.SetValue(k, std::max(kh, (float)0));

				mRow = k;
				k = t;
			}
			else
			{
				k = mRow;
				for (; mRow < rowNr; mRow++)
					fCellHeights.SetValue(mRow, backup[mRow] - backup[mRow - 1]);

				fCellHeights.SetValue(rowNr, y + dy - minY);
			}
					
			if (dy != 0)
			{
				y += dy;
				lastP.y += dy;

				if (dy < 0)
					b.top += dy;
				
				StPenState save(this);
				StClipCells clip(this);
				ScrollRect(b, 0, dy);
				Window()->UpdateIfNeeded();
				
				if (dy > 0)
					b.top += dy;

				if (multi)
				{
					int v = fSelection.top;
					do
					{
						TouchLine(v);
						DrawBorderCell(v++, false);
					}
					while (v <= fSelection.bottom && bounds.top + fCellHeights[v] + fBorderHeight < bounds.bottom);
				}
				else
				{
					TouchLine(mRow);
					DrawBorderCell(mRow, false);
				}
				
				for (; k < rowNr; k++)
				{
					TouchLine(k);
					DrawBorderCell(k, false);
				}
			}
			
			if (dy < 0)
			{
				float Y;
				int v;
				
				Y = bounds.bottom + dy - fBorderHeight;
				v = fSelection.bottom;
				do v++;
				while (fCellHeights[v] < Y)
					;

				StPenState save(this);
				
				while (v <= kRowCount && bounds.top + fCellHeights[v] + fBorderHeight < bounds.bottom)
				{
					TouchLine(v);
					DrawBorderCell(v++, false);
				}
			}
			
			if (dy)
			{
				DrawAllLines();
				if (fFirstGraphic)
					fFirstGraphic->HandleUpdate(bounds);
			}
		}
		GetMouse(&newP, &buttons);
	}
	while (buttons);
	
	if (!(backup == fCellHeights))
	{
		CRunArray temp(fCellHeights);
		fCellHeights = backup;
		
		((CCellWindow*)Window())->RegisterCommand(new CResizeCommand(this, temp, true));
		((CCellWindow*)Window())->SetDirty(true);
	}
} /* ResizeRow */

void CCellView::ResizeCol(BPoint where, int colNr)
{
	float x, minX, maxRow;
	BPoint newP, lastP;
	BRect bounds(Bounds()), r, b;
	uint32 buttons, cnt;
	bool multi;
	int mCol = colNr;
	CRunArray backup(fCellWidths);

	StPenState save(this);
	ClearAnts();

	cnt = fSelection.right - fSelection.left + 1;

	while (fCellWidths[colNr + 1] == fCellWidths[colNr])
		colNr++;

	multi = fSelection.left != fSelection.right
					&& colNr >= fSelection.left
					&& colNr <= fSelection.right;

	if (colNr <= fFrozen.h)
	{
		x = fBorderWidth + fCellWidths[colNr];
		minX = fBorderWidth + fCellWidths[multi?fSelection.left - 1 : colNr - 1];
	}
	else
	{
		x = fBorderWidth + fCellWidths[colNr] + fCellWidths[fFrozen.h] - 
			fCellWidths[fPosition.h - 1];
		minX = fBorderWidth + fCellWidths[fFrozen.h] - fCellWidths[fPosition.h - 1] + 
			fCellWidths[multi?fSelection.left - 1 : colNr - 1];
	}
	
	r.top = 1;
	r.bottom = fBorderHeight - 1;
	r.left = minX + 1;
	if (multi)
		r.right = minX + fCellWidths[fSelection.right] - 
			fCellWidths[fSelection.left - 1] - 1;
	else
		r.right = minX + fCellWidths[colNr] - fCellWidths[colNr - 1] - 1;
		
	b = bounds;
	b.left = r.right;
	
	maxRow = fPosition.v;
	while (fCellHeights[++maxRow] - fCellHeights[fPosition.v] < bounds.bottom - fBorderHeight)
		;
	
	lastP = where;
	newP = where;
	do
	{
		if (newP.x != lastP.x)
		{
			float dx;
			int k = colNr;
			
			dx = newP.x - lastP.x;
			
			if (lastP.x + dx < bounds.left + fBorderWidth)
				dx = std::min(lastP.x - bounds.left - fBorderWidth, (float)0);
			if (lastP.x + dx > bounds.right)
				dx = std::max(bounds.right - lastP.x, (float)0);
			
			if (multi && x + dx < minX)
				dx = minX - x;

			if (multi)
			{
				float w, t;
				
				w = x + dx - minX;
				if (w > 0)
					w = Round(w / (colNr - fSelection.left + 1));
				
				t = fCellWidths[fSelection.right];				
				fCellWidths.SetValue(fSelection.left, fSelection.right, w);
				dx = fCellWidths[fSelection.right] - t;
			}
			else if (x + dx < minX)
			{
				int t = k;
				
				fCellWidths.SetValue(colNr, 0);

				while (k > 1 && backup[k - 1] + fBorderWidth >= x + dx)
					k--;

				ASSERT(k<colNr);

				if (k <= mCol)
					while (mCol > k)
						fCellWidths.SetValue(mCol--, 0);
				else
				{
					t = mCol;
					for (;mCol<k;mCol++)
						fCellWidths.SetValue(mCol, backup[mCol] - backup[mCol - 1]);
				}

				float kw = x + dx - fBorderWidth;
				kw -= backup[k - 1];
				fCellWidths.SetValue(k, std::max(kw, (float)0));

				mCol = k;
				k = t;
			}
			else
			{
				k = mCol;
				for (; mCol < colNr; mCol++)
					fCellWidths.SetValue(mCol, backup[mCol] - backup[mCol - 1]);

				fCellWidths.SetValue(colNr, x + dx - minX);
			}
					
			if (dx != 0)
			{
				x += dx;
				lastP.x += dx;

				if (dx < 0)
					b.left += dx;
				
				StPenState save(this);
				StClipCells clip(this);
				ScrollRect(b, dx, 0);
				Window()->UpdateIfNeeded();
				
				if (dx > 0)
					b.left += dx;
				
				if (multi)
				{
					int h = fSelection.left, v;
					do
					{
						for (v = 1; v < fFrozen.v; v++)
							DrawCell(cell(h, v));
						for (v = fPosition.v; v < maxRow; v++)
							DrawCell(cell(h, v));
						DrawBorderCell(h++, true);
					}
					while (h <= fSelection.right && bounds.left + fCellWidths[h] + fBorderWidth < bounds.right);
				}
				else
				{
					int v;
					for (v = 1; v < fFrozen.v; v++)
						DrawCell(cell(mCol, v));
					for (v = fPosition.v; v < maxRow; v++)
						DrawCell(cell(mCol, v));
					DrawBorderCell(mCol, true);
				}
				
				for (; k < colNr; k++)
				{
					int v;
					for (v = 1; v < fFrozen.v; v++)
						DrawCell(cell(k, v));
					for (v = fPosition.v; v < maxRow; v++)
						DrawCell(cell(k, v));
					DrawBorderCell(k, true);
				}
			}
			
			if (dx < 0)
			{
				float X;
				int h;
				
				X = bounds.right + dx - fBorderWidth;
				h = fSelection.right;
				do h++;
				while (fCellWidths[h] < X);
				
				while (h <= kColCount && bounds.left + fCellWidths[h] + fBorderWidth < bounds.right)
				{
					int v;
					for (v = 1; v < fFrozen.v; v++)
						DrawCell(cell(h, v));
					for (v = fPosition.v; v < maxRow; v++)
						DrawCell(cell(h, v));
					DrawBorderCell(h++, true);
				}
			}
			
			if (dx && fFirstGraphic)
				fFirstGraphic->HandleUpdate(bounds);
		}
		GetMouse(&newP, &buttons);
	}
	while (buttons);

	if (!(backup == fCellWidths))
	{
		CRunArray temp(fCellWidths);
		fCellWidths = backup;
		
		((CCellWindow*)Window())->RegisterCommand(new CResizeCommand(this, temp, false));
		((CCellWindow*)Window())->SetDirty(true);
	}
} /* ResizeCol */
