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

type 'Func'
{
	array {
		cstring[10];
		integer;
		integer;
		integer;
	}
};

resource 'Func' (128, "Functions")
{
	{
		"ABS", 1, 0, 4,
		"ACOS", 1, 1, 4,
		"ANNUITY", 2, 2, 3,
		"ASC", 1, 3, 6,
		"ASIN", 1, 4, 4,
		"ATAN", 1, 5, 4,
		"AVG", 65535, 6, 5,
		"CEILING", 1, 7, 5,
		"CELL", 2, 8, 1,
		"CHOOSE", 65534, 9, 7,
		"CHR", 1, 10, 6,
		"COLUMN", 1, 11, 1,
		"COMPOUND", 2, 12, 3,
		"COS", 1, 13, 4,
		"COT", 1, 14, 4,
		"COUNT", 65535, 15, 5,
		"DATE", 3, 16, 2,
		"DAY", 1, 17, 2,
		"DB", 4, 18, 3,
		"DOCUMENT", 0, 19, 1,
		"DOW", 1, 20, 2,
		"ERR", 0, 21, 7,
		"ERROR", 1, 22, 7,
		"EXP", 1, 23, 4,
		"FALSE", 0, 24, 7,
		"FLOOR", 1, 25, 5,
		"FRAC", 1, 26, 4,
		"FV", 3, 27, 3,
		"HINDEX", 2, 28, 1,
		"HLOOKUP", 3, 29, 1,
		"HOUR", 1, 30, 2,
		"IF", 3, 31, 7,
		"IFERR", 3, 32, 7,
		"INT", 1, 33, 4,
		"IRR", 2, 34, 3,
		"ISNULL", 1, 35, 7,
		"ISNUM", 1, 36, 7,
		"ISTEXT", 1, 37, 7,
		"LEFT", 2, 38, 6,
		"LENGTH", 1, 39, 6,
		"LN", 1, 40, 4,
		"LOG", 1, 41, 4,
		"MAX", 65535, 42, 5,
		"MID", 3, 43, 6,
		"MIN", 65535, 44, 5,
		"MINUTE", 1, 45, 2,
		"MOD", 2, 46, 4,
		"MONTH", 1, 47, 2,
		"NA", 0, 48, 7,
		"NCOLS", 1, 49, 1,
		"NOW", 0, 50, 2,
		"NPV", 2, 51, 3,
		"NROWS", 1, 52, 1,
		"NUM2C", 1, 53, 6,
		"NUMPAGES", 0, 54, 1,
		"OFFSET", 3, 55, 1,
		"PAGE", 0, 56, 1,
		"PI", 0, 57, 4,
		"PMT", 3, 58, 3,
		"PV", 3, 59, 3,
		"RANDOM", 1, 60, 4,
		"RIGHT", 2, 61, 6,
		"ROUND", 2, 62, 5,
		"ROW", 1, 63, 1,
		"SECOND", 1, 64, 2,
		"SIGN", 1, 65, 4,
		"SIN", 1, 66, 4,
		"SL", 3, 67, 3,
		"SOYD", 4, 68, 3,
		"SQRT", 1, 69, 4,
		"STDDEV", 65535, 70, 5,
		"SUM", 65535, 71, 5,
		"TAN", 1, 72, 4,
		"TIME", 3, 73, 2,
		"TIME2C", 1, 74, 6,
		"VARIANCE", 65535, 75, 5,
		"VINDEX", 2, 76, 1,
		"VLOOKUP", 3, 77, 1,
		"YEAR", 1, 78, 2,
		"TRUE", 0, 79, 7,
		"OR", 65535, 80, 7,
		"AND", 65535, 81, 7,
		"POWER", 2, 82, 4,
		"DEC2HEX", 2, 83, 8,
		"ATAN2", 2, 84, 4
	}
};
