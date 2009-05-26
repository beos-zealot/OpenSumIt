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
	FillDialog.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

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

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   CELLCOMMANDS_H
#include "CellCommands.h"
#endif

#ifndef   FILLDIALOG_H
#include "FillDialog.h"
#endif

#ifndef   FILLCOMMAND_H
#include "FillCommand.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#ifndef   RES5_H
#include "res5.h"
#endif

#ifndef   CELLPARSER_H
#include "CellParser.h"
#endif


#include <Autolock.h>
#include <TextControl.h>
#include <Message.h>

CFillDialog::CFillDialog(BRect frame, const char *name, CCellWindow *owner,
		window_type type, int flags)
	: CRDialog(frame, name, owner, type, flags)
{
} /* CFillDialog::CFillDialog */

void CFillDialog::PostInit(CCellWindow *inWindow)
{
	BAutolock lock(this);

	fView = inWindow->GetCellView();
	fCells = fView->GetContainer();

	fView->GetSelection(fRange);

	BAutolock clock(fCells);
	Value start, next;
	fCells->GetValue(fRange.TopLeft(), start);
	
	if (start.fType == eNumData)
	{
		fStart = start.fDouble;
		fFillType = eNumData;
	}
	else if (start.fType == eTimeData)
	{
		fStart = start.fTime;
		fFillType = eTimeData;
	}
	else if (start.fType == eNoData)
	{
		fStart = 0.0;
		fFillType = eNumData;
	}
	else
		THROW((errOnlyNrsOrDates));
	
	fSteps = (fRange.bottom - fRange.top + 1) * (fRange.right - fRange.left + 1) - 1;
	fIncrement = 0;

	cell c = fRange.TopLeft();
	if (fRange.left == fRange.right)
	{
		c.v++;
		fCells->GetValue(c, next);
	}
	else
	{
		c.h++;
		fCells->GetValue(c, next);
	}
	
	if (next.fType == fFillType)
	{
		if (fFillType == eNumData)
			fIncrement = next.fDouble - start.fDouble;
		else if (fFillType == eTimeData)
		{
			struct tm one, two;
			one = *gmtime(&start.fTime);
			two = *gmtime(&next.fTime);
			fTimeInc.tm_sec	= two.tm_sec - one.tm_sec;
			fTimeInc.tm_min	= two.tm_min - one.tm_min;
			fTimeInc.tm_hour	= two.tm_hour - one.tm_hour;
			fTimeInc.tm_mday	= two.tm_mday - one.tm_mday;
			fTimeInc.tm_mon	= two.tm_mon - one.tm_mon;
			fTimeInc.tm_year	= two.tm_year - one.tm_year;
			
			ReduceTime(&fTimeInc);
		}
	}
	else
	{
		fCells->GetValue(fRange.BotRight(), next);
		
		if (next.fType == fFillType)
		{
			if (fFillType == eNumData)
				fIncrement = (next.fDouble - start.fDouble) / fSteps;
			else if (fFillType == eTimeData)
			{
				struct tm one, two;
				one = *gmtime(&start.fTime);
				two = *gmtime(&next.fTime);
				fTimeInc.tm_sec	= (two.tm_sec - one.tm_sec) / fSteps;
				fTimeInc.tm_min	= (two.tm_min - one.tm_min) / fSteps;
				fTimeInc.tm_hour	= (two.tm_hour - one.tm_hour) / fSteps;
				fTimeInc.tm_mday	= (two.tm_mday - one.tm_mday) / fSteps;
				fTimeInc.tm_mon	= (two.tm_mon - one.tm_mon) / fSteps;
				fTimeInc.tm_year	= (two.tm_year - one.tm_year) / fSteps;
				
				ReduceTime(&fTimeInc);
			}
		}
		else
		{
			memset(&fTimeInc, 0, sizeof(fTimeInc));
			fTimeInc.tm_mday = 1;
			fIncrement = 1;
		}
	}
		
	char s[256];
	
	FormatFirst(s);
	SetText("first", s);
	FormatStep(s);
	SetText("step", s);
	FormatLast(s);
	SetText("last", s);

	fFirstBox = dynamic_cast<BTextControl*>(FindView("first"));
	fStepBox = dynamic_cast<BTextControl*>(FindView("step"));
	fLastBox = dynamic_cast<BTextControl*>(FindView("last"));

	((BControl*)FindView("day"))->SetEnabled(fFillType == eTimeData);
	((BControl*)FindView("week"))->SetEnabled(fFillType == eTimeData);
	((BControl*)FindView("month"))->SetEnabled(fFillType == eTimeData);
	((BControl*)FindView("quarter"))->SetEnabled(fFillType == eTimeData);
	((BControl*)FindView("year"))->SetEnabled(fFillType == eTimeData);

	fFirstBox->MakeFocus();
	
	Show();
} /* CFillDialog::PostInit */

void CFillDialog::UpdateFields()
{
	BAutolock lock(this);
	
	char s[256];

	BHandler *vp;
	CurrentMessage()->FindPointer("source", (void**)&vp);
	BTextControl *tc = dynamic_cast<BTextControl *>(vp);
	
	if (tc)
	{
		if (!tc->TextView()->IsFocus())
			return;

		Value v;
		try
		{
			// fprintf( stderr, "About to ParseToValue %s\n", tc->Text() ) ;
			ParseToValue(tc->Text(), v);
			if (tc == fFirstBox && (v.fType != eNumData && v.fType != eTimeData))
			{
				THROW((errOnlyNrsOrDates));
			}
			else if (tc == fLastBox && v.fType != fFillType)
			{
				THROW((errMustBeSameAsStart));
			}
			else if (tc == fStepBox && v.fType != eNumData)
			{
				THROW((errMustBeNumericalStep));
			}
		}
		catch (CErr& e)
		{
			SetText("last", "not valid");
//			e.DoError();
//			tc->MakeFocus();
			return;
		}

		if (tc == fFirstBox)
		{
			fFillType = v.fType;
			if (fFillType == eNumData)
				fStart = v.fDouble;
			else
				fStart = (double)v.fTime;
		}
		else if (tc == fStepBox)
		{
			fIncrement = v.fDouble;
			if (fFillType == eTimeData)
			{
				memset(&fTimeInc, 0, sizeof(struct tm));
				if (IsOn("day"))
					fTimeInc.tm_mday = static_cast<int>(fIncrement);
				else if (IsOn("week"))
					fTimeInc.tm_mday = static_cast<int>(7 * fIncrement);
				else if (IsOn("month"))
					fTimeInc.tm_mon = static_cast<int>(fIncrement);
				else if (IsOn("quarter"))
					fTimeInc.tm_mon = static_cast<int>(3 * fIncrement);
				else if (IsOn("year"))
					fTimeInc.tm_year = static_cast<int>(fIncrement) ;
			}
		}
		else if (tc == fLastBox)
		{
			if (fFillType == eNumData)
				fIncrement = (v.fDouble - fStart) / fSteps;
			else
			{
				time_t first = static_cast<time_t>(fStart),
				       last  = v.fTime ;
				struct tm one, two;
				one = *gmtime(&first);
				two = *gmtime(&last);
				fTimeInc.tm_sec	= (two.tm_sec - one.tm_sec) / fSteps;
				fTimeInc.tm_min	= (two.tm_min - one.tm_min) / fSteps;
				fTimeInc.tm_hour	= (two.tm_hour - one.tm_hour) / fSteps;
				fTimeInc.tm_mday	= (two.tm_mday - one.tm_mday) / fSteps;
				fTimeInc.tm_mon	= (two.tm_mon - one.tm_mon) / fSteps;
				fTimeInc.tm_year	= (two.tm_year - one.tm_year) /fSteps;
				
				ReduceTime(&fTimeInc);
			}
		}
	}
	else
	{
		memset(&fTimeInc, 0, sizeof(struct tm));
		if (IsOn("day"))
			fTimeInc.tm_mday = static_cast<int>(fIncrement) ;
		else if (IsOn("week"))
			fTimeInc.tm_mday = static_cast<int>(7 * fIncrement) ;
		else if (IsOn("month"))
			fTimeInc.tm_mon = static_cast<int>(fIncrement) ;
		else if (IsOn("quarter"))
			fTimeInc.tm_mon = static_cast<int>(3 * fIncrement) ;
		else if (IsOn("year"))
			fTimeInc.tm_year = static_cast<int>(fIncrement) ;
	}
	
	((BControl*)FindView("day"))->SetEnabled(fFillType == eTimeData);
	((BControl*)FindView("week"))->SetEnabled(fFillType == eTimeData);
	((BControl*)FindView("month"))->SetEnabled(fFillType == eTimeData);
	((BControl*)FindView("quarter"))->SetEnabled(fFillType == eTimeData);
	((BControl*)FindView("year"))->SetEnabled(fFillType == eTimeData);

	if (tc != fFirstBox)
	{
		FormatFirst(s);
		SetText("first", s);
	}
	FormatStep(s);
	SetText("step", s);
	FormatLast(s);
	SetText("last", s);
} /* CFillDialog::UpdateFields */

bool CFillDialog::OKClicked()
{
	UpdateFields();
	BMessage msg(msg_Fill);
	
	msg.AddDouble("first", fStart);
	msg.AddDouble("increment", fIncrement);
	msg.AddInt32("type", fFillType);
	msg.AddBool("rows", IsOn("rows"));
	msg.AddBool("day", IsOn("day"));
	msg.AddBool("week", IsOn("week"));
	msg.AddBool("month", IsOn("month"));
	msg.AddBool("quarter", IsOn("quarter"));
	msg.AddBool("year", IsOn("year"));
	
	fOwner->PostMessage(&msg, fView);

	return true;
} /* CFillDialog::OKClicked */

void CFillDialog::FormatFirst(char *s)
{
	if (fFillType == eNumData)
		ftoa(fStart, s);
	else
	{
		Value v;
		v = (time_t)fStart;
		gFormatTable.FormatValue(0, v, s);
	}
} /* CFillDialog::FormatFirst */

void CFillDialog::FormatStep(char *s)
{
	ftoa(fIncrement, s);
} /* CFillDialog::FormatStep */

void CFillDialog::FormatLast(char *s)
{
	if (fFillType == eNumData)
		ftoa(fStart + fSteps * fIncrement, s);
	else
	{
		struct tm start, end;
		time_t t;
		
		t = (time_t)fStart;
		
		start = *gmtime(&t);
	
		memset(&end, 0, sizeof(struct tm));
		end.tm_sec	= start.tm_sec + fSteps * fTimeInc.tm_sec;
		end.tm_min	= start.tm_min + fSteps * fTimeInc.tm_min;
		end.tm_hour	= start.tm_hour + fSteps * fTimeInc.tm_hour;
		end.tm_mday	= start.tm_mday + fSteps * fTimeInc.tm_mday;
		end.tm_mon	= start.tm_mon + fSteps * fTimeInc.tm_mon;
		end.tm_year	= start.tm_year + fSteps * fTimeInc.tm_year;
		
		t = mktime(&end) - timezone;
		Value v = t;
		
		gFormatTable.FormatValue(0, v, s);
	}
} /* CFillDialog::FormatLast */


CFillDialog* CFillDialog::Construct(CCellWindow *inWindow)
{
	BMallocIO buf;
	BRect frame;
	char name[256];
	window_type type;
	int flags;
	
	InitValues(4, buf, frame, name, type, flags, inWindow);
	
	CFillDialog* result = new CFillDialog(frame, name, inWindow, type, flags);
	if (result)
	{
		try
		{
			result->ConstructFromTemplate(buf);
			result->PostInit(inWindow);
		}
		catch (CErr& e)
		{
			e.DoError();
			delete result;
			result = NULL;
		}
	}
	
	return result;
} /* CFillDialog::Construct */

void CFillDialog::ReduceTime(struct tm *inc)
{
	if ((inc->tm_sec != 0) + (inc->tm_min != 0) + (inc->tm_hour != 0) +
		(inc->tm_mday != 0) + (inc->tm_mon != 0) + (inc->tm_year != 0) > 1)
	{
		MAlert *a = new MWarningAlert(GetMessage(msgMustBeIntegral));
		a->Go();
		memset(inc, 0, sizeof(struct tm));
		if (IsOn("day"))
			inc->tm_mday = static_cast<int>(fIncrement) ;
		else if (IsOn("week"))
			inc->tm_mday = static_cast<int>(7 * fIncrement) ;
		else if (IsOn("month"))
			inc->tm_mon  = static_cast<int>(fIncrement) ;
		else if (IsOn("quarter"))
			inc->tm_mon  = static_cast<int>(3 * fIncrement) ;
		else if (IsOn("year"))
			inc->tm_year = static_cast<int>(fIncrement) ;
	}

	if (inc->tm_mday)
	{
		if (inc->tm_mday % 7)
		{
			fIncrement = inc->tm_mday;
			SetOn("day", true);
		}
		else
		{
			fIncrement = inc->tm_mday / 7;
			SetOn("week", true);
		}
	}
	else if (inc->tm_mon)
	{
		if (inc->tm_mon %3)
		{
			fIncrement = inc->tm_mon;
			SetOn("month", true);
		}
		else
		{
			fIncrement = inc->tm_mon / 3;
			SetOn("quarter", true);
		}
	}
	else if (inc->tm_year)
	{
		fIncrement = inc->tm_year;
		SetOn("year", true);
	}
} /* CFillDialog::ReduceTime */
