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
	DragCommand.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLITERATOR_H
#include "CellIterator.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   DRAGCOMMAND_H
#include "DragCommand.h"
#endif

#ifndef   UNDOSTRS_H
#include "undostrs.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   FORMULA_H
#include "Formula.h"
#endif

#ifndef   STLOCKER_H
#include "StLocker.h"
#endif


CDragCommand::CDragCommand(CCellView *destView, CContainer *srcContainer,
	CContainer *destContainer, range *srcRange, range *destRange, int action)
	: CCommand(kDragStrID)
{
	fDestView = destView;
	fSrcContainer = srcContainer;
	fSrcContainer->Reference();
	fDstContainer = destContainer;
	fSrcRange = *srcRange;
	fDstRange = *destRange;
	fAction = action;
	fDestView->GetSelection(fSavedSelection);
	fSavedFormulas = NULL;
	
	fSrcSavedCells = new CContainer(NULL);
	fDstSavedCells = new CContainer(NULL);

	if (srcContainer != destContainer && action == dragLink)
		THROW((errCanOnlyCopy));
} 

CDragCommand::~CDragCommand()
{
	if (fSavedFormulas)
	{
		std::map<cell,void*>::iterator i;
		
		for (i = fSavedFormulas->begin(); i != fSavedFormulas->end(); i++)
			free((*i).second);
		
		delete fSavedFormulas;
	}
	fSrcSavedCells->Release();
	fDstSavedCells->Release();
	fSrcContainer->Release();
}

void CDragCommand::DoCommand()
{
	cell c, newLoc, offset;
	CCellIterator sIter(fSrcContainer, &fSrcRange),
				  dIter(fDstContainer, &fDstRange),
				  svdIter(fSrcSavedCells, &fSrcRange);

	StLocker<CContainer> sLock(fSrcSavedCells);
	StLocker<CContainer> dLock(fDstSavedCells);
	
	while (sIter.NextExisting(c))
	{
		StLocker<CContainer> sLock(fSrcContainer);
		if (fAction == dragMove)
		{
			fSrcContainer->MoveCell(fSrcSavedCells, c, c);
			fDestView->TouchLine(c.v);
		}
		else
			fSrcContainer->CopyCell(fSrcSavedCells, c, c);
	}
	
	while (dIter.NextExisting(c))
	{
		StLocker<CContainer> dLock(fDstContainer);
		fDstContainer->MoveCell(fDstSavedCells, c, c);
		fDestView->TouchLine(c.v);
	}
	
	offset.v = fDstRange.top - fSrcRange.top;
	offset.h = fDstRange.left - fSrcRange.left;
	
	if (fAction == dragMove)
	{
		fSavedFormulas = new std::map<cell,void*>;
		CCellIterator next(fSrcContainer);
		cell c;
		
		while (next.NextExisting(c))
		{
			CFormula form(fSrcContainer->GetCellFormula(c));
			if (form.IsFormula())
			{
				void *p = form.UpdateReferences(c, fSrcRange, fDstRange);
				if (p)
					(*fSavedFormulas)[c] = p;
			}
		}
		
		fSavedNames = new namemap;
		
		CNameTable *tbl = fSrcContainer->GetNameTable();
		
		if (tbl)
		{
			namemap::iterator i;

			for (i = tbl->begin(); i != tbl->end(); i++)
			{
				if ((*i).second == fSrcRange)
				{
					(*fSavedNames)[(*i).first] = (*i).second;
					(*i).second = fDstRange;
				}
			}
		}
	}
	
	while (svdIter.NextExisting(c))
	{
		StLocker<CContainer> dLock(fDstContainer);
		newLoc.v = c.v + offset.v;
		newLoc.h = c.h + offset.h;
		fSrcSavedCells->CopyCell(fDstContainer, c, newLoc,
			&fSrcRange, fAction == dragMove);
		if (fAction == dragLink)
		{
			CFormula form;
			int ix = 0;

			cell o = c.GetRefCell(newLoc);
			
			form.AddToken(valCell, &o, ix);
			form.AddToken(opEnd, NULL, ix);
			fDstContainer->SetCellFormula(newLoc, form.DetachString());
		}
		fDestView->TouchLine(newLoc.v);
	}
	
	fDestView->SetSelection(fDstRange);
	fDestView->CalculateAllCells();
	fDestView->DrawAllLines();
	fDestView->DrawStatus();
	
	return ;
}

void CDragCommand::UndoCommand()
{
	cell c;
	CCellIterator sIter(fSrcSavedCells, &fSrcRange),
				  dIter(fDstContainer, &fDstRange);	
	
	StLocker<CContainer> sLock(fSrcSavedCells);
	StLocker<CContainer> dLock(fDstSavedCells);

	while (dIter.Next(c))
	{
		StLocker<CContainer> dLock(fDstContainer);
		fDstContainer->DisposeCell(c);
		fDstSavedCells->MoveCell(fDstContainer, c, c);
		fDestView->TouchLine(c.v);
	}

	while (sIter.NextExisting(c))
	{
		StLocker<CContainer> sLock(fSrcContainer);
		fSrcSavedCells->MoveCell(fSrcContainer, c, c);
		fDestView->TouchLine(c.v);
	}

	if (fSavedFormulas)
	{
		std::map<cell,void*>::iterator i;
		for (i = fSavedFormulas->begin(); i != fSavedFormulas->end(); i++)
			fSrcContainer->SetCellFormula((*i).first, (*i).second);
		delete fSavedFormulas;
		fSavedFormulas = NULL;
		
		CNameTable *tbl = fSrcContainer->GetNameTable();
		
		if (tbl)
		{
			namemap::iterator n;
			
			for (n = fSavedNames->begin(); n != fSavedNames->end(); n++)
				(*tbl)[(*n).first] = (*n).second;
		}
	}

	fDestView->SetSelection(fSavedSelection);
	fDestView->CalculateAllCells();
	fDestView->DrawAllLines();
	fDestView->DrawBorders();
	fDestView->DrawStatus();
}
