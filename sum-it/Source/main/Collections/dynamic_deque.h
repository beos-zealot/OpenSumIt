#ifndef DYNAMIC_DEQUE_H
#define DYNAMIC_DEQUE_H

/**********************************************************
 * dynamic_deque.h
 * Copyright (c) 2000
 * Tim Vernum, All Rights Reserved
 **********************************************************
 * This code may be freely used for any purpose.
 * This code is distributed WITHOUT WARRANTY
 **********************************************************/

template <class T>
class dynamic_deque : public std::deque<T *>
{
	public:
		 dynamic_deque( void ) ;
		~dynamic_deque( void ) ;
} ;

template <class T>
inline dynamic_deque<T>::dynamic_deque( void )
{
}

template <class T>
inline dynamic_deque<T>::~dynamic_deque( void )
{
	while (!empty())
	{
		T * t = back() ;
		pop_back() ;
		delete t ;
	}
}

#endif
