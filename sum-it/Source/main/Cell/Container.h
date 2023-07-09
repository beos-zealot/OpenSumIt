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
	Container.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef CONTAINER_H
#define CONTAINER_H

/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#include <map>

#ifndef   CELL_H
#include "Cell.h"
#endif

#ifndef   RANGE_H
#include "Range.h"
#endif

#ifndef   RUNARRAY2_H
#include "RunArray2.h"
#endif

#ifndef   CELLDATA_H
#include "CellData.h"
#endif

#include <Locker.h>

class CCellView;
class CCellIterator;
class CFormula;
class CNameTable;
class CCalculateJob;
struct CellStyle;
class CSet;
class CFormula;

enum SplitType {
	noSplit, hSplit, vSplit
};

typedef std::map<cell,CellData> cellmap;
//typedef btree<cell,CellData> cellmap;

class CContainer : public BLocker {
	friend class CCellIterator;
	friend class CCalculateJob;

	virtual ~CContainer();

public:
	CContainer(CCellView *inPane = NULL, CNameTable *inNames = NULL);

	void Reference();
	void Release();

	virtual bool Lock();
	virtual void Unlock();
	
	bool WriteLock();
	void WriteUnlock();

	long GetCellCount()			{ return fCellData.size(); }
	void GetBounds(range& r);

/* cell aanmaak en verwijder routines */

	void NewCell(const cell& inLocation, const Value& inValue, void *inFormula);
	void DisposeCell(const cell& c);
	void CopyCell(CContainer *destContainer, const cell& srcLoc, const cell& destLoc,
			range *inFrom = NULL, bool isDragMove = false);
	void MoveCell(CContainer *destContainer, const cell& srcLoc, const cell& destLoc,
			SplitType split = noSplit, int first = 0, int count = 0);
	void ExchangeCells(const cell& a, const cell& b);

/* Celllijst manipulaties */

	bool GetNextCellInRow(cell&, bool mayBeEmpty = false);
	bool GetPreviousCellInRow(cell&, bool mayBeEmpty = false);
	bool GetNextCell(cell&, bool mayBeEmpty = false);
	bool GetPreviousCell(cell&, bool mayBeEmpty = false);

/* cell inhoud manipulaties */

	bool CalcCell(const cell&);
	int GetCellResult(const cell&, char *, bool);
	void GetCellFormula(const cell&, char *, bool rcStyle = false);
	void* GetCellFormula(const cell&);
	void SetCellFormula(const cell& inLoc, void *inFormula);
	
	void GetCellStyle(const cell&, CellStyle&);
	void SetCellStyle(const cell&, CellStyle&);
	int GetCellStyleNr(const cell&);
	void SetCellStyleNr(const cell&, int);

	void GetColumnStyle(int, CellStyle&);
	void SetColumnStyle(int, CellStyle&);
	int GetColumnStyleNr(int);
	void SetColumnStyleNr(int, int);
	
	void GetDefaultCellStyle(CellStyle&);
	void SetDefaultCellStyle(CellStyle&);
	bool GetValue(const cell&, Value&);
	void SetValue(const cell&, const Value&);
	int GetType(const cell&);
	double GetDouble(const cell&);
	int CompareValue(const cell&, const cell&);
	int GetCellDepth(const cell&);
	void SetCellDepth(const cell&, int);
	bool CellIsConstant(const cell&);
	bool CellHasFormula(const cell&);
	bool RefersToNamedRange(const char *inName);
	void CollectFunctionNrs(CSet& funcs);
	int CollectFontList(int *fontList);
	int CollectFormats(int *formatList);
	int CollectStyles(int *styleList);

/* accessors */
	CCellView* GetOwner() const			{ return fInView; };
	CRunArray2& GetColumnStyles()		{ return fColumnStyles; }
	cell CalculatingCell() const		{ return fCalculatingCell; }
	CNameTable *GetNameTable() const	{ return fNames; }
	
	range ResolveName(const char *name);
	
	long CountCells(range *inRange = NULL);
	bool Exists(const cell& c);
	
private:
	void Visit(const cell&, void*);
	bool GetCellData(const cell&, CellData&);
	
	long DoCalculate();
	static long Calculate(void *inData);
	
/* en de fields */
	BLocker fWriteLocker;
	int32 fReferenceCount;
	cellmap fCellData;
	CCellView *fInView;
	CNameTable *fNames;
	bool fNewNames;
	int fDefaultCellStyle;
	CRunArray2 fColumnStyles;
	cell fCalculatingCell;
};

inline bool CContainer::WriteLock()
{
	return fWriteLocker.Lock();
}

inline void CContainer::WriteUnlock()
{
	fWriteLocker.Unlock();
}

class StWriteLock {
public:
	StWriteLock(CContainer *container)
		{ fContainer = container; container->WriteLock(); };
	~StWriteLock()
		{ fContainer->WriteUnlock(); };
private:
	CContainer *fContainer;
};

#endif
