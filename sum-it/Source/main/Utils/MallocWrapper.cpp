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
	Copyright 1997, Hekkelman Programmatuur

*/

#ifndef   MALLOCWRAPPER_H
#include "MallocWrapper.h"
#endif

#ifndef   MYERROR_H
#include "MyError.h"
#endif

#ifndef   RESOURCEMANAGER_H
#include "ResourceManager.h"
#endif

#include <Debug.h>
#include <cstring>

static sem_id gWrapperSem = create_sem(1, "wrapper");

typedef struct MyPtr {
	MyPtr *next;
	long size;
	const char *creator;
	int lineNr;
} MyPtr;

static MyPtr gFirst = { NULL, 0, "first", 0 }, gLastDisposed;

#if TRACKMEM
void CheckFailed(mcheck_status status)
{
	PrintUndisposed();

	puts("last disposed:");
	MyPtr *p = &gLastDisposed;
	printf("%p size %08x, allocated at %s line %d\n",
		p + sizeof(MyPtr), (int)p->size, p->creator, (int)p->lineNr);

} /* CheckFailed */
#endif

static void Append(MyPtr *ptr)
{
	if (acquire_sem(gWrapperSem) == B_NO_ERROR)
	{
		MyPtr *next = &gFirst;
		
		while (next->next)
			next = next->next;
		
		next->next = ptr;
		ptr->next = NULL;
		release_sem(gWrapperSem);
	}
}

static void Remove(MyPtr *ptr)
{
	if (acquire_sem(gWrapperSem) == B_NO_ERROR)
	{
		MyPtr *next = &gFirst;
		
		while (next->next != ptr)
		{
			if (!next->next)
			{
				printf("Heap Corrupt : Cannot find %s / %d\n", ptr->creator, ptr->lineNr);
				PrintUndisposed();
				debugger( "Corrupt Heap" ) ;
				THROW((errHeapIsCorrupt));
			}
			ASSERT(next->next);
			next = next->next;
		}
		
		if (next)
			next->next = ptr->next;
		release_sem(gWrapperSem);
	}
}

void *Malloc(size_t size, const char *caller, int lineNr)
{
	size_t newSize;
	MyPtr *result;
	
	newSize = size + sizeof(MyPtr);
	
	result = (MyPtr *)malloc(newSize);
	ASSERT(result);
	
	result->size = size;
	result->creator = caller;
	result->lineNr = lineNr;
	result->next = NULL;
	Append(result);
	return (char *)result + sizeof(MyPtr);
}

void *Calloc(int count, size_t size, const char *caller, int lineNr)
{
	void *p = Malloc(count * size, caller, lineNr);
	memset(p, 0, count * size);
	return p;
}

void *Realloc(void *oldPtr, size_t size, const char *caller, int lineNr)
{
	size_t newSize = size + sizeof(MyPtr);
	MyPtr *p = (MyPtr *)((char *)oldPtr - sizeof(MyPtr)), *result;
	
	if (gResourceManager.FindHandle(oldPtr))
	{
		fprintf( stderr, "ERROR: Realloc on Resource\n" ) ;
		return oldPtr ;
//		return realloc(oldPtr, size);
	}
	else
	{
		Remove(p);
		result = (MyPtr *)realloc(p, newSize);
		ASSERT(result);
		Append(result);
		
		return (char *)result + sizeof(MyPtr);
	}
}

char *Strdup(const char *s, const char *caller, int lineNr)
{
	char *p = (char *)Malloc(strlen(s) + 1, caller, lineNr);
	strcpy(p, s);
	return p;
}

void Free(void *p)
{
	if( !p )
		return ;

	MyPtr *mp = (MyPtr *)((char *)p - sizeof(MyPtr));
	gLastDisposed = *mp;

	Remove(mp);
	
	free(mp);
}

void PrintUndisposed()
{
	MyPtr *p = gFirst.next;
	
	while (p)
	{
		printf("%p size %8.8d, allocated at %s line %d\n",
			p + sizeof(MyPtr), (int)p->size, p->creator, p->lineNr);
		p = p->next;
	}
}
