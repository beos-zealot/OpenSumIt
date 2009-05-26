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
	MyError.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.
	
	Bevat macros die handig zijn bij error checking
*/

#ifndef MYERROR_H
#define MYERROR_H

#ifndef INCLUDE_ERRMSG
#define INCLUDE_ERRMSG 1
#endif

#if INCLUDE_ERRMSG
#include "errmsg.h"
#endif

#include <errno.h>

class CErr {
protected:
	CErr();
public:
	CErr(int err, ...) throw();
	CErr(const char *msg, ...) throw();

	virtual void	DoError() throw();
	
	void SetMessage(const char *newMessage);
	
	operator int()	{ return fErr; };
	operator char*() { return fMessage; };
	
protected:
	long fErr;
	char fMessage[256];
};

#if DEBUG
#  define THROW(args)	{ \
						fprintf(stderr, "### Sum-It Error\n# %s\n#----\nFile \"%s\"; Line %d;\n#----\n", #args, __FILE__, __LINE__); \
						throw CErr args; \
					}
#  define CATCHED		printf("catched at %s %d\n", __FILE__, __LINE__)
#else
#  define THROW(args)	throw CErr args;
#  define CATCHED
#endif

#define FailNil(p)	{ if (!(p)) THROW((errInsufficientMemory)); }
#define FailOSErr(e,m)	{ long err = (e); if (err < B_NO_ERROR) THROW((m,err));}
#define CHECKWRITE(stream,data,size) \
	if ((stream).Write((data),(size)) < (int)(size)) THROW((errIOWrite, errno))

//#define FailIOErr(e,m)	{ long err = (e); if (err < B_NO_ERROR) THROW((m/*,err*/));}

#endif
