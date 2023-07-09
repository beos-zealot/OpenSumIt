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
/*	$Id: Excel.cpp,v 1.1.1.1 2000/03/05 06:41:15 tpv Exp $

	Copyright Hekkelman Programmatuur b.v.
	Maarten L. Hekkelman

	Created: 11/27/98 13:26:27
*/

#ifndef   EXCEL_H
#include "Excel.h"
#endif

#ifndef   FILEFORMAT_H
#include "FileFormat.h"
#endif

#ifndef   FORMULA_H
#include "Formula.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   FUNCTIONS_H
#include "Functions.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif


#include <stdarg.h>
#include <cstdio>

CExcel5Filter::CExcel5Filter(BPositionIO& inStream, CCellView *cellView, CContainer *container)
	: fCellView(cellView), fContainer(container)
{
	f1904 = false;
	fNewFunctionNr = kFunctionCount;

	inStream.Seek(0, SEEK_SET);
	FailOSErr(GetBookStream(inStream), "Failed to open Excel File (err: %d)");
} // CExcel5Filter::CExcel5Filter

CExcel5Filter::~CExcel5Filter()
{
	std::vector<XLSHFormula>::iterator fi;
	for (fi = fSharedFormulas.begin(); fi != fSharedFormulas.end(); fi++)
		free((*fi).p);

	std::vector<xlName>::iterator ni;
	for (ni = fNames.begin(); ni != fNames.end(); ni++)
	{
		if ((*ni).type == xnNamedReference)
			free((*ni).namedRef);
	}
} // CExcel5Filter::~CExcel5Filter

//void CExcel5Filter::WriteChunk(short chunkType, ushort chunkSize,
//	const void* data)
//{
//	long l = 2;
//
//	chunkType = B_HOST_TO_BENDIAN_INT16(chunkType);
//	chunkSize = B_HOST_TO_BENDIAN_INT16(chunkSize);
//
//	fOutStream->Write(&chunkType, l);
//	fOutStream->Write(&chunkSize, l);
//	if (data)
//		fOutStream->Write(data, chunkSize);
//} // CExcel5Filter::WriteChunk
//
//const int kBufSize = 2080;
//
//void CExcel5Filter::WriteNumber(scCell& cl)
//{
//	do
//	{
//		scStyle *s = (scStyle *)fStyles.ItemAt(cl.style);
//		if (!s) break;
//		scFormat *fm = (scFormat *)fFormats.ItemAt(s->format);
//		if (!fm) break;
//		if (fm->nr < 14 + 2048 || fm->nr > 22 + 2048) break;
//
//		// number is probably a date
//
//		double d;
//		memcpy(&d, cl.num, sizeof(double));
//
//		if (f1904)
//			d -= 24106;
//		else
//			d -= 25569;
//
//		time_t t = d * 24 * 60 * 60;
//		memcpy(cl.num, &t, sizeof(time_t));
//
//		WriteChunk(kscCellDateTime, kscCellSize + sizeof(time_t), &cl);
//		return;
//	}
//	while (false);
//
//	WriteChunk(kscCellNumber, kscCellSize + sizeof(double), &cl);
//} // WriteNumber
//
//void CExcel5Filter::WriteHeader()
//{
//// Write the version number
//	scVersion vers;
//	vers.major = 3;
//	vers.minor = 0;
//	WriteChunk(kscVersion, sizeof(vers), &vers);
//
//	fHeaderInfo.functionCount = fFunctions.CountItems() + kFunctionCount;
//	WriteChunk(kscHeader, sizeof(scHeader), &fHeaderInfo);
//
//// Then there are the functions used in this document
//	for (int i = 0; i < fFunctions.CountItems(); i++)
//		WriteChunk(kscFunc, sizeof(scFunc), fFunctions.ItemAt(i));
//
//// Followed by the formatting information. Fonts first
//	for (int i = 0; i < fFonts.CountItems(); i++)
//	{
//		scFont *f = (scFont *)fFonts.ItemAt(i);
//		char *fs, *fn;
//		fs = (char *)f + sizeof(scFont);
//		fn = fs + strlen(fs) + 1;
//		uint32 size = sizeof(scFont) + strlen(fs) + strlen(fn) + 2;
//		WriteChunk(kscFont, size, fFonts.ItemAt(i));
//	}
//
//// Then we get the number formats
//	for (int i = 0; i < fFormats.CountItems(); i++)
//	{
//		scFormat *f = (scFormat *)fFormats.ItemAt(i);
//		WriteChunk(kscFormat, sizeof(scFormat) + strlen(f->formatString),
//			fFormats.ItemAt(i));
//	}
//
//// The style table
//	for (int i = 0; i < fStyles.CountItems(); i++)
//		WriteChunk(kscStyle, sizeof(scStyle), fStyles.ItemAt(i));
//
//// The Names
//	for (int i = 0; i < fNames.CountItems(); i++)
//	{
//		xlName *n = (xlName *)fNames.ItemAt(i);
//		if (n->type == xnNamedReference)
//			WriteChunk(kscName,
//				n->namedRef->reference[0] == valCell ? 38 : 42,
//				n->namedRef);
//	}
//
//// The only view we have extracted
//	WriteChunk(kscView, sizeof(scView), &fViewInfo);
//
//// Write the widths of the last view
//	int size = fCellWidths.Count() * sizeof(short) * 2;
//	void *p = malloc(size);
//	if (!p) throw CErr("Insufficient memory");
//	fCellWidths.Write(p);
//	WriteChunk(kscWidths, size, p);
//	free(p);
//
//// And the heights of course
//	size = fCellHeights.Count() * sizeof(short) * 2;
//	p = malloc(size);
//	if (!p) throw CErr("Insufficient memory");
//	fCellHeights.Write(p);
//	WriteChunk(kscHeights, size, p);
//	free(p);
//
//// Then write the styles for the columns
//	size = fColStyles.Count() * sizeof(short) * 2;
//	p = malloc(size);
//	if (!p) throw CErr("Insufficient memory");
//	scCSElement *sp = (scCSElement *)p;
//	fColStyles.Write(p);
//	WriteChunk(kscColStyles, size, p);
//	free(p);
//} // CExcel5Filter::WriteHeader

void CExcel5Filter::Translate()
{
	StWriteLock lock(fContainer);
	Pass1();
	fBook.Seek(0, SEEK_SET);
	Pass2();
} // CExcel5Filter::Translate
