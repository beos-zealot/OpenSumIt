/*
	Copyright 1997, M.L. Hekkelman, Hekkelman Programmatuur
	
	Templates voor een BTree met vrije keuze van sleutel en data.
	Voor de sleutel moeten in ieder geval operator< en operator==
	gedefinieerd zijn.
*/

#ifndef BTREE_T
#define BTREE_T

#include "BTree.h"
#include <support/Debug.h>
#include "MyError.h"

//#pragma mark --- Node ---

//#if DEBUG
//void WarnForUnlockedTree(int line);
//#define CHECKTREELOCK(t)	if (!t->IsLocked()) WarnForUnlockedTree(__LINE__);
//#else
#define CHECKTREELOCK(t)
//#endif
//
template<class K, class D>
CNode<K,D>::CNode()
{
	p = NULL;
} /* CNode<K,D>::CNode() */

template<class K, class D>
CNode<K,D>::CNode(const K& k)
	: key(k)
{
	p = NULL;
} /* CNode<K,D>::CNode(const K& k) */

template<class K, class D>
CNode<K,D>::CNode(const K& k, const D& d)
	: key(k), data(d)
{
	p = NULL;
} /* CNode<K,D>::CNode(const K& k, const D& d) */

//#pragma mark -
//#pragma mark --- Page ---

template<class K, class D>
CPage<K,D>::CPage()
{
	p0 = NULL;
	m = 0;
	e[0].p = NULL;
} /* CPage<K,D>::CPage() */

template<class K, class D>
CPage<K,D>::~CPage()
{
} /* CPage<K,D>::~CPage() */

template<class K, class D>
inline void
CPage<K,D>::MoveItems(CPage<K,D> *tp, int ti, CPage<K,D> *fp, int fi, int count)
{
	memmove(tp->e + ti, fp->e + fi, count * sizeof(CNode<K,D>));
} /* CPage<K,D>::MoveItems */

template<class K, class D>
void CPage<K,D>::Insert(const K& k, const D& d, bool& h, CNode<K,D>& v)
{
	int L = 0, R = m - 1, i;
	
	while (L <= R)
	{
		i = (L + R) / 2;
		
		if (e[i].key == k)
		{
			THROW((errDuplicateKey));
		}
		else if (e[i].key < k)
			L = i + 1;
		else
			R = i - 1;
	}

	CNode<K,D> u(k, d);
	CPage<K,D> *p;
	
	p = (R > -1) ? e[R].p : p0;
	
	if (p)
		p->Insert(k, d, h, u);
	else
	{
		h = true;
//		u.data = d;
	}

	if (h)
	{
		if (m < 2 * n)	// past er nog op
		{
			h = false;
			m++;
			MoveItems(this, R + 2, this, R + 1, m - R - 2);
			e[R + 1] = u;
		}
		else			// splitsen van pagina
		{
			CPage<K,D> *b = new CPage;
			FailNil(b);
			
			if (R < n)
			{
				if (R == (n - 1))
					v = u;
				else
				{
					v = e[n - 1];
					MoveItems(this, R + 2, this, R + 1, n - R - 2);
					e[R + 1] = u;
				}
				MoveItems(b, 0, this, n, n);
			}
			else
			{
				R -= n;
				v = e[n];
				MoveItems(b, 0, this, n + 1, R);
				b->e[R] = u;
				MoveItems(b, R + 1, this, n + R + 1, n - R - 1);
			}
			m = n;
			b->m = n;
			b->p0 = v.p;
			v.p = b;
		}
	}
} /* void CPage<K,D>::Insert */

template<class K, class D>
bool CPage<K,D>::Find(const K& k, D **dp)
{
	int L = 0, R = m - 1, i;

	do
	{
		i = (L + R) / 2;
		
		if (e[i].key == k)
		{
			if (dp)
				*dp = &e[i].data;
			return true;
		}
		else if (e[i].key < k)
			L = i + 1;
		else
			R = i - 1;
	}
	while (L <= R);
	
	CPage<K,D> *p = R > -1 ? e[R].p : p0;
	return p ? p->Find(k, dp) : false;
} /* bool CPage<K,D>::Find */

template<class K, class D>
void CPage<K,D>::Underflow(CPage<K,D> *a, int s, bool& h)
{
	CPage<K,D> *b, *c = this;
	int k, mb, mc;
	
	mc = c->m;
	if (s < (mc - 1))
	{
		s++;
		b = c->e[s].p;
		mb = b->m;
		k = (mb - n + 1) / 2;
		a->e[n - 1] = c->e[s];
		a->e[n - 1].p = b->p0;
		if (k > 0)
		{
			MoveItems(a, n, b, 0, k - 1);
			c->e[s] = b->e[k - 1];
			c->e[s].p = b;
			b->p0 = b->e[k - 1].p;
			mb -= k;
			MoveItems(b, 0, b, k, mb);
			b->m = mb;
			a->m = n - 1 + k;
			h = false;
		}
		else
		{
			MoveItems(a, n, b, 0, n);
			MoveItems(c, s, c, s + 1, mc - s);
			a->m = 2 * n;
			c->m = mc - 1;
			h = (mc <= n);
			delete b;
			b = NULL;
		}
	}
	else
	{
		b = s ? c->e[s - 1].p : c->p0;
		
		mb = b->m + 1;
		k = (mb - n) / 2;
		if (k > 0)
		{
			MoveItems(a, k, a, 0, n - 1);
			a->e[k - 1] = c->e[s];
			a->e[k - 1].p = a->p0;
			mb -= k;
			MoveItems(a, 0, b, mb, k - 1);
			a->p0 = b->e[mb - 1].p;
			c->e[s] = b->e[mb - 1];
			c->e[s].p = a;
			b->m = mb - 1;
			a->m = n - 1 + k;
			h = false;
		}
		else
		{
			b->e[mb - 1] = c->e[s];
			b->e[mb - 1].p = a->p0;
			MoveItems(b, mb, a, 0, n - 1);
			b->m = 2 * n;
			c->m = mc - 1;
			h = (mc <= n);
			delete a;
			a = NULL;
		}
	}
} /* void CPage<K,D>::Underflow */

template<class K, class D>
void CPage<K,D>::Remove(CPage<K,D> *a, int R, bool& h)
{
	CPage<K,D> *q;
	
	q = e[m - 1].p;
	if (q)
	{
		q->Remove(a, R, h);
		if (h)
			Underflow(q, m - 1, h);
	}
	else
	{
		e[m - 1].p = a->e[R].p;
		a->e[R] = e[m - 1];
		m--;
		h = (m < n);
	}
} /* void CPage<K,D>::Removev */

template<class K, class D>
void CPage<K,D>::Delete(const K& k, bool& h)
{
	int L = 0, R = m, i;
	
	while (L < R)
	{
		i = (L + R) / 2;
		
		if (e[i].key < k)
			L = i + 1;
		else
			R = i;
	}

	CPage<K,D> *q;
	q = (R == 0 ? p0 : e[R - 1].p);
	
	if (R < m && R > -1 && e[R].key == k)
	{
		if (!q)
		{
			m--;
			h = (m < n);
			MoveItems(this, R, this, R + 1, m - R);
		}
		else
		{
			q->Remove(this, R, h);
			if (h)
				Underflow(q, R - 1, h);
		}
	}
	else
	{
		if (q)
		{
			q->Delete(k, h);
			if (h)
				Underflow(q, R - 1, h);
		}
		else
			h = false;
	}
} /* void CPage<K,D>::Delete */

template<class K, class D>
void CPage<K,D>::Destroy()
{
	if (p0)
	{
		p0->Destroy();
		delete p0;
		p0 = NULL;
	}
	
	for (int i = 0; i < m; i++)
	{
		if (e[i].p)
		{
			e[i].p->Destroy();
			delete e[i].p;
			e[i].p = NULL;
		}
		e[i].~CNode();
	}
} /* void CPage<K,D>::Destroy */

#if DEBUG
template<class K, class D>
bool CPage<K,D>::Validate()
{
	int i;
	
	for (i = 0; i < m-1; i++)
		if (e[i].key >= e[i+1].key)
		{
			printf("key %d > %d\n", i, i+1);
			return false;
		}
	
	if (p0 && (p0->e[0].key >= e[0].key || p0->e[p0->m-1].key >= e[0].key))
	{
		printf("keys van p0 > e[0].key\n");
		return false;
	}
	
	if (p0 && !p0->Validate())
		return false;
		
	for (i = 0; i < m; i++)
		if (e[i].p && !e[i].p->Validate())
			return false;

	return true;
} // CPage::Validate
#endif

//#pragma mark -
//#pragma mark --- Tree ---

template<class K, class D>
CBTree<K,D>::CBTree()
{
	root = NULL;
	count = 0;
} /* CBTree<K,D>::CBTree */

template<class K, class D>
CBTree<K,D>::~CBTree()
{
	CHECKTREELOCK(this);
	if (root)
	{
		delete root;
		root = NULL;
	}
} /* CBTree<K,D>::~CBTree */

template<class K, class D>
void CBTree<K,D>::Insert(const K& k, const D& d)
{
	CHECKTREELOCK(this);
	if (!root)
	{
		root = new CPage<K,D>;
		FailNil(root);
	}
	
	bool h;
	CNode<K,D> v;
	
	root->Insert(k, d, h, v);
	if (h)
	{
		CPage<K,D> *q = root;
		root = new CPage<K,D>;
		FailNil(root);
		root->m = 1;
		root->p0 = q;
		root->e[0] = v;
	}
	
	count++;
} /* void CBTree<K,D>::Insert */

template<class K, class D>
void CBTree<K,D>::Delete(const K& k)
{
	CHECKTREELOCK(this);
	if (root)
	{
		bool h;

		root->Delete(k, h);
		
		if (h && root->m == 0)
		{
			CPage<K,D> *q = root;
			root = q->p0;
			delete q;
			q = NULL;
		}
		
		count--;
	}
} /* void CBTree<K,D>::Delete */

template<class K, class D>
bool CBTree<K,D>::Find(const K& k, D **dp)
{
	CHECKTREELOCK(this);

	if (dp) *dp = NULL;
	
	if (root)
		return root->Find(k, dp);
	else
		return false;
} /* CBTree<K,D>::Find */

template<class K, class D>
D& CBTree<K,D>::operator[] (const K& k)
{
//	StLocker<BLocker> lock(this);

	CPage<K,D> *p = root;
	
	while (p)
	{
		int L = 0, R = p->m - 1, i;
		
		do
		{
			i = (L + R) / 2;
			
			if (p->e[i].key == k)
				return p->e[i].data;
			else if (p->e[i].key < k)
				L = i + 1;
			else
				R = i - 1;
		}
		while (L <= R);
		
		p = R > -1 ? p->e[R].p : p->p0;
	}
	
	THROW((errKeyNotFound));
	
//	D temp;
//	return temp;
} /* CBTree<K,D>::operator[] */

template<class K, class D>
void CBTree<K,D>::Destroy()
{
	CHECKTREELOCK(this);
	if (root)
		root->Destroy();
	delete root;
	root = NULL;
} /* CBTree<K,D>::Destroy */

#if DEBUG
template<class K, class D>
bool CBTree<K,D>::Validate()
{
	CHECKTREELOCK(this);
	if (root)
		return root->Validate();
	else
		return true;
} // CBTree<K,D>::Validate
#endif

const int kMaxStackDepth = 25;

//#pragma mark -
//#pragma mark --- Iterator ---

template<class K, class D>
CBTreeIterator<K,D>::CBTreeIterator(CBTree<K,D>* tree)
{
	fTree = tree;
	fStackHeight = -2;
	fCount = tree->count;
} /* CBTreeIterator<K,D>::CBTreeIterator */

template<class K, class D>
CBTreeIterator<K,D>::~CBTreeIterator()
{
} /* CBTreeIterator<K,D>::~CBTreeIterator */

template<class K, class D>
bool CBTreeIterator<K,D>::Next()
{
//	StLocker<BLocker> lock(this);
	CHECKTREELOCK(fTree);
	
	CPage<K,D> **p = fPageStack;
	int *i = fIndexStack;
	int& h = fStackHeight;
	
	if (h == -2)
	{
		if (fTree->root == NULL)
			return false;

		h = 0;
		p[0] = fTree->root;
		i[0] = -1;
		
		while (p[h]->p0)
		{
			p[h + 1] = p[h]->p0;
			i[h + 1] = -1;
			h++;
		}
		
		i[h] = 0;
	}
	else if (h == -1)
		return false;
	else if (i[h] < p[h]->m && p[h]->e[i[h]].p)
	{
		p[h + 1] = p[h]->e[i[h]].p;
		i[h + 1] = -1;
		h++;

		while (p[h]->p0)
		{
			p[h + 1] = p[h]->p0;
			i[h + 1] = -1;
			h++;
		}
		
		i[h] = 0;
	}
	else if (++i[h] == p[h]->m)
	{
		do
			h--;
		while (h >= 0 && ++i[h] == p[h]->m);
	}
	
	fCount = fTree->count;
	
	return
		fStackHeight >= 0 &&
		fPageStack[fStackHeight] != NULL &&
		fPageStack[fStackHeight]->m > fIndexStack[fStackHeight];
} /* CBTreeIterator<K,D>::Next */

template<class K, class D>
bool CBTreeIterator<K,D>::Previous()
{
//	StLocker<BLocker> lock(this);
	CHECKTREELOCK(fTree);
	
	CPage<K,D> **p = fPageStack;
	int *i = fIndexStack;
	int& h = fStackHeight;
	
	if (h == -2)
	{
		return false;
	}
	else if (h == -1)
	{
		if (fTree->root == NULL)
			return false;

		h = 0;
		p[0] = fTree->root;
		i[0] = p[0]->m - 1;
		
		while (p[h]->e[i[h]].p)
		{
			p[h + 1] = p[h]->e[i[h]].p;
			i[h + 1] = p[h + 1]->m - 1;
			h++;
		}
	}
	else if (--i[h] >= 0 && p[h]->e[i[h]].p)
	{
		p[h + 1] = p[h]->e[i[h]].p;
		i[h + 1] = p[h + 1]->m - 1;
		h++;

		while (p[h]->e[i[h]].p)
		{
			p[h + 1] = p[h]->e[i[h]].p;
			i[h + 1] = p[h + 1]->m - 1;
			h++;
		}
	}
	else if (i[h] < 0)
	{
		do
			h--;
		while (h >= 0 && i[h] < 0);
	}
	
	fCount = fTree->count;
	
	return
		fStackHeight >= 0 &&
		fPageStack[fStackHeight] != NULL &&
		fIndexStack[fStackHeight] >= 0;
} /* CBTreeIterator<K,D>::Previous */

template<class K, class D>
bool CBTreeIterator<K,D>::GoTo(const K& k)
{
//	StLocker<BLocker> lock(this);
	CHECKTREELOCK(fTree);

	CPage<K,D> **p = fPageStack;
	int *i = fIndexStack;
	int& h = fStackHeight;

	if (fTree->root == NULL)	// lege boom
	{
		h = -1;
		return false;
	}
//	else if (fTree->count == fCount &&
//		h >= 0 && i[h] < p[h]->m &&
//		p[h]->e[i[h]].key == k)
//		return true;
	
	h = 0;
	i[h] = 0;
	p[h] = fTree->root;

	while (p[h])
	{	
		int L = 0, R = p[h]->m - 1;
		
		do
		{
			i[h] = (L + R) / 2;
			
			if ((p[h])->e[i[h]].key == k)
				return true;
			else if (p[h]->e[i[h]].key < k)
				L = i[h] + 1;
			else
				R = i[h] - 1;
		}
		while (L <= R);
		
		i[h] = R;
		
		p[h + 1] = (R > -1 ? p[h]->e[R].p : p[h]->p0);
		h++;
	}

	h--;

	if (i[h] == -1)
	{
		while (i[h] == -1 && h >= 0)
			h--;
		
		if (h == -1)
			h = -2;
	}
	else
	{
		while (h >= 0 && i[h] == p[h]->m - 1 && k < p[h]->e[i[h]].key)
			h--;
	}

	return false;
} /* CBTreeIterator<K,D>::GoTo */

template<class K, class D>
void CBTreeIterator<K,D>::Reset()
{
//	StLocker<BLocker> lock(this);

	fStackHeight = -2;
} /* CBTreeIterator<K,D>::Reset */
	
template<class K, class D>
const K& CBTreeIterator<K,D>::Key() const
{
	CHECKTREELOCK(fTree);
	if (fStackHeight < 0 ||
		fPageStack[fStackHeight] == NULL ||
		fIndexStack[fStackHeight] < 0 ||
		fIndexStack[fStackHeight] >= fPageStack[fStackHeight]->m)
		THROW((errNoCurrentRecord));
	return fPageStack[fStackHeight]->e[fIndexStack[fStackHeight]].key;
} /* CBTreeIterator<K,D>::Key */

template<class K, class D>
D& CBTreeIterator<K,D>::Data()
{
	CHECKTREELOCK(fTree);
	if (fStackHeight < 0 ||
		fPageStack[fStackHeight] == NULL ||
		fIndexStack[fStackHeight] < 0 ||
		fIndexStack[fStackHeight] >= fPageStack[fStackHeight]->m)
		THROW((errNoCurrentRecord));
	return fPageStack[fStackHeight]->e[fIndexStack[fStackHeight]].data;
} /* CBTreeIterator<K,D>::Data */

template<class K, class D>
bool CBTreeIterator<K,D>::Before()
{
	return fStackHeight == -2;
} /* bool CBTreeIterator<K,D>::Before() */

template<class K, class D>
bool CBTreeIterator<K,D>::After()
{
	return fStackHeight == -1;
} /* bool CBTreeIterator<K,D>::After() */

//template<class K, class D>
//bool CBTreeIterator<K,D>::Lock()
//{
//	if (inherited::Lock())
//		return fTree->Lock();
//	else
//		return false;
//} /* CBTreeIterator<K,D>::Lock */
//
//template<class K, class D>
//void CBTreeIterator<K,D>::Unlock()
//{
//	inherited::Unlock();
//	fTree->Unlock();
//} /* CBTreeIterator<K,D>::Lock */

#endif
