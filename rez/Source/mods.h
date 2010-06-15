#ifndef MODS_H
#define MODS_H

/*************************************************************************
 * Modifications to the standard REZ distribution.
 * Copyright (c) 2000, Tim Vernum
 * This code may be freely used for any purpose
 *************************************************************************/

extern bool gListDepend ;
extern bool gParseOnly ;

extern void StartDepend( const char * out ) ;
extern void DependFile( const char * file ) ;
extern void EndDepend( void ) ;

#endif

