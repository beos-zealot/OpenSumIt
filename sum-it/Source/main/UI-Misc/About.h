#ifndef ABOUT_H
#define ABOUT_H

#include <Window.h>

class CAboutBox : public BWindow
{
	public:
		CAboutBox();
		void ResizeToFit( BRect r ) ;
};

#endif

