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
#include "Formula.h"
#include "Value.h"
#include "parser.h"
#include "MyError.h"
#include "Utils.h"
#include "CellParser.h"
#include "FunctionUtils.h"
#include "Container.h"
#include "CellView.h"
#include "StringTable.h"

CParser::CParser(CContainer *inContainer,
	char inListSep, char inDecSep, char inDateSep, char inTimeSep)
	: mArgCnt(0)
	, mLookahead(0)
	, mRelop(0)
	, mOffset(0)
	, mIsFormula(false)
	, mExpr(NULL)
	, mExprStart(NULL)
	, mTokenStart(NULL)
	, mContainer(inContainer)
	, mNum(0.0)
	, mTime(0)
	, mBool(false)
	, mListSep(inListSep)
	, mDecSep(inDecSep)
	, mDateSep(inDateSep)
	, mTimeSep(inTimeSep)
{
	memset( mToken, 0, sizeof(mToken) ) ;
} // CParser::CParser

CParser::~CParser()
{
	mFormula.Clear();
} // CParser::~CParser

void CParser::Match(int token)
{
	//	fprintf( stderr, "CParser::Match(%d) /%d\n", token, mLookahead ) ;
	if (mLookahead == token)
	{
		if (token != TEXT && token != END && token != IDENT)
			mIsFormula = true;
		mTokenStart = mExpr;
		
		bool mayBeTime =
			token == '('|| token == LIST;
		
		if( mLookahead != END )
			mLookahead = GetNextToken(mayBeTime);
	}
	else
	{
		char t[256];
		
		switch (token)
		{
			case END:		getindstring(t, 6, errMMEnd); break;
			case ')':		getindstring(t, 6, errMMClosingParen); break;
			case CELL:		getindstring(t, 6, errMMCell); break;
			case NUMBER:	getindstring(t, 6, errMMNumber); break;
			case LIST:		getindstring(t, 6, errMMList); break;
			default: 		sprintf(t, "%c", mLookahead); break;
		}
		
		if (mLookahead == END)
			getindstring(mToken, 6, errMMEnd);

		throw CParseErr(mTokenStart-mExprStart, strlen(mToken), errExpected, t, mToken);
	}
} // CParser::Match

bool CParser::Parse(const char *inString, cell inLocation)
{

	//	fprintf( stderr, "CParser::Parse( \"%s\", [ %hd , %hd ] )\n",
	//		inString , inLocation.h , inLocation.v ) ;
	
	mLoc = inLocation;
	mExpr = mExprStart = inString;
	mOffset = 0;
	
	bool result = true;

	try
	{
		mTokenStart = mExpr;
		mLookahead = GetNextToken(true);
		
		if (mLookahead == RELOP && mRelop == opEQ)
		{
			Match(RELOP);
			mIsFormula = true;
		}
		else
			mIsFormula = false;
	
		RelExpr();
		Match(END);
	
		AddToken(opEnd);
	}
	catch (CErr& e)
	{
		if (mIsFormula)
			throw;
		
		result = false;
	}
	
	return result;
} // CParser::Parse

int CParser::ScanFirstToken(const char *inExpr, const char **outPos, cell inLocation)
{
	mLoc = inLocation;
	mExpr = mExprStart = inExpr;
	mOffset = 0;
	
	int result = GetNextToken(true);
	
	*outPos = mExpr;
	
	return result;
} // CParser::ScanFirstToken

void CParser::RelExpr()
{
	BoolExpr();
	while (true)
	{
		switch (mLookahead)
		{
			case '&': Match('&'); BoolExpr(); AddToken(opAND); break;
			case '|': Match('|'); BoolExpr(); AddToken(opOR); break;
			default: return;
		}
	}
} // CParser::RelExpr

void CParser::BoolExpr()
{
	if (mLookahead == '!')
	{
		Match('!');
		BoolExpr();
		AddToken(opNOT);
	}
	else
	{
		Expr();
		if (mLookahead == RELOP)
		{
			int savedRelop = mRelop;
			Match(RELOP);
			Expr();
			AddToken(savedRelop);
		}
	}
} // CParser::BoolExpr

void CParser::Expr()
{
	Term();
	while (true)
	{
		switch (mLookahead)
		{
			case '-': Match('-'); Term(); AddToken(opMinus); break;
			case '+': Match('+'); Term(); AddToken(opPlus); break;
			default: return;
		}
	}
} // CParser::Expr

void CParser::Term()
{
	Term2();
	while (true)
	{
		switch (mLookahead)
		{
			case '/': Match('/'); Term2(); AddToken(opDiv); break;
			case '*': Match('*'); Term2(); AddToken(opMul); break;
			default: return;
		}
	}
} // CParser::Term

void CParser::Term2()
{
	Factor();
	while (true)
	{
		switch (mLookahead)
		{
			case RAISE: Match(RAISE); Factor(); AddToken(opRaise); break;
			case '^': Match('^'); Factor(); AddToken(opRaise); break;
			default: return;
		}
	}
} // CParser::Term2

void CParser::Factor()
{
	switch (mLookahead)
	{
		case '-':
			Match('-');
			Factor();
			AddToken(opNegate);
			break;

		case NUMBER:
		{
			double d = mNum;
			Match(NUMBER);
			if (mLookahead == '%')
			{
				Match('%');
				AddToken(valPerc, &d);
			}
			else
				AddToken(valNum, &d);
			break;
		}
		
		case '(':
			Match('(');
			RelExpr();
			Match(')');
			AddToken(opParen);
			break;
		
		case CELL:
		{
			range r;
			
			r.TopLeft() = mCell;
			
			Match(CELL);
			
			mIsFormula = true;
			
			if (mLookahead == RANGE)
			{
				Match(RANGE);
				r.BotRight() = mCell;
				Match(CELL);
				AddToken(valRange, &r);
			}
			else
				AddToken(valCell, &r.TopLeft());
			break;
		}
		
		case IDENT:
		{
			char name[256];
			strcpy(name, mToken);
			int s = mTokenStart - mExprStart;

			FuncCallData fcd;
			fcd.funcNr = GetFunctionNr(name);
			int expectedArgs = gFuncArrayByNr[fcd.funcNr].argCnt;

			Match(IDENT);

			if (mLookahead == '(')
			{
				Match('(');
				ParamList();
				Match(')');
			
				if (fcd.funcNr < 0)
					throw CParseErr(s, strlen(name), errUnknownFunction, name);
				
				if (expectedArgs == -1 || expectedArgs == mArgCnt ||
					(expectedArgs < 0 || expectedArgs <= -fcd.funcNr))
				{
					fcd.argCnt = mArgCnt;
				}
				else if (expectedArgs < 0)
					throw CParseErr(s, strlen(name), errIncorrectNrOfArgs2,
							expectedArgs, mArgCnt);
				else
					throw CParseErr(s, strlen(name), errIncorrectNrOfArgs,
							expectedArgs, mArgCnt);
				
				AddToken(opFunc, &fcd);
			}
			else
			{
				if (fcd.funcNr >= 0)
				{
					if (expectedArgs != -1 && expectedArgs != 0)
						throw CParseErr(s, strlen(name), errIncorrectNrOfArgs,
							expectedArgs, 0);
				
					fcd.argCnt = 0;

					AddToken(opFunc, &fcd);
				}
				else if (mContainer &&
						 mContainer->GetOwner() &&
						 mContainer->GetOwner()->IsNamedRange(name))
				{
					AddToken(valName, name);
				}
				else
					throw CParseErr(s, strlen(name), errUnknownIdentifier, name);
			}
			break;
		}
		
		case TEXT:
			AddToken(valStr, mToken);
			Match(TEXT);
			break;
		
		case TIME:
			AddToken(valTime, &mTime);
			Match(TIME);
			break;
		
		case BOOL:
			AddToken(valBool, &mBool);
			Match(BOOL);
			break;
		
		default:
			Match(NUMBER);
	}
} // CParser::Factor

void CParser::ParamList()
{
	int args = 0;
	
	mIsFormula = true;
	
	while (mLookahead != ')')
	{
		if (mLookahead != LIST)
			RelExpr();
		else
			AddToken(valNil);
		
		if (mLookahead != ')')
			Match(LIST);
		
		args++;
	}
	
	mArgCnt = args;
} // CParser::ParamList

