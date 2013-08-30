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

#ifndef _POLKA_SHAPEDRAWINGAREA_H_
#define _POLKA_SHAPEDRAWINGAREA_H_

#include <string>
#include <map>
#include <gtkmm/drawingarea.h>

namespace Polka {

class ShapeDrawingArea : public Gtk::DrawingArea
{
public:
	ShapeDrawingArea();
	virtual ~ShapeDrawingArea();

	class Shape;

	// coord space
	void setOffset( int x, int y );
	void setScale( int h, int v );
	void setSize( int width, int height );

	int dx() const;
	int dy() const;
	int width() const;
	int height() const;
	int hScale() const;
	int vScale() const;
	
	// shape access
	void add( int priority, const Cairo::RefPtr<Shape>& s );
	bool remove( int priority );
	Cairo::RefPtr<Shape> shape( int priority );

protected:
	bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );

private:
	// ordered map of overlay objects
	std::map<int, Cairo::RefPtr<Shape> > m_Shapes;
	int m_DX, m_DY, m_Width, m_Height, m_HScale, m_VScale;
};

// base class for shapes
class ShapeDrawingArea::Shape 
{
public:
	virtual ~Shape();

	// shape visibility
	void setVisible( bool value = true );
	bool isVisible() const;

	// all shapes have a position and size
	void setSize( int w, int h );
	void setLocation( int x, int y );
	void move( int x, int y );

	// access to position and size
	int x() const;
	int y() const;
	int width() const;
	int height() const;

	void redraw();

	// shortcut for updating the defined area with an optional border.
	// x, y, w, h are scaled but the border is not
	void update( int border = 0 );
	void updateAll();
	void updateArea(int x, int y, int w, int h);

protected:
	// no public constructors
	Shape();

	bool assigned() const;
	ShapeDrawingArea& parent() const;

	// implement redraw request in derived class
	virtual void requestUpdate() = 0;
	
	// drawing base
	virtual void drawShape( const Cairo::RefPtr<Cairo::Context>& cr ) = 0;
	
private:
	bool m_Visible;
	int	m_X, m_Y, m_W, m_H;
	ShapeDrawingArea *m_pParent;

	friend class ShapeDrawingArea;
	
	void setParent( ShapeDrawingArea *parent );
};

} // namespace Polka 

#endif // _POLKA_SHAPEDRAWINGAREA_H_

