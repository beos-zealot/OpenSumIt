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
	CellView.graph

	Copyright 1997, Hekkelman Programmatuur
*/

#ifndef   CELLVIEW_H
#include "CellView.h"
#endif

#ifndef   GRAPHIC_H
#include "Graphic.h"
#endif

#ifndef   GRAPHPLUGIN_H
#include "GraphPlugIn.h"
#endif

#ifndef   FILEFORMAT_H
#include "FileFormat.h"
#endif

#ifndef   UTILS_H
#include "Utils.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   GLOBALS_H
#include "Globals.h"
#endif

#include <cstring>
#include <cstdlib>

#include <Message.h>
#include <Messenger.h>
#include <Entry.h>
#include <Directory.h>
#include <Path.h>
#include <File.h>

CGraphPlugIn* CCellView::CreateChart(cell anchor, BRect frame,
	const char *name, range r)
{
	try
	{
		BEntry eDir;
		FailOSErr(gAppDir->FindEntry("Graphs",  &eDir), errNoGraphDir);
		BDirectory dir(&eDir);
		
		if (!dir.IsDirectory())
			THROW((errNoGraphDir));
		
		BEntry ePlug;
		BFile plug;
		char *plugName;
		image_id addongraph;
		
		while (dir.GetNextEntry(&ePlug) == B_NO_ERROR)
			if (ePlug.IsFile())
			{
				BPath path;
				ePlug.GetPath(&path);
				
				addongraph = load_add_on(path.Path());
				if (addongraph > B_ERROR)
				{
					if (get_image_symbol(addongraph, "Name",
						B_SYMBOL_TYPE_DATA, (void **)&plugName) != B_NO_ERROR)
					{
						unload_add_on(addongraph);
						addongraph = B_ERROR;
						continue;
					}
					
					if (strcmp(plugName, name))
					{
						unload_add_on(addongraph);
						addongraph = B_ERROR;
						continue;
					}
					
					break;
				}
			}
	
		if (addongraph > B_ERROR)
		{
			CreateGraph createGraph;
			if (get_image_symbol(addongraph, "NewGraphPlugIn",
				B_SYMBOL_TYPE_TEXT, (void **)&createGraph) != B_NO_ERROR)
				THROW((errCreatingGraph));

			CGraphPlugIn *g = (*createGraph)(anchor, frame, r, addongraph);
			return g;
		}
		else
			THROW((errGraphNotFound, name));
	}
	catch (CErr& e)
	{
		e.DoError();
	}
	
	return NULL;
} /* CCellView::CreateChart */

CGraphic* CCellView::FirstGraphic()
{
	return fFirstGraphic;
} /* CCellView::FirstGraphic */

void CCellView::AddGraphic(CGraphic *g)
{
	if (!g) return;

	if (fFirstGraphic)
	{
		CGraphic *n = fFirstGraphic;
		while (n->Next()) n = n->Next();
		n->SetNext(g);
	}
	else
		fFirstGraphic = g;
	
	g->AttachedToCellView(this);
	Invalidate(g->Frame());
} /* CCellView::AddGraphic */

void CCellView::RemoveGraphic(CGraphic *g)
{
	CGraphic *f = fFirstGraphic;
	
	if (g == fFirstGraphic)
		fFirstGraphic = g->Next();
	
	Invalidate(g->Frame());
	f->Remove(g);
	g->RemovedFromCellView();
} /* CCellView::RemoveGraphic */

void CCellView::MoveToFront(CGraphic* g)
{
	if (!g || !fFirstGraphic || g == fFirstGraphic) return;
	
	if (fFirstGraphic->HasFocus())
		fFirstGraphic->MakeFocus(false);
	
	fFirstGraphic->Remove(g);
	g->SetNext(fFirstGraphic);
	fFirstGraphic = g;
	
	Invalidate(g->Frame());
} /* CCellView::MoveToFront */

void CCellView::SetClip(BRegion *newClip)
{
	BRegion clip;
	clip.Set(Bounds());
	
	if (fFirstGraphic)
		fFirstGraphic->CreateClip(clip);

	if (newClip) clip.IntersectWith(newClip);

	ConstrainClippingRegion(&clip);
} /* CCellView::SetClip */

void CCellView::GetClip(BRegion& clip)
{
	GetClippingRegion(&clip);
	
	if (! clip.Frame().IsValid())		// R4b3 returns bogus data
		clip.Set(fCellBounds);
		
	if (fFirstGraphic)
		fFirstGraphic->CreateClip(clip);
} // CCellView::GetClip

void CCellView::UpdateCharts()
{
	CGraphic *g = fFirstGraphic;
	
	while (g)
	{
		CGraphPlugIn *p = dynamic_cast<CGraphPlugIn*>(g);
		if (p)
			p->DataChanged();
		g = g->Next();
	}
} /* CCellView::UpdateCharts */

StClipCells::StClipCells(CCellView *view, BRegion *newClip)
{
	fView = view;
	fView->GetClip(fOldClip);
	fView->SetClip(newClip);
} /* StClipCells::StClipCells */

StClipCells::~StClipCells()
{
	fView->ConstrainClippingRegion(NULL);
	fView->ConstrainClippingRegion(&fOldClip);
} /* StClipCells::~StClipCells */

void CCellView::WriteCharts(BPositionIO& stream)
{
	CGraphic *g = fFirstGraphic;
	
	while (g)
	{
		CGraphPlugIn *p = dynamic_cast<CGraphPlugIn*>(g);
		if (p)
		{
			scChunk c;
			scChart ci;
			
			memset(&ci, 0, sizeof(ci));
			
			BMessage s(*p->Settings());
			ssize_t l = s.FlattenedSize();
			char *f = (char *)MALLOC(l);
			s.Flatten(f, l);
			
			p->GetPosition(ci.anchor, ci.frame);
			ci.area = p->Range();
			
			c.type = htons(kscChart);
			c.size = htons(44 + strlen(p->PlugName()) + 1 + l);

			swap_order(ci.anchor);
			swap_order(ci.frame);
			swap_order(ci.area);
			
			CHECKWRITE(stream, &c, 4);
			CHECKWRITE(stream, &ci, 44);
			CHECKWRITE(stream, p->PlugName(), strlen(p->PlugName()) + 1);
			CHECKWRITE(stream, f, l);
			
			FREE(f);
		}
		g = g->Next();
	}
} /* CCellView::WriteCharts */

void CCellView::ReadChart(BPositionIO& stream, int size)
{
	scChart ci;
	char name[256];
	
	CSwapStream  csw(stream) ;
	csw >> ci;

	ReadCString(stream, 255, name);

	BMessage msg(msg_ChartSettings);
	FailOSErr(msg.Unflatten(&stream), "Failed to unflatten chart data");
	msg.Unflatten(&stream);
	
	CGraphPlugIn *g = CreateChart(ci.anchor, ci.frame, name, ci.area);
	if (g)
	{
		if (fFirstGraphic)
		{
			CGraphic *n = fFirstGraphic;
			while (n->Next()) n = n->Next();
			n->SetNext(g);
		}
		else
			fFirstGraphic = g;

		g->AttachedToCellView(this);
		BMessenger(g).SendMessage(&msg);
	}
} /* CCellView::ReadCharts */

bool CCellView::GraphicHasFocus()
{
	return fFirstGraphic && fFirstGraphic->HasFocus();
} /* CCellView::GraphicHasFocus */

void CCellView::OffsetGraphicReferences(bool vertical, int start, int count)
{
	CGraphic *g = fFirstGraphic;
	
	while (g)
	{
		BRect s;
		cell a;
		
		g->GetPosition(a, s);
		if (vertical)
		{
			if (a.h >= start)
			{
				a.h += count;
				g->SetPosition(a, s);
			}
		}
		else
		{
			if (a.v >= start)
			{
				a.v += count;
				g->SetPosition(a, s);
			}
		}
		
		CGraphPlugIn *p = dynamic_cast<CGraphPlugIn*>(g);
		if (p)
		{
			range r = p->Range();
			
			if (vertical)
			{
				if (r.right >= start)
					r.right += count;
				if (r.left >= start)
					r.left += count;
			}
			else
			{
				if (r.bottom >= start)
					r.bottom += count;
				if (r.top >= start)
					r.top += count;
			}
			
			p->SetRange(r);
		}

		g = g->Next();
	}
} // CCellView::OffsetGraphicReferences
