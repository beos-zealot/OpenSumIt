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
	CellView.commands.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

//#ifndef   DRAWUTILS_H
//#include "DrawUtils.h"
//#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   CELLCOMMANDS_H
#include "CellCommands.h"
#endif

//#ifndef   RESIZECOMMANDS_H
//#include "ResizeCommands.h"
//#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLITERATOR_H
#include "CellIterator.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   DIGITSDIALOG_H
#include "DigitsDialog.h"
#endif

#ifndef   COPYCOMMANDS_H
#include "CopyCommands.h"
#endif

#ifndef   MYCLIPBOARD_H
#include "MyClipboard.h"
#endif

#ifndef   PASTESPECIALDIALOG_H
#include "PasteSpecialDialog.h"
#endif

//#ifndef   DRAGCOMMAND_H
//#include "DragCommand.h"
//#endif

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
#endif

#ifndef   SORTDIALOG_H
#include "SortDialog.h"
#endif

#ifndef   FILLDIALOG_H
#include "FillDialog.h"
#endif

#ifndef   NAMESDIALOG_H
#include "NamesDialog.h"
#endif

//#ifndef   FORMULA_H
//#include "Formula.h"
//#endif

//#ifndef   MTHREAD_H
//#include "MThread.h"
//#endif

//#ifndef   CALCULATEJOB_H
//#include "CalculateJob.h"
//#endif
#ifndef   CALCDEFS_H
#include "CalcDefs.h"
#endif

#ifndef   TEXT2CELLS_H
#include "Text2Cells.h"
#endif

//#ifndef   GOTODIALOG_H
//#include "GotoDialog.h"
//#endif

#ifndef   NAMETABLE_H
#include "NameTable.h"
#endif

#ifndef   FINDDIALOG_H
#include "FindDialog.h"
#endif

#ifndef   SORTCOMMAND_H
#include "SortCommand.h"
#endif

#ifndef   FILLCOMMAND_H
#include "FillCommand.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   DATEFORMATDIALOG_H
#include "DateFormatDialog.h"
#endif

#ifndef   ANIMATEGRAPHDIALOG_H
#include "AnimateGraphDialog.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

//#ifndef   BTREE_T
//#include "BTree.t"
//#endif

#ifndef   COLORMENUITEM_H
#include "ColorMenuItem.h"
#endif

#ifndef   SELECTIONVIEW_H
#include "SelectionView.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   SUM_IT_H
#include "Sum-It.h"
#endif

#ifndef   STYLECOMMAND_H
#include "StyleCommand.h"
#endif

#ifndef   RES5_H
#include "res5.h"
#endif

#ifndef   HCOLORPICKERINTF_H
#include "HColorPickerIntf.h"
#endif

//#ifndef   FINDCOMMANDS_H
//#include "FindCommands.h"
//#endif

#ifndef   CHOOSEGRAPHDIALOG_H
#include "ChooseGraphDialog.h"
#endif

#ifndef   GRAPHPLUGIN_H
#include "GraphPlugIn.h"
#endif

#ifndef   GRAPHICCOMMANDS_H
#include "GraphicCommands.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#include <support/Debug.h>
#include <ctype.h>
#include <MenuBar.h>

void CCellView::MessageReceived(BMessage *inMessage)
{
	CCellWindow *wind = (CCellWindow *)Window();
	CellStyle cs, ns;
	
	fContainer->Lock();
	fContainer->GetCellStyle(fCurCell, cs);
	
	CFormatter cf(cs.fFormat);
	if (cf.FormatID() == -1)
		cf = gFormatTable[cs.fFormat];
		
	fContainer->GetColumnStyle(fSelection.left, ns);
	fContainer->Unlock();
	bool makesDirty = true;
	
	try
	{
		if (inMessage->WasDropped())
		{
			HandleDrop(inMessage);
			return;
		}
		
		switch (inMessage->what)
		{
			case B_MESSAGE_NOT_UNDERSTOOD:
				if (inMessage->IsSourceWaiting())
				{
					BMallocIO buf;
					CTextConverter conv(buf, fContainer);
					conv.ConvertToText(NULL);
					char c = 0;
					buf.Write(&c, 1);
					
					BMessage *reply = new BMessage(B_SIMPLE_DATA);

					reply->AddString("text", (char *)buf.Buffer());
					inMessage->SendReply(reply);
				}
				makesDirty = false;
				break;
			
			case B_COPY:
				if (GraphicHasFocus())
					wind->RegisterCommand(new CCopyGraphicCommand(this));
				else
					wind->RegisterCommand(new CCopyCommand(this, fContainer));
				makesDirty = false;
				break;
			
			case B_CUT:
				if (GraphicHasFocus())
					wind->RegisterCommand(new CCutGraphicCommand(this));
				else
					wind->RegisterCommand(new CCutCommand(this, fContainer));
				break;
			
			case B_PASTE:
				wind->RegisterCommand(new CPasteCommand(this, fContainer,
						true, true,
						fSelection.top != fSelection.bottom ||
						fSelection.left != fSelection.right));
				break;
			
			case msg_PasteSpecial:
				new CPasteSpecialDialog(this, fContainer);
				makesDirty = false;
				break;
			
			case msg_PasteFunction:
				if (!fEntering)
					fEditBox->MakeFocus();
				wind->GetPasteFunctionDialog();
				makesDirty = false;
				break;
			
			case msg_Clear:
				if (GraphicHasFocus())
					wind->RegisterCommand(new CDeleteGraphicCommand(this));
				else
					wind->RegisterCommand(new CClearCommand(this, fContainer));
				break;
			
			case msg_FontSelected:
			{
				BMenuItem *menu;
				inMessage->FindPointer("source", (void **)&menu);
				wind->RegisterCommand(new CFontStyleCommand(this, fContainer, menu->Label()));
				break;
			}
				
			case msg_StyleSelected:
			{
				BMenuItem *menu;
				inMessage->FindPointer("source", (void **)&menu);
				wind->RegisterCommand(new CFaceStyleCommand(this, fContainer, menu->Label()));
				break;
			}
				
			case msg_SizeSelected:
			{
				BMenuItem *menu;
				inMessage->FindPointer("source", (void **)&menu);
				float newSize = atof_i(menu->Label());
				wind->RegisterCommand(new CSizeStyleCommand(this, fContainer, newSize));
				break;
			}
			
			case msg_TextColorSelected:
			{
				ssize_t l;
				rgb_color *newColor;
				inMessage->FindData("color", B_RGB_COLOR_TYPE, (const void **)&newColor, &l);
				wind->RegisterCommand(new CTextColorStyleCommand(this, fContainer, *newColor));
				break;
			}
			
			case msg_TextColorOther:
			{
				CColorPicker *p = new CColorPicker(CColorPicker::NicePosition(),
					GetMessage(msgChooseColor), false);
				
				BMessage msg(msg_TextColorSelected);
				rgb_color c = gFontSizeTable[cs.fFont].FontColor();
				msg.AddData("color", B_RGB_COLOR_TYPE, &c, 4);
				
				p->Connect(&msg, this);
				break;
			}
			
			case msg_CellColorSelected:
			{
				ssize_t l;
				rgb_color *newColor;
				inMessage->FindData("color", B_RGB_COLOR_TYPE, (const void **)&newColor, &l);
				wind->RegisterCommand(new CLowColorStyleCommand(this, fContainer, *newColor));
				break;
			}
			
			case msg_CellColorOther:
			{
				CColorPicker *p = new CColorPicker(CColorPicker::NicePosition(),
					GetMessage(msgChooseColor), false);
				
				BMessage msg(msg_CellColorSelected);
				msg.AddData("color", B_RGB_COLOR_TYPE, &cs.fLowColor, 4);
				
				p->Connect(&msg, this);
				break;
			}
			
			case msg_AlignGeneral:
				wind->RegisterCommand(new CAlignmentStyleCommand(this, fContainer, eAlignGeneral));
				break;
					
			case msg_AlignLeft:
				wind->RegisterCommand(new CAlignmentStyleCommand(this, fContainer, eAlignLeft));
				break;
					
			case msg_AlignCenter:
				wind->RegisterCommand(new CAlignmentStyleCommand(this, fContainer, eAlignCenter));
				break;
					
			case msg_AlignRight:
				wind->RegisterCommand(new CAlignmentStyleCommand(this, fContainer, eAlignRight));
				break;
			
			case msg_FormatGeneral:
				wind->RegisterCommand(new CFormatStyleCommand(this, fContainer, eGeneral));
				break;
			
			case msg_FormatCurrency:
				wind->RegisterCommand(new CFormatStyleCommand(this, fContainer, eCurrency));
				break;
			
			case msg_FormatScientific:
				wind->RegisterCommand(new CFormatStyleCommand(this, fContainer, eScientific));
				break;
			
			case msg_FormatFixed:
				wind->RegisterCommand(new CFormatStyleCommand(this, fContainer, eFixed));
				break;
			
			case msg_FormatPercentage:
				wind->RegisterCommand(new CFormatStyleCommand(this, fContainer, ePercent));
				break;
			
			case msg_FormatDateTime:
				CDateFormatDialog::Construct(wind, cs.fFormat);
				break;

			case msg_FormatDateTimeChanged:
				wind->RegisterCommand(new CTimeFormatStyleCommand(this, fContainer, inMessage->FindInt32("newdateformat")));
				break;

			case msg_FormatCommas:
			{
				BMenuItem *menu;
				inMessage->FindPointer("source", (void **)&menu);
				wind->RegisterCommand(new CCommasStyleCommand(this, fContainer, !menu->IsMarked()));
				break;
			}
			
			case msg_FormatDigitsCmd:
				wind->GetDigitsDialog(cf.Digits());
				makesDirty = false;
				break;
			
			case msg_FormatDigits:
				wind->RegisterCommand(new CDigitsStyleCommand(this, fContainer, inMessage->FindInt32("digits")));
				break;
			
			case msg_SortCmd:
				wind->GetSortDialog();
				makesDirty = false;
				break;
			
			case msg_Sort:
			{
				bool asc1, asc2, asc3, rows;
				long key1, key2, key3;
				
				asc1 = inMessage->FindBool("asc1");
				asc2 = inMessage->FindBool("asc2");
				asc3 = inMessage->FindBool("asc3");
				rows = inMessage->FindBool("rows");
				key1 = inMessage->FindInt32("key1");
				key2 = inMessage->FindInt32("key2");
				key3 = inMessage->FindInt32("key3");
				
				wind->RegisterCommand(new CSortCommand(this, fContainer,
					key1, key2, key3, asc1, asc2, asc3, rows));
				break;
			}
			
			case msg_FillCmd:
				wind->GetFillDialog();
				makesDirty = false;
				break;
			
			case msg_Fill:
			{
				double start, increment;
				start = inMessage->FindDouble("start");
				increment = inMessage->FindDouble("increment");
				wind->RegisterCommand(new CFillCommand(this, fContainer, inMessage));
				break;
			}
			
			case msg_SelectAll:
				SetSelection(range(1, 1, kColCount, kRowCount));
				makesDirty = false;
				break;
			
			case msg_GoToCmd:
				wind->GetGoToDialog();
				makesDirty = false;
				break;
			
			case msg_GoTo:
			{
				const range *r;
				ssize_t l;
				
				if (inMessage->FindData("range", 'data', (const void **)&r, &l) != B_NO_ERROR)
				{
					CName name(inMessage->FindString("name"));
					r = &(*fNames)[name];
				}
				
				if (r)
				{
					if (fEntering)
						fEditBox->EnterRange(*r);
					else
					{
						SetSelection(*r);
						ScrollToSelection();
					}
				}
				makesDirty = false;
				break;
			}
			
			case msg_FillDown:
				wind->RegisterCommand(new CFillDownCommand(this, fContainer));
				break;
				
			case msg_FillRight:
				wind->RegisterCommand(new CFillRightCommand(this, fContainer));
				break;
			
			case msg_InsertRC:
				wind->RegisterCommand(new CInsertCommand(this, fContainer));
				break;
			
			case msg_DeleteRC:
				wind->RegisterCommand(new CDeleteCommand(this, fContainer));
				break;
			
			case msg_Recalculate:
				CalculateAllCells(true);
				makesDirty = false;
				break;
			
			case msg_CalculationDone:
			{
				fJob = NULL;
				
				CSet *dirty;
				if (inMessage->FindPointer("dirtylines", (void **)&dirty) == B_OK)
				{
					fDirtyLines |= *dirty;
					delete dirty;
				}
				
				DrawAllLines();
				UpdateCharts();
				makesDirty = false;
				break;
			}
			
			case msg_NameCmd:
			{
				CNamesDialog *d = (CNamesDialog *)wind->GetNamesDialog();
				
				if (d)
				{
					BMessage msg(msg_FillInRange);
					msg.AddData("range", 'rang', &fSelection, sizeof(range));
					d->PostMessage(&msg);
				}
				makesDirty = false;
				break;
			}
			
			case msg_AddName:
			{
				ssize_t l;
				range *r;
				FailOSErr(inMessage->FindData("range", 'rang', (const void **)&r, &l),
					errMessageMissing);
				try
				{
					AddNamedRange(inMessage->FindString("name"), *r);
					wind->GetNamesDialog()->PostMessage(msg_NameListChange);
				}
				catch (CErr& e)
				{
					CATCHED;
					if (e != errDuplicateKey)
						e.DoError();
				}
				break;
			}
			
			case msg_RemoveName:
				try
				{
					RemoveNamedRange(inMessage->FindString("name"));
					wind->GetNamesDialog()->PostMessage(msg_NameListChange);
				}
				catch(CErr& e)
				{
					CATCHED;
				}
				break;
			
			case msg_FindCmd:
				gApp->FindDialog()->Show();
				gApp->FindDialog()->Activate();
				makesDirty = false;
				break;
			
			case msg_Find:
				makesDirty = false;
			case msg_Replace:
			case msg_ReplaceAndFind:
			case msg_ReplaceAll:
				Find(inMessage);
				break;
			
			case msg_QueryCanReplace:
			{
				BMessage reply(msg_ReplyCanReplace);
				reply.AddBool("canreplace", CanReplace(inMessage->FindString("what"),
					inMessage->FindBool("case")));
				inMessage->SendReply(&reply);
				makesDirty = false;
				break;
			}
			
			case msg_FindAgainCmd:
				if (fWhatToFind)
					FindNext(fWrap);
				else
				{
					gApp->FindDialog()->Show();
					gApp->FindDialog()->Activate();
				}
				makesDirty = false;
				break;
			
			case msg_NavigateKey:
				switch (inMessage->FindInt32("raw_char"))
				{
					case B_UP_ARROW: Step(stepUp); break;
					case B_DOWN_ARROW: Step(stepDown); break;
					case B_LEFT_ARROW: Step(stepLeft); break;
					case B_RIGHT_ARROW: Step(stepRight); break;
				}
				makesDirty = false;
				break;
			
			case msg_Grid:	
				fShowGrid = !fShowGrid;
				Invalidate(fCellBounds);
				break;
			
			case msg_Freeze:
				if (fSelection.left == 1 &&
					fSelection.top == 1 &&
					fSelection.bottom == kRowCount &&
					fSelection.right < kColCount)
				{
					fPosition.h += fSelection.right;
					fFrozen.h = fSelection.right;
				}
				else if (fSelection.top == 1 &&
					fSelection.left == 1 &&
					fSelection.right == kColCount &&
					fSelection.bottom < kRowCount)
				{
					fPosition.v += fSelection.bottom;
					fFrozen.v = fSelection.bottom;
				}
				AdjustScrollBars();
				Invalidate();
				break;
			
			case msg_UnFreeze:
				if (fSelection.left == 1 &&
					fSelection.top == 1 &&
					fSelection.bottom == kRowCount &&
					fSelection.right >= fFrozen.h)
				{
					fPosition.h -= fFrozen.h;
					fFrozen.h = 0;
				}
				if (fSelection.top == 1 &&
					fSelection.left == 1 &&
					fSelection.right == kColCount &&
					fSelection.bottom >= fFrozen.v)
				{
					fPosition.v -= fFrozen.h;
					fFrozen.v = 0;
				}
				AdjustScrollBars();
				Invalidate();
				break;
			
			case msg_Head:
				fShowBorders = !fShowBorders;
				fCellBounds = Bounds();
				if (fShowBorders)
				{
					fCellBounds.left += fBorderWidth;
					fCellBounds.top += fBorderHeight;
				}
				Invalidate();
				break;
			
			case msg_ChangeBorderFontFamily:
			{
				font_family bFamily;
				font_style bStyle;
				float bSize;
				gFontSizeTable.GetFontInfo(fBorderFontID, &bFamily, &bStyle, &bSize);
				
				const char *s;
				inMessage->FindString("family", &s);
				strcpy(bFamily, s);
				
				fBorderFontID = gFontSizeTable.GetFontID(
					bFamily, bStyle, bSize);
				RecalculateBorderWidths();
				Invalidate();
				break;
			}
				
			case msg_ChangeBorderFontSize:
			{
				font_family bFamily;
				font_style bStyle;
				float bSize;
				gFontSizeTable.GetFontInfo(fBorderFontID, &bFamily, &bStyle, &bSize);
				
				inMessage->FindFloat("size", &bSize);
				
				fBorderFontID = gFontSizeTable.GetFontID(
					bFamily, bStyle, bSize);
				RecalculateBorderWidths();
				Invalidate();
				break;
			}

			case msg_ChangeBorderFont:
				fBorderFontID = gFontSizeTable.GetFontID(
					inMessage->FindString("family"),
					inMessage->FindString("style"),
					inMessage->FindFloat("size"));
				RecalculateBorderWidths();
				Invalidate();
				break;
			
			case msg_DisplayFormulas:
				fShowFormulas = !fShowFormulas;
				Invalidate(fCellBounds);
				break;
			
			case msg_WindowOptions:
				ns.fFont = gFontSizeTable.GetFontID(
					inMessage->FindString("docfamily"),
					inMessage->FindString("docstyle"),
					inMessage->FindFloat("docsize"));
				fContainer->SetDefaultCellStyle(ns);
				fBorderFontID = gFontSizeTable.GetFontID(
					inMessage->FindString("borderfamily"),
					inMessage->FindString("borderstyle"),
					inMessage->FindFloat("bordersize"));
				fAutoRecalc = inMessage->FindBool("autorecalc");
				if (fDisplayZero != inMessage->FindBool("displayzero"))
				{
					fDisplayZero = !fDisplayZero;
					CalculateAllCells();
				}
				
				fShowBorders = inMessage->FindBool("disphead");
				fShowGrid = inMessage->FindBool("dispgrid");
				
				fPrGrid = inMessage->FindBool("prgrid");
				fPrBorders = inMessage->FindBool("prhead");
				
				RecalculateBorderWidths();
				Invalidate();
				fSelView->ChooseFont();
				break;
			
			case msg_GetSelection:
			{
				BMessage reply(msg_SetSelection);
				reply.AddData("selection", 'rang', &fSelection, sizeof(fSelection));
				inMessage->SendReply(&reply);
				break;
			}

			case msg_AnimateCell :
			{
				cell c;
				inMessage->FindInt32("cell", (int32 *)&c);
				if (!fEntering)
				{
					StWriteLock lock(fContainer);
					fContainer->SetValue(c, inMessage->FindDouble("value"));
					TouchLine(c.v);
					DrawStatus();
					CalculateAllCells(true);
				}
				break;
			}
			
			case msg_CreateGraph:
				new CChooseGraphDialog(wind, fSelection);
				makesDirty = false;
				break;
			
			case msg_DisplayGraph:
			{
				BRect r = fCellBounds;
				r.InsetBy(10, 10);
				r.OffsetTo(10, 10);
				CGraphPlugIn *g = CreateChart(fPosition, r,
					inMessage->FindString("graphname"), fSelection);
				wind->RegisterCommand(new CAddGraphicCommand(this, g));
				break;
			}
	
			case msg_GraphOptions:
			{
				CGraphPlugIn *plug = dynamic_cast<CGraphPlugIn*>(fFirstGraphic);
				if (plug)
					plug->DisplayOptions();
				makesDirty = false;
				break;
			}
	
			default:
				BView::MessageReceived(inMessage);
				makesDirty = false;
				break;
			
		}
	}
	
	catch(CErr& e)
	{
		CATCHED;
		e.DoError();
		return;
	}
	
	catch(...)
	{
		CATCHED;
		ASSERT(FALSE);
	}
	
	if (makesDirty)
		wind->SetDirty(true);
} /* CCellView::MessageReceived */

void CCellView::MenusBeginning()
{
	CCellWindow *cw = (CCellWindow *)Window();
	BMenuBar *mbar = (BMenuBar *)cw->FindView("mbar");
	BMenuItem *menuItem;
	CGraphPlugIn *plug =
		fFirstGraphic ? dynamic_cast<CGraphPlugIn*>(fFirstGraphic) : NULL;

	if (fEntering)
	{
		mbar->FindItem(msg_PasteFunction)->SetEnabled(true);
	}
	else
	{
		if (fSelection.left == 1 && fSelection.right == kColCount)
		{
			menuItem = mbar->FindItem(msg_InsertRC);
			menuItem->SetEnabled(true);
			menuItem->SetLabel(GetIndString(5, menu_InsertRow));
			menuItem = mbar->FindItem(msg_DeleteRC);
			menuItem->SetEnabled(true);
			menuItem->SetLabel(GetIndString(5, menu_DeleteRow));
		}
		else if (fSelection.top == 1 && fSelection.bottom == kRowCount)
		{
			menuItem = mbar->FindItem(msg_InsertRC);
			menuItem->SetEnabled(true);
			menuItem->SetLabel(GetIndString(5, menu_InsertCol));
			menuItem = mbar->FindItem(msg_DeleteRC);
			menuItem->SetEnabled(true);
			menuItem->SetLabel(GetIndString(5, menu_DeleteCol));
		}
		else
		{
			menuItem = mbar->FindItem(msg_InsertRC);
			menuItem->SetEnabled(false);
			menuItem = mbar->FindItem(msg_DeleteRC);
			menuItem->SetEnabled(false);
		}
	
		CellStyle cs;
		CFormatter cf;
		font_family fontFamily;
		font_style fontStyle;
		float fontSize;
//		int/*ValueType*/ type;
		bool hasData = true;
		
		cell c;
		fContainer->Lock();
		CCellIterator iter(fContainer, &fSelection);
		if (!iter.NextExisting(c))
		{
			c = fCurCell;
			hasData = false;
		}
	
		fContainer->GetCellStyle(c, cs);
//		type = fContainer->GetType(c);
//		hasData = (type != eNoData) || fContainer->CellHasFormula(c);
		fContainer->Unlock();
		rgb_color fontColor;
		gFontSizeTable.GetFontInfo(cs.fFont, &fontFamily, &fontStyle, &fontSize, &fontColor);

		if (cs.fFormat >= eFirstNewFormat)
			cf = gFormatTable[cs.fFormat];
		else
			cf = CFormatter(cs.fFormat);
		
		mbar->FindItem(msg_SelectAll)->SetEnabled(true);
	
		mbar->FindItem(B_CUT)->SetEnabled(hasData || GraphicHasFocus());
		mbar->FindItem(B_COPY)->SetEnabled(hasData || GraphicHasFocus());
		mbar->FindItem(B_PASTE)->SetEnabled(gClipboard->ContainsData());
		mbar->FindItem(msg_Clear)->SetEnabled(hasData || GraphicHasFocus());
		mbar->FindItem(msg_PasteSpecial)->SetEnabled(gClipboard->ContainsData());
//		mbar->FindItem(msg_PasteFunction)->SetEnabled(false);
	
		mbar->FindItem(msg_AlignGeneral)->SetEnabled(true);
		mbar->FindItem(msg_AlignGeneral)->SetMarked(cs.fAlignment == eAlignGeneral);
		mbar->FindItem(msg_AlignLeft)->SetEnabled(true);
		mbar->FindItem(msg_AlignLeft)->SetMarked(cs.fAlignment == eAlignLeft);
		mbar->FindItem(msg_AlignCenter)->SetEnabled(true);
		mbar->FindItem(msg_AlignCenter)->SetMarked(cs.fAlignment == eAlignCenter);
		mbar->FindItem(msg_AlignRight)->SetEnabled(true);
		mbar->FindItem(msg_AlignRight)->SetMarked(cs.fAlignment == eAlignRight);
	
		mbar->FindItem(msg_FormatGeneral)->SetEnabled(true);
		mbar->FindItem(msg_FormatGeneral)->SetMarked(cf.OldFormatID() == eGeneral);
		mbar->FindItem(msg_FormatCurrency)->SetEnabled(true);
		mbar->FindItem(msg_FormatCurrency)->SetMarked(cf.OldFormatID() == eCurrency);
		mbar->FindItem(msg_FormatPercentage)->SetEnabled(true);
		mbar->FindItem(msg_FormatPercentage)->SetMarked(cf.OldFormatID() == ePercent);
		mbar->FindItem(msg_FormatFixed)->SetEnabled(true);
		mbar->FindItem(msg_FormatFixed)->SetMarked(cf.OldFormatID() == eFixed);
		mbar->FindItem(msg_FormatScientific)->SetEnabled(true);
		mbar->FindItem(msg_FormatScientific)->SetMarked(cf.OldFormatID() == eScientific);
//		mbar->FindItem(msg_FormatFormula)->SetEnabled(true);
//		mbar->FindItem(msg_FormatFormula)->SetMarked(cf.OldFormatID() == eFormula);
	
		mbar->FindItem(msg_FormatDateTime)->SetEnabled(true);
		mbar->FindItem(msg_FormatDateTime)->SetMarked(false);
	
		mbar->FindItem(msg_FormatDigitsCmd)->SetEnabled(true);
		mbar->FindItem(msg_FormatCommas)->SetEnabled(true);
		mbar->FindItem(msg_FormatCommas)->SetMarked(cf.Commas());
	
		mbar->FindItem(msg_FindAgainCmd)->SetEnabled(fWhatToFind != NULL);
		bool canReplace = CanReplace(fWhatToFind, fIgnoreCase);
		mbar->FindItem(msg_Replace)->SetEnabled(canReplace);
		mbar->FindItem(msg_ReplaceAndFind)->SetEnabled(canReplace);
		mbar->FindItem(msg_ReplaceAll)->SetEnabled(canReplace);
		
		BMenuItem *item;
		BMenu *menu;

		menu = mbar->SubmenuAt(2)->SubmenuAt(0);
		for (int i = 0; i < menu->CountItems(); i++)
		{
			item = menu->ItemAt(i);
			item->SetMarked(!strcmp(item->Label(), fontFamily));
		}
		
		menu = mbar->SubmenuAt(2)->SubmenuAt(2);
		for (int i = 0; i < menu->CountItems(); i++)
		{
			item = menu->ItemAt(i);
			item->SetMarked(atoi(item->Label()) == (int)fontSize);
		}
		
		menu = mbar->SubmenuAt(2)->SubmenuAt(1);
		for (int i = menu->CountItems() - 1; i >= 0; i--)
		{
			delete menu->RemoveItem(i);
		}
		
		for (int i = 0; i < count_font_styles(fontFamily); i++)
		{
			font_style newStyle;
			get_font_style(fontFamily, i, &newStyle);
			menu->AddItem(new BMenuItem(newStyle, new BMessage(msg_StyleSelected)));
			menu->ItemAt(i)->SetTarget(this);
			if (strcasecmp(newStyle, fontStyle) == 0)
				menu->ItemAt(i)->SetMarked(true);
		}
		
		menu = mbar->SubmenuAt(2)->SubmenuAt(3);
		for (int i = 0; i < menu->CountItems(); i++)
		{
			item = menu->ItemAt(i);
			item->SetMarked(((CColorMenuItem *)item)->IsColorEqual(fontColor));
		}
		
		menu = mbar->SubmenuAt(2)->SubmenuAt(4);
		for (int i = 0; i < menu->CountItems(); i++)
		{
			item = menu->ItemAt(i);
			item->SetMarked(((CColorMenuItem *)item)->IsColorEqual(cs.fLowColor));
		}
		
		mbar->FindItem(msg_FillDown)->	
			SetEnabled(fSelection.top != fSelection.bottom);
		mbar->FindItem(msg_FillRight)->
			SetEnabled(fSelection.left != fSelection.right);
		mbar->FindItem(msg_FillCmd)->
			SetEnabled(fSelection.left != fSelection.right ||
						fSelection.top != fSelection.bottom);
		mbar->FindItem(msg_SortCmd)->
			SetEnabled(fSelection.left != fSelection.right ||
						fSelection.top != fSelection.bottom);
		mbar->FindItem(msg_DisplayFormulas)->SetMarked(fShowFormulas);
		
//		menu->FindItem(msg_Grid)->SetMarked(fShowGrid);
//		menu->FindItem(msg_Head)->SetMarked(fShowBorders);
		
		if (fSelection.left == 1 && fSelection.top == 1 &&
			(fSelection.right == kColCount || fSelection.bottom == kRowCount))
		{
			mbar->FindItem(msg_Freeze)->SetEnabled(
				fSelection.right != kColCount || fSelection.bottom != kRowCount);
			mbar->FindItem(msg_UnFreeze)->SetEnabled(fFrozen.h || fFrozen.v);
		}
		else
		{
			mbar->FindItem(msg_Freeze)->SetEnabled(false);
			mbar->FindItem(msg_UnFreeze)->SetEnabled(false);
		}
		
		mbar->FindItem(msg_GraphOptions)->SetEnabled(plug && plug->HasFocus());
		mbar->FindItem(msg_CreateGraph)->SetEnabled(hasData &&
			(fSelection.top != fSelection.bottom || fSelection.left != fSelection.right));
	}
} /* CCellView::MenusBeginning */

