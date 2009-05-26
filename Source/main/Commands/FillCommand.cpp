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
	FillCommand.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   CELLCOMMANDS_H
#include "CellCommands.h"
#endif

#ifndef   FILLCOMMAND_H
#include "FillCommand.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   STLOCKER_H
#include "StLocker.h"
#endif

#ifndef   UNDOSTRS_H
#include "undostrs.h"
#endif

#ifndef   CELLITERATOR_H
#include "CellIterator.h"
#endif


#include <Message.h>
#include <Autolock.h>

CFillCommand::CFillCommand(CCellView *inView, CContainer *inCells,
	BMessage *msg)
	: CCellCommand(kFillStrID, inView, inCells)
{
	fSavedCells = NULL;
	fCellView->GetSelection(fRange);
	fStart = msg->FindDouble("first");
	fIncrement = msg->FindDouble("increment");
	fNrs = msg->FindInt32("type") == eNumData;
	fRows = msg->FindBool("rows");
	if (msg->FindBool("day"))
		fTime = 1;
	else if (msg->FindBool("week"))
		fTime = 2;
	else if (msg->FindBool("month"))
		fTime = 3;
	else if (msg->FindBool("quarter"))
		fTime = 4;
	else if (msg->FindBool("year"))
		fTime = 5;
	fSteps = (fRange.right - fRange.left + 1) * (fRange.bottom - fRange.top + 1) - 1;
} /* CFillCommand */

CFillCommand::~CFillCommand()
{
	if (fSavedCells)
		fSavedCells->Release();
} /* ~CFillCommand */

void CFillCommand::DoCommand()
{
	cell c = fRange.TopLeft();
	struct tm inc, first;
	int style = fSourceContainer->GetCellStyleNr(fRange.TopLeft());
	
	memset(&inc, 0, sizeof(struct tm));
	
	fSavedCells = new CContainer;
	BAutolock lock(fSavedCells);

	Value v;
	if (fNrs)
		v = fStart;
	else
		v = (time_t)fStart;

	if (!fNrs)
	{
		switch (fTime)
		{
			case 1:	inc.tm_mday = static_cast<int>(fIncrement)     ; break;
			case 2:	inc.tm_mday = static_cast<int>(fIncrement * 7) ; break;
			case 3:	inc.tm_mon  = static_cast<int>(fIncrement)     ; break;
			case 4:	inc.tm_mon  = static_cast<int>(fIncrement * 3) ; break;
			case 5:	inc.tm_year = static_cast<int>(fIncrement)     ; break;
		}
		first = *localtime(&v.fTime);
	}
	
	int x, y;
	
	if (fRows)
	{
		for (y = fRange.top; y <= fRange.bottom; y++)
			for (x = fRange.left; x <= fRange.right; x++)
			{
				c = cell(x, y);
				
				BAutolock lock(fSourceContainer);
				fSourceContainer->CopyCell(fSavedCells, c, c);
				
				fSourceContainer->SetValue(c, v);
				fSourceContainer->SetCellStyleNr(c, style);
				
				if (fNrs)
					v.fDouble += fIncrement;
				else
				{
					first.tm_sec += inc.tm_sec;
					first.tm_min += inc.tm_min;
					first.tm_hour += inc.tm_hour;
					first.tm_mday += inc.tm_mday;
					first.tm_mon += inc.tm_mon;
					first.tm_year += inc.tm_year;
					
					v.fTime = mktime(&first);

		// Warning! this is a hack
					struct tm *lt = localtime(&v.fTime);
					v.fTime -= lt->tm_sec;
				}
				fCellView->TouchLine(c.v);
			}
	}
	else
	{
		for (x = fRange.left; x <= fRange.right; x++)
			for (y = fRange.top; y <= fRange.bottom; y++)
			{
				c = cell(x, y);
				
				BAutolock lock(fSourceContainer);
				fSourceContainer->CopyCell(fSavedCells, c, c);
				
				fSourceContainer->SetValue(c, v);
				fSourceContainer->SetCellStyleNr(c, style);
				
				if (fNrs)
					v.fDouble += fIncrement;
				else
				{
					first.tm_sec += inc.tm_sec;
					first.tm_min += inc.tm_min;
					first.tm_hour += inc.tm_hour;
					first.tm_mday += inc.tm_mday;
					first.tm_mon += inc.tm_mon;
					first.tm_year += inc.tm_year;
					
					v.fTime = mktime(&first);

		// Warning! this is a hack
					struct tm *lt = localtime(&v.fTime);
					v.fTime -= lt->tm_sec;
				}
				fCellView->TouchLine(c.v);
			}
	}
	
	UpdateView();
} /* DoCommand */

void CFillCommand::UndoCommand()
{
	cell c;
	
	CCellIterator iter(fSavedCells, &fRange);
	
	while (iter.Next(c))
	{
		BAutolock lock(fSourceContainer);
		fSourceContainer->DisposeCell(c);
		fSavedCells->CopyCell(fSourceContainer, c, c);
		fCellView->TouchLine(c.v);
	}
	
	fSavedCells->Release();
	fSavedCells = NULL;
	
	UpdateView();
} /* UndoCommand */

