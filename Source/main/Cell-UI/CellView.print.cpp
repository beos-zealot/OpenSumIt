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
	CellView.print.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   CONFIG_H
#include "Config.h"
#endif

#include <Message.h>
#include <PrintJob.h>


static int Next(CSet& set, int cur)
{
	do cur++;
	while (!set[cur] && cur < set.Size());
	return cur;
}

void CCellView::Repaginate()
{
	int h, v;
	range r;
	float x;

	fHPageCount = fVPageCount = 0;
	
	if (!fMargins.IsValid())
		return;
	
	fContainer->Lock();
	fContainer->GetBounds(r);
	fContainer->Unlock();

	x = fPrBorders ? -fBorderWidth : 0;
	
	fHPageBreaks.Clear();
	for (h = 1; h <= kColCount; h++)
	{
		if (fCellWidths[h] - x > fMargins.Width())
		{
			x = fCellWidths[h - 1];
			if (fPrBorders)
				x -= fBorderWidth;

			fHPageBreaks += h - 1;
			fHPageCount++;
			
			if (h > r.right)
				break;
		}
	}

	x = (fPrBorders ? -fBorderHeight : 0);
	fVPageBreaks.Clear();
	for (v = 1; v <= kRowCount; v++)
	{
		if (fCellHeights[v] - x > fMargins.Height())
		{
			x = fCellHeights[v - 1];
			if (fPrBorders)
				x -= fBorderHeight;

			fVPageBreaks += v - 1;
			fVPageCount++;
			
			if (v > r.bottom)
				break;
		}
	}
} /* CCellView::Repaginate */

void CCellView::DoPageSetup()
{
	BPrintJob prJob("a page setup job");
	
	if (fPageSetup)
	{
		BMessage *s = new BMessage;
		if (s && s->Unflatten((char *)fPageSetup) == B_NO_ERROR)
			prJob.SetSettings(s);
	}
	
	int result = prJob.ConfigPage();
	
	if (result == B_NO_ERROR)
	{
//		MV	What is it supposed to do?
//		prJob.CommitJob();
	
		fPageSetupSize = prJob.Settings()->FlattenedSize();
		if (fPageSetup) FREE(fPageSetup);
		fPageSetup = (char *)MALLOC(fPageSetupSize);
		FailNil(fPageSetup);
		result = prJob.Settings()->Flatten((char *)fPageSetup, fPageSetupSize);
		FailOSErr(result, "error flattening %d");
	
		fPaperRect = prJob.PaperRect();
		fMargins = prJob.PrintableRect();

		Repaginate();
	}
} /* CCellView::DoPageSetup */

void CCellView::DoPrint()
{
	if (!fPageSetup)
		DoPageSetup();
	
	BPrintJob prJob("a page setup job");
	BMessage *s = new BMessage;
	if (s && s->Unflatten((char *)fPageSetup) == B_NO_ERROR)
		prJob.SetSettings(s);

	int result = prJob.ConfigJob();
	
	if (result == B_NO_ERROR)
	{
//		MV	What is it supposed to do?
//		prJob.CommitJob();
		
		prJob.BeginJob();
		
		int first = prJob.FirstPage();
		int last = prJob.LastPage();
		int i;
		
		if (first < 1) first = 1;
		if (last > fHPageCount * fVPageCount) last = fHPageCount * fVPageCount;
		
		for (i = first; i <= last; i++)
		{
			PrintPage(prJob, i);
			prJob.SpoolPage();
		}

		prJob.CommitJob();
	}
} /* CCellView::DoPrint */

void CCellView::PrintPage(BPrintJob& prJob, int pageNr)
{
	BRect paper, cellBounds = fCellBounds;
	range bounds;
	cell oldPosition;
	int hPageIndx, vPageIndx, hCells, vCells, p;
	
	bool showedGrid = fShowGrid;
	bool showedBorders = fShowBorders;
	fShowGrid = fPrGrid;
	fShowBorders = fPrBorders;
	
	oldPosition = fPosition;
	
	fContainer->Lock();
	fContainer->GetBounds(bounds);
	fContainer->Unlock();

	hPageIndx = (pageNr - 1) % fHPageCount;
	vPageIndx = (pageNr - 1) / fHPageCount;

	p = hPageIndx;
	fPosition.h = 0;
	while (p--)
		fPosition.h = Next(fHPageBreaks, fPosition.h);
	fPosition.h++; 

	hCells = min(Next(fHPageBreaks, fPosition.h), (int)bounds.right) - fPosition.h;

	p = vPageIndx;
	fPosition.v = 0;
	while (p--)
		fPosition.v = Next(fVPageBreaks, fPosition.v);
	fPosition.v++;
		
	vCells = min(Next(fVPageBreaks, fPosition.v), (int)bounds.bottom) - fPosition.v;

	paper.Set(0, 0, fCellWidths[fPosition.h + hCells] - fCellWidths[fPosition.h - 1],
		fCellHeights[fPosition.v + vCells] - fCellHeights[fPosition.v - 1]);
	
	if (fShowBorders)
	{
		paper.right += fBorderWidth;
		paper.bottom += fBorderHeight;
	}
	
	prJob.DrawView(this, paper, BPoint(0, 0));

	fPosition = oldPosition;

	fShowGrid = showedGrid;
	fShowBorders = showedBorders;
	fCellBounds = cellBounds;
} /* CCellView::PrintPage */

int CCellView::GetPageNrForCell(cell c)
{
	int result = 0, i;
	
	i = 0;
	
	do
	{
		i = Next(fHPageBreaks, i);
		result++;
	}
	while (i < c.h);

	i = 0;
	
	do
	{
		i = Next(fVPageBreaks, i);
		result += fHPageCount;
	}
	while (i < c.v);
	
	return result - 2;
} /* CCellView::GetPageNrForCell */
