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

#define B_EOF	0x00A		//End of File
#define CALCCOUNT	0x00C		//Iteration Count
#define CALCMODE	0x00D		//Calculation Mode
#define PRECISION	0x00E		//Precision
#define REFMODE	0x00F		//Reference Mode
#define DELTA	0x010		//Iteration Increment
#define ITERATION	0x011		//Iteration Mode
#define PROTECT	0x012		//Protection Flag
#define PASSWORD	0x013		//Protection Password
#define TABID	0x03D		//Sheet Tab Index Array (BIFF7)
#define HEADER	0x014		//Print Header on Each Page
#define FOOTER	0x015		//Print Footer on Each Page
#define EXTERNCOUNT	0x016		//Number of External References
#define EXTERNSHEET	0x017		//External Reference 
#define WINDOWPROTECT	0x019		//Windows Are Protected
#define VERTICALPAGEBREAKS	0x01A		//Explicit Column Page Breaks
#define FILESHARING2	0x0A5		//File-Sharing Info for Shared Lists (BIFF7)
#define HORIZONTALPAGEBREAKS	0x01B		//Explicit Row Page Breaks
#define NOTE	0x01C		//Note Associated with a Cell
#define SELECTION	0x01D		//Current Selection
#define DIMENSIONS	0x000		//Cell Table Size
#define BLANK	0x001		//Cell Value, Blank Cell
#define NUMBER	0x003		//Cell Value, Floating-Point Number
#define LABEL	0x004		//Cell Value, String Constant
#define BOOLERR	0x005		//Cell Value, Boolean or Error 
#define STRING	0x007		//String Value of a Formula
#define ROW	0x008		//Describes a Row
#define INDEX	0x00B		//Index Record 
#define NAME	0x018		//Defined Name
#define B_1904	0x022		//1904 Date System
#define ARRAY	0x021		//Array-Entered Formula
#define EXTERNNAME	0x023		//Externally Referenced Name 
#define DEFAULTROWHEIGHT	0x025		//Default Row Height
#define FONT	0x031		//Font Description
#define TABLE	0x036		//Data Table
#define WINDOW2	0x03E		//Sheet Window Information
#define LEFTMARGIN	0x026		//Left Margin Measurement
#define RIGHTMARGIN	0x027		//Right Margin Measurement
#define RK	0x07E		//Cell Value, RK Number 
#define TOPMARGIN	0x028		//Top Margin Measurement
#define BOTTOMMARGIN	0x029		//Bottom Margin Measurement
#define STYLE	0x093		//Style Information
#define PRINTHEADERS	0x02A		//Print Row/Column Labels
#define PRINTGRIDLINES	0x02B		//Print Gridlines Flag
#define FILEPASS	0x02F		//File Is Password-Protected
#define CONTINUE	0x03C		//Continues Long Records 
#define WINDOW1	0x03D		//Window Information
#define BACKUP	0x040		//Save Backup Version of the File
#define FORMULA	0x006		//Cell Formula 
#define PANE	0x041		//Number of Panes and Their Position
#define FORMAT	0x01E		//Number Format 
#define CODEPAGE	0x042		//Default Code Page
#define PLS	0x04D		//Environment-Specific Print Record
#define DCON	0x050		//Data Consolidation Information
#define DCONREF	0x051		//Data Consolidation References
#define DCONNAME	0x052		//Data Consolidation Named References
#define DEFCOLWIDTH	0x055		//Default Width for Columns
#define XCT	0x059		//CRN Record Count
#define CRN	0x05A		//Nonresident Operands
#define FILESHARING	0x05B		//File-Sharing Information 
#define WRITEACCESS	0x05C		//Write Access User Name
#define OBJ	0x05D		//Describes a Graphic Object
#define UNCALCED	0x05E		//Recalculation Status
#define SAVERECALC	0x05F		//Recalculate Before Save
#define TEMPLATE	0x060		//Workbook Is a Template
#define OBJPROTECT	0x063		//Objects Are Protected
#define COLINFO	0x07D		//Column Formatting Information
#define IMDATA	0x07F		//Image Data
#define GUTS	0x080		//Size of Row and Column Gutters
#define BOF	0x009		//Beginning of File
#define WSBOOL	0x081		//Additional Workspace Information
#define GRIDSET	0x082		//State Change of Gridlines Option
#define HCENTER	0x083		//Center Between Horizontal Margins
#define VCENTER	0x084		//Center Between Vertical Margins
#define BOUNDSHEET	0x085		//Sheet Information
#define WRITEPROT	0x086		//Workbook Is Write-Protected
#define ADDIN	0x087		//Workbook Is an Add-in Macro
#define EDG	0x088		//Edition Globals
#define PUB	0x089		//Publisher
#define COUNTRY	0x08C		//Default Country and WIN.INI Country
#define HIDEOBJ	0x08D		//Object Display Options
#define SORT	0x090		//Sorting Options
#define SUB	0x091		//Subscriber
#define PALETTE	0x092		//Color Palette Definition
#define LHRECORD	0x094		//.WK? File Conversion Information
#define LHNGRAPH	0x095		//Named Graph Information
#define SOUND	0x096		//Sound Note
#define LPR	0x098		//Sheet Was Printed Using LINE.PRINT( )
#define STANDARDWIDTH	0x099		//Standard Column Width
#define FNGROUPNAME	0x09A		//Function Group Name
#define FILTERMODE	0x09B		//Sheet Contains Filtered List
#define FNGROUPCOUNT	0x09C		//Built-in Function Group Count
#define AUTOFILTERINFO	0x09D		//Drop-Down Arrow Count
#define AUTOFILTER	0x09E		//AutoFilter Data
#define SCL	0x0A0		//Window Zoom Magnification
#define SETUP	0x0A1		//Page Setup
#define COORDLIST	0x0A9		//Polygon Object Vertex Coordinates
#define GCW	0x0AB		//Global Column Width Flags
#define SCENMAN	0x0AE		//Scenario Output Data
#define SCENARIO	0x0AF		//Scenario Data
#define SXVIEW	0x0B0		//View Definition
#define SXVD	0x0B1		//View Fields
#define SXVI	0x0B2		//View Item
#define SXIVD	0x0B4		//Row/Column Field IDs
#define SXLI	0x0B5		//Line Item Array
#define SXPI	0x0B6		//Page Item
#define DOCROUTE	0x0B8		//Routing Slip Information
#define RECIPNAME	0x0B9		//Recipient Name
#define SHRFMLA	0x0BC		//Shared Formula
#define MULRK	0x0BD		//Multiple RK Cells
#define MULBLANK	0x0BE		//Multiple Blank Cells
#define MMS	0x0C1		//ADDMENU/DELMENU Record Group Count
#define ADDMENU	0x0C2		//Menu Addition
#define DELMENU	0x0C3		//Menu Deletion
#define SXDI	0x0C5		//Data Item
#define SXSTRING	0x0CD		//String
#define SXTBL	0x0D0		//Multiple Consolidation Source Info
#define SXTBRGIITM	0x0D1		//Page Item Name Count
#define SXTBPG	0x0D2		//Page Item Indexes
#define OBPROJ	0x0D3		//Visual Basic Project
#define SXIDSTM	0x0D5		//Stream ID
#define RSTRING	0x0D6		//Cell with Character Formatting
#define DBCELL	0x0D7		//Stream Offsets
#define BOOKBOOL	0x0DA		//Workbook Option Flag
#define SXEXT	0x0DC		//External Source Information
#define SCENPROTECT	0x0DD		//Scenario Protection
#define OLESIZE	0x0DE		//Size of OLE Object
#define UDDESC	0x0DF		//Description String for Chart Autoformat
#define XF	0x0E0		//Extended Format 
#define INTERFACEHDR	0x0E1		//Beginning of User Interface Records
#define INTERFACEEND	0x0E2		//End of User Interface Records
#define SXVS	0x0E3		//View Source
#define TABIDCONF	0x0EA		//Sheet Tab ID of Conflict History (BIFF7)

#define B_EOF_txt	"End of File"
#define CALCCOUNT_txt	"Iteration Count"
#define CALCMODE_txt	"Calculation Mode"
#define PRECISION_txt	"Precision"
#define REFMODE_txt	"Reference Mode"
#define DELTA_txt	"Iteration Increment"
#define ITERATION_txt	"Iteration Mode"
#define PROTECT_txt	"Protection Flag"
#define PASSWORD_txt	"Protection Password"
#define TABID_txt	"Sheet Tab Index Array (BIFF7)"
#define HEADER_txt	"Print Header on Each Page"
#define FOOTER_txt	"Print Footer on Each Page"
#define EXTERNCOUNT_txt	"Number of External References"
#define EXTERNSHEET_txt	"External Reference "
#define WINDOWPROTECT_txt	"Windows Are Protected"
#define VERTICALPAGEBREAKS_txt	"Explicit Column Page Breaks"
#define FILESHARING2_txt	"File-Sharing Info for Shared Lists (BIFF7)"
#define HORIZONTALPAGEBREAKS_txt	"Explicit Row Page Breaks"
#define NOTE_txt	"Note Associated with a Cell"
#define SELECTION_txt	"Current Selection"
#define DIMENSIONS_txt	"Cell Table Size"
#define BLANK_txt	"Cell Value, Blank Cell"
#define NUMBER_txt	"Cell Value, Floating-Point Number"
#define LABEL_txt	"Cell Value, String Constant"
#define BOOLERR_txt	"Cell Value, Boolean or Error "
#define STRING_txt	"String Value of a Formula"
#define ROW_txt	"Describes a Row"
#define INDEX_txt	"Index Record "
#define NAME_txt	"Defined Name"
#define B_1904_txt	"1904 Date System"
#define ARRAY_txt	"Array-Entered Formula"
#define EXTERNNAME_txt	"Externally Referenced Name "
#define DEFAULTROWHEIGHT_txt	"Default Row Height"
#define FONT_txt	"Font Description"
#define TABLE_txt	"Data Table"
#define WINDOW2_txt	"Sheet Window Information"
#define LEFTMARGIN_txt	"Left Margin Measurement"
#define RIGHTMARGIN_txt	"Right Margin Measurement"
#define RK_txt	"Cell Value, RK Number "
#define TOPMARGIN_txt	"Top Margin Measurement"
#define BOTTOMMARGIN_txt	"Bottom Margin Measurement"
#define STYLE_txt	"Style Information"
#define PRINTHEADERS_txt	"Print Row/Column Labels"
#define PRINTGRIDLINES_txt	"Print Gridlines Flag"
#define FILEPASS_txt	"File Is Password-Protected"
#define CONTINUE_txt	"Continues Long Records "
#define WINDOW1_txt	"Window Information"
#define BACKUP_txt	"Save Backup Version of the File"
#define FORMULA_txt	"Cell Formula "
#define PANE_txt	"Number of Panes and Their Position"
#define FORMAT_txt	"Number Format "
#define CODEPAGE_txt	"Default Code Page"
#define PLS_txt	"Environment-Specific Print Record"
#define DCON_txt	"Data Consolidation Information"
#define DCONREF_txt	"Data Consolidation References"
#define DCONNAME_txt	"Data Consolidation Named References"
#define DEFCOLWIDTH_txt	"Default Width for Columns"
#define XCT_txt	"CRN Record Count"
#define CRN_txt	"Nonresident Operands"
#define FILESHARING_txt	"File-Sharing Information "
#define WRITEACCESS_txt	"Write Access User Name"
#define OBJ_txt	"Describes a Graphic Object"
#define UNCALCED_txt	"Recalculation Status"
#define SAVERECALC_txt	"Recalculate Before Save"
#define TEMPLATE_txt	"Workbook Is a Template"
#define OBJPROTECT_txt	"Objects Are Protected"
#define COLINFO_txt	"Column Formatting Information"
#define IMDATA_txt	"Image Data"
#define GUTS_txt	"Size of Row and Column Gutters"
#define BOF_txt	"Beginning of File"
#define WSBOOL_txt	"Additional Workspace Information"
#define GRIDSET_txt	"State Change of Gridlines Option"
#define HCENTER_txt	"Center Between Horizontal Margins"
#define VCENTER_txt	"Center Between Vertical Margins"
#define BOUNDSHEET_txt	"Sheet Information"
#define WRITEPROT_txt	"Workbook Is Write-Protected"
#define ADDIN_txt	"Workbook Is an Add-in Macro"
#define EDG_txt	"Edition Globals"
#define PUB_txt	"Publisher"
#define COUNTRY_txt	"Default Country and WIN.INI Country"
#define HIDEOBJ_txt	"Object Display Options"
#define SORT_txt	"Sorting Options"
#define SUB_txt	"Subscriber"
#define PALETTE_txt	"Color Palette Definition"
#define LHRECORD_txt	".WK? File Conversion Information"
#define LHNGRAPH_txt	"Named Graph Information"
#define SOUND_txt	"Sound Note"
#define LPR_txt	"Sheet Was Printed Using LINE.PRINT( )"
#define STANDARDWIDTH_txt	"Standard Column Width"
#define FNGROUPNAME_txt	"Function Group Name"
#define FILTERMODE_txt	"Sheet Contains Filtered List"
#define FNGROUPCOUNT_txt	"Built-in Function Group Count"
#define AUTOFILTERINFO_txt	"Drop-Down Arrow Count"
#define AUTOFILTER_txt	"AutoFilter Data"
#define SCL_txt	"Window Zoom Magnification"
#define SETUP_txt	"Page Setup"
#define COORDLIST_txt	"Polygon Object Vertex Coordinates"
#define GCW_txt	"Global Column Width Flags"
#define SCENMAN_txt	"Scenario Output Data"
#define SCENARIO_txt	"Scenario Data"
#define SXVIEW_txt	"View Definition"
#define SXVD_txt	"View Fields"
#define SXVI_txt	"View Item"
#define SXIVD_txt	"Row/Column Field IDs"
#define SXLI_txt	"Line Item Array"
#define SXPI_txt	"Page Item"
#define DOCROUTE_txt	"Routing Slip Information"
#define RECIPNAME_txt	"Recipient Name"
#define SHRFMLA_txt	"Shared Formula"
#define MULRK_txt	"Multiple RK Cells"
#define MULBLANK_txt	"Multiple Blank Cells"
#define MMS_txt	"ADDMENU/DELMENU Record Group Count"
#define ADDMENU_txt	"Menu Addition"
#define DELMENU_txt	"Menu Deletion"
#define SXDI_txt	"Data Item"
#define SXSTRING_txt	"String"
#define SXTBL_txt	"Multiple Consolidation Source Info"
#define SXTBRGIITM_txt	"Page Item Name Count"
#define SXTBPG_txt	"Page Item Indexes"
#define OBPROJ_txt	"Visual Basic Project"
#define SXIDSTM_txt	"Stream ID"
#define RSTRING_txt	"Cell with Character Formatting"
#define DBCELL_txt	"Stream Offsets"
#define BOOKBOOL_txt	"Workbook Option Flag"
#define SXEXT_txt	"External Source Information"
#define SCENPROTECT_txt	"Scenario Protection"
#define OLESIZE_txt	"Size of OLE Object"
#define UDDESC_txt	"Description String for Chart Autoformat"
#define XF_txt	"Extended Format "
#define INTERFACEHDR_txt	"Beginning of User Interface Records"
#define INTERFACEEND_txt	"End of User Interface Records"
#define SXVS_txt	"View Source"
#define TABIDCONF_txt	"Sheet Tab ID of Conflict History (BIFF7)"
