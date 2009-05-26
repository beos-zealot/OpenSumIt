#ifndef   STATICSTRING_H
#include "StaticString.h"
#endif

/*******************************************************
 * (c) 1999, Tim Vernum <tpv@users.sourceforge.net>
 * All rights reserved
 *******************************************************
 * File   : StaticString.cpp
 * Author : Tim Vernum
 * Purpose: BStringView that will GetPreferredSize() properly
 *******************************************************/

StaticString :: StaticString( BRect t , const char * n, const char * str,
					  uint32 rsz, uint32 flg )
:	inherited( t,n,str,rsz,flg )
{
}

void StaticString :: GetPreferredSize( float * w , float * h )
{
	font_height fh ;
	GetFontHeight( &fh ) ;
	*h = fh.ascent + fh.descent + fh.leading ;
	*w = StringWidth( Text() ) * 1.1 ;
}

void StaticString :: SetViewColor( rgb_color col )
{
	inherited::SetViewColor( col ) ;
	SetLowColor( col ) ;
	Invalidate() ;
}
