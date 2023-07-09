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
	CellWindow.IO.c
	
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

#ifndef   CONTAINER_H
#include "Container.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   PROGRESSVIEW_H
#include "ProgressView.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   MYTEXTCONTROL_H
#include "MyTextControl.h"
#endif

#ifndef   CELLPARSER_H
#include "CellParser.h"
#endif

#ifndef   TEXT_CELLS_H
#include "Text2Cells.h"
#endif

//#ifndef   DATATYPES_H
//#include "Datatypes.h"
//#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   NODEINFO_H
#include "NodeInfo.h"
#endif

#ifndef   MTHREAD_H
#include "MThread.h"
#endif

#ifndef   STLOCKER_H
#include "StLocker.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   EXCEL_H
#include "Excel.h"
#endif

#include <sys/stat.h>
#include <ctype.h>

#include <support/Debug.h>
#include <BufferIO.h>
#include <MenuBar.h>
#include <Directory.h>
#include <FilePanel.h>
#include <MenuItem.h>
#include <Autolock.h>

const char kDemoMsg[] = "Saving is disabled in this demo version of Sum-It\n\n"
								"Please visit http://www.beatware.com for more info on Sum-It and other products of BeatWare.";

void CCellWindow::Open()
{
	try {
		BFile file(fEntry, B_READ_ONLY);
		FailOSErr(file.InitCheck(), errIOOpen);

		ReadData(file);

		fCells->AdjustScrollBars();
		fCells->DrawStatus();
		fCells->Invalidate();
		fCells->CalculateAllCells();
	}
	catch (CErr& e) {
		e.DoError();
		PostMessage(B_QUIT_REQUESTED);
	}
} /* CCellWindow::Open */

void CCellWindow::Save()
{
#if !DEMO
	if (fEntry)
	{
		BEntry e(fEntry), b;
		bool existed = e.Exists();
		time_t created;
		
		try
		{
			if (existed)
			{
				FailOSErr(BFile(fEntry, B_READ_ONLY).GetCreationTime(&created), errIOGeneral);
	
				char name[B_FILE_NAME_LENGTH];
				strcpy(name, fEntry->name);
				if (strlen(name) > B_FILE_NAME_LENGTH - 2)
					name[B_FILE_NAME_LENGTH - 2] = 0;
				strcat(name, "~");
				
				FailOSErr(e.Rename(name), errIOGeneral);
				b = e;
			}
			
			BFile file;
			BDirectory dir;
			
			FailOSErr(e.GetParent(&dir), errIOGeneral);
			FailOSErr(dir.CreateFile(fEntry->name, &file, true), errIOGeneral);
			WriteData(file);
	
			BNodeInfo(&file).SetType(kOpenSumItMimeString);
			if (existed)
			{
				FailOSErr(file.SetCreationTime(created), errIOGeneral);
				FailOSErr(e.Remove(), errIOGeneral);
			}
		}
		catch (CErr& err)
		{
			err.DoError();
// Now don't check error codes anymore... hope this is right
			e.SetTo(fEntry);
			e.Remove();
			if (existed)
				b.Rename(fEntry->name);
		}
	}
	else
		SaveAs();
#else
	MInfoAlert(kDemoMsg).Go();
#endif
} /* CCellWindow::Save */

void CCellWindow::SaveAs()
{
#if !DEMO
	if (fSavePanel)
	{
		if (!fSavePanel->IsShowing())
			fSavePanel->Show();
		else
			fSavePanel->Window()->Activate();
		return;
	}

	fSavePanel = new BFilePanel(B_SAVE_PANEL);
	BWindow *w = fSavePanel->Window();
	w->Lock();
	
	char s[256];
	sprintf(s, GetMessage(msgSaveTitle), Title());
	
	w->SetTitle(s);
	fSavePanel->SetSaveText(Title());
	
	BMenuBar *mbar = w->KeyMenuBar();
	if (mbar)
	{
		BMenu *fileFormats = new BMenu(GetMessage(msgFileFormat));
		fileFormats->AddItem(new BMenuItem(GetMessage(msgDefault), new BMessage(msg_FileFormat)));
		fileFormats->AddItem(new BMenuItem(GetMessage(msgSaveAsSYLK), new BMessage(msg_FileFormat)));
		fileFormats->AddItem(new BMenuItem(GetMessage(msgSaveAsCSV), new BMessage(msg_FileFormat)));
		fileFormats->AddItem(new BMenuItem(GetMessage(msgSaveAsTabDelimited), new BMessage(msg_FileFormat)));
		fileFormats->ItemAt(0)->SetMarked(true);
		fileFormats->SetRadioMode(true);
		mbar->AddItem(fileFormats);
	}

	if (fEntry)
	{
		BEntry e(fEntry), p;
		e.GetParent(&p);
		fSavePanel->SetPanelDirectory(&p);
	}
	else
		fSavePanel->SetPanelDirectory(gCWD);

	fSavePanel->SetTarget(this);
	w->Unlock();
	fSavePanel->Show();
#else
	MInfoAlert(kDemoMsg).Go();
#endif
} /* CCellWindow::SaveAs */

void CCellWindow::SaveRequested(const entry_ref& directory, const char *name)
{
	try
	{
		bool simpleSave = true;
		int format = 0;

		BDirectory dir(&directory);

		BEntry e(&dir, name);
		BFile file;
		
		FailOSErr(e.InitCheck(), errIOGeneral);
		if (e.Exists())
			e.Remove();

		if (fSavePanel)
		{
			BWindow *w = fSavePanel->Window();
			BAutolock lock(w);

			BMenuItem *item = w->KeyMenuBar()->FindItem(msg_FileFormat);
			format = item->Menu()->IndexOf(item->Menu()->FindMarked());
		}

		switch (format)
		{
			case 1:
				SaveAsSylk(directory, name);
				break;

			case 2:
			case 3:
				if (dir.CreateFile(name, &file, false) == B_OK)
				{
					CTextConverter cvt(file, fCells->GetContainer());
					cvt.SetFieldSeparator(format == 2 ? ',' : '\t');
					cvt.SetQuoteTextFields(format == 2);
					cvt.ConvertToText(NULL, fCells);
				}
				break;

			default:
				if (!fEntry) fEntry = new entry_ref;
				FailOSErr(e.GetRef(fEntry), errIOGeneral);
				Save();
				SetTitle(name);
				break;
		}
		
		if (fSavePanel)
		{
			delete fSavePanel;
			fSavePanel = NULL;
		}

		if (simpleSave)
		{
			CCommand *cmd;
			while ((cmd = (CCommand *)fUndoneCommands.Pop()) != NULL)
				delete cmd;
			while ((cmd = (CCommand *)fCommands.Pop()) != NULL)
				delete cmd;
		}
	}
	catch (CErr& e)
	{
		CATCHED;
		e.DoError();
		delete fEntry;
		fEntry = NULL;
	}
} /* CCellWindow::SaveRequested */

void CCellWindow::Revert()
{
	char title[256];
	sprintf(title, GetMessage(msgRevert), Title());
	
	MAlert *a = new MInfoAlert(title, GetMessage(msgCancel), GetMessage(msgOK));
	if (a->Go() == 2)
	{
		BFile file;
		FailOSErr(file.SetTo(fEntry, B_READ_ONLY), errIOGeneral);
		fCells->ReInit();
		ReadData(file);
		fDirty = false;
	}

	CCommand *cmd;
	while ((cmd = (CCommand *)fUndoneCommands.Pop()) != NULL)
		delete cmd;
	while ((cmd = (CCommand *)fCommands.Pop()) != NULL)
		delete cmd;
}

void CCellWindow::WriteData(BFile& file)
{
	BAutolock lock(fCells->GetContainer());
	
	try
	{
		Lock();

		if (fCells->GetContainer()->GetCellCount() < 10000)
		{
			BMallocIO buf;
			fCells->Write(buf);
			
			CHECKWRITE(file, buf.Buffer(), buf.BufferLength());
		}
		else
			fCells->Write(file);

		SetDirty(false);
		
		if (fWaitForSave)
			PostMessage(B_QUIT_REQUESTED);
		else
			Unlock();
	}
	catch (CErr& e)
	{
		CATCHED;
		
		if (e == errIOWrite)
		{
			strcat(e, "\n");
			strcat(e, errno ? strerror(errno) : "Disk full?");
		}
		
		Unlock();
		throw e;
	}
} /* CCellWindow::WriteData */

void CCellWindow::SaveAsSylk(const entry_ref& directory, const char *name)
{
	BFile file;
	FailOSErr(BDirectory(&directory).CreateFile(name, &file, false), errIOOpen);

	int fd;
	FILE *f = NULL;
	
	try
	{
		fd = file.Dup();
		if (fd < 0)
			THROW((errIOOpen, errno));

		f = fdopen(fd, "w");
		if (f == NULL)
			THROW((errIOOpen, errno));
		
		fCells->WriteSylk(f);
	}
	catch (CErr& e)
	{
		e.DoError();
	}

	if (f) fclose(f);
	if (fd) close(fd);
} // CCellWindow::SaveAsSylk

void CCellWindow::ReadData(BFile& file)
{
	file.Seek(0, SEEK_SET); 

	BBufferIO inStream(&file, 65536, false); 
	BMallocIO outStream;
	
	try
	{
		fCells->Read(inStream);

// since this was a sum-it file, check it's mimetype
		char mimetype[B_MIME_TYPE_LENGTH];
		BNodeInfo ni(&file);
		ni.GetType(mimetype);
		
		if (strcmp(mimetype, kOpenSumItMimeString))
		{
			if (ni.SetType(kOpenSumItMimeString))
			{
				BFile F(fEntry, B_READ_WRITE);
				BNodeInfo(&F).SetType(kOpenSumItMimeString);
			}
		}
		return;
	}
	
	catch (CErr& e)
	{
		CATCHED;
		// so its not a sum-it file
	}
	
	int32 l;
	inStream.Seek(0, SEEK_SET);
	inStream.Read(&l, sizeof(int32));
	if (ntohl(l) == 'ID;P')
	{
		fCells->ReadSylk(inStream);
		Show();
		return;
	}
	
	try
	{
		CExcel5Filter filter(inStream, fCells, fCells->GetContainer());
		filter.Translate();
		Show();
		return;
	}
	catch (CErr& e)
	{
		// not an excel file...
	}
	
	char title[32], oldTitle[64];
	strcpy(oldTitle, Title());
	CCellWindow::GetUntitledName(title);
	SetTitle(title);
	Show();
	UpdateIfNeeded();
	
	try
	{
		inStream.Seek(0, SEEK_SET);
		range r;
		char s[256];
		
		sprintf(s, GetMessage(msgOpenTextFile), oldTitle);
		
		BAlert *a = new BAlert("alert", s,
			GetMessage(msgCancel),
			GetMessage(msgComma),
			GetMessage(msgTab),
			B_WIDTH_AS_USUAL,
			B_INFO_ALERT);

		CTextConverter conv(inStream, fCells->GetContainer());

		switch (a->Go())
		{
			case 0:
				THROW((errUnknownFileFormat, fEntry->name));
				break;
			case 1:
				conv.SetFieldSeparator(',');
				break;
			case 2:
				conv.SetFieldSeparator('\t');
				break;
		}
		
		UpdateIfNeeded();
		
		conv.ConvertFromText(r, fCells, true);
		
		delete fEntry;
		fEntry = NULL;
		
		fCells->DrawStatus();
		fCells->AdjustScrollBars();
		return;
	}
	
	catch (CErr& e)
	{
		CATCHED;
		
		if (e == errUnknownFileFormat)
			throw;
		// obviously no text file
	}

	THROW((errUnknownFileFormat, fEntry->name));
} // CReadThread::Read

