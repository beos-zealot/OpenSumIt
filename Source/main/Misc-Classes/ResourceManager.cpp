/**********************************************************
 * ResourceManager.cpp
 * Copyright (c) 2000
 * Tim Vernum, All Rights Reserved
 **********************************************************
 * This code may be freely used for any purpose.
 * This code is distributed WITHOUT WARRANTY
 **********************************************************/

#include "ResourceManager.h"

ResourceManager gResourceManager ;
	
const void * ResourceManager:: FindHandle( const void * h )
{
	type_code type ;
	int32 id ;
	size_t s ;
	const char * name ;
	if( resources.GetResourceInfo( h, &type, &id, &s, &name ) )
		return h ;
	else
		return NULL ;
}

size_t ResourceManager:: HandleSize( const void * h )
{
	type_code type ;
	int32 id ;
	size_t s ;
	const char * name ;
	if( resources.GetResourceInfo( h, &type, &id, &s, &name ) )
		return s ;
	else
		return 0 ;
}

const void * ResourceManager:: LoadResource( type_code type , int32 id )
{
	size_t s ;
	return resources.LoadResource( type, id, &s ) ; 
}

const void * ResourceManager:: LoadResource( type_code type , const char * name )
{
	size_t s ;
	return resources.LoadResource( type, name, &s ) ; 
}

