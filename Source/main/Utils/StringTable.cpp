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
	StringTable.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   STRINGTABLE_H
#include "StringTable.h"
#endif

#ifndef   RESOURCEMANAGER_H
#include "ResourceManager.h"
#endif


const char* GetIdxString(const char* Buf,long size,long idx);

/*
	
	GetIdxString
	
	Sample Code, Cyril VELTER 1996 (cyril@micronet.fr).
	
	This function extract a string from a buffer of data
	in StrL format. size is the size of the buffer and idx
	is the position of the string in the list.
	
	buf is typicaly get by file.FindResource('StrL',XX,&len)
*/


const char* GetIdxString(const char* Buf,long size,long idx)
{
	// current position in the buffer
	long pnt=0;
	
	// scan buffer
	while(true)
	{
		// is there at least one string (3 bytes)
		if (size-pnt>2)
		{
			// decode string length 
			short sz;
			((char*)(&sz))[0]=Buf[pnt];
			((char*)(&sz))[1]=Buf[pnt+1];
			
			// is there enought data in buffer to extract string
			if (size-pnt>=2+sz)
			{
				
				// is it our index
				if (idx==0)
				{
//					// String memory allocation
//					char* str=(char*)(MALLOC(sz+1));
//					FailNil(str);
//					
//					// String extraction
//					memcpy(str,Buf+pnt+2,sz);
//					
//					// Fix 0 at the end of the string
//					str[sz]=0;
//					
//					// return the string
//					return str;
					return Buf + pnt + 2;
				}
				else
					idx--;
				pnt+=2+sz;
			}
			else
				return NULL;
		}
		else
			return NULL;
	}
	return NULL;
} /* GetIdxString */

const char* GetIndString(long resID, long indx)
{
	const char *h;
	
	h = reinterpret_cast<const char*>( gResourceManager.LoadResource('StrL', resID) );
	return h ? GetIdxString(h, gResourceManager.HandleSize(h), indx) : NULL;
} /* GetIndString */

const char *GetMessage(long indx)
{
	return GetIndString(9, indx);
} // GetMessage

const char *GetName(long indx)
{
	return GetIndString(10, indx);
} // GetName

void getindstring(char *s, long resID, long indx)
{
	const char *h , *t = NULL;
	
	h = (const char *)gResourceManager.LoadResource('StrL', resID);
	
	if (h)
		t = GetIdxString( h, gResourceManager.HandleSize(h), indx);

	if (t)
		strcpy(s, t);
	else
		*s = 0;
} // getindstring
