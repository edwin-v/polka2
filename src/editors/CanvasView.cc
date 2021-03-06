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

#include "CanvasView.h"
#include "Canvas.h"
#include "Palette.h"
#include <iostream>
using namespace std;
namespace Polka {

CanvasView::CanvasView( const std::string& _id )
	: AccelBase(_id), m_pCanvas(0), m_Dragging(false), m_ViewLocked(false)
{
	add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::BUTTON2_MOTION_MASK | Gdk::SCROLL_MASK);

	// add pixel grid
	m_rPixelGrid = GridShape::create();
	m_rPixelGrid->setSize(1,1);
	add(0, m_rPixelGrid);
	changeGrid(0);
	// add tile grid
	m_rTileGrid = GridShape::create();
	add(1, m_rTileGrid);
	changeGrid(1);

	m_GridSelect.signalPixelGridChanged().connect( sigc::bind<int>(sigc::mem_fun(*this, &CanvasView::changeGrid), 0) );
	m_GridSelect.signalTileGridChanged().connect( sigc::bind<int>(sigc::mem_fun(*this, &CanvasView::changeGrid), 1) );
}

CanvasView::~CanvasView()
{
}

void CanvasView::setCanvas( Canvas *canvas )
{
	m_pCanvas = canvas;
	m_Dragging = false;
	m_ViewLocked = false;
	queue_draw();
}

bool CanvasView::hasCanvas() const
{
	return m_pCanvas != 0;
}

Canvas& CanvasView::canvas()
{
	return *m_pCanvas;
}

GridSelector& CanvasView::gridSelector()
{
	return m_GridSelect;
}

int CanvasView::scale() const
{
	if( m_pCanvas )
		return m_pCanvas->viewScale();
	else
		return 1;
}

int CanvasView::hscale() const
{
	if( m_pCanvas )
		return m_pCanvas->viewScale()*m_pCanvas->pixelScaleHor();
	else
		return 1;
}

int CanvasView::vscale() const
{
	if( m_pCanvas )
		return m_pCanvas->viewScale()*m_pCanvas->pixelScaleVer();
	else
		return 1;
}

int CanvasView::dx() const
{
	if( m_pCanvas )
		return m_pCanvas->viewHorOffset();
	else
		return 0;
}

int CanvasView::dy() const
{
	if( m_pCanvas )
		return m_pCanvas->viewVerOffset();
	else
		return 0;
}

void CanvasView::changeGrid( int id )
{
	GridSelector::Type t = id==0 ? m_GridSelect.pixelGrid() : m_GridSelect.tileGrid();
	Cairo::RefPtr<GridShape> grid = id==0 ? m_rPixelGrid : m_rTileGrid;
	
	switch( t ) {
		case GridSelector::GRID_DOT:
			grid->setType( GridShape::GRID_DOTS );
			grid->setBaseWidth( 1 );
			grid->setBaseColor( 0, 0, 0 );
			grid->setLineWidth( 1 );
			grid->setLineColor( 1, 1, 1 );
			grid->setVisible();
			break;
		case GridSelector::GRID_SHADE:
			grid->setType( GridShape::GRID_SHADES );
			grid->setBaseWidth( 1 );
			grid->setLineWidth( 1 );
			if( id == 0 ) {
				grid->setBaseColor( 0, 0, 0, 0.2 );
				grid->setLineColor( 1, 1, 1, 0.1 );
			} else {
				grid->setBaseColor( 0, 0, 0, 0.3 );
				grid->setLineColor( 1, 1, 1, 0.15 );
			}
			grid->setVisible();
			break;
		default:
			grid->setVisible(false);
			break;
	}
	
	queue_draw();
}

void CanvasView::canvasChanged( const Gdk::Rectangle& r )
{
	if( r.has_zero_area() ) return;

	// calculate partial update rectangle from pixel coords
	queue_draw_area( r.get_x() * hscale() - dx(), r.get_y() * vscale() - dy(),
	                 r.get_width() * hscale()   , r.get_height() * vscale() );
}

void CanvasView::changeCursor( Glib::RefPtr<Gdk::Cursor> cursor )
{
	if( !cursor ) cursor = Gdk::Cursor::create(Gdk::FLEUR);
	get_window()->set_cursor(cursor);
}

void CanvasView::restoreCursor()
{
	get_window()->set_cursor();
}

bool CanvasView::on_scroll_event(GdkEventScroll* event)
{
	if( m_ViewLocked || !hasCanvas() ) return false;
	
	int sc = scale(), ox = dx(), oy = dy();
	if(event->direction == GDK_SCROLL_UP) {
		if( scale() < 32 ) {
			sc<<=1;
			// zoom in on spot
			ox = (event->x + ox)*2 - event->x;
			oy = (event->y + oy)*2 - event->y;
		}
	} else if(event->direction == GDK_SCROLL_DOWN) {
		if( sc > 1 ) {
			sc>>=1;
			// zoom out on spot
			ox = (event->x + ox)*0.5 - event->x;
			oy = (event->y + oy)*0.5 - event->y;
		}
	}
	// correct position
	m_pCanvas->setViewScale(sc);
	clipDeltas( ox, oy );
	m_pCanvas->setViewOffset(ox, oy);
	
	queue_draw();
	return true;
}

bool CanvasView::on_button_press_event(GdkEventButton *event)
{
	if( m_ViewLocked || !hasCanvas() ) return false;

	// handle middle button drag start
	if( !m_Dragging ) {
		if( isAccel( ACC_PAN, event->button, 0, event->state)) {
			m_Dragging = true;
			m_DragFromX = event->x;
			m_DragFromY = event->y;
			// change cursor to drag
			changeCursor();
		}
	}
	return true;	
	
}

bool CanvasView::on_motion_notify_event(GdkEventMotion* event)
{
	if( m_ViewLocked || !hasCanvas() ) return false;

	if( m_Dragging ) {
		
		// move offset
		int ex = round(event->x), ey = round(event->y);   // due to gtk producting occasional fractional parts
		int ox = dx() - (ex - m_DragFromX);
		m_DragFromX = ex;
		int oy = dy() - (ey - m_DragFromY);
		m_DragFromY = ey;

		clipDeltas( ox, oy, true );
		m_pCanvas->setViewOffset( ox, oy );

		queue_draw();
	}
	return true;
}

bool CanvasView::on_button_release_event(GdkEventButton *event)
{
	if( m_ViewLocked || !hasCanvas() ) return false;

	// handle middle button drag end
	if( m_Dragging && isAccel( ACC_PAN, event->button, 0, event->state) ) {
		queue_draw();
		m_Dragging = false;
		restoreCursor();
	}
	return true;	
	
}

void CanvasView::clipDeltas( int& ox, int& oy, bool adjust_drag )
{
	// scales
	int hsc = hscale(), vsc = vscale();
	// correct offset
	const Gtk::Allocation& a = get_allocation();
	if( hsc*m_pCanvas->width() - ox < 16 ) {
		if( adjust_drag )
			m_DragFromX -= hsc*m_pCanvas->width() - ox - 16;
		ox = hsc*m_pCanvas->width() - 16;
	} else if( a.get_width() + ox < 16 ) {
		if( adjust_drag )
			m_DragFromX += a.get_width() + ox - 16;
		ox = -a.get_width() + 16;
	}
	if( vsc*m_pCanvas->height() - oy < 16 ) {
		if( adjust_drag )
			m_DragFromY -= vsc*m_pCanvas->height() - oy - 16;
		oy = vsc*m_pCanvas->height() - 16;
	} else if( a.get_height() + oy < 16 ) {
		if( adjust_drag )
			m_DragFromY += a.get_height() + oy - 16;
		oy = -a.get_height() + 16;
	}
}

void CanvasView::setFastUpdate( bool fast )
{
	if( fast ) {
		// disable slow drawing features
		if( m_GridSelect.pixelGrid() == GridSelector::GRID_SHADE )
			m_rPixelGrid->setVisible(false);
	} else {
		// reset features
		if( m_GridSelect.pixelGrid() != GridSelector::GRID_OFF )
			m_rPixelGrid->setVisible();
		if( m_GridSelect.tileGrid() != GridSelector::GRID_OFF )
			m_rTileGrid->setVisible();
	}
}

bool CanvasView::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// clip to content and get area
	//double x1, y1, x2, y2;
	//cr->get_clip_extents( x1, y1, x2, y2 );

	if( !m_pCanvas ) return true;
		
	// paint the background
	cr->save();
	cr->translate( -dx(), -dy() );
	cr->scale( hscale(), vscale() );
	cr->rectangle( 0, 0, m_pCanvas->width(), m_pCanvas->height() );
	Cairo::RefPtr<Cairo::SurfacePattern> sp = Cairo::SurfacePattern::create( m_pCanvas->getImage() );
	sp->set_filter(Cairo::FILTER_FAST);
	cr->set_source(sp);
	//cr->rectangle(event->area.x + double(m_DX)/m_Scale, event->area.y + double(m_DY)/m_Scale,
	//              event->area.width, event->area.height);
	cr->fill();

	// reset scaling for drawing cursors		
	cr->restore();

	// set coord space
	setSize( m_pCanvas->width(), m_pCanvas->height() );
	setOffset( dx(), dy() );
	setScale( hscale(), vscale() );

	// set tile grid size
	m_rTileGrid->setSize( m_pCanvas->tileGridWidth(), m_pCanvas->tileGridHeight() );
	m_rTileGrid->setLocation( m_pCanvas->tileGridHorOffset(), m_pCanvas->tileGridVerOffset() );

	ShapeDrawingArea::on_draw(cr);
	
	return true;
}

void CanvasView::lockView( int ox, int oy, int sc )
{
	if( !m_ViewLocked ) {
		m_UnlockDX = dx();
		m_UnlockDY = dy();
		m_UnlockScale = scale();
	}
	
	m_ViewLocked = true;
	
	m_pCanvas->setViewScale(sc);
	m_pCanvas->setViewOffset( ox, oy );

	queue_draw();
}

void CanvasView::unlockView()
{
	m_ViewLocked = false;

	m_pCanvas->setViewScale(m_UnlockScale);
	m_pCanvas->setViewOffset( m_UnlockDX, m_UnlockDY );
	
	queue_draw();
}

} // namespace Polka
