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
	Preferences.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   CONFIG_H
#include "Config.h"
#endif

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <Path.h>
#include <FindDirectory.h>


CPreferences *gPrefs = NULL;

CPreferences::CPreferences(const char *preffilename)
{
	BPath settings;
	
	find_directory(B_USER_SETTINGS_DIRECTORY, &settings, true);

	char p[1024];
	strcpy(p, settings.Path());
	strcat(p, "/");
	strcat(p, preffilename);
	fFile = STRDUP(p);
	FailNil(fFile);
}

CPreferences::~CPreferences()
{
	while (fSettings.CountItems())
	{
		CPrefSetting *item = (CPrefSetting *)fSettings.LastItem();
		fSettings.RemoveItem(item);
		delete item;
	}
	FREE(fFile);
}

const char *CPreferences::GetPrefString(const char *name, const char *def)
{
	CPrefSetting *pref = GetSetting(name);
	if (pref)
		return pref->GetValue();
	else
	{
		SetPrefString(name, def);
		return def;
	}
}

int CPreferences::GetPrefInt(const char *name, int def)
{
	CPrefSetting *pref = GetSetting(name);
	if (pref)
		return atoi(pref->GetValue());
	else
	{
		SetPrefInt(name, def);
		return def;
	}
}

double CPreferences::GetPrefDouble(const char *name, double def)
{
	CPrefSetting *pref = GetSetting(name);
	if (pref)
		return atof(pref->GetValue());
	else
	{
		SetPrefDouble(name, def);
		return def;
	}
}

CPreferences::CPrefSetting *CPreferences::GetSetting(const char *name) const
{
	CPrefSetting *result;
	
	for (int i = 0; i < fSettings.CountItems(); i++)
	{
		result = (CPrefSetting *)fSettings.ItemAt(i);
		if (!strcasecmp(result->GetName(), name))
			return result;
	}
	
	return NULL;
}

void CPreferences::SetPrefString(const char *name, const char *value)
{
	CPrefSetting *pref = GetSetting(name);
	if (pref)
		pref->SetValue(value);
	else
		fSettings.AddItem(new CPrefSetting(name, value));
}

void CPreferences::SetPrefInt(const char *name, int value)
{
	char s[10];
	sprintf(s, "%d", value);
	SetPrefString(name, s);
}

void CPreferences::SetPrefDouble(const char *name, double value)
{
	char s[20];
	sprintf(s, "%g", value);
	SetPrefString(name, s);
}

CPreferences::CPrefSetting::CPrefSetting(const char *name, const char *value)
{
	fName = STRDUP(name);
	FailNil(fName);
	fValue = STRDUP(value);
	FailNil(fValue);
}

CPreferences::CPrefSetting::~CPrefSetting()
{
	FREE(fName);
	FREE(fValue);
}

void CPreferences::CPrefSetting::SetValue(const char *value)
{
	FREE(fValue);
	fValue = STRDUP(value);
	FailNil(fValue);
}

void CPreferences::ReadPrefFile()
{
	FILE *f;
	
	f = fopen(fFile, "r");
	
	if (!f)
		THROW((errIOOpen, fFile));
	
	char s[2048];
	
	while (fgets(s, 2047, f))
	{
		char *n, *v;
		
		n = s;
		v = strchr(s, '=');
		if (!v)
			continue;
		
		*v++ = 0;
		v[strlen(v) - 1] = 0;
		
		fSettings.AddItem(new CPrefSetting(n, v));
	}
	
	fclose(f);
}

void CPreferences::WritePrefFile()
{
	FILE *f;
	
	f = fopen(fFile, "w");
	
	if (!f)
		THROW((errIOOpen, fFile));
	
	for (int i = 0; i < fSettings.CountItems(); i++)
	{
		CPrefSetting *pref = (CPrefSetting *)fSettings.ItemAt(i);
		fprintf(f, "%s=%s\n", pref->GetName(), pref->GetValue());
	}
	
	fclose(f);
}
