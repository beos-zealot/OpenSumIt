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
	CopyCommands.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLITERATOR_H
#include "CellIterator.h"
#endif

#ifndef   CELLUTILS_H
#include "CellUtils.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   CELLCOMMANDS_H
#include "CellCommands.h"
#endif

//#ifndef   DIALOGWINDOW_H
//#include "DialogWindow.h"
//#endif

#ifndef   COPYCOMMANDS_H
#include "CopyCommands.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   PROGRESSVIEW_H
#include "ProgressView.h"
#endif

#ifndef   CELLPARSER_H
#include "CellParser.h"
#endif

#ifndef   UNDOSTRS_H
#include "undostrs.h"
#endif

#ifndef   STLOCKER_H
#include "StLocker.h"
#endif

#ifndef   MYCLIPBOARD_H
#include "MyClipboard.h"
#endif

#include <Autolock.h>
#include <Picture.h>


CCutCommand::CCutCommand(CCellView *inView, CContainer *inCells)
	: CCellCommand (kCutStrID, inView, inCells)
{
	inView->GetSelection(fAffected);
	fSavedCells = new CContainer(NULL);
} /* CCutCommand:: */

CCutCommand::~CCutCommand()
{
	fSavedCells->Release();
} /* ~CCutCommand */
	
void CCutCommand::DoCommand()
{
	CContainer *theCopy = new CContainer(NULL);
	FailNil(theCopy);
	cell c;	
	CCellIterator iter(fSourceContainer, &fAffected);

	StLocker<CContainer> slock(fSavedCells);
	
	BPicture *pic = new BPicture;
	BRect r;
	fCellView->GetPictureOfSelection(pic, r);

	while (iter.NextExisting(c))
	{
		StLocker<CContainer> lock(fSourceContainer);
		fSourceContainer->CopyCell(fSavedCells, c, c);
		fSourceContainer->MoveCell(theCopy, c, c);
		fCellView->TouchLine(c.v);
	}

	UpdateView();

	gClipboard->PutCells(theCopy, &fAffected,
		pic, r, fCellView->Window()->Title());
} /* DoCommand */

void CCutCommand::UndoCommand()
{
	cell c;
	CCellIterator iter(fSavedCells, &fAffected);	

	StLocker<CContainer> slock(fSavedCells);
	while (iter.NextExisting(c))
	{
		StLocker<CContainer> lock(fSourceContainer);
		fSavedCells->CopyCell(fSourceContainer, c, c);
		fCellView->TouchLine(c.v);
	}
	
	fCellView->SetSelection(fAffected);
	UpdateView();
} /* UndoCommand */

void CCutCommand::RedoCommand()
{
	cell c;	
	CCellIterator iter(fSourceContainer, &fAffected);

	while (iter.NextExisting(c))
	{
		StLocker<CContainer> lock(fSourceContainer);
		fSourceContainer->DisposeCell(c);
		fCellView->TouchLine(c.v);
	}
	
	fCellView->SetSelection(fAffected);
	UpdateView();
} /* RedoCommand */


CCopyCommand::CCopyCommand(CCellView *inView, CContainer *inCells) 
	: CCellCommand (0, inView, inCells, false)
{
	inView->GetSelection(fAffected);

//	if (fAffected.left == 1 && fAffected.top == 1 &&
//		fAffected.right == kColCount && fAffected.bottom == kRowCount)
//		inView->GetContainer()->GetBounds(fAffected);
} /* CCopyCommand:: */

void CCopyCommand::DoCommand()
{
	CContainer *theCopy = new CContainer(NULL);
	FailNil(theCopy);
	cell c;
	
	StProgress progress(fCellView,
		fSourceContainer->CountCells(&fAffected), false);
	
	BPicture *pic = new BPicture;
	BRect r;
	fCellView->GetPictureOfSelection(pic, r);

	CCellIterator iter(fSourceContainer, &fAffected);	

	StLocker<CContainer> clock(theCopy);
	StLocker<CContainer> lock(fSourceContainer);
	while (iter.NextExisting(c))
	{
		fSourceContainer->CopyCell(theCopy, c, c);
		progress.Step();
	}

	gClipboard->PutCells(theCopy, &fAffected,
		pic, r, fCellView->Window()->Title());
} /* DoCommand */

CClearCommand::CClearCommand(CCellView *inView, CContainer *inCells)
	: CCellCommand (kClearStrID, inView, inCells)
{
	inView->GetSelection(fAffected);
	fSavedCells = new CContainer(NULL);
	FailNil(fSavedCells);
} /* CClearCommand:: */

CClearCommand::~CClearCommand()
{
	fSavedCells->Release();
} /* ~CClearCommand */
	
void CClearCommand::DoCommand()
{
	cell c;

	StProgress progress(fCellView,
		fSourceContainer->CountCells(&fAffected), false);
	BAutolock slock(fSavedCells);
	
	CCellIterator iter(fSourceContainer, &fAffected);	

	while (iter.NextExisting(c))
	{
		StLocker<CContainer> lock(fSourceContainer);
		fSourceContainer->CopyCell(fSavedCells, c, c);
		fSourceContainer->DisposeCell(c);
		fCellView->TouchLine(c.v);
		progress.Step();
	}

	UpdateView();
} /* DoCommand */

void CClearCommand::UndoCommand()
{
	cell c;

	StProgress progress(fCellView,
		fSavedCells->CountCells(&fAffected), false);
	
	StLocker<CContainer> slock(fSavedCells);
	CCellIterator iter(fSavedCells, &fAffected);	

	while (iter.NextExisting(c))
	{
		StLocker<CContainer> lock(fSourceContainer);
		fSavedCells->CopyCell(fSourceContainer, c, c);
		fCellView->TouchLine(c.v);
		progress.Step();
	}
	
	fCellView->SetSelection(fAffected);
	UpdateView();
} /* UndoCommand */

void CClearCommand::RedoCommand()
{
	cell c;	

	StProgress progress(fCellView,
		fSourceContainer->CountCells(&fAffected), false);
	
	CCellIterator iter(fSourceContainer, &fAffected);

	while (iter.NextExisting(c))
	{
		StLocker<CContainer> lock(fSourceContainer);
		fSourceContainer->DisposeCell(c);
		fCellView->TouchLine(c.v);
		progress.Step();
	}
	
	fCellView->SetSelection(fAffected);
	UpdateView();
} /* RedoCommand */

CPasteCommand::CPasteCommand(CCellView *inView, CContainer *inCells,
	bool inPasteFormat, bool inPasteContent, bool inFillSelection,
	bool inTranspose, bool inOriginalRefs, PasteEnums inPasteWhat,
	PasteEnums inPasteHow)
	: CCellCommand (kPasteStrID, inView, inCells)
{
	inView->GetSelection(fAffected);
	fSavedCells = new CContainer(NULL);
	FailNil(fSavedCells);

	fPasteFormat = inPasteFormat;
	fPasteContent = inPasteContent;
	fFillSelection = inFillSelection;
	fTranspose = inTranspose;
	fOriginalRefs = inOriginalRefs;
	fPasteWhat = inPasteWhat;
	fPasteHow = inPasteHow;

	gClipboard->GetCells(&fClipboard, &fClipRect);
	FailNil(fClipboard);
	fClipboard->Reference();
} /* CPasteCommand:: */

CPasteCommand::~CPasteCommand()
{
	if (fSavedCells) fSavedCells->Release();
	if (fClipboard) fClipboard->Release();
} /* ~CPasteCommand */
	
void CPasteCommand::DoCommand()
{
	CContainer *sContainer, *tContainer, *dContainer;
	int tmp, dw, dh, sw, sh, x, y;
	cell c, offset, newLoc;
	range srcR, destR;
	char dStr[256], s[256];
	int cellStyle;

	StLocker<CContainer> lock(fSavedCells);

	sContainer = fClipboard;
	srcR = fClipRect;
	
	tContainer = new CContainer (NULL);
	tContainer->Lock();
	FailNil(tContainer);

	dContainer = fSourceContainer;
	StLocker<CContainer> dlock(dContainer);
	StLocker<CContainer> slock(sContainer);
	if (fFillSelection)
	{
		destR = srcR;
		destR.OffsetBy(fAffected.left - destR.left, fAffected.top - destR.top);
		destR.right = destR.left + fAffected.right - fAffected.left;
		destR.bottom = destR.top + fAffected.bottom - fAffected.top;
	}
	else
	{
		destR = srcR;
		destR.OffsetBy(fAffected.left - destR.left, fAffected.top - destR.top);
	}

	if (fTranspose)
	{
		tmp = destR.right - destR.left;
		destR.right = destR.left + (destR.bottom - destR.top);
		destR.bottom = destR.top + tmp;
	}

	if (destR.right > kColCount)
		destR.right = kColCount;
	if (destR.bottom > kRowCount)
		destR.bottom = kRowCount;

	dw = destR.right - destR.left + 1;
	dh = destR.bottom - destR.top + 1;
	sw = srcR.right - srcR.left + 1;
	sh = srcR.bottom - srcR.top + 1;

	fDestination = destR;

	CCellIterator dIter(dContainer, &destR);
	while (dIter.NextExisting(c))
		fSourceContainer->CopyCell(fSavedCells, c, c);

	offset.v = destR.top - srcR.top;
	offset.h = destR.left - srcR.left;

	dIter.Reset();
	if (fPasteContent && fPasteHow == pOverwrite)
		while (dIter.NextExisting(c))
			dContainer->DisposeCell(c);
	
	StProgress progress(fCellView,
		2 * (destR.right - destR.left + 1) * (destR.bottom - destR.top + 1),
		false);
	
	for (y = destR.top; y <= destR.bottom; y++)
		for (x = destR.left; x <= destR.right; x++)
		{
			if (fTranspose)
			{
				c.v = (x - destR.left) % sh + srcR.top;
				c.h = (y - destR.top) % sw + srcR.left;
			}
			else
			{
				c.v = (y - destR.top) % sh + srcR.top;
				c.h = (x - destR.left) % sw + srcR.left;
			}
	
			newLoc.v = y;
			newLoc.h = x;
	
			if (fPasteWhat == pFormulas && fOriginalRefs)
			{
				sContainer->GetCellFormula(c, s);
				TryToParseString(s, newLoc, tContainer, false);
				if (fPasteFormat)
				{
					cellStyle = sContainer->GetCellStyleNr(c);
					tContainer->SetCellStyleNr(newLoc, cellStyle);
				}
			}
			else if (fPasteWhat != pReferences) {
				sContainer->CopyCell(tContainer, c, newLoc);
			}
			else
			{
				c.GetName(s);
				TryToParseString(s, newLoc, tContainer, false);
			}
	
			progress.Step();
		}
	
	dIter.Reset();
	while (dIter.Next(c))
	{
		if (fPasteFormat)
			cellStyle = tContainer->GetCellStyleNr(c);
		else
			cellStyle = fSavedCells->GetCellStyleNr(c);
		
		if (fPasteContent &&
			fPasteHow == pOverwrite &&
			fPasteWhat == pFormulas) {
			tContainer->CopyCell(dContainer, c, c);
		}
		else if (fPasteContent)
		{
			if (fPasteWhat == pFormulas || fPasteWhat == pReferences) {
				tContainer->GetCellFormula(c, dStr);
			}
			else if (fPasteWhat == pValues) {
				tContainer->GetCellResult(c, dStr, true);
			}
			if (fPasteHow != pOverwrite)
			{
				dContainer->GetCellFormula(c, s);

				if (*s)
				{
					memmove(s + 2, s, strlen(s) + 1);
					s[0] = '=';
					s[1] = '(';
					
					switch(fPasteHow)
					{
						case pAdd:
							strcat(s, ")+(");
							break;
						case pSubtract:
							strcat(s, ")-(");
							break;
						case pMultiply:
							strcat(s, ")*(");
							break;
						case pDivide:
							strcat(s, ")/(");
							break;
							
						default:
							// there was a warning about not all enum values
							// handled in switch statement.
							break;
					}
					strcat(s, dStr);
					strcat(s, ")");
					strcpy(dStr, s);
				}
			}

			TryToParseString(dStr, c, dContainer, false);
		}

		dContainer->SetCellStyleNr(c, cellStyle);
		
		fCellView->TouchLine(c.v);
		progress.Step();
	}

	fCellView->SetSelection(destR);
	UpdateView();

	tContainer->Release();
} /* DoCommand */

void CPasteCommand::UndoCommand()
{
	cell c;

	StLocker<CContainer> slock(fSavedCells);
	CCellIterator iter(fSavedCells, &fDestination);
	
	StProgress progress(fCellView,
		(fDestination.right - fDestination.left + 1) *
		(fDestination.bottom - fDestination.top + 1),
		false);
	
	while (iter.Next(c))
	{
		StLocker<CContainer> lock(fSourceContainer);
		fSourceContainer->DisposeCell(c);
		fSavedCells->CopyCell(fSourceContainer, c, c);
		fCellView->TouchLine(c.v);
		progress.Step();
	}
	
	UpdateView();
} /* UndoCommand */

void CPasteCommand::RedoCommand()
{
	DoCommand();
} /* RedoCommand */
