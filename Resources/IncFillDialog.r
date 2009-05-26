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

Resource 'DLOG' (4, "Increment Fill")
{
	nonmodal,
	{0, 0, 220, 240},
	"Incremental Fill",
	{
		Box					{{ 10, 10, 100, 68 }, "Series in" },
			RadioButton		{{ 10, 16, 78, 32 }, "rows", "Rows", 1 },
			RadioButton		{{ 10, 36, 78, 52 }, "cols", "Columns", 0 },
		End					{},
		Box					{{ 110, 10, 210, 128 }, "Date" },
			Radiobutton		{ { 10, 16, 90, 32 }, "day", "Day", 1 },
			Radiobutton		{ { 10, 36, 90, 52 }, "week", "Week", 0 },
			Radiobutton		{ { 10, 56, 90, 72 }, "month", "Month", 0 },
			Radiobutton		{ { 10, 76, 90, 92 }, "quarter", "Quarter", 0 },
			Radiobutton		{ { 10, 96, 90, 112 }, "year", "Year", 0 },
		End					{},
		TextEdit			{ { 10, 140, 210, 156 }, "first", "First Value:", "", "", 32 },
		TextEdit			{ { 10, 160, 210, 176 }, "step", "Step Value:", "", "", 32 },
		TextEdit			{ { 10, 180, 210, 196 }, "last", "Last Value:", "", "", 32 },
		Button				{ {152, 210, 210, 230}, "ok", "OK", 'OK  ' },
		Button				{ { 84, 210, 142, 230}, "cancel", "Cancel", 'Cncl' }
	}
};
