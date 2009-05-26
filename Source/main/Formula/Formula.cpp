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
	Formula.c
	
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

#include <support/Debug.h>
#ifndef   NAMETABLE_H
#include "NameTable.h"
#endif

#ifndef   SET_H
#include "Set.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   CONFIG_H
#include "Config.h"
#endif


const size_t kBuildBufferSize = 1000;

// globals
bool gWithEqualSign;

enum Precedence {
	eAddition, eRoot, eMultiplication, eRaising, eNumber,
	eLogic, eAnd, eOr, eNot
};

CFormula::CFormula()
{
	fString = NULL;
} /* CFormula::CFormula */

CFormula::CFormula(void *inString)
{
	fString = (long *)inString;
} /* CFormula::CFormula */

void CFormula::AddToken(PFToken inToken, const void *inData, int& ioOffset)
{
	if (!fString)
	{
		ioOffset = 0;
		fString = (long *)MALLOC(sizeof(long) * kBuildBufferSize);
		FailNil(fString);
	}
	
	int indx, toAdd;
	indx = ioOffset / kPFWordSize;
	
	switch (inToken)
	{
		case opFunc:
			toAdd = sizeof(FuncCallData);
			break;
		case valBool:
			toAdd = sizeof(bool);
			break;
		case valNum:
		case valPerc:
			toAdd = sizeof(double);
			break;
		case valTime:
			toAdd = sizeof(time_t);
			break;
		case valName:
		case valStr:
			toAdd = strlen((const char *)inData) + 1;
			break;
		case valCell:
			toAdd = sizeof(cell);
			break;
		case valRange:
			toAdd = sizeof(range);
			break;
// •••R4Hack
//		case valXRef:
//			toAdd = sizeof(XRef);
//			break;
		default:
			toAdd = 0;
			break;
	}

	if (toAdd & kPFAlignBits) /* We houden alles 'word aligned'*/
		toAdd = (toAdd & ~kPFAlignBits) + kPFWordSize;

	ioOffset += toAdd + kPFWordSize;

	fString[indx] = inToken;
	indx++;

	if (toAdd && (inData != NULL) )
		memcpy(fString + indx, inData, toAdd);
	else if (toAdd)
		memset(fString + indx, 0, toAdd);
} /* CFormula::AddToken */

void CFormula::Calculate(cell inLocation, Value& outResult, CContainer *inContainer) const
{
	PFToken nextOpcode;
	Value stack[kMaxStackHeight];
	int stackIndx, indx, sLen;
	cell theCell;
	range theRange;
	FuncCallData theFuncData;

	if (!fString)
	{
		outResult.fType = eNoData;
		return;
	}

	stackIndx = -1;
	indx = 0;

	while ((nextOpcode = (PFToken)fString[indx++]) != opEnd)
	{
		switch(nextOpcode)
		{
			case opLT:
				if (stack[stackIndx - 1].IsNan())	{}
				else if (stack[stackIndx].IsNan())	stack[stackIndx - 1] = stack[stackIndx];
				else	stack[stackIndx - 1] = stack[stackIndx - 1] < stack[stackIndx];
				stackIndx--;
				break;

			case opLE:
				if (stack[stackIndx - 1].IsNan())	{}
				else if (stack[stackIndx].IsNan())	stack[stackIndx - 1] = stack[stackIndx];
				else	stack[stackIndx - 1] = stack[stackIndx - 1] <= stack[stackIndx];
				stackIndx--;
				break;

			case opEQ:
				if (stack[stackIndx - 1].IsNan())	{}
				else if (stack[stackIndx].IsNan())	stack[stackIndx - 1] = stack[stackIndx];
				else	stack[stackIndx - 1] = stack[stackIndx - 1] == stack[stackIndx];
				stackIndx--;
				break;

			case opGE:
				if (stack[stackIndx - 1].IsNan())	{}
				else if (stack[stackIndx].IsNan())	stack[stackIndx - 1] = stack[stackIndx];
				else	stack[stackIndx - 1] = stack[stackIndx - 1] >= stack[stackIndx];
				stackIndx--;
				break;

			case opGT:
				if (stack[stackIndx - 1].IsNan())	{}
				else if (stack[stackIndx].IsNan())	stack[stackIndx - 1] = stack[stackIndx];
				else	stack[stackIndx - 1] = stack[stackIndx - 1] > stack[stackIndx];
				stackIndx--;
				break;

			case opNE:
				if (stack[stackIndx - 1].IsNan())	{}
				else if (stack[stackIndx].IsNan())	stack[stackIndx - 1] = stack[stackIndx];
				else	stack[stackIndx - 1] = stack[stackIndx - 1] != stack[stackIndx];
				stackIndx--;
				break;

			case opRaise:
				stack[stackIndx - 1] ^= stack[stackIndx];
				stackIndx--;
				break;

			case opMul:
				stack[stackIndx - 1] *= stack[stackIndx];
				stackIndx--;
				break;

			case opDiv:
				stack[stackIndx - 1] /= stack[stackIndx];
				stackIndx--;
				break;

			case opPlus:
				stack[stackIndx - 1] += stack[stackIndx];
				stackIndx--;
				break;

			case opMinus:
				stack[stackIndx - 1] -= stack[stackIndx];
				stackIndx--;
				break;

			case opNegate:
				if (stack[stackIndx].fType == eNumData)
					stack[stackIndx] = -stack[stackIndx].fDouble;
				else
					stack[stackIndx] = gRefNan;
				break;

			case opAND:
				stack[stackIndx - 1] &= stack[stackIndx];
				stackIndx--;
				break;

			case opOR:
				stack[stackIndx - 1] |= stack[stackIndx];
				stackIndx--;
				break;

			case opNOT:
				if (stack[stackIndx].fType == eBoolData)
					stack[stackIndx] = (bool)!stack[stackIndx].fBool;
				else
					stack[stackIndx] = gRefNan;
				break;

			case opFunc:
				theFuncData = *((FuncCallData *)(fString + indx));
				indx += sizeof(FuncCallData) / kPFWordSize;

				stackIndx -= theFuncData.argCnt - 1;
				if (stackIndx < 0)
				{
					char s[32];
					inLocation.GetName(s);
/**/					strcat(s, "(f)");
					THROW((errIllPFString, s));
				}
				if (theFuncData.funcNr >= 0 &&
					theFuncData.funcNr < gFuncCount &&
					gFuncs[theFuncData.funcNr])
					try
					{
						(gFuncs[theFuncData.funcNr])
							(stack + stackIndx, theFuncData.argCnt, inContainer);
					}
					catch(...)
					{
						CATCHED;
						stack[stackIndx] = gFuncNan;
					}
				else
					stack[stackIndx] = gFuncNan;
				break;

			case valNum:
			case valPerc:
				stackIndx++;
				stack[stackIndx] = *((double *)(fString + indx));
				indx += sizeof(double) / kPFWordSize;
				break;
			
			case valTime:
				stackIndx++;
				stack[stackIndx] = *((time_t *)(fString + indx));
				indx += sizeof(time_t) / kPFWordSize;
				break;

			case valStr:
				stackIndx++;
				stack[stackIndx] = (char *)(fString + indx);
				sLen = strlen(stack[stackIndx].fText) + 1;
				if (sLen & kPFAlignBits)
					sLen = (sLen & ~kPFAlignBits) + kPFWordSize;
				indx += sLen / kPFWordSize;
				break;

			case valCell:
				stackIndx++;
				theCell = *((cell *)(fString + indx));
				indx += sizeof(cell) / kPFWordSize;
				inContainer->GetValue(theCell.GetFlatCell(inLocation), stack[stackIndx]);
				break;

			case valRange:
				stackIndx++;
				theRange = *((range *)(fString + indx));
				
				if (theRange.TopLeft() == theRange.BotRight())
					inContainer->GetValue(theRange.TopLeft(), stack[stackIndx]);
				else
				{
					stack[stackIndx].fType = eRangeData;
					stack[stackIndx] = theRange.GetFlatRange(inLocation);
				}
				indx += sizeof(range) / kPFWordSize;
				break;
			
			case valBool:
				stackIndx++;
				stack[stackIndx] = *((bool *)(fString + indx));
				indx++;
				break;
			
			case valName:
			{
				stackIndx++;
				char *s = (char *)(fString + indx);
				sLen = strlen(s) + 1;
				if (sLen & kPFAlignBits)
					sLen = (sLen & ~kPFAlignBits) + kPFWordSize;
				indx += sLen / kPFWordSize;
				
				try
				{
					FailNil(inContainer);
					stack[stackIndx] = inContainer->ResolveName(s);
					range r = stack[stackIndx];
					if (r.TopLeft() == r.BotRight())
						inContainer->GetValue(r.TopLeft(), stack[stackIndx]);
				}
				catch(CErr& e)
				{
					CATCHED;
					stack[stackIndx] = gNameNan;
				}
				break;
			}
			
			case valNil:
				stackIndx++;
				stack[stackIndx].fType = eNoData;
				break;
// •••R4Hack
//			case valXRef:
//			{
//				stackIndx++;
//				XRef ref = *(XRef *)(fString + indx);
//				indx += sizeof(XRef) / kPFWordSize;
//				inContainer->GetOwner()->ResolveXRef(ref.fileNr,
//					ref.cell.GetFlatCell(inLocation), stack[stackIndx]);
//				break;
//			}
			
			case opParen:
				break;

			default:
			{
				char s[32];
				inLocation.GetName(s);
				strcat(s, "(t)");
				THROW((errIllPFString, s));
			}
		}
	}

	if (stackIndx != 0)
	{
		char s[32];
		inLocation.GetName(s);
		strcat(s, "(i)");
		THROW((errIllPFString, s));
	}

	outResult = stack[0];
} /* CFormula::Calculate */

void CFormula::UnMangle(char *outString, cell inLocation, CContainer *inContainer, bool rcStyle) const
{
	char *stack[kMaxStackHeight];
	int stackIndx, indx, sLen, i;
	PFToken nextOpcode;
	double d;
	cell theCell;
	range theRange;
	FuncCallData theFuncData;

	void *p = MALLOC(kMaxStackHeight * kMaxStringLength);
	FailNil(p);
	
	for (int i = 0; i < kMaxStackHeight; i++)
		stack[i] = (char *)p + i * kMaxStringLength;

	stackIndx = -1;
	indx = 0;

	try
	{
		while ((nextOpcode = (PFToken)fString[indx++]) != opEnd)
		{
			switch(nextOpcode)
			{
				case opLT:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "<");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opLE:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "<=");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opEQ:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "=");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opGE:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, ">=");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opGT:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, ">");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opNE:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "<>");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
				
				case opNOT:
					strcpy(outString, "!");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx], outString);
					break;
				
				case opAND:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "&");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opOR:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "|");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opRaise:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "^");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opMul:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "*");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opDiv:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "/");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opPlus:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "+");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opMinus:
					strcpy(outString, stack[stackIndx - 1]);
					strcat(outString, "-");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx - 1], outString);
					stackIndx--;
					break;
	
				case opNegate:
					strcpy(outString, "-");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx], outString);
					break;
	
				case opRoot:
					strcpy(outString, "√");
					strcat(outString, stack[stackIndx]);
					strcpy(stack[stackIndx], outString);
					break;
	
				case opParen:
					sLen = strlen(stack[stackIndx]);
					memmove(stack[stackIndx]+1, stack[stackIndx], sLen);
					stack[stackIndx][0] = '(';
					stack[stackIndx][sLen+1] = ')';
					stack[stackIndx][sLen+2] = 0;
					break;
				
				case opFunc:
				{
					theFuncData = *((FuncCallData *)(fString + indx));
					indx += sizeof(FuncCallData) / kPFWordSize;
	
					if (theFuncData.funcNr >= 0 &&
						theFuncData.funcNr < gFuncCount)
						strcpy(outString, gFuncArrayByNr[theFuncData.funcNr].funcName);
					else
						sprintf(outString, GetIndString(1, 19), theFuncData.funcNr);
						
					stackIndx -= theFuncData.argCnt - 1;
	
						// cannot guarantee that theFuncData.argCnt is always correct anymore
						// since we now build formula's from Excel formula's and those, yeah, well.. Microsoft huh?
					if (stackIndx < 0)
					{
						char name[32];
						inLocation.GetName(name);
						THROW((errIllPFString, name));
					}

					strcat(outString, "(");
					for(i = 0; i < theFuncData.argCnt; i++)
					{
						if (i)
						{
							char s[2];
							if (rcStyle)
								s[0] = ',';
							else
								s[0] = gListSeparator;
							s[1] = 0;
							strcat(outString, s);
						}
						strcat(outString, stack[stackIndx + i]);
					}
					strcat(outString, ")");
	
					strcpy(stack[stackIndx], outString);
					break;
				}
	
				case valNum:
				case valPerc:
					d = *((double *)(fString + indx));
					indx += sizeof(double) / kPFWordSize;
	
					if (nextOpcode == valPerc)
						d *= 100;
	
					ftoa(d, outString);
					
					if (rcStyle && gDecimalPoint != '.')
					{
						char *dp = strchr(outString, gDecimalPoint);
						if (dp) *dp = '.';
					}
	
					if (nextOpcode == valPerc)
						strcat(outString, "%");
	
					stackIndx++;
					strcpy(stack[stackIndx], outString);
					break;
	
				case valTime:
				{
					Value t = *((time_t *)(fString + indx));
					indx += sizeof(time_t) / kPFWordSize;
					
					stackIndx++;
					gFormatTable.FormatValue(0, t, stack[stackIndx]);
					break;
				}
	
				case valStr:
					stackIndx++;
					strcpy(stack[stackIndx], "\"");
					strcat(stack[stackIndx], (char *)(fString + indx));
					strcat(stack[stackIndx], "\"");
					sLen = 1 + strlen((char *)(fString + indx));
					if (sLen & kPFAlignBits)
						sLen = (sLen & ~kPFAlignBits) + kPFWordSize;
					indx += sLen / kPFWordSize;
					break;
	
				case valCell:
					theCell = *((cell *)(fString + indx));
					if (rcStyle)
						theCell.GetRCName(outString);
					else
						theCell.GetFormulaName(outString, inLocation);
					stackIndx++;
					strcpy(stack[stackIndx], outString);
					indx += sizeof(cell) / kPFWordSize;
					break;
	
				case valRange:
					theRange = *((range *)(fString + indx));
					if (rcStyle)
						theRange.GetRCName(outString);
					else
						theRange.GetFormulaName(outString, inLocation);
					stackIndx++;
					strcpy(stack[stackIndx], outString);
					indx += sizeof(range) / kPFWordSize;
					break;
				
				case valName:
					stackIndx++;
					strcpy(stack[stackIndx], (char *)(fString + indx));
					sLen = 1 + strlen((char *)(fString + indx));
					if (sLen & kPFAlignBits)
						sLen = (sLen & ~kPFAlignBits) + kPFWordSize;
					indx += sLen / kPFWordSize;
					break;
	// •••R4Hack
	//			case valXRef:
	//				stackIndx++;
	//				XRef ref;
	//				ref = *(XRef *)(fString + indx);
	//				indx += sizeof(XRef) / kPFWordSize;
	//				inContainer->GetOwner()->GetXRefDesc(ref.fileNr,
	//					ref.cell.GetRefCell(inLocation), stack[stackIndx]);
	//				break;
	
				case valNil:
					stackIndx++;
					stack[stackIndx][0] = 0;
					break;
				
				case valBool:
					stackIndx++;
					if (*(bool *)(fString + indx))
						strcpy(stack[stackIndx], gTrueString);
					else
						strcpy(stack[stackIndx], gFalseString);
					indx++;
					break;
	
				default:
				{
					char name[32];
					inLocation.GetName(name);
					THROW((errIllPFString, name));
				}
			}
		}
	
		if (stackIndx != 0)
		{
			char name[32];
			inLocation.GetName(name);
			THROW((errIllPFString, name));
		}
	
		if (gWithEqualSign && !rcStyle)
		{
			strcpy(outString, "=");
			strcat(outString, stack[0]);
		}
		else
			strcpy(outString, stack[0]);
	}
	catch (CErr& e)
	{
		strcpy(outString, e);
	}
	
	FREE(p);
} /* CFormula::UnMangle */

bool CFormula::IsConstant() const
{
	if (!fString)
		return true;

	PFToken theOpcode;
	int indx = 0, l;
	
	do {
		theOpcode = (PFToken)fString[indx];
		indx++;

		switch(theOpcode) {
			case opFunc:	return false;
			case valCell:	return false;
			case valRange:return false;
//			case valXRef:	return false;
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
				
			default:
				// there was a warning about not all enum values handled in
				// switch statement.
				break;
		}
	}
	while (theOpcode != opEnd);

	return true;
} /* CFormula::IsConstant */

long CFormula::StringLength() const
{
	if (!fString)
		return 0;

	PFToken theOpcode;
	int indx = 0, l;
	
	do {
		theOpcode = (PFToken)fString[indx];
		indx++;

		switch(theOpcode) {
			case opFunc:
				indx += sizeof(FuncCallData) / kPFWordSize;
				break;
			case valNum:
			case valPerc:
				indx += sizeof(double) / kPFWordSize;
				break;
			case valTime:
				indx += sizeof(time_t) / kPFWordSize;
				break;
			case valStr:
			case valName:
				l = 1 + strlen((char *)(fString + indx));
				if (l & kPFAlignBits)
					l = (l & ~kPFAlignBits) + kPFWordSize;
				indx += l / kPFWordSize;
				break;
			case valBool:
				indx++;
				break;
			case valCell:
				indx += sizeof(cell) / kPFWordSize;
				break;
			case valRange:
				indx += sizeof(range) / kPFWordSize;
				break;
			default:
				// there was a warning about not all enum values handled in
				// switch statement.
				break;
//			case valXRef:
//				indx += sizeof(XRef) / kPFWordSize;
//				break;
		}
	}
	while (theOpcode != opEnd);

	return indx * sizeof(long);
} /* CFormula::StringLength */

void* CFormula::CopyString() const
{
	long l = StringLength();
	void *t = MALLOC(l);
	FailNil(t);
	memcpy(t, fString, l);
	return t;
} /* CFormula::CopyString */

void CFormula::Clear()
{
	if (fString)
	{
		FREE(fString);
		fString = NULL;
	}
} /* CFormula::Clear */

void* CFormula::DetachString()
{
	void* result = fString;
	fString = NULL;
	return result;
} /* CFormula::DetachString */

void CFormula::operator=(void *inString)
{
	if (fString)
		FREE(fString);
	fString = (long *)inString;
} /* CFormula::operator= */

bool CFormula::ReduceToValue(Value& val, CContainer *inContainer) const
{
	if (!fString)
	{
		val = Value();
		return true;
	}
	
	int i;
	
	switch (fString[0])
	{
		case valNum:
		case valPerc:
			i = 1 + sizeof(double) / kPFWordSize;
			if (fString[i] == opNegate)
			{
				val = -(*(double *)(fString + 1));
				i++;
			}
			else
				val = *(double *)(fString + 1);
			
			return (fString[i] == opEnd);
		case valTime:
			if (fString[1 + sizeof(time_t) / kPFWordSize] == opEnd)
			{
				val = *(time_t *)(fString + 1);
				return true;
			}
			break;
		case valBool:
			if (fString[2] == opEnd)
			{
				val = *(bool *)(fString + 1);
				return true;
			}
			break;
		case valStr:
			i = strlen((char *)(fString + 1)) + 1;
			if (i & kPFAlignBits)
				i = (i & ~kPFAlignBits) + kPFWordSize;
			i /= kPFWordSize;
			if (fString[i + 1] == opEnd)
			{
				val = (char *)(fString + 1);
				return true;
			}
			break;
		case valName:
			i = strlen((char *)(fString + 1)) + 1;
			if (i & kPFAlignBits)
				i = (i & ~kPFAlignBits) + kPFWordSize;
			i /= kPFWordSize;
			if (fString[i + 1] == opEnd)
			{
				if (!inContainer)
				{
					val = (char *)(fString + 1);
					return true;
				}
				
				try
				{
					inContainer->ResolveName((char *)(fString + 1));
					return false;
				}
				catch (CErr& e)
				{
					val = (char *)(fString + 1);
					return true;
				}
			}
			break;
	}
	
	return false;
} /* CFormula::ReduceToValue */
