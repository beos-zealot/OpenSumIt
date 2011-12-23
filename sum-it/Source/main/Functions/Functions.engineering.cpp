/*
	Functions.engineering.cpp
	
	Copyright 2011, Gediminas Jarulaitis
	All rights reserved. Distributed under the terms of the MIT License.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Container.h"
#include "CellIterator.h"
#include "FunctionUtils.h"
#include "Functions.h"
#include "Globals.h"

void DEC2HEXFunction(Value *stack, int argCnt, CContainer *cells)
{
	double number;
	double places;
	
	if (GetDoubleArgument(stack, argCnt, 1, &number) && 
		GetDoubleArgument(stack, argCnt, 2, &places))
	{
		char str[256];
		char format[256];
		
		int n =	static_cast<int>(number);
		int p = static_cast<int>(places);		

		sprintf(format, "%%.%iX", p);
		sprintf(str, format, n);
		
		stack[0] = str;
	}
	else
		stack[0] = gValueNan;
}

