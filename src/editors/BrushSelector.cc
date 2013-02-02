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

#include "BrushSelector.h"
#include "Brush.h"
#include <iostream>
#include <iomanip>

namespace Polka {

BrushSelector::BrushSelector()
	: m_SelectId(-1), m_HoverId(-1), m_pBrushes(0)
{
	add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK);
	
	set_size_request( 48, 100 );
}

BrushSelector::~BrushSelector()
{
}

void BrushSelector::setBrushVector( const std::vector<Brush*>& brushvec )
{
	m_pBrushes = &brushvec;
	regenerate();
	selectBrush(0);
	queue_draw();
}

BrushSelector::SignalBrushSelected BrushSelector::signalBrushSelected()
{
	return m_SignalBrushSelected;
}

void BrushSelector::selectBrush( int id )
{
	if( !m_pBrushes ) return;
	
	if( id < 0 )
		id = -1;
	else if( id >= int(m_pBrushes->size()) )
		id = m_pBrushes->size()-1;
	
	if( m_SelectId != id ) {
		m_SelectId = id;
		m_SignalBrushSelected.emit(id);
	}
}

void BrushSelector::regenerate()
{
	int w = get_width()/2+1;
	
	// reset coords
	m_X.clear();
	m_Y.clear();
	
	int lineWidth = 0;
	int lineStart = 0;
	int lineHeight = 0;
	int totHeight = 0;
	int maxWidth = w;
	bool last;
	for( guint i = 0; i <= m_pBrushes->size(); ++i ) {
		last = i==m_pBrushes->size();
		// check if line fits
		if( last || lineWidth + (*m_pBrushes)[i]->width()+4 >= w ) {
			// correct vertical position for this line
			for( guint j = lineStart; j < i; ++j ) {
				m_Y.push_back( totHeight + (lineHeight+4-(*m_pBrushes)[j]->height())/2 );
			}
			totHeight += lineHeight+4;
			if( lineWidth > maxWidth ) maxWidth = lineWidth;
			lineWidth = lineHeight = 0;
			lineStart = i;
		}
		if( last ) break;
		
		// add to line
		m_X.push_back( lineWidth+2 );
		lineWidth += (*m_pBrushes)[i]->width()+4;
		if( (*m_pBrushes)[i]->height() > lineHeight )
			lineHeight = (*m_pBrushes)[i]->height();
	}

	// create new image
	int h = get_height()/2+1;
	if( totHeight>h ) h = totHeight;
	m_Image = Cairo::ImageSurface::create( Cairo::FORMAT_RGB24, maxWidth, h );
	// fill with bg pattern
	for( int y = 0; y < m_Image->get_height(); y++ ) {
		char *dat = (char *)(m_Image->get_data() + m_Image->get_stride()*y);
		for( int x = 0; x < m_Image->get_width(); x++ ) {
			char c = 192 + 32*(((x>>3)+(y>>3))&1);
			*dat++ = c;
			*dat++ = c;
			*dat++ = c;
			dat++;
		}
	}
	
	// draw brushes
	for( guint i = 0; i < m_pBrushes->size(); ++i ) { std::cout << "BrushXY " << i << "/" << maxWidth << "px: " << m_X[i] << ", " << m_Y[i] << std::endl;
		Brush *bsh = (*m_pBrushes)[i];
		Shape *shp = dynamic_cast<Shape*>(bsh);
		const int *bdata = bsh->data();
		for( int y = 0; y < bsh->height(); y++ ) {
			// calculate start point
			guint *idata = (guint*)(m_Image->get_data() + (m_Y[i]+y)*m_Image->get_stride() + 4*m_X[i]);
			for( int x = 0; x < bsh->height(); x++ ) {
				if( *bdata != -1 ) {
					if( shp ) {
						// use black for shapes
						*idata = 0;
					} else {
						*idata = *bdata; // xxxxxxxxxxxxxxxxxxxxxxxxxxx PALETTE
					}
				}
				++idata;
				++bdata;
			}
		}
	}
}

bool BrushSelector::on_button_press_event(GdkEventButton *event)
{
	if( event->button == 1 && m_HoverId >= 0 )
		if( m_HoverId >= 0 ) 
			selectBrush(m_HoverId);
	return true;
}

bool BrushSelector::on_button_release_event(GdkEventButton *event)
{
	return false;
}

bool BrushSelector::on_motion_notify_event(GdkEventMotion* event)
{
	if( event->state & 0x0F00 ) {
		m_HoverId = -1;
		queue_draw();
		return true;
	}

	for( guint i = 0; i < m_pBrushes->size(); ++i ) {
		Brush *b = (*m_pBrushes)[i];
		// check horizontal
		if( event->x/2 >= m_X[i]-2 && event->x/2 < m_X[i]+b->width()+2 )
			if( event->y/2 >= m_Y[i]-2 && event->y/2 < m_Y[i]+b->height()+2 ) {
				// got one
				if( m_HoverId != int(i) ) {
					m_HoverId = i;
					queue_draw();
				}
				return true;
					
			}
	}
	if( m_HoverId >= 0 ) {
		m_HoverId = -1;
		queue_draw();
	}		
	return true;
}

bool BrushSelector::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// draw normal stuff
	//Gtk::DrawingArea::on_draw(cr);
	
	if( m_Image ) {
		if( m_Image->get_width() != get_width()/2+1 ) regenerate();

		// create area
		cr->scale(2, 2);
		// fill with image
		Cairo::RefPtr<Cairo::SurfacePattern> sp = Cairo::SurfacePattern::create( m_Image );
		sp->set_filter(Cairo::FILTER_FAST);
		cr->set_source(sp);
		cr->paint();
		
		// draw hover rect
		cr->set_line_width(1.0);
		if( m_HoverId >= 0 ) {
			cr->rectangle( m_X[m_HoverId]-1.5, m_Y[m_HoverId]-1.5, (*m_pBrushes)[m_HoverId]->width()+3, (*m_pBrushes)[m_HoverId]->height()+3 );
			cr->set_source_rgb( 0.5, 0.5, 0.5 );
			cr->stroke();
		}
		// draw hover rect
		if( m_SelectId >= 0 ) {
			cr->rectangle( m_X[m_SelectId]-1.5, m_Y[m_SelectId]-1.5, (*m_pBrushes)[m_SelectId]->width()+3, (*m_pBrushes)[m_SelectId]->height()+3 );
			cr->set_source_rgb( 0, 0, 0 );
			cr->stroke();
		}
			
	}
	return true;
}


} // namespace Polka 
