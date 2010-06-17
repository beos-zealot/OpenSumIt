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
	CellWindow.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef CELLWINDOW_H
#define CELLWINDOW_H

#include "Stack.h"
#include "Range.h"

#include <StorageDefs.h>
#include <Window.h>
#include <FilePanel.h>
#include <Roster.h>
#include <Menu.h>

class CEditBox;
class CCommand;
class CCellView;
class TBFileStream;
class CTextControl;
class CDialog;
class MThread;

const ulong
	msg_Quit = 'Quit',
	msg_Read = 'RDat';

//#pragma mark class CCellWindow
class CCellWindow : public BWindow {

public:
	CCellWindow();
	CCellWindow(const entry_ref& inRef);
	~CCellWindow();

	virtual void		MessageReceived(BMessage *inMessage);
	virtual	void		SaveRequested(const entry_ref& directory, const char *name);
	virtual bool		QuitRequested();
	virtual	void		MenusBeginning();
	virtual void		MenusEnded();
	bool				ShowingMenus() { return fShowingMenus; };
	
	void				SetDirty(bool inDirty);
	
	bool				RegisterCommand(CCommand *newCommand);
	
	void				Open();
	void				Save();
	void				SaveAs();
	void				SaveAsSylk(const entry_ref& directory, const char *name);
	void				Revert();
	
	void				ReadData(BFile& file);
	void				WriteData(BFile& file);
	
//#pragma mark (Cell Accessors)
	CCellView*			GetCellView() { return fCells; };
	
	// Don't force clients to know interface to CCellView, just to get
	//  access to a BHandler
	BHandler *			GetCellHandler() { return (BHandler*)fCells; };

	// Don't force clients to know interface to CCellView, just to get
	//  access to a formula
	void *				GetCellFormula( const cell& ) ;
	
	CTextControl*		GetEditBox() { return fEditBox; };
	
	void				AddDialog(CDialog *inDialog, bool inWindowModal = false);
	void				RemoveDialog(CDialog *inDialog);
	void				MakeModal(CDialog *inDialog);
	
	static long			WindowCount();
	static CCellWindow* 	TopWindow();
	static CCellWindow*	FindWindow(const entry_ref& inRef);
	static void			Stack();
	static void			Tile();
//	static void			TileVertical();
	static void			GetMaxWindowRect(BRect *outRect, range *inRect);
	static void			GetUntitledName(char *newName);

	CDialog* GetSortDialog();
	CDialog* GetFillDialog();
	CDialog* GetNamesDialog();
	CDialog* GetGoToDialog();
	CDialog* GetPasteFunctionDialog();
	CDialog* GetDigitsDialog(int current);

	const entry_ref* EntryRef() { return fEntry; };
		
protected:

	void				InitWindow();
	void BuildButton(BRect inFrame, const char *name, ulong message, int normalID, int pushedID);
	
	virtual void		WindowActivated(bool active);
	
	BList			fDialogs;
	BList			fGraphs;
	CDialog			*fWindowModal;
	entry_ref			*fEntry;
	bool				fDirty;
	bool				fWaitForSave;
	BMenu*			fFileMenu;
	BMenu*			fEditMenu;
	BMenu*			fFormatMenu;
	BMenu*			fFontMenu;
	BMenu*			fStyleMenu;
	BMenu*			fSizeMenu;
	BMenu*			fTextColorMenu;
	BMenu*			fCellColorMenu;
	BMenu*			fDataMenu;
	BMenu*			fGraphMenu;
	BMenu*			fWindowMenu;
	BMenu*			fHelpMenu;
	CTextControl*		fEditBox;
	CCellView*		fCells;
	CStack			fCommands;
	CStack			fUndoneCommands;
	bool				fShowingMenus;
	BMenuItem*		fUndoMenuItem;
	BMenuItem*		fRedoMenuItem;
	
	BFilePanel		*fSavePanel;

	static BRect		GetFrameRect();
	
	static BList		sWindowList;
	static long		sUntitledCount;
};

inline long
CCellWindow::WindowCount()
	{ return (sWindowList.CountItems()); }

inline CCellWindow*
CCellWindow::TopWindow()
	{ return (CCellWindow *)sWindowList.FirstItem(); }

#endif
