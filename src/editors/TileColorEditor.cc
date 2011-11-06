#include "TileColorEditor.h"
#include "Palette.h"


namespace Polka {


TileColorEditor::TileColorEditor()
	: m_pPalette( 0 ), m_Select( 4 )
{
	// accept mouse butten events
	add_events( Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON1_MOTION_MASK );

	// set defaults
	for( int i = 0; i < 8; i++) m_Colors[i] = 202+i; m_X = 0; m_Y = 0;
	setZoomSize(16);
}

TileColorEditor::~TileColorEditor()
{
}

TileColorEditor::SignalColorChanged TileColorEditor::signalColorChanged()
{
	return m_SignalColorChanged;
}

TileColorEditor::SignalColorSelected TileColorEditor::signalColorSelected()
{
	return m_SignalColorSelected;
}

void TileColorEditor::setColors( int x, int y, const Byte *colors )
{
	for( int i = 0; i < 8; i++)
		m_Colors[i] = colors[i];
	m_X = x;
	m_Y = y;
	queue_draw();
}

void TileColorEditor::setPalette( const Palette *palette )
{
	m_pPalette = palette;
	queue_draw();
}

void TileColorEditor::setZoomSize( int zoom )
{
	m_Size = zoom;
	// adjust widget size
	auto sc = get_style_context();
	auto border = sc->get_border();
	m_Left = border.get_left();
	m_Top = border.get_top();

	set_size_request( 2*m_Size, 8*m_Size );
}

void TileColorEditor::setSelectColor( int color )
{
	m_Select = color & 15;
}

bool TileColorEditor::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{

	cr->set_antialias( Cairo::ANTIALIAS_NONE );
	cr->set_line_width(1.0);


	// paint the squares
	for( int c = 0; c < 8; c++ ) {
		// foreground colour
		int col = m_Colors[c] >> 4;
		cr->set_source_rgb( m_pPalette->r( col ),
							m_pPalette->g( col ),
							m_pPalette->b( col ) );
		cr->rectangle( 0, c*m_Size, m_Size-1, m_Size-1 );
		cr->fill();
		cr->set_source_rgba( 1.0, 1.0, 1.0, 0.5 );
		cr->rectangle( 0, c*m_Size, m_Size-2, m_Size-2 );
		cr->stroke();
		
		// background colour
		col = m_Colors[c] & 15;
		cr->set_source_rgb( m_pPalette->r( col ),
							m_pPalette->g( col ),
							m_pPalette->b( col ) );
		cr->rectangle( m_Size, c*m_Size, m_Size-1, m_Size-1 );
		cr->fill();
		cr->set_source_rgba( 0.0, 0.0, 0.0, 0.25 );
		cr->rectangle( m_Size, c*m_Size, m_Size-2, m_Size-2 );
		cr->stroke();
	}
	// draw the  frame
	/*get_style()->paint_shadow(window, Gtk::STATE_NORMAL, Gtk::SHADOW_IN, 
		Gdk::Rectangle( event->area.x, event->area.y, event->area.width, event->area.height ),
		*this, "frame", 0, 0, 2*m_Left + 2*m_Size, 2*m_Top + 8*m_Size );*/

	return true;
}

bool TileColorEditor::on_button_press_event(GdkEventButton* event)
{
	if( event->button == 1 ) {
		m_FgDown = event->x < m_Left+m_Size;
		setColor( event->y );
	} else if( event->button == 3 ) {
		int line = (event->y - m_Top)/m_Size;
		if( line < 0 || line > 7 ) return true;
		int col;
		if( event->x >= m_Left+m_Size )
			col = m_Colors[line] & 15;
		else
			col = m_Colors[line] >> 4;
		m_SignalColorSelected.emit( col );
	}
	return true;	
}

bool TileColorEditor::on_motion_notify_event (GdkEventMotion* event)
{
	setColor( event->y );
	return true;
}

void TileColorEditor::setColor( int y )
{
		int line = (y - m_Top)/m_Size;
		if( line < 0 || line > 7 ) return;
		if( m_FgDown ) {
			// alter foreground
			m_Colors[line] = (m_Colors[line] & 0x0F) + 16*m_Select;
		} else {
			// alter background
			m_Colors[line] = (m_Colors[line] & 0xF0) + m_Select;
		}
		// signal
		m_SignalColorChanged.emit( m_X, m_Y+line, m_Colors[line] );
		
		queue_draw();
}

} // namespace Polka
