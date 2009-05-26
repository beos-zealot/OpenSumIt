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
	Container.styles.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#include <support/Debug.h>

#include "Cell.h"
#include "Formula.h"
#include "Value.h"
#include "Formula.h"
#include "CellData.h"
#include "Container.h"
#include "CellView.h"
#include "MyError.h"
#include "Formatter.h"
#include "Utils.h"
#include "StringTable.h"
#include "CellIterator.h"
#include "CellStyle.h"
#include "FontMetrics.h"

#if DEBUG
void WarnForUnlockedContainer(int lineNr);
#define CHECKLOCK	if (!fWriteLocker.IsLocked() && fInView) WarnForUnlockedContainer(__LINE__);

//THROW((errContainerNotLocked));
#else
#define CHECKLOCK	
#endif

void CContainer::GetCellStyle(const cell& inLoc, CellStyle& outStyle)
{	//CHECKLOCK
	outStyle = gStyleTable[GetCellStyleNr(inLoc)];
} /* GetCellStyle */

void CContainer::SetCellStyle(const cell& inLoc, CellStyle& inStyle)
{	CHECKLOCK
	SetCellStyleNr(inLoc, gStyleTable.GetStyleID(inStyle));
} /* SetCellStyle */

int CContainer::GetCellStyleNr(const cell& inLoc)
{	//CHECKLOCK
	cellmap::iterator i;
	
	if ((i = fCellData.find(inLoc)) != fCellData.end())
		return (*i).second.mStyle;
	else
		return GetColumnStyleNr(inLoc.h);
} /* GetCellStyleNr */

void CContainer::SetCellStyleNr(const cell& inLoc, int inStyle)
{	CHECKLOCK
	cellmap::iterator i;
	
	if ((i = fCellData.find(inLoc)) == fCellData.end())
	{
		if (inStyle != fDefaultCellStyle)
		{
			Value v;
			NewCell(inLoc, v, NULL);
			fCellData[inLoc].mStyle = inStyle;
		}
		else
			return;
	}	
	else
		(*i).second.mStyle = inStyle;
} /* SetCellStyleNr */

void CContainer::GetColumnStyle(int colNr, CellStyle& outStyle)
{	//CHECKLOCK
	outStyle = gStyleTable[GetColumnStyleNr(colNr)];
} /* GetColumnStyle */

void CContainer::SetColumnStyle(int colNr, CellStyle& inStyle)
{	CHECKLOCK
	SetColumnStyleNr(colNr, gStyleTable.GetStyleID(inStyle));
} /* SetColumnStyle */

int CContainer::GetColumnStyleNr(int colNr)
{	//CHECKLOCK
	if (fColumnStyles[colNr] >= 0)
		return fColumnStyles[colNr];
	else
		return fDefaultCellStyle;
} /* GetColumnStyleNr */

void CContainer::SetColumnStyleNr(int colNr, int inStyle)
{	CHECKLOCK
	fColumnStyles.SetValue(colNr, inStyle);
} /* SetColumnStyleNr */

void CContainer::GetDefaultCellStyle(CellStyle& outStyle)
{
	outStyle = gStyleTable[fDefaultCellStyle];
} /* GetDefaultCellStyle */

void CContainer::SetDefaultCellStyle(CellStyle& inStyle)
{
	fDefaultCellStyle = gStyleTable.GetStyleID(inStyle);
} /* CContainer::SetDefaultCellStyle */

int CContainer::CollectStyles(int *styleList)
{
	int result = 0;
	cellmap::iterator i;
	
	for (i = fCellData.begin(); i != fCellData.end(); i++)
	{
		int styleNr = (*i).second.mStyle;
		bool isNew = true;
		
		for (int i = 0; i < result && isNew; i++)
			if (styleNr == styleList[i])
			{
				isNew = false;
				break;
			}
		
		if (isNew)
			styleList[result++] = styleNr;
	}
	
	for (int i = 1; i < kColCount; i++)
	{
		int styleNr = fColumnStyles[i];
		bool isNew = true;

		if (styleNr < 0) continue;
		
		for (int i = 0; i < result && isNew; i++)
			if (styleNr == styleList[i])
			{
				isNew = false;
				break;
			}
			
		if (isNew)
			styleList[result++] = styleNr;
	}
	
	return result;
} // CContainer::CollectStyles

