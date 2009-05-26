// CellView.XLIO.c

#include <support/Debug.h>
#include "Excel.h"
#include "XL_Biff_codes.h"
#include "FileFormat.h"

#include <List.h>

// zoek naar sequentie: 09 08 08 00 00 05

const int next[] = { 0x09, 0x08, 0x08, 0x00, 0x00, 0x05 };

struct XLFormula {
	short row, col;
	short x;
	short rwFirst, colFirst;
};

BList gSharedFormulas;

//struct FormatSpec {
//	short id;
//	CellFormat cf;
//
//	FormatSpec(short inID, char *fStr);
//};

//FormatSpec::FormatSpec(short inID, char *fStr)
//{
//	id = inID;
//	char *cp;
//	
//	if ((cp = strchr(fStr, ';')) != NULL)
//		*cp = 0;
//		
//	if ((cp = strchr(fStr, ',')) != NULL)
//		cf.cfCommas = true;
//		
//	cf.cfDigits = 0;
//	if ((cp = strchr(fStr, '.')) != NULL)
//	{
//		while (*++cp == '0')
//			cf.cfDigits++;
//		cf.cfFormat = eFixed;
//	}
//	
//	if ((cp = strchr(fStr, '$')) != NULL)
//		cf.cfFormat = eCurrency;
//	else if ((cp = strchr(fStr, '%')) != NULL)
//		cf.cfFormat = ePercent;
//} // FormatSpec::FormatSpec

bool CExcel5Filter::LocateBof(BPositionIO& stream)
{
	int state = 0;
	char c;

	stream.Seek(0);
	
	try
	{
		while (state < 6)
		{
			stream.Read(&c, sizeof(c));
			
			if (c == next[state])
				state++;
			else if (state > 0)
			{
				state = 0;
				stream.Seek(stream.Position() - 1);
			}
		}
	
		if (state == 6)
		{
			stream.Seek(stream.Position() - 6);
			return true;
		}
	}
	catch(...){}
	return false;
} // LocateBof

const int kBufSize = 2080;

template<class T>
bool GetFromList(BList& lst, short indx, T& v)
{
	if (indx >= 0 && indx < lst->CountItems())
	{
		void *p = lst->ItemAt(indx);
		v = *(T *)&p;
		return true;
	}
	return false;
} // GetFromList

template<class T>
void AppendToList(BList& lst, T v)
{
	void *p;
	p = (void *)(*((long *)&v));
	lst.AddItem(p);
} // AppendToList

void ProcessXLRecord(int code, int len, DStream& inStream, DStream& outStream)
{
	int upcode = code >> 8;
	
	switch (code & 0x00FF)
	{
		case SELECTION:
		{
			char p;
			Read(inStream, p);
			if (p == 3)
			{
				ReadSwapped(inStream, fCurCell.v);
				fCurCell.v++;
				ReadSwapped(inStream, fCurCell.h);
				fCurCell.h++;
				char c;
				ReadSwapped(inStream, x);
				ReadSwapped(inStream, x);
				
				rect r;
				ReadSwapped(inStream, r.top);
				r.top++;
				ReadSwapped(inStream, r.bottom);
				r.bottom++;
				Read(inStream, c);
				r.left = c+1;
				Read(inStream, c);
				r.right = c+1;
				SetSelection(r);
			}
			break;
		}
		case DIMENSIONS:	break;
		case BLANK:	
		{
			ReadSwapped(inStream, row);
			ReadSwapped(inStream, col);
			ReadSwapped(inStream, x);
			
			c.v = row+1;
			c.h = col+1;
			
			fContainer->NewCell(c, v, NULL);
			if (GetFromList(lists+3, x, cf))
				fContainer->SetCellFormat(c, cf);
			break;
		}
		case NUMBER:
		{
			long l[2];
			
			ReadSwapped(inStream, row);
			ReadSwapped(inStream, col);
			ReadSwapped(inStream, x);
		
			ReadSwapped(inStream, l[1]);
			ReadSwapped(inStream, l[0]);
			d = *((double *)l);
			
			v = d;
			c.v = row+1;
			c.h = col+1;
			
			fContainer->NewCell(c, v, NULL);	
			if (GetFromList(lists+3, x, cf))
				fContainer->SetCellFormat(c, cf);
			break;
		}
		case LABEL:
		case RSTRING:
		{
			char label[512];
			short l;
			
			ReadSwapped(inStream, row);
			ReadSwapped(inStream, col);
			ReadSwapped(inStream, x);
			ReadSwapped(inStream, l);
			
			if (l > 511) l = 511;
			Read(inStream, label, l);
			label[l] = 0;
			
			c.v = row+1;
			c.h = col+1;
			v = label;
			
			fContainer->NewCell(c, v, NULL);
			if (GetFromList(lists+3, x, cf))
				fContainer->SetCellFormat(c, cf);
			break;
		}
		case BOOLERR:
		{
			ReadSwapped(inStream, row);
			ReadSwapped(inStream, col);
			ReadSwapped(inStream, x);
			char b, f;
			Read(inStream, b);
			Read(inStream, f);
			
			c.v = row+1;
			c.h = col+1;
			
			if (f)
				switch (b)
				{
//					case 0: v = gRefNan; break;
					case 7: v = gDivNan; break;
					case 15: v = gValueNan; break;
					case 23: v = gRefNan; break;
					case 29: v = gRefNan; break;
//					case 36: v = gValueNan; break;
					case 42: v = gNANan; break;
					default: v = gErrorNan; break;
				}
			else
				v = (bool)b;
			
			fContainer->NewCell(c, v, NULL);
			if (GetFromList(lists+3, x, cf))
				fContainer->SetCellFormat(c, cf);
			break;
		}
		case STRING:		break;
		case ROW:		break;
		case INDEX:		break;
		case NAME:
		{
			
			break;
		}
		case B_1904:		break;
		case ARRAY:		break;
		case EXTERNNAME:		break;
		case DEFAULTROWHEIGHT:
		{
			short w, f;
			ReadSwapped(inStream, f);
			ReadSwapped(inStream, w);
			
			if (!(f & 0x01))
				fCellHeights.SetValue(1, kRowCount,
					round(w / 20.0));
			break;
		}
		case FONT:
		{
			short f;
			float fSize;
			ReadSwapped(inStream, x);
			fSize = x/20.0;
			ReadSwapped(inStream, f);

			Read(inStream, x);
			Read(inStream, x);
			Read(inStream, x);
			Read(inStream, x);
			Read(inStream, x);
			
			font_name fn;
			Read(inStream, fn);
			
			AppendToList(lists+2,
				gFontSizeTable->GetFormatID(fn, fSize));
			break;
		}
		case TABLE:		break;
		case WINDOW2:
		{
			short f;
			ReadSwapped(inStream, f);
			ReadSwapped(inStream, row);
			ReadSwapped(inStream, col);
			
			fPosition.v = row+1;
			fPosition.h = col+1;
			
			fShowGrid = (f & 0x02) != 0;
			if ((f & 0x04) != 0)
			{
				fBorderWidth = fBorderHeight = 0;
				BRect b(Bounds());
				fCellGrid->MoveTo(0.0, 0.0);
				fCellGrid->ResizeTo(b.Width(), b.Height());
			}
			break;
		}
		case LEFTMARGIN:		break;
		case RIGHTMARGIN:		break;
		case RK:
		{
			long l;
			
			ReadSwapped(inStream, row);
			ReadSwapped(inStream, col);
			ReadSwapped(inStream, x);
			ReadSwapped(inStream, l);
			
			if (l & 0x02)
				d = (double)(l>>2);
			else
			{
				*((long *)&d  ) = l&0xFFFFFFFC;
				*((long *)&d+1) = 0;
			}
			if (l & 0x01)
				d /= 100;
			
			v = d;
			
			c.v = row+1;
			c.h = col+1;
			
			fContainer->NewCell(c, v, NULL);
			if (GetFromList(lists+3, x, cf))
				fContainer->SetCellFormat(c, cf);
			break;
		}
		case TOPMARGIN:		break;
		case BOTTOMMARGIN:	break;
		case STYLE:			break;
		case PRINTHEADERS:
			ReadSwapped(inStream, x);
			fPrBorders = (x == 1);
			break;
		case PRINTGRIDLINES:
			ReadSwapped(inStream, x);
			fPrGrid = (x == 1);
			break;
		case FILEPASS:		break;
		case CONTINUE:		break;
//		case WINDOW1:		break;
		case BACKUP:		break;
		case FORMULA:
		{
			char *p;
			short cce, grbit;
			long l[2], chn;
			
			CFormula form;
			
			ReadSwapped(inStream, row);
			ReadSwapped(inStream, col);
			ReadSwapped(inStream, x);
			ReadSwapped(inStream, l[1]);
			ReadSwapped(inStream, l[0]);
			ReadSwapped(inStream, grbit);
			ReadSwapped(inStream, chn);
			ReadSwapped(inStream, cce);
			
			c.h = col+1;
			c.v = row+1;
			
			p = (char *)malloc(cce);
			inStream->Read(p, cce);
			try
			{
				if (grbit & 0x08)
				{
					XLFormula *xl;
					xl = new XLFormula;
					xl->row = row;
					xl->col = col;
					xl->x = x;
					xl->rwFirst = read_16_swap((short *)(p+1));
					xl->colFirst = read_16_swap((short *)(p+3));
					lists[0].AddItem(xl);
				}
				else
				{
					form.ParseXLString(c, shar, p, cce);
					fContainer->NewCell(c, v, form.DetachString());
				}
			}
			catch(CErr e)
			{
				e.DoError();
			}
			free(p);
			if (GetFromList(lists+3, x, cf))
				fContainer->SetCellFormat(c, cf);
			break;
		}
		case PANE:			break;
		case FORMAT:
		{	
			char s[256];
			ReadSwapped(inStream, x);
			ReadString(inStream, s);
			lists[4].AddItem(new FormatSpec(x, s));
			break;
		}
		case CODEPAGE:		break;
		case PLS:			break;
		case DCON:			break;
		case DCONREF:		break;
		case DCONNAME:		break;
		case DEFCOLWIDTH:
		{
			short w;
			ReadSwapped(inStream, w);
			
			fCellWidths.SetValue(1, kColCount,
				round(w * StringWidth("x")));
			break;
		}
		case XCT:			break;
		case CRN:			break;
		case FILESHARING:	break;
		case WRITEACCESS:	break;
		case OBJ:			break;
		case UNCALCED:		break;
		case SAVERECALC:	break;
		case TEMPLATE:		break;
		case OBJPROTECT:	break;
		case COLINFO:
		{
			short first, last, wi, f;
			float w;
			
			ReadSwapped(inStream, first);
			ReadSwapped(inStream, last);
			ReadSwapped(inStream, wi);
			ReadSwapped(inStream, x);
			ReadSwapped(inStream, f);
			
			if (f & 0x11)
				fCellWidths.SetValue(first+1,last+1,0);
			else
			{
				w = round(StringWidth("x") * wi / 256);
				fCellWidths.SetValue(first+1,last+1,w);
			}
			break;
		}
		case IMDATA:		break;
		case GUTS:			break;
		case WSBOOL:		break;
		case HCENTER:		break;
		case VCENTER:		break;
		case BOUNDSHEET:	break;
		case WRITEPROT:		break;
		case ADDIN:			break;
		case EDG:			break;
		case PUB:			break;
		case COUNTRY:		break;
		case HIDEOBJ:		break;
		case SORT:			break;
		case SUB:			break;
		case PALETTE:		break;
		case LHRECORD:		break;
		case LHNGRAPH:		break;
		case SOUND:			break;
		case LPR:			break;
		case STANDARDWIDTH:	break;
		case FNGROUPNAME:	break;
		case FILTERMODE:	break;
		case FNGROUPCOUNT:	break;
		case AUTOFILTERINFO:break;
		case AUTOFILTER:	break;
		case SCL:			break;
		case SETUP:			break;
		case COORDLIST:		break;
		case GCW:			break;
		case SCENMAN:		break;
		case SCENARIO:		break;
		case SXVIEW:		break;
		case SXVD:			break;
		case SXVI:			break;
		case SXIVD:			break;
		case SXLI:			break;
		case SXPI:			break;
		case DOCROUTE:		break;
		case RECIPNAME:		break;
		case MULRK:
		{
			long l;
			
			ReadSwapped(inStream, row);
			ReadSwapped(inStream, col);
			
			c.v = row+1;
			c.h = col+1;
			
			int i = (len-10)/6;

			while (i--)
			{
				ReadSwapped(inStream, x);
				ReadSwapped(inStream, l);
				
				if (l & 0x02)
					d = (double)(l>>2);
				else
				{
					*((long *)&d  ) = l&0xFFFFFFFC;
					*((long *)&d+1) = 0;
				}
				if (l & 0x01)
					d /= 100;
				
				v = d;
				fContainer->NewCell(c, v, NULL);
				if (GetFromList(lists+3, x, cf))
					fContainer->SetCellFormat(c, cf);
				c.h++;
			}
			break;
		}
		case MULBLANK:
		{
			ReadSwapped(inStream, row);
			ReadSwapped(inStream, col);
			
			c.v = row+1;
			c.h = col+1;
			
			int i = (len-10)/2;

			while (i--)
			{
				ReadSwapped(inStream, x);
				fContainer->NewCell(c, v, NULL);
				if (GetFromList(lists+3, x, cf))
					fContainer->SetCellFormat(c, cf);
				c.h++;
			}
			break;
		}
		case MMS:			break;
		case ADDMENU:		break;
		case DELMENU:		break;
		case SXDI:			break;
		case SXSTRING:		break;
		case SXTBL:			break;
		case SXTBRGIITM:	break;
		case SXTBPG:		break;
		case OBPROJ:		break;
		case SXIDSTM:		break;
		case DBCELL:		break;
		case BOOKBOOL:		break;
		case SXEXT:			break;
		case SCENPROTECT:	break;
		case OLESIZE:		break;
		case UDDESC:		break;
		case XF:
		{
			short f1, f2;
			CellFormat cf;
			
			ReadSwapped(inStream, f1);

			ReadSwapped(inStream, f2);
			FormatSpec *fc;
			
			if (f2 >= 0 && f2 < 5)
			{
				switch (f2)
				{
// gokje...
					case 1:
						cf.cfFormat = eFixed;
						cf.cfDigits = 0;
						break;

					case 2:
						cf.cfFormat = eFixed;
						cf.cfDigits = 2;
						break;

					case 3:
						cf.cfFormat = eFixed;
						cf.cfDigits = 2;
						cf.cfCommas = true;
						break;

					case 4:
						cf.cfFormat = eCurrency;
						cf.cfDigits = 2;
						cf.cfCommas = true;
						break;
						
					case 0:	// general
					default:
						break;
				}
			}
			else
			{
				for (int i=0;i<lists[4].CountItems();i++)
				{
					fc = (FormatSpec*)lists[4].ItemAt(i);
					if (fc->id == f2)
					{
						cf = fc->cf;
						break;					
					}
				}
			}

			if (f1 >= 4) f1++;
			if (GetFromList(&lists[2], f1, f1))
				cf.cfFontSize = f1;
			
			Read(inStream, x);
			ReadSwapped(inStream, f1);
			switch (f1 & 0x07)
			{
				case 1: cf.cfAlignment = eAlignLeft; break;
				case 2: cf.cfAlignment = eAlignCenter; break;
				case 3: cf.cfAlignment = eAlignRight; break;
				default: cf.cfAlignment = eAlignGeneral; break;
			}
			
			AppendToList(lists+3, cf);
			break;
		}
		case INTERFACEHDR:	break;
		case INTERFACEEND:	break;
		case SXVS:			break;
		case TABIDCONF:		break;
		default:			break;
	}
} // ProcessXLRecord

//void ProcessSharedFormulas(BList *lists)
//{
//	int i, j;
//	CellFormat cf;
//	
//	for (i=0;i<lists[1].CountItems();i++)
//	{
//		XLSHFormula *sf = (XLSHFormula *)lists[1].ItemAt(i);
//		
//		for (j=0;j<lists[0].CountItems();j++)
//		{
//			XLFormula *f = (XLFormula*)lists[0].ItemAt(j);
//			if (f->rwFirst == sf->rwFirst &&
//				f->colFirst == sf->colFirst)
//			{
//				cell c, s;
//				Value v;
//				CFormula form;
//				
//				c.v = f->row+1;
//				c.h = f->col+1;
//				
//				s.v = f->rwFirst+1;
//				s.h = f->colFirst+1;
//				
//				try
//				{
//					form.ParseXLString(c, s, sf->p, sf->cce);
//					fContainer->NewCell(c, v, form.DetachString());
//					if (GetFromList(lists+3, f->x, cf))
//						fContainer->SetCellFormat(c, cf);
//				}
//				catch(CErr e)
//				{
//					e.DoError();
//				}
//			}
//		}
//	}
//} // ProcessSharedFormulas

int CollectSharedFormulas(DStream& inStream)
{
	short code, len, bofrec=0;
	ssize_t nextOffset;
	int err = B_NO_ERROR;
	
	nextOffset = inStream->Position();

	do
	{
		inStream->Seek(nextOffset);
		ReadSwapped(inStream, code);
		ReadSwapped(inStream, len);
		
		if (code == 0x0809)
		{
			short vers;
			ReadSwapped(inStream, vers);
			ReadSwapped(inStream, bofrec);
			
			if (vers != 0x0500)
				err = B_ERROR;
		}
		else if (code == B_EOF)
		{
			if (bofrec == 0x0010 && gotACell)
				break;
		}
		else if ((bofrec == 0x0010 || bofrec == 0x0005) &&
			code == SHRFMLA)
		{
			XLSHFormula *shxl;
			shxl = (XLSHFormula *)malloc(sizeof(XLSHFormula));
			ReadSwapped(inStream, shxl->rwFirst);
			ReadSwapped(inStream, shxl->rwLast);
			Read(inStream, shxl->colFirst);
			Read(inStream, shxl->colLast);
			ReadSwapped(inStream, x);
			ReadSwapped(inStream, shxl->cce);
			
			shxl->p = (char *)malloc(shxl->cce);
			inStream->Read(shxl->p, shxl->cce);
			
			gSharedFormulas.AddItem(shxl);
		}

		nextOffset += 4 + len;
	}
	while (!err && inStream->Position() < inStream->Size());
	
	return err;
} // CollectSharedFormulas

int ConvertExcelFile(DStream& inStream, DStream& outStream)
{
	if (!LocateBof(inStream)) return B_ERROR;

	int err = B_NO_ERROR;
	bool gotACell = false;

	short code, len, bofrec=0;
	ssize_t nextOffset;
	
	nextOffset = inStream->Position();
	err = CollectSharedFormulas(inStream);
	if (err) return err;

	do
	{
		inStream->Seek(nextOffset);
		ReadSwapped(inStream, code);
		ReadSwapped(inStream, len);
		
		if (code == 0x0809)
		{
			short vers;
			ReadSwapped(inStream, vers);
			ReadSwapped(inStream, bofrec);
			
			if (vers != 0x0500)
				err = B_ERROR;
		}
		else if (code == B_EOF)
		{
			if (bofrec == 0x0010 && gotACell)
				break;
		}
		else if (bofrec == 0x0010 || bofrec == 0x0005)
			switch (code)
			{
				case LABEL:
				case RK:
				case RSTRING:
				case MULRK:
				case MULBLANK:
				case BLANK:
				case FORMULA:
					gotACell = true;
				default:
					ProcessXLRecord(code, len, inStream, lists);
			}

		nextOffset += 4 + len;
	}
	while (!err && inStream->Position() < inStream->Size());
	
	return err;
} // ReadXLFile
