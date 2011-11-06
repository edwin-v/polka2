#include "TilePatternEditor.h"
#include "Palette.h"


namespace Polka {


TilePatternEditor::TilePatternEditor()
	: m_Data( 0 ), m_pPalette( 0 )
{
	// accept mouse butten events
	add_events( Gdk::BUTTON_PRESS_MASK | 
	            Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON3_MOTION_MASK );

	// set defaults
	setZoomSize(16);
}

TilePatternEditor::~TilePatternEditor()
{
}

TilePatternEditor::SignalTileChanged TilePatternEditor::signalTileChanged()
{
	return m_SignalTileChanged;
}

TilePatternEditor::SignalDrawPixel TilePatternEditor::signalDrawPixel()
{
	return m_SignalDrawPixel;
}

void TilePatternEditor::setData( int x, int y, unsigned int *data, int stride )
{
	m_Data = data;
	m_X = x;
	m_Y = y;
	m_Stride = stride/4;
	queue_draw();
}

void TilePatternEditor::setPalette( const Palette *palette )
{
	m_pPalette = palette;
	queue_draw();
}

void TilePatternEditor::setZoomSize( int zoom )
{
	m_Size = zoom;
	set_size_request( 8*m_Size, 8*m_Size );
}

void TilePatternEditor::updateColor( int x, int y, Byte color )
{
	if( m_Data == 0 || m_pPalette == 0 ) return;
	if( m_X != x ) return;
	// calc line to redraw
	int line = y - m_Y;
	if( line < 0 || line >= 8 ) return;
	// set new colour
	unsigned int *ldat = m_Data + line*m_Stride;
	ldat[1] = (color << 24);
	updateLine( line );
}

void TilePatternEditor::updateLine( int line )
{
	unsigned int *ldat = m_Data + line*m_Stride;
	// redraw line data
	int fcol = ldat[1] >> 28;
	int bcol = (ldat[1] >> 24) & 15;
	Byte pat = ldat[0] >> 24;
	for( int p = 0, mask = 128; p < 8; p++, mask >>= 1 ) {
		int col = pat&mask ? fcol : bcol;
		// write colour from palette
		ldat[p] = (ldat[p] & 0xFF000000)
			+ ( int(round(m_pPalette->r(col)*255)) << 16 )
			+ ( int(round(m_pPalette->g(col)*255)) << 8 )
			+ ( int(round(m_pPalette->b(col)*255)) );
	}
	// update display
	queue_draw();
	m_SignalTileChanged.emit( m_X, m_Y+line );
}

bool TilePatternEditor::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	cr->set_antialias( Cairo::ANTIALIAS_NONE );
	cr->set_line_width(1.0);
	
	// paint the squares
	for( int y = 0; y < 8; y++ ) {
		unsigned int *dat = m_Data + y*m_Stride;
		for( int x = 0; x < 8; x++ ) {
			cr->set_source_rgb( double(dat[x] & 0x00FF0000) / 0x00FF0000,
								double(dat[x] & 0x0000FF00) / 0x0000FF00,
								double(dat[x] & 0x000000FF) / 0x000000FF );
			cr->rectangle( x*m_Size, y*m_Size, m_Size-1, m_Size-1 );
			cr->fill();
		}
		if( y < 7 ) {
			cr->set_source_rgb( 0.1, 0.1, 0.1 );
			cr->move_to( (y+1)*m_Size-1, 0 );
			cr->rel_line_to( 0, 8*m_Size-1 );
			cr->stroke();
			cr->move_to( 0, (y+1)*m_Size-1 );
			cr->rel_line_to( 8*m_Size-1, 0 );
			cr->stroke();
		}
	}

	return true;
}

bool TilePatternEditor::on_button_press_event(GdkEventButton* event)
{
	m_DrawFG = event->button == 1;
	drawAction( event->x, event->y );	
	return true;
}

void TilePatternEditor::drawAction( int x, int y )
{
	// inside widget
	if( x < 0 || x >= 8*m_Size || y < 0 || y >= 8*m_Size ) {
		m_SignalDrawPixel.emit( *this, x, y, m_DrawFG );
		return;
	}
	
	// get pixel
	int line = y/m_Size;
	int col  = x/m_Size;

	col = (128 >> col);

	// change data
	unsigned int *ldat = m_Data + line*m_Stride;
	if( m_DrawFG ) {
		// set foreground
		ldat[0] |= (col << 24);
	} else {
		// set background
		ldat[0] &= ~(col << 24);
	}
	// update
	updateLine(line);
}

bool TilePatternEditor::on_motion_notify_event (GdkEventMotion* event)
{
	drawAction( event->x, event->y );
	return true;
}

void TilePatternEditor::tryDrawPixel( Widget& w, double x, double y, bool fg )
{
	int X, Y;
	if( w.translate_coordinates( *this, x, y, X, Y ) ) {
		m_DrawFG = fg;
		if( X > 0 && Y > 0 && X < 8*m_Size && Y < 8*m_Size )
			drawAction( X, Y );
	}
}

} // namespace Polka
