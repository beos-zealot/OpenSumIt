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
	GraphPlugIn.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef GRAPHPLUGIN_H
#define GRAPHPLUGIN_H

#ifndef   GRAPHIC_H
#include "Graphic.h"
#endif

#include <Font.h>
#include <DataIO.h>

class CGraphView;
class CGraphOptionsDialog;

enum {
	msg_DeleteGraph = 'DelG',
	msg_ChartSettings = 'CSet'
};

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

class _EXPORT CGraphPlugIn : public CGraphic {
public:
			CGraphPlugIn(cell anchor, BRect frame, const char *name, range r, image_id addon);
			~CGraphPlugIn();
					
virtual	void DataChanged();
virtual	void DisplayOptions();
virtual	void GetOptionsTemplate(BMallocIO& tmpl);
					
			void MouseDown(BPoint where);
			void MessageReceived(BMessage *msg);
					
virtual	void MakeFocus(bool active);

virtual	BMessage *Settings();
						range Range()				{ return fRange; };
						void SetRange(range r);

virtual	char* PlugName() = 0;

						image_id AddOn()			{ return fAddOn; };
		
			bool GetCellBool(cell c, bool *b);
			bool GetCellText(cell c, char *s);
			bool GetCellDouble(cell c, double *d);
			bool GetCellTime(cell c, time_t *t);
			void GetCellValue(cell c, char *s);
					
			char** GuessVerticalLabels(range r);
			char** GuessHorizontalLabels(range r);
				
			void DisposeLabels(char** labels);
				
			void GetMinAndMaxValues(range r, bool includeZero,
								float height, float& min, float& max, float& step,
								float& lstep);

			double Interpolate(cell c, bool horizontal = true);

						range fRange;
						BFont fGraphFont;
						image_id fAddOn;
						CGraphOptionsDialog *fGraphOptionsDialog;
};

typedef CGraphPlugIn* (*CreateGraph)(cell anchor, BRect frame, range r, image_id addon);

#if !BUILDING_APP

_IMPORT float ftoa(double d, char *s);
_IMPORT double Round(double d);
_IMPORT double Round(double d, int digits);

extern "C" {

_EXPORT extern char Name[];
_EXPORT extern uchar Icon[];

_EXPORT CGraphPlugIn* NewGraphPlugIn(cell anchor, BRect frame, range r, image_id addon);

}
#endif

#endif
