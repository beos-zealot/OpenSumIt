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
	Globals.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <GraphicsDefs.h>
#include <Path.h>
#include <Directory.h>

_EXPORT extern double 	gRefNan, gCircleNan, gSqrtNan, gPowerNan, gValueNan, gDivNan,
				gAddNan, gFinanceNan, gEvalNan, gInvTrigNan, gLogNan, gMulNan,
				gNANan, gErrorNan, gDateNan, gTimeNan, gFuncNan, gNameNan;

extern char
	gDecimalPoint, gThousandSeparator, gListSeparator,
	gTimeSeparator, gDateSeparator;

extern bool
	g24Hours;

extern bool
	gCurrencyBefore,
	gCurrencyParens;

extern char
	gCurrencySymbol[];

extern int
	gCurrencyDigits;

extern BPath		gAppName;
extern BDirectory	*gAppDir, *gCWD;

#define kNanStrings			1
#define kFormattingStrings	2
#define kTrueIndex			0
#define kFalseIndex			1

const rgb_color
	kRed 		= {255, 0, 0, 255},
	kGreen 		= {0, 255, 0, 255},
	kBlue 		= {0, 0, 255, 255},
	kCyan 		= {0, 255, 255, 255},
	kMagenta 	= {255, 0, 255, 255},
	kYellow 	= {255, 255, 0, 255},
	kBlack 		= {0, 0, 0, 255},
	kWhite		= {255, 255, 255, 255},
	kGray		= {238, 238, 238, 255},
	kBackYellow	= {255, 255, 153, 255},
	kBackGreen	= {204, 255, 204, 255},
	kBackBlue	= {153, 255, 255, 255},
	kPink		= {255, 204, 204, 255},
	kPurple		= {204, 204, 255, 255},
	kPRed 		= {255, 0xaa, 0xaa, 255},
	kPGreen 	= {0xaa, 255, 0xaa, 255},
	kPBlue 		= {0xaa, 0xaa, 255, 255},
	kPCyan 		= {0xaa, 255, 255, 255},
	kPMagenta 	= {255, 0xaa, 255, 255},
	kPYellow 	= {255, 255, 0xaa, 255},
	kHighlight = { 80, 80, 80, 255};

#define kOpenSumItSignature "application/x-vnd.open.sum-it"
#define kOpenSumItMimeString "application/x-sum-it-document"

#endif
