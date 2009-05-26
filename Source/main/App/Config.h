#ifndef CONFIG_H
#define CONFIG_H

/*
	Config.h
	
	Copyright 1997, Hekkelman Programmatuur
	Copyright 2000, Tim Vernum
	
	Part of Sum-It for BeOS
*/

/*** Revision History
 ***
 *** TPV (2000-Feb-06) Created to remove need for sum-it.headers.pch++
 ***/

#ifndef DEBUG
#define DEBUG 1
#endif

#ifndef TRACKMEM
#define TRACKMEM 0
#endif

#ifndef BUILDING_APP
#define BUILDING_APP 1
#endif

#ifndef NULL
#define NULL 0
#endif

#include <BeBuild.h>
class _EXPORT CGraphic;
class _EXPORT CInterface;
class _EXPORT CGraphPlugIn;

#if TRACKMEM
#	define MALLOC(s)		Malloc((s), __FILE__, __LINE__)
#	define CALLOC(c,s)		Calloc((c), (s), __FILE__, __LINE__)
#	define REALLOC(p,s)	Realloc((p), (s), __FILE__, __LINE__)
#	define STRDUP(s)		Strdup((s), __FILE__, __LINE__)
#	define FREE(p)		Free(p)
#	include "MallocWrapper.h"
#else
#	define MALLOC		malloc
#	define CALLOC		calloc
#	define REALLOC		realloc
#	define STRDUP		strdup
#	define FREE			free
#endif

#include <cstring>

//#define min(a,b) ((a)<(b)?(a):(b))
//#define max(a,b) ((a)>(b)?(a):(b))

#endif
