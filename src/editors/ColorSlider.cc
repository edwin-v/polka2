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

#include "ColorSlider.h"
#include <cairomm/refptr.h>
#include <cairomm/context.h>
#include <iostream>

namespace Polka {


ColorSlider::ColorSlider( Gtk::Orientation orient )
	: m_Orientation( orient ), m_Changing(false), m_Changed(false)
{
	// create the slider adjustment object
	m_refAdj = Gtk::Adjustment::create( 3, 0, 7, 1, 1 );
	m_refAdj->signal_value_changed().connect( sigc::mem_fun(*this,
	                        &ColorSlider::onChange) );
	
	// init colors
	m_R0 = m_G0 = m_B0 = 0.2;
	m_R1 = m_G1 = m_B1 = 0.8;
	
	// listen to events
	add_events( Gdk::BUTTON_PRESS_MASK );
	add_events( Gdk::BUTTON_RELEASE_MASK );
	add_events( Gdk::BUTTON1_MOTION_MASK );
}

ColorSlider::~ColorSlider()
{
}

ColorSlider::SignalChanged ColorSlider::signalChanged()
{
	return m_SignalChanged;
}

ColorSlider::SignalChanging ColorSlider::signalChanging()
{
	return m_SignalChanging;
}

Glib::RefPtr<Gtk::Adjustment> ColorSlider::getAdjustment()
{
	return m_refAdj;
}

void ColorSlider::onChange()
{
	queue_draw();
	if( !m_Changing )
		m_SignalChanged.emit();
}

void ColorSlider::setRange( int upper )
{
	m_refAdj->set_upper( upper );
	queue_draw();
}

int ColorSlider::range() const
{
	return int(m_refAdj->get_upper());
}

int ColorSlider::value() const
{
	return int(m_refAdj->get_value());
}

void ColorSlider::setLowColor( double r, double g, double b )
{
	m_R0 = r;
	m_G0 = g;
	m_B0 = b;
	queue_draw();
}

void ColorSlider::setHighColor( double r, double g, double b )
{
	m_R1 = r;
	m_G1 = g;
	m_B1 = b;
	queue_draw();
}

bool ColorSlider::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	const Gtk::Allocation& a = get_allocation();

	// calculate sizes
	auto sc = get_style_context();
	auto border = sc->get_border();
	double wx = (double)a.get_width() - border.get_left() - border.get_right();
	double wy = (double)a.get_height() - border.get_top() - border.get_bottom();

	// adjust for orientation 
	if( m_Orientation == Gtk::ORIENTATION_HORIZONTAL ) {
		m_Delta = int( wx / (m_refAdj->get_upper() + 0.5) );
		m_StartX = a.get_width() - border.get_right() - int(m_refAdj->get_upper())*m_Delta;
		m_StartY = border.get_top();
	} else {
		m_Delta = int( wy / (m_refAdj->get_upper() + 0.5) );
		m_StartX = border.get_left();
		m_StartY = border.get_top() + int(m_refAdj->get_upper() - 1)*m_Delta;
	}

	cr->save();

	// paint the background
	cr->rectangle( border.get_left(), border.get_top(), wx, wy );
	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->fill();

	// draw the foreground
	cr->set_source_rgb(m_R1, m_G1, m_B1);

	for( int i = 1; i <= int(m_refAdj->get_upper()); i++ ) {
	
		// adjust the colour if needed
		if( i > int(m_refAdj->get_value()) )
			cr->set_source_rgb(m_R0, m_G0, m_B0);
		
		// draw a rectangle
		if( m_Orientation == Gtk::ORIENTATION_HORIZONTAL )
			cr->rectangle( m_StartX+m_Delta*(i-1)+1, m_StartY+1, m_Delta-2, wy-2 );
		else
			cr->rectangle( m_StartX+1, m_StartY-m_Delta*(i-1)+1, wx-2, m_Delta-2 );
		cr->fill();
		
		// increase the position
	}
	
	cr->restore();
	//get_style()->paint_shadow(window, Gtk::STATE_NORMAL, Gtk::SHADOW_IN, 
	//			   Gdk::Rectangle( event->area.x, event->area.y, event->area.width, event->area.height ),
	//			   *this, "frame", 0, 0, a.get_width(), a.get_height() );
	sc->context_save();
	sc->add_class( GTK_STYLE_CLASS_FRAME );
	sc->render_frame( cr, 0, 0, a.get_width(), a.get_height() );
	sc->context_restore();
	return true;
}

bool ColorSlider::on_button_press_event(GdkEventButton* event)
{
	if( event->button == 1 && is_sensitive() ) {
		m_Changing = true;
		m_Changed = false;
		m_ChangeStart = int(m_refAdj->get_value());
		changePosition( int(event->x), int(event->y) );
	}
	return true;	
}

bool ColorSlider::on_button_release_event(GdkEventButton* event)
{
	if( event->button == 1 && is_sensitive() ) {
		m_Changing = false;
		if( m_Changed ) {
			m_Changed = false;
			if( m_ChangeStart != int(m_refAdj->get_value()) )
				m_SignalChanged.emit();
		}
	}
	return true;
}

bool ColorSlider::on_motion_notify_event(GdkEventMotion *event)
{
	if( m_Changing && is_sensitive() )
		changePosition( int(event->x), int(event->y) );
	return true;
}
		
void ColorSlider::changePosition( int x, int y )
{
	int pos;
	// calculate the value under the mouse
	if( m_Orientation == Gtk::ORIENTATION_HORIZONTAL )
		pos = (x - m_StartX + m_Delta)/m_Delta;
	else
		pos = (m_StartY + 2*m_Delta - y)/m_Delta;

	// update if changed
	if( pos != int(m_refAdj->get_value()) ) {
		m_refAdj->set_value(pos);
		m_Changed = true;
		m_SignalChanging.emit();
	}
}

} // namespace Polka
