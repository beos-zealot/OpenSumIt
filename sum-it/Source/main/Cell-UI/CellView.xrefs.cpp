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
	CellView.xrefs.c
	
	Copyright 1997, Hekkelman Programmatuur
*/

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   SUM_IT_H
#include "Sum-It.h"
#endif

#ifndef   ERRMSG_H
#include "errmsg.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif


#include <Entry.h>
#include <Directory.h>

int CCellView::MakeXRef(const char *fileName)
{
	CCellWindow *w = (CCellWindow *)Window();
	BEntry e(w->EntryRef());
	BDirectory d;
	
	if (e.InitCheck() || !e.IsDirectory())
		d = *gCWD;
	else
		e.GetParent(&d);
	
	if (d.FindEntry(fileName, &e) == B_NO_ERROR)
	{
		int i;
		entry_ref xref;
		e.GetRef(&xref);
		
		for (i = 0; i < fXRefs.CountItems(); i++)
		{
			if (xref == *(entry_ref *)fXRefs.ItemAt(i))
				return i;
		}
		
		fXRefs.AddItem(new entry_ref(xref));
		gApp->OpenWindow(xref);
	}
	else
		THROW((errMissingXRef, fileName));

	return fXRefs.CountItems() - 1;
} /* CCellView::MakeXRef */

void CCellView::GetXRefDesc(int ref, cell c, char *desc)
{
	entry_ref *xref = (entry_ref *)fXRefs.ItemAt(ref);
	if (!xref) THROW((errIllegalXRefNr));

	char s[32];
	c.GetFormulaName(s, cell());

	sprintf(desc, "\"%s\"!%s", xref->name, s);
} /* CCellView::GetXRefDesc */

void CCellView::ResolveXRef(int ref, cell c, Value& val)
{
	entry_ref *xref = (entry_ref *)fXRefs.ItemAt(ref);
	if (!xref) THROW((errIllegalXRefNr));

	CCellWindow *w = CCellWindow::FindWindow(*xref);
	if (w)
		w->GetCellView()->GetContainer()->GetValue(c, val);
	else
		val = gRefNan;
} /* CCellView::ResolveXRef */
