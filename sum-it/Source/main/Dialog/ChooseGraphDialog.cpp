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
	ChooseGraphDialog.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   CELLWINDOW_H
#include "CellWindow.h"
#endif

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   COMMAND_H
#include "Command.h"
#endif

#ifndef   CELLCOMMANDS_H
#include "CellCommands.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   GRAPHPLUGIN_H
#include "GraphPlugIn.h"
#endif

#ifndef   CHOOSEGRAPHDIALOG_H
#include "ChooseGraphDialog.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   USRMSG_H
#include "usrmsg.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif

#ifndef   PREFERENCES_H
#include "Preferences.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#include <support/Debug.h>
#include <unistd.h>

#include <Message.h>
#include <Autolock.h>

#include <Screen.h>
#include <Bitmap.h>

#include <ListItem.h>
#include <ListView.h>
#include <ScrollView.h>

#include <Entry.h>
#include <Directory.h>
#include <File.h>
#include <Path.h>

#include <cstring>


static bool gSelectedMapInited = false;
static uchar gSelectedMap[256];

static void InitSelectedMap()
{
	BScreen screen;
	rgb_color c;
	int i;
	
	for (i = 0; i < 256; i++)
	{
		c = screen.ColorForIndex(i);
		c.red = c.red * 2 / 3;
		c.green = c.green * 2 / 3;
		c.blue = c.blue * 2 / 3;
		gSelectedMap[i] = screen.IndexForColor(c);
	}
	
	gSelectedMapInited = true;
} /* InitSelectedMap */

class CGraphItem : public BListItem {
public:
	CGraphItem(const char *name, const uchar *icon);
	~CGraphItem();
	virtual void DrawItem(BView *owner,
					BRect bounds,
					bool complete = false);
	char fName[32];
	BBitmap *fIcon, *fSelectedIcon;
};

CGraphItem::CGraphItem(const char *name, const uchar *icon)
{
	ASSERT(strlen(name) <= 31);
	strcpy(fName, name);
	SetHeight(34);
	
	BRect r(0, 0, 31, 31);
	fIcon = new BBitmap(r, B_COLOR_8_BIT);
	FailNil(fIcon);
	fIcon->SetBits(icon, 1024, 0, B_COLOR_8_BIT);
	
	uchar selectedIcon[1024];
	
	for (int i = 0; i < 1024; i++)
		selectedIcon[i] = gSelectedMap[icon[i]];

	fSelectedIcon = new BBitmap(r, B_COLOR_8_BIT);
	fSelectedIcon->SetBits(selectedIcon, 1024, 0, B_COLOR_8_BIT);
	
	return ;
}

CGraphItem::~CGraphItem()
{
	delete fIcon;
	delete fSelectedIcon;
}

void CGraphItem::DrawItem(BView *owner, BRect bounds, bool complete)
{
	if (IsSelected())
		owner->SetLowColor(kB_GrayTable[8]);
	else
		owner->SetLowColor(kWhite);
		
	owner->FillRect(bounds, B_SOLID_LOW);
	
	BPoint p = bounds.LeftBottom();
	p.x += 42;
	p.y -= 2 + 2;
	owner->DrawString(fName, p);
	p.y += 2;
	p.x -= 2;
	owner->MovePenTo(p);
	p.x = bounds.right;
	owner->StrokeLine(p);
	
	if (IsSelected())
		owner->SetLowColor(kWhite);
	
	p = bounds.LeftTop();
	p.x++; p.y++;
	owner->DrawBitmap(IsSelected() ? fSelectedIcon : fIcon, p);
}

CChooseGraphDialog::CChooseGraphDialog(CCellWindow *w, range inRange)
	: CDialog(
		BRect(100.0, 100.0, 420.0, 280.0), 
		"Choose a graph",
		w,
		BRect(252.0, 147.0, 310.0, 170.0),
		BRect(184.0, 147.0, 242.0, 170.0)
		)
{
	BAutolock lock(this);
	
	fRange = inRange;
	
	if (!gSelectedMapInited)
		InitSelectedMap();
	
	BRect r(10, 10, 310-B_V_SCROLL_BAR_WIDTH, 130);
	fList = new BListView(r, "lijst");
	fList->SetInvocationMessage(new BMessage(msg_OK));
	
	BScrollView *scroller = new BScrollView(NULL, fList,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true);
	
	fMainView->AddChild(scroller);
	fList->SetFont(be_plain_font);

	CollectGraphs();
	
	fList->MakeFocus();
	Show();
} // CChooseGraphDialog::CChooseGraphDialog

CChooseGraphDialog::~CChooseGraphDialog()
{
	int i;
	for (i=0;i<fList->CountItems();i++)
		delete ((CGraphItem *)fList->ItemAt(i));
} // CChooseGraphDialog::CChooseGraphDialog

bool CChooseGraphDialog::OKClicked()
{
	CGraphItem *i = (CGraphItem *)fList->ItemAt(fList->CurrentSelection());
	if (i)
	{
		BMessage msg(msg_DisplayGraph);
		msg.AddString("graphname", i->fName);
		fOwner->PostMessage(&msg, fOwner->GetCellView());
	}
	return true;
} // CChooseGraphDialog::OKClicked

void CChooseGraphDialog::CollectGraphs()
{
	BEntry eDir;
	FailOSErr(gAppDir->FindEntry("Graphs",  &eDir), errNoGraphDir);
	BDirectory dir(&eDir);
	
	if (dir.InitCheck() != B_NO_ERROR || !dir.IsDirectory())
		THROW((errNoGraphDir));
	
	BEntry e;
	BFile plug;
	BPath path;
	const char *plugName;
	const uchar *plugIcon;
	image_id addongraph;
	
//bug alert!!! work around for BeOS AA:PR bug	
	gAppDir->GetEntry(&e);
	e.GetPath(&path);
	chdir(path.Path());
	
	while (dir.GetNextEntry(&e) == B_NO_ERROR)
		if (e.IsFile())
		{
			e.GetPath(&path);

			addongraph = load_add_on(path.Path());
			if (addongraph > B_ERROR)
			{
				if (get_image_symbol(addongraph, "Name",
					B_SYMBOL_TYPE_DATA, (void **)&plugName) != B_NO_ERROR ||
					get_image_symbol(addongraph, "Icon",
					B_SYMBOL_TYPE_DATA, (void **)&plugIcon) != B_NO_ERROR)
				{
					unload_add_on(addongraph);
					addongraph = B_ERROR;
					continue;
				}
				
				fList->AddItem(new CGraphItem(plugName, plugIcon));
				fList->LastItem()->SetHeight(34);
				
				unload_add_on(addongraph);
			}
		}
	
	if (fList->CountItems() == 0)
	{
		MAlert *a = new MWarningAlert(GetMessage(msgNoGraphsFound));
		a->Go();
		PostMessage(B_QUIT_REQUESTED);
	}
}
