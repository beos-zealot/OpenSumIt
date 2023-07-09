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
	GraphPlugIn.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELL_H
#include "Cell.h"
#endif

#ifndef   RANGE_H
#include "Range.h"
#endif

#ifndef   GRAPHPLUGIN_H
#include "GraphPlugIn.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   GRAPHOPTIONSDIALOG_H
#include "GraphOptionsDialog.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif


#include <AppDefs.h>
#include <Autolock.h>
#include <DataIO.h>
#include <Message.h>

CGraphPlugIn::CGraphPlugIn(cell anchor, BRect frame, const char *name, range r, image_id addon)
	: CGraphic(anchor, frame, name)
{
	fRange = r;
	fAddOn = addon;
	
	font_family fam;
	font_style sty;
	float size;
	
	be_plain_font->GetFamilyAndStyle(&fam, &sty);
	size = be_plain_font->Size();
	
	fGraphFont.SetFamilyAndStyle(
		gPrefs->GetPrefString("graph font family", fam),
		gPrefs->GetPrefString("graph font style", sty));
	fGraphFont.SetSize(gPrefs->GetPrefDouble("graph font size", size));
	
	fGraphOptionsDialog = NULL;
} // CGraphPlugIn::CGraphPlugIn

CGraphPlugIn::~CGraphPlugIn()
{
	if (fGraphOptionsDialog)
	{
		if (fGraphOptionsDialog->Lock())
			fGraphOptionsDialog->Quit();
	}
} // CGraphPlugIn::~CGraphPlugIn

void CGraphPlugIn::DataChanged()
{
	fParent->Invalidate(Bounds());
} /* CGraphPlugIn::DataChanged */

void CGraphPlugIn::DisplayOptions()
{
	if (fGraphOptionsDialog)
		fGraphOptionsDialog->Activate(true);
	else
	{
		BMallocIO tmpl;
		GetOptionsTemplate(tmpl);
		fGraphOptionsDialog = CGraphOptionsDialog::Construct(this, tmpl, Settings());
	}
} /* CGraphPlugIn::DisplayOptions */

void CGraphPlugIn::MakeFocus(bool inFocus)
{
	CGraphic::MakeFocus(inFocus);
	
	if (!inFocus && fGraphOptionsDialog)
		fGraphOptionsDialog->PostMessage(B_QUIT_REQUESTED);
} /* CGraphPlugIn::MakeFocus */

bool CGraphPlugIn::GetCellBool(cell c, bool *b)
{
	CContainer *cnt = fParent->GetContainer();
	
	try
	{
		BAutolock lock(cnt);

		Value v;
		if (cnt->GetValue(c, v) && v.fType == eBoolData)
		{
			*b = v.fBool;
			return true;
		}
	}
	catch (...) { }

	return false;
} /* GetCellBool */

bool CGraphPlugIn::GetCellText(cell c, char *s)
{
	CContainer *cnt = fParent->GetContainer();
	
	try
	{
		BAutolock lock(cnt);
		Value v;
		if (cnt->GetValue(c, v) && v.fType == eTextData)
		{
			strcpy(s, v.fText);
			return true;
		}
	}
	catch (...) { }

	return false;
} /* GetCellText */

bool CGraphPlugIn::GetCellDouble(cell c, double *d)
{
	CContainer *cnt = fParent->GetContainer();
	
	try
	{
		BAutolock lock(cnt);
		Value v;
		if (cnt->GetValue(c, v) && v.fType == eNumData)
		{
			*d = v.fDouble;
			return true;
		}
	}
	catch (...) { }

	return false;
} /* GetCellDouble */

bool CGraphPlugIn::GetCellTime(cell c, time_t *t)
{
	CContainer *cnt = fParent->GetContainer();
	
	try
	{
		BAutolock lock(cnt);
		Value v;
		if (cnt->GetValue(c, v) && v.fType == eTimeData)
		{
			*t = v.fTime;
			return true;
		}
	}
	catch (...) { }

	return false;
} /* GetCellTime */

void CGraphPlugIn::GetCellValue(cell c, char *s)
{
	CContainer *cnt = fParent->GetContainer();
	
	try
	{
		BAutolock lock(cnt);
		cnt->GetCellResult(c, s, true);
	}
	catch(...){}
} // CGraphPlugIn::GetCellValue

double CGraphPlugIn::Interpolate(cell c, bool horizontal)
{
	double d1, d2;
	
	if (GetCellDouble(c, &d1))
		return d1;
	
	if (horizontal)
	{
		cell c2 = c;
		c2.h--;
		if (!GetCellDouble(c2, &d1))
			d1 = Nan(1);
		c2.h += 2;
		if (!GetCellDouble(c2, &d2))
			d2 = Nan(1);
	}
	else
	{
		cell c2 = c;
		c2.h--;
		if (!GetCellDouble(c2, &d1))
			d1 = Nan(1);
		c2.h += 2;
		if (!GetCellDouble(c2, &d2))
			d2 = Nan(1);
	}
	
	if (!isnan(d1) && !isnan(d2))
		return (d1 + d2) / 2;
	else if (!isnan(d1))
		return d1;
	else
		return d2;
} // CGraphPlugIn::Interpolate

void CGraphPlugIn::GetMinAndMaxValues(range r, bool includeZero,
	float height, float& min, float& max, float& step, float& lstep)
{	
	cell c;
	double d;
	
#undef HUGE_VAL
#define HUGE_VAL 1e6
	
	min = HUGE_VAL;
	max = -HUGE_VAL;
	
	for (c.v = r.top; c.v <= r.bottom; c.v++)
		for (c.h = r.left; c.h <= r.right; c.h++)
		{
			if (GetCellDouble(c, &d))
			{
				if (d > max)
					max = d;
				if (d < min)
					min = d;
			}
		}
	
	if (min == HUGE_VAL || (includeZero && min > 0))
		min = 0;
	if (max == -HUGE_VAL || (includeZero && max < 0))
		max = 0;

	float dy = max - min, g;
	int log_dy = (int)floor(log10(dy));
	
	g = pow(10, log_dy);
	
	min = g * floor(min / g);
	max = g * ceil(max / g);
	
	lstep = (max - min) / g;
	step = height / lstep;
	
	if (step >= 250.0)
	{
		step /= 10.0;
		lstep *= 10.0;
	}
	else if (step >= 125.0)
	{
		step /= 5.0;
		lstep *= 5.0;
	}
	else if (step >= 50.0)
	{
		step /= 2.0;
		lstep *= 2.0;
	}
	else if (step <= 3)
	{
		step *= 10.0;
		lstep /= 10.0;
	}
	else if (step <= 5)
	{
		step *= 5.0;
		lstep /= 5.0;
	}
	else if (step <= 12.5)
	{
		step *= 2.0;
		lstep /= 2.0;
	}
	
	if (step > height)
	{
		step = height;
		lstep = 1;
	}
	
	step = floor(step);
	
} // CGraphPlugIn::GetMinAndMaxValues

char** CGraphPlugIn::GuessVerticalLabels(range r)
{
	char **result = NULL, s[256];
	cell c;
	
	c.v = r.top; c.h = r.left - 1;
	
	GetCellValue(c, s);
	if (s[0])
	{
		result = (char **)CALLOC(r.bottom - r.top + 2,
			sizeof(char *));
		
		if (!result)
			return NULL;
		
		result[0] = STRDUP(s);
		FailNil(result[0]);
		for (++c.v; c.v <= r.bottom; c.v++)
		{
			GetCellValue(c, s);
			result[c.v - r.top] = STRDUP(s);
			FailNil(result[c.v - r.top]);
		}
		
		return result;
	}
	else
		return NULL;
} // CGraphPlugIn::GuessVerticalLabels

char** CGraphPlugIn::GuessHorizontalLabels(range r)
{
	char **result = NULL, s[256];
	cell c;
	
	c.v = r.top - 1; c.h = r.left;
	
	GetCellValue(c, s);
	if (s[0])
	{
		result = (char **)CALLOC(r.right - r.left + 2,
			sizeof(char *));
		
		if (!result)
			return NULL;
		
		result[0] = STRDUP(s);
		FailNil(result[0]);
		
		for (++c.h; c.h <= r.right; c.h++)
		{
			GetCellValue(c, s);
			result[c.h - r.left] = STRDUP(s);
			FailNil(result[c.h - r.left]);
		}
		
		return result;
	}
	else
		return NULL;
} // CGraphPlugIn::GuessHorizontalLabels

void CGraphPlugIn::DisposeLabels(char **labels)
{
	if (labels)
	{
		int i = 0;
		while (labels[i] != NULL)
		{
			FREE(labels[i]);
			i++;
		}
		FREE(labels);
	}
} // CGraphPlugIn::DisposeLabels

void CGraphPlugIn::GetOptionsTemplate(BMallocIO& templ)
{
} /* CGraphPlugIn::GetOptionsTemplate */

void CGraphPlugIn::MouseDown(BPoint where)
{
	int32 clicks;
	fParent->Window()->CurrentMessage()->FindInt32("clicks", &clicks);
	if (clicks == 2)
		DisplayOptions();
	else
		CGraphic::MouseDown(where);
} /* CGraphPlugIn::MouseDown */

void CGraphPlugIn::MessageReceived(BMessage *msg)
{
	if (msg->what == msg_GraphOptionsDialogClosed)
		fGraphOptionsDialog = NULL;
	else
		CGraphic::MessageReceived(msg);
} /* CGraphPlugIn::MessageReceived */

BMessage *CGraphPlugIn::Settings()
{
	BMessage *set = new BMessage(msg_ChartSettings);
	
	return set;
} /* CGraphPlugIn::Settings */ 

void CGraphPlugIn::SetRange(range r)
{
	fRange = r;
	DataChanged();
} // CGraphPlugin::SetRange
