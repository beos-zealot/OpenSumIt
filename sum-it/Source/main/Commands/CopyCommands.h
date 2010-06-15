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
	CopyCommands.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

class CCutCommand : public CCellCommand {
public:
					CCutCommand(CCellView *inView, CContainer *inCells);
					~CCutCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();
	virtual void	RedoCommand();

private:
	CContainer		*fSavedCells;
	range			fAffected;
};

class CCopyCommand : public CCellCommand {
public:
					CCopyCommand(CCellView *inView, CContainer *inCells);
	
	virtual void	DoCommand();

private:
	range			fAffected;
};

class CClearCommand : public CCellCommand {
public:
					CClearCommand(CCellView *inView, CContainer *inCells);
					~CClearCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();
	virtual void	RedoCommand();

private:
	CContainer	*fSavedCells;
	range			fAffected;
};

enum PasteEnums {
	pValues, pFormulas, pReferences,
	pOverwrite, pAdd, pSubtract, pMultiply, pDivide
};

class CPasteCommand : public CCellCommand {
public:
					CPasteCommand(CCellView *inView, CContainer *inCells,
						bool inPasteFormat = true, bool inPasteContent = true,
						bool inFillSelection = false, bool inTranspose = false,
						bool inOriginalRefs = false, PasteEnums inPasteWhat = pFormulas,
						PasteEnums inPasteHow = pOverwrite);
					~CPasteCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();
	virtual void	RedoCommand();
	
protected:
	CContainer	*fSavedCells, *fClipboard;
	range			fAffected, fDestination, fClipRect;
	bool			fPasteFormat, fPasteContent, fFillSelection, fTranspose, fOriginalRefs;
	PasteEnums	fPasteWhat, fPasteHow;
};
