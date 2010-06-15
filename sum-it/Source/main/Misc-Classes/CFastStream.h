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

#include "MyError.h"

class CFastStream : public BDataIO
{
  public:
	CFastStream (const char *data, size_t size)
		: mData(data), mSize(size)
	{
		mCur = mData;
		mEnd = mData + mSize;
	}
	
	ssize_t Read (void *p, size_t s)
	{
		register char *P = (char *)p;
		
		if (mCur + s > mEnd)
			THROW((errIORead));
		
		while (s--)
			*P++ = *mCur++;
		
		return s;
	}

	ssize_t Write(const void *buffer, size_t size)
	{
		ASSERT(false);
		return 0;
	}
	
	void Seek (off_t offset, uint32 seek_mode)
	{
		switch (seek_mode)
		{
			case SEEK_SET:
				mCur = mData + offset;
				break;
			case SEEK_CUR:
				mCur += offset;
				break;
			case SEEK_END:
				mCur = mEnd + offset;
				break;
		}
	}
	
	bool AtEnd() const
		{ return mCur >= mEnd; }
	
	void ReadBlock (void *p, size_t s)
	{
		memcpy(p, mCur, s);
		mCur += s;
	}
	
  private:
	const char *mData, *mCur, *mEnd;
	size_t mSize;
};

template<class T>
inline CFastStream& operator >> (CFastStream& stream, T& t)
{
	stream.Read(&t, sizeof(T));
	return stream;
} // operator >>

template<>
inline CFastStream& operator>> (CFastStream& stream, short& t)
{
	stream.Read(&t, sizeof(short));
	t = ntohs(t);
	return stream;
} // operator >> <short>

template<>
inline CFastStream& operator>> (CFastStream& stream, unsigned short& t)
{
	stream.Read(&t, sizeof(short));
	t = ntohs(t);
	return stream;
} // operator >> <short>

template<>
inline CFastStream& operator>> (CFastStream& stream, long& t)
{
	stream.Read(&t, sizeof(long));
	t = ntohl(t);
	return stream;
} // operator >> <short>

template<>
inline CFastStream& operator>> (CFastStream& stream, unsigned long& t)
{
	stream.Read(&t, sizeof(long));
	t = ntohs(t);
	return stream;
} // operator >> <short>

template<>
inline CFastStream& operator>> (CFastStream& stream, double& t)
{
	stream.Read(&t, sizeof(double));
	t = B_BENDIAN_TO_HOST_DOUBLE(t);
	return stream;
} // operator >> <short>

inline CFastStream& operator >> (CFastStream& stream, char *s)
{
	unsigned char l;
	stream >> l;
	stream.ReadBlock (s, l);
	s[l] = 0;
	return stream;
} // operator >> 

inline void ReadCString(CFastStream& inStream, int inMaxLen, char *outString)
{
	char c;
	
	do	inStream >> c;
	while ((*outString++ = c) != 0 && --inMaxLen);
} // ReadCString
