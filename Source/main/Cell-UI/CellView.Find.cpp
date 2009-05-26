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
	CellView.Find.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
#endif

#ifndef   FINDCOMMANDS_H
#include "FindCommands.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   CONFIG_H
#include "Config.h"
#endif


#include <ctype.h>
#include <Debug.h>

#include <Message.h>
#include <Autolock.h>
#include <Beep.h>

void initskip(uchar *p, int skip[], bool ignoreCase);
int mismatchcasesearch(const uchar *p, const uchar *a, int skip[]);
int mismatchsearch(const uchar *p, const uchar *a, int skip[]);


void CCellView::Find(BMessage *msg)
{
	const char *s;
	if (msg->FindString("what", &s) == B_NO_ERROR)
	{
		if (fWhatToFind) FREE(fWhatToFind);
		fWhatToFind = STRDUP(s);
		FailNil(fWhatToFind);
	}

	if (msg->FindString("with", &s) == B_NO_ERROR)
	{
		if (fReplaceWith) FREE(fReplaceWith);
		fReplaceWith = STRDUP(s);
		FailNil(fReplaceWith);
	}
	
	bool b;
	if (msg->FindBool("where", &b) == B_NO_ERROR) fFindInFormulas = b;
	if (msg->FindBool("wrap", &b) == B_NO_ERROR) fWrap = b;
	if (msg->FindBool("case", &b) == B_NO_ERROR) fIgnoreCase = b;
	
	CCellWindow *w = (CCellWindow *)Window();
	w->Activate(true);
	
	switch (msg->what)
	{
		case msg_Find:
			FindNext(fWrap);
			break;
			
		case msg_Replace:
			if (CanReplace(fWhatToFind, fIgnoreCase))
				w->RegisterCommand(new CReplaceCommand(this, fContainer, 
					fWhatToFind, fFOffset, fReplaceWith, false, fWrap));
			break;
		
		case msg_ReplaceAndFind:
			if (CanReplace(fWhatToFind, fIgnoreCase))
				w->RegisterCommand(new CReplaceCommand(this, fContainer, 
					fWhatToFind, fFOffset, fReplaceWith, true, fWrap));
			break;
		
		case msg_ReplaceAll:
			w->RegisterCommand(new CReplaceAllCommand(this, fContainer,
				fWhatToFind, fReplaceWith));
			break;
	}
} /* CCellView::Find */

bool CCellView::CanReplace(const char *what, bool ignoreCase)
{
	if (!what || !fFindInFormulas) return false;
	
	char s[256];
	
	BAutolock lock(fContainer);
	fContainer->GetCellFormula(fCurCell, s);

	if (*s)
	{
		if (ignoreCase)
			return strncasecmp(what, s + fFOffset, strlen(what)) == 0;
		else
			return strncmp(what, s + fFOffset, strlen(what)) == 0;
	}
	
	return false;
} /* CCellView::CanReplace */

void initskip(uchar *p, int skip[], bool ignoreCase)
{
	int M = strlen((char *)p), i;
	
	for (i = 0; i < 255; i++)
		skip[i] = M;
	
	if (!ignoreCase)
	{
		for (i = 0; i < M; i++)
			skip[p[i]] = M - i - 1;
	}
	else
	{
		for (i = 0; i < M; i++)
		{
			skip[tolower(p[i])] = M - i - 1;
			skip[toupper(p[i])] = M - i - 1;
			p[i] = toupper(p[i]);
		}
	}
}

int mismatchcasesearch(const uchar *p, const uchar *a, int skip[])
{
	ASSERT(p);
	ASSERT(a);
	ASSERT(skip);
	int i, j, t, M = strlen((char *)p), N = strlen((char *)a);

	for (i = M - 1, j = M - 1; j >= 0; i--, j--)
		while (toupper(a[i]) != p[j])
		{
			t = skip[a[i]];
			i += (M - j > t) ? M - j : t;
			if (i >= N)
				return N;
			j = M - 1;
		}
	return i;
}

int mismatchsearch(const uchar *p, const uchar *a, int skip[])
{
	ASSERT(p);
	ASSERT(a);
	ASSERT(skip);
	int i, j, t, M = strlen((char *)p), N = strlen((char *)a);

	for (i = M - 1, j = M - 1; j >= 0; i--, j--)
		while (a[i] != p[j])
		{
			t = skip[a[i]];
			i += (M - j > t) ? M - j : t;
			if (i >= N)
				return N;
			j = M - 1;
		}
	return i;
}

bool CCellView::FindNext(bool wrap)
{
	int skip[256];
	cell first, next;
	bool wrapped = false;
	char s[256];
	
	BAutolock lock(fContainer);

	if (fContainer->GetCellCount() == 0)
	{
		beep();
		return false;
	}
	
	initskip((uchar *)fWhatToFind, skip, fIgnoreCase);
	first = next = fCurCell;
	fFOffset++;
	
	while (true)
	{
		if (fFindInFormulas)
			fContainer->GetCellFormula(next, s);
		else
			fContainer->GetCellResult(next, s, true);

		if (*s)
		{
			unsigned int offset = fIgnoreCase ?
				mismatchcasesearch((uchar *)fWhatToFind, (uchar *)s + fFOffset, skip) :
				mismatchsearch((uchar *)fWhatToFind, (uchar *)s + fFOffset, skip);

			offset += fFOffset + 1;
			
			if (offset < strlen(s))
			{
				SetSelection(next);
				fFOffset = offset;
				return true;
			}
		}

		fFOffset = 0;

		if (!fContainer->GetNextCell(next))
		{
			if (wrap && !wrapped)
			{
				next = cell(0, 0);
				fFOffset = -1;
				wrapped = true;
				continue;
			}
			else
				break;
		}

		if (first <= next && wrapped)
			break;
	}
	
	beep();
	return false;
} /* CCellView::FindNext */
