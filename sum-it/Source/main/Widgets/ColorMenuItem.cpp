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
	ColorMenuItem.c

	Copyright 1997, Hekkelman Programmatuur

	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef   COLORMENUITEM_H
#include "ColorMenuItem.h"
#endif

#include <cstring>

CColorMenuItem::CColorMenuItem(
	const char	*label,
	rgb_color	color,
	BMessage 	*message,
	bool		useColor,
	char 		shortcut,
	uint32 		modifiers)
		: BMenuItem(label, message, shortcut, modifiers)
{
	mColor = color;
	mUseColor = useColor;

	char s[32];
	strcpy(s, "     ");
	strcat(s, Label());
	SetLabel(s);

	BMessage *msg = new BMessage(*message);
	msg->AddData("color", B_RGB_COLOR_TYPE, &mColor, sizeof(rgb_color));
	SetMessage(msg);
	return ;
}

bool CColorMenuItem::IsColorEqual(rgb_color	color)
{
	return ( (color.red == mColor.red) &&
			 (color.green == mColor.green) &&
			 (color.blue == mColor.blue) &&
			 (color.alpha == mColor.alpha) );
}

void CColorMenuItem::DrawContent()
{
	if (mUseColor)
	{
		rgb_color saveColor;

		BRect r;

		r = Frame();

		r.left += 13;
		r.right = r.left + 10;
		r.top += (r.Height() - 6.0) / 2;
		r.bottom = r.top + 6;

		BMenuItem::DrawContent();

		Menu()->StrokeRect(r);
		r.InsetBy(1, 1);

		saveColor = Menu()->HighColor();
		Menu()->SetHighColor(mColor);

		Menu()->FillRect(r);

		Menu()->SetHighColor(saveColor);
	}
	else
		BMenuItem::DrawContent();
}
