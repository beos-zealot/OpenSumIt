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
	Formula.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef FORMULA_H
#define FORMULA_H

#ifndef DEFINE_CELL
	#ifndef   CELL_H
	#include "Cell.h"
	#endif
	
	#ifndef   RANGE_H
	#include "Range.h"
	#endif
#endif

#include <BeBuild.h>
#include <DataIO.h>
#include <cstddef>

struct Value;
class CContainer;
class CNameTable;
class CSet;
class CName;

enum PFToken {
	opEnd, opRaise, opMul, opDiv, opRoot,
	opPlus, opMinus, opNegate, opLT, opLE,
	opEQ, opGE, opGT, opNE, opFunc,
	valNum, valBool, valStr, valCell, valNil,
	valRange, valPerc, valTime, opNOT, opAND,
	opOR, valName, opParen,
	valXRef
};

const long
	kMaxStackHeight = 25,
	kMaxStringLength = 256,
	kPFWordSize = sizeof(long),
	kPFAlignBits = kPFWordSize - 1;

struct FuncCallData {
	short funcNr;
	short argCnt;
};
//
//struct XRef {
//	int fileNr;
//	cell cell;
//};

class CFormula {
	friend class CFormulaIterator;
public:
	CFormula();
	CFormula(void *inString);
//	~CFormula();

	void AddToken(PFToken inToken, const void *inData, int& ioOffset);
	void Calculate(cell inLocation, Value& outResult, CContainer *inContainer) const;
	void UnMangle(char *outString, cell inLocation, CContainer *inContainer, bool rcStyle = false) const;
	void UpdateReferences(cell inLocation, bool horizontal, int first, int count);
	void UpdateReferences(cell inLocation, int h, int v, range src);
	void* UpdateReferences(cell inLoc, range src, range dst);
	
	bool ReduceToValue(Value& val, CContainer *inContainer) const;
	
	bool RefersToName(const char *name = NULL) const;
	bool GetNextName(int& indx, CName& name) const;
	
	bool IsFormula() const
		{ return fString != NULL; }
	bool IsConstant() const;
	
	void CollectFunctionNrs(CSet& funcs) const;
	
	void Clear();
	void* DetachString();
	void* CopyString() const;
	long StringLength() const;
	void operator=(void *inString);
	
	void Write(BPositionIO& inStream);
	void Read(BPositionIO& inStream, int *inFuncList);
	
	void ReadOldMacFormula(BPositionIO& inStream, cell inLoc);
	
	long operator[](int indx) const
		{ return fString[indx]; };

private:
	long *fString;
};

class CFormulaIterator {
public:
	typedef long IterData[5];

	CFormulaIterator(void*, cell inLocation);
	
	bool Next(cell&);

	void GetData(IterData& outData);
	void SetData(const IterData& inData);

private:
	long *fString;
	int fIndex;
	range fRange;
	cell fLocation;
};

extern bool gWithEqualSign;

#endif
