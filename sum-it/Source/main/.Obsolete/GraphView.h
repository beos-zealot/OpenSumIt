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
	GraphView.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

class CCellView;
class CGraphPlugIn;

class CGraphView : public BView {
public:	
	CGraphView(BRect frame, CCellView *cv);

	virtual void Draw(BRect update);
	void DataChanged();
	void Reposition();
	virtual void SetFont(const BFont *font, uint32 mask = B_FONT_ALL);

protected:
	virtual void MouseDown(BPoint where);
	virtual void MakeFocus(bool isFocus);
	virtual void MouseMoved(BPoint point, ulong transit, const BMessage *message);
	void ResizeGraph(BPoint where, int corner);
	void DragGraph(BPoint where);
	virtual void FrameResized(float width, float height);
	
	virtual void MessageReceived(BMessage *msg);
	virtual void KeyDown(const char *bytes, long numOfBytes);

private:
	void DeleteGraph();
	void DrawHandles();
	void StorePosition();
	bool WaitMouseMoved(BPoint where, bool moveIsOK);
	void DoGraphPopup(BPoint where);
	void ForceRedraw();

	cell fAnchor;
	float fHOffset, fVOffset, fWidth, fHeight;
	CCellView *fCellView;
	CGraphPlugIn *fPlugin;
	BBitmap *fBitmap;
	bool fResizing;
};
