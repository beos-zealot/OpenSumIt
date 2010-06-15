#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

/**********************************************************
 * ResourceManager.h
 * Copyright (c) 2000
 * Tim Vernum, All Rights Reserved
 **********************************************************
 * This code may be freely used for any purpose.
 * This code is distributed WITHOUT WARRANTY
 **********************************************************/

/*
 * This is just a wrapper for a BResources object.
 * It adds the follow features
 * 	+ InitCheck()
 *  + Set from a path/entry
 *  + Shortcuts for (some) standard Resource types 
 */

#include <Resources.h>
#include <File.h>
#include <Path.h>

class ResourceManager
{
	public:
		ResourceManager( void ) ;
		ResourceManager( const BPath * ) ;
		ResourceManager( const entry_ref * ) ;
		ResourceManager( const BEntry * ) ;

		status_t SetTo( const BPath * ) ;
		status_t SetTo( const entry_ref * ) ;
		status_t SetTo( const BEntry * ) ;

		status_t InitCheck( void ) ;
		
		const void * LoadResource( type_code, int32 ) ;
		const void * LoadResource( type_code, const char * ) ;

		const void * LoadCursor(int32) ;
		const void * LoadCursor(const char *) ;

		const void * LoadMenuBar(int32) ;
		const void * LoadMenuBar(const char *) ;

		const void * LoadMenu(int32) ;
		const void * LoadMenu(const char *) ;
		
		const void * LoadMiniIcon(int32) ;
		const void * LoadMiniIcon(const char *) ;

		const void * LoadDialog(int32) ;
		const void * LoadDialog(const char *) ;
		
		const void * FindHandle( const void * ) ;
		size_t HandleSize( const void * ) ;
	
	private:
		status_t SetTo( BFile * ) ;
		BResources resources ;
		status_t init_check ;
} ;

inline ResourceManager:: ResourceManager( void )
: init_check( B_NO_INIT )
{
}

inline ResourceManager:: ResourceManager( const BPath * path )
: init_check( B_NO_INIT )
{
	SetTo(path) ;
}

inline ResourceManager:: ResourceManager( const entry_ref * ref )
: init_check( B_NO_INIT )
{
	SetTo( ref ) ;
}

inline ResourceManager::ResourceManager( const BEntry * ent )
: init_check( B_NO_INIT )
{
	SetTo( ent );
}

inline status_t ResourceManager::SetTo( const BPath * path )
{
	BFile file( path->Path(), B_READ_ONLY ) ;
	return SetTo( &file ) ;
}

inline status_t ResourceManager::SetTo( const entry_ref * ref )
{
	BFile file(ref, B_READ_ONLY ) ;
	return SetTo( &file ) ;
}
	
inline status_t ResourceManager::SetTo( const BEntry * ent )
{
	BFile file(ent, B_READ_ONLY ) ;
	return SetTo( &file ) ;
}

inline status_t ResourceManager:: SetTo( BFile * file )
{
	if((init_check = file->InitCheck()) == B_OK)
		init_check = resources.SetTo( file ) ;
	return init_check ;
}

inline status_t ResourceManager:: InitCheck( void )
{
	return init_check ;
}
		
inline const void * ResourceManager:: LoadCursor(int32 id)
{
	return LoadResource( 'CURS', id ) ;
}

inline const void * ResourceManager:: LoadCursor(const char * name)
{
	return LoadResource( 'CURS', name ) ;
}

inline const void * ResourceManager:: LoadMenuBar(int32 id) 
{
	return LoadResource( 'MBAR', id ) ;
}
inline const void * ResourceManager:: LoadMenuBar( const char * name )
{
	return LoadResource( 'MBAR', name ) ;
}

inline const void * ResourceManager:: LoadMenu( int32 id )
{
	return LoadResource( 'MENU', id ) ;
}
inline const void * ResourceManager:: LoadMenu( const char * name ) 
{
	return LoadResource( 'MENU', name ) ;
}

inline const void * ResourceManager:: LoadMiniIcon( int32 id )
{
	return LoadResource( 'MICN', id ) ;
}
inline const void * ResourceManager:: LoadMiniIcon( const char * name ) 
{
	return LoadResource( 'MICN', name ) ;
}

inline const void * ResourceManager:: LoadDialog( int32 id )
{
	return LoadResource( 'DLOG', id ) ;
}
inline const void * ResourceManager:: LoadDialog( const char * name ) 
{
	return LoadResource( 'DLOG', name ) ;
}

extern ResourceManager gResourceManager ;

#endif
