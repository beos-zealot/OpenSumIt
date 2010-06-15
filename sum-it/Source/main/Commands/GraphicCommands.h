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
	ChartCommands
*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 ***/

#ifndef GRAPHICCOMMANDS_H
#define GRAPHICCOMMANDS_H

class CGraphic;
class CCellView;

#include <Message.h>

class CGraphicCommand : public CCommand {
public:
	CGraphicCommand(int undoStrID, CCellView *view, CGraphic *graphic);
	~CGraphicCommand();

protected:
	CCellView *fCellView;
	CGraphic *fGraphic;
};

class CCutGraphicCommand : public CGraphicCommand {
public:
	CCutGraphicCommand(CCellView *inView);
	~CCutGraphicCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();

protected:
	bool fDeleted;
};

class CCopyGraphicCommand : public CGraphicCommand {
public:
	CCopyGraphicCommand(CCellView *view);
	~CCopyGraphicCommand();
	
	virtual void DoCommand();
};

class CAddGraphicCommand : public CGraphicCommand {
public:
	CAddGraphicCommand(CCellView *view, CGraphic *g);
	~CAddGraphicCommand();

	virtual void DoCommand();
	virtual void UndoCommand();

private:
	bool fAdded;
};

class CDeleteGraphicCommand : public CGraphicCommand {
public:
	CDeleteGraphicCommand(CCellView *view);
	~CDeleteGraphicCommand();
	
	void DoCommand();
	void UndoCommand();

private:
	bool fDeleted;
};

class CMoveGraphicCommand : public CGraphicCommand {
public:
	CMoveGraphicCommand(CCellView *view, cell oldAnchor, BRect oldSize,
		cell newAnchor, BRect newSize);
	
	void DoCommand();
	void UndoCommand();

private:
	cell fOldAnchor, fNewAnchor;
	BRect fOldSize, fNewSize;
};

class CResizeGraphicCommand : public CGraphicCommand {
public:
	CResizeGraphicCommand(CCellView *view, BPoint oldSize, BPoint newSize);
	
	void DoCommand();
	void UndoCommand();

private:
	BPoint	fNewSize, fOldSize;
};

class CChartOptionsCommand : public CGraphicCommand {
public:
	CChartOptionsCommand(CCellView *view, BMessage& newSettings);

	void DoCommand();

private:
	BMessage fSettings;
};
#endif
