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
	Functions.math.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "Container.h"
#include "CellIterator.h"
#include "FunctionUtils.h"
#include "Functions.h"
#include "Globals.h"


void ABSFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d))
	{
		stack[0] = fabs(d);
	}
	else
	{
		stack[0] = gValueNan;
	}
}

void SUMFunction(Value *stack, int argCnt, CContainer *cells)
{
	double theResult = 0.0, tmp;
	int i;
	range cRange;
	cell c;
	Value val;

	for (i = 1; i <= argCnt; i++)
	{
		if (GetDoubleArgument(stack, argCnt, i, &tmp))
		{
			theResult += tmp;
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
					theResult += val.fDouble;
			}
		}
	}

	stack[0] = theResult;
}

void ACOSFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d)) 
		stack[0] = acos(d);
	else
		stack[0] = gRefNan;
}

void ASINFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d))
		stack[0] = asin(d);
	else
		stack[0] = gRefNan;
}

void ATANFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d)) 
		stack[0] = atan(d);
	else
		stack[0] = gRefNan;
}

void ATAN2Function(Value *stack, int argCnt, CContainer *cells)
{
	double x, y;
	
	if (GetDoubleArgument(stack, argCnt, 1, &x) &&
		GetDoubleArgument(stack, argCnt, 2, &y))
	{
		if (x==0 && y==0)
			stack[0] = gDivNan;
		else
			stack[0] = atan2(y,x);
	}		
	else
		stack[0] = gValueNan;
}

void COSFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d)) 
		stack[0] = cos(d);
	else
		stack[0] = gRefNan;
}

void COTFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d)) 
		stack[0] = 1 / tan(d);
	else
		stack[0] = gRefNan;
}

void EXPFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d)) 
		stack[0] = exp(d);
	else
		stack[0] = gRefNan;
}

void FRACFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d, h;

	if (GetDoubleArgument(stack, argCnt, 1, &d))
		stack[0] = modf(d, &h);
	else
		stack[0] = gRefNan;
}

void INTFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d)) 
		stack[0] = rint(d);
	else
		stack[0] = gRefNan;
}

void LNFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d)) 
		stack[0] = log(d);
	else
		stack[0] = gRefNan;
}

void LOGFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d)) 
		stack[0] = log10(d);
	else
		stack[0] = gRefNan;
}

void MODFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d1, d2;

	if (GetDoubleArgument(stack, argCnt, 1, &d1) &&
		GetDoubleArgument(stack, argCnt, 2, &d2))
		stack[0] = fmod(d1, d2);
	else
		stack[0] = gRefNan;
}

void PIFunction(Value *stack, int, CContainer *)
{
//	stack[0] = PI; //3.141592653589793239;
	stack[0] = 3.141592653589793239;
}

void POWERFunction(Value *stack, int argCnt, CContainer *cells)
{
	double number;
	double power;

	if (GetDoubleArgument(stack, argCnt, 1, &number) &&
		GetDoubleArgument(stack, argCnt, 2, &power))
		stack[0] = pow(number, power);
	else
		stack[0] = gRefNan;
}

void RANDOMFunction(Value *stack, int, CContainer *)
{
	stack[0] = (double)rand();
}

void SIGNFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d))
	{
		if (isnan(d))
			stack[0] = d;
		else if (d < 0)
			stack[0] = -1.0;
		else if (d > 0)
			stack[0] = 1.0;
		else
			stack[0] = 0.0;
	}
	else
		stack[0] = gRefNan;
}

void SINFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d))
		stack[0] = sin(d);
	else
		stack[0] = gRefNan;
}

void SQRTFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d))
		stack[0] = sqrt(d);
	else
		stack[0] = gRefNan;
}

void TANFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d)) 
		stack[0] = tan(d);
	else
		stack[0] = gRefNan;
}

void AVGFunction(Value *stack, int argCnt, CContainer *cells)
{
	long countedCells = 0;
	range cRange;
	int i;
	cell c;
	double theResult, tmp;
	Value val;

	theResult = 0.0;

	for (i = 1; i <= argCnt; i++)
	{

		if (GetDoubleArgument(stack, argCnt, i, &tmp))
		{
			theResult += tmp;
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
					theResult += val.fDouble;
					countedCells++;
				}
// 				else if (val.fType != eNoData)
// 				{
// 					theResult = gValueNan;
// 					break;
// 				}
			}
		}
	}

	if (countedCells)
		theResult /= countedCells;
	stack[0] = theResult;
}

void CEILINGFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;
	
	if (GetDoubleArgument(stack, argCnt, 1, &d))
		stack[0] = ceil(d);
	else
		stack[0] = gValueNan;
}

void COUNTFunction(Value *stack, int argCnt, CContainer *cells)
{
	int i;
	long countedCells;
	range cRange;
	cell c;
	Value val;

	countedCells = 0;

	for (i = 1; i <= argCnt; i++)
	{

		if (GetDoubleArgument(stack, argCnt, i, &val.fDouble))
		{
			countedCells++;
		}
		else if (GetRangeArgument(stack, argCnt, i, &cRange))
		{
			if (!cRange.IsValid())
			{
				stack[0] = gRefNan;
				return;
			}
			
			CCellIterator iter(cells, &cRange);
			while (iter.NextExisting(c))
			{
				cells->GetValue(c, val);
				if (val.fType == eNumData) 
					countedCells++;
			}
		}
	}

	stack[0] = (double)countedCells;
}

void FLOORFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;
	
	if (GetDoubleArgument(stack, argCnt, 1, &d))
		stack[0] = floor(d);
	else
		stack[0] = gValueNan;
}

void MAXFunction(Value *stack, int argCnt, CContainer *cells)
{
	double theResult = 0.0;
	int i;
	range cRange;
	cell c;
	Value val;
	bool foundFirst = false;

	for (i = 1; i <= argCnt; i++)
	{

		if (GetDoubleArgument(stack, argCnt, i, &val.fDouble))
		{
			if (!foundFirst || theResult < val.fDouble)
			{
				theResult = val.fDouble;
				foundFirst = true;
			}
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
					if (!foundFirst || theResult < val.fDouble)
					{
						theResult = val.fDouble;
						foundFirst = true;
					}
			}
		}
	}

	stack[0] = theResult;
}

void MINFunction(Value *stack, int argCnt, CContainer *cells)
{
	double theResult = 0.0;
	int i;
	range cRange;
	cell c;
	Value val;
	bool foundFirst = false;

	for (i = 1; i <= argCnt; i++)
	{

		if (GetDoubleArgument(stack, argCnt, i, &val.fDouble))
		{
			if (!foundFirst || theResult > val.fDouble)
			{
				theResult = val.fDouble;
				foundFirst = true;
			}
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
					if (!foundFirst || theResult > val.fDouble)
					{
						theResult = val.fDouble;
						foundFirst = true;
					}
			}
		}
	}

	stack[0] = theResult;
}

void ROUNDFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d, n;
	
	if (GetDoubleArgument(stack, argCnt, 1, &d) &&
		GetDoubleArgument(stack, argCnt, 2, &n))
	{
		if (isnan(d))
			;
		else if (isnan(n))
			d = n;
		else if (n > 15 || n < -15)
			d = gValueNan;
		else
		{
			double deel, heel, factor;
			
			factor = pow(10.0, n);
			
			d *= factor;
			deel = modf(d, &heel);
			d = heel / factor;
			if (deel > 0.5 && fmod(heel, 2.0) == 1.0)
				d += 1.0;
		}
		stack[0] = d;
	}
	else
		stack[0] = gValueNan;
}

