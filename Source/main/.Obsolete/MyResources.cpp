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
	MyResources.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 *** TPV (2000-Feb-10) Various memory fixes
 ***/

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   MYRESOURCES_H
#include "MyResources.h"
#endif

#ifndef   CONFIG_H
#include "Config.h"
#endif

#include <cstdlib>

#include <File.h>
#include <Resources.h>

// types en constantes
typedef struct {
	void *resData;
#if DEBUG
	ulong marker;
#endif
	ulong type;
	ulong resLength;
	char* name;
	int id;
} ResInfo;

const int kMaxRes = 128;

// globals:
static ResInfo *gResTable = NULL;
static BResources *gResFile = NULL;

void OpenResFile(const char *name)
{
	if (gResTable == NULL)
		gResTable = (ResInfo *)CALLOC(sizeof(ResInfo), kMaxRes);
	FailNil(gResTable);

//	BFile *f = new BFile();
	BFile f ;
//	FailNil(f);
	
	status_t err;
	
	err = f.SetTo(name, B_READ_ONLY);
	if (err)
		THROW((errOpeningResFile, name));
	
	BResources *r = new BResources();
	FailNil(r);
	err = r->SetTo(&f);
	if (err)
		THROW((errOpeningResFile, name));
	
	gResFile = r;
} /* OpenResFile */

void CloseResFile(int /*id*/)
{
	for (int i = 0; i < kMaxRes && gResTable[i].type != 0; i++)
		FREE (gResTable[i].resData);
	FREE(gResTable);
	gResTable = NULL;

	if (gResFile)
		delete gResFile;
	gResFile = NULL;
}

void *GetResource(ulong type, int id)
{
	int i;
	
	for (i = 0; i < kMaxRes && gResTable[i].type != 0; i++)
		if (gResTable[i].type == type && gResTable[i].id == id)
			return &gResTable[i].resData;	// geef 'Handle' terug
	
	if (i < kMaxRes)
	{
		gResTable[i].resData =
			gResFile->FindResource(type, id, &gResTable[i].resLength);
		if (gResTable[i].resData)
		{
#if DEBUG
			gResTable[i].marker = 'Rsrc';
#endif
			gResTable[i].type = type;
			gResTable[i].id = id;
			gResTable[i].name = NULL;
			return &gResTable[i].resData;
		}
	}

	return NULL;
}

size_t GetHandleSize(void* h)
{
	return ((ResInfo *)h)->resLength;
} /* GetHandleSize */

void *GetCursor(int id)
{
	void **c = (void **)GetResource('CURS', id);
	if (!c) THROW((errResNotFound));
	return *c;
} /* GetCursor */

void *RecoverHandle(void *p)
{
	int i;

	for (i = 0; i < kMaxRes && gResTable[i].type != 0; i++)
		if (gResTable[i].resData == p)
			return &gResTable[i].resData;	// geef 'Handle' terug

	return NULL;
}
