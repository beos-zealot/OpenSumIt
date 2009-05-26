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
	Excel.pass1.c

*/

#include "Excel.h"
#include "XL_Biff_codes.h"
#include "Excel.colors.h"
#include "CellView.h"
#include "Utils.h"
#include "FontMetrics.h"
#include "Formatter.h"
#include "CellStyle.h"
#include "Container.h"

void CExcel5Filter::Pass1()
{
	CExcelStream es(fBook);

	short code, len, bofrec;
	int32 offset;
	
	offset = 0;

	fBook.Seek(offset, SEEK_SET);
	es >> code >> len;
	offset += 4 + len;
		
	if (code != 0x0809)
		throw CErr("Expected BOF record");

	es >> bofrec >> bofrec;
	if (bofrec != 0x0005) throw CErr("Expected global section");
	
	while (offset < fBook.BufferLength())
	{
		fBook.Seek(offset, SEEK_SET);
		es >> code >> len;
		offset += 4 + len;

		if (code == 0x0809)
			throw CErr("Unexpected start of new substream");
		else if (code == B_EOF)
			break;

		switch (code)
		{
			case LABEL:
			case RK:
			case RSTRING:
			case MULRK:
			case MULBLANK:
			case BLANK:
			case FORMULA:
				throw CErr("Did not expect data insize globals area");
			default:
				HandleXLRecordForPass1(code, len);
		}
	}

	fBook.Seek(offset, SEEK_SET);
	es >> code >> len;
	offset += 4 + len;
		
	if (code != 0x0809)
		throw CErr("Expected new substream");

	es >> bofrec >> bofrec;
	if (bofrec != 0x0010) throw CErr("Expected beginning of data for sheet 1");
	
	while (offset < fBook.BufferLength())
	{
		fBook.Seek(offset, SEEK_SET);
		es >> code >> len;
		offset += 4 + len;

		if (code == 0x0809)
			throw CErr("Unexpected start of new substream");
		else if (code == B_EOF)
			break;

		HandleXLRecordForPass1(code, len);
	}
} // CExcel5Filter::Pass1

void CExcel5Filter::Selection()
{
	CExcelStream es(fBook);

	char p;
	short x;

	es >> p;
	if (p == 3)
	{
		cell curCell;
		es >> curCell.v >> curCell.h;
//		fViewInfo.curCell.v++;
//		fViewInfo.curCell.h++;

		es >> x >> x;
		
		range r;
		char c;

		es >> r.top >> r.bottom;
		r.top++;
		r.bottom++;

		es >> c;
		r.left = c + 1;

		es >> c;
		r.right = c + 1;

		fCellView->SetSelection(r);
	}
}

void CExcel5Filter::Name()
{
	CExcelStream es(fBook);

	char name[256], c;
	short grbit, deflen;

	es >> grbit >> c >> c >> deflen;
	
	if (deflen == 0)	// dan zal het wel een functie zijn, hoop ik
	{
		fBook.Seek(fBook.Position() + 8, SEEK_SET);
		es.Read(name, c);
		name[c] = 0;
		
		int funcNr = GetFunctionNr(name);
		
		if (funcNr < 0)
			CErr("File contains an unknown function: %s", name).DoError();
		
		fNames.push_back(xlName(funcNr));
	}
	else
	{ 
		fBook.Seek(fBook.Position() + 8, SEEK_SET);
		es.Read(name, c);
		name[c] = 0;
		
		range r;
		es >> c;
		
		switch (c)
		{
			case 0x3B:
				fBook.Seek(fBook.Position() + 14, SEEK_SET);
			case 0x07:
			case 0x2D:
				es >> r.top;			r.top = (r.top & 0x3FFF) + 1;
				es >> r.bottom;	r.bottom = (r.bottom & 0x3FFF) + 1;
				es >> c;				r.left = c + 1;
				es >> c;				r.right = c + 1;
				break;
			default:
//				throw CErr("Named reference %s too complex: %d", name, c);
				return;
		}
		
		fCellView->AddNamedRange(name, r);
		fNames.push_back(name);
	}
} 

void CExcel5Filter::Font()
{
	CExcelStream es(fBook);

	short x;
	float size;
	es >> x;
	size = x / 20.0;
	
	short grbit;
	es >> grbit;
	
	short color;
	es >> color;
	
	es >> x;
	font_style fs;
	if (x <= 0x0190)
		if (grbit & 2)
			strcpy(fs, "Italic");
		else
			strcpy(fs, "Roman");
	else
		if (grbit & 2)
			strcpy(fs, "Bold Italic");
		else
			strcpy(fs, "Bold");
		
	es >> x;
	es >> x;
	es >> x;
	
	font_family fn;
	es >> (unsigned char *)fn;
	
	rgb_color c = { 0, 0, 0, 0};
	if (color >= 8 && color < 64)
		c = kExcelColorTable[color - 8];
	
	fFonts.push_back(gFontSizeTable.GetFontID(fn, fs, size, c));
} 

void CExcel5Filter::Xf()
{
	CExcelStream es(fBook);
	
	short x;
	ushort y;
	CellStyle style;
	
	style.fLowColor.alpha = 255;
	
	es >> y;
	if (y >= 4) y--;
	style.fFont = fFonts[y];
	
	es >> y;
	
	if (y < 5)
	{
		switch (y)
		{
// gokje... (that means gamble in Dutch)
			case 1: 
				style.fFormat = 3; //eFixed;
				break;

			case 2:
				style.fFormat = 3; //eFixed;
				style.fFormat |= 2 << 4;
				break;

			case 3:
				style.fFormat = 3; //eFixed;
				style.fFormat |= 2 << 4;
				style.fFormat |= 1 << 9;
				break;

			case 4:
				style.fFormat = 1; //eCurrency;
				style.fFormat |= 2 << 4;
				style.fFormat |= 1 << 9;
				break;
				
			case 0:
			default:
				style.fFormat = 0;	// general
				break;
		}
	}
	else	
		style.fFormat = fFormats[y];

	es >> x >> x;
	style.fAlignment = x & 0x07;
	
	fStyles.push_back(gStyleTable.GetStyleID(style));
} 

void CExcel5Filter::HandleXLRecordForPass1(int code, int len)
{
	CExcelStream es(fBook);
	
	short x;
	
	switch (code & 0x00FF)
	{
		case BLANK:
		case RK:
		case FORMULA:
		case NUMBER:
		case LABEL:
		case RSTRING:
		case BOOLERR:
//			fHeaderInfo.cellCount++;
			break;
		case MULRK:
//			fHeaderInfo.cellCount += (len-4)/6;
			break;
		case MULBLANK:
//			fHeaderInfo.cellCount += (len-4)/2;
			break;

		case B_1904:
			es >> x;
			f1904 = (x == 1);
			break;
		case SELECTION:
			Selection();
			break;
		case NAME:
			Name();
			break;
		case DEFAULTROWHEIGHT:
		{
			es >> x;
			if (!(x & 0x01))
			{
				es >> x;
				fCellView->GetHeights().SetValue(1, kRowCount,
					ceil(x / 20.0) + 1);
			}
			break;
		}
		case ROW:
		{
			short h;
			es >> x >> h >> h >> h;
			fCellView->GetHeights().SetValue(x + 1, ceil(h / 20) + 1);
			break;
		}
		case FONT:
			Font();
			break;
		case WINDOW2:
		{
			es >> x;
			fCellView->SetShowGrid(x & 0x02);
			fCellView->SetShowBorders(x & 0x04);
			
			es >> x;
//			fViewInfo.position.v = x + 1;
			es >> x;
//			fViewInfo.position.h = x + 1;
			break;
		}
//		case STYLE:
//			break;
//		case PRINTHEADERS:
//			ReadSwapped(inStream, x);
//			fPrBorders = (x == 1);
//			break;
//		case PRINTGRIDLINES:
//			ReadSwapped(inStream, x);
//			fPrGrid = (x == 1);
//			break;
		case FORMAT:
		{	
			char s[256];
			es >> x >> (unsigned char *)s;
			
			fFormats.push_back(gFormatTable.GetFormatID(s));
			break;
		}
		case DEFCOLWIDTH:
		{
			es >> x;
			
			fCellView->GetWidths().SetValue(1, kColCount,
				ceil(x * be_plain_font->StringWidth("x")) + 1);
			break;
		}
		case COLINFO:
		{
			short first, last, wi, f;
			
			es >> first >> last >> wi >> x >> f;
			
			if (f & 0x11)
				fCellView->GetWidths().SetValue(first + 1,last + 1,0);
			else
			{
				fCellView->GetWidths().SetValue(first + 1, last + 1,
					floor(be_plain_font->StringWidth("x") * wi / 256));
			}
			
			fContainer->GetColumnStyles().SetValue(first + 1, last + 1, x);
			break;
		}
		case XF:
			Xf();
			break;
		case SHRFMLA:
		{
			XLSHFormula shxl;
			es >> shxl.rwFirst >> shxl.rwLast >> shxl.colFirst >> shxl.colLast >> x >> shxl.cce;
			
			shxl.p = (char *)calloc(1, shxl.cce);
			if (!shxl.p) throw CErr(1);
			es.Read(shxl.p, shxl.cce);
			
			fSharedFormulas.push_back(shxl);
			break;
		}
	}
} 
