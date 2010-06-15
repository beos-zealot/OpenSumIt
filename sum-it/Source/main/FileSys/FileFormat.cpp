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
	FileFormat.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include "FileFormat.h"
#include "MyError.h"

CSwapStream& operator>>(CSwapStream& stream, scChunk& v)
{
	stream.s.Read(&v, sizeof(v));
	
	v.type = ntohs(v.type);
	v.size = ntohs(v.size);
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scVersion& v)
{
	stream.s.Read(&v, sizeof(v));
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scHeader& v)
{
	stream.s.Read(&v, sizeof(v));
	
	v.defaultFormat = ntohs(v.defaultFormat);
	v.flags = ntohl(v.flags);
	v.functionCount = ntohs(v.functionCount);
	v.cellCount = ntohl(v.cellCount);
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scView& v)
{
	stream.s.Read(&v, sizeof(v));
	
	swap_order(v.windowRect);
	swap_order(v.position);
	swap_order(v.frozen);
	swap_order(v.curCell);
	swap_order(v.selection);
	swap_order(v.flags);
	swap_order(v.headingFont);
	swap_order(v.fill);
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scPrintInfo& v)
{
	stream.s.Read(&v, sizeof(v));
	
	swap_order(v.page);
	swap_order(v.margins);
	swap_order(v.flags);

	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scCSElement& v)
{
	stream.s.Read(&v, sizeof(v));

	swap_order(v.index);
	swap_order(v.style);
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scColStyles& v)
{
	stream.s.Read(&v, sizeof(v));
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scName& v)
{
	stream.s.Read(&v, sizeof(v));
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scFunc& v)
{
	stream.s.Read(&v, sizeof(v));
	
	swap_order(v.funcNr);
	swap_order(v.argCnt);
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scFont& v)
{
	stream.s.Read(&v, sizeof(v));
	
	swap_order(v.size);
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scFormat& v)
{
	stream.s.Read(&v, sizeof(v));
	
	swap_order(v.nr);
	swap_order(v.info[0]);
	swap_order(v.info[1]);
	swap_order(v.info[2]);
	swap_order(v.info[3]);
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scStyle& v)
{
	stream.s.Read(&v, sizeof(v));
	
	swap_order(v.font);
	swap_order(v.format);
	swap_order(v.flags);
//	v.lowColor.alpha = 255 - v.lowColor.alpha;

	if (	(v.lowColor.alpha == 0) && 
			((v.lowColor.red != 0) || (v.lowColor.green != 0) || (v.lowColor.blue != 0))
		) {
		v.lowColor.alpha = 255;
#if DEBUG
		printf("Modif\n");
		printf("%d %d %d\n",v.lowColor.red,v.lowColor.green,v.lowColor.blue);
#endif
	}

	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scCell& v)
{
	stream.s.Read(&v, kscCellSize);
	
	swap_order(v.loc);
	swap_order(v.style);
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scString& v)
{
	stream.s.Read(&v, sizeof(v));
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scFormula& v)
{
	stream.s.Read(&v, sizeof(v));
	
	return stream;
} /* operator>> */

CSwapStream& operator>>(CSwapStream& stream, scChart& v)
{
	stream.s.Read(&v, 44);
	
	swap_order(v.anchor);
	swap_order(v.frame);
	swap_order(v.area);
	
	return stream;
} /* operator>> */
