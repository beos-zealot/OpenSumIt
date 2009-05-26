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
	MyError.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 *** TPV (2000-Feb-10) Bug fixed for Seg Fault
 ***/

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   MALERT_H
#include "MAlert.h"
#endif

#include <stdarg.h>
#include <support/Debug.h>
#include <cstring>

CErr::CErr()
{
} /* CErr::CErr */

CErr::CErr(int err, ...) throw ()
{
	fErr = err;
	fMessage[0] = 0;
	
	const char *r;
	
	if (err == 0)
		strcpy(fMessage, "No Error");
	else	if (err < 0)
		strcpy(fMessage, strerror(err));
	else
	{
		r = GetIndString(6, err);
		// fprintf( stderr, "Err String (%d) = %s\n", err, r ) ;
		if (!r)
			snprintf(fMessage, sizeof(fMessage), "A strange and unknown error occured (nr: %d)", err);
		else
		{
			char msg[2048];
		
			va_list vl;
			va_start(vl, err);
			vsprintf(msg, r, vl);
			va_end(vl);
			
			if (strlen(msg) > 255)
			{
				strncpy(fMessage, msg, 255);
				fMessage[255] = 0;
			}
			else
				strcpy(fMessage, msg);
		}
	}
//#if DEBUG
//	puts(fMessage);
//	DEBUGGER((fMessage));
//#endif
} /* CErr::CErr */

CErr::CErr(const char *errString, ...) throw()
{
	va_list vl;
	va_start(vl, errString);
	vsprintf(fMessage, errString, vl);
	va_end(vl);
	fErr = B_ERROR;
//#if DEBUG
//	puts(fMessage);
//	DEBUGGER((fMessage));
//#endif
} // CErr::CErr

void CErr::DoError() throw ()
{
	if (fErr)
	{
		MAlert *a = new MStopAlert(fMessage ? fMessage : "A Terrible error occured");
		a->Go();
	}
} /* CErr::DoError */

void CErr::SetMessage(const char *newMessage)
{
	strcpy(fMessage, newMessage);
} /* CErr::SetMessage */
