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
	CellIterator.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

class CInterface;

class CCellIterator {
	friend class CInterface;
public:
	CCellIterator(CContainer *inContainer, range *range = NULL);
					
	bool Next(cell& c);
	bool NextExisting(cell& c);
	
	bool GoTo(cell c);
	void Reset();
	
private:
	cellmap& fCellData;
	range fRange;
	bool fBeforeStart;
};

//#pragma export on
class _EXPORT CInterface {
public:
	CInterface(CContainer *container, range *range);
	virtual ~CInterface();
	
	bool Next();
	
	bool GetDouble(double& d);
	bool GetText(char *s, int maxLen);
	bool GetTime(time_t& t);
	bool GetBool(bool& b);
	
private:
	CCellIterator *fIterator;
	cell fCurrent;
};

bool GetCellBool(void *cookie, cell c, bool *b);
bool GetCellText(void *cookie, cell c, char *s);
bool GetCellDouble(void *cookie, cell c, double *d);
bool GetCellTime(void *cookie, cell c, time_t *t);

//#pragma export reset
