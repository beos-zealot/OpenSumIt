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
	Functions.finance.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "Container.h"
#include "CellIterator.h"
#include "FunctionUtils.h"
#include "Functions.h"
#include "Globals.h"

double NetPresentValue(CContainer *cells, range r, double rate);


void ANNUITYFunction(Value *stack, int argCnt, CContainer *cells)
{
	double rate, periods;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &rate) &&
	   	GetDoubleArgument(stack, argCnt, 2, &periods))
		stack[0] = (1 - pow(1 + rate, -periods)) / rate;
	else
		stack[0] = gRefNan;
}

void COMPOUNDFunction(Value *stack, int argCnt, CContainer *cells)
{
	double rate, periods;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &rate) &&
	    GetDoubleArgument(stack, argCnt, 2, &periods)) 
		stack[0] = pow(1 + rate, periods);
	else
		stack[0] = gRefNan;
}

void DBFunction(Value *stack, int argCnt, CContainer *cells)
{
	double inv, term, mult, period;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &inv) &&
	   	GetDoubleArgument(stack, argCnt, 2, &term) &&
	   	GetDoubleArgument(stack, argCnt, 3, &mult) &&
	   	GetDoubleArgument(stack, argCnt, 4, &period)) 
		stack[0] = inv * mult / term * pow(1 - mult / term, period - 1);
	else
		stack[0] = gRefNan;
}

void FVFunction(Value *stack, int argCnt, CContainer *cells)
{
	double pmt, rate, payments;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &pmt) &&
	    GetDoubleArgument(stack, argCnt, 2, &rate) &&
	    GetDoubleArgument(stack, argCnt, 3, &payments)) 
		stack[0] = (pmt * (pow(1 + rate, payments) - 1)) / rate;
	else
		stack[0] = gRefNan;
}

void PMTFunction(Value *stack, int argCnt, CContainer *cells)
{
	double principal, rate, term;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &principal) &&
	    GetDoubleArgument(stack, argCnt, 2, &rate) &&
	    GetDoubleArgument(stack, argCnt, 3, &term)) 
		stack[0] = (principal * rate) / (1 - pow(1 + rate, -term));
	else
		stack[0] = gRefNan;
}

void PVFunction(Value *stack, int argCnt, CContainer *cells)
{
	double pmt, rate, payments;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &pmt) &&
	    GetDoubleArgument(stack, argCnt, 2, &rate) &&
	    GetDoubleArgument(stack, argCnt, 3, &payments))
	{
		if (rate != 0)
			stack[0] = (pmt * (1 - pow(1 + rate, -payments))) / rate;
		else
			stack[0] = gFinanceNan;
	}
	else
		stack[0] = gRefNan;
}

void SLFunction(Value *stack, int argCnt, CContainer *cells)
{
	double inv, residual, periods;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &inv) &&
	    GetDoubleArgument(stack, argCnt, 2, &residual) &&
	    GetDoubleArgument(stack, argCnt, 3, &periods))
	{
		if (periods != 0)
			stack[0] = (inv - residual) / periods;
		else
			stack[0] = gFinanceNan;
	}
	else
		stack[0] = gRefNan;
}

void SOYDFunction(Value *stack, int argCnt, CContainer *cells)
{
	double inv, residual, term, period;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &inv) &&
	    GetDoubleArgument(stack, argCnt, 2, &residual) &&
	    GetDoubleArgument(stack, argCnt, 2, &term) &&
	    GetDoubleArgument(stack, argCnt, 3, &period))
		stack[0] = (2 * (1 + term - period) * (inv - residual)) / (term * (term + 1));
	else
		stack[0] = gRefNan;
}

double NetPresentValue(CContainer *cells, range r, double rate)
{
	double result = 0.0;
	cell c;
	Value val;
	int i = 1;
	
	if (!r.IsValid())
		return gRefNan;
	
	CCellIterator iter(cells, &r);
	while (iter.NextExisting(c))
	{
		cells->GetValue(c, val);
		if (val.fType == eNumData) 
			result += (double)val / pow(1 + rate, i++);
	}
	
	return result;
} /* NetPresentValue */

void NPVFunction(Value *stack, int argCnt, CContainer *cells)
{
	double rate;
	range flows;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &rate) &&
	    GetRangeArgument(stack, argCnt, 2, &flows))
		stack[0] = NetPresentValue(cells, flows, rate);
	else
		stack[0] = gRefNan;
}

void IRRFunction(Value *stack, int argCnt, CContainer *cells)
{
	double estimate;
	range flows;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &estimate) &&
		GetRangeArgument(stack, argCnt, 2, &flows) &&
	    flows.IsValid())
	{
		try
		{
			double res, mint, maxt, minr, maxr;
			int i = 0;
			
			minr = maxr = 0;
			mint = maxt = 0;
			
			i = 0;
			while (minr >= 0)
			{
				mint += 1;
				minr = NetPresentValue(cells, flows, mint);
				if (++i >= 100) throw gFinanceNan;
			}
	
			i = 0;		
			while (maxr <= 0)
			{
				maxt -= 1;
				maxr = NetPresentValue(cells, flows, maxt);
				if (++i >= 100) throw gFinanceNan;
			}
	
			i = 0;
			while (true)
			{
				if (i++ >= 40) throw gFinanceNan;
				res = NetPresentValue(cells, flows, estimate);
				if (fabs(res) <= 0.00000001)
				{
					stack[0] = estimate;
					break;
				}
				if (res > 0)
				{
					maxt = estimate;
					maxr = res;
				}
				else if (res < 0)
				{
					mint = estimate;
					minr = res;
				}
				if (minr / -10 > maxr)
					estimate = (maxt * 10 + mint) / 11;
				else if (minr / -2 > maxr)
					estimate = (maxt * 2 + mint) / 3;
				else if (minr * -10 < maxr)
					estimate = (maxt + mint * 10) / 11;
				else if (minr * -2 < maxr)
					estimate = (maxt + mint * 2) / 3;
				else
					estimate = (maxt + mint) / 2;
			}
		}
		catch (double d)
		{
			stack[0] = d;
		}
	}
	else
		stack[0] = gRefNan;
}

