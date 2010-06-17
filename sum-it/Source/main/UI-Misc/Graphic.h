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
	Graphic
	
	Copyright 1997, Hekkelman Programmatuur
	
*/

#ifndef GRAPHIC_H
#define GRAPHIC_H

class CCellView;

#include <BeBuild.h>
#include <Handler.h>
#include <Rect.h>
#include <Picture.h>
#include <Region.h>

#ifndef cell_defined
#define cell_defined 1
struct cell {
	short v, h;

	cell() { v = h = 0; };
};

struct range {
	short top, left, bottom, right;
	range() { top = left = bottom = right = 0; };
};
#endif

class _EXPORT CGraphic : public BHandler {
public:
			CGraphic(cell anchor, BRect frame, const char *name);
virtual	~CGraphic();
			
			BRect Bounds();
			BRect Frame();
			bool IsVisible();
		
			void Copy(BPicture*& pict, BRect& r);
		
virtual	void Draw(BRect updateRect);
virtual	void MouseDown(BPoint where);
		
virtual	void FrameResized(float w, float h);
virtual	void FrameMoved(float x, float y);
		
virtual	void MakeFocus(bool active);
			bool HasFocus();
		
			void ResizeTo(float w, float h);
			void MoveTo(float x, float y);
			void MoveBy(float dx, float dy);
		
			void MessageReceived(BMessage *msg);
			bool HandleMouseDown(BPoint where);
			CGraphic* Contains(BPoint where);
			void HandleUpdate(BRect updateRect);
			void CreateClip(BRegion& clip);
			void ClipAbove(BRegion& clip);
			void AttachedToCellView(CCellView *parent);
			void RemovedFromCellView();
			
			CGraphic* Next();
			void SetNext(CGraphic* next);
			void Remove(CGraphic *g);
		
			BView* Parent()	{ return (BView *)fParent; };
			void GetPosition(cell& anchor, BRect& size);
			void SetPosition(cell anchor, BRect size);
		
protected:
			void TrackResize(BPoint where);
			void TrackDrag(BPoint where);
		
			void DrawBorders();
		
						bool			fHasFocus;
						bool			fMakingCopy;
						cell			fAnchor;
						float		fWidth, fHeight;
						BPoint		fOffset;
						CCellView		*fParent;
						CGraphic		*fNext, *fPrevious;
};

#if !__INTEL__
#	pragma export reset
#endif

#endif
