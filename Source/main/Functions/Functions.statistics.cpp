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
	Functions.statistics.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "Container.h"
#include "CellIterator.h"
#include "FunctionUtils.h"
#include "Functions.h"
#include "Globals.h"

void STDDEVFunction(Value *stack, int argCnt, CContainer *cells)
{
	VARIANCEFunction(stack, argCnt, cells);
//	if (stack[0].fType == eNumData && !isnan(stack[0].fDouble))
// deze if is overbodig, stack[0] is altijd een nummer, desnoods een nan
// en de sqrt van een nan is die nan zelf weer...
		stack[0].fDouble = sqrt(stack[0].fDouble);
}

void VARIANCEFunction(Value *stack, int argCnt, CContainer *cells)
{
	long countedCells = 0;
	range cRange;
	int i;
	cell c;
	double theResult, tmp, average;
	Value val;

	theResult = 0.0;
	average = 0.0;

	for (i = 1; i <= argCnt; i++)
	{

		if (GetDoubleArgument(stack, argCnt, i, &tmp))
		{
			average += tmp;
			countedCells++;
		}
		else if (GetRangeArgument(stack, argCnt, i, &cRange))
		{
			if (!cRange.IsValid())
			{
				theResult = gRefNan;
				break;
			}
		
			CCellIterator iter(cells, &cRange);
			while (iter.NextExisting(c))
			{
				cells->GetValue(c, val);
				if (val.fType == eNumData)
				{
					average += val.fDouble;
					countedCells++;
				}
			}
		}
	}
	
	if (countedCells)
	{
		average /= countedCells;
		
		for (i = 1; i <= argCnt; i++)
		{
			if (GetDoubleArgument(stack, argCnt, i, &tmp))
				theResult += (tmp - average) * (tmp - average);
			else if (GetRangeArgument(stack, argCnt, i, &cRange))
			{
				if (!cRange.IsValid())
				{
					theResult = gRefNan;
					break;
				}
				CCellIterator iter(cells, &cRange);
				while (iter.NextExisting(c))
				{
					cells->GetValue(c, val);
					if (val.fType == eNumData)
						theResult += (val.fDouble - average) * (val.fDouble - average);
				}
			}
		}
		
		theResult /= countedCells - 1;
	}
	
	stack[0] = theResult;
}

