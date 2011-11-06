#include "TileEditGrid.h"
#include "Palette.h"
#include <gtkmm/alignment.h>

namespace Polka {


TileEditGrid::TileEditGrid()
	: Gtk::Table( 9, 16, false ), m_Zoom(16), m_pPalette(0)
{
	// place color toggles
	for( int b = 0; b < 6; b++ ) {
		Gtk::Alignment *a = manage( new Gtk::Alignment( Gtk::ALIGN_END, Gtk::ALIGN_CENTER, 0.0, 0.0 ) );
		m_ColorToggles[b].set_no_show_all();
		m_ColorToggles[b].set_active(true);
		m_ColorToggles[b].set_label("C");
		m_ColorToggles[b].signal_toggled().connect(
				sigc::bind<int>( sigc::mem_fun( *this,
						&TileEditGrid::onColorToggle), b ) );
		a->add( m_ColorToggles[b] );
		attach( *a, 2+2*b, 3+2*b, 0, 1, Gtk::FILL, Gtk::SHRINK );
	}

	set_row_spacings( 0 );
	set_col_spacings( 0 );

	setGrid( 4, 4, 0, 0, 0, 0 );
}

TileEditGrid::~TileEditGrid()
{
}

TilePatternEditor::SignalTileChanged TileEditGrid::signalTileChanged()
{
	return m_SignalTileChanged;
}

TileColorEditor::SignalColorSelected TileEditGrid::signalColorSelected()
{
	return m_SignalColorSelected;
}

void TileEditGrid::setGrid( int width, int height,
                            int x, int y, unsigned char *data, int stride )
{
	if( width < 1 ) width = 1;
	if( width > 8 ) width = 8;
	if( height < 1 ) height = 1;
	if( height > 8 ) height = 8;
	// store size
	m_Width = width;
	m_Height = height;
	// show color selector buttons
	for( int b = 0; b < 6; b++ ) {
		if( b < m_Width-2 ) {
			m_ColorToggles[b].show();
		} else {
			m_ColorToggles[b].hide();
		}
	}
	// create new editor grid
	std::vector<TilePatternEditor*>::iterator pit = m_PatternEditors.begin();
	std::vector<TileColorEditor*>::iterator cit = m_ColorEditors.begin();
	for( int h = 0; h < height; h++ ) {
		for( int w = 0; w < width; w++ ) {
			// add new editor if end
			if( pit == m_PatternEditors.end() ) {
				// create editors
				m_PatternEditors.push_back( new TilePatternEditor );
				m_ColorEditors.push_back( new TileColorEditor );
				pit = m_PatternEditors.end() - 1;
				cit = m_ColorEditors.end() - 1;
				// connect signals
				(*pit)->signalTileChanged().connect( sigc::mem_fun(*this,
				                                     &TileEditGrid::tileChanged) );
				(*pit)->signalDrawPixel().connect( sigc::mem_fun(*this,
				                                     &TileEditGrid::drawPixelToAll) );
				(*cit)->signalColorChanged().connect( sigc::mem_fun( **pit,
				                                     &TilePatternEditor::updateColor) );
				(*cit)->signalColorSelected().connect( sigc::mem_fun( *this,
				                                     &TileEditGrid::colorSelected) );
				// set current zoom
				(*pit)->setZoomSize(m_Zoom);
				(*cit)->setZoomSize(m_Zoom);
				(*cit)->set_no_show_all();
			} else {
				// remove from table
				remove( *(*pit) );
				remove( *(*cit) );
			}
			// attach to table
			int ppos = 2*w, cpos = 2*w+1;
			if( w == 0 ) { cpos = 0; ppos = 1; }
			attach( *(*pit), ppos, ppos+1, h+1, h+2, Gtk::SHRINK, Gtk::SHRINK );
			attach( *(*cit), cpos, cpos+1, h+1, h+2, Gtk::SHRINK, Gtk::SHRINK, 2, 0 );
			// show widgets
			(*pit)->show();
			if( w == 0 || w == m_Width-1 )
				(*cit)->show();
			else if( m_ColorToggles[w-1].get_active() )
				(*cit)->show();
			else
				(*cit)->hide();
			// set widget data
			if( data ) {
				// calc pattern data
				unsigned char *dat = data + (y+h*8)*stride + (x+w*8)*4;
				// set editor data
				(*pit)->setData( x+w*8, y+h*8, (unsigned int*)dat, stride );
				(*pit)->setPalette( m_pPalette );
				// set colors
				Byte colors[8];
				unsigned int *ldat = (unsigned int *)dat + 1;
				for( int i = 0; i < 8; i++, ldat += stride/4 )
					colors[i] = (*ldat)>>24;
				(*cit)->setColors( x+w*8, y+h*8, colors );
				(*cit)->setPalette( m_pPalette );
			} else {
				(*cit)->setPalette(0);
				(*pit)->setPalette(0);
				(*pit)->setData(0, 0, 0, 0);
			}
			// next
			pit++; cit++;
		}
	}
	// delete remaining objects
	while( pit != m_PatternEditors.end() ) {
		remove( *(*pit) );
		delete *pit;
		m_PatternEditors.erase(pit);
		remove( *(*cit) );
		delete *cit;
		m_ColorEditors.erase(cit);
	}
}

void TileEditGrid::reset()
{
	std::vector<TilePatternEditor*>::iterator pit = m_PatternEditors.begin();
	std::vector<TileColorEditor*>::iterator cit = m_ColorEditors.begin();
	while( pit != m_PatternEditors.end() ) {
		(*cit)->setPalette(0);
		(*pit)->setPalette(0);
		(*pit)->setData(0, 0, 0, 0);
		pit++; cit++;
	}
}

void TileEditGrid::setPalette( const Palette *palette )
{
	m_pPalette = palette;
	std::vector<TilePatternEditor*>::iterator pit = m_PatternEditors.begin();
	std::vector<TileColorEditor*>::iterator cit = m_ColorEditors.begin();
	while( pit != m_PatternEditors.end() ) {
		(*cit)->setPalette(m_pPalette);
		(*pit)->setPalette(m_pPalette);
		pit++; cit++;
	}
}

void TileEditGrid::onColorToggle( int n )
{
	if( n < m_Width-2 ) {
		std::vector<TileColorEditor*>::iterator cit = m_ColorEditors.begin() + 1 + n;
		int v = m_ColorToggles[n].get_active();
		for( int i = 0; i < m_Height; i++ ) {
			if(v)
				(*cit)->show();
			else
				(*cit)->hide();
			cit += m_Width;
		}
	}
}

void TileEditGrid::tileChanged( int x, int y )
{
	m_SignalTileChanged.emit(x, y);
}

void TileEditGrid::colorSelected( int c )
{
	m_SignalColorSelected.emit(c);
}

void TileEditGrid::setSelectColor( int color )
{
	std::vector<TileColorEditor*>::iterator cit = m_ColorEditors.begin();
	while( cit != m_ColorEditors.end() ) {
		(*cit)->setSelectColor( color );
		cit++;
	}
}

void TileEditGrid::drawPixelToAll( Gtk::Widget& w, int x, int y, bool fg )
{
	std::vector<TilePatternEditor*>::iterator pit = m_PatternEditors.begin();
	while( pit != m_PatternEditors.end() ) {
		if( *pit != &w ) 
			(*pit)->tryDrawPixel( w, x, y, fg );
		pit++;
	}
}

void TileEditGrid::setZoomSize( int zoom )
{
	m_Zoom = zoom;
	std::vector<TilePatternEditor*>::iterator pit = m_PatternEditors.begin();
	std::vector<TileColorEditor*>::iterator cit = m_ColorEditors.begin();
	while( pit != m_PatternEditors.end() ) {
		(*cit)->setZoomSize(zoom);
		(*pit)->setZoomSize(zoom);
		pit++; cit++;
	}
}

} // namespace Polka 
