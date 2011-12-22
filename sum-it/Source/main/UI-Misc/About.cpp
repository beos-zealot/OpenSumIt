#include "About.h"
#include "Colours.h"
#include "StaticString.h"

#include <View.h>
#include <Picture.h>

#include <cmath>
#include <cstdlib>
#include <time.h>

static uint32 Random( uint32 i )
{
	time_t t ;
	time(&t) ;
	t ^= rand() ;
//	float f = fabs(t)/RAND_MAX ;
	return uint32(t % i) ;
}

class AboutBGView : public BView
{
	public:
		AboutBGView( void ) ;
		
		void ResizeToFit( BRect r ) ;
		void Draw( BRect r ) ;
		void Pulse( void ) ;
	private:
		BPicture pict ;
} ;

AboutBGView::AboutBGView( void )
: BView(  BRect(0,0, 300,100 ), "bg", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_PULSE_NEEDED )
{
	SetHighColor( Colours::Grey75 ) ;
	SetLowColor( Colours::Yellow );
	SetFontSize(24) ;
}

void AboutBGView:: ResizeToFit( BRect r )
{
	BRect fr = Frame() ;
	float w , h ;
	
	if( (r.Width()+16) > fr.Width()  )
		w = r.Width() + 16 ;
	else
		w = fr.Width() ;

	if( (r.Height()+48) > fr.Height()  )
		h = r.Height() + 48 ;
	else
		h = fr.Height() ;
	
	ResizeTo(w,h) ;
}

void AboutBGView::Draw( BRect )
{
	DrawPicture( &pict ) ;
}

const char Symbols[] = "0123456789+-*/=%$" ;
void AboutBGView::Pulse( void )
{
	BRect fr = Frame() ;
	BPoint pt( fr.left + Random( fr.IntegerWidth() )
			 , fr.top  + Random( fr.IntegerHeight() ) ) ;
	char text[2] ;
	text[0] = Symbols[Random(sizeof(Symbols))] ;
	text[1] = 0 ;

	AppendToPicture( &pict ) ;
	DrawString( text, pt ) ;
	EndPicture( ) ;
	DrawString( text, pt ) ;
}

class AboutStringView : public StaticString
{
	public:
		AboutStringView( void ) ;
} ;

AboutStringView::AboutStringView( void )
: StaticString( BRect( 50, 25, 250, 75 ),
	"text", "(Watch This Space)",
	B_FOLLOW_H_CENTER | B_FOLLOW_V_CENTER )
{
	SetFontSize( 16 ) ;
}

CAboutBox :: CAboutBox()
: BWindow(BRect(50, 50, 350, 150), "OpenSum-It",
		B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK )
{
	SetPulseRate( 250 * 1000 ) ;

	AboutBGView * bg = new AboutBGView( ) ;
	AddChild(bg) ;
	
	AboutStringView * fg = new AboutStringView( ) ;
	bg->AddChild(fg) ;
	
	fg  ->ResizeToPreferred() ;
	bg  ->ResizeToFit( fg->Bounds() ) ;
	this->ResizeToFit( bg->Bounds() ) ;

	bg->SetViewColor( Colours::Yellow ) ;
	fg->SetViewColor( Colours::Yellow ) ;

	Show();
};

void CAboutBox:: ResizeToFit( BRect r )
{
	BRect fr = Frame() ;
	float w , h ;
	
	if( r.Width() > fr.Width()  )
		w = r.Width() ;
	else
		w = fr.Width() ;

	if( r.Height() > fr.Height()  )
		h = r.Height() ;
	else
		h = fr.Height() ;
	
	ResizeTo(w,h) ;
}
