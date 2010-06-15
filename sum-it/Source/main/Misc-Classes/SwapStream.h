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
/*	$Id: SwapStream.h,v 1.1.1.1 2000/03/05 06:41:30 tpv Exp $
	
	Copyright Hekkelman Programmatuur
	Maarten L. Hekkelman
	
	Created: 11/15/98 21:21:27
*/

/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-06) Added Headers Guards
 ***/

#ifndef SWAPSTREAM_H
#define SWAPSTREAM_H

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#include <SupportDefs.h>
#include <DataIO.h>
#include <ByteOrder.h>

class CSwapStream
{
  public:
	CSwapStream(BPositionIO& s) : s(s) {}
	
	template <class T>
	CSwapStream& operator >> (T& t)
	{
		if (s.Read(&t, sizeof(T)) != sizeof(T))
			THROW((errIORead, errno));
		return *this;
	}
	
//	template <>
	CSwapStream& operator >> (short& t)
	{
		if (s.Read(&t, sizeof(t)) != sizeof(t))
			THROW((errIORead, errno));
		t = B_BENDIAN_TO_HOST_INT16(t);
		return *this;
	}
	
//	template <>
	CSwapStream& operator >> (unsigned short& t)
	{
		if (s.Read(&t, sizeof(t)) != sizeof(t))
			THROW((errIORead, errno));
		t = B_BENDIAN_TO_HOST_INT16(t);
		return *this;
	}
	
//	template <>
	CSwapStream& operator >> (long& t)
	{
		if (s.Read(&t, sizeof(t)) != sizeof(t))
			THROW((errIORead, errno));
		t = B_BENDIAN_TO_HOST_INT32(t);
		return *this;
	}
	
//	template <>
	CSwapStream& operator >> (unsigned long& t)
	{
		if (s.Read(&t, sizeof(t)) != sizeof(t))
			THROW((errIORead, errno));
		t = B_BENDIAN_TO_HOST_INT32(t);
		return *this;
	}

//	template <>
	CSwapStream& operator >> (float& t)
	{
		if (s.Read(&t, sizeof(t)) != sizeof(t))
			THROW((errIORead, errno));
		t = B_BENDIAN_TO_HOST_FLOAT(t);
		return *this;
	}

//	template <>
	CSwapStream& operator >> (double& t)
	{
		if (s.Read(&t, sizeof(t)) != sizeof(t))
			THROW((errIORead, errno));
		t = B_BENDIAN_TO_HOST_DOUBLE(t);
		return *this;
	}
	
	CSwapStream& operator >> (char *t)
	{
		char l;
		*this >> l;
		if (s.Read(t, l) != l)
			THROW((errIORead, errno));
		t[l] = 0;
		return *this;
	}
	
	// write

	template <class T>
	CSwapStream& operator << (T t)
	{
		if (s.Write(&t, sizeof(t)) != sizeof(t))
			THROW((errIOWrite, errno));
		return *this;
	}
	
//	template <>
	CSwapStream& operator << (short t)
	{
		t = B_HOST_TO_BENDIAN_INT16(t);
		if (s.Write(&t, sizeof(t)) != sizeof(t))
			THROW((errIOWrite, errno));
		return *this;
	}
	
//	template <>
	CSwapStream& operator << (unsigned short t)
	{
		t = B_HOST_TO_BENDIAN_INT16(t);
		if (s.Write(&t, sizeof(t)) != sizeof(t))
			THROW((errIOWrite, errno));
		return *this;
	}
	
//	template <>
	CSwapStream& operator << (long t)
	{
		t = B_HOST_TO_BENDIAN_INT32(t);
		if (s.Write(&t, sizeof(t)) != sizeof(t))
			THROW((errIOWrite, errno));
		return *this;
	}
	
//	template <>
	CSwapStream& operator << (unsigned long t)
	{
		t = B_HOST_TO_BENDIAN_INT32(t);
		if (s.Write(&t, sizeof(t)) != sizeof(t))
			THROW((errIOWrite, errno));
		return *this;
	}

//	template <>
	CSwapStream& operator << (float t)
	{
		t = B_HOST_TO_BENDIAN_FLOAT(t);
		if (s.Write(&t, sizeof(t)) != sizeof(t))
			THROW((errIOWrite, errno));
		return *this;
	}

//	template <>
	CSwapStream& operator << (double t)
	{
		t = B_HOST_TO_BENDIAN_DOUBLE(t);
		if (s.Write(&t, sizeof(t)) != sizeof(t))
			THROW((errIOWrite, errno));
		return *this;
	}
	
	CSwapStream& operator << (char *t)
	{
		char l = strlen(t);
		*this << l;
		if (s.Write(t, l) != l)
			THROW((errIOWrite, errno));
		return *this;
	}
	
	BPositionIO& s;
};


#endif
