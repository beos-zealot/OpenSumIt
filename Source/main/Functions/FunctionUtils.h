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
	FunctionUtils.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef FUNCTIONUTILS_H
#define FUNCTIONUTILS_H

#include "Value.h"

typedef struct {
	char funcName[10];
	short argCnt, funcNr, groupNr;
} FuncRec;

typedef void (*Function)(Value *stack, int argCnt, CContainer *cells);

extern Function	*gFuncs;
extern FuncRec	*gFuncArrayByNr, *gFuncArrayByName;
extern long		gFuncCount;
extern const char	**gFuncDescriptions, **gFuncPasteStrings;

void InitFunctions();

_EXPORT bool GetDoubleArgument(Value *inStack, int inArgCnt,
	int inArgNr, double *outDouble);
_EXPORT bool GetTextArgument(Value *inStack, int inArgCnt,
	int inArgNr, char *outText);
_EXPORT bool GetBooleanArgument(Value *inStack, int inArgCnt,
	int inArgNr, bool *outBoolean);
_EXPORT bool GetTimeArgument(Value *inStack, int inArgCnt,
	int inArgNr, time_t *outTime);
_EXPORT bool GetRangeArgument(Value *inStack, int inArgCnt,
	int inArgNr, range *outRange);

_EXPORT void Return(Value *stack, ValueType type, void *data);

_EXPORT bool CheckForNanParameters(Value *inStack, int inArgCnt);

#endif
