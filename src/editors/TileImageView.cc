#include "TileImageView.h"
#include "Palette.h"
#include <iostream>
using namespace std;
namespace Polka {


TileImageView::TileImageView()
	: m_Scale(2), m_HoverX(-1), m_SelectX(-1)
{
	// accept mouse button and move events
	add_events( Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK
	            | Gdk::LEAVE_NOTIFY_MASK );
	// default
	setSelector(8);
}

TileImageView::~TileImageView()
{
}

TileImageView::SignalSelectionChanged TileImageView::signalSelectionChanged()
{
	return m_SignalSelectionChanged;
}

void TileImageView::setImage( Cairo::RefPtr<Cairo::ImageSurface> image )
{
	if( image ) {
		m_Image = image;
		set_size_request( image->get_width() *m_Scale,
		                  image->get_height()*m_Scale );
	} else {
		m_Image.clear();
		set_size_request(0, 0);
	}
	queue_draw();
}

bool TileImageView::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// paint the background
	cr->save();
	cr->scale( m_Scale, m_Scale );
	Cairo::RefPtr<Cairo::SurfacePattern> sp = Cairo::SurfacePattern::create(m_Image);
	sp->set_filter(Cairo::FILTER_FAST);
	cr->set_source(sp);
	cr->rectangle(0.0, 0.0, m_Image->get_width()*m_Scale, m_Image->get_height()*m_Scale );
	cr->fill();

	// reset scaling for drawing cursors		
	cr->restore();
	// select rect
	if( m_SelectX >= 0 ) {
		cr->set_source_rgb( 1, 1, 1 );
		drawSelectRect( cr, m_SelectX, m_SelectY, m_SelectWidth, m_SelectHeight);
	}
	// cursor rect
	if( m_HoverX >= 0 ) {
		cr->set_source_rgb( 1, 0, 0 );
		drawSelectRect( cr, m_HoverX, m_HoverY, m_SelectWidth, m_SelectHeight);
	}
	
	return true;
}

void TileImageView::drawSelectRect( const Cairo::RefPtr<Cairo::Context>& context,
	double x, double y, double w, double h )
{
	context->set_antialias( Cairo::ANTIALIAS_NONE );
	context->set_line_width(1.0);
	context->rectangle( x*m_Scale-1, y*m_Scale-1, w*m_Scale+2, h*m_Scale+2 );
	context->stroke();
	context->set_source_rgba( 0, 0, 0, .5 );
	context->rectangle( x*m_Scale, y*m_Scale, w*m_Scale, h*m_Scale );
	context->stroke();
	context->set_source_rgb( 0, 0, 0 );
	context->rectangle( x*m_Scale-2, y*m_Scale-2, w*m_Scale+4, h*m_Scale+4 );
	context->stroke();
}

void TileImageView::tileUpdated( int x, int y )
{
	// redraw tile at x,y
	queue_draw_area( x*m_Scale, y*m_Scale, 8*m_Scale, 8*m_Scale );
}

void TileImageView::setSelection( int x, int y )
{
	if( !m_Image ) return;
	// redraw current position
	drawSelect();
	// clip to edges
	if( x < 0 )
		x = 0;
	else if( x+m_SelectWidth > m_Image->get_width() )
		x = m_Image->get_width() - m_SelectWidth;
	if( y < 0 )
		y = 0;
	else if( y+m_SelectHeight > m_Image->get_height() )
		y = m_Image->get_height() - m_SelectHeight;
	// set selection snapped
	m_SelectX = (x / m_GridHor) * m_GridHor;
	m_SelectY = (x / m_GridVer) * m_GridVer;
	// redraw new position
	drawSelect();
	// signal change
	m_SignalSelectionChanged.emit( m_SelectX, m_SelectY );
}

void TileImageView::setSelector( int width, int height, int horgrid, int vergrid )
{
	// redraw current positions
	drawSelect();
	drawHover();

	m_SelectWidth = width;
	if( height )
		m_SelectHeight = height;
	else
		m_SelectHeight = width;
	
	m_GridHor = horgrid;
	if( vergrid )
		m_GridVer = vergrid;
	else
		m_GridVer = horgrid;

	// reset hover
	m_HoverX = -1;
	
	// snap inside boundaries
	setSelection( m_SelectX, m_SelectY );
	
}

void TileImageView::drawHover()
{
	if( m_HoverX >= 0 )
		queue_draw_area( m_HoverX*m_Scale-2, m_HoverY*m_Scale-2,
		                 m_SelectWidth*m_Scale+5, m_SelectHeight*m_Scale+5 );
}

void TileImageView::drawSelect()
{
	if( m_SelectX >= 0 )
		queue_draw_area( m_SelectX*m_Scale-2, m_SelectY*m_Scale-2,
		                 m_SelectWidth*m_Scale+5, m_SelectHeight*m_Scale+5 );
}

bool TileImageView::on_button_press_event(GdkEventButton* event)
{
	if( event->button == 1 ) {
		if( m_HoverX >= 0 ) {
			// redraw current position
			drawSelect();
			// set selection to current hover
			m_SelectX = m_HoverX;
			m_SelectY = m_HoverY;
			// redraw new position
			drawSelect();
			// signal selection
			m_SignalSelectionChanged.emit( m_SelectX, m_SelectY );
		}
	}
	return true;
}

bool TileImageView::on_motion_notify_event (GdkEventMotion* event)
{
	if( m_SelectWidth >= 0 ) {
		// redraw for existing hover
		drawHover();
		// calc hover rectangle location
		m_HoverX = (event->x / m_Scale) - ((m_SelectWidth/2-1)/m_GridHor)*m_GridHor;
		m_HoverY = (event->y / m_Scale) - ((m_SelectHeight/2-1)/m_GridVer)*m_GridVer;
		// clip to edges
		if( m_HoverX < 0 )
			m_HoverX = 0;
		else if( m_HoverX+m_SelectWidth > m_Image->get_width() )
			m_HoverX = m_Image->get_width() - m_SelectWidth;
		if( m_HoverY < 0 )
			m_HoverY = 0;
		else if( m_HoverY+m_SelectHeight > m_Image->get_height() )
			m_HoverY = m_Image->get_height() - m_SelectHeight;
		// snap to grid
		m_HoverX = (m_HoverX / m_GridHor) * m_GridHor;
		m_HoverY = (m_HoverY / m_GridVer) * m_GridVer;
		// redraw
		drawHover();
	}
	return true;
}

bool TileImageView::on_leave_notify_event (GdkEventCrossing* event)
{
	drawHover();
	m_HoverX = -1;
	return true;
}

} // namespace Polka
