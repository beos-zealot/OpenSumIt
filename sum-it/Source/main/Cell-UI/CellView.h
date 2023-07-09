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
	CellView.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef CELLVIEW_H
#define CELLVIEW_H

#ifndef   SET_H
#include "Set.h"
#endif

#ifndef   RUNARRAY_H
#include "RunArray.h"
#endif

#ifndef   VALUE_H
#include "Value.h"
#endif


#include <View.h>
#include <Region.h>
#include <List.h>
#include <DataIO.h>
#include <PrintJob.h>

#include <cstdio>

class CCellWindow;
class CSelectionView;
class CProgressView;
class CContainer;
class CEditBox;
class CFontMetrics;
class CCellScrollBar;
class CTextControl;
class CName;
class CNameTable;
class CCalculateJob;
class CGraphic;
class CGraphPlugIn;

class CCellView : public BView {
public:
	CCellView(BRect frame, CTextControl* editBox, bool fromFile);
	~CCellView();
	
	void ReInit();
				
	void AttachedToWindow();
	void AttachSelectionView(CSelectionView* selView);
	
	void RecalculateBorderWidths();
	
	void AttachProgressView(CProgressView* inProgressView);
	CProgressView* GetProgressView() { return fProgressView; };
	void StartCalculation(int inCount);
	void StepCalculation();
	void StopCalculation();

	void WindowActivated(bool active);
	void MakeFocus(bool focused = TRUE);
	bool IsEntering() const;
	void SetEntering(bool flag);

// Een paar accessor functies
	CRunArray& GetHeights() 			{ return fCellHeights; };
	CRunArray& GetWidths()  			{ return fCellWidths; };
	void GetSelection(range& r)			{ r = fSelection; };
	CContainer* GetContainer()			{ return fContainer; };
	int BorderFontID()				{ return fBorderFontID; };
	bool DoesAutoRecalc()			{ return fAutoRecalc; };
	bool DoesDisplayZero()			{ return fDisplayZero; };
	bool PrintsGrid()				{ return fPrGrid; };
	bool PrintsBorders()			{ return fPrBorders; };
	bool ShowsGrid()				{ return fShowGrid; };
	void SetShowGrid(bool on)	{ fShowGrid = on; }
	bool ShowsBorders()			{ return fShowBorders; };
	void SetShowBorders(bool on)	{ fShowBorders = on; }
	cell CurCell()				{ return fCurCell; };
	int FoundOffset()				{ return fFOffset; };
	BRect CellBounds()				{ return fCellBounds; };
	
	range GetNamedRange(const char *inName);
	void AddNamedRange(const char *inName, range inRange);
	void RemoveNamedRange(const char *inName);
	bool IsNamedRange(const char *inName);
	CNameTable* GetNameTable() { return fNames; };

// cell methods
	void GetCellRect(cell c, BRect& r);
	void GetCellTopLeft(cell c, BPoint& p);
	void GetCellBounds(cell c, BRect& r);
	long GetCellSpan(orientation posture, bool upDirection);
	void SelectionToRect(BRect& rect);
	void SelectionToRegion(BRegion& rgn);
	float GetColumnWidth(int colNr);
	bool GetCellHitBy(BPoint where, cell& c);
	void SetSylkColumnWidths(int start, int stop, int width);

// Scrolling methods
	void ScrollToSelection();
	void ScrollToPosition(long x, long y);
	void ScrollToNewValues(long x, long y);
	void AdjustScrollBars();
	void ScrollRect(BRect r, float dx, float dy);

// Draw methods
	void Draw(BRect updateRect);
	void DrawBorders();
	void DrawBorderCell(int nr, bool horizontal);
	void DrawStatus(range *selection = NULL, bool drawEditField = TRUE);
	void DrawGrid();
	void DrawLine(BRegion& updateRgn, int lineNr);
	void DrawCell(cell c);
	void DrawCellInRect(cell c, BRect r);
	void TouchLine(int lineNr);
	void DrawAllLines(void);
	void HiliteSelection(bool background = false, bool subtract = true);
	void ChangeSelection(BRegion *oldRgn, BRegion *newRgn);
	void SetBorderFont(CFontMetrics **metrics = NULL);
	void DrawButtons();
	void GetPictureOfSelection(BPicture *pic, BRect& r);

// Ants
	virtual void Pulse();
	void MarchAnts();
	void ClearAnts();

// Manipulation
	typedef enum {
		stepLeft, stepRight, stepUp, stepDown
	} StepDirection;

	bool EnterCell(const char *s, BMessage *inMessage);
	void RejectCell();
	void SumButton();
	
	void CalculateAllCells(bool override = false);
	void CancelCalculation();

	virtual	void MouseDown(BPoint point);
	bool WaitMouseMoved(BPoint where, bool moveIsOK);
	void SelectCell(BPoint where);
	void ResizeRow(BPoint where, int rowNr);
	void ResizeCol(BPoint where, int colNr);
	void SelectRow(BPoint where, int rowNr);
	void SelectCol(BPoint where, int colNr);
	void Step(StepDirection step);
	
	void BorderMenu(BPoint where);
	void GridMenu(BPoint where);
	
	void SetSelection(range r);
	void SetSelection(cell c)				{ SetSelection(range(c.h, c.v, c.h, c.v)); };
	bool PointIsInSelection(BPoint point);

	void KeyDown(const char *bytes, int32 numBytes);
	void MessageReceived(BMessage *theMessage);

	void MenusBeginning();

// Drag & Drop
	void StartDrag(BPoint where, bool copy);
	void MouseMoved(BPoint point, uint32 transit, const BMessage *message);
	void HandleDrop(BMessage *inMessage);

// IO
	void Write(BPositionIO& stream);
	void Read(BPositionIO& stream);

	void WriteWidths(BPositionIO& stream);
	void ReadWidths(BPositionIO& stream);

	void WriteFontSizes(BPositionIO& stream);
	void ReadFontSizes(BPositionIO& stream);

	void WriteCharts(BPositionIO& stream);
	void ReadChart(BPositionIO& stream, int size);
	
	void ReadSylk (BPositionIO& in);
	void WriteSylk (FILE *f);

// Find
	void Find(BMessage *msg);
	bool FindNext(bool wrap);
	bool CanReplace(const char *what, bool ignoreCase);
	void ResetOffset(int newOffset)	{ fFOffset = newOffset; };
	
// Printing
	void Repaginate();
	void DoPageSetup();
	void DoPrint();
	void PrintPage(BPrintJob& prJob, int pageNr);
	int CountPages()				{ return fHPageCount * fVPageCount; };
	int GetPageNrForCell(cell c);

// External refs
	int MakeXRef(const char *fileName);
	void GetXRefDesc(int ref, cell c, char *desc);
	void ResolveXRef(int ref, cell c, Value& val);

// Graph support
	CGraphic* FirstGraphic();
	void AddGraphic(CGraphic *g);
	void RemoveGraphic(CGraphic *g);
	void MoveToFront(CGraphic* g);
	void SetClip(BRegion* rgn);
	void GetClip(BRegion& clip);
	bool GraphicHasFocus();
	void OffsetGraphicReferences(bool vertical, int start, int count);
	
	CGraphPlugIn* CreateChart(cell anchor, BRect frame,
		const char *name, range r);
	void UpdateCharts();

protected:
	BList fXRefs;

	enum {
		curNoCursor,
		curHand,
		curPlus,
		curColResize,
		curRowResize,
		curDrag
	};

	BRect			fCellBounds;
	CNameTable		*fNames;
	CSet				fDirtyLines;
	range			fSelection;
	cell				fCurCell;
	cell				fAnchorCell;
	cell				fFrozen;
	float			fBorderWidth, fBorderHeight;
	int				fBorderFontID;
	CRunArray			fCellWidths, fCellHeights;
	cell				fPosition;
	cell				fMaxPosition;
	CSelectionView*	fSelView;
	CProgressView*	fProgressView;
	CContainer*		fContainer;
	CTextControl*		fEditBox;
	CCellScrollBar	*fHScrollBar, *fVScrollBar;
	int				fCurrentCursor;
	char*			fPageSetup;
	ssize_t			fPageSetupSize;
	BRect			fPaperRect;
	BRect			fMargins;
	int				fVPageCount, fHPageCount;
	CSet				fVPageBreaks, fHPageBreaks;
	bool				fShowGrid;
	bool				fShowBorders;
	bool				fEntering;
	bool				fPrGrid;
	bool				fPrBorders;
	bool				fOnlyTheLines;
	bool				fWindowActive;
	bool				fDragIsAcceptable;
	bool				fFromFile;
	bool				fFindInFormulas;
	bool				fIgnoreCase;
	bool				fWrap;
	bool				fAutoRecalc;
	bool				fShowFormulas;
	bool				fDragACopy;
	bool				fSuppressHighlight;
	bool				fDisplayZero;
	bool				fNew;		// fucking beos...
	char*			fWhatToFind;
	char*			fReplaceWith;
	int				fFOffset;
	CCalculateJob		*fJob;
	CGraphic			*fFirstGraphic;

	static double		fgAntsTime;
	static int		fgAntsPat;
};

class StClipCells {
public:
	StClipCells(CCellView *v, BRegion *newClip = NULL);
	~StClipCells();
private:
	CCellView *fView;
	BRegion fOldClip;
};

#endif
