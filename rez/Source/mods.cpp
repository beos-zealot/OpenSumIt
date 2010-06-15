#include "mods.h"
#include <cstdio>

/*************************************************************************
 * Modifications to the standard REZ distribution.
 * Copyright (c) 2000, Tim Vernum
 * This code may be freely used for any purpose
 *************************************************************************/

bool gListDepend = false ;
bool gParseOnly = false ;

void DependFile( const char * file )
{
	if( gListDepend )
	{
		printf( "\t%s \\\n", file ) ;
	}
}

void StartDepend( const char * out )
{
	if( gListDepend )
	{
		printf( "%s : \\\n", out ) ;
	}
}

void EndDepend( void )
{
	if( gListDepend )
	{
		printf( "\n" ) ;
	}
}

