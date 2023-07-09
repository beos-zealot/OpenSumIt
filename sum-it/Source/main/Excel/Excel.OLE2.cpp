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
	Excel.OLE2.c

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   EXCEL_H
#include "Excel.h"
#endif

#include <DataIO.h>
#include <cstdio>

const uint32 BLOCKSIZE = 512;

struct oleEntry
{
	short name[32];
	long unknown[13];
	long start;
	long size;
	long unknown2;
};

#if DEBUG
// this should be part of BeOS...

void Write(BPositionIO& dest, BPositionIO& source)
{
	dest.SetSize(0);
	source.Seek(0, SEEK_SET);

	char buffer[32768];
	int l;
	do
	{
		l = source.Read(buffer, 32768);
		if (l > 0)
			dest.Write(buffer, l);
	}
	while (l > 0);
}
#endif

status_t CExcel5Filter::GetBookStream(BPositionIO& stream) throw()
{
	CExcelStream es(stream);
	unsigned long l[128];
	long size = 512;
	int i;

	es.Read(l, size);

	if (l[0] != B_HOST_TO_LENDIAN_INT32(0xE011CFD0)) return B_ERROR;

	long indxAddress, dirAddress;
	indxAddress = (B_LENDIAN_TO_HOST_INT32(l[19]) + 1) * BLOCKSIZE;
	dirAddress = (B_LENDIAN_TO_HOST_INT32(l[12]) + 1) * BLOCKSIZE;

	oleEntry e;
	char e_name[32];

	if (stream.Seek(dirAddress, SEEK_SET) != dirAddress)
		return B_ERROR;
	size = sizeof(oleEntry);

	do
	{
		// lame, must change
		if (stream.Position() > dirAddress + BLOCKSIZE) return B_ERROR;

		if (stream.Read(&e, size) != size)
			return B_ERROR;

		for (i = 0; i < 32; i++)
			e_name[i] = B_LENDIAN_TO_HOST_INT16(e.name[i]);
	}
	while (strcmp(e_name, "Book") && strcmp(e_name, "Workbook"));

	e.size = B_LENDIAN_TO_HOST_INT32(e.size);
	e.start = B_LENDIAN_TO_HOST_INT32(e.start);

	if (stream.Seek(indxAddress, SEEK_SET) != indxAddress)
		return B_ERROR;
	size = 512;
	if (stream.Read(l, size) != size)
		return B_ERROR;

	int indx = e.start;
	do
	{
		char buf[512];
		if (stream.Seek((indx + 1) * BLOCKSIZE, SEEK_SET) != (indx + 1) * BLOCKSIZE)
			return B_ERROR;
		long k = min_c(e.size, (int)BLOCKSIZE);
		if (stream.Read(buf, k) != k)
			return B_ERROR;
		if (fBook.Write(buf, k) != k)
			return B_ERROR;
		e.size -= BLOCKSIZE;
		if (e.size < 0) break;

		indx = B_LENDIAN_TO_HOST_INT32(l[indx]);
	}
	while (e.size > 0 && indx > 0);

#if DEBUG
	BFile dump;
	BDirectory("/tmp").CreateFile("OLE", &dump);
	Write(dump, fBook);
#endif

	fBook.Seek(0, SEEK_SET);
	return B_NO_ERROR;
} /* GetBookStream */
