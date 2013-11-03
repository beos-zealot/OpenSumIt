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
	Sum-It.c
	
	Copyright 1996, 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.


*/

#ifndef   RDIALOG_H
#include "RDialog.h"
#endif

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CONTAINER_H
#include "Container.h"
#endif

//#ifndef   MYRESOURCES_H
//#include "MyResources.h"
//#endif

#ifndef   RESOURCEMANAGER_H
#include "ResourceManager.h"
#endif

#ifndef   FUNCTIONUTILS_H
#include "FunctionUtils.h"
#endif

#ifndef   FORMATTER_H
#include "Formatter.h"
#endif

#ifndef   MYCLIPBOARD_H
#include "MyClipboard.h"
#endif

#ifndef   FINDDIALOG_H
#include "FindDialog.h"
#endif

#ifndef   FONTMETRICS_H
#include "FontMetrics.h"
#endif

#ifndef   CELLSTYLE_H
#include "CellStyle.h"
#endif

#ifndef   PREFSDIALOG_H
#include "PrefsDialog.h"
#endif

#ifndef   MESSAGES_H
#include "messages.h"
#endif

#ifndef   SUM_IT_H
#include "Sum-It.h"
#endif

#ifndef   ABOUT_H
#include "About.h"
#endif

#ifndef   FORMULA_H
#include "Formula.h"
#endif

#ifndef   FONTSTYLE_H
#include "FontStyle.h"
#endif

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif

#ifndef   MYMATH_H
#include "MyMath.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif


#ifndef   CONFIG_H
#include "Config.h"
#endif

#include <support/Debug.h>

#include <Roster.h>
#include <Path.h>
#include <Directory.h>
#include <FilePanel.h>
#include <MimeType.h>
#include <String.h>

#define PRERELEASE 0

#if PRERELEASE
#include <parsedate.h>
#ifndef   TIMELIMIT_H
#include "TimeLimit.h"
#endif

#endif

double 	gRefNan, gCircleNan, gSqrtNan, gPowerNan, gValueNan, gDivNan,
		gAddNan, gFinanceNan, gEvalNan, gInvTrigNan, gLogNan, gMulNan,
		gNANan, gErrorNan, gDateNan, gTimeNan, gFuncNan, gNameNan;

char	gDecimalPoint, gThousandSeparator, gListSeparator,
		gDateSeparator, gTimeSeparator;
bool	g24Hours;
bool gDatatypesInstalled;

extern int gGridColor;

CSumItApplication *gApp;
BPath gAppName;
BDirectory *gAppDir, *gCWD;

int main(int argc, char *argv[])
{
#if TRACKMEM
	mcheck(CheckFailed);
#endif

	CSumItApplication app;
	
	app.Run();
	
	gPrefs->WritePrefFile();
	
	delete gPrefs;
	CFontStyle::Cleanup();
	
#if TRACKMEM
	FREE(gFuncs);
	FREE(gFuncArrayByName);
	FREE(gFuncDescriptions);
	FREE(gFuncPasteStrings);
	
	PrintUndisposed();
#endif
	
	return 0;
}

CSumItApplication::CSumItApplication()
	  : BApplication(kOpenSumItSignature)
{
#if PRERELEASE
//	{
//		time_t now, expire;
//		time(&now);
//		expire = parsedate("nov 1, 1997", now);
//		printf("expire at %08x\n", expire);
//		expire = 0x345a62f0;

//		if (difftime(now, expire) > 0) {
//			MAlert *a = new MWarningAlert("This version of sum-it has expired");
//			a->Go();
//			exit(1);
//		}
//		TimeLimit limit;
//	}
#endif

	try
	{
		gApp = this;
		
		app_info ai;
		GetAppInfo(&ai);
		
		BEntry entry(&ai.ref);
		entry.GetPath(&gAppName);
		
		BPath dir;
		gAppName.GetParent(&dir);
		gAppDir = new BDirectory(dir.Path());
		gCWD = new BDirectory(*gAppDir);
			
		gResourceManager.SetTo( &gAppName ) ;
		InitFunctions();
		srand((int)(system_time() % INT_MAX));
			
		try
		{
			gPrefs = new CPreferences("Sum-It.Prefs");
			gPrefs->ReadPrefFile();
		}
		catch (CErr& e)
		{
			CATCHED;
			if (e != errIOOpen)
			{
				e.DoError();
				exit(1);
			}
		}
	
		gErrorNan = Nan(1);
		gRefNan = Nan(3);
		gCircleNan = Nan(4);
		gSqrtNan = Nan(5);
		gPowerNan = Nan(6);
		gValueNan = Nan(7);
		gDivNan = Nan(8);
		gAddNan = Nan(9);
		gFinanceNan = Nan(10);
		gEvalNan = Nan(11);
		gInvTrigNan = Nan(12);
		gLogNan = Nan(13);
		gMulNan = Nan(14);
		gNANan = Nan(15);
		gDateNan = Nan(16);
		gTimeNan = Nan(17);
//		gCellNan = Nan(18);
		gFuncNan = Nan(19);
		gNameNan = Nan(20);
		
		font_family fam;
		font_style sty;
		float siz;
		
		be_plain_font->GetFamilyAndStyle(&fam, &sty);
		siz = be_plain_font->Size();
		
		gFontSizeTable.GetFontID(
			gPrefs->GetPrefString("defdoc font family", fam),
			gPrefs->GetPrefString("defdoc font style", sty),
			gPrefs->GetPrefDouble("defdoc font size", siz),
			kBlack);
		
		InitFormatter();
		
		fOpenPanel = NULL;
		
		gClipboard = new CClipboard;
		gClipboard->Run();

		fFindDialog = CFindDialog::Construct();
		fFindDialog->Run();
		
		gWithEqualSign = gPrefs->GetPrefInt("formula starts with equal", 0);
		gGridColor = gPrefs->GetPrefInt("dark gridlines") ? 10 : 6;
		
		InstallMimeType();
		
		SetPulseRate(1000000);
	}
	
	catch (CErr& e)
	{
		CATCHED;
		e.DoError();
		exit(1);
	}
	
	catch (...)
	{
		CATCHED;
		ASSERT(false);
	}
}

void
CSumItApplication::AboutRequested()
{
	new CAboutBox;
}	

void
CSumItApplication::InstallMimeType()
{
	// install mime type of documents
	BMimeType mime(kOpenSumItMimeString);
	status_t ret = mime.InitCheck();
	if (ret != B_OK) {
		fprintf(stderr, "Could not init native document mime type (%s): %s.\n",
			kOpenSumItMimeString, strerror(ret));
		return;
	}

	if (mime.IsInstalled() && !(modifiers() & B_SHIFT_KEY)) {
		// mime is already installed, and the user is not
		// pressing the shift key to force a re-install
		printf("Mime is already installed!\n");
		return;
	}

	ret = mime.Install();
	if (ret != B_OK && ret != B_FILE_EXISTS) {
		fprintf(stderr, "Could not install native document mime type (%s): %s.\n",
			kOpenSumItMimeString, strerror(ret));
		return;
	}
	// set preferred app
	ret = mime.SetPreferredApp(kOpenSumItSignature);
	if (ret != B_OK) {
		fprintf(stderr, "Could not set native document preferred app: %s\n",
			strerror(ret));
	}

	// set descriptions
	ret = mime.SetShortDescription("OpenSum-It Spreadsheet");
	if (ret != B_OK) {
		fprintf(stderr, "Could not set short description of mime type: %s\n",
			strerror(ret));
	}
	ret = mime.SetLongDescription("OpenSum-It Spreadsheet File");
	if (ret != B_OK) {
		fprintf(stderr, "Could not set long description of mime type: %s\n",
			strerror(ret));
	}

	// set sniffer rule
	
	// short explanation what 0x565200020300 means:
	// 0x5652 (chunk type) 'VR' - version
	// 0x0002 (chunk size)  
	// 0x0300 (chunk value)  - version number
	// More info in sum-it/Source/main/FileSys/FileFormat.h
	const char* snifferRule = "1.0 (0x565200020300)";
	ret = mime.SetSnifferRule(snifferRule);
	if (ret != B_OK) {
		BString parseError;
		BMimeType::CheckSnifferRule(snifferRule, &parseError);
		fprintf(stderr, "Could not set sniffer rule of mime type: %s\n",
			parseError.String());
	}

	// set document icon
	BResources* resources = AppResources();
		// does not need to be freed (belongs to BApplication base)
	if (resources != NULL) {
		size_t size;
		const void* iconData = resources->LoadResource('VICN', "OpenSum-It_DocumentIcon",
			&size);
		if (iconData != NULL && size > 0) {
			if (mime.SetIcon(reinterpret_cast<const uint8*>(iconData), size)
				!= B_OK) {
				fprintf(stderr, "Could not set vector icon of mime type.\n");
			}
		} else {
			fprintf(stderr, "Could not find icon in app resources "
				"(data: %p, size: %ld).\n", iconData, size);
		}
	} else
		fprintf(stderr, "Could not find app resources.\n");
}

void
CSumItApplication::ReadyToRun()
{
	if (CCellWindow::WindowCount() < 1)
	{
		if (gPrefs->GetPrefInt("start with new", 1))
			PostMessage(msg_NewWindow);
		else
			PostMessage(msg_OpenWindow);
	}
}

CSumItApplication::~CSumItApplication()
{
	if (fOpenPanel)
		delete fOpenPanel;
} // CSumItApplication::~CSumItApplication

void
CSumItApplication::MessageReceived(
	BMessage	*inMessage)
{
	switch (inMessage->what)
	{
		case msg_WindowAdded:
			break;
			
		case msg_WindowRemoved:
			if (CCellWindow::WindowCount() < 1)
				PostMessage(B_QUIT_REQUESTED);
			break;
			
		case msg_NewWindow:
			MakeNewWindow();
			break;
		
		case msg_Help:
			ShowHelp();
			break;
		
		case msg_OpenWindow:
		{
			if (fOpenPanel == NULL)
				fOpenPanel = new BFilePanel(B_OPEN_PANEL);
			if (fOpenPanel->IsShowing())
				fOpenPanel->Window()->Activate();
			else
			{
				BEntry entry;
				gCWD->GetEntry(&entry);
	
				entry_ref ref;
				entry.GetRef(&ref);
				fOpenPanel->SetPanelDirectory(&ref);
				fOpenPanel->Show();
			}
			break;
		}
			
		case msg_ShowClipboard:
			if (gClipboard->IsHidden()) gClipboard->Show();
			gClipboard->Activate();
			break;
			
		default:
			BApplication::MessageReceived(inMessage);
			break;
	}
}

void CSumItApplication::RefsReceived(
	BMessage	*inMessage)
{
	ulong 	type;
	long	count;
	inMessage->GetInfo("refs", &type, &count);
	for (long i = --count; i >= 0; i--)
	{
		entry_ref item;
		if (inMessage->FindRef("refs", i, &item) == B_NO_ERROR)
		{
			BEntry entry(&item);
			if (entry.InitCheck() == B_NO_ERROR && entry.IsFile())
				OpenWindow(item);
		}
	}
}

void CSumItApplication::MakeNewWindow()
{
	try
	{
		new CCellWindow;
	}
	catch(CErr& e)
	{
		CATCHED;
		e.DoError();
	}
}

CCellWindow* CSumItApplication::OpenWindow(const entry_ref& inRef) throw()
{
	try
	{
		CCellWindow *window = NULL;
		
		BEntry entry(&inRef);
		BPath doc, dir;
		entry.GetPath(&doc);
		doc.GetParent(&dir);
		gCWD->SetTo(dir.Path());
		
		window = CCellWindow::FindWindow(inRef);
		if (window != NULL)
		{
			window->Activate();
			return window;
		}
		else
			return new CCellWindow(inRef);
	}
	catch(CErr& e)
	{
		CATCHED;
		e.DoError();
		return NULL;
	}
}

bool CSumItApplication::QuitRequested()
{
	CCellWindow *w = CCellWindow::TopWindow();
	
	while (w)
	{
		w->Lock();
		if (w->QuitRequested())
			w->Quit();
		else
		{
			w->Unlock();
			return false;
		}
		
		w = CCellWindow::TopWindow();
	}
	
	if (fFindDialog->Lock()) {
		fFindDialog->Quit();
	}
	if (gClipboard->Lock()) {
		gClipboard->Quit();
	}

	return true;
} // QuitRequested

void CSumItApplication::ArgvReceived(int32 argc, char **argv)
{
	int i;

	for (i = 1; i < argc; i++)
	{
		entry_ref ref;
		
		if (get_ref_for_path(argv[i], &ref) == B_NO_ERROR)
			OpenWindow(ref);
	}
} /* CSumItApplication::ArgvReceived */

void CSumItApplication::Pulse()
{
//	if (CCellWindow::WindowCount() < 1 && !(fOpenPanel == NULL || fOpenPanel->IsShowing()))
//	{
//		if (gPrefs->GetPrefInt("start with new"))
//			PostMessage(B_QUIT_REQUESTED);
//		else
//			PostMessage(msg_NewWindow);
//	}
} /* CSumItApplication::Pulse */

void CSumItApplication::ShowHelp()
{
	try
	{
		BEntry entry;
		gAppDir->FindEntry("Docs", &entry);
		BDirectory docdir(&entry);
		docdir.FindEntry("index.html", &entry);
		
		if (entry.InitCheck() || !entry.Exists())
			THROW((errNoDocumentation));
		
		entry_ref ref;
		entry.GetRef(&ref);
		
		BMessage msg(B_REFS_RECEIVED);
		msg.AddRef("refs", &ref);

		entry_ref browser;
		if (be_roster->FindApp("text/html", &browser) || 
				be_roster->FindApp("application/x-vnd.Haiku-WebPositive", &browser))
			THROW((errNoBrowser));
		
		if (be_roster->IsRunning(&browser))
		{
			BMessenger msr(NULL, be_roster->TeamFor(&browser));
			msr.SendMessage(&msg);
		}
		else if (be_roster->Launch(&browser, &msg))
			THROW((errNoBrowser));
	}
	catch (CErr& e)
	{
		e.DoError();
	}
} /* CSumItApplication::ShowHelp */

//BHandler *CSumItApplication::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier,
//					int32 form, const char *property)
//{
////	puts("CSumItApplication::ResolveSpecifier");
////	puts(property);
////	puts("specifier:");
////	specifier->PrintToStream();
////	puts("msg:");
////	msg->PrintToStream();
//	BHandler *result = NULL;
//	
//	if (form == 'Ref ')
//	{
//		entry_ref ref;
//		
//		if (specifier->FindRef("Refs", &ref) == B_OK)
//		{
//			msg->PopSpecifier();
//
//			CCellWindow *w = OpenWindow(ref);
//			w->PostMessage(msg);
//		}
//	}
//	else if (strcmp(property, "Document") == 0 && form == B_NAME_SPECIFIER)
//	{
//		entry_ref ref;
//		char *n;
//		
//		if (specifier->FindString("name", &n) == B_OK &&
//			get_ref_for_path(n, &ref))
//		{
//			msg->PopSpecifier();
//
//			CCellWindow *w = OpenWindow(ref);
//			w->PostMessage(msg);
//		}
//	}
//	else
//		result = inherited::ResolveSpecifier(msg, index, specifier, form, property);
//	
//	return result;
//} /* CSumItApplication::ResolveSpecifier */
