#ifndef KEYFILTER_H
#define KEYFILTER_H

/*************************************************
 * KeyFilter.h
 * Copyright (c) 2000, Tim Vernum
 * May be freely used for any purpose
 *************************************************/

#include <MessageFilter.h>
#include <AppDefs.h>

class KeyFilter : public BMessageFilter
{
	public:
		enum	{
			MSG_FilteredKeyDown = '#KYD' ,
		} ;
		
		KeyFilter( uint32 key, uint32 msg_code = MSG_FilteredKeyDown )
		:	BMessageFilter( B_KEY_DOWN )
		,	fKey(key)
		,	fMsgId(msg_code)
		{
		}
		
		filter_result Filter( BMessage * msg, BHandler ** ) ;
 
	private:
		uint32 fKey ;
		uint32 fMsgId ; 
} ;

#endif

