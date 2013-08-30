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

#include "ShapeDrawingArea.h"
#include "Brush.h"


namespace Polka {


ShapeDrawingArea::ShapeDrawingArea()
{
	// default coordspace
	m_DX = m_DY = 0;
	m_Width = m_Height = 0;	// default to allocated size
	m_HScale = m_VScale = 1;
}

ShapeDrawingArea::~ShapeDrawingArea()
{
}

void ShapeDrawingArea::setOffset( int x, int y )
{
	m_DX = x;
	m_DY = y;
}

void ShapeDrawingArea::setScale( int h, int v )
{
	m_HScale = h;
	m_VScale = v;
}

void ShapeDrawingArea::setSize( int width, int height )
{
	m_Width = width;
	m_Height = height;
}

int ShapeDrawingArea::dx() const
{
	return m_DX;
}

int ShapeDrawingArea::dy() const
{
	return m_DY;
}

int ShapeDrawingArea::width() const
{
	if( m_Width > 0 )
		return m_Width;
	else
		return get_width();
}

int ShapeDrawingArea::height() const
{
	if( m_Height > 0 )
		return m_Height;
	else
		return get_height();
}

int ShapeDrawingArea::hScale() const
{
	return m_HScale;
}

int ShapeDrawingArea::vScale() const
{
	return m_VScale;
}


Cairo::RefPtr<ShapeDrawingArea::Shape> ShapeDrawingArea::shape( int priority )
{
	Cairo::RefPtr<Shape> r;
	auto shape = m_Shapes.find( priority );
	if( shape != m_Shapes.end() ) r = shape->second;
	
	return r;
}

void ShapeDrawingArea::add( int priority, const Cairo::RefPtr<Shape>& s )
{
	m_Shapes[priority] = s;
	s->setParent(this);
}

bool ShapeDrawingArea::remove( int priority )
{
	auto shape = m_Shapes.find( priority );
	if( shape == m_Shapes.end() ) return false;
	
	// queue redraw
	shape->second->requestUpdate();
	// unlink shape	
	shape->second->setParent(0);
	m_Shapes.erase( shape );
	return true;
}

bool ShapeDrawingArea::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// translate image to offset
	cr->translate( -m_DX, -m_DY );
	
	auto it = m_Shapes.begin();
	while( it != m_Shapes.end() ) {
		if( it->second->isVisible() ) {
			cr->save();
			it->second->drawShape(cr);
			cr->restore();
		}
		++it;
	}
	
	return true;
}

/*
 * Shape implementation
 */

ShapeDrawingArea::Shape::Shape()
	: m_Visible(false), m_X(0), m_Y(0), m_W(0), m_H(0), m_pParent(0)
{
}
	
ShapeDrawingArea::Shape::~Shape()
{
}
		
void ShapeDrawingArea::Shape::setVisible( bool value )
{
	if( m_Visible != value ) {
		m_Visible = value;
		requestUpdate();
	}
}

bool ShapeDrawingArea::Shape::isVisible() const
{
	return m_Visible;
}

void ShapeDrawingArea::Shape::setSize( int w, int h )
{
	if( m_W != w ||  m_H != h ) {
		requestUpdate();
		m_W = w;
		m_H = h;
		requestUpdate();
	}
}

void ShapeDrawingArea::Shape::setLocation( int x, int y )
{
	if( m_X != x ||  m_Y != y ) {
		requestUpdate();
		m_X = x;
		m_Y = y;
		requestUpdate();
	}
}

void ShapeDrawingArea::Shape::move( int x, int y )
{
	if( m_X != x ||  m_Y != y ) {
		requestUpdate();
		m_X += x;
		m_Y += y;
		requestUpdate();
	}
}

int ShapeDrawingArea::Shape::x() const
{
	return m_X;
}

int ShapeDrawingArea::Shape::y() const
{
	return m_Y;
}

int ShapeDrawingArea::Shape::width() const
{
	return m_W;
}

int ShapeDrawingArea::Shape::height() const
{
	return m_H;
}

bool ShapeDrawingArea::Shape::assigned() const
{
	return m_pParent != 0;
}

ShapeDrawingArea& ShapeDrawingArea::Shape::parent() const
{
	return *m_pParent;
}

void ShapeDrawingArea::Shape::setParent( ShapeDrawingArea *parent )
{
	m_pParent = parent;
	requestUpdate();
}

void ShapeDrawingArea::Shape::redraw()
{
	requestUpdate();
}

void ShapeDrawingArea::Shape::update( int border )
{
	if( m_pParent && m_pParent->get_is_drawable() ) 
		m_pParent->queue_draw_area( m_X * m_pParent->hScale() - m_pParent->dx() - border, 
		                            m_Y * m_pParent->vScale() - m_pParent->dy() - border,
		                            m_W * m_pParent->hScale() + 2*border, m_H * m_pParent->vScale() + 2*border );
}

void ShapeDrawingArea::Shape::updateAll()
{
	if( m_pParent && m_pParent->get_is_drawable() )
		m_pParent->queue_draw();
}

void ShapeDrawingArea::Shape::updateArea(int x, int y, int w, int h)
{
	if( m_pParent && m_pParent->get_is_drawable() )
		m_pParent->queue_draw_area(x - m_pParent->dx() , y - m_pParent->dy() , w, h);
}

} // namespace Polka 
