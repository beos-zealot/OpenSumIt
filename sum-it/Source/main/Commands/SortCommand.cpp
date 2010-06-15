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
	SortCommand.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/


#include "Container.h"
#include "CellView.h"
#include "Command.h"
#include "CellCommands.h"
#include "CellWindow.h"
#include "SortCommand.h"
#include "MyError.h"
#include "undostrs.h"
#include "StLocker.h"

CSortCommand::CSortCommand(CCellView *inView, CContainer *inCells,
	int inKey1, int inKey2, int inKey3,
	bool inAsc1, bool inAsc2, bool inAsc3,
	bool inByRow)
	: CCellCommand(kSortStrID, inView, inCells)
{
	fOrder = NULL;
	
	inView->GetSelection(fSortRange);
	
	fAsc1 = inAsc1 ? 1 : -1;
	fAsc2 = inAsc2 ? 1 : -1;
	fAsc3 = inAsc3 ? 1 : -1;
	
	fKey1 = inKey1;
	fKey2 = inKey2;
	fKey3 = inKey3;
	
	fByRow = inByRow;
} /* CSortCommand */

CSortCommand::~CSortCommand()
{
	if (fOrder) delete[] fOrder;
} /* ~CSortCommand */
					
void CSortCommand::DoCommand()
{
	int count, i;
	
	if (fByRow)
		count = fSortRange.bottom - fSortRange.top + 1;
	else
		count = fSortRange.right - fSortRange.left + 1;
	
	fOrder = new int[count];
	FailNil(fOrder);
	
	for (i = 0; i < count; i++)
		fOrder[i] = i;
	
	QuickSort(0, count - 1);
	
	UpdateView();
} /* DoCommand */

void CSortCommand::QuickSort(int l, int r)
{
	int i, j;
	int t;
	
	if (r > l)
	{
		i = l - 1; j = r;
		for (;;)
		{
			while (CompareRows(++i, r) < 0 && i < r) ;
			while (CompareRows(--j, r) > 0 && j > l) ;
			if (i >= j) break;
			t = fOrder[i]; fOrder[i] = fOrder[j]; fOrder[j] = t;
			SwapRow(i, j);
		}
		if (i != r)
		{
			t = fOrder[i]; fOrder[i] = fOrder[r]; fOrder[r] = t;
			SwapRow(i, r);
		}
		QuickSort(l, i - 1);
		QuickSort(i + 1, r);
	}
} /* QuickSort */

int CSortCommand::CompareRows(int l, int r)
{
	cell cl, cr;
	int result = 0;
	
	StLocker<CContainer> lock(fSourceContainer);
	if (fByRow)
	{
		if (fKey1)
		{
			cl.h = cr.h = fKey1;
			cl.v = fSortRange.top + l;
			cr.v = fSortRange.top + r;
			result = fAsc1 * fSourceContainer->CompareValue(cl, cr);
		}
		
		if (!result && fKey2)
		{
			cl.h = cr.h = fKey2;
			cl.v = fSortRange.top + l;
			cr.v = fSortRange.top + r;
			result = fAsc2 * fSourceContainer->CompareValue(cl, cr);
		}

		if (!result && fKey3)
		{
			cl.h = cr.h = fKey3;
			cl.v = fSortRange.top + l;
			cr.v = fSortRange.top + r;
			result = fAsc3 * fSourceContainer->CompareValue(cl, cr);
		}
	}
	else
	{
		if (fKey1)
		{
			cl.v = cr.v = fKey1;
			cl.h = fSortRange.left + l;
			cr.h = fSortRange.left + r;
			result = fAsc1 * fSourceContainer->CompareValue(cl, cr);
		}
		
		if (!result && fKey2)
		{
			cl.v = cr.v = fKey2;
			cl.h = fSortRange.left + l;
			cr.h = fSortRange.left + r;
			result = fAsc2 * fSourceContainer->CompareValue(cl, cr);
		}

		if (!result && fKey3)
		{
			cl.v = cr.v = fKey3;
			cl.h = fSortRange.left + l;
			cr.h = fSortRange.left + r;
			result = fAsc3 * fSourceContainer->CompareValue(cl, cr);
		}
	}
	
	return result;
} /* CompareRows */

void CSortCommand::SwapRow(int t, int b)
{
	cell s, d;
	int i;
	
	StLocker<CContainer> lock(fSourceContainer);
	if (fByRow)
	{
		s.v = t + fSortRange.top; d.v = b + fSortRange.top;
		for (i = fSortRange.left; i <= fSortRange.right; i++)
		{
			s.h = d.h = i;
			fSourceContainer->ExchangeCells(s, d);
		}
		fCellView->TouchLine(s.v);
		fCellView->TouchLine(d.v);
	}
	else
	{
		s.h = t + fSortRange.left; d.h = b + fSortRange.left;
		for (i = fSortRange.top; i <= fSortRange.bottom; i++)
		{
			s.v = d.v = i;
			fSourceContainer->ExchangeCells(s, d);
			fCellView->TouchLine(i);
		}
	}
} /* SwapRow */

void CSortCommand::UndoCommand()
{
	int i, j, t, count;
	
	if (fByRow)
		count = fSortRange.bottom - fSortRange.top + 1;
	else
		count = fSortRange.right - fSortRange.left + 1;
	
	for (i = 0; i < count; i++)
		if (fOrder[i] != i)
			for (j = i + 1; j < count; j++)
				if (fOrder[j] == i)
				{
					t = fOrder[i]; fOrder[i] = fOrder[j]; fOrder[j] = t;
					SwapRow(i, j);
					break;
				}
	
	delete[] fOrder;
	fOrder = NULL;
	
	UpdateView();
} /* UndoCommand */
