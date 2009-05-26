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
	Copyright 1997, Hekkelman Programmatuur
	
*/

#include "CellCommands.h"

class CStyleCommand : public CCellCommand {
public:
	CStyleCommand(CCellView *inView, CContainer *inContainer);
	~CStyleCommand();
	
	virtual void	DoCommand();
	virtual void	UndoCommand();
	virtual void	RedoCommand();

protected:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight) = 0;

	range		fAffected;
	CellStyle	fNewStyle;
	int			*fSavedStyles, fType;
	bool		fRowSelected, fColSelected;
	CRunArray	fSavedHeights;
	CRunArray2	fSavedColumnStyles;
};

class CFontStyleCommand : public CStyleCommand {
public:
	CFontStyleCommand(CCellView *inView, CContainer *inContainer,
		const char *family);

private:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight);
	
	font_family fNewFamily;
};

class CFaceStyleCommand : public CStyleCommand {
public:
	CFaceStyleCommand(CCellView *inView, CContainer *inContainer,
		const char *style);

private:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight);
	
	font_style fNewFace;
};

class CSizeStyleCommand : public CStyleCommand {
public:
	CSizeStyleCommand(CCellView *inView, CContainer *inContainer,
		float newSize);

private:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight);
	
	float fNewSize;
};

class CTextColorStyleCommand : public CStyleCommand {
public:
	CTextColorStyleCommand(CCellView *inView, CContainer *inContainer,
		rgb_color newColor);

private:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight);
	
	rgb_color fNewColor;
};

class CLowColorStyleCommand : public CStyleCommand {
public:
	CLowColorStyleCommand(CCellView *inView, CContainer *inContainer,
		rgb_color newColor);

private:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight);
	
	rgb_color fNewColor;
};

class CAlignmentStyleCommand : public CStyleCommand {
public:
	CAlignmentStyleCommand(CCellView *inView, CContainer *inContainer,
		int newAlignment);

private:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight);
	
	int fNewAlignment;
};

class CFormatStyleCommand : public CStyleCommand {
public:
	CFormatStyleCommand(CCellView *inView, CContainer *inContainer,
		int newFormat);

private:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight);
	
	int fNewFormat;
};

class CTimeFormatStyleCommand : public CStyleCommand {
public:
	CTimeFormatStyleCommand(CCellView *inView, CContainer *inContainer,
		int newFormat);

private:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight);
	
	int fNewFormat;
};

class CDigitsStyleCommand : public CStyleCommand {
public:
	CDigitsStyleCommand(CCellView *inView, CContainer *inContainer,
		int newDigits);

private:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight);
	
	int fNewDigits;
};

class CCommasStyleCommand : public CStyleCommand {
public:
	CCommasStyleCommand(CCellView *inView, CContainer *inContainer,
		bool newCommas);

private:
	virtual void ChangeStyle(CellStyle& ioStyle, float& ioHeight);
	
	bool fNewCommas;
};

