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
	DateFormatDialog.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   DATEFORMATDIALOG_H
#include "DateFormatDialog.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#include <Message.h>

CDateFormatDialog::CDateFormatDialog(BRect frame, const char *name, CCellWindow *owner,
		window_type type, int flags)
	: CRDialog(frame, name, owner, type, flags)
{
} /* CDateFormatDialog::CDateFormatDialog */

void CDateFormatDialog::PostInit(ulong oldFormat)
{
	fView = fOwner->GetCellView();
	
	SetOn("day", oldFormat & 1);
	switch (oldFormat >> 1 & 3)
	{
		case 0:	SetOn("month", false); break;
		case 1:	SetOn("month", true); SetOn("m_nr", true); break;
		case 2:	SetOn("month", true); SetOn("m_short", true); break;
		case 3:	SetOn("month", true); SetOn("m_long", true); break;
	}
	switch (oldFormat >> 3 & 3)
	{
		case 0:	SetOn("year", false); break;
		case 1:	SetOn("year", true); SetOn("y_short", true); break;
		case 2:	SetOn("year", true); SetOn("y_long", true); break;
	}
	SetOn("hour", oldFormat >> 5 & 1);
	SetOn("minute", oldFormat >> 6 & 1);
	SetOn("second", oldFormat >> 7 & 1);

	UpdateFields();
	
	Show();
} /* CDateFormatDialog::PostInit */

void CDateFormatDialog::UpdateFields()
{
	char s[256];
	time_t t;

	time(&t);
	gFormatTable.FormatValue(BuildBitField(), Value(t), s);
	
	SetText("demo", s);

	SetEnabled("m_nr", IsOn("month"));
	SetEnabled("m_short", IsOn("month"));
	SetEnabled("m_long", IsOn("month"));
	SetEnabled("y_short", IsOn("year"));
	SetEnabled("y_long", IsOn("year"));
} /* CDateFormatDialog::UpdateFields */

bool CDateFormatDialog::OKClicked()
{
	BMessage msg(msg_FormatDateTimeChanged);
	msg.AddInt32("newdateformat", BuildBitField());
	fOwner->PostMessage(&msg, fView);

	return true;
} /* CDateFormatDialog::OKClicked */

CDateFormatDialog* CDateFormatDialog::Construct(CCellWindow *owner, ulong oldFormat)
{
	BMallocIO buf;
	BRect frame;
	char name[256];
	window_type type;
	int flags;
	
	InitValues(2, buf, frame, name, type, flags, owner);
	
	CDateFormatDialog* result = new CDateFormatDialog(frame, name, owner, type, flags);
	if (result)
	{
		try
		{
			result->ConstructFromTemplate(buf);
			result->PostInit(oldFormat);
		}
		catch (CErr& e)
		{
			e.DoError();
			delete result;
			result = NULL;
		}
	}
	
	return result;
} /* CDateFormatDialog::Construct */

long CDateFormatDialog::BuildBitField()
{
	long bf = 0;
	
	if (IsOn("day"))				bf |= 1;
	if (IsOn("month"))
	{
		if (IsOn("m_nr"))			bf |= 1 << 1;
		else if (IsOn("m_short"))	bf |= 2 << 1;
		else						bf |= 3 << 1;
	}
	if (IsOn("year"))
	{
		if (IsOn("y_short"))		bf |= 1 << 3;
		else						bf |= 2 << 3;
	}
	if (IsOn("hour"))				bf |= 1 << 5;
	if (IsOn("minute"))			bf |= 1 << 6;
	if (IsOn("second"))			bf |= 1 << 7;
	
	return bf;
} /* CDateFormatDialog::BuildBitField */
