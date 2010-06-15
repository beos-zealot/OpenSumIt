#ifndef STATICSTRING_H
#define STATICSTRING_H

/*******************************************************
 * (c) 1999, Tim Vernum <tpv@users.sourceforge.net>
 * All rights reserved
 *******************************************************
 * File   : StaticString.h
 * Author : Tim Vernum
 * Purpose: BStringView that will GetPreferredSize() properly
 *******************************************************/

#include <StringView.h>

class StaticString : public BStringView
{
	typedef BStringView inherited ;
	public:
		StaticString( BRect , const char * n, const char * str,
					  uint32 = B_FOLLOW_LEFT  | B_FOLLOW_TOP ,
					  uint32 = B_WILL_DRAW ) ;
		
		void GetPreferredSize( float * ,float * ) ;
		void SetViewColor( rgb_color ) ;
} ;

#endif
