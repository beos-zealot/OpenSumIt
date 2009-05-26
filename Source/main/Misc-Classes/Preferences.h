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
	Preferences.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 ***/

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <List.h>

class CPreferences {
public:
	CPreferences(const char *preffilename);
	virtual ~CPreferences();

	const char *GetPrefString(const char *name, const char *def = NULL);
	int GetPrefInt(const char *name, int def = 0);
	double GetPrefDouble(const char *name, double def = 0.0);

	void SetPrefString(const char *name, const char *value);
	void SetPrefInt(const char *name, int value);
	void SetPrefDouble(const char *name, double value);
	
	void ReadPrefFile();
	void WritePrefFile();

protected:
	void Clear();

	class CPrefSetting {
	public:
		CPrefSetting(const char *name, const char *value);
			
		CPrefSetting(const char *name, int *value);
		CPrefSetting(const char *name, double *value);
		~CPrefSetting();
		
		const char *GetName() const { return fName; };
		const char *GetValue() const { return fValue; };
		
		void SetValue(const char *newVal);
		
	protected:
		char *fName, *fValue;
	};
	
	CPrefSetting *GetSetting(const char *name) const;
	
	BList fSettings;
	char *fFile;
};

extern CPreferences *gPrefs;
#endif
