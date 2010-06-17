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
	CellView.calc.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   PROGRESSVIEW_H
#include "ProgressView.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   FUNCTIONUTILS_H
#include "FunctionUtils.h"
#endif

#ifndef   FUNCTIONS_H
#include "Functions.h"
#endif

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#include <Window.h>
#include <Beep.h>
#include <cstring>

void CCellView::StartCalculation(int inCount)
{
	Window()->Lock();
	fProgressView->Start(inCount);
	Window()->Unlock();
} /* CCellView::StartCalculation */

void CCellView::StepCalculation()
{
	Window()->Lock();
	fProgressView->Step();
	Window()->Unlock();
} /* CCellView::StepCalculation */

void CCellView::StopCalculation()
{
	Window()->Lock();
	fProgressView->Stop();
	Window()->Unlock();
} /* CCellView::StopCalculation */

void CCellView::SumButton()
{
	cell c;
	range r;
	
	c = fCurCell;
	
	bool start = false;
	
	while (!start && c.v > 1)
	{
		c.v--;
		switch (fContainer->GetType(c))
		{
			case eNumData:
				start = true;
				break;
			case eNoData:
				break;
			default:
				c.v = 1;
				break;
		}
	}
	
	if (start)
	{
		r.left = r.right = fCurCell.h;
		r.bottom = fCurCell.v - 1;
		
		while (fContainer->GetType(c) == eNumData && c.v > 0)
			c.v--;
		
		r.top = c.v + 1;
	
		const char *f = gFuncPasteStrings[kSUMFuncNr];	
		char *cp = strchr(f, '(');
		FailNil(cp);

		int start, len;
		start = cp - f + 1;
		len = strlen(f) - start - 1;
		
		fEditBox->MakeFocus();
		fEditBox->EnterString(f, start, len);
		fEditBox->EnterRange(r);
		return;
	}
	
	c = fCurCell;
	
	while (!start && c.h > 1)
	{
		c.h--;
		switch (fContainer->GetType(c))
		{
			case eNumData:
				start = true;
				break;
			case eNoData:
				break;
			default:
				c.h = 1;
				break;
		}
	}
	
	if (start)
	{
		r.top = r.bottom = fCurCell.v;
		r.right = fCurCell.h - 1;
		
		while (fContainer->GetType(c) == eNumData && c.h > 0)
			c.h--;
		
		r.left = c.h + 1;
	
		const char *f = gFuncPasteStrings[kSUMFuncNr];	
		char *cp = strchr(f, '(');
		FailNil(cp);

		int start, len;
		start = cp - f + 1;
		len = strlen(f) - start - 1;
		
		fEditBox->MakeFocus();
		fEditBox->EnterString(f, start, len);
		fEditBox->EnterRange(r);
		return;
	}
	
	beep();
} /* CCellView::SumButton */
