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
	FindCommands
	
	Copyright 1997, Hekkelman Programmatuur
*/

#include "CellCommands.h"
#include "CellView.h"
#include "Container.h"
#include "CellParser.h"
#include "FindCommands.h"
#include "undostrs.h"
#include "Utils.h"

CReplaceCommand::CReplaceCommand(CCellView *inView, CContainer *inContainer,
	const char *what, int at, const char *with, bool findNext, bool wrap)
	: CCellCommand(kReplaceID, inView, inContainer)
{
	fFindNext = findNext;
	fFormula = NULL;
	fWhat = STRDUP(what);
	fWith = STRDUP(with);
	fWrap = wrap;
	fRCell = inView->CurCell();
	fAt = at;
} /* CReplaceCommand::CReplaceCommand */
	
CReplaceCommand::~CReplaceCommand()
{
	if (fFormula) FREE(fFormula);
	if (fWhat) FREE(fWhat);
	if (fWith) FREE(fWith);
} /* CReplaceCommand::~CReplaceCommand */

void CReplaceCommand::DoCommand()
{
	char f[512], *o;
	fSourceContainer->GetCellFormula(fRCell, f);
	fFormula = STRDUP(f);
	
	int l1, l2;
	
	l1 = strlen(fWhat);
	l2 = strlen(fWith);
	o = f + fAt;
	
	memmove(o + l2, o + l1, strlen(o + l1) + 1);
	strncpy(o, fWith, l2);
	
	fCellView->ResetOffset(fAt + l2);

	if (!TryToParseString(f, fRCell, fSourceContainer, true))
		THROW((B_NO_ERROR));

	if (fFindNext)
		fCellView->FindNext(fWrap);

	fCellView->TouchLine(fRCell.v);
	UpdateView();
	fCellView->DrawStatus();
} /* CReplaceCommand::DoCommand */

void CReplaceCommand::UndoCommand()
{
	if (!TryToParseString(fFormula, fRCell, fSourceContainer, true))
		THROW((B_NO_ERROR));

	fCellView->SetSelection(fRCell);
	fCellView->TouchLine(fRCell.v);
	UpdateView();
	fCellView->DrawStatus();
} /* CReplaceCommand::UndoCommand */

CReplaceAllCommand::CReplaceAllCommand(CCellView *inView, CContainer *inContainer,
		const char *what, const char *with)
	: CCellCommand(kReplaceAllID, inView, inContainer)
{
	fWhat = STRDUP(what);
	fWith = STRDUP(with);
} /* CReplaceAllCommand::CReplaceAllCommand */

CReplaceAllCommand::~CReplaceAllCommand()
{
	if (fWhat) FREE(fWhat);
	if (fWith) FREE(fWith);

	std::map<cell,char*>::iterator i;
	
	for (i = fSavedFormulas.begin(); i != fSavedFormulas.end(); i++)
		FREE((*i).second);
} /* CReplaceAllCommand::~CReplaceAllCommand */
	
void	CReplaceAllCommand::DoCommand()
{
	fCellView->SetSelection(cell(1,1));
	
	while (fCellView->FindNext(false))
	{
		cell c = fCellView->CurCell();
		
		char f[512], *o;
		fSourceContainer->GetCellFormula(c, f);

		if (fSavedFormulas.count(c) == 0)
			fSavedFormulas[c] = STRDUP(f);
		
		int l1, l2, at;
		
		at = fCellView->FoundOffset();
		l1 = strlen(fWhat);
		l2 = strlen(fWith);
		o = f + at;
		
		memmove(o + l2, o + l1, strlen(o + l1) + 1);
		strncpy(o, fWith, l2);
		
		fCellView->ResetOffset(at + l2);
	
		if (!TryToParseString(f, c, fSourceContainer, true))
			THROW((B_NO_ERROR));
	
		fCellView->TouchLine(c.v);
	}
	UpdateView();
	fCellView->DrawStatus();
} /* CReplaceAllCommand::DoCommand */

void	CReplaceAllCommand::UndoCommand()
{
	std::map<cell,char*>::iterator i;
	
	for (i = fSavedFormulas.begin(); i != fSavedFormulas.end(); i++)
	{
		TryToParseString((*i).second, (*i).first, fSourceContainer, true);
		fCellView->TouchLine((*i).first.v);
	}
	fSavedFormulas.clear();

	UpdateView();
	fCellView->DrawStatus();
} /* CReplaceAllCommand::UndoCommand */

