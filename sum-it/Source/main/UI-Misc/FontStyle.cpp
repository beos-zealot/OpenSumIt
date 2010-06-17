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
	FontStyle, a class for caching stringwidths
	
	Copyright 1997, Hekkelman Programmatuur
*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   FONTSTYLE_H
#include "FontStyle.h"
#endif

#ifndef   UTF_SUPPORT_H
#include "utf-support.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   CONFIG_H
#include "Config.h"
#endif


#include <cstring>
#include <algorithm>

#include <Debug.h>
#include <Autolock.h>


const char kDefChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
	"~!@#$%^&*()_+=-[]{}\\|;:<>'\"";

BList CFontStyle::fgFontStyleList;

CFontStyle* CFontStyle::Locate(const char *name, const char *style, float size)
{
	BFont font;
	font.SetFamilyAndStyle(name, style);
	font.SetSize(size);

	for (int i = 0; i < fgFontStyleList.CountItems(); i++)
	{
		CFontStyle *fs = (CFontStyle *)fgFontStyleList.ItemAt(i);
		if (fs->fFont == font)
			return fs;
	}
	
	CFontStyle* result = new CFontStyle(name, style, size);
	fgFontStyleList.AddItem(result);
	
	return result;
} /* CFontStyle::Locate */
	
float CFontStyle::operator[](const char *s)
{
	int unicode = municode(s);
	BAutolock lock(&fLock);
	
	int i = unicode % fEscapementsCount, j = i - 1;
	if (j < 0) j+= fEscapementsCount;
	
	while (true)
	{
		if (fEscapements[i].unicode == unicode)
			return fEscapements[i].escapement;

		if (fEscapements[i].unicode == 0)
		{
			float w[1];
			char buf[10];
			int cl = mcharlen(s);
			
			strncpy(buf, s, cl);
			buf[cl] = 0;

			fFont.GetEscapements(buf, 1, w);
			w[0] *= fFont.Size();

			fEscapements[i].unicode = unicode;
			fEscapements[i].escapement = w[0];
			
			fCharCount++;
			
			if (fCharCount > fEscapementsCount / 2)
				ReHash();
			
			return w[0];
		}

// should never be true
		if (i == j)
		{
			ASSERT(false);
			break;
		}
		
		i = (i + 1) % fEscapementsCount;
	}
	
	ReHash();

	return operator[](s);
} /* CFontStyle::operator[] */
	
CFontStyle::CFontStyle(const char *name, const char *style, float size)
	: fLock("font lock")
{
	fEscapementsCount = 0;
	fEscapements = NULL;
	fCharCount = 0;
	fFont.SetFamilyAndStyle(name, style);
	fFont.SetSize(size);
	InitTable();
} /* CFontStyle::CFontStyle */

CFontStyle::~CFontStyle()
{
	if (fEscapements) FREE(fEscapements);
}

void CFontStyle::InitTable()
{
	int dc = mstrlen(kDefChars);
	float *w;

	fEscapementsCount = 2 * dc;
	fEscapements = (EscapementEntry *)calloc(fEscapementsCount, sizeof(EscapementEntry));
	FailNil(fEscapements);
	
	w = (float *)malloc(dc * sizeof(float));
	FailNil(w);

	fFont.GetEscapements(kDefChars, mstrlen(kDefChars), w);
	
	const char *s = kDefChars;
	
	for (int i = 0; i < dc; i++)
	{
		int cl = mcharlen(s);
		int uc = municode(s);
		int ix = uc % fEscapementsCount;

		while (fEscapements[ix].unicode != 0)
			ix = (ix + 1) % fEscapementsCount;
		
		fEscapements[ix].unicode = uc;
		fEscapements[ix].escapement = w[i] * fFont.Size();
		
		s += cl;
	}
	
	free(w);
} /* CFontStyle::InitTable */

void CFontStyle::ReHash()
{
	int nc = std::max(1, fEscapementsCount * 2);
	EscapementEntry *n = (EscapementEntry *)CALLOC(nc, sizeof(EscapementEntry));
	FailNil(n);
	
	for (int i = 0; i < fEscapementsCount; i++)
	{
		int ni = fEscapements[i].unicode % nc;
		
		while (n[ni].unicode != 0)
			ni = (ni + 1) % nc;
		
		ASSERT(ni >= 0);
		ASSERT(ni < nc);
		
		n[ni].unicode = fEscapements[i].unicode;
		n[ni].escapement = fEscapements[i].escapement;
	}
	
	if (fEscapements) FREE(fEscapements);
	fEscapements = n;
	fEscapementsCount = nc;
} /* CFontStyle::ReHash */

void CFontStyle::PrintToStream()
{
	for (int i = 0; i < fEscapementsCount; i++)
		if (fEscapements[i].unicode)
		{
			printf("entry %d: char %d(%c) = %g\n", i, fEscapements[i].unicode,
				fEscapements[i].unicode, fEscapements[i].escapement);
		}
} /* CFontStyle::PrintToStream */

void CFontStyle::Cleanup()
{
	CFontStyle *fs;
	while ((fs = (CFontStyle *)fgFontStyleList.RemoveItem((int32)0)) != NULL)
		delete fs;
} /* CFontStyle::Cleanup */

float CFontStyle::StringWidth(const char *s, int len) {
	float r = 0;
	
	while (len > 0) {
		r += CharWidth(s);
		int cl = mcharlen(s);
		s += cl;
		len -= cl;
	}
	
	return r;
} /* CFontStyle::StringWidth */
