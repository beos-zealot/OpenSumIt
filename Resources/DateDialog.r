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

#include "SumItTypes.r"

Resource 'DLOG' (2, "DateTimeFormat")
{
	nonmodal,
	{0, 0, 270, 210},
	"Date/Time Format",
	{
		Checkbox			{ { 10, 10, 110, 26 }, "day", "Day", 1 },
		Checkbox			{ { 10, 30, 80, 42 }, "month", "Month", 1 },
		View				{ { 90, 30, 270, 48 }, "" },
			Radiobutton		{ { 0, 0, 59, 16 }, "m_nr", "Nr", 1 },
			Radiobutton		{ { 60, 0, 119, 16 }, "m_short", "Short", 0 },
			Radiobutton		{ { 120, 0, 179, 16 }, "m_long", "long", 0 },
		End					{},
		Checkbox			{ { 10, 50, 80, 62 }, "year", "Year", 1 },
		View				{ { 90, 50, 270, 68 }, "" },
			Radiobutton		{ { 0, 0, 59, 16 }, "y_short", "Short", 1 },
			Radiobutton		{ { 60, 0, 119, 16 }, "y_long", "Long", 0 },
		End					{},
		Checkbox 			{ { 10, 70, 110, 82 }, "hour", "Hour", 0 },
		Checkbox			{ { 10, 90, 110, 102 }, "minute", "Minute", 0 },
		Checkbox			{ { 10, 110, 110, 122 }, "second", "Seconds", 0 },
		Box					{ { 10, 130, 260, 170 }, "Example"}, 
			Caption			{ { 10, 14, 240, 30 }, "demo", "demo text" },
		End					{},
		Button				{ { 202, 180, 260, 200 }, "ok", "OK", 'OK  ' },
		Button				{ { 134, 180, 192, 200 }, "cancel", "Cancel", 'Cncl' }
	}
};
