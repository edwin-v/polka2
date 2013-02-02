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

#ifndef _POLKA_BRUSH_H_
#define _POLKA_BRUSH_H_

#include "Pen.h"
#include <cairomm/surface.h>

namespace Polka {

static const int NOOFFS = -100000;

class Palette;
class Shape;

class Brush : public Pen
{
public:
	Brush( int width, int height, int offsetx = NOOFFS, int offsety = NOOFFS );
	virtual ~Brush();
	
	virtual const int *data() const;
	virtual int *data();

	// set data
	virtual void setColor( int col );
	virtual void setData( const int *data, int trans_col = -1 );
	virtual void setTransparentColor( int col );
	
	// create image surface
	Cairo::RefPtr<Cairo::ImageSurface> getImage( const Palette& pal );

	Shape *convertToShape();
	void flip( bool vertical = false );
	void rotate( bool ccw = false );

protected:
	int *m_Data;
	int m_TransparentColor;
	Cairo::RefPtr<Cairo::ImageSurface> m_refImage;
	const Palette *m_pRefPal;
};

class Shape : public Brush
{
public:
	Shape( int width, int height, int offsetx = NOOFFS, int offsety = NOOFFS );
	virtual ~Shape();
	
	// recolor
	virtual void setColor( int col );
	virtual void setTransparentColor( int col );
};


} // namespace Polka

#endif // _POLKA_BRUSH_H_
