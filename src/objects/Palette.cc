#include "Palette.h"
#include "Project.h"
#include "UndoAction.h"
#include "Functions.h"
#include "ResourceManager.h"
#include <glibmm/i18n.h>
#include <cmath>

#include <iostream>

namespace Polka {

static const char *COLS_ID = "COLORS";
static const char *SWAP_ID = "SWAP";
static const char *SWAP_ITEM = "SWAP_COLORS";
static const char *GRADIENT_ID = "GRAD";
static const char *GRADIENT_ITEM = "GRADIENT_COLORS";

Palette::Palette( Project& _prj, const std::string& _id, int depth, int size )
	: Object( _prj, _id ), m_Depth( depth ), m_SkipSave( false )
{
	if( size < 1 ) size = 1;
	m_Red.resize( size );
	m_Green.resize( size );
	m_Blue.resize( size );
	m_DispRed.resize( size );
	m_DispGreen.resize( size );
	m_DispBlue.resize( size );
}

Palette::~Palette()
{
}

void Palette::setColor( int nr, double r, double g, double b )
{
	// create undo data
	UndoAction& action = project().undoHistory().createAction( *this );
	action.setName( _("Change palette color") );
	Storage& s = action.setUndoData( COLS_ID );
	storeColors( s, nr );
	
	int n = (1<<m_Depth)-1;
	m_Red[nr]   = r;
	m_Green[nr] = g;
	m_Blue[nr]  = b;
	m_DispRed[nr]   = round(n*r)/n;
	m_DispGreen[nr] = round(n*g)/n;
	m_DispBlue[nr]  = round(n*b)/n;

	// add redo data
	Storage &sr = action.setRedoData( COLS_ID );
	storeColors( sr, nr );

	update();
}

int Palette::depth() const
{
	return m_Depth;
}

int Palette::size() const
{
	return int(m_Red.size());
}

double Palette::r( unsigned int nr ) const
{
	if( nr < m_DispRed.size() )
		return m_DispRed[nr];
	else
		return 0;
}

double Palette::g( unsigned int nr ) const
{
	if( nr < m_DispGreen.size() )
		return m_DispGreen[nr];
	else
		return 0;
}

double Palette::b( unsigned int nr ) const
{
	if( nr < m_DispBlue.size() )
		return m_DispBlue[nr];
	else
		return 0;
}

void Palette::setSkipSave( bool value )
{
	m_SkipSave = value;
}

void Palette::initColor( int nr, int r, int g, int b )
{
	int n = (1<<m_Depth)-1;
	m_Red[nr]   = m_DispRed[nr]   = double(r)/n;
	m_Green[nr] = m_DispGreen[nr] = double(g)/n;
	m_Blue[nr]  = m_DispBlue[nr]  = double(b)/n;
}

int Palette::store( Storage& s )
{
	if( !m_SkipSave ) {
		storeColors( s );
	}
	return 0;
}

int Palette::restore( Storage& s )
{
	if( !m_SkipSave ) {
		return restoreColors(s);
	}
	return 0;
}

void Palette::copyColor( int src, int dest )
{
	if( src < 0 || src >= size() ) return;
	if( dest < 0 || dest >= size() ) return;
	if( src == dest ) return;
	
	setColor( dest , m_Red[src], m_Green[src], m_Blue[src] );
}

void Palette::swapColor( int c1, int c2 )
{
	if( c1 < 0 || c1 >= size() ) return;
	if( c2 < 0 || c2 >= size() ) return;
	if( c1 == c2 ) return;

	doSwapColor( c1, c2 );

	// create undo
	UndoAction& action = project().undoHistory().createAction( *this );
	action.setName( _("Swap palette color") );
	
	Storage& s = action.setUndoData( SWAP_ID );
	s.createItem( SWAP_ITEM, "II" );
	s.setField( 0, c1 );
	s.setField( 1, c2 );
	
	s = action.setRedoData( SWAP_ID );
	s.createItem( SWAP_ITEM, "II" );
	s.setField( 0, c1 );
	s.setField( 1, c2 );

	update();
}

void Palette::doSwapColor( int n1, int n2 )
{
	double r = m_Red[n1];
	double g = m_Green[n1];
	double b = m_Blue[n1];
	m_Red[n1] = m_Red[n2];
	m_Green[n1] = m_Green[n2];
	m_Blue[n1] = m_Blue[n2];
	m_Red[n2] = r;
	m_Green[n2] = g;
	m_Blue[n2] = b;
	changeDisplayColors(n1);
	changeDisplayColors(n2);
}

void Palette::createGradient( int c1, int c2 )
{
	if( c1 < 0 || c1 >= size() ) return;
	if( c2 < 0 || c2 >= size() ) return;

	if( c2 < c1 ) {
		int c = c1;
		c1 = c2;
		c2 = c;
	}

	if( c2 - c1 < 2 ) return;

	// gradient ok, store undo info
	UndoAction& action = project().undoHistory().createAction( *this );
	action.setName( _("Palette gradient") );
	// set all "inside" colors for undo
	Storage& su = action.setUndoData( COLS_ID );
	storeColors( su, c1+1, c2-1 );
	// set redo action
	Storage& sr = action.setRedoData( GRADIENT_ID );
	sr.createItem( GRADIENT_ITEM, "II" );
	sr.setField( 0, c1 );
	sr.setField( 1, c2 );

	doCreateGradient( c1, c2 );
	
	update();
}

void Palette::doCreateGradient( int c1, int c2 )
{
	// get components
	double r1 = m_Red[c1],   r2 = m_Red[c2];
	double g1 = m_Green[c1], g2 = m_Green[c2];
	double b1 = m_Blue[c1],  b2 = m_Blue[c2];

	// set gradient
	double f;
	for( int c = c1 + 1; c < c2; c++ ) {
		f = double(c-c1)/(c2-c1);
		m_Red[c]   = r1 + (r2-r1)*f;
		m_Green[c] = g1 + (g2-g1)*f;
		m_Blue[c]  = b1 + (b2-b1)*f;
	}
	changeDisplayColors(c1+1, c2-1);
}

void Palette::changeDisplayColors( int n1, int n2 )
{
	if( n2 < 0 ) n2 = n1;
	int n = (1<<m_Depth)-1;
	for( int c = n1; c <= n2; c++ ) {
		m_DispRed[c]   = round(n*m_Red[c]  )/n;
		m_DispGreen[c] = round(n*m_Green[c])/n;
		m_DispBlue[c]  = round(n*m_Blue[c] )/n;
	}
}

void Palette::storeColors( Storage& s, int n1, int n2 )
{
	if( n1 < 0 ) {
		n1 = 0;
		n2 = m_Red.size()-1;
	} else if( n2 < n1 )
		n2 = n1;

	if( n1 != 0 ) {
		s.createItem( "FIRST_COLOR", "I" );
		s.setField( 0, n1 );
	}
	s.createItem( "RGB", "[FFF]" );
	for( int c = n1; c <= n2; c++ ) {
		s.setField( c-n1, 0, m_Red[c] );
		s.setField( c-n1, 1, m_Green[c] );
		s.setField( c-n1, 2, m_Blue[c] );
	}
}

int Palette::restoreColors( Storage& s )
{
	int c1 = 0;
	if( s.findItem("FIRST_COLOR") )
		if( s.checkFormat("I") )
			c1 = s.integerField(0);
	
	if( !s.findItem("RGB") ) return Storage::EMISSINGDATAFATAL;
	if( !s.checkFormat("[FFF]") ) return Storage::EINCORRECTDATATYPE;
	// get and check color count
	unsigned int cnum = s.arraySize(); std::cout << "numrgb: " << cnum << std::endl;
	if( c1+cnum > m_Red.size() ) 
		cnum = m_Red.size() - c1;
	// read colors
	for( unsigned int c = 0; c < cnum; c++ ) {
		m_Red[c+c1] = s.floatField( c, 0 );
		m_Green[c+c1] = s.floatField( c, 1 );
		m_Blue[c+c1] = s.floatField( c, 2 );
	}
	changeDisplayColors(c1, c1+cnum-1);
	return 0;
}

void Palette::undo( const std::string& id, Storage& s )
{
	performAction( id, s );
}

void Palette::redo( const std::string& id, Storage& s )
{
	performAction( id, s );
}

void Palette::performAction( const std::string& id, Storage& s )
{
	if( id == COLS_ID )
		restoreColors(s);
	else if( id == SWAP_ID ) {
		if( s.findItem( SWAP_ITEM ) ) {
			if( s.checkFormat("II") ) {
				int n1 = s.integerField(0);
				int n2 = s.integerField(1);
				doSwapColor(n1, n2);
			}
		}
	} else if( id == GRADIENT_ID ) {
		if( s.findItem( GRADIENT_ITEM ) ) {
			if( s.checkFormat("II") ) {
				int n1 = s.integerField(0);
				int n2 = s.integerField(1);
				doCreateGradient(n1, n2);
			}
		}
	}

	update();
}

} // namespace Polka
