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
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   EXCEL_H
#include "Excel.h"
#endif

#ifndef   XL_PTG_H
#include "XL_Ptg.h"
#endif

#ifndef   MYMATH_H
#include "MyMath.h"
#endif

#ifndef   FUNCTIONUTILS_H
#include "FunctionUtils.h"
#endif


#include <support/Debug.h>

#define VFIXED	0x4000
#define HFIXED	0x2000

const uchar
	kPtgMap[] = {
		0,
		0,		//ptgExp	0x0001		//control
		0,		//ptgTbl	0x0002		//control
		opPlus,	//ptgAdd	0x0003		//operator
		opMinus,	//ptgSub	0x0004		//operator
		opMul,	//ptgMul	0x0005		//operator
		opDiv,	//ptgDiv	0x0006		//operator
		opRaise,	//ptgPower	0x0007		//operator
		opPlus,	//ptgConcat	0x0008		//operator
		opLT,	//ptgLT	0x0009		//operator
		opLE, 	//ptgLE	0x000A		//operator
		opEQ,	//ptgEQ	0x000B		//operator
		opGE,	//ptgGE	0x000C		//operator
		opGT,	//ptgGT	0x000D		//operator
		opNE,	//ptgNE	0x000E		//operator
		0,		//ptgIsect	0x000F		//operator
		0, 		//ptgUnion	0x0010		//operator
		0,		//ptgRange	0x0011		//operator
		0,		//ptgUplus	0x0012		//operator
		opNegate,	//ptgUminus	0x0013		//operator
		0,		//ptgPercent	0x0014		//operator
		opParen,	//ptgParen	0x0015		//control
		valNil,	//ptgMissArg	0x0016		//operand
		valStr,	//ptgStr	0x0017		//operand
		0,
		0,		//ptgAttr	0x0019		//control
		0,		//ptgSheet	0x001A		//(ptg DELETED)
		0,		//ptgEndSheet	0x001B		//(ptg DELETED)
		valNum,	//ptgErr	0x001C		//operand
		valBool,	//ptgBool	0x001D		//operand
		valNum,	//ptgInt	0x001E		//operand
		valNum,	//ptgNum	0x001F		//operand
		0,		//ptgArray	0x0020		//operand, reference class
		opFunc,	//ptgFunc	0x0021		//operator
		opFunc,	//ptgFuncVar	0x0022		//operator
		0,		//ptgName	0x0023		//operand, reference class
		valCell,	//ptgRef	0x0024		//operand, reference class
		valRange,	//ptgArea	0x0025		//operand, reference class
		0,		//ptgMemArea	0x0026		//operand, reference class
		valCell,	//ptgMemErr	0x0027		//operand, reference class
		valCell,	//ptgMemNoMem	0x0028		//control
		0,		//ptgMemFunc	0x0029		//control
		valCell,	//ptgRefErr	0x002A		//operand, reference class
		valRange,	//ptgAreaErr	0x002B		//operand, reference class
		valCell,	//ptgRefN	0x002C		//operand, reference class
		valRange,	//ptgAreaN	0x002D		//operand, reference class
		0,		//ptgMemAreaN	0x002E		//control
		0,		//ptgMemNoMemN	0x002F		//control
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,		//ptgNameX	0x0039		//operand, reference class
		0, 		//ptgRef3d	0x003A		//operand, reference class
		0,		//ptgArea3d	0x003B		//operand, reference class
		0,		//ptgRefErr3d	0x003C		//operand, reference class
		0		//ptgAreaErr3d	0x003D		//operand, reference class
	};

void Read(uchar *s, int indx, short& t)
{
	memcpy(&t, s + indx, sizeof(t));
	t = B_LENDIAN_TO_HOST_INT16(t);
} // Read<char>

void Read(uchar *s, int indx, unsigned short& t)
{
	memcpy(&t, s + indx, sizeof(t));
	t = B_LENDIAN_TO_HOST_INT16(t);
} // Read<char>

void Read(uchar *s, int indx, long& t)
{
	memcpy(&t, s + indx, sizeof(t));
	t = B_LENDIAN_TO_HOST_INT32(t);
} // Read<char>

void Read(uchar *s, int indx, unsigned long& t)
{
	memcpy(&t, s + indx, sizeof(t));
	t = B_LENDIAN_TO_HOST_INT32(t);
} // Read<char>

void Read(uchar *s, int indx, double& t)
{
	memcpy(&t, s + indx, sizeof(t));
	t = B_LENDIAN_TO_HOST_DOUBLE(t);
} // Read<char>

static void MapFunction(FuncCallData& fcd)
{
	switch (fcd.funcNr)
	{
		case 0x0000: fcd.funcNr = kCOUNTFuncNr; break;
		case 0x0001: fcd.funcNr = kIFFuncNr; break;
		
		case 0x0004: fcd.funcNr = kSUMFuncNr; break;
		
		case 0x0006: fcd.funcNr = kMINFuncNr; break;
		case 0x0007: fcd.funcNr = kMAXFuncNr; break;
		case 0x0008: fcd.funcNr = kROWFuncNr; break;
		case 0x0009: fcd.funcNr = kCOLUMNFuncNr; break;

		case 0x000b: fcd.funcNr = kNPVFuncNr; break;

		case 0x000f: fcd.funcNr = kSINFuncNr; fcd.argCnt = 1; break;
		case 0x0010: fcd.funcNr = kCOSFuncNr; fcd.argCnt = 1; break;
		case 0x0011: fcd.funcNr = kTANFuncNr; fcd.argCnt = 1; break;
		case 0x0012: fcd.funcNr = kATANFuncNr; fcd.argCnt = 1; break;

		case 0x0014: fcd.funcNr = kSQRTFuncNr; fcd.argCnt = 1; break;
		case 0x0015: fcd.funcNr = kEXPFuncNr; fcd.argCnt = 1; break;
		case 0x0016: fcd.funcNr = kLNFuncNr; fcd.argCnt = 1; break;

		case 0x0018: fcd.funcNr = kABSFuncNr; fcd.argCnt = 1; break;
		case 0x0019: fcd.funcNr = kINTFuncNr; fcd.argCnt = 1; break;
		case 0x001a: fcd.funcNr = kSIGNFuncNr; fcd.argCnt = 1; break;
		case 0x001b: fcd.funcNr = kROUNDFuncNr; fcd.argCnt = 2; break;

		case 0x001f: fcd.funcNr = kMIDFuncNr; fcd.argCnt = 3; break;

		case 0x0024: fcd.funcNr = kANDFuncNr; break;
		case 0x0025: fcd.funcNr = kORFuncNr; break;

		case 0x0027: fcd.funcNr = kMODFuncNr; fcd.argCnt = 1; break;

		case 0x0038: fcd.funcNr = kPVFuncNr; break;
		case 0x0039: fcd.funcNr = kFVFuncNr; break;

		case 0x003b: fcd.funcNr = kPMTFuncNr; break;

		case 0x003e: fcd.funcNr = kIRRFuncNr; break;

		case 0x0041: fcd.funcNr = kDATEFuncNr; fcd.argCnt = 3; break;
		case 0x0042: fcd.funcNr = kTIMEFuncNr; fcd.argCnt = 3; break;
		case 0x0043: fcd.funcNr = kDAYFuncNr; fcd.argCnt = 1; break;
		case 0x0044: fcd.funcNr = kMONTHFuncNr; fcd.argCnt = 1; break;
		case 0x0045: fcd.funcNr = kYEARFuncNr; fcd.argCnt = 1; break;

		case 0x0047: fcd.funcNr = kHOURFuncNr; fcd.argCnt = 1; break;
		case 0x0048: fcd.funcNr = kMINUTEFuncNr; fcd.argCnt = 1; break;
		case 0x0049: fcd.funcNr = kSECONDFuncNr; fcd.argCnt = 1; break;

		case 0x004e: fcd.funcNr = kOFFSETFuncNr; break;
		
		case 0x0061: fcd.funcNr = kATAN2FuncNr; break;
		case 0x0062: fcd.funcNr = kASINFuncNr; fcd.argCnt = 1; break;
		case 0x0063: fcd.funcNr = kACOSFuncNr; fcd.argCnt = 1; break;
		case 0x0064: fcd.funcNr = kCHOOSEFuncNr; break;
		case 0x0065: fcd.funcNr = kHLOOKUPFuncNr; break;
		case 0x0066: fcd.funcNr = kVLOOKUPFuncNr; break;
		case 0x006d: fcd.funcNr = kLOGFuncNr; break;

		case 0x0073: fcd.funcNr = kLEFTFuncNr; break;
		case 0x0074: fcd.funcNr = kRIGHTFuncNr; break;

		case 0x007F: fcd.funcNr = kISTEXTFuncNr; fcd.argCnt = 1; break;

		case 0x00d6: fcd.funcNr = kASCFuncNr; fcd.argCnt = 1; break;
		
		case 0x00a9: fcd.funcNr = kCOUNTAFuncNr; break; 	// ? excel 2007 and later - fcd.argCnt = 255; 
															// ? excel 2003 and earlier - fcd.argCnt = 30;
		case 0x00f7: fcd.funcNr = kDBFuncNr; break;
		
		case 0x0151: fcd.funcNr = kPOWERFuncNr; break;
		
//		case 0x0---: fcd.funcNr = kDEC2HEXFuncNr; break; //find Excel's DEC2HEX function nr and map it !!!

		default: fcd.funcNr = -1;
	}

	if (fcd.funcNr >= 0)
		fcd.argCnt = gFuncArrayByNr[fcd.funcNr].argCnt;
	else
		fcd.argCnt = 0;
} // MapFunction

void CExcel5Filter::ParseXLFormula(CFormula& formula,
	cell inLoc, cell shared, const void *data, int len)
{
	int indx = 0;
	int ptg, basePtg, lastPtg;
	uchar *str = (uchar *)data;
	PFToken opCode;
	int funcStack[32], *funcStackPtr = funcStack;
	int argStack[32], *argStackPtr = argStack;
	FuncCallData fcd;
	short x;
	int offset = 0;

	*argStackPtr = 0;

	while (indx < len)
	{
		lastPtg = ptg;
		ptg = str[indx++];
		basePtg = PTG_BASE(ptg);

		opCode = (PFToken)kPtgMap[basePtg];
		
		switch(basePtg)
		{
			case ptgStr:
			{
				char st[256], len;
				
				len = str[indx];
				
				strncpy(st, (char *)str + indx + 1, len);
				st[len]=0;
				
				formula.AddToken(valStr, st, offset);

				indx+=len;
				break;
			}
			case ptgAttr:
			{
				if (str[indx] & 0x01)
					{};	// formule is niet constant...
				
				if (str[indx] & 0x02)
					{};	// optimized if function
				
				if (str[indx] & 0x04)
				{	// optimized choose
					Read(str, indx + 1, x);
					indx += 2 * (x + 1);	
				}

				if (str[indx] & 0x08)
					{};	// goto
				
				if (str[indx] & 0x10)
				{
					fcd.funcNr = kSUMFuncNr;
					fcd.argCnt = 1;
					formula.AddToken(opFunc, &fcd, offset);
				}
				
				if (str[indx] & 0x20)
					{}; // Basic style assignment ???
				
				if (str[indx] & 0x40)
					{}; // Formula contains spaces
				
				break;
			}
			case ptgErr:
			{
				double e = Nan(str[indx]);
				formula.AddToken(valNum, &e, offset);
				break;
			}
			case ptgBool:
			{
				bool b = (bool)str[indx];
				formula.AddToken(valBool, &b, offset);
				break;
			}
			case ptgInt:
			{
				Read(str, indx, x);
				double d = x;
				formula.AddToken(valNum, &d, offset);
				break;
			}
			case ptgNum:
			{
				double d;
				Read(str, indx, d);
				formula.AddToken(valNum, &d, offset);
				break;
			}
			case ptgPercent:
			{
//				buffer.Seek(buffer.Position() - 9, SEEK_SET);
//				opCode = valPerc;
//				buffer << opCode;
//				buffer.Seek(buffer.Position() + 8, SEEK_SET);
//				formula[(offset - sizeof(double)) / kPFWordSize - 1] = valPerc;
				break;
			}
			case ptgFunc:
			{
				Read(str, indx, fcd.funcNr);
				MapFunction(fcd);
				ASSERT(fcd.funcNr >= 0);
				formula.AddToken(opFunc, &fcd, offset);
				break;
			}
			case ptgFuncVar:
			{
				Read(str, indx + 1, fcd.funcNr);
				fcd.funcNr &= 0x7FFF;
				fcd.argCnt = str[indx] & 0x7F;
				
				if (fcd.funcNr != 0xFF)
				{
					fcd.argCnt = -1;
					MapFunction(fcd);
					ASSERT(fcd.argCnt < 0 || fcd.argCnt == (str[indx] & 0x7F));
					fcd.argCnt = str[indx] & 0x7F;
				}
				else
				{
					fcd.funcNr = *--funcStackPtr;
					fcd.argCnt--;
				}
				formula.AddToken(opFunc, &fcd, offset);
				break;
			}
			case ptgName:
			{
				short i;
				Read(str, indx, i);
				i--;
				if (i < 0 || i >= (short)fNames.size())
					THROW(("Name reference to void %d,%d", inLoc.v, inLoc.h));
				if (fNames[i].type == xnFunction)
					*funcStackPtr++ = fNames[i].funcNr;
				else
				{
					opCode = valName;
					formula.AddToken(valName, fNames[i].namedRef, offset);
				}
				break;
			}
			case ptgRef:
			{
				cell c;
				Read(str, indx, x);
				
				if (x & 0x8000)
					c.v = (x & 0x3FFF) - inLoc.v + 1;
				else
					c.v = (x & 0x3FFF) + 1;

				if (x & 0x4000)
					c.h = *((char *)str + indx + 2) - inLoc.h + 1;
				else
					c.h = *((char *)str + indx + 2) + 1;
				
				if ((x & 0x8000) == 0) c.h ^= VFIXED;
				if ((x & 0x4000) == 0) c.h ^= HFIXED;
				
				formula.AddToken(valCell, &c, offset);
				break;
			}
			case ptgArea:
			{
				Read(str, indx, x);
				range r;
				
				if (x & 0x8000)
					r.top = (x & 0x3FFF) - inLoc.v + 1;
				else
					r.top = (x & 0x3FFF) + 1;

				if (x & 0x4000)
					r.left = *((char *)str + indx + 4) - inLoc.h + 1;
				else
					r.left = *((char *)str + indx + 4) + 1;
				
				if ((x & 0x8000)==0) r.left ^= VFIXED;
				if ((x & 0x4000)==0) r.left ^= HFIXED;

				Read(str, indx + 2, x);
				
				if (x & 0x8000)
					r.bottom = (x & 0x3FFF) - inLoc.v + 1;
				else
					r.bottom = (x & 0x3FFF) + 1;

				if (x & 0x4000)
					r.right = *((char *)str + indx + 5) - inLoc.h + 1;
				else
					r.right = *((char *)str + indx + 5) + 1;
				
				if ((x & 0x8000)==0) r.right ^= VFIXED;
				if ((x & 0x4000)==0) r.right ^= HFIXED;
				
				formula.AddToken(valRange, &r, offset);
				break;
			}
			case ptgRefErr:
			case ptgMemErr:
			case ptgMemNoMem:
			{
				cell c;
				c.v = c.h = 0;
				formula.AddToken(valCell, &c, offset);
				break;
			}
			case ptgRefN:
			{
				Read(str, indx, x);
				cell c;
				
				if (x & 0x2000)
					c.v = x | 0xC000;
				else
					c.v = x & 0x3FFF;
					
				c.v %= 0x3FFF;
				c.h = *((char *)str + indx + 2);
				
				if ((x&0x8000)==0)
				{
					c.v++;
					c.h ^= VFIXED;
				}
				if ((x&0x4000)==0)
					c.h ^= HFIXED;
				
				formula.AddToken(valCell, &c, offset);
				break;
			}
			case ptgAreaN:
			{
				Read(str, indx, x);
				range r;
				
				if (x & 0x2000)
					r.top = x | 0xC000;
				else
					r.top = x & 0x3FFF;
					
				r.top %= 0x3FFF;
				
				Read(str, indx + 2, x);

				if (x & 0x2000)
					r.bottom = x | 0xC000;
				else
					r.bottom = x & 0x3FFF;
					
				r.bottom %= 0x3FFF;
				
				r.left = *((char *)str + indx + 4);
				r.right = *((char *)str + indx + 5);
				
				formula.AddToken(valRange, &r, offset);
				break;
			}
			default:
				if (opCode != opEnd)
					formula.AddToken(opCode, NULL, offset);
//				else if (indx != len)
//					THROW(("Unable to parse Excel formula for cell %d,%d", inLoc.h, inLoc.v));
		}

		indx += ptgLen[basePtg];
	}
	
	formula.AddToken(opEnd, NULL, offset);
} // ParseXLString
