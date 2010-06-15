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

Resource 'DLOG' (8, "Find")
{
	nonmodal,
	{ 0, 0, 330, 112 },
	"Find",
	{
		TextEdit		{ { 10, 10, 230, 26 }, "find", "Find:", "", "", 0 },
		TextEdit		{ { 10, 32, 230, 48 }, "replace", "Replace:", "", "", 0 },
		Radiobutton		{ { 10, 66, 100, 82 }, "form", "In Formulas", 1 },
		Radiobutton		{ { 10, 86, 100, 102 }, "val", "In Values", 0 },
		Checkbox		{ { 110, 66, 220, 82 }, "ign_case", "Ignore case", 1 },
		Checkbox		{ { 110, 86, 220, 102 }, "wrap", "Wrap", 1 },
		Button			{ { 240, 6, 324, 26 }, "ok", "Find", 'Find' },
		Button			{ { 240, 32, 324, 52 }, "repl", "Replace", 'Rplc' },
		Button			{ { 240, 58, 324, 78 }, "r&f", "Replace & Find", 'RpFn' },
		Button			{ { 240, 84, 324, 104 }, "repl_all", "Replace All", 'RpAl' }
	}
};
