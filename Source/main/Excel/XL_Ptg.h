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

#define ptgExp	0x0001		//control
#define ptgTbl	0x0002		//control
#define ptgAdd	0x0003		//operator
#define ptgSub	0x0004		//operator
#define ptgMul	0x0005		//operator
#define ptgDiv	0x0006		//operator
#define ptgPower	0x0007		//operator
#define ptgConcat	0x0008		//operator
#define ptgLT	0x0009		//operator
#define ptgLE	0x000A		//operator
#define ptgEQ	0x000B		//operator
#define ptgGE	0x000C		//operator
#define ptgGT	0x000D		//operator
#define ptgNE	0x000E		//operator
#define ptgIsect	0x000F		//operator
#define ptgUnion	0x0010		//operator
#define ptgRange	0x0011		//operator
#define ptgUplus	0x0012		//operator
#define ptgUminus	0x0013		//operator
#define ptgPercent	0x0014		//operator
#define ptgParen	0x0015		//control
#define ptgMissArg	0x0016		//operand
#define ptgStr	0x0017		//operand
#define ptgAttr	0x0019		//control
#define ptgSheet	0x001A		//(ptg DELETED)
#define ptgEndSheet	0x001B		//(ptg DELETED)
#define ptgErr	0x001C		//operand
#define ptgBool	0x001D		//operand
#define ptgInt	0x001E		//operand
#define ptgNum	0x001F		//operand
#define ptgArray	0x0020		//operand, reference class
#define ptgFunc	0x0021		//operator
#define ptgFuncVar	0x0022		//operator
#define ptgName	0x0023		//operand, reference class
#define ptgRef	0x0024		//operand, reference class
#define ptgArea	0x0025		//operand, reference class
#define ptgMemArea	0x0026		//operand, reference class
#define ptgMemErr	0x0027		//operand, reference class
#define ptgMemNoMem	0x0028		//control
#define ptgMemFunc	0x0029		//control
#define ptgRefErr	0x002A		//operand, reference class
#define ptgAreaErr	0x002B		//operand, reference class
#define ptgRefN	0x002C		//operand, reference class
#define ptgAreaN	0x002D		//operand, reference class
#define ptgMemAreaN	0x002E		//control
#define ptgMemNoMemN	0x002F		//control
#define ptgNameX	0x0039		//operand, reference class
#define ptgRef3d	0x003A		//operand, reference class
#define ptgArea3d	0x003B		//operand, reference class
#define ptgRefErr3d	0x003C		//operand, reference class
#define ptgAreaErr3d	0x003D		//operand, reference class
#define ptgArrayV	0x0040		//operand, value class
#define ptgFuncV	0x0041		//operator
#define ptgFuncVarV	0x0042		//operator
#define ptgNameV	0x0043		//operand, value class
#define ptgRefV	0x0044		//operand, value class
#define ptgAreaV	0x0045		//operand, value class
#define ptgMemAreaV	0x0046		//operand, value class
#define ptgMemErrV	0x0047		//operand, value class
#define ptgMemNoMemV	0x0048		//control
#define ptgMemFuncV	0x0049		//control
#define ptgRefErrV	0x004A		//operand, value class
#define ptgAreaErrV	0x004B		//operand, value class
#define ptgRefNV	0x004C		//operand, value class
#define ptgAreaNV	0x004D		//operand, value class
#define ptgMemAreaNV	0x004E		//control
#define ptgMemNoMemNV	0x004F		//control
#define ptgFuncCEV	0x0058		//operator
#define ptgNameXV	0x0059		//operand, value class
#define ptgRef3dV	0x005A		//operand, value class
#define ptgArea3dV	0x005B		//operand, value class
#define ptgRefErr3dV	0x005C		//operand, value class
#define ptgAreaErr3dV	0x005D		//operand, value class
#define ptgArrayA	0x0060		//operand, array class
#define ptgFuncA	0x0061		//operator
#define ptgFuncVarA	0x0062		//operator
#define ptgNameA	0x0063		//operand, array class
#define ptgRefA	0x0064		//operand, array class
#define ptgAreaA	0x0065		//operand, array class
#define ptgMemAreaA	0x0066		//operand, array class
#define ptgMemErrA	0x0067		//operand, array class
#define ptgMemNoMemA	0x0068		//control
#define ptgMemFuncA	0x0069		//control
#define ptgRefErrA	0x006A		//operand, array class
#define ptgAreaErrA	0x006B		//operand, array class
#define ptgRefNA	0x006C		//operand, array class
#define ptgAreaNA	0x006D		//operand, array class
#define ptgMemAreaNA	0x006E		//control
#define ptgMemNoMemNA	0x006F		//control
#define ptgFuncCEA	0x0078		//operator
#define ptgNameXA	0x0079		//operand, array class (NEW ptg)
#define ptgRef3dA	0x007A		//operand, array class (NEW ptg)
#define ptgArea3dA	0x007B		//operand, array class (NEW ptg)
#define ptgRefErr3dA	0x007C		//operand, array class (NEW ptg)
#define ptgAreaErr3dA	0x007D		//operand, array class (NEW ptg)

#define ptgExp_txt	"ptgExp"
#define ptgTbl_txt	"ptgTbl"
#define ptgAdd_txt	"ptgAdd"
#define ptgSub_txt	"ptgSub"
#define ptgMul_txt	"ptgMul"
#define ptgDiv_txt	"ptgDiv"
#define ptgPower_txt	"ptgPower"
#define ptgConcat_txt	"ptgConcat"
#define ptgLT_txt	"ptgLT"
#define ptgLE_txt	"ptgLE"
#define ptgEQ_txt	"ptgEQ"
#define ptgGE_txt	"ptgGE"
#define ptgGT_txt	"ptgGT"
#define ptgNE_txt	"ptgNE"
#define ptgIsect_txt	"ptgIsect"
#define ptgUnion_txt	"ptgUnion"
#define ptgRange_txt	"ptgRange"
#define ptgUplus_txt	"ptgUplus"
#define ptgUminus_txt	"ptgUminus"
#define ptgPercent_txt	"ptgPercent"
#define ptgParen_txt	"ptgParen"
#define ptgMissArg_txt	"ptgMissArg"
#define ptgStr_txt	"ptgStr"
#define ptgAttr_txt	"ptgAttr"
#define ptgSheet_txt	"ptgSheet"
#define ptgEndSheet_txt	"ptgEndSheet"
#define ptgErr_txt	"ptgErr"
#define ptgBool_txt	"ptgBool"
#define ptgInt_txt	"ptgInt"
#define ptgNum_txt	"ptgNum"
#define ptgArray_txt	"ptgArray"
#define ptgFunc_txt	"ptgFunc"
#define ptgFuncVar_txt	"ptgFuncVar"
#define ptgName_txt	"ptgName"
#define ptgRef_txt	"ptgRef"
#define ptgArea_txt	"ptgArea"
#define ptgMemArea_txt	"ptgMemArea"
#define ptgMemErr_txt	"ptgMemErr"
#define ptgMemNoMem_txt	"ptgMemNoMem"
#define ptgMemFunc_txt	"ptgMemFunc"
#define ptgRefErr_txt	"ptgRefErr"
#define ptgAreaErr_txt	"ptgAreaErr"
#define ptgRefN_txt	"ptgRefN"
#define ptgAreaN_txt	"ptgAreaN"
#define ptgMemAreaN_txt	"ptgMemAreaN"
#define ptgMemNoMemN_txt	"ptgMemNoMemN"
#define ptgNameX_txt	"ptgNameX"
#define ptgRef3d_txt	"ptgRef3d"
#define ptgArea3d_txt	"ptgArea3d"
#define ptgRefErr3d_txt	"ptgRefErr3d"
#define ptgAreaErr3d_txt	"ptgAreaErr3d"
#define ptgArrayV_txt	"ptgArrayV"
#define ptgFuncV_txt	"ptgFuncV"
#define ptgFuncVarV_txt	"ptgFuncVarV"
#define ptgNameV_txt	"ptgNameV"
#define ptgRefV_txt	"ptgRefV"
#define ptgAreaV_txt	"ptgAreaV"
#define ptgMemAreaV_txt	"ptgMemAreaV"
#define ptgMemErrV_txt	"ptgMemErrV"
#define ptgMemNoMemV_txt	"ptgMemNoMemV"
#define ptgMemFuncV_txt	"ptgMemFuncV"
#define ptgRefErrV_txt	"ptgRefErrV"
#define ptgAreaErrV_txt	"ptgAreaErrV"
#define ptgRefNV_txt	"ptgRefNV"
#define ptgAreaNV_txt	"ptgAreaNV"
#define ptgMemAreaNV_txt	"ptgMemAreaNV"
#define ptgMemNoMemNV_txt	"ptgMemNoMemNV"
#define ptgFuncCEV_txt	"ptgFuncCEV"
#define ptgNameXV_txt	"ptgNameXV"
#define ptgRef3dV_txt	"ptgRef3dV"
#define ptgArea3dV_txt	"ptgArea3dV"
#define ptgRefErr3dV_txt	"ptgRefErr3dV"
#define ptgAreaErr3dV_txt	"ptgAreaErr3dV"
#define ptgArrayA_txt	"ptgArrayA"
#define ptgFuncA_txt	"ptgFuncA"
#define ptgFuncVarA_txt	"ptgFuncVarA"
#define ptgNameA_txt	"ptgNameA"
#define ptgRefA_txt	"ptgRefA"
#define ptgAreaA_txt	"ptgAreaA"
#define ptgMemAreaA_txt	"ptgMemAreaA"
#define ptgMemErrA_txt	"ptgMemErrA"
#define ptgMemNoMemA_txt	"ptgMemNoMemA"
#define ptgMemFuncA_txt	"ptgMemFuncA"
#define ptgRefErrA_txt	"ptgRefErrA"
#define ptgAreaErrA_txt	"ptgAreaErrA"
#define ptgRefNA_txt	"ptgRefNA"
#define ptgAreaNA_txt	"ptgAreaNA"
#define ptgMemAreaNA_txt	"ptgMemAreaNA"
#define ptgMemNoMemNA_txt	"ptgMemNoMemNA"
#define ptgFuncCEA_txt	"ptgFuncCEA"
#define ptgNameXA_txt	"ptgNameXA"
#define ptgRef3dA_txt	"ptgRef3dA"
#define ptgArea3dA_txt	"ptgArea3dA"
#define ptgRefErr3dA_txt	"ptgRefErr3dA"
#define ptgAreaErr3dA_txt	"ptgAreaErr3dA"

#define PTG_BASE(ptg) (((ptg&0x40)?(ptg|0x20):ptg)&0x3F)

const char ptgLen[] =
{
	0, // filler
	4, // ptgExp
	4, // ptgTbl
	0, // ptgAdd
	0, // ptgSub
	0, // ptgMul
	0, // ptgDiv
	0, // ptgPower
	0, // ptgConcat
	0, // ptgLT
	0, // ptgLE
	0, // ptgEQ
	0, // ptgGE
	0, // ptgGT
	0, // ptgNE
	0, // ptgIsect
	0, // ptgUnion
	0, // ptgRange
	0, // ptgUplus
	0, // ptgUminus
	0, // ptgPercent
	0, // ptgParen
	0, // ptgMissArg
	1, // ptgStr
	0, // filler
	3, // ptgAttr
	0, // ptgSheet
	0, // ptgEndSheet
	1, // ptgErr
	1, // ptgBool
	2, // ptgInt
	8, // ptgNum

	7, // ptgArray
	2, // ptgFunc
	3, // ptgFuncVar
	14, // ptgName
	3, // ptgRef
	6, // ptgArea
	6, // ptgMemArea
	0, // ptgMemErr
	6, // ptgMemNoMem
	2, // ptgMemFunc
	0, // ptgRefErr
	0, // ptgAreaErr
	3, // ptgRefN
	6, // ptgAreaN
	2, // ptgMemAreaN
	2, // ptgMemNoMemN
	0, 0, 0, 0, 0, 0, 0, 0, 0, // fillers
	24, // ptgNameX
	17, // ptgRef3d
	20, // ptgArea3d
	0, // ptgRefErr3d
	0 // ptgAreaErr3d
};
