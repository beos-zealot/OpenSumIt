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
	CellScrollBar.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/


#include "CellScrollBar.h"
#include "CellView.h"

CCellScrollBar::CCellScrollBar(BRect frame, const char *name, BView *target,
	long min, long max, orientation posture)
	: BScrollBar(frame, name, target, min, max, posture)
{
	
} /* CCellScrollBar::CCellScrollBar */

void CCellScrollBar::ValueChanged(float newValue)
{
	CCellView *target = (CCellView *)Target();
	long x = 0, y = 0;
	
	if (Orientation() == B_HORIZONTAL)
		x = (int)newValue;
	else
		y = (int)newValue;
	
	target->ScrollToPosition(x, y);
} /* CCellScrollBar::ValueChanged */

//long CCellScrollBar::GetMaxRC()
//{
//	float min, max, r;
////	CCellView *target = (CCellView *)Target();
////	
//	GetRange(&min, &max);
//	r = max;
////	
////	if (Orientation() == B_HORIZONTAL)
////	{
////		r = kColCount;
////		while (target->GetWidths()[r] > max)
////			r--;
////	}
////	else
////	{
////		r = kRowCount;
////		while (target->GetHeights()[r] > max)
////			r--;
////	}
//	
//	return r;
//} /* CCellScrollBar::GetMaxRC */
