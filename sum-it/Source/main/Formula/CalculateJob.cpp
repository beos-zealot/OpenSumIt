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
	CalculateJob.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   CONFIG_H
#include "Config.h"
#endif

#ifndef   CALCULATEJOB_H
#include "CalculateJob.h"
#endif

#ifndef   CALCDEFS_H
#include "CalcDefs.h"
#endif

#ifndef   CELL_H
#include "Cell.h"
#endif

//#ifndef   FORMULA_H
//#include "Formula.h"
//#endif

#ifndef   CELLDATA_H
#include "CellData.h"
#endif

//#ifndef   CELLVIEW_H
//#include "CellView.h"
//#endif

#ifndef   SET_H
#include "Set.h"
#endif

#ifndef   PROGRESSVIEW_H
#include "ProgressView.h"
#endif

//#ifndef   VALUE_H
//#include "Value.h"
//#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

//#ifndef   BTREE_T
//#include "BTree.t"
//#endif

#ifndef   CALCLOOPER_H
#include "CalcLooper.h"
#endif

//#ifndef   NAMETABLE_H
//#include "NameTable.h"
//#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif

#include <StLocker.h>

#include <Autolock.h>
#include <Message.h>
#include <Messenger.h>

#include <cstdio>

/* Implementatie */

const ulong kBufferCount = 500;

CCalculateJob::CCalculateJob(BView *inView, CContainer *inContainer)
	: MThread("CalculateJob")
{
	fContainer = inContainer;
	fInView = inView;
	fChangedLines = new CSet;
	fPortID = create_port(5, "calcport");
	FailOSErr(fPortID, errCreatingPort);
	fCalcThread1 = new CCalcThread(fContainer, fChangedLines, fPortID);
	fCalcThread1->Run();
//	fCalcThread2 = new CCalcThread(fContainer, fChangedLines, fPortID);
//	fCalcThread2->Run();
} /* CCalculateJob::CCalculateJob */

CCalculateJob::~CCalculateJob()
{
	delete_port(fPortID);
} /* CCalculateJob::~CCalculateJob */

long CCalculateJob::Execute() throw()
{
	long indx = 0, graphSize, result = 0;
	cell *graph = NULL;

	graphSize = fContainer->GetCellCount();
	
	try
	{
		if (graphSize)
		{
			cellmap::iterator iter;
				
			graph = (cell *)MALLOC(graphSize * sizeof(cell));
			FailNil(graph);
		
			StWriteLock lock(fContainer);
			for (iter = fContainer->fCellData.begin(); iter != fContainer->fCellData.end(); iter++)
			{
				if (!(*iter).second.mConstant)
				{
					graph[indx] = (*iter).first;
					(*iter).second.mStatus = kUndefined;
					indx++;
				}
				else
					graphSize--;
			}
		
			StProgress progress(fContainer->fInView,
				graphSize, pColorRed, true);
		
// Bezoek alle cellen die nog niet berekend zijn
			for (indx = 0; indx < graphSize; indx++)
			{
				int depth = fContainer->fCellData[graph[indx]].mStatus;
				if (depth == kUndefined)
					Visit(graph[indx], &progress);
				if (Cancelled())
					break;
			}

			if (Cancelled())
			{
				BAutolock lock(fContainer);
				for (indx = 0; indx < graphSize; indx++)
					if (fContainer->fCellData[graph[indx]].mStatus == kUndefined)
					{
						Value v = gEvalNan;
						fContainer->SetValue(graph[indx], v);
					}
			}

			long l = 0;
			write_port(fPortID, 'quit', &l, 4);
//			write_port(fPortID, 'quit', &l, 4);
			wait_for_thread(fCalcThread1->Thread(), &l);
//			wait_for_thread(fCalcThread2->Thread(), &l);
		
// ruim de graaf op
			FREE(graph);
		}
	}
		
	catch (CErr& err)
	{
		CATCHED;
	
		fCalcThread1->Cancel();
//		fCalcThread2->Cancel();
		long l;
		wait_for_thread(fCalcThread1->Thread(), &l);
//		wait_for_thread(fCalcThread2->Thread(), &l);

		err.DoError();
		if (graph)
			FREE(graph);
		result = -1;
	}

	if (!Cancelled())
	{
		BMessage msg(msg_CalculationDone);
		msg.AddPointer("dirtylines", fChangedLines);
		BMessenger(fInView).SendMessage(&msg);
	}
	else
		delete fChangedLines;

	return result;
} /* CCalculateJob::DoCalculate */

void CCalculateJob::Visit(cell inCell, StProgress *inProgress)
{
	cellmap::iterator i;

	{
		StLocker<CContainer> lock(fContainer);
		
		i = fContainer->fCellData.find(inCell);
		if (i == fContainer->fCellData.end())
			return;
	}

	CFormulaIterator refs((*i).second.mFormula, inCell);
	cell dependsOn;

	if ((*i).second.mConstant || !refs.Next(dependsOn))
	{
		fContainer->fCellData[inCell].mStatus = kCalculated;
		write_port(fPortID, 'cell', &inCell, 4);
		inProgress->Step();
		return;
	}
	
	do
	{
		(*i).second.mStatus = kOnStack;
	
		do
		{
			i = fContainer->fCellData.find(dependsOn);
			
			if (dependsOn == inCell)
				fContainer->fCellData[inCell].mStatus = kCircle;
			else if (i != fContainer->fCellData.end() &&
				!(*i).second.mConstant)
			{
				if ((*i).second.mStatus == kUndefined)
				{
					fStack.push(inCell, refs, dependsOn);
					refs = CFormulaIterator((*i).second.mFormula, dependsOn);
					inCell = dependsOn;
					(*i).second.mStatus = kOnStack;
				}
				else if ((*i).second.mStatus == kCircle || (*i).second.mStatus == kOnStack)
				{
					fContainer->fCellData[inCell].mStatus = kCircle;
					break;
				}
			}
		}
		while (refs.Next(dependsOn));

		if (fContainer->fCellData[inCell].mStatus == kCircle)
			fContainer->fCellData[inCell] = gCircleNan;
		else
		{
			fContainer->fCellData[inCell].mStatus = kCalculated;
			write_port(fPortID, 'cell', &inCell, 4);
			inProgress->Step();
		}
	}
	while(fStack.pop(inCell, refs, dependsOn) &&
			(i = fContainer->fCellData.find(inCell)) != fContainer->fCellData.end() &&
			(*i).second.mStatus != kCircle && !Cancelled()
			);
} /* Visit */
