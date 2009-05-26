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
	CellView.ants.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "CellWindow.h"
#include "CellView.h"
#include "Utils.h"
#include "DrawUtils.h"
#include "Globals.h"

const double kAntsDelay = 90000.0;

const pattern
	gStripes[16] = {
		{{ 0xC1, 0x83, 0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0 }}, 
		{{ 0x83, 0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xC1 }},
		{{ 0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xC1, 0x83 }}, 
		{{ 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xC1, 0x83, 0x07 }}, 
		{{ 0x1C, 0x38, 0x70, 0xE0, 0xC1, 0x83, 0x07, 0x0E }}, 
		{{ 0x38, 0x70, 0xE0, 0xC1, 0x83, 0x07, 0x0E, 0x1C }}, 
		{{ 0x70, 0xE0, 0xC1, 0x83, 0x07, 0x0E, 0x1C, 0x38 }}, 
		{{ 0xE0, 0xC1, 0x83, 0x07, 0x0E, 0x1C, 0x38, 0x70 }}, 
		
		{{ 0xE3, 0xC7, 0x8F, 0x1F, 0x3E, 0x7C, 0xF8, 0xF1 }}, 
		{{ 0xC7, 0x8F, 0x1F, 0x3E, 0x7C, 0xF8, 0xF1, 0xE3 }}, 
		{{ 0x8F, 0x1F, 0x3E, 0x7C, 0xF8, 0xF1, 0xE3, 0xC7 }}, 
		{{ 0x1F, 0x3E, 0x7C, 0xF8, 0xF1, 0xE3, 0xC7, 0x8F }}, 
		{{ 0x3E, 0x7C, 0xF8, 0xF1, 0xE3, 0xC7, 0x8F, 0x1F }}, 
		{{ 0x7C, 0xF8, 0xF1, 0xE3, 0xC7, 0x8F, 0x1F, 0x3E }}, 
		{{ 0xF8, 0xF1, 0xE3, 0xC7, 0x8F, 0x1F, 0x3E, 0x7C }}, 
		{{ 0xF1, 0xE3, 0xC7, 0x8F, 0x1F, 0x3E, 0x7C, 0xF8 }}
	};

double	CCellView::fgAntsTime = 0.0;
int		CCellView::fgAntsPat = 0;

void CCellView::Pulse()
{
	if ((fWindowActive && !fEntering && IsFocus() && !GraphicHasFocus()) ||
		fDragIsAcceptable)
		MarchAnts();
} /* CCellView::Pulse */

void CCellView::MarchAnts()
{
	BRect r, r2;
	int a = 0;
	
	StPenState save(this);

	if (system_time() < fgAntsTime)
		return;

	fgAntsTime = system_time() + kAntsDelay;

	if (++fgAntsPat >= 8)
		fgAntsPat = 0;
	
	if (fEntering)
	{
		GetCellRect(fSelection.TopLeft(), r);
		GetCellRect(fSelection.BotRight(), r2);
		
		r = r | r2;
		r.InsetBy(-1.0, -1.0);
		a = 8;
	}
	else
	{
		if ((fCurCell.v < fPosition.v &&
			fCurCell.v > fFrozen.v) ||
			(fCurCell.h < fPosition.h &&
			fCurCell.h > fFrozen.h))
			return;
		
		GetCellRect(fCurCell, r);
	}
	
	BRect b = fCellBounds;
	b.InsetBy(-1, -1);
	r = b & r;
	
	if (r.Width() > 0 && r.Height() > 0)
	{
		StClipCells cc(this);
		SetHighColor(keyboard_navigation_color());
		StrokeLine(r.LeftTop(), r.RightTop(), gStripes[a + fgAntsPat]);
		StrokeLine(r.RightTop(), r.RightBottom(), gStripes[a + 7 - fgAntsPat]);
		StrokeLine(r.LeftBottom(), r.RightBottom(), gStripes[a + 7 - fgAntsPat]);
		StrokeLine(r.LeftTop(), r.LeftBottom(), gStripes[a + fgAntsPat]);
//		StrokeLine(r.LeftTop(), r.RightTop(), gStripes[a + 7 - fgAntsPat]);
//		StrokeLine(r.RightTop(), r.RightBottom(), gStripes[a + 7 - fgAntsPat]);
//		StrokeLine(r.LeftBottom(), r.RightBottom(), gStripes[a + fgAntsPat]);
//		StrokeLine(r.LeftTop(), r.LeftBottom(), gStripes[a + fgAntsPat]);
		SetHighColor(kBlack);
	}
} /* CCellView::MarchAnts */

void CCellView::ClearAnts()
{
	BRect r1, r2;
	cell c = fCurCell;

	StPenState save(this);
	
	if (!fEntering)
	{
		if ((fCurCell.v < fPosition.v && fCurCell.v > fFrozen.v) ||
			(fCurCell.h < fPosition.h && fCurCell.h > fFrozen.h))
			return;

		DrawCell(c);
	}
	else
	{
		StClipCells clip(this);
	
		GetCellRect(fSelection.TopLeft(), r1);
		GetCellRect(fSelection.BotRight(), r2);

		BRect r = r1 | r2;
		r.InsetBy(-1.0, -1.0);

		BRect b = fCellBounds;
		b.InsetBy(-1, -1);
		r = b & r;

		BeginLineArray(4);
		AddLine(r.LeftTop(), r.RightTop(), 
			kB_GrayTable[fSelection.top == fPosition.v ? 13 : 6]);
		AddLine(r.LeftTop(), r.LeftBottom(), 
			kB_GrayTable[fSelection.left == fPosition.h ? 13 : 6]);
		AddLine(r.LeftBottom(), r.RightBottom(), kB_GrayTable[6]);
		AddLine(r.RightTop(), r.RightBottom(), kB_GrayTable[6]);
		EndLineArray();
	}
} /* CCellView::ClearAnts */
