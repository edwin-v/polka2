/*
	Copyright (C) 2013 Edwin Velds

    This file is part of Polka 2.

    Polka 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Polka 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Polka 2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ColorSelector.h"
#include "Palette.h"


namespace Polka {

const int MIN_S = 6;
const int NAT_S = 16;

ColorSelector::ColorSelector( int width, int height, bool display_selection )
	: m_pPalette(0), m_ReqWidth(width), m_ReqHeight(height), m_Selection(display_selection),
	  m_Dragging(false), m_PriColor(-1), m_SecColor(-1), m_HovColor(0)
{
	add_events( Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK |
	            Gdk::LEAVE_NOTIFY_MASK );
	
	// init markers
	m_rCursor = RectangleShape::create(RectangleShape::FULL_INSIDE);
	m_rCursor->setBaseWidth(1);
	m_rCursor->setLineDashSize(1);
	add( 10, m_rCursor );
	m_rSecondaryMarker = RectangleShape::create(RectangleShape::FULL_INSIDE);
	m_rSecondaryMarker->setLineColor( 1, 0, 0 );
	m_rSecondaryMarker->setBaseWidth(1);
	m_rSecondaryMarker->setLineDashSize(1);
	add( 5, m_rSecondaryMarker );
}

ColorSelector::~ColorSelector()
{
}

void ColorSelector::setPalette( const Palette *palette )
{
	// ignore if not changed
	if( palette == m_pPalette ) return;

	m_pPalette = palette;

	// reset
	queue_draw();
}

int ColorSelector::primaryColor() const
{
	return m_PriColor;
}

int ColorSelector::secondaryColor() const
{
	return m_SecColor;
}

int ColorSelector::hoverColor() const
{
	return m_HovColor;
}

IntSignal ColorSelector::signalColorClicked()
{
	return m_SignalColorClicked;
}

VoidSignal ColorSelector::signalColorDoubleClicked()
{
	return m_SignalColorDoubleClicked;
}

void ColorSelector::setSelection( int pri, int sec )
{
	m_PriColor = ( pri < 0 || pri > m_pPalette->size() ) ? -1 : pri;
	m_SecColor = ( pri < 0 || pri > m_pPalette->size() ) ? -1 : sec;
	queue_draw();
}

void ColorSelector::setSize( int width, int height )
{
	m_Width = width;
	m_Height = height;
}

void ColorSelector::reset()
{
	m_HovColor = -1;
	setPalette(0);
}

Gtk::SizeRequestMode ColorSelector::get_request_mode_vfunc() const
{
	if( m_ReqWidth > 0 )
		return Gtk::SIZE_REQUEST_HEIGHT_FOR_WIDTH;
	else
		return Gtk::SIZE_REQUEST_WIDTH_FOR_HEIGHT;
}

void ColorSelector::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
	int sz = m_pPalette?m_pPalette->size():1;
	
	int w = m_ReqWidth;
	if( m_ReqWidth < 1 && m_ReqHeight < 1 ) {
		// no restrictions, assume approximately square
		w = ceil( sqrt(double(sz)) );
	} else if( m_ReqWidth < 1 ) {
		// height fixed only
		w = ceil( sz/m_ReqHeight );
	}
	minimum_width = MIN_S*w;
	natural_width = NAT_S*w;
}

void ColorSelector::get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const
{
	int sz = m_pPalette?m_pPalette->size():1;

	if( m_ReqWidth < 1 ) {
		// width free, calc minimum...
		int w = width / MIN_S;
		int h = sz/w; if( sz%w ) h++;
		minimum_height = h * width/w;
		// ...and natural
		w = width / NAT_S;
		h = sz/w; if( sz%w ) h++;
		natural_height = h * width/w;		
	} else if( m_ReqHeight < 1 ) {
		// width fixed, calc height
		int h = sz/m_ReqWidth; if( sz%m_ReqWidth ) h++;
		minimum_height = MIN_S*h;
		natural_height = h * width / m_ReqWidth;
	} else {
		// both fixed
		minimum_height = MIN_S*m_ReqHeight;
		natural_height = m_ReqHeight * width / m_ReqWidth;
	}
}

void ColorSelector::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
	int sz = m_pPalette?m_pPalette->size():1;
	
	int h = m_ReqHeight;
	if( m_ReqWidth < 1 && m_ReqHeight < 1 ) {
		// no restrictions, assume approximately square
		h = floor( sqrt(double(sz)) );
	} else if( m_ReqHeight < 1 ) {
		// width fixed only
		h = ceil( sz/m_ReqWidth );
	}
	minimum_height = MIN_S*h;
	natural_height = NAT_S*h;
}

void ColorSelector::get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const
{
	int sz = m_pPalette?m_pPalette->size():1;

	if( m_ReqHeight < 1 ) {
		// height free, calc minimum...
		int h = height / MIN_S;
		int w = sz/h; if( sz%h ) w++;
		minimum_width = w * height/h;
		// ...and natural
		h = height / NAT_S;
		w = sz/h; if( sz%h ) w++;
		natural_width = w * height/h;		
	} else if( m_ReqWidth < 1 ) {
		// height fixed, calc width
		int w = sz/m_ReqHeight; if( sz%m_ReqHeight ) w++;
		minimum_width = MIN_S*w;
		natural_width = w * height / m_ReqHeight;
	} else {
		// both fixed
		minimum_width = MIN_S*m_ReqWidth;
		natural_width = m_ReqWidth * height / m_ReqHeight;
	}
}

void ColorSelector::calcLayout()
{
	if( !m_pPalette ) return;

	if( m_ReqWidth < 1 && m_ReqHeight < 1 ) {
		const Gtk::Allocation& a = get_allocation();

		// calculate sizes
		double wx = (double)a.get_width();
		double wy = (double)a.get_height();

		// calculate layout
		m_Width = m_pPalette->size();
		m_Height = 1;
		while( m_Width > 1 ) {
			if (fabs(wx/m_Width - wy/m_Height) < fabs(wx/(m_Width>>1) - wy/(m_Height<<1)) )
				break;
			m_Width >>= 1;
			m_Height <<= 1;
		}
	} else if( m_ReqWidth < 1 ) {
		m_Width = m_pPalette->size() / m_ReqHeight + (m_pPalette->size() % m_ReqHeight > 0);
		m_Height = m_ReqHeight;
	} else if( m_ReqHeight < 1 ) {
		m_Width = m_ReqWidth;
		m_Height = m_pPalette->size() / m_ReqWidth + (m_pPalette->size() % m_ReqWidth > 0);
	} else {
		m_Width = m_ReqWidth;
		m_Height = m_ReqHeight;
	}
}

bool ColorSelector::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	const Gtk::Allocation& a = get_allocation();

	// calculate sizes
	double wx = (double)a.get_width();
	double wy = (double)a.get_height();

	calcLayout();
	
	if( m_pPalette ) {
		int c = 0;
		// draw colors
		for( int y = 0; y < m_Height; y++ ) {
			double cy =  round( y   *wy/m_Height);
			double cyn = round((y+1)*wy/m_Height) - cy;
			for( int x = 0; x < m_Width; x++ ) {
				// exit if done
				if( c >= m_pPalette->size() ) break;
				// set palette color
				cr->set_source_rgb( m_pPalette->r(c), m_pPalette->g(c), m_pPalette->b(c) );
				
				// draw rectangle
				double cx =  round( x   *wx/m_Width);
				double cxn = round((x+1)*wx/m_Width) - cx;

				cr->rectangle( cx, cy, cxn, cyn );
				cr->fill();
				
				// draw selection marker
				if( m_Selection ) {
					if( c == m_PriColor ) {
						m_rCursor->setLocation( cx, cy );
						m_rCursor->setSize( cxn, cyn );
					} else if( c == m_SecColor ) {
						m_rSecondaryMarker->setLocation( cx, cy );
						m_rSecondaryMarker->setSize( cxn, cyn );
					}
				}

				// hover highlight
				if( c == m_HovColor ) {
					cr->rectangle( cx+0.5, cy+0.5, cxn-1.0, cyn-1.0 );
					cr->set_source_rgba( 1, 1, 1, 0.6 );
					cr->set_line_width(1);
					cr->stroke();
				}

				c++;
			}
		}
		if( m_Selection ) {
			// draw overlays
			m_rCursor->setVisible( m_PriColor != -1 );
			m_rSecondaryMarker->setVisible( m_SecColor != -1 );
		}
	}

	return ShapeDrawingArea::on_draw(cr);
}

bool ColorSelector::on_button_press_event(GdkEventButton* event)
{
	if( !m_pPalette ) return true;
	
	if( event->button == 1 ) {
		if( event->type == GDK_BUTTON_PRESS ) {
			m_PriColor = calcColor( event->x, event-> y );
			m_SecColor = -1;
			queue_draw();
			m_SignalColorClicked.emit(1);
			if( m_Selection ) m_Dragging = true;
		} else if( event->type == GDK_2BUTTON_PRESS ) {
			m_SignalColorDoubleClicked.emit();
		}
	} else if( event->button == 3 ) {
		m_SecColor = m_HovColor;
		queue_draw();
		m_SignalColorClicked.emit(3);
	}
	return true;	
}

bool ColorSelector::on_motion_notify_event(GdkEventMotion* event)
{
	if( !m_pPalette ) return true;
	
	int c = calcColor( event->x, event->y );

	if( c != m_HovColor ) {
		m_HovColor = c;
		queue_draw();
	}
	
	if( m_Dragging ) {
		if( m_SecColor != m_HovColor ) {
			m_SecColor = m_HovColor;
			m_SignalColorClicked.emit(3);
		}
	}

	return true;	
}

int ColorSelector::calcColor( int x, int y )
{
	const Gtk::Allocation& a = get_allocation();
	
	int cx = x / ( a.get_width()  / m_Width );
	int cy = y / ( a.get_height() / m_Height );
	int c = cy*m_Width+cx;
	
	return c >= m_pPalette->size() || c < 0 ? -1 : c;
}

bool ColorSelector::on_button_release_event(GdkEventButton* event)
{
	if( event->button == 1 )
		m_Dragging = false;
	return true;
}

bool ColorSelector::on_leave_notify_event(GdkEventCrossing* /*event*/)
{
	if( m_HovColor >= 0 ) {
		m_HovColor = -1;
		queue_draw();
	}
	return false;
}

} // namespace Polka 
