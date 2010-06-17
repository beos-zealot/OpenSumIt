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
	Formula.iter.c
	
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

#ifndef   SET_H
#include "Set.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#include <support/Debug.h>
#include <cstring>

CFormulaIterator::CFormulaIterator(void *inFormula, cell inLocation)
{
	fIndex = 0;
	fString = (long *)inFormula;
	fLocation = inLocation;
} /* CFormulaIterator::CFormulaIterator */

bool CFormulaIterator::Next(cell& ioCell)
{
	int l;
	PFToken theOpcode;

	if (fIndex < 0) { /* fIndex is kleiner dan nul om aan te geven dat
								   we in een range zitten */
		ioCell.h++;
		if (ioCell.h > fRange.right) {
			ioCell.h = fRange.left;
			ioCell.v++;
			if (ioCell.v > fRange.bottom)
				fIndex = -fIndex;
			else
				return true;
		}
		else
			return true;
	}

	do {
		theOpcode = (PFToken)fString[fIndex];
		fIndex++;

		switch(theOpcode) {
			case opFunc:
				fIndex += sizeof(FuncCallData) / kPFWordSize;
				break;
			case valNum:
			case valPerc:
				fIndex += sizeof(double) / kPFWordSize;
				break;
			case valTime:
				fIndex += sizeof(time_t) / kPFWordSize;
				break;
			case valBool:
				fIndex++;
				break;
			case valStr:
			case valName:
				l = 1 + strlen((char *)(fString + fIndex));
				if (l & kPFAlignBits)
					l = (l & ~kPFAlignBits) + kPFWordSize;
				fIndex += l / kPFWordSize;
				break;
			case valCell:
				ioCell = ((cell *)(fString + fIndex))->GetFlatCell(fLocation);
				fIndex += sizeof(cell) / kPFWordSize;
				break;
			case valRange:
				fRange = ((range *)(fString + fIndex))->GetFlatRange(fLocation);
				fIndex += sizeof(range) / kPFWordSize;
				fIndex = -fIndex;
				ioCell = fRange.TopLeft();
				break;
//			case valXRef:
//				fIndex += sizeof(XRef) / kPFWordSize;
//				break;
			default:
				// there was a warning about not all enum values handled in
				// switch statement.
				break;
		}
	}
	while (theOpcode != valCell && theOpcode != valRange && theOpcode != opEnd);

	return (theOpcode == valCell || theOpcode == valRange);
} /* CFormulaIterator::Next */

void CFormulaIterator::SetData(const IterData& inData)
{
	memcpy(this, &inData, sizeof(CFormulaIterator));
} /* CFormulaIterator::SetData */

void CFormulaIterator::GetData(IterData& outData)
{
	memcpy(&outData, this, sizeof(CFormulaIterator));
} /* CFormulaIterator::GetData */
