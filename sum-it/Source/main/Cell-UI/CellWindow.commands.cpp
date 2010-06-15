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
	CellWindow.commands.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
#endif

#ifndef   CELLPARSER_H
#include "CellParser.h"
#endif

#ifndef   DOCOPTIONS_H
#include "DocOptions.h"
#endif

#ifndef   ANIMATEGRAPHDIALOG_H
#include "AnimateGraphDialog.h"
#endif

#ifndef   CHOOSEGRAPHDIALOG_H
#include "ChooseGraphDialog.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   RDIALOG_H
#include "RDialog.h"
#endif

#ifndef   PREFSDIALOG_H
#include "PrefsDialog.h"
#endif

#ifndef   GRAPHICCOMMANDS_H
#include "GraphicCommands.h"
#endif

#ifndef   GRAPHOPTIONSDIALOG_H
#include "GraphOptionsDialog.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   UNDOSTRS_H
#include "undostrs.h"
#endif

#include <Application.h>
#include <Entry.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Autolock.h>

void CCellWindow::MessageReceived(BMessage	*inMessage)
{
	try
	{
		switch (inMessage->what)
		{
			case msg_NewWindow:
				be_app->PostMessage(msg_NewWindow);
				break;
				
			case msg_CloseWindow:
			case B_QUIT_REQUESTED:
				if (QuitRequested())
					Quit();
				break;
				
			case msg_Save:
				Save();
				break;
				
			case msg_SaveAs:
				SaveAs();
				break;
			
#if !DEMO 
			case B_SAVE_REQUESTED:
			{
				entry_ref dir;
				const char *name;
				
				FailOSErr(inMessage->FindRef("directory", &dir), errMessageMissing);
				FailOSErr(inMessage->FindString("name", &name), errMessageMissing);
				SaveRequested(dir, name);
				break;
			}
#endif
				
			case msg_Revert:
				Revert();
				break;
				
			case msg_Undo:
				if (fCommands.Top())
				{
					StWriteLock lock(fCells->GetContainer());
					try
					{
						CCommand *cmd = (CCommand *)fCommands.Top();
						cmd->UndoCommand();
						fUndoneCommands.Push(fCommands.Pop());
					}
					catch (CErr& e)
					{
						CATCHED;
						e.DoError();
					}
				}
				break;
			
			case msg_Redo:
				if (fUndoneCommands.Top())
				{
					StWriteLock lock(fCells->GetContainer());
					try
					{
						CCommand *cmd = (CCommand *)fUndoneCommands.Top();
						cmd->RedoCommand();
						fCommands.Push(fUndoneCommands.Pop());
					}
					catch (CErr& e)
					{
						CATCHED;
						e.DoError();
					}
				}
				break;
			
	//		case msg_WindowOptionsCmd:
	//			CDocOptions::Construct(this);
	//			break;
	//			
			case msg_TextEntered:
				if (fCells->IsEntering() &&
					!fCells->EnterCell(fEditBox->Text(), inMessage))
					fEditBox->MakeFocus();
				break;
			
			case msg_TextCancelled:
				fCells->RejectCell();
				break;
			
			case msg_SumButton:
				fCells->SumButton();
				break;
			
			case msg_SelectWindow:
			{
				CCellWindow *w;
				w = (CCellWindow *)inMessage->FindInt32("window");
				w->Activate(true);
				break;
			}
			
			case msg_Zoom:
				Zoom();
				break;
			
			case msg_Stack:
				Stack();
				break;
			
			case msg_Tile:
				Tile();
				break;
			
			case msg_AnimateGraphs:
				new CAnimateGraphDialog(this);
				break;
			
			case msg_Print:
				fCells->DoPrint();
				break;
			
			case msg_PageSetup:
				fCells->DoPageSetup();
				break;
			
			case msg_PrefsCmd:
				CPrefsDialog::Construct(this);
				break;
			
			case msg_Command:
			{
				CChartOptionsCommand *cmd;
				if (inMessage->FindPointer("command", (void **)&cmd) == B_NO_ERROR)
					RegisterCommand(cmd);
				SetDirty(true);
				break;
			}
			
			case msg_Read:
				Open();
				break;
			
	//		case 'test':
	//		{
	////			int c = 0;
	//			cell c;
	//
	//			StWriteLock lock(fCells->GetContainer());
	//						
	//			for (int i = 0; i < 500; i++)
	//				for (int j = 0; j < 10000; j++)
	//				{
	//					Value v;
	////					printf("\r%d ", ++c);
	//					v = (double)(j * 500 + i);
	//					c.h = i + 1;
	//					c.v = j + 1;
	//					fCells->GetContainer()->SetValue(c, v);
	//				}
	//			break;
	//		}
			
			default:
				BWindow::MessageReceived(inMessage);
				break;
		}
	}
	catch (CErr& e)
	{
		e.DoError();
	}
} /* CCellWindow::MessageReceived */

void
CCellWindow::MenusBeginning()
{
	static int gWindowMenuCount = -1;

	if (gWindowMenuCount == -1)
		gWindowMenuCount = fWindowMenu->CountItems();

	fShowingMenus = true;

	BMenuItem *menuItem = NULL;
	
	fFileMenu->FindItem(msg_Save)->SetEnabled(fDirty);
	
	menuItem = fFileMenu->FindItem(msg_Revert);
	menuItem->SetEnabled(fDirty && fEntry);
	
	if (fCommands.Top())
	{
		fUndoMenuItem->SetEnabled(TRUE);
		
		char s[256];
		CCommand *cmd = (CCommand *)fCommands.Top();
		cmd->GetMenuLabel(true, s);
		fUndoMenuItem->SetLabel(s);
	}
	else
	{
		fUndoMenuItem->SetEnabled(FALSE);		
		fUndoMenuItem->SetLabel(GetIndString(3, kUndo));
	}

	if (fUndoneCommands.Top())
	{
		fRedoMenuItem->SetEnabled(TRUE);
		
		char s[256];
		CCommand *cmd = (CCommand *)fUndoneCommands.Top();
		cmd->GetMenuLabel(false, s);
		fRedoMenuItem->SetLabel(s);
	}
	else
	{
		fRedoMenuItem->SetEnabled(FALSE);		
		fRedoMenuItem->SetLabel(GetIndString(3, kRedo));
	}
	
	fCells->MenusBeginning();
	
	for (int i = fWindowMenu->CountItems() - 1; i >= gWindowMenuCount; i--)
	{
		BMenuItem *item = fWindowMenu->RemoveItem(i);
		delete item;
	}
	
	for (int i = 0; i < sWindowList.CountItems(); i++)
	{	
		BMessage *msg = new BMessage(msg_SelectWindow);
		CCellWindow *w = (CCellWindow *)sWindowList.ItemAt(i);
		msg->AddInt32("window", (long)w);
		fWindowMenu->AddItem(new BMenuItem(w->Title(), msg));
	}
	
//	fWindowMenu->FindItem(msg_Stack)->SetEnabled(false);
//	fWindowMenu->FindItem(msg_Tile)->SetEnabled(false);
//	fWindowMenu->FindItem(msg_TileVertical)->SetEnabled(false);
	
} /* CCellWindow::MenusBeginning */

void CCellWindow::MenusEnded()
{
	fShowingMenus = false;
} // CCellWindow::MenusEnded

bool CCellWindow::RegisterCommand(CCommand *theCommand)
{
	bool result = true;
	
	if (theCommand)
	{
		fCells->CancelCalculation();
		StWriteLock wlock(fCells->GetContainer());

		BAutolock lock(this);
		if (!lock.IsLocked())
			return false;
		
		try
		{
			theCommand->DoCommand();

			fCells->Repaginate();

			if (theCommand->IsUndoable())
			{
				fCommands.Push(theCommand);

				CCommand *cmd;
				while ((cmd = (CCommand *)fUndoneCommands.Pop()) != NULL)
					delete cmd;
			}
			else
				delete theCommand;
		}

		catch(CParseErr e)
		{
			CATCHED;
			e.DoError();
			
			long a, b;
			a = e.GetOffset();
			b = e.GetLen();
			
			fEditBox->Select(e.GetOffset(), e.GetOffset() + e.GetLen());
			delete theCommand;
			result = false;
		}
		
		catch (CErr& e)
		{
			CATCHED;
			e.DoError();

			delete theCommand;
			result = false;
		}
	}
	
	return result;
} /* CCellWindow::RegisterCommand */
