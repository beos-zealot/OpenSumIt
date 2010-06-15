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
	Formatter.number.c
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 *** TPV (2000-Feb-20) Added Headers Guards
 ***/

#include "CellView.h"
#include "Formatter.h"
#include "StringTable.h"
#include "FontMetrics.h"
#include "CellStyle.h"
#include "Utils.h"
#include "MyError.h"
#include "Round.h"

#include <support/Debug.h>
#include <locale.h>
#include <ctype.h>

extern char gCurrencySymbol[32];
extern bool gCurrencyBefore;
extern bool gCurrencyParens;

#define kMaxDigits 15

static const double bit_values[] =
{
	1.0E+001,
	1.0E+002,
	1.0E+004,
	1.0E+008,
	1.0E+016,
	1.0E+032,
	1.0E+064,
	1.0E+128,
	1.0E+256
};

#define max_bits	511		/* (2**n)-1 where n is the number of elements in bit_values */

static const double digit_values[] =
{
	1.0E+01,
	1.0E+02,
	1.0E+03,
	1.0E+04,
	1.0E+05,
	1.0E+06,
	1.0E+07,
	1.0E+08,
	1.0E+09,
	1.0E+10,
	1.0E+11,
	1.0E+12,
	1.0E+13,
	1.0E+14,
	1.0E+15,
	1.0E+16
};

/* globals */

static const double
	kMaxGeneral = 1.00E+15,
	kMinGeneral = 1.00E-15;

/* Implementation */

static double CrunchDouble(double x, int *exp10)
{
	int exp2;

	frexp(x, &exp2);
	*exp10 = (exp2 * 301029) / 1000000;
	
	{
		const double *	p = bit_values;
		int				n = *exp10;
		
		if (n < 0)
		{
			for (n = -n; n; p++, n >>= 1)
				if (n & 1)
					x *= *p;
		}
		else if (n > 0)
		{
			double f = 1.0;
			
			for (; n; p++, n >>= 1)
				if (n & 1)
					f *= *p;
			
			x /= f;
		}
	}

	while (fabs(x) >= 1.0)
	{
		x *= 0.1;
		++*exp10;
	}
	
	while (fabs(x) < 0.1)
	{
		x *= 10.0;
		--*exp10;
	}
	
	return x;
} /* CrunchDouble */

static int round_decimal(char *s, int& exp, int new_length)
{
	char		c;
	char *	p;
	int		carry, l;
	
	l = strlen(s);
	
	if (new_length < 0)
	{
return_zero:
		*s++ = '0';
		*s = 0;
		return 1;
	}
	
	if (new_length >= l)
		return l;
	
	p     = s + new_length + 1;
	c     = *--p - '0';
	carry = (c >= 5);
	
	while (new_length)
	{
		c = *--p - '0' + carry;
		
		if ((carry = (c > 9)) != 0)
			--new_length;
		else
		{
			*p++ = c + '0';
			*p = 0;
			break;
		}
	}
	
	if (carry)
	{
		exp += 1;
		*s++ = '1';
		*s = 0;
		return 1;
	}
	else if (new_length == 0)
		goto return_zero;
	
	return new_length;
} /* round_decimal */

static void num2dec(double x, int& exp10, char *s)
{
	char		*p = s;
	int		n, digits, i;
	
	digits = kMaxDigits + 1;
	
	if (x < 0)
		x = -x;

	while (digits)
	{
		long long_x;
		n = digits;
		
		if (n > 8)
			n = 8;
		
		digits -= n;
		
		p += n;
		
		x *= digit_values[n-1];

		long_x = (long)x;
		
		x -= (double) long_x;
		
		for (i = n + 1; --i;)
		{
			*--p = (long_x % 10) + '0';
			long_x /= 10;
		}
		
		p += n;
	}
	
	*p = 0;
	
	round_decimal(s, exp10, kMaxDigits);
} /* num2dec */

static void FormatFixed(char *d, int exp10, int digits,
	int decimals, bool commas, char *s)
{
	int n, l;

	if (digits == 1 && exp10 < 1)
		l = round_decimal(d, exp10, 0);
	else
		l = round_decimal(d, exp10, digits);

	if (exp10 < 1)
		n = decimals - abs(exp10);
	else
		n = digits;

	if (n < 0)
	{
		*s++ = '0';
		*s++ = gDecimalPoint;
		while (decimals--)
			*s++ = '0';
		*s = 0;
		return;
	}

	if (exp10 < 1)
	{
		*s++ = '0';
		*s++ = gDecimalPoint;
		digits--;
		for (int i = exp10; i < 0 && digits; i++)
		{
			*s++ = '0';
			digits--;
		}
	}
	
	char *p = s;
	
	while (digits > 0)
	{
		n = digits;
		
		if (n > exp10 && exp10 > 0)
			n = exp10;
		
		digits -= n;
		
		while (n--)
		{
			if (commas && exp10 > 0 && p != s && (exp10 % 3) == 0)
				*p++ = gThousandSeparator;
			exp10--;
			
			if (l-- > 0)
				*p++ = *d++;
			else
				*p++ = '0';
		}
		
		if (!exp10)
			*p++ = gDecimalPoint;
	}

	*p = 0;
} /* FormatFixed */

bool CFormatter::operator==(const CFormatter& inOther) const
{
	return fFormatID == fFormatID &&
		fDigits == inOther.fDigits &&
		fCommas == inOther.fCommas;
} // CFormatter::operator==

float CFormatter::FormatDouble(double inValue, char *outStr,
	int inFontNr, float inWidth)
{
	int vk, ak, digits, exp10, i;
	float sWid, digitWidth;
	bool sgn;
	char *p, dec[kMaxDigits + 3];
	double x;
	CFontMetrics fm;
	
	try
	{
		fm = gFontSizeTable[inFontNr];
	}
	catch (CErr& e)
	{
		CATCHED;
		ASSERT(false);
		fm = gFontSizeTable[0];
	}

	*outStr = 0;
	sWid = -1;

	digitWidth = fm['0'];
	
	if ((__HI(inValue) & 0x7FF00000) == 0x7FF00000)
	{
		int nanNr = (__HI(inValue) >> 5) & 0x000000FF;
		
		getindstring(outStr, kNanStrings, nanNr);

		if (strlen(outStr) == 0)
			getindstring(outStr, kNanStrings, 1);
	}
	else if (inValue == 0.0 && (fFormatID == eGeneral || fFormatID == eScientific))
	{
		strcpy(outStr, (digitWidth <= inWidth ? "0" : ""));
		sWid = fm[*outStr];
	}
	else if ((fabs(inValue) > kMaxGeneral ||
			  fabs(inValue) < kMinGeneral) &&
			 fFormatID == eGeneral)
	{
		CFormatter nf(eScientific);
		sWid = nf.FormatDouble(inValue, outStr, inFontNr, inWidth);
	}	
	else switch (fFormatID)
	{
		case eGeneral:
			sgn = (inValue < 0.0);
		
			x = CrunchDouble(inValue, &exp10);
			num2dec(x, exp10, dec);
			
			vk = max(1, exp10);
			ak = max(0, kMaxDigits - vk);
			
			sWid = (sgn ? fm['-'] : 0.0);
			sWid += vk * digitWidth;
			sWid += fm[gDecimalPoint];
			if (fCommas)
				sWid += ((vk - 1) / 3) * fm[gThousandSeparator];
				
			ak = min((int)((inWidth - sWid) / digitWidth), ak);
			
			if (sWid <= inWidth)
			{
				if (x < 0)
				{
					*outStr = '-';
					FormatFixed(dec, exp10, vk + ak, ak, fCommas, outStr + 1);
				}
				else
					FormatFixed(dec, exp10, vk + ak, ak, fCommas, outStr);
				
				p = outStr + strlen(outStr);
				
				while (p[-1] == '0' && p > outStr)
				{
					*--p = 0;
				}
	
				if (p[-1] == gDecimalPoint)
					*--p = 0;

				sWid = fm.StringWidth(outStr);
			}

			if (*outStr == 0 || sWid > inWidth)
			{
				CFormatter nf(eScientific);
				sWid = nf.FormatDouble(inValue, outStr, inFontNr, inWidth);
			}
			break;
			
		case eScientific:
			x = CrunchDouble(inValue, &exp10);
			num2dec(x, exp10, dec);
			exp10--;
			
			if (x < 0)
			{
				sWid = fm['-'];
				sgn = true;
			}
			else
			{
				sWid = 0.0;
				sgn = false;
			}
			
			sWid += fm[gDecimalPoint];

			if (exp10 < 1)
				sWid += fm['-'];
			
			int n;
			
			if (abs(exp10) < 10)
			{
				sWid += 2 * digitWidth;
				n = 1;
			}
			else if (abs(exp10) < 100)
			{
				sWid += 3 * digitWidth;
				n = 2;
			}
			else if (abs(exp10) < 1000)
			{
				sWid += 4 * digitWidth;
				n = 3;
			}
			else
			{
				sWid += 5 * digitWidth;
				n = 4;
			}
			
			digits = min((int)((inWidth - sWid) / digitWidth), kMaxDigits);
			digits = round_decimal(dec, exp10, digits);

			p = outStr;
			
			if (sgn)
				*p++ = '-';
			
			*p++ = dec[0];
			digits--;
			
			if (digits > 1)
			{
				*p++ = gDecimalPoint;
				
				ak = 1;
				while (digits--)
					*p++ = dec[ak++];
				
				while (p[-1] == '0' && p > outStr)
					*--p = 0;
				
				if (*p == gDecimalPoint)
					*--p = 0;
			}
			
			*p++ = 'e';
			
			if (exp10 < 0)
			{
				*p++ = '-';
				exp10 = -exp10;
			}
			
			p += n;
			*p = 0;
			
			for (int j = n + 1; --j;)
			{
				*--p = (exp10 % 10) + '0';
				exp10 /= 10;
			}

			sWid = -1;
			break;

		case eFixed:
		case ePercent:
			if (inValue != 0)
				x = CrunchDouble(inValue, &exp10);
			else
				x = exp10 = 0;
			num2dec(x, exp10, dec);
			
			if (fFormatID == ePercent)
				exp10 += 2;
			
			if (exp10 < 1)
				digits = fDigits + 1;
			else
				digits = fDigits + exp10;

			if (digits > 100)		// keep it decent please
				sWid = inWidth + 1;
			else
			{
				p = outStr;
	
				if (inValue < 0)
					*p++ = '-';
	
				FormatFixed(dec, exp10, digits, fDigits, fCommas, p);
				
				if (fDigits == 0 &&
					outStr[strlen(outStr) - 1] == gDecimalPoint)
					outStr[strlen(outStr) - 1] = 0;
				
				if (fFormatID == ePercent)
					strcat(outStr, "%");
			}
			break;

		case eCurrency:
			if (inValue != 0)
				x = CrunchDouble(inValue, &exp10);
			else
				x = exp10 = 0;
			num2dec(x, exp10, dec);
			
			if (exp10 < 1)
				digits = fDigits + 1;
			else
				digits = fDigits + exp10;

			p = outStr;

			if (inValue < 0)
			{
				if (gCurrencyParens)
					*p++ = '(';
				else
					*p++ = '-';
			}
			
			if (gCurrencyBefore)
			{
				i = 0;
				while (gCurrencySymbol[i])
					*p++ = gCurrencySymbol[i++];
			}

			FormatFixed(dec, exp10, digits, fDigits, fCommas, p);
			p += strlen(p);
			
			if (fDigits == 0 && p[-1] == gDecimalPoint)
				*--p = 0;

			if (!gCurrencyBefore)
			{
				i = 0;
				while (gCurrencySymbol[i])
					*p++ = gCurrencySymbol[i++];
				*p = 0;
			}

			if (gCurrencyParens)
			{
				if (inValue < 0)
				{
					*p++ = ')';
					*p = 0;
				}
				else
					sWid = fm.StringWidth(outStr) + fm[')'];
			}
			break;
	}

	if (sWid == -1) sWid = fm.StringWidth(outStr);
	
	if (sWid > inWidth)
	{
		int i = static_cast<int>(floor(inWidth / fm['#']));
		p = outStr;
		
		sWid = i * fm['#'];

		while (i--)
			*p++ = '#';
		*p = 0;
	}
	
	return sWid;
} /* CFormatter::FormatDouble */

