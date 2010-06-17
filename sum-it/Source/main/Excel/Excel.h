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
	Excel filter
*/

#include "MyError.h"
#include "FileFormat.h"
#include "Formula.h"
#include "Functions.h"
//#include <ByteOrder.h>
#include <vector>

class CCellView;
class CContainer;
class CFormula;

struct XLSHFormula
{
	short rwFirst, rwLast;
	char colFirst, colLast;
	short reserved;
	short cce;
	char *p;
};

enum
{
	xnFunction,
	xnNamedReference,
	xnOther
};

struct xlName
{
	int type;
	union
	{
		int funcNr;
		char *namedRef;
	};
	
	xlName() : type(xnOther) {}
	xlName(int funcNr) : type(xnFunction), funcNr(funcNr) {}
	xlName(const char *name) : type(xnNamedReference), namedRef(strdup(name)) {}
};

class CExcel5Filter
{
  public:
	CExcel5Filter(BPositionIO& inStream, CCellView *cellView, CContainer *container);
	virtual ~CExcel5Filter();
	
	void Translate();

  private:
	
	status_t GetBookStream(BPositionIO& stream) throw();
	bool LocateBof(BPositionIO& stream);
	
	class CExcelStream
	{
	  public:
		CExcelStream(BPositionIO& s) : fStream(s) {}
		
		void Read (void *buffer, int size)
		{
			if (fStream.Read(buffer, size) != size)
				THROW((errIORead));
		}
		
		CExcelStream& operator>> (char& x)
		{
			Read(&x, sizeof(x));
			return *this;
		}
		
		CExcelStream& operator>> (unsigned char& x)
		{
			Read(&x, sizeof(x));
			return *this;
		}
		
		CExcelStream& operator>> (short& x)
		{
			Read(&x, sizeof(x));
			x = B_LENDIAN_TO_HOST_INT16(x);
			return *this;
		}
		
		CExcelStream& operator>> (unsigned short& x)
		{
			Read(&x, sizeof(x));
			x = B_LENDIAN_TO_HOST_INT16(x);
			return *this;
		}
		
		CExcelStream& operator>> (long& x)
		{
			Read(&x, sizeof(x));
			x = B_LENDIAN_TO_HOST_INT32(x);
			return *this;
		}
		
		CExcelStream& operator>> (unsigned long& x)
		{
			Read(&x, sizeof(x));
			x = B_LENDIAN_TO_HOST_INT32(x);
			return *this;
		}
		
		CExcelStream& operator>> (float& x)
		{
			Read(&x, sizeof(x));
			x = B_LENDIAN_TO_HOST_FLOAT(x);
			return *this;
		}
		
		CExcelStream& operator>> (double& x)
		{
			Read(&x, sizeof(x));
			x = B_LENDIAN_TO_HOST_DOUBLE(x);
			return *this;
		}
		
		CExcelStream& operator >> (unsigned char *p)
		{
			Read(p, sizeof(*p));
			Read(p + 1, *p);
			return *this;
		}
		
	  private:
		BPositionIO& fStream;
	};

	void Pass1();
	void Pass2();
	void HandleXLRecordForPass1(int code, int len);
	void Selection();
	void Name();
	void Font();
	void Xf();
	
	void HandleXLRecordForPass2(int code, int len);
	void ParseXLFormula(CFormula& formula, cell loc, cell shared,
		const void *data, int len);
	
	bool f1904;
	int fNewFunctionNr;
	BMallocIO fBook;
	std::vector<xlName> fNames;
	std::vector<int> fFonts, fStyles, fFormats;
	std::vector<XLSHFormula> fSharedFormulas;
	
	CCellView *fCellView;
	CContainer *fContainer;
};

