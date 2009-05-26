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
	Functions.spreadsheet.c
	
	Copyright 1995 - 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   FUNCTIONUTILS_H
#include "FunctionUtils.h"
#endif

#ifndef   FUNCTIONS_H
#include "Functions.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif


#include <Window.h>

void CHOOSEFunction(Value *stack, int argCnt, CContainer *cells)
{
	Value result;
	double d;
	int indx;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetDoubleArgument(stack, argCnt, 1, &d))
		indx = static_cast<int>(rint(d));
	else
		indx = argCnt;	// levert dus een fout op...
	
	if (indx <= argCnt - 1 && indx > 0)
		stack[0] = stack[indx];
	else
		stack[0] = gRefNan;
}

void ERRFunction(Value *stack, int , CContainer *)
{
	stack[0] = gErrorNan;
}

void ERRORFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d))
	{
		if (isnan(d))
			stack[0] = d;
		else
		{
//			char n[32];
//			sprintf(n, "%d", (int)rint(d));
			stack[0] = Nan( static_cast<int>(rint(d)) );
		}
	}
	else
		stack[0] = gRefNan;
}

void FALSEFunction(Value *stack, int, CContainer *)
{
	stack[0] = (bool)false;
}

void TRUEFunction(Value *stack, int, CContainer *)
{
	stack[0] = (bool)true;
}

void IFFunction(Value *stack, int argCnt, CContainer *cells)
{
	bool b;
	double d;

	if (GetBooleanArgument(stack, argCnt, 1, &b))
	{
		if (b && (argCnt >= 2)) 
			stack[0] = stack[1];
		else if (!b && (argCnt >= 3)) 
			stack[0] = stack[2];
		else
			stack[0].Clear();
	}
	else if (GetDoubleArgument(stack, argCnt, 1, &d) && isnan(d))
		stack[0] = d;
	else
		stack[0] = gRefNan;
}

void IFERRFunction(Value *stack, int argCnt, CContainer *cells)
{
	double d;

	if (GetDoubleArgument(stack, argCnt, 1, &d) && isnan(d) &&
	   (argCnt >= 2)) 
		stack[0] = stack[1];
	else if (argCnt >= 3) 
		stack[0] = stack[2];
	else
		stack[0].Clear();
}

void ISNULLFunction(Value *stack, int, CContainer *)
{
	stack[0] = (bool)(stack[0].fType == eNoData);
}

void ISNUMFunction(Value *stack, int, CContainer *)
{
	stack[0] = (bool)(stack[0].fType == eNumData && !isnan(stack[0].fDouble));
}

void ISTEXTFunction(Value *stack, int, CContainer *)
{
	stack[0] = (bool)(stack[0].fType == eTextData);
}

void NAFunction(Value *stack, int, CContainer *)
{
	stack[0] = gNANan;
}

void CELLFunction(Value *stack, int argCnt, CContainer *cells)
{
	double row = 0, column = 0;
	cell c;
	
	if ( GetDoubleArgument(stack, argCnt, 1, &column)
	  && !isnan(column)
	  && (c.h = static_cast<short>( rint(column) )) > 0
	  && c.h <= kColCount
	  && GetDoubleArgument(stack, argCnt, 2, &row)
	  && !isnan(row)
	  && ( c.v = static_cast<short>(rint(row)) ) > 0 && c.v <= kRowCount)
	{
		cell c;
		Value val;
		
		c.h = static_cast<short>(rint(column)) ;
		c.v = static_cast<short>(rint(row)) ;
		
		cells->GetValue(c, val);
		
		stack[0] = val;
	}
	else if (isnan(column))
		stack[0] = column;
	else if (isnan(row))
		stack[0] = row;
	else
		stack[0] = gRefNan;
}

void COLUMNFunction(Value *stack, int argCnt, CContainer *cells)
{
	range cRange;
	double d;
	
	if (GetRangeArgument(stack, argCnt, 1, &cRange) && cRange.IsValid())
		stack[0] = (double)cRange.left;
	else if (GetDoubleArgument(stack, argCnt, 1, &d) && isnan(d))
		stack[0] = d;
	else
		stack[0] = gRefNan;
}

void HINDEXFunction(Value *stack, int argCnt, CContainer *cells)
{
	range cRange;

	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetRangeArgument(stack, argCnt, 2, &cRange) && cRange.IsValid())
	{
		char keyS[256];
		double key;
		time_t keyD;
		int v = cRange.top;
		cell c;
		Value val;
		bool stop = false;
		
		if (GetDoubleArgument(stack, argCnt, 1, &key) && !isnan(key))
		{
			c.h = cRange.left - 1; c.v = v;
			do
			{
				c.h++;
				cells->GetValue(c, val);
				if (val.fType == eNumData)
					stop = (key <= val.fDouble);
			}
			while (c.h < cRange.right && !stop);
			
			stack[0] = (double)(c.h - cRange.left + 1);
			if (!stop)
				stack[0].fDouble += 1;
		}
		else if (GetTextArgument(stack, argCnt, 1, keyS))
		{
			c.h = cRange.left - 1; c.v = v;
			do
			{
				c.h++;
				cells->GetValue(c, val);
				if (val.fType == eTextData)
					stop = (strcmp(keyS, val.fText) <= 0);
			}
			while (c.h <= cRange.right && !stop);
			
			stack[0] = (double)(c.h - cRange.left + 1);
			if (!stop)
				stack[0].fDouble += 1;
		}
		else if (GetTimeArgument(stack, argCnt, 1, &keyD))
		{
			c.h = cRange.left - 1; c.v = v;
			do
			{
				c.h++;
				cells->GetValue(c, val);
				if (val.fType == eTimeData)
//					stop = memcmp(&keyD, &val.dTime, sizeof(LongDateTime)) <= 0;
					stop = (keyD <= val.fTime);
			}
			while (c.h <= cRange.right && !stop);
			
			stack[0] = (double)(c.h - cRange.left + 1);
			if (!stop)
				stack[0].fDouble += 1;
		}
	}
	else
		stack[0] = gRefNan;
}

void HLOOKUPFunction(Value *stack, int argCnt, CContainer *cells)
{
	range cRange;
	double offset;
	bool handled = false;
	Value val;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetRangeArgument(stack, argCnt, 2, &cRange) &&
		GetDoubleArgument(stack, argCnt, 3, &offset) &&
		cRange.IsValid())
	{
		char keyS[256];
		double key;
		time_t keyD;
		int v = cRange.top;
		cell c;
		bool stop = false;
		
		if (GetDoubleArgument(stack, argCnt, 1, &key) && !isnan(key))
		{
			c.h = cRange.left - 1; c.v = v;
			do
			{
				c.h++;
				cells->GetValue(c, val);
				if (val.fType == eNumData)
					stop = (key <= val.fDouble);
			}
			while (c.h <= cRange.right && !stop);
			
			if (stop)
			{
				c.v += static_cast<short>(rint(offset));
				cells->GetValue(c, stack[0]);
				handled = true;
			}
		}
		else if (GetTextArgument(stack, argCnt, 1, keyS))
		{
			c.h = cRange.left - 1; c.v = v;
			do
			{
				c.h++;
				cells->GetValue(c, val);
				if (val.fType == eTextData)
					stop = (strcmp(keyS, val.fText) <= 0);
			}
			while (c.h <= cRange.right && !stop);
			
			if (stop)
			{
				c.v += static_cast<short>(rint(offset)) ;
				cells->GetValue(c, stack[0]);
				handled = true;
			}
		}
		else if (GetTimeArgument(stack, argCnt, 1, &keyD))
		{
			c.h = cRange.left - 1; c.v = v;
			do
			{
				c.h++;
				cells->GetValue(c, val);
				if (val.fType == eTimeData)
					stop = (keyD <= val.fTime);
			}
			while (c.h <= cRange.right && !stop);
			
			if (stop)
			{
				c.v += static_cast<short>( rint(offset) ) ;
				cells->GetValue(c, stack[0]);
				handled = true;
			}
		}
	}
	
	if (!handled)
		stack[0] = gRefNan;
}

void NCOLSFunction(Value *stack, int argCnt, CContainer *cells)
{
	range cRange;
	double d;
	
	if (GetRangeArgument(stack, argCnt, 1, &cRange) && cRange.IsValid())
		stack[0] = (double)(cRange.right - cRange.left + 1);
	else if (GetDoubleArgument(stack, argCnt, 1, &d) && isnan(d))
		stack[0] = d;
	else
		stack[0] = gRefNan;
}

void NROWSFunction(Value *stack, int argCnt, CContainer *cells)
{
	range cRange;
	double d;
	
	if (GetRangeArgument(stack, argCnt, 1, &cRange) && cRange.IsValid())
		stack[0] = (double)(cRange.bottom - cRange.top + 1);
	else if (GetDoubleArgument(stack, argCnt, 1, &d) && isnan(d))
		stack[0] = d;
	else
		stack[0] = gRefNan;
}

void NUMPAGESFunction(Value *stack, int , CContainer *cells)
{
	CCellView *theView;
	
	if (cells && (theView = cells->GetOwner()) != NULL)
		stack[0] = (double)theView->CountPages();
	else
		stack[0] = gNANan;
}

void OFFSETFunction(Value *stack, int argCnt, CContainer *cells)
{
	range cRange;
	double dh, dv;
	int h, v;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetRangeArgument(stack, argCnt, 1, &cRange) &&
		cRange.IsValid() &&
		GetDoubleArgument(stack, argCnt, 2, &dh) &&
		( h = static_cast<int>(rint(dh)) ) + cRange.left > 0 && h + cRange.right <= kColCount &&
		GetDoubleArgument(stack, argCnt, 3, &dv) &&
		( v = static_cast<int>(rint(dv)) ) + cRange.top > 0 && v + cRange.bottom <= kRowCount)
	{
		cRange.OffsetBy(h, v);
		stack[0] = cRange;
	}
	else
		stack[0] = gRefNan;
}

void PAGEFunction(Value *stack, int , CContainer *cells)
{
	CCellView *theView;
	
	if (cells && (theView = cells->GetOwner()) != NULL)
		stack[0] = (double)theView->GetPageNrForCell(cells->CalculatingCell());
	else
		stack[0] = gNANan;
}

void DOCUMENTFunction(Value *stack, int, CContainer *cells)
{
	CCellView *thePane;
	
	if (cells && (thePane = cells->GetOwner()) != NULL)
#if !__BEOS__
	{
		Str255 s;
		thePane->Doc()->GetDescriptor(s);
		stack[0] = p2cstr(s);
	}
#else
		stack[0] = thePane->Window()->Title();
#endif
	else
		stack[0] = gNANan;
}

void ROWFunction(Value *stack, int argCnt, CContainer *cells)
{
	range cRange;
	double d;
	
	if (GetRangeArgument(stack, argCnt, 1, &cRange) && cRange.IsValid())
		stack[0] = (double)cRange.top;
	else if (GetDoubleArgument(stack, argCnt, 1, &d) && isnan(d))
		stack[0] = d;
	else
		stack[0] = gRefNan;
}

void VINDEXFunction(Value *stack, int argCnt, CContainer *cells)
{
	range cRange;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetRangeArgument(stack, argCnt, 2, &cRange) && cRange.IsValid())
	{
		char keyS[256];
		double key;
		time_t keyD;
		int h = cRange.left;
		cell c;
		Value val;
		bool stop = false;
		
		if (GetDoubleArgument(stack, argCnt, 1, &key))
		{
			c.h = h; c.v = cRange.top - 1;
			do
			{
				c.v++;
				cells->GetValue(c, val);
				if (val.fType == eNumData)
					stop = (key <= val.fDouble);
			}
			while (c.v <= cRange.bottom && !stop);
			
			stack[0] = (double)(c.v - cRange.top + 1);
			if (!stop)
				stack[0].fDouble += 1;
		}
		else if (GetTextArgument(stack, argCnt, 1, keyS))
		{
			c.h = h; c.v = cRange.top - 1;
			do
			{
				c.v++;
				cells->GetValue(c, val);
				if (val.fType == eTextData)
					stop = (strcmp(keyS, val.fText) <= 0);
			}
			while (c.v <= cRange.bottom && !stop);
			
			stack[0] = (double)(c.v - cRange.top + 1);
			if (!stop)
				stack[0].fDouble += 1;
		}
		else if (GetTimeArgument(stack, argCnt, 1, &keyD))
		{
			c.h = h; c.v = cRange.top - 1;
			do
			{
				c.v++;
				cells->GetValue(c, val);
				if (val.fType == eTimeData)
					stop = (keyD <= val.fTime);
			}
			while (c.v <= cRange.bottom && !stop);
			
			stack[0] = (double)(c.v - cRange.top + 1);
			if (!stop)
				stack[0].fDouble += 1;
		}
	}
	else
		stack[0] = gRefNan;
}

void VLOOKUPFunction(Value *stack, int argCnt, CContainer *cells)
{
	range cRange;
	double offset;
	bool handled = false;
	
	if (CheckForNanParameters(stack, argCnt))
		return;
	
	if (GetRangeArgument(stack, argCnt, 2, &cRange) &&
		cRange.IsValid() &&
		GetDoubleArgument(stack, argCnt, 3, &offset))
	{
		char keyS[256];
		double key;
		time_t keyD;
		int h = cRange.left;
		cell c;
		Value val;
		bool stop = false;
		
		if (GetDoubleArgument(stack, argCnt, 1, &key))
		{
			c.h = h; c.v = cRange.top - 1;
			do
			{
				c.v++;
				cells->GetValue(c, val);
				if (val.fType == eNumData)
					stop = (key <= val.fDouble);
			}
			while (c.v <= cRange.bottom && !stop);
			
			if (stop)
			{
				c.h += static_cast<short>(rint(offset)) ;
				cells->GetValue(c, stack[0]);
				handled = true;
			}
		}
		else if (GetTextArgument(stack, argCnt, 1, keyS))
		{
			c.h = h; c.v = cRange.top - 1;
			do
			{
				c.v++;
				cells->GetValue(c, val);
				if (val.fType == eTextData)
					stop = (strcmp(keyS, val.fText) <= 0);
			}
			while (c.v <= cRange.bottom && !stop);
			
			if (stop)
			{
				c.h += static_cast<short>(rint(offset)) ;
				cells->GetValue(c, stack[0]);
				handled = true;
			}
		}
		else if (GetTimeArgument(stack, argCnt, 1, &keyD))
		{
			c.h = h; c.v = cRange.top - 1;
			do
			{
				c.v++;
				cells->GetValue(c, val);
				if (val.fType == eTimeData)
					stop = (keyD <= val.fTime);
			}
			while (c.v <= cRange.bottom && !stop);
			
			if (stop)
			{
				c.h += static_cast<short>(rint(offset)) ;
				cells->GetValue(c, stack[0]);
				handled = true;
			}
		}
	}
	
	if (!handled)
		stack[0] = gRefNan;
}
