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
	RDialog, a dialog build from a resource template

	Copyright 1997, Hekkelman Programmatuur
*/

#ifndef   DIALOG_H
#include "Dialog.h"
#endif

#ifndef   RDIALOG_H
#include "RDialog.h"
#endif

#ifndef   RESOURCEMANAGER_H
#include "ResourceManager.h"
#endif

#ifndef   TABSHEET_H
#include "TabSheet.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   DRAWUTILS_H
#include "DrawUtils.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#include <cstdio>

#include <DataIO.h>

#include <InterfaceKit.h>

static float gFactor = -1;

static void ReadRect(BPositionIO& stream, BRect& r)
{
	short left, top, right, bottom;

	stream.Read(&left, sizeof(short));
	stream.Read(&top, sizeof(short));
	stream.Read(&right, sizeof(short));
	stream.Read(&bottom, sizeof(short));

	r.left = gFactor * left;
	r.top = gFactor * top;
	r.right = gFactor * right;
	r.bottom = gFactor * bottom;
} /* ResizeRect */

CRDialog::CRDialog(BRect frame, const char *name, CCellWindow *owner,
		window_type type, int flags)
	: CDialog(frame, name, owner, type, flags)
{
} /* CRDialog::CRDialog */

CRDialog::~CRDialog()
{
} /* CRDialog::~CRDialog */

void CRDialog::ConstructFromTemplate(BPositionIO& buf)
{
	int32 token, l, x, size;
	BRect r;
	char name[256], label[256];
	BView *v = fMainView, *nv = NULL;

	size_t pos = buf.Position();
	buf.Seek(0, SEEK_END);
	size = buf.Position();
	buf.Seek(pos, SEEK_SET);

	do
	{
		l = 4;
		buf.Read(&token, l);

		switch (token)
		{
			case 'btn ':
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				ReadCString(buf, 255, label);
				l = 4;
				buf.Read(&x, l);
				nv = new BButton(r, name, label, new BMessage(x));
				v->AddChild(nv);
				if (strcmp(name, "ok") == 0)
					SetDefaultButton((BButton *)nv);
				break;
			case 'tabs':
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				nv = new CTabSheet(r, name);
				v->AddChild(nv);
				v = nv;
				break;
			case 'shyt':
			{
				CTabSheet *t = dynamic_cast<CTabSheet*>(v);
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				v = t->AddSheet(name);
				break;
			}
			case 'box ':
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				nv = AddBox(r, *name ? name : NULL, v);
				v = nv;
				break;
			case 'ppup':
			{
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				ReadCString(buf, 255, label);
				buf.Read(&x, 4);
				AddPopUpMenu(r, name, *label ? label : NULL, v);
				if (x) ((BMenuField *)FindView(name))->SetDivider(x);
				nv = v->ChildAt(v->CountChildren() - 1);
				break;
			}
			case 'txte':
			{
				char allowed[256], text[256];
				int length;

				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				ReadCString(buf, 255, label);
				ReadCString(buf, 255, text);
				ReadCString(buf, 255, allowed);
				buf.Read(&length, 4);
				nv = AddTextControl(r, name, *label ? label : NULL,
					*text ? text : NULL, *allowed ? allowed : NULL,
					length, v);
				break;
			}
			case 'chkb':
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				ReadCString(buf, 255, label);
				buf.Read(&x, 4);
				nv = AddCheckBox(r, name, label, x ? B_CONTROL_ON : B_CONTROL_OFF, v);
				break;
			case 'rdbt':
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				ReadCString(buf, 255, label);
				buf.Read(&x, 4);
				nv = AddRadioButton(r, name, label, x ? B_CONTROL_ON : B_CONTROL_OFF, v);
				break;
			case 'view':
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				nv = new BView(r, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
				nv->SetViewColor(kB_GrayTable[6]);
				v->AddChild(nv);
				v = nv;
				break;
			case 'list':
				ReadRect(buf, r);
				r.right -= B_V_SCROLL_BAR_WIDTH;
				ReadCString(buf, 255, name);
				nv = AddListView(r, name, v);
//				v = nv->Parent();
				break;
			case 'sttx':
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				ReadCString(buf, 255, label);
				nv = AddStringView(r, name, label, v);
				break;
			case 'txtv':
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				nv = AddTextView(r, name, v);
				break;
			case 'user':
				ReadRect(buf, r);
				ReadCString(buf, 255, name);
				buf.Read(&x, 4);
				nv = AddUserView(r, name, x, v);
				break;
			case 'end ':
				v = v->Parent();
				break;
			case 0:
				v = NULL;	// causes drop out of loop
				break;
			default:
				THROW((errDamagedResources));
		}
	}
	while (v && buf.Position() < size);
} /* CRDialog::ConstructFromTemplate */

void CRDialog::InitValues(int resID, BPositionIO& buf,
		BRect& frame, char *name, window_type& type,
		int& flags, BWindow *owner)
{
	if (gFactor < 0)
		gFactor = be_plain_font->Size() / 10.0;

	if (resID >= 0)
	{
		const void *p = gResourceManager.LoadDialog(resID);
		FailNil(p);
		char *cp = (char *)p;
		buf.Write(cp, gResourceManager.HandleSize(p));
	}

	buf.Seek(0, SEEK_SET);

	uint32 token;
	ssize_t l;

	l = 4;
	buf.Read(&token, l);

	if (token == 'dlog')
		type = B_TITLED_WINDOW;
	else if (token == 'mdlg')
		type = B_MODAL_WINDOW;
	else
		THROW((errDamagedResources));

	ReadRect(buf, frame);
	ReadCString(buf, 255, name);

	if (owner)
	{
		BRect r = owner->Frame();
		frame.OffsetBy((r.Width() - frame.Width()) / 2,
			(r.Height() - frame.Height()) / 3);
	}
	else
	{
		BScreen s;
		frame.OffsetBy((s.Frame().Width() - frame.Width()) / 2,
			(s.Frame().Height() - frame.Height()) / 3);
	}

	flags = B_NOT_RESIZABLE | B_NOT_ZOOMABLE;
} /* CRDialog::InitValues */

BView* CRDialog::AddUserView(BRect r, const char *name,
	uint32 v, BView *parent)
{
	THROW((errDamagedResources));
	return NULL;
} /* CRDialog::AddUserView */
