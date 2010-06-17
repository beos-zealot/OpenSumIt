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
	lexer.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

// Lexical analyser

#include "CellView.h"
#include "Container.h"
#include "Formula.h"
#include <ctype.h>
#include "parser.h"
#include "MyError.h"
#include "Utils.h"
#include "Formatter.h"
#include "NameTable.h"
#include "Globals.h"

#define UNDEFINED		-1
#define MAXTOKENLENGTH	255

void CParser::Restart(int& start, int& state)
{
	switch(start)
	{
		case 0:		start = 300;	break;
		case 300:	start = 9;		break;
		case 9:		start = 200;	break;
		case 200:	start = 250;	break;
		case 250:	start = 12;		break;
		case 12:	start = 20;		break;
		case 20:	start = 25;		break;
		case 25:	start = 28;		break;
		case 28:	start = 100;	break;
	}
	state = start;
} /* CParser::Restart */

#define RESTART		{ Restart(start, state); \
					  mExpr = ss; \
					  *t = 0; \
					  te = t; \
					  continue; \
					 }
#define RETRACT		{ mExpr--, *--te = 0; }
#define GETNEXTCHAR	(ch = *te++ = *mExpr++, *te = 0, ch)

int CParser::GetNextToken(bool acceptTime)
{
	//	fprintf( stderr, "CParser::GetNextToken(%s)\n", acceptTime ? "true" : "false" ) ;
	
	char ch, buf[MAXTOKENLENGTH + 1], *t = buf, *te, *tm;
	const char *cp, *ss;
	int token;
	int state, start;

	int pm = 0, am = 0;
	int h =0, m = 0, s = 0;
	int a = 0, b = 0, c = 0;
	
	ss = mExpr;
	*t = 0;
	te = t;
	tm = t + MAXTOKENLENGTH;
	
	token = UNDEFINED;
	state = start = 0;

	while (token == UNDEFINED)
	{
		if (te >= tm)
			THROW((errTokenLength, token));

		//	fprintf( stderr, "state = %d ; ch = %c (%d) ; token = %d ; buf = %s ; pos= %d\n",
		//		state, ch, ch, token, buf, (te-buf) ) ;

		switch(state)
		{
			case 0:
				GETNEXTCHAR;
				if (ch == '\r' || ch == '\n' || ch == ' ' || ch == '\t')
				{
					ss++;
					te = t;
				}
				else if (ch == '<')
					state = 1;
				else if (ch == '=')
				{
					GETNEXTCHAR;
					if (ch != '=')
						RETRACT;
					token = RELOP;
					mRelop = opEQ;
				}
				else if (ch == '>')
					state = 2;
				else if (ch == '!')
					state = 3;
				else
					RESTART;
				break;

			case 1:
				GETNEXTCHAR;
				if (ch == '=')
				{
					token = RELOP;
					mRelop = opLE;
				}
				else if (ch == '>')
				{
					token = RELOP;
					mRelop = opNE;
				}
				else
				{
					RETRACT;
					mRelop = opLT;
					token = RELOP;
				}
				break;

			case 2:
				GETNEXTCHAR;
				if (ch == '=')
				{
					token = RELOP;
					mRelop = opGE;
				}
				else
				{
					RETRACT;
					mRelop = opGT;
					token = RELOP;
				}
				break;

			case 3:
				GETNEXTCHAR;
				if (ch == '=')
				{
					token = RELOP;
					mRelop = opNE;
				}
				else
					RESTART;
				break;
		
			case 300:
				if (GETNEXTCHAR == 'R')
					state = 301;
				else
					RESTART;
				break;
			
			case 301:
				if (GETNEXTCHAR == '[')
					state = 302;
				else if (ch == 'C')
				{
					mCell.v = 0;
					state = 305;
				}
				else
					RESTART;
				break;
			
			case 302:
				cp = mExpr;
				if (isdigit(GETNEXTCHAR) || ch == '-' || ch == '+')
					state = 303;
				else
					RESTART;
				break;
			
			case 303:
				if (isdigit(GETNEXTCHAR))
					state = 303;
				else if (ch == ']')
					state = 304;
				else
					RESTART;
				break;
			
			case 304:
				mCell.v = atoi(cp);
				if (GETNEXTCHAR != 'C')
					RESTART;
				state = 305;
				break;
			
			case 305:
				if (GETNEXTCHAR == '[')
					state = 306;
				else
				{
					RETRACT;
					mCell.h = 0;
					state = 309;
				}
				break;
			
			case 306:
				cp = mExpr;
				if (isdigit(GETNEXTCHAR) || ch == '-' || ch == '+')
					state = 307;
				else
					RESTART;
				break;
			
			case 307:
				if (isdigit(GETNEXTCHAR))
					state = 307;
				else if (ch == ']')
					state = 308;
				else
					RESTART;
				break;
			
			case 308:
				mCell.h = atoi(cp);
				state = 309;
				break;
			
			case 309:
				token = CELL;
				break;
			
			case 9:
				GETNEXTCHAR;
				if (isalpha(ch) || ch == '$' || ch == '_')
					state = 10;
				else
					RESTART;
				break;

			case 10:
				GETNEXTCHAR;
				if (isalnum(ch) || ch == '$' || ch == '_')
					state = 10;
				else
					state = 11;
				break;

			case 11:
				RETRACT;
				token = IDENT;
				break;

			case 200:
				GETNEXTCHAR;
				if (isdigit(ch) && acceptTime)
				{
					h = ch - '0';
					state = 201;
				}
				else
					RESTART;
				break;
			
			case 201:
				GETNEXTCHAR;
				if (isdigit(ch))
				{
					h = h * 10 + ch - '0';
					state = 202;
				}
				else if (ch == mTimeSep)
					state = 203;
				else
					RESTART;
				break;
			
			case 202:
				GETNEXTCHAR;
				if (ch == mTimeSep)
					state = 203;
				else
					RESTART;
				break;
			
			case 203:
				GETNEXTCHAR;
				if (isdigit(ch))
				{
					m = 10 * (ch - '0');
					state = 204;
				}
				else
					RESTART;
				break;
			
			case 204:
				GETNEXTCHAR;
				if (isdigit(ch))
				{
					m += ch - '0';
					state = 205;
				}
				else
					RESTART;
				break;
			
			case 205:
				GETNEXTCHAR;
				if (ch == mTimeSep)
					state = 206;
				else if (tolower(ch) == 'a')
					state = 211;
				else if (tolower(ch) == 'p')
					state = 209;
				else
					state = 213;
				break;
			
			case 206:
				GETNEXTCHAR;
				if (isdigit(ch))
				{
					s = 10 * (ch - '0');
					state = 207;
				}
				else
					RESTART;
				break;
			
			case 207:
				GETNEXTCHAR;
				if (isdigit(ch))
				{
					s += ch - '0';
					state = 208;
				}
				else
					RESTART;
				break;
			
			case 208:
				GETNEXTCHAR;
				if (tolower(ch) == 'p')
					state = 209;
				else if (tolower(ch) == 'a')
					state = 211;
				else
					state = 213;
				break;
			
			case 209:
				GETNEXTCHAR;
				if (tolower(ch) == 'm')
				{
					pm = 1;
					state = 214;
				}
				else
				{
					RETRACT;
					state = 213;
				}
				break;
				
			case 211:
				GETNEXTCHAR;
				if (tolower(ch) == 'm')
				{
					am = 1;
					state = 214;
				}
				else
				{
					RETRACT;
					state = 213;
				}
				break;
		
			case 213:
				RETRACT;
				state = 214;
				break;
			
			case 214:
				if (h == 12 && (am || !g24Hours))
					h = 0;
				mTime = ((h + 12 * pm) * 60 + m) * 60 + s;
				token = TIME;
				break;

			case 250:
				if (isdigit(GETNEXTCHAR) && acceptTime)
					state = 251;
				else
					RESTART;
				break;
			
			case 251:
				a = a * 10 + (ch - '0');
				if (isdigit(GETNEXTCHAR))
					state = 251;
				else if (ch == mDateSep)
					state = 252;
				else
					RESTART;
				break;
			
			case 252:
				if (isdigit(GETNEXTCHAR))
					state = 253;
				else
					RESTART;
				break;
			
			case 253:
				b = b * 10 + (ch - '0');
				if (isdigit(GETNEXTCHAR))
					state = 253;
				else if (ch == mDateSep)
					state = 254;
				else
				{
					c = -1;
					state = 256;
				}
				break;
			
			case 254:
				if (isdigit(GETNEXTCHAR))
					state = 255;
				else
					RESTART;
				break;
			
			case 255:
				c = c * 10 + (ch - '0');
				if (isdigit(GETNEXTCHAR))
					state = 255;
				else
					state = 256;
				break;
			
			case 256:
				RETRACT;
				int d, m, y;
				
				switch (gDateOrder)
				{
					case doDMY:	d = a;		m = b - 1;	y = c;		break;
					case doMDY:	m = a - 1;	d = b;		y = c;		break;
					case doDYM:	d = a;		y = b;		m = c - 1;	break;
					case doMYD:	m = a - 1;	y = b;		d = c;		break;
					case doYMD:	y = a;		m = b - 1;	d = c;		break;
					case doYDM:	y = a;		d = b;		m = c - 1;	break;
				}
				
				if (y < 0)
				{
					time_t t;
					time(&t);
					
					struct tm *lt = localtime(&t);
					y = lt->tm_year;
				}
				else if (y > 1900)
					y -= 1900;

				if (y < 70)
					y += 100;

				if (y > 200 || m > 11 || d > 31)
					RESTART;
				
				struct tm time, *lt;
				memset(&time, 0, sizeof(time));
				time.tm_mday = d;
				time.tm_mon = m;
				time.tm_year = y;
								
				mTime = mktime(&time);
				
				lt = localtime(&mTime);

#if __dest_os == __be_os
	// Warning! this is a hack for a bug in BeOS
				mTime -= lt->tm_sec;
#endif
				
				if (d != lt->tm_mday || m != lt->tm_mon || y != lt->tm_year)
					RESTART;
				
				token = TIME;
				break;
			
			case 12:
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 13;
				else
					RESTART;
				break;
			
			case 13:
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 13;
				else if (ch == mDecSep)
					state = 14;
				else if (toupper(ch) == 'E')
					state = 16;
				else
					RESTART;
				break;

			case 14:
				te[-1] = '.';
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 15;
				else
					RESTART;
				break;

			case 15:
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 15;
				else if (toupper(ch) == 'E')
					state = 16;
				else
					RESTART;
				break;

			case 16:
				GETNEXTCHAR;
				if (ch == '+' || ch == '-')
					state = 17;
				else if (isdigit(ch))
					state = 18;
				else
					RESTART;
				break;

			case 17:
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 18;
				else
					RESTART;
				break;

			case 18:
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 18;
				else
					state = 19;
				break;

			case 19:
				RETRACT;
				token = NUMBER;
				break;

			case 20:
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 21;
				else
					RESTART;
				break;

			case 21:
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 21;
				else if (ch == mDecSep)
					state = 22;
				else
					RESTART;
				break;

			case 22:
				te[-1] = '.';
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 23;
				else
					RESTART;
				break;

			case 23:
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 23;
				else
					state = 24;
				break;

			case 24:
				RETRACT;
				token = NUMBER;
				break;

			case 25:
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 26;
				else
					RESTART;
				break;

			case 26:
				GETNEXTCHAR;
				if (isdigit(ch))
					state = 26;
				else
					state = 27;
				break;

			case 27:
				RETRACT;
				token = NUMBER;
				break;

			case 28:
				GETNEXTCHAR;
				if (ch == '"')
					state = 29;
				else
					RESTART;
				break;

			case 29:
				GETNEXTCHAR;
				if (te >= tm)
					THROW((errTokenLength, t));
				if (ch == '"')
					state = 30;
				else if (ch == 0)
				{
					RETRACT;
					state = 30;
				}
				else
					state = 29;
				break;

			case 30:
				token = TEXT;
				if (te[-1] == '"')
					te[-1] = 0;

				strcpy(mToken, t + 1);
				break;

			case 100:
				GETNEXTCHAR;
					// fprintf( stderr, "100/%c(%d)\n", ch,ch ) ;
				switch (ch)
				{
					case 0:
						token = END;
						break;
						
					case ':':
						token = RANGE;
						break;
						
					case '.':
						if (GETNEXTCHAR == '.')
							token = RANGE;
						else
						{
							RETRACT;
							token = ch;
						}
						break;
					
					case '*':
						if (GETNEXTCHAR == '*')
							token = RAISE;
						else
						{
							RETRACT;
							token = '*';
						}
						break;
					
					default:
						token = ch == mListSep ? LIST : ch;
				}
				break;
		}
	}

	if (token == NUMBER)
	{
		mNum = atof(t);
		strcpy(mToken, t);
	}
	else if (token == IDENT)
	{
		cell c;
		strcpy(mToken, t);
		
		if (cell::GetFormulaCell(t, mLoc, c))
		{
			mCell = c;
			token = CELL;
		}
#if __BEOS__ || __HAIKU__
		else if (strcasecmp(t, gTrueString) == 0)
#else
		else if (::relstring(t, gTrueString, false, false) == 0)
#endif
		{
			mBool = true;
			token = BOOL;
		}
#if __BEOS__ || __HAIKU__
		else if (strcasecmp(t, gFalseString) == 0)
#else
		else if (::relstring(t, gFalseString, false, false) == 0)
#endif
		{
			mBool = false;
			token = BOOL;
		}
	}
	else if (token != TEXT)
		strcpy(mToken, buf);

	return token;
} /* yylex */
