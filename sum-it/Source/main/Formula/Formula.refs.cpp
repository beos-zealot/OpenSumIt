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
	Formula.refs.c
	
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

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   NAMETABLE_H
#include "NameTable.h"
#endif


#if __BEOS__
#ifndef   BTREE_T
#include "BTree.t"
#endif

#ifndef   SET_H
#include "Set.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#endif

#include <Debug.h>
#include <cstring>

bool CFormula::RefersToName(const char *name) const
{
	if (!fString)
		return false;

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
			case valBool:
				indx++;
				break;
			case valStr:
			{
				char *s = (char *)(fString + indx);
				
				l = 1 + strlen(s);
				if (l & kPFAlignBits)
					l = (l & ~kPFAlignBits) + kPFWordSize;
				indx += l / kPFWordSize;
				break;
			}
			case valName:
			{
				char *s = (char *)(fString + indx);
				
#if __BEOS__ || __HAIKU__
				if (name == NULL || strcasecmp(name, s) == 0)
					return true;
#else
				if (name == NULL || ::relstring(name, s, false, true))
					return true;
#endif
				
				l = 1 + strlen(s);
				if (l & kPFAlignBits)
					l = (l & ~kPFAlignBits) + kPFWordSize;
				indx += l / kPFWordSize;
				break;
			}
			case valCell:
				indx += sizeof(cell) / kPFWordSize;
				break;
			case valRange:
				indx += sizeof(range) / kPFWordSize;
				break;
//			case valXRef:
//				indx += sizeof(XRef) / kPFWordSize;
//				break;	
			default:
				// there was a warning about not all enum values handled in
				// switch statement.
				break;
		}
	}
	while (theOpcode != opEnd);

	return false;
} /* CFormula::RefersToName */

bool CFormula::GetNextName(int& indx, CName& name) const
{
	if (!fString)
		return false;

	PFToken theOpcode;
	int l;
	
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
			case valBool:
				indx++;
				break;
			case valStr:
			{
				char *s = (char *)(fString + indx);
				
				l = 1 + strlen(s);
				if (l & kPFAlignBits)
					l = (l & ~kPFAlignBits) + kPFWordSize;
				indx += l / kPFWordSize;
				break;
			}
			case valName:
			{
				name = (char *)(fString + indx);
				return true;
			}
			case valCell:
				indx += sizeof(cell) / kPFWordSize;
				break;
			case valRange:
				indx += sizeof(range) / kPFWordSize;
				break;
//			case valXRef:
//				indx += sizeof(XRef) / kPFWordSize;
//				break;
			default:
				// there was a warningt about not all enum values handled
				// in switch statement
				break;
		}
	}
	while (theOpcode != opEnd);

	return false;
} /* CFormula::GetNextName */

void CFormula::UpdateReferences(cell inLocation, bool horizontal,
	int first, int count)
{
	if (!fString)
		return;

	PFToken theOpcode;
	int indx = 0, l;
	
	do {
		theOpcode = (PFToken)fString[indx++];

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
			{
				cell c = *(cell *)(fString + indx);
				c.Offset(inLocation, horizontal, first, count);
				*(cell *)(fString + indx) = c;
				
				indx += sizeof(cell) / kPFWordSize;
				break;
			}
			case valRange:
			{
				range r = *(range *)(fString + indx);
				r.Offset(inLocation, horizontal, first, count);
				*(range *)(fString + indx) = r;
				
				indx += sizeof(range) / kPFWordSize;
				break;
			}
//			case valXRef:
//				indx += sizeof(XRef) / kPFWordSize;
//				break;
			default:
				// there was a warning about not all enum values handled in
				// switch statement
				break;
		}
	}
	while (theOpcode != opEnd);
} /* CFormula::UpdateReferences */

void CFormula::UpdateReferences(cell inLocation, int h, int v, range src)
{
	if (!fString)
		return;

	PFToken theOpcode;
	int indx = 0, l;
	
	do {
		theOpcode = (PFToken)fString[indx++];

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
			{
				cell c = *(cell *)(fString + indx);
				if (!src.Contains(c.GetFlatCell(inLocation)))
					c.Offset(h, v);
				*(cell *)(fString + indx) = c;
				
				indx += sizeof(cell) / kPFWordSize;
				break;
			}
			case valRange:
			{
				range r = *(range *)(fString + indx);
				if (!src.Contains(r.TopLeft().GetFlatCell(inLocation)) &&
					!src.Contains(r.BotRight().GetFlatCell(inLocation)))
				{
					r.TopLeft().Offset(h, v);
					r.BotRight().Offset(h, v);
				}
				*(range *)(fString + indx) = r;
				
				indx += sizeof(range) / kPFWordSize;
				break;
			}
//			case valXRef:
//				indx += sizeof(XRef) / kPFWordSize;
//				break;
			default:
				// there was a warning about not all enum values handled
				// in switch statement.
				break;
		}
	}
	while (theOpcode != opEnd);
} /* CFormula::UpdateReferences */

void* CFormula::UpdateReferences(cell inLoc, range src, range dst)
{
	if (!fString)
		return NULL;

	PFToken theOpcode;
	int indx = 0, l, v = dst.top - src.top, h = dst.left - src.left;
	void *result = NULL;
	
	do {
		theOpcode = (PFToken)fString[indx++];

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
			{
				cell c = *(cell *)(fString + indx);
				if (src.Contains(c.GetFlatCell(inLoc)))
				{
					if (!result) result = CopyString();
					c.Offset(h, v);
				}
				else if (dst.Contains(c.GetFlatCell(inLoc)))
				{
					if (!result) result = CopyString();
					c = cell::InvalidCell;
				}
				*(cell *)(fString + indx) = c;
				
				indx += sizeof(cell) / kPFWordSize;
				break;
			}
			case valRange:
			{
				range r = *(range *)(fString + indx);
				if (src.Contains(r.TopLeft().GetFlatCell(inLoc)))
				{
					if (!result) result = CopyString();
					r.TopLeft().Offset(h, v);
				}				
				else if (dst.Contains(r.TopLeft().GetFlatCell(inLoc)))
				{
					if (!result) result = CopyString();
					r.TopLeft() = cell::InvalidCell;
				}
			
				if (src.Contains(r.BotRight().GetFlatCell(inLoc)))
				{
					if (!result) result = CopyString();
					r.BotRight().Offset(h, v);
				}
				else if (dst.Contains(r.BotRight().GetFlatCell(inLoc)))
				{
					if (!result) result = CopyString();
					r.BotRight() = cell::InvalidCell;
				}
				*(range *)(fString + indx) = r;
				
				indx += sizeof(range) / kPFWordSize;
				break;
			}
//			case valXRef:
//				indx += sizeof(XRef) / kPFWordSize;
//				break;
			default:
				// there was a warning about not all enum values handled
				// in switch statement.
				break;
		}
	}
	while (theOpcode != opEnd);
	
	return result;
} /* CFormula::UpdateReferences */
