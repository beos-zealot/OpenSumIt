#include "KeyFilter.h"

#include <Message.h>

filter_result KeyFilter::Filter( BMessage * msg, BHandler ** )
{
	uint32 key ;
	if( msg->FindInt32( "raw_char", (int32*)&key ) == B_OK )
	{
		if( key == fKey )
			msg->what = fMsgId ;
	}
	return B_DISPATCH_MESSAGE;
}
