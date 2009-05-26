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
	FileFormat.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

// spreadsheet file format for Datatypes
// 
// useful constants and structs
//

#ifndef FILEFORMAT_H
#define FILEFORMAT_H

// Uncomment this struct declaration if you don't have the cell
// type defined in your code.

#if 0
	struct cell {
		short v, h;
		
		cell() : v(0), h(0) {};
		cell(int h, int v) : v(v), h(h) {};
	};
	struct range {
		short top, left, bottom, right;

		range() : top(0), left(0), bottom(0), right(0) {};
		range(int l, int t, int r, int b)
			: top(t), left(l), bottom(b), right(r) {};
	};
#else

#   ifndef   CELL_H
#	include "Cell.h"
#   endif

#   ifndef   RANGE_H
#	include "Range.h"
#   endif

#endif

#include <SupportDefs.h>
#include <GraphicsDefs.h>
#include <Rect.h>

enum scChunkType {
	kscVersion = 'VR',		// version
	kscHeader = 'HD',		// header, contains global data
	kscView = 'VW',			// defines the views that should be opened
	kscPrint = 'PR',		// Print info
	kscWidths = 'WD',		// widths...
	kscHeights = 'HG',		// ...and heights of the cells 
	kscName = 'NM',			// named reference
	kscFont = 'FN',			// Fonts
	kscFormat = 'FM',		// Numberformat
	kscStyle = 'ST',		// Format
	kscColStyles = 'CS',	// Formats for columns
	kscCellEmpty = 'CE',
	kscCellBool = 'CB',
	kscCellNumber = 'CN',
	kscCellDateTime = 'CD',
	kscCellText = 'CT',		// Cells
	kscString = 'TX',		// string value of a cell
	kscFunc = 'FC',			// Function description
	kscFormula = 'FR',		// The parsed formula
	kscChart = 'CH',		// A chart
	kscEnd = 'EN'
};

struct scChunk {
	ushort /*scChunkType*/ type;
	ushort size;
};

const short kVersion = 0x0300;

struct scVersion {
	uchar major;
	uchar minor;
};

struct scHeader {
	ushort defaultFormat;
	short filler0;
	long flags;
	short functionCount;
	short filler1;
	ulong cellCount;
};

enum {
	kscAutoRecalc = 0x00000001
};

struct scView {
	BRect windowRect;
	cell position;
	cell frozen;
	cell curCell;
	range selection;
	long flags;
	ushort headingFont;
	ushort fill;
};

enum {
	kscShowGrid		= 0x00000001,
	kscShowHeadings	= 0x00000002,
	kscDisplayZero	= 0x00000004
};

struct scPrintInfo {
	BRect page;
	BRect margins;
	long flags;
};

struct scWidthElement {
	short index;
	short width;
};

struct scWidths {
	scWidthElement data[1];
};

struct scCSElement {
	short index;
	short style;
};

struct scColStyles {
	scCSElement data[1];
};

struct scName {
	char name[32];
	char reference[1];
};

struct scFunc {
	char name[10];
	ushort funcNr;
	short argCnt;
};

struct scFont {
	float size;
	rgb_color color;
	char reserved[4];
//	font_style style;	// these are packed, i.e. written
//	font_family name;	// directly after each other to save space
};

struct scFormat {
	short nr;
	short info[4];
	char formatString[1];
};

enum {
	kscDigitsMask = 0x0000000F,
	kscCommasMask = 0x00000010
};

struct scStyle {
	ushort font;
	ushort format;
	rgb_color lowColor;
	rgb_color borders[4];
	long flags;
	uchar align;
	char reserved;
};

enum {
	kscAlignGeneral,
	kscAlignLeft,
	kscAlignCenter,
	kscAlignRight,
	kscAlignJust,
	kscAlignFill
};

enum {
	kscLocked = 0x00000001,
	kscHidden = 0x00000002
};

struct scCell {
	cell loc;
	ushort style;
	short num[4]; // wil anders niet alignen
};

const int kscCellSize = sizeof(scCell)-sizeof(double);

struct scString {
	char string[1];
};

struct scFormula {
	char formula[1];
};

struct scChart {
	cell anchor;
	BRect frame;
	range area;
	long reserved[4];
	char name[1];		// directly followed by a flattened BMessage
};

#include "SwapStream.h"

CSwapStream& operator>>(CSwapStream& stream, scChunk& v);
CSwapStream& operator>>(CSwapStream& stream, scVersion& v);
CSwapStream& operator>>(CSwapStream& stream, scHeader& v);
CSwapStream& operator>>(CSwapStream& stream, scView& v);
CSwapStream& operator>>(CSwapStream& stream, scPrintInfo& v);
CSwapStream& operator>>(CSwapStream& stream, scCSElement& v);
CSwapStream& operator>>(CSwapStream& stream, scColStyles& v);
CSwapStream& operator>>(CSwapStream& stream, scName& v);
CSwapStream& operator>>(CSwapStream& stream, scFunc& v);
CSwapStream& operator>>(CSwapStream& stream, scFont& v);
CSwapStream& operator>>(CSwapStream& stream, scFormat& v);
CSwapStream& operator>>(CSwapStream& stream, scStyle& v);
CSwapStream& operator>>(CSwapStream& stream, scCell& v);
CSwapStream& operator>>(CSwapStream& stream, scString& v);
CSwapStream& operator>>(CSwapStream& stream, scFormula& v);
CSwapStream& operator>>(CSwapStream& stream, scChart& v);

inline void swap_order(long& l)
{
	l = ntohl(l);
} /* swap_order */

inline void swap_order(unsigned long& l)
{
	l = ntohl(l);
} /* swap_order */

inline void swap_order(short& s)
{
	s = ntohs(s);
} /* swap_order */

inline void swap_order(unsigned short& s)
{
	s = ntohs(s);
} /* swap_order */

inline void swap_order(float& f)
{
	f = B_BENDIAN_TO_HOST_FLOAT(f);
} /* swap_order */

inline void swap_order(BRect& r)
{
	swap_order(r.left);
	swap_order(r.top);
	swap_order(r.right);
	swap_order(r.bottom);
} /* swap_order */

inline void swap_order(cell& c)
{
	c.v = ntohs(c.v);
	c.h = ntohs(c.h);
} /* swap_order */

inline void swap_order(range& r)
{
	swap_order(r.left);
	swap_order(r.top);
	swap_order(r.right);
	swap_order(r.bottom);
} /* swap_order */


#endif
