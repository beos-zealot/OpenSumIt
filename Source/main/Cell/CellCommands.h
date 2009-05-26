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
	CellCommands.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef CELLCOMMANDS_H
#define CELLCOMMANDS_H

#include "CellStyle.h"
#include "Command.h"
#include "Range.h"
#include "RunArray.h"

class CCellView;
class CContainer;

class CCellCommand : public CCommand {
public:
	CCellCommand(int inStrID, CCellView *inView, CContainer *inCells,	
		bool inUndoable = true);
	
	virtual void	UpdateView(bool redrawAll = false, bool recalculate = true);
	
protected:
	CCellView		*fCellView;
	CContainer		*fSourceContainer;
};

class CFillDownCommand : public CCellCommand {
public:
					CFillDownCommand(CCellView *inView, CContainer *inContainer);
					~CFillDownCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();
	virtual void	RedoCommand();

private:
	CContainer		*fSavedCells;
	range			fAffected;
};

class CFillRightCommand : public CCellCommand {
public:
					CFillRightCommand(CCellView *inView, CContainer *inContainer);
					~CFillRightCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();
	virtual void	RedoCommand();

private:
	CContainer		*fSavedCells;
	range			fAffected;
};

class CEnterCellCommand : public CCellCommand {
public:
					CEnterCellCommand(CCellView *inView, CContainer *inContainer,
						const char *inFormula, cell inCell);
					~CEnterCellCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();

private:
	cell			fCell;
	char			*fFormula;
	CellStyle		fStyle;
	int			fType;
};

class CEraseCommand : public CCellCommand {
public:
	CEraseCommand(CCellView *inView, CContainer *inContainer);
	~CEraseCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();
	virtual void	RedoCommand();

private:
	range			fAffected;
	CContainer	*fSavedCells;
};

class CInsertCommand : public CCellCommand {
public:
					CInsertCommand(CCellView *inView, CContainer *inContainer);
					~CInsertCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();
	virtual void	RedoCommand();

private:
	range			fAffected;
	CRunArray		fSavedWidths;
	int			fNumOfCols, fNumOfRows;
	bool			fDoColumns;
};

class CDeleteCommand : public CCellCommand {
public:
					CDeleteCommand(CCellView *inView, CContainer *inContainer);
					~CDeleteCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();
	virtual void	RedoCommand();

private:
	CContainer		*fSavedCells;
	range			fAffected;
	CRunArray		fSavedWidths;
	int			fNumOfCols, fNumOfRows;
	bool			fDoColumns;
};

typedef enum {
	chFont,
	chSize,
	chStyle,
	chFontColor,
	chCellColor,
	chAddStyle,
	chDelStyle,
	chLowColor,
	chFormat,
	chTimeFormat,
	chDigits,
	chCommas,
	chAlignment
} EStyleCmd;


#endif
