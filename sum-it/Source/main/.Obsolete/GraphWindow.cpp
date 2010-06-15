/*
	GraphWindow.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

// CGraphWindow

#include "CellWindow.h"
#include "CellView.h"
#include "Container.h"
#include "GraphPlugIn.h"
#include "GraphWindow.h"
#include "MyError.h"
#include "Utils.h"
#include "MyClipboard.h"

CGraphWindow::CGraphWindow(CCellWindow *inOwner, BRect inRect, image_id inAddOn)
	: BWindow(inRect, "Graph", B_DOCUMENT_WINDOW, 0)
{
	fOwner  = inOwner;
	fAddOnImage = inAddOn;
	
	inOwner->GetCellView()->GetSelection(&fSelection);
	fContainer = inOwner->GetCellView()->GetContainer();
	
	CGraphPlugIn* (*createGraph)(BRect frame, range selection, CContainer *container); 
	if (get_image_symbol(fAddOnImage, "NewGraphPlugIn__F5BRect5rangeP10CContainer",
		B_SYMBOL_TYPE_TEXT, &createGraph) != B_NO_ERROR)
		THROW((errCreatingGraph));
	
	BRect r(Bounds());
	r.bottom = GetMBarHeight();
	BMenuBar *mbar = new BMenuBar(r, "mbar");
	AddChild(mbar);

	BMenu *file = new BMenu("File");
	file->AddItem(new BMenuItem("Close", new BMessage(B_QUIT_REQUESTED), 'W'));
	mbar->AddItem(file);
	
	BMenu *edit = new BMenu("Edit");
	edit->AddItem(new BMenuItem("Copy", new BMessage(B_COPY), 'C'));
	mbar->AddItem(edit);
	
	r = Bounds();
	r.top += GetMBarHeight();
	
	BView *background = new BView(r, "background", B_FOLLOW_ALL_SIDES,
		B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE);
	AddChild(background);
	
	r.OffsetTo(0, 0);
	r.InsetBy(B_V_SCROLL_BAR_WIDTH, B_H_SCROLL_BAR_HEIGHT);
	fMainView = (*createGraph)(r, fSelection, fContainer);
	if (fMainView == NULL)
		THROW((errCreatingGraphView));

	background->AddChild(fMainView);
	fMainView->DataChanged();

	Show();
} /* CGraphWindow::CGraphWindow */

CGraphWindow::~CGraphWindow()
{
	fMainView->RemoveSelf();
	delete fMainView;
	unload_add_on(fAddOnImage);
} /* CGraphWindow::~CGraphWindow */

bool CGraphWindow::QuitRequested()
{
	Hide();
	return true;
} /* CGraphWindow::QuitRequested */
	
void CGraphWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case msg_Reload:
			fMainView->DataChanged();
			break;

		case B_COPY:
			Copy();
			break;

		default:
			BWindow::MessageReceived(msg);
			break;
	}
} /* CGraphWindow::MessageReceived */

void CGraphWindow::Copy()
{
	BPicture *pict;
	BRect range;
	
	fMainView->Copy(range, pict);

	if (pict)
		gClipboard->PutChart(pict, range);		
} /* CGraphWindow::Copy */
