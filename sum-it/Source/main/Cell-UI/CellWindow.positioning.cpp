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
	CellWindow.positioning.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#include <Screen.h>
#include <algorithm>

static void GetDesktop(BRect &outRect)
{
	BScreen screen;
	outRect = screen.Frame();
	outRect.InsetBy(4, 4);
} /* DesktopSpace */

void CCellWindow::Stack()
{
	BRect r;
	int i, c;
	
	c = 1 << current_workspace();
	
	GetDesktop(r);
	
	r.top += 15;
	r.InsetBy(10, 5);
	
	for (i = 0; i < sWindowList.CountItems(); i++)
	{
		CCellWindow *w = (CCellWindow *)sWindowList.ItemAt(i);
		
		w->SetWorkspaces(c);

		w->MoveTo(r.left, r.top);
		w->ResizeTo(r.right - r.left, r.bottom - r.top);
		
		r.top += 20;
		r.left += 5;
		
		w->Activate(true);
	}
} /* CCellWindow::Stack */

void CCellWindow::Tile()
{
	BRect r;
	int i, cnt, ws, ch, cv, ww, wh;
	
	ws = 1 << current_workspace();
	
	GetDesktop(r);
	r.InsetBy(5.0, 5.0);
	
	cnt = sWindowList.CountItems();
	
	ch = cnt;
	cv = 1;
	
	while (cv < ch)
	{
		cv++;
		if (cnt % cv)
			ch = (cnt / cv) + 1;
		else
			ch = (cnt / cv);
	}
	
	ww = static_cast<int>(r.Width() / ch);
	wh = static_cast<int>(r.Height() / cv);
	
	for (i = 0; i < sWindowList.CountItems(); i++)
	{
		CCellWindow *w = (CCellWindow *)sWindowList.ItemAt(i);
		w->SetWorkspaces(ws);
		
		w->MoveTo(r.left + (i % ch) * ww, 20.0 + r.top + (i / ch) * wh);
		w->ResizeTo(ww - 8.0, wh - 15.0);
		
		w->Activate();
	}
} /* CCellWindow::Tile */

void CCellWindow::GetMaxWindowRect(BRect *outRect, range *inRect)
{
	GetDesktop(*outRect);
	
	if (inRect)
	{
		outRect->left = max(outRect->left,     static_cast<float>(inRect->left) );
		outRect->top = max(outRect->top,       static_cast<float>(inRect->top) );
		outRect->right = min(outRect->right,   static_cast<float>(inRect->right) );
		outRect->bottom = min(outRect->bottom, static_cast<float>(inRect->bottom) );
		
		if (!outRect->IsValid())
			GetMaxWindowRect(outRect, NULL);
	}
} /* CCellWindow::GetMaxWindowRect */
