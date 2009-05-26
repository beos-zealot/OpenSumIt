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

Resource 'DLOG' (0)
{
	nonmodal,
	{0, 0, 300, 230},
	"Settings",
	{
		TabBook					{{   0, 0, 300, 200},	"tabs" },
			TabSheet			{{   0, 0, 0, 0},	"Fonts" },
				Popup 			{{  10, 10, 200, 30 },"neworcur","Default Font for", 0},
				Box 			{{  30, 45, 220, 85}, "Document font" },
					Popup		{{   5, 14, 150, 30},"docfont","",0},
					TextEdit	{{ 150, 16, 180, 32},"docsize","","","0123456789",2},
				End				{},
				Box				{{  30, 95, 220, 135},"Heading font"},
					Popup		{{   5, 14, 150, 30},"borderfont",	"",	0},
					TextEdit	{{ 150, 16, 180, 32}, "bordersize", "", "", "0123456789", 2 },
				End				{},
			End					{},
			TabSheet			{{   0, 0, 0, 0}, "Worksheet" },
				Caption			{{  10, 10, 240, 26 }, "", "These options apply to the current document" },
				CheckBox		{{  10, 30, 200, 46}, "autorecalc", "Automatic recalculate", 1 },
				CheckBox		{{  10, 50, 200, 66}, "displayzero", "Display zero results", 0 },
				CheckBox		{{  10, 70, 200, 86 }, "dispgrid", "Display grid", 1 },
				CheckBox		{{  10, 90, 200, 106 }, "disphead", "Display headings", 1 },
			End					{},
			TabSheet			{{   0, 0, 0, 0}, "Numbers" },
				Box				{{  15, 5, 270, 89 }, "Separators" },
					TextEdit	{{  10, 14, 115, 30}, "decsep", "Decimal:", "", "", 1 },
					TextEdit	{{  10, 35, 115, 51}, "thoussep", "Thousands:", "", "", 1 },
					TextEdit	{{  10, 56, 115, 72}, "listsep", "List:", "", "", 1 },
				End				{},
				Box				{{  15, 94, 270, 174 }, "Currency" },
					TextEdit	{{  10, 14, 115, 30}, "c_symbol", "Symbol:", "$", "", 4 },
					Radiobutton	{{ 130, 14, 195, 30}, "c_before", "Before", 1 },
					Radiobutton	{{ 200, 14, 245, 30 }, "c_after", "After", 0 },
					TextEdit	{{  10, 36, 115, 52 }, "c_digits", "Digits:", "2", "0123456789", 1 },
					CheckBox	{{  10, 57, 239, 73 }, "c_neg_par", "Put negatives in parentheses", 0 },
				End				{},
			End					{},
			TabSheet			{{   0, 0, 0, 0 }, "Dates" },
				Box				{{  15, 10, 270, 69 }, "Separators" },
					TextEdit	{{  10, 14, 115, 30}, "timesep", "Time:", "", "", 1},
					TextEdit	{{  10, 36, 115, 52 }, "datesep", "Date:", "", "", 1 },
				End				{},
				Popup			{{  15, 75, 150, 91 }, "dateorder", "Order:", 50 },
				CheckBox		{{  15, 97, 150, 113 }, "24 hours", "Use 24 hours", 0 },
			End					{},
			TabSheet			{{   0, 0, 0, 0}, "Misc" },
				CheckBox		{{  10, 10, 240, 26}, "excel", "Arrow key enters data/moves cursor", 0 },
				CheckBox		{{  10, 30, 240, 46}, "grayselect", "Lighten active cells indicator", 0 },
				CheckBox		{{  10, 50, 240, 66 }, "formula starts with equal", "Put '=' in front of formulas", 0 },
				CheckBox		{{  10, 70, 240, 86 }, "dark gridlines", "Darken gridlines", 0 },
				Box				{{  10, 95, 200, 152 }, "At Startup" },
					Radiobutton	{{  10, 14, 180, 30}, "donew", "Open a new worksheet", 1 },
					Radiobutton	{{  10, 35, 180, 51}, "doopen", "Show a File Browser dialog", 0 },
				End				{},
			End					{},
			TabSheet			{{   0, 0, 0, 0 }, "Printing" },
				Button			{{  10, 10, 130, 30 }, "pagesetup", "Default Page Setup", 'Page' },
				Box				{{  10, 50, 270, 110 }, "Print Options for %s" },
					CheckBox	{{  10, 14, 230, 30 }, "prgrid", "Print gridlines", 0 },
					CheckBox	{{  10, 34, 230, 50 }, "prhead", "Print row and column headings", 0 },
				End				{},
			End					{},
		End						{},
		Button					{{ 239, 204, 297, 224}, "ok", "Apply", 'OK  ' },
		Button					{{ 171, 204, 229, 224}, "cancel", "Revert", 'Cncl' }
	}
};
