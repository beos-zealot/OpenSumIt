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

type 'MENU' {
	pstring;						// name
	array {
		switch {
			case Item:
				key byte = 1;
				pstring;				 	// label
				longint;					// command
				integer none = 0,			// modifier keys
						shift = 1,
						control = 4;
				byte noKey = 0;				// command key
			case ColorItem:
				key byte = 2;
				pstring;					// label
				longint;					// command
				integer none = 0;			// modifiers
				byte noKey = 0;				// key
				byte;						// color red
				byte;						//		 green
				byte;						//		 blue
			case Separator:
				key byte = 3;
			case Submenu:
				key byte = 4;
				longint;					// submenuid
		}
	};
	byte = 0;
};

type 'MBAR' {
	array {
		integer;		// menu id's
	};
};

Resource 'MENU' (1, "File")
{
	"File",
	{
		Item		{	"New", 'NewW', none, 'N' },
		Item		{	"Open", 'OpnW', none, 'O' },
		Item		{	"Close", 'ClsW', none, 'W' },
		Separator	{},
		Item		{	"Save", 'Save', none, 'S' },
		Item		{	"Save As", 'SvAs', shift, 'S' },
		Item		{	"Revert", 'Rvrt', none, noKey },
		Separator	{},
		Item		{	"Page Setup", 'Pgsu', none, noKey },
		Item		{	"Print", 'Prnt', none, 'P' },
		Separator	{},
		Item		{	"Quit", 'ClsW', none, 'Q' }
	}
};

Resource 'MENU' (2, "Edit")
{
	"Edit",
	{
		Item		{	"Undo", 'Undo', none, 'Z'},
		Item		{	"Redo", 'Redo', shift, 'Z' },
		Separator	{},
		Item		{	"Cut", 'CCUT', none, 'X' },
		Item		{	"Copy", 'COPY', none, 'C' },
		Item		{	"Paste", 'PSTE', none, 'V' },
		Item		{	"Paste Special", 'Pspc', control, 'V' },
		Item		{	"Paste Function", 'Pfnc', shift, 'V' },
		Separator	{},
		Item		{	"Select All", 'SAll', none, 'A' },
		Item		{	"Clear", 'Cler', none, noKey },
		Separator	{},
		Item		{	"Insert Column", 'InRC', none, noKey },
		Item		{	"Delete Column", 'DlRC', none, noKey },
	}
};

Resource 'MENU' (3, "Format")
{
	"Format",
	{
		Submenu		{	10 },
		Submenu		{	11 },
		Submenu		{	12 },
		Submenu		{	13 },
		Submenu		{	14 },
		Separator	{},
		Item		{	"Align General", 'Agnr', none, noKey },
		Item		{	"Align Left", 'Alft', none, noKey },
		Item		{	"Align Center", 'Acnt', none, noKey },
		Item		{	"Align Right", 'Argt', none, noKey },
		Separator	{},
		Item		{	"General", 'Fgnr', none, noKey },
		Item		{	"Currency", 'Fcur', none, noKey },
		Item		{	"Percentage", 'Fprc', none, noKey },
		Item		{	"Fixed", 'Ffix', none, noKey },
		Item		{	"Scientific", 'Fsci', none, noKey },
		Separator	{},
		Item		{	"Date/Time", 'Fdat', none, noKey },
		Item		{	"Digits", 'FdCm', none, noKey },
		Item		{	"Commas", 'Fcms', none, noKey },
	}
};

Resource 'MENU' (4, "Data")
{
	"Data",
	{
		Item		{	"Fill Down", 'FilD', none, 'D' },
		Item		{	"Fill Right", 'FilR', none, 'R' },
		Item		{	"Increment Fill", 'FiCm', none, noKey },
		Item		{	"Sort", 'SrCm', none, noKey },
		Separator	{},
		Item		{	"Names", 'NmCm', none, noKey },
		Separator	{},
		Item		{	"Display Formulas", 'DFrm', none, noKey },
		Separator	{},
		Item		{	"Recalculate", 'Rclc', none, '=' },
		Separator	{},
		Item		{	"Go To", 'GoTC', none, noKey },
		Item		{	"Find", 'FinC', none, 'F' },
		Item		{	"Find Next", 'FinA', none, 'G' },
		Item		{	"Replace", 'Rplc', none, noKey },
		Item		{	"Replace & Find Next", 'RpFn', none, 'L' },
		Item		{	"Replace All", 'RpAl', none, noKey },
	}
};

Resource 'MENU' (5, "Chart")
{
	"Chart",
	{
		Item		{	"Create Chart", 'CGrp', none, noKey },
		Item		{	"Chart Options", 'GOpt', none, noKey },
		Item		{	"Animate", 'AGrf', none, noKey },
	}
};

Resource 'MENU' (6, "View")
{
	"View",
	{
		Item		{	"Freeze", 'Frez', none, noKey },
		Item		{	"Unfreeze", 'UnFr', none, noKey },
		Separator	{},
		Item		{	"Stack", 'Stck', none, noKey },
		Item		{	"Tile", 'Tile', none, noKey },
		Item		{	"Full Screen", 'Zoom', none, '/' },
		Separator	{},
		Item		{	"Show Clipboard", 'ShCl', none, noKey },
		Item		{	"Settings", 'Pref', none, noKey },
		Separator	{},
	}
};

Resource 'MENU' (7, "Help")
{
	"Help",
	{
		Item		{	"On-line Help", 'Help', none, noKey },
		Item		{	"About", '_ABR', none, noKey },
	}
};

Resource 'MENU' (10, "Font")
{
	"Font",
	{
	}
};

Resource 'MENU' (11, "Style")
{
	"Style",
	{
	}
};

Resource 'MENU' (12, "Size")
{
	"Size",
	{
		Item		{	"9", 'Size', none, noKey },
		Item		{	"10", 'Size', none, noKey },
		Item		{	"12", 'Size', none, noKey },
		Item		{	"14", 'Size', none, noKey },
		Item		{	"18", 'Size', none, noKey },
		Item		{	"24", 'Size', none, noKey },
		Item		{	"36", 'Size', none, noKey },
		Item		{	"48", 'Size', none, noKey },
		Item		{	"72", 'Size', none, noKey }
	}
};

Resource 'MENU' (13, "Font Color")
{
	"Font Color",
	{
		ColorItem	{	"Black", 'TClr', none, noKey, 0, 0, 0 },
		ColorItem	{	"Red", 'TClr', none, noKey, 143, 0, 0 },
		ColorItem	{	"Green", 'TClr', none, noKey, 0, 143, 0 },
		ColorItem	{	"Blue", 'TClr', none, noKey, 0, 0, 143 },
		ColorItem	{	"Cyan", 'TClr', none, noKey, 0, 143, 143 },
		ColorItem	{	"Magenta", 'TClr', none, noKey, 143, 0, 143 },
		ColorItem	{	"Yellow", 'TClr', none, noKey, 143, 143, 0 },
		Separator	{},
		Item		{	"Other", 'TCot', none, noKey },
	}
};

Resource 'MENU' (14, "Cell Color")
{
	"Cell Color",
	{
		ColorItem	{	"White", 'CClr', none, noKey, 255, 255, 255 },
		ColorItem	{	"Red", 'CClr', none, noKey, 255, 170, 170 },
		ColorItem	{	"Green", 'CClr', none, noKey, 170, 255, 170 },
		ColorItem	{	"Blue", 'CClr', none, noKey, 170, 170, 255 },
		ColorItem	{	"Cyan", 'CClr', none, noKey, 170, 255, 255 },
		ColorItem	{	"Magenta", 'CClr', none, noKey, 255, 170, 255 },
		ColorItem	{	"Yellow", 'CClr', none, noKey, 255, 255, 170 },
		Separator	{},
		Item		{	"Other", 'CCot', none, noKey },
	}
};

Resource 'MBAR' (1)
{
	{
		1, 2, 3, 4, 5, 6, 7
	}
};

