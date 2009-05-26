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
	CalcStack.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CALCSTACK_H
#include "CalcStack.h"
#endif

//#ifndef   FORMULA_H
//#include "Formula.h"
//#endif

//#ifndef   CELLDATA_H
//#include "CellData.h"
//#endif

//#ifndef   VALUE_H
//#include "Value.h"
//#endif

//#ifndef   CONTAINER_H
//#include "Container.h"
//#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

//#ifndef   MTHREAD_H
//#include "MThread.h"
//#endif

//#ifndef   SET_H
//#include "Set.h"
//#endif

//#ifndef   CALCULATEJOB_H
//#include "CalculateJob.h"
//#endif

//#ifndef   CALCLOOPER_H
//#include "CalcLooper.h"
//#endif

CCalcStack::CCalcStack()
{
	fTop = fPreFlight - 1;
	fPreIndx = -1;
} /* CCalcStack */

void CCalcStack::push(cell inCell, CFormulaIterator& inIterator, cell inDependsOn)
{
	StackElement *newElement;
	
	if (++fPreIndx < kPreFlightStackSize)
		newElement = fPreFlight + fPreIndx;
	else
		newElement = new StackElement;

	FailNil(newElement);
	
	newElement->sCell = inCell;
	inIterator.GetData(newElement->sIter);
	newElement->sDependsOn = inDependsOn;
	newElement->sNext = fTop;
	fTop = newElement;
} /* push */

bool CCalcStack::pop(cell& outCell, CFormulaIterator& inIterator, cell& outDependsOn)
{
	if (fTop != fPreFlight - 1)
	{
		StackElement *next = fTop->sNext;
		outCell = fTop->sCell;
		inIterator.SetData(fTop->sIter);
		outDependsOn = fTop->sDependsOn;
		
		if (fPreIndx-- >= kPreFlightStackSize)
			delete fTop;

		fTop = next;
		return true;
	}
	else
		return false;
} /* pop */

CCalcStack::~CCalcStack()
{
	cell c, l;
	CFormulaIterator r(NULL, c);

	while (fPreIndx >= kPreFlightStackSize)
		pop(l, r, c);
} // ~CCalcStack
