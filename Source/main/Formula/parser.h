#include "Formula.h"

enum {
	IDENT = 258, CELL, TEXT, NUMBER, TIME, RELOP, END, RANGE, LIST, BOOL, RAISE, UMINUS, UNOT
};

class CParser
{
  public:
	CParser(CContainer *inContainer, char inListSep, char inDecSep, char inDateSep, char inTimeSep);
	~CParser();
	
	bool Parse(const char *expr, cell inLocation);
	int ScanFirstToken(const char *inExpr, const char **outPos, cell inLocation);
	
	operator double ()
		{ return mNum; }
	operator bool ()
		{ return mBool; }
	operator time_t ()
		{ return mTime; }
	
	const CFormula& Formula() const
		{ return mFormula; }
	
  private:
	
	// lexical analyser
	int GetNextToken(bool acceptTime);
	void Restart(int& start, int& state);
  
	// parser
	void RelExpr();
	void BoolExpr();
	void Expr();
	void Term();
	void Term2();
	void Factor();
	void ParamList();

	void Match(int expected);
	
	void AddToken(int token, void *data = NULL)
		{ mFormula.AddToken((PFToken)token, data, mOffset); }
	
	int 			mArgCnt;
	int				mLookahead;
	int 			mRelop;
	int 			mOffset;
	
	bool			mIsFormula;
	const char		*mExpr, *mExprStart, *mTokenStart;
	char			mToken[256];
	cell 			mLoc;
	CContainer 		*mContainer;
	double 			mNum;
	cell 			mCell;
	time_t 			mTime;
	bool			mBool;
	char 			mListSep, mDecSep, mDateSep, mTimeSep;
	CFormula		mFormula;
};

