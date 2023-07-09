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
	FontMetrics.c

	Copyright 1997, Hekkelman Programmatuur

	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   BTREE_T
#include "BTree.t"
#endif

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif

#ifndef   UTF_SUPPORT_H
#include "utf-support.h"
#endif

#ifndef   FONTSTYLE_H
#include "FontStyle.h"
#endif


#include <StLocker.h>

#include <support/Debug.h>

#include <Autolock.h>

CFontSizeTable gFontSizeTable;

CFontMetrics::CFontMetrics()
{
	fFontStyle = NULL;
} // CFontMetrics::CFontMetrics

CFontMetrics::CFontMetrics(const char *inFamily, const char *inStyle,
	float inFontSize, rgb_color inFontColor)
{
	fFont.SetFamilyAndStyle(inFamily, inStyle);
	fFont.GetFamilyAndStyle(&fFamily, &fStyle);
	fFont.SetSize(fSize = inFontSize);

	if (strcmp(fFamily, "<unknown family>") == 0)
	{
		fFont.SetFamilyAndStyle(
			gPrefs->GetPrefString("defdoc font family"),
			inStyle);

		fFont.GetFamilyAndStyle(&fFamily, &fStyle);

		if (strcmp(fStyle, "<unknown style>") == 0)
		{
			fFont.SetFamilyAndStyle(
				gPrefs->GetPrefString("defdoc font family"),
				gPrefs->GetPrefString("defdoc font style"));
			fFont.GetFamilyAndStyle(&fFamily, &fStyle);
		}
	}

	fFontColor = inFontColor;
	fFontStyle = CFontStyle::Locate(fFamily, fStyle, fSize);
} /* CFontMetrics::CFontMetrics */

CFontMetrics::CFontMetrics(const CFontMetrics& m)
{
	fFont = m.fFont;
	fFontStyle = m.fFontStyle;
	strcpy(fFamily, m.fFamily);
	strcpy(fStyle, m.fStyle);
	fFontColor = m.fFontColor;
	fSize = m.fSize;
} // CFontMetrics::CFontMetrics

float CFontMetrics::operator[] (int inChar) const
{
	char s[2] = { 0, 0 };
	s[0] = inChar;

	if (!fFontStyle)
	{
		ASSERT(false);
		return be_plain_font->StringWidth(s);
	}

	return fFontStyle->CharWidth(s);
} /* CFontMetrics::operator[] */

float CFontMetrics::StringWidth(const char *inString) const
{
	int i = 0;
	float result = 0;

	while (inString[i])
	{
		result += fFontStyle->CharWidth(inString + i);
		i += mcharlen(inString + i);
	}

	return result;
} /* CFontMetrics::StringWidth */

void CFontMetrics::SetFontSizeColor(BView *inView)
{
	inView->SetFont(&fFont);
	inView->SetHighColor(fFontColor);
} /* CFontMetrics::SetFontSize */

bool CFontMetrics::operator==(const CFontMetrics& inOther)
{
	if (fFont != inOther.fFont ||
		*(long *)&fFontColor != *(long *)&inOther.fFontColor)
		return false;
	else
		return true;
} /* CFontMetrics::operator== */

//////////////////////////////////////////////////////////
//
// FontSizeTable
//
CFontSizeTable::CFontSizeTable()
{
} /* CFontSizeTable::CFontSizeTable */

uint32 CFontSizeTable::GetFontID(const char *fontName, const char *fontStyle,
		float fontSize, rgb_color fontColor)
{
	StLocker<CFontSizeTable> lock(this);
	std::vector<CFontMetrics>::iterator i;
	CFontMetrics ns(fontName, fontStyle, fontSize, fontColor);

	for (i = fFonts.begin(); i != fFonts.end(); i++)
	{
		if ((*i) == ns)
			return i - fFonts.begin();
	}

	fFonts.push_back(ns);
	return fFonts.size() - 1;
} /* CFontSizeTable::GetFontID */

void CFontSizeTable::SetFontID(BView *view, uint32 formatID,
	CFontMetrics **outMetrics)
{
	BAutolock lock(this);

	if (formatID < 0 || formatID >= fFonts.size())
	{
		ASSERT(false);
		formatID = 0;
	}

	fFonts[formatID].SetFontSizeColor(view);
	if (outMetrics)
		*outMetrics = &fFonts[formatID];
} /* CFontSizeTable::SetFontID */

void CFontSizeTable::GetFontInfo(uint32 formatID,
	font_family *fontName, font_style *fontStyle, float *fontSize,
	rgb_color *fontColor)
{
	CFontMetrics *fm;

	BAutolock lock(this);
	fm = &fFonts[formatID];
	fm->fFont.GetFamilyAndStyle(fontName, fontStyle);
	*fontSize = fm->fFont.Size();
	if (fontColor) *fontColor = fm->fFontColor;
} /* CFontSizeTable::GetFontAndSize */
