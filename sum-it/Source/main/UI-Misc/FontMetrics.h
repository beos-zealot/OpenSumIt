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
	FontMetrics.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef FONTMETRICS_H
#define FONTMETRICS_H

#ifndef   GLOBALS_H
#include "Globals.h"
#endif

#include <vector>

#include <Locker.h>
#include <Font.h>


class CFontSizeTable;
class CFontStyle;

class CFontMetrics {
	friend class CFontSizeTable;
public:
	CFontMetrics();
	CFontMetrics(const char *inFamily, const char *inStyle,
			   float inSize, const rgb_color inColor);
	CFontMetrics(const CFontMetrics& m);

	float StringWidth(const char *inString) const;
	float operator[] (int inChar) const;
	void SetFontSizeColor(BView *inView);

	bool operator==(const CFontMetrics& inOther);

	const BFont& Font() const	{ return fFont; };
	rgb_color FontColor() const		{ return fFontColor; };
	const char *Family() const		{ return fFamily; };
	const char *Style() const 		{ return fStyle; };
	float Size() const				{ return fSize; };
	
private:
	BFont fFont;
	CFontStyle *fFontStyle;
	font_family fFamily;
	font_style fStyle;
	rgb_color fFontColor;
	float fSize;
};

// FontSizeTable

class CFontSizeTable: public BLocker {
	friend class CFontMetrics;
public:	
	CFontSizeTable();
	
	ulong GetFontID(const char *fontName, const char *fontStyle,
		float fontSize, rgb_color fontColor = kBlack);
	void SetFontID(BView *view, ulong formatID,
		CFontMetrics **metrics = NULL);
	void GetFontInfo(ulong formatID, font_family *familyName,
		font_style *familyStyle, float *size, rgb_color *fontColor = NULL);
	
	const CFontMetrics& operator[](int indx)
		{ return fFonts[indx]; };

	unsigned long Count() const
		{ return fFonts.size(); };

private:
	std::vector<CFontMetrics> fFonts;
};

extern CFontSizeTable gFontSizeTable;

#endif
