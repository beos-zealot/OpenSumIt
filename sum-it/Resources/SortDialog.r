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

Resource 'DLOG' (10, "Sort")
{
	nonmodal,
	{0, 0, 220, 198},
	"Sort",
	{
		Caption			{ { 10, 10, 210, 26}, "range", "Sort Range: %s" },
		Box				{ { 10, 36, 210, 61 }, "" },
			Radiobutton	{ { 10, 4, 100, 20 }, "bycol", "Sort Columns", 0 },
			Radiobutton { { 101, 4, 198, 20 }, "byrow", "Sort Rows", 1 },
		End				{},
		Box				{ { 10, 70, 210, 158 }, "" },
			Caption		{ { 10, 4, 100, 20 }, "keydesc", "Sort %s" },
			Caption		{ { 101, 4, 198, 20 }, "", "Sort Order" },
			Popup			{ { 10, 24, 100, 40 }, "Key 1", "", 0 },
			Popup			{ { 101, 24, 198, 40 }, "Order 1", "", 0 },
			Popup			{ { 10, 44, 100, 60 }, "Key 2", "", 0 },
			Popup			{ { 101, 44, 198, 60 }, "Order 2", "", 0 },
			Popup			{ { 10, 64, 100, 80 }, "Key 3", "", 0 },
			Popup			{ { 101, 64, 198, 80 }, "Order 3", "", 0 },
		End				{},
		Button			{ { 150, 168, 210, 188 }, "ok", "OK", 'OK  ' },
		Button			{ { 80, 168, 140, 188 }, "cancel", "Cancel", 'Cncl' }
	}
};
