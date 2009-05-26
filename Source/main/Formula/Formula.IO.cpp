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
	Formula.IO.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   FORMULA_H
#include "Formula.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   FUNCTIONUTILS_H
#include "FunctionUtils.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

//#ifndef   MYRESOURCES_H
//#include "MyResources.h"
//#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   STRINGUTILS_H
#include "StringUtils.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   FUNCTIONS_H
#include "Functions.h"
#endif

#ifndef   SET_H
#include "Set.h"
#endif

#ifndef   SWAPSTREAM_H
#include "SwapStream.h"
#endif

#include <support/Debug.h>

void CFormula::Write(BPositionIO& inStream)
{	
	CSwapStream str(inStream);
	int indx = 0;
	char token;
	
	do
	{
		token = (PFToken)fString[indx++];
	
		str << token;

		switch (token)
		{
			case opFunc:
			{
				FuncCallData fcd = *(FuncCallData *)(fString + indx);
				str << fcd.funcNr;
				str << fcd.argCnt;
				indx += sizeof(FuncCallData) / kPFWordSize;
				break;
			}
			case valBool:
				str << *(bool *)(fString + indx);
				indx++;
				break;
			case valNum:
			case valPerc:
			{
				str << *(double *)(fString + indx);
				indx += sizeof(double) / kPFWordSize;
				break;
			}
			case valName:
			case valStr:
			{
				char l = strlen((char*)(fString+indx));
				str << (char *)(fString + indx);
				l++;
				if (l & kPFAlignBits) /* We houden alles 'word aligned'*/
					l = (l & ~kPFAlignBits) + kPFWordSize;
				indx += l / kPFWordSize;
				break;
			}
			case valCell:
			{
				cell c = *(cell *)(fString + indx);
				str << c.v << c.h;
				indx += sizeof(cell) / kPFWordSize;
				break;
			}
			case valRange:
			{
				range r = *(range *)(fString + indx);
				str << r.top << r.left << r.bottom << r.right;
				indx += sizeof(range) / kPFWordSize;
				break;
			}
// •••R4Hack
//			case valXRef:
//			{
//				int ref = *(int *)(fString + indx);
//				CHECKWRITE(str, &ref, sizeof(XRef));
//				indx += sizeof(XRef) / kPFWordSize;
//				break;
//			}
		}
	}
	while (token != opEnd);
	
} /* CFormula::Write */

void CFormula::Read(BPositionIO& inStream, int *inFuncList)
{
	CSwapStream str(inStream);
	char b;
	PFToken opCode;
	int offset = 0;
	long buffer[1024];
	
	fString = buffer;
	
	do
	{
		str >> b;
		opCode = (PFToken)b;

		switch (opCode)
		{
			case opFunc:
			{
				short funcNr, argCnt;
				FuncCallData fcd;
				
				str >> funcNr >> argCnt;

				if (funcNr >= kFunctionCount)
					funcNr = inFuncList[funcNr];
				
				fcd.funcNr = funcNr;
				fcd.argCnt = argCnt;
				AddToken(opCode, &fcd, offset);
				break;
			}
			case valBool:
			{
				bool b;
				str >> b;
				AddToken(opCode, &b, offset);
				break;
			}
			case valNum:
			case valPerc:
			{
				double d;
				str >> d;
				AddToken(opCode, &d, offset);
				break;
			}
			case valTime:
			{
				time_t t;
				str >> t;
				AddToken(opCode, &t, offset);
				break;
			}
			case valName:
			case valStr:
			{
				char s[256];
				str >> s;
				AddToken(opCode, s, offset);
				break;
			}
			case valCell:
			{
				cell c;
				str >> c.v >> c.h;
				AddToken(opCode, &c, offset);
				break;
			}
			case valRange:
			{
				range r;
				str >> r.top >> r.left >> r.bottom >> r.right;
				AddToken(opCode, &r, offset);
				break;
			}
// •••R4Hack
//			case valXRef:
//			{
//				int ref;
//				str.Read(&ref, sizeof(XRef));
//				AddToken(valXRef, &ref, offset);
//				break;
//			}
			case opEnd:
			case opRaise:
			case opMul:
			case opDiv:
			case opRoot:
			case opPlus:
			case opMinus:
			case opNegate:
			case opLT:
			case opLE:
			case opEQ:
			case opGE:
			case opGT:
			case opNE:
			case opNOT:
			case opAND:
			case opOR:
			case opParen:
				AddToken(opCode, NULL, offset);
				break;
			default:
			{
#if DEBUG
				printf("incorrect opcode: %d ", opCode);
#endif
				fString = NULL;
				THROW((errCorruptedFile));
			}
		}
	}
	while (opCode != opEnd);

	int l = offset * kPFWordSize;
	fString = (long *)MALLOC(l);
	FailNil(fString);
	memcpy(fString, buffer, l);
} /* CFormula::Read */

void CFormula::CollectFunctionNrs(CSet& funcs) const
{
	if (!fString)
		return;

	PFToken theOpcode;
	int indx = 0, l;
	
	do {
		theOpcode = (PFToken)fString[indx];
		indx++;

		switch(theOpcode)
		{
			case opFunc:
			{
				int fn = ((FuncCallData*)(fString+indx))->funcNr;
				if (!funcs[fn])
					funcs += fn;
				indx += sizeof(FuncCallData) / kPFWordSize;
				break;
			}
			case valNum:
			case valPerc:
				indx += sizeof(double) / kPFWordSize;
				break;
			case valTime:
				indx += sizeof(time_t) / kPFWordSize;
				break;
			case valBool:
				indx++;
				break;
			case valStr:
			case valName:
				l = 1 + strlen((char *)(fString + indx));
				if (l & kPFAlignBits)
					l = (l & ~kPFAlignBits) + kPFWordSize;
				indx += l / kPFWordSize;
				break;
			case valCell:
				indx += sizeof(cell) / kPFWordSize;
				break;
			case valRange:
				indx += sizeof(range) / kPFWordSize;
				break;
				
			default:
				// there was a wearning about not all enum values handled in
				// switch statement.
				break;
				
// •••R4Hack
//			case valXRef:
//				indx += sizeof(XRef) / kPFWordSize;
//				break;
		}
	}
	while (theOpcode != opEnd);
} // CFormula::CollectFunctionNrs
