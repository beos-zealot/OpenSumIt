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
	Excel.pass2.c
	$Log: Excel.pass2.cpp,v $
	Revision 1.2  2000/05/13 19:20:33  svenweidauer
	i've removed all warnings now (i hope)
	
*/

#include <support/Debug.h>
#include "Excel.h"
#include "XL_Biff_codes.h"
#include "FileFormat.h"
#include "XL_Ptg.h"
#include "MyMath.h"
#include "Container.h"
#include "Formula.h"

void CExcel5Filter::Pass2()
{
	CExcelStream es(fBook);
	
	short code, len;
	ssize_t offset;
	
	offset = 0;

	fBook.Seek(offset, SEEK_SET);
	es >> code >> len;
	offset += 4 + len;

	while (offset < fBook.BufferLength())
	{
		fBook.Seek(offset, SEEK_SET);
		es >> code >> len;
		offset += 4 + len;

		if (code == B_EOF)
			break;

		HandleXLRecordForPass2(code, len);
	}

	fBook.Seek(offset, SEEK_SET);
	es >> code >> len;
	offset += 4 + len;
		
	while (offset < fBook.BufferLength())
	{
		fBook.Seek(offset, SEEK_SET);
		es >> code >> len;
		offset += 4 + len;

		if (code == B_EOF)
			break;

		HandleXLRecordForPass2(code, len);
	}
} // CExcel5Filter::Pass2

void CExcel5Filter::HandleXLRecordForPass2(int code, int len)
{
	CExcelStream es(fBook);
	
	Value v;
	cell c;
	ushort style;
	
	switch (code & 0x00FF)
	{
		case BLANK:	
		{
			es >> c.v >> c.h >> style;
			c.v++;
			c.h++;
			fContainer->NewCell(c, v, NULL);
			fContainer->SetCellStyleNr(c, fStyles[style]);
			break;
		}
		case NUMBER:
		{
			es >> c.v >> c.h >> style;
			c.v++;
			c.h++;
		
			double d;
			es >> d;
			v = d;
			
			fContainer->NewCell(c, v, NULL);
			fContainer->SetCellStyleNr(c, fStyles[style]);
			break;
		}
		case LABEL:
		case RSTRING:
		{
			char label[512];
			short l;
			
			es >> c.v >> c.h >> style >> l;
			c.v++;
			c.h++;
			
			if (l > 511) l = 511;
			es.Read(label, l);
			label[l] = 0;
			v = label;

			fContainer->NewCell(c, v, NULL);
			fContainer->SetCellStyleNr(c, fStyles[style]);
			break;
		}
		case BOOLERR:
		{
			es >> c.v >> c.h >> style;
			c.v++;
			c.h++;

			char b, f;
			es >> b >> f;
			
			if (f)
			{
				switch (b)
				{
//					case 0: d = gRefNan; break;
					case 7: v = Nan(8); /*gDivNan;*/ break;
					case 15: v = Nan(7); /*gValueNan;*/ break;
					case 23: v = Nan(3); /*gRefNan;*/ break;
					case 29: v = Nan(3); /*gRefNan;*/ break;
//					case 36: v = nan(7); /*gValueNan;*/ break;
					case 42: v = Nan(15); /*gNANan;*/ break;
					default: v = Nan(1); /*gErrorNan;*/ break;
				}
				fContainer->NewCell(c, v, NULL);
				fContainer->SetCellStyleNr(c, fStyles[style]);
			}
			else
			{
				v = (bool)(b != 0);
				fContainer->NewCell(c, v, NULL);
				fContainer->SetCellStyleNr(c, fStyles[style]);
			}
			break;
		}
		case RK:
		{
			es >> c.v >> c.h >> style;
			c.v++;
			c.h++;

			long l;
			es >> l;
			
			double d;
			
			if (l & 0x02)
				d = (double)(l>>2);
			else
			{
				long long L = l & 0xFFFFFFFC;
				L <<= 32;
				memcpy(&d, &L, 8);
			}
			if (l & 0x01)
				d /= 100;
			
				v = d;
			fContainer->NewCell(c, v, NULL);
			fContainer->SetCellStyleNr(c, fStyles[style]);
			break;
		}
		case FORMULA:
		{
			es >> c.v >> c.h >> style;
			c.v++;
			c.h++;

			short cce, grbit, num[4];
			long chn;
			
			es >> num[3] >> num[2] >> num[1] >> num[0] >> grbit >> chn >> cce;
			
			if (num[0] == -1)
			{
				switch (num[3])
				{
					case 0:	// text cell
						fContainer->NewCell(c, v, NULL);
						fContainer->SetCellStyleNr(c, fStyles[style]);
						break;
					case 1: // bool cell
						v = (num[1] != 0);
						fContainer->NewCell(c, v, NULL);
						fContainer->SetCellStyleNr(c, fStyles[style]);
						break;
					case 2: // err cell
					{
						switch (num[1] >> 8)
						{
		//					case 0: d = gRefNan; break;
							case 7: v = Nan(8); /*gDivNan;*/ break;
							case 15: v = Nan(7); /*gValueNan;*/ break;
							case 23: v = Nan(3); /*gRefNan;*/ break;
							case 29: v = Nan(3); /*gRefNan;*/ break;
		//					case 36: v = Nan(7); /*gValueNan;*/ break;
							case 42: v = Nan(15); /*gNANan;*/ break;
							default: v = Nan(1); /*gErrorNan;*/ break;
						}
						fContainer->NewCell(c, v, NULL);
						fContainer->SetCellStyleNr(c, fStyles[style]);
						break;
					}
					default:
						throw CErr("Illegal result type in formula. %d,%d",
							c.v, c.h);
				}
			}
			else
			{
				v = B_LENDIAN_TO_HOST_DOUBLE(*(double *)num);
				fContainer->NewCell(c, v, NULL);
				fContainer->SetCellStyleNr(c, fStyles[style]);
			}
			
			try
			{
				cell shared;
				CFormula form;
				shared.v = shared.h = 0;
				char o;

// grbit & 0x08 does not seem to work as advertised...
				
				es >> o;
				if (o == ptgExp)
				{
					es >> shared.v >> shared.h;
					
					int i;
					for (i = 0; i < fSharedFormulas.size(); i++)
					{
						if (fSharedFormulas[i].rwFirst == shared.v && fSharedFormulas[i].colFirst == shared.h)
							break;
					}
	
					if (i == fSharedFormulas.size())
						throw CErr("Could not find shared formula %d,%d for cell %d,%d",	
							shared.v, shared.h, c.v, c.h);				
					
					ParseXLFormula(form, c, shared, fSharedFormulas[i].p, fSharedFormulas[i].cce);
				}
				else
				{
					fBook.Seek(fBook.Position() - 1, SEEK_SET);
					void *p = malloc(cce);
					es.Read(p, cce);
					ParseXLFormula(form, c, shared, p, cce);
					free(p);
				}
				
				try
				{
					form.Calculate(c, v, fContainer);
					fContainer->SetValue(c, v);
				}
				catch (...)
				{
					int ix = 0;
					form.AddToken(valStr, "!ERROR", ix);
					form.AddToken(opEnd, NULL, ix);
				}

				fContainer->SetCellFormula(c, form.DetachString());
			}
			catch (CErr e) {
				puts(e);
//				e.DoError();
			}
			break;
		}
		case STRING:
		{
			char label[512];
			short l;
			
			es >> l;
			if (l > 511) l = 511;
			es.Read(label, l);
			label[l] = 0;

			v = label;
			fContainer->SetValue(c, v);
			break;
		}
		case MULRK:
		{
			long l;
			double d;
			
			es >> c.v >> c.h;
			c.v++; c.h++;
			
			int i = (len-4)/6;

			while (i--)
			{
				es >> style >> l;
				
				if (l & 0x02)
					d = (double)(l>>2);
				else
				{
					long long L = (l & 0xFFFFFFFC);
					L <<= 32;
					memcpy(&d, &L, 8);
				}
				if (l & 0x01)
					d /= 100;
				
				v = d;
				fContainer->NewCell(c, v, NULL);
				fContainer->SetCellStyleNr(c, fStyles[style]);
				c.h++;
			}
			break;
		}
		case MULBLANK:
		{
			es >> c.v; c.v++;
			es >> c.h; c.h++;
			
			int i = (len-4)/2;

			while (i--)
			{
				es >> style;
				fContainer->SetCellStyleNr(c, fStyles[style]);
				c.h++;
			}
			break;
		}
	}
} // CExcel5Filter::HandleXLRecordForPass2
