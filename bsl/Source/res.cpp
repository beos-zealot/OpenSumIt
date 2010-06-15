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
	Resourcewriting routine
*/

#include <Be.h>
#include "res.h"
#include <Path.h>

void WriteResource(const char *file, int trunc, int type, int id,
	const void *data, size_t size, const char *name)
{
	BDirectory d(".");
	BFile resFile;
	BEntry e;
	
	if (trunc)
	{
		if (d.CreateFile(file, &resFile) != B_NO_ERROR)
		{
			perror("error creating file");
			exit(1);
		}
	}
	else
	{
		if (d.FindEntry(file, &e) || resFile.SetTo(&e, O_RDWR))
		{
			perror("error");
			exit(1);
		}
	}
	
	BResources res;
	
	if (res.SetTo(&resFile, trunc) != B_NO_ERROR)
	{
		perror("error in set to");
		exit(1);
	}
	
	if (res.HasResource(type, id))
		res.RemoveResource(type, id);

	if (res.AddResource(type, id, data, size, name) != B_NO_ERROR)
	{
		perror("Error adding resource");
		exit(1);
	}
} /* WriteResource */
