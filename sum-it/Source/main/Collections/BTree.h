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
/*	$Id: BTree.h,v 1.1.1.1 2000/03/05 06:41:02 tpv Exp $
	
	Copyright Hekkelman Programmatuur
	
	Created: 11/05/98 20:16:34
*/
/*** Revision History
 ***
 *** TPV (2000-Feb-06) Removed need for global header "sum-it.headers.pch++"
 ***/

#ifndef BTREE_H
#define BTREE_H

#include <Locker.h>

const int
	n = 32,
	kMaxIteratorStackHeight = 10;

template<class K, class D> class CNode;
template<class K, class D> class CPage;
template<class K, class D> class CBTree;
template<class K, class D> class CBTreeIterator;

template<class K, class D>
class CNode {
	friend class CPage<K,D>;
	friend class CBTree<K,D>;
	friend class CBTreeIterator<K,D>;
public:
	CNode();
	CNode(const K&);
	CNode(const K&, const D&);
	
private:
	K key;
	D data;
	CPage<K,D> *p;
};

template<class K, class D>
class CPage {
	friend class CBTree<K,D>;
	friend class CBTreeIterator<K,D>;
public:
	CPage();
	virtual ~CPage();
	
	void Insert(const K& k, const D& d, bool& h, CNode<K,D>& v);
	void Delete(const K& k, bool& h);
	bool Find(const K& k, D **dp = NULL);

	void Destroy();

#if DEBUG
	bool Validate();
#endif

private:
	static inline void MoveItems(CPage *fp, int fi, CPage *tp, int ti, int count);
	void Underflow(CPage *a, int s, bool& h);
	void Remove(CPage *a, int R, bool& h);

	int m;
	CPage<K,D>* p0;
	CNode<K,D> e[n*2];
};

template<class K, class D>
class CBTree : public BLocker {
	friend class CBTreeIterator<K,D>;
public:
	CBTree();
	virtual ~CBTree();

	void Insert(const K& k, const D& d);
	void Delete(const K& k);
	bool Find(const K& k, D **dp = NULL);
	
	void Destroy();

#if DEBUG
	bool Validate();
#endif

	D& operator[] (const K& k);
	
	int Count() const
		{ return count; };
	
private:
	CPage<K,D> *root;
	int count;
};

template<class K, class D>
class CBTreeIterator {
public:
	CBTreeIterator(CBTree<K,D>* tree);
	virtual ~CBTreeIterator();

	bool operator()()
		{ return Next(); };
	bool Next();
	bool Previous();
	void Reset();
	bool GoTo(const K& k);
	bool Before();
	bool After();
	
//	virtual bool Lock();
//	virtual void Unlock(); 
	
	const K& Key() const;
	D& Data();
	
private:
	CBTree<K,D>* fTree;
	int fStackHeight;
	CPage<K,D>* fPageStack[kMaxIteratorStackHeight];
	int fIndexStack[kMaxIteratorStackHeight];
	int fCount;
};

#endif
