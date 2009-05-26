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

Resource 'DLOG' (1, "Document options")
{
	nonmodal,
	{0, 0, 300, 200},
	"Options for %s",
	{
		TabBook					{{0, 0, 300, 170}, "tabs" },
			TabSheet			{{0, 0, 0, 0}, "Fonts" },
				Box				{ {43, 20, 243, 64}, "Document font" },
					Popup		{{5, 14, 150, 30}, "docfont", "", 0 },
					TextEdit	{{160, 16, 190, 32}, "docsize", "", "", "0123456789", 2 },
				End				{},
				Box				{{42, 80, 243, 124}, "Heading font" },
					Popup		{{5, 14, 150, 30}, "borderfont", "", 0 },
					TextEdit	{{160, 16, 190, 32}, "bordersize", "", "", "0123456789", 2 },
				End				{},
			End					{},
			TabSheet			{{0, 0, 0, 0}, "Calculation" },
				CheckBox		{{ 10, 10, 200, 26}, "autorecalc", "Automatic recalculate", 0 },
				CheckBox		{{ 10, 30, 200, 46}, "displayzero", "Display zero results", 0 },
			End					{},
		End						{},
		Button					{{239, 174, 297, 194}, "ok", "Save", 'OK  ' },
		Button					{{171, 174, 229, 194}, "cancel", "Revert", 'Cncl' }
	}
};
