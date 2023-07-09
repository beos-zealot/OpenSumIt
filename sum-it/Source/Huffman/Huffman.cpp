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

#include "Huffman.h"

#ifndef CALLOC
#define CALLOC calloc
#endif

#ifndef MALLOC
#define MALLOC malloc
#endif

#ifndef FREE
#define FREE free
#endif

#include <ByteOrder.h>
#include <cstdlib>
#include <cstring>
#	define htonl(x) B_HOST_TO_BENDIAN_INT32(x)
#	define ntohl(x) B_BENDIAN_TO_HOST_INT32(x)
#	define htons(x) B_HOST_TO_BENDIAN_INT16(x)
#	define ntohs(x) B_BENDIAN_TO_HOST_INT16(x)

#define kVersion htonl('Hfmn')	// ???????

struct HuffmanHeader {
	unsigned long hVersion;
	long hSourceLength;
	long hCompressedLength;
	short hCodeCount;
};
const int kHuffmanHeaderSize = 16;

void PQDownHeap(long heap[], long count[], long N, long k);
void RadixInsert(unsigned short v, unsigned short i,
	unsigned short b, struct node **p);
void DeleteTrie(struct node *p);

//
// Implementatie
//

void PQDownHeap(long heap[], long count[], long N, long k)
{
	long j, v;
	
	v = heap[k];
	while (k <= N / 2)
	{
		j = k * 2;
		if (j < N && count[heap[j]] > count[heap[j + 1]])
			j++;
		if (count[v] < count[heap[j]])
			break;
		heap[k] = heap[j];
		k = j;
	}
	heap[k] = v;
} /* PQDownHeap */

void EncodeHuffman(const void *inData, long inDataLength,
	void **outData, long *outLength)
{
	long *heap, *count, *dad, *code, *len;
	unsigned char *sp, *buf, *ap;
	long i, j, k, N;
	unsigned short *cp;
	struct HuffmanHeader h;

	h.hVersion = kVersion;
	h.hSourceLength = inDataLength;

	heap = (long *)CALLOC(512 * sizeof(long), 5);
	count = heap + 512;
	dad = count + 512;
	code = dad + 512;
	len = code + 512;
	
	sp = (unsigned char *)inData;
	
	for (i = 0; i < inDataLength; i++)
		count[sp[i]]++;
	
	N = 0;
	for (i = 0; i < 256; i++)
		if (count[i])
			heap[++N] = i;
	
	for (k = N; k >= 1; k--)
		PQDownHeap(heap, count, N, k);
	
	h.hCodeCount = N;
	
	while (N > 1)
	{
		long t = heap[1];
		heap[1] = heap[N--];
		PQDownHeap(heap, count, N, 1);
		count[255 + N] = count[heap[1]] + count[t];
		dad[t] = 255 + N;
		dad[heap[1]] = -255 - N;
		heap[1] = 255 + N;
		PQDownHeap(heap, count, N, 1);
	}

	dad[255+N] = 0;
	
	for (k = 0; k < 256; k++)
	{
		if (!count[k])
		{
			code[k] = 0;
			len[k] = 0;
		}
		else
		{
			long t = dad[k], x = 0;
			i = 0;
			j = 1;
			
			while (t)
			{
				if (t < 0)
				{
					x += j;
					t = -t;
				}
				t = dad[t];
				j += j;
				i++;
			}
			
			code[k] = x;
			len[k] = i;
		}
	}
	
	buf = (unsigned char *)CALLOC(inDataLength, 1);
	if (!buf)
		return;
	
	{
		long b = 7, j;
		unsigned char *bp = buf;
		
		for (j = 0; j < inDataLength; j++)
		{
			for (i = len[sp[j]] - 1; i >= 0; i--)
			{
				if (code[sp[j]] & 1 << i)
					*bp |= 1 << b;
				if (--b < 0)
				{
					b = 7;
					bp++;
				}
			}
		}
		bp++;
		
		h.hCompressedLength = bp - buf;
	}
	
	*outLength = sizeof(h) + h.hCompressedLength +
		h.hCodeCount * (sizeof(short) + sizeof(char));
	*outData = MALLOC(*outLength);
	
	memcpy(*outData, &h, sizeof(h));
	
	cp = (unsigned short *)((char *)*outData + kHuffmanHeaderSize);
	ap = (unsigned char *)(cp + h.hCodeCount);
	
	for (i = 0; i < 256; i++)
		if (len[i])
		{
			*cp++ = code[i] << (16 - len[i]);
			*ap++ = i;
		}

	memcpy(ap, buf, h.hCompressedLength);
	
	FREE(buf);
	FREE(heap);
} /* EncodeHuffman */

#define nBranch 1
#define nLeaf 2
struct node {
	struct node *l, *r;
	short t;
	short key, info;
};

/* Procedure om een radix search trie (ja dat klopt!) te bouwen */
void RadixInsert(unsigned short v, unsigned short i,
	unsigned short b, struct node **p)
{
	struct node *x;

	if (!*p)
	{
		*p = (struct node *)MALLOC(sizeof(struct node));
		if (*p)
		{
			(*p)->t = nLeaf;
			(*p)->key = v;
			(*p)->info = i;
			(*p)->l = NULL;
			(*p)->r = NULL;
		}
	}
	else if ((*p)->t == nBranch)
	{
		if (v & 1 << b)
			RadixInsert(v, i, b - 1, &(*p)->r);
		else
			RadixInsert(v, i, b - 1, &(*p)->l);
	}
	else
	{
		x = (struct node *)CALLOC(sizeof(struct node), 1);
		if (x)
		{
			x->t = nBranch;
			if ((*p)->key & 1 << b)
				x->r = *p;
			else
				x->l= *p;
			*p = x;
			RadixInsert(v, i, b, &x);
		}
	}
} /* RadixInsert */

void DeleteTrie(struct node *p)
{
	if (p->l)
		DeleteTrie(p->l);
	if (p->r)
		DeleteTrie(p->r);
	FREE(p);
} /* DeleteTrie */

long DecodeHuffman(const void *inData, void **outData)
{
	struct node *root, *n;
	unsigned short *code;
	unsigned char *ascii, *sp, *dp;
	long i, j, l;
	struct HuffmanHeader *h;
	
	h = (struct HuffmanHeader *)inData;
	
	h->hSourceLength = ntohl(h->hSourceLength);
	h->hCompressedLength = ntohl(h->hCompressedLength);
	h->hCodeCount = ntohs(h->hCodeCount);

	*outData = NULL;
	
/* Check de header, we willen geen bogus data decoderen... */
	if (h->hVersion != kVersion)
		return -1;

/* pointers naar de verschillende data onderdelen */
	code = (unsigned short *)((char *)inData + kHuffmanHeaderSize);
	ascii = (unsigned char *)(code + h->hCodeCount);

/* Bouw de decodeer Trie, dit is een radix search trie */
	root = NULL;
	for (i = 0; i < h->hCodeCount; i++)
		RadixInsert(ntohs(code[i]), ascii[i], 15, &root);

/* Initialiseer de data */
	sp = ascii + h->hCodeCount;
	dp = (unsigned char *)MALLOC(h->hSourceLength);
	if (dp)
		*outData = dp;
	else
		return -1;

/* en start decoderen */
	l = 0;
	n = root;
	
	for (i = 0; i < h->hCompressedLength && l < h->hSourceLength; i++)
	{
		for (j = 7; j >= 0; j--)
		{
			n = (sp[i] & (1 << j)) ? n->r : n->l;
			if (!n)
			{
				FREE(dp);
				*outData = NULL;
				return -1;
			}
			if (n->t == nLeaf)
			{
				if (l < h->hSourceLength)
				{
					dp[l] = n->info;
					if (++l == h->hSourceLength)
						break;
					n = root;
				}
			}
		}
	}
	
	DeleteTrie(root);
	
	return h->hSourceLength;
} /* DecodeHuffman */
