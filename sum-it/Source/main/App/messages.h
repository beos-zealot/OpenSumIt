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
	messages.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

const uint32
	msg_NewWindow 			= 'NewW',
	msg_OpenWindow			= 'OpnW',
	msg_CloseWindow			= 'ClsW',
	msg_Save				= 'Save',
	msg_SaveAs				= 'SvAs',
	msg_FileFormat			= 'FFrm',
	msg_Revert				= 'Rvrt',
	
	msg_Print					= 'Prnt',
	msg_PageSetup				= 'Pgsu',
	
	msg_WindowOptionsCmd	= 'WOpC',
	msg_WindowOptions		= 'WOpt',
	
	msg_Undo				= 'Undo',
	msg_Redo				= 'Redo',
	msg_PasteSpecial		= 'Pspc',
	msg_FunctionChosen		= 'FCho',
	msg_PasteFunction		= 'Pfnc',
	msg_Clear				= 'Cler',
	msg_SelectAll			= 'SAll',

	msg_FontSelected 		= 'FSel',
	msg_StyleSelected		= 'Styl',
	msg_SizeSelected		= 'Size',
	msg_TextColorSelected	= 'TClr',
	msg_CellColorSelected	= 'CClr',
	
	msg_TextColorOther		= 'TCot',
	msg_CellColorOther		= 'CCot',

	msg_FillDown			= 'FilD',
	msg_FillRight			= 'FilR',
	msg_InsertRC			= 'InRC',
	msg_DeleteRC			= 'DlRC',
	msg_TextEntered			= 'Entr',
	msg_TextCancelled		= 'Cncl',
	msg_ProgressInfo		= 'Prgs',
	
	msg_AlignLeft			= 'Alft',
	msg_AlignCenter			= 'Acnt',
	msg_AlignRight			= 'Argt',
	msg_AlignGeneral		= 'Agnr',
	
	msg_FormatGeneral		= 'Fgnr',
	msg_FormatCurrency		= 'Fcur',
	msg_FormatPercentage	= 'Fprc',
	msg_FormatFixed			= 'Ffix',
	msg_FormatScientific	= 'Fsci',
//	msg_FormatFormula		= 'Ffrm',
	msg_FormatDateTime		= 'Fdat',
	msg_FormatCommas		= 'Fcms',
	msg_FormatDigitsCmd		= 'FdCm',
	msg_FormatDigits		= 'Fdig',

	msg_Recalculate			= 'Rclc',
	msg_SortCmd				= 'SrCm',
	msg_Sort				= 'Sort',
	msg_FillCmd				= 'FiCm',
	msg_Fill				= 'Fill',
	
	msg_WindowAdded 		= 'WAdd',
	msg_WindowRemoved		= 'WRmv',
	msg_NameCmd				= 'NmCm',
	msg_AddName				= 'ANam',
	msg_RemoveName			= 'RNam',
	msg_NameListChange		= 'LNam',
	msg_FillInRange			= 'FRng',
	
	msg_DragCells			= 'DrgC',
	msg_NavigateKey			= 'NavK',
	
	msg_GoToCmd				= 'GoTC',
	msg_GoTo				= 'GoTo',
	
	msg_FindCmd				= 'FinC',
	msg_FindAgainCmd		= 'FinA',
	msg_Find				= 'Find',
	msg_Replace			= 'Rplc',
	msg_ReplaceAndFind		= 'RpFn',
	msg_ReplaceAll		= 'RpAl',
	
	msg_DisplayFormulas		= 'DFrm',
	
	msg_PrefsCmd			= 'Pref',
	msg_PrefsChanged		= 'PrCh',
	
	msg_CalcOptions			= 'COpt',
	msg_ViewOptions			= 'VOpt',

	msg_Tile				= 'Tile',
	msg_TileVertical		= 'TilV',
	msg_Stack				= 'Stck',
	msg_Zoom				= 'Zoom',
	
	msg_Freeze				= 'Frez',
	msg_UnFreeze			= 'UnFr',
	
	msg_Grid				= 'Grid',
	msg_Head				= 'Head',
	
	msg_ShowClipboard		= 'ShCl',

	msg_SelectWindow		= 'SelW',
	
	msg_ChangeBorderFont	= 'BChg',
	msg_ChangeBorderFontFamily	= 'BChF',
	msg_ChangeBorderFontSize	= 'BChS',
	
	msg_CreateGraph		= 'CGrp',
	msg_GraphOptions		= 'GOpt',
	msg_GraphInit			= 'GIni',
	
	msg_FileRead			= 'Read',
	
	msg_SumButton			= 'Sum ',
	msg_Command			= 'Cmnd',
	
	msg_Help				= 'Help';

