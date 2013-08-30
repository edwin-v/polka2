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

#include "ShapeDrawingObjects.h"
#include "Brush.h"


namespace Polka {

/* -------------------------------
 * ImageShape
 * -------------------------------
 */

ImageShape::ImageShape()
	: m_Smooth(false)
{
}

Cairo::RefPtr<ImageShape> ImageShape::create()
{
	return Cairo::RefPtr<ImageShape>( new ImageShape );
}


void ImageShape::setImage( const Cairo::RefPtr<Cairo::ImageSurface>& image )
{
	m_Image = image;
	update();
}

Cairo::RefPtr<Cairo::ImageSurface> ImageShape::image() const
{
	return m_Image;
}

void ImageShape::setSmoothScaling( bool val )
{
	if( val != m_Smooth ) {
		m_Smooth = val;
		update();
	}
}

bool ImageShape::smoothScaling() const
{
	return m_Smooth;
}

void ImageShape::requestUpdate()
{
	update();
}

void ImageShape::drawShape( const Cairo::RefPtr<Cairo::Context>& cr )
{
	if( m_Image ) {
	
		// set global scale
		cr->scale( parent().hScale(), parent().vScale() );
		// translate to image origin
		cr->translate( -x(), -y() );
		
		if( width() > 0 && height() > 0 ) {
			// scale to target size
			cr->scale( double(width()) / m_Image->get_width(), double(height()) / m_Image->get_height() );
		}
		
		cr->rectangle( 0, 0, m_Image->get_width(), m_Image->get_height() );
		Cairo::RefPtr<Cairo::SurfacePattern> sp = Cairo::SurfacePattern::create( m_Image );
		if( !m_Smooth ) 
			sp->set_filter(Cairo::FILTER_FAST);
		cr->set_source(sp);
		cr->fill();
		
	}
}



/* 
 * LineShapeBase
 */

LineShapeBase::LineShapeBase()
	: m_BaseWidth(1), m_LineWidth(1),
	  m_BaseR(0), m_BaseG(0), m_BaseB(0), m_BaseA(1), 
	  m_LineR(1), m_LineG(1), m_LineB(1), m_LineA(1),
	  m_BaseDash(0), m_LineDash(4)
{
}
	
void LineShapeBase::setBaseWidth( int width )
{
	if( m_BaseWidth > width ) requestUpdate();
	m_BaseWidth = width;
	requestUpdate();
}

void LineShapeBase::setBaseColor( double r, double g, double b, double a )
{
	m_BaseR = r;
	m_BaseG = g;
	m_BaseB = b;
	m_BaseA = a;
	requestUpdate();
}

void LineShapeBase::setBaseDashSize( int dash )
{
	m_BaseDash = dash;
	requestUpdate();
}

int LineShapeBase::baseWidth() const
{
	return m_BaseWidth;
}

double LineShapeBase::baseR() const
{
	return m_BaseR;
}

double LineShapeBase::baseG() const
{
	return m_BaseG;
}

double LineShapeBase::baseB() const
{
	return m_BaseB;
}

double LineShapeBase::baseAlpha() const
{
	return m_BaseA;
}

int LineShapeBase::baseDashSize() const
{
	return m_BaseDash;
}

void LineShapeBase::applyBaseColor( const Cairo::RefPtr<Cairo::Context>& cr ) const
{
	cr->set_source_rgba(m_BaseR, m_BaseG, m_BaseB, m_BaseA);
}

void LineShapeBase::applyBaseDash( const Cairo::RefPtr<Cairo::Context>& cr ) const
{
	if( m_BaseDash > 0 ) {
		std::vector<double> p({double(m_BaseDash)});
		cr->set_dash( p, 0 );
	} else {
		cr->unset_dash();
	}
}

void LineShapeBase::setLineWidth( int width )
{
	if( m_LineWidth > width ) requestUpdate();
	m_LineWidth = width;
	requestUpdate();
}

void LineShapeBase::setLineColor( double r, double g, double b, double a )
{
	m_LineR = r;
	m_LineG = g;
	m_LineB = b;
	m_LineA = a;
	requestUpdate();
}

void LineShapeBase::setLineDashSize( int dash )
{
	m_LineDash = dash;
	requestUpdate();
}
	
int LineShapeBase::lineWidth() const
{
	return m_LineWidth;
}

double LineShapeBase::lineR() const
{
	return m_LineR;
}

double LineShapeBase::lineG() const
{
	return m_LineG;
}

double LineShapeBase::lineB() const
{
	return m_LineB;
}

double LineShapeBase::lineAlpha() const
{
	return m_LineA;
}

int LineShapeBase::lineDashSize() const
{
	return m_LineDash;
}

void LineShapeBase::applyLineColor( const Cairo::RefPtr<Cairo::Context>& cr ) const
{
	cr->set_source_rgba(m_LineR, m_LineG, m_LineB, m_LineA);
}

void LineShapeBase::applyLineDash( const Cairo::RefPtr<Cairo::Context>& cr ) const
{
	if( m_LineDash > 0 ) {
		std::vector<double> p({double(m_LineDash)});
		cr->set_dash( p, 0 );
	} else {
		cr->unset_dash();
	}
}


/*
 * Rectangle object for ShapeDrawingArea
 * 
 *   Draws a rectangle over/around pixels
 */

RectangleShape::RectangleShape( Type t )
	: m_Type(t)
{
}

Cairo::RefPtr<RectangleShape> RectangleShape::create( Type t )
{
	return Cairo::RefPtr<RectangleShape>( new RectangleShape(t) );
}
	
void RectangleShape::setType( Type t )
{
	m_Type = t;
}


RectangleShape::Type RectangleShape::type() const
{
	return m_Type;
}

void RectangleShape::requestUpdate()
{
	if( assigned() ) {
		double offx, offy;
		calcOffsets( offx, offy );
		update( ceil( std::max(-offx, -offy) + 0.5*std::max( baseWidth(), lineWidth() ) ) );
	}
}

void RectangleShape::calcOffsets( double& ox, double& oy )
{
	switch( m_Type ) {
		case LINE_INSIDE:
			ox = oy = 0.5*lineWidth();
			break;
		case LINE_OUTSIDE:
			ox = oy = -0.5*lineWidth();
			break;
		case FULL_INSIDE:
			ox = oy = 0.5*baseWidth();
			break;
		case FULL_OUTSIDE:
			ox = oy = -0.5*baseWidth();
			break;
		default:
			ox = 0.5*parent().hScale();
			oy = 0.5*parent().vScale();
			break;
	}
}

void RectangleShape::drawShape( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// no antialiasing
	cr->set_antialias( Cairo::ANTIALIAS_NONE );

	// pixel rect offsets
	int hsc = parent().hScale(), vsc = parent().vScale();
	double offx, offy;
	calcOffsets( offx, offy );
	
	// draw rectangle with edge
	cr->rectangle( offx + x()*hsc, offy + y()*vsc, -2*offx + width()*hsc, -2*offy + height()*vsc );

	// draw line base
	if( baseWidth() > 0 ) {
		cr->set_line_width( baseWidth() );
		applyBaseColor(cr);
		applyBaseDash(cr);
		cr->stroke_preserve();
	}

	// draw line
	cr->set_line_width( lineWidth() );
	applyLineColor(cr);
	applyLineDash(cr);
	cr->stroke();
}



/*
 * GridShape object for ShapeDrawingArea
 * 
 * Implements 'dot', 'line' and 'shade' grids.
 */

GridShape::GridShape( GridType type )
	: m_Type(type)
{
	setSize( 1, 1 );
}


void GridShape::setType( GridType type )
{
	m_Type = type;
	requestUpdate();
}

GridShape::GridType GridShape::type() const
{
	return m_Type;	
}

Cairo::RefPtr<GridShape> GridShape::create( GridType type )
{
	return Cairo::RefPtr<GridShape>( new GridShape( type ) );
}

void GridShape::requestUpdate()
{
	updateAll();
}

void GridShape::drawShape( const Cairo::RefPtr<Cairo::Context>& cr )
{
	double x1, y1, x2, y2;

	// shortcuts
	int hsc = parent().hScale();
	int vsc = parent().vScale();

	// clip to content and get area
	cr->rectangle( 0, 0, hsc*parent().width(), vsc*parent().height() );
	cr->clip();
	cr->get_clip_extents(x1, y1, x2, y2);
	// no antialiasing
	cr->set_antialias( Cairo::ANTIALIAS_NONE );

	// grid shortcuts
	int gsx = width();
	int gsy = height();
	int gox = x();
	int goy = y();
	// calc smallest scale
	int sc = std::min(hsc*gsx, vsc*gsy);

	if( m_Type == GRID_LINES ) {
		int maxt = std::max(baseWidth(), lineWidth());
		// draw double lines over boundaries
		if( sc > maxt+1 ) {
			// first draw base line
			if( baseWidth() > 0 ) {
				cr->set_line_width( baseWidth() );
				applyBaseColor(cr);
				applyBaseDash(cr);
				// horizontal lines
				if( gsy > 0 ) {
					double y = (floor((y1-ceil(0.5*maxt) - goy) / (gsy*vsc)) * gsy + goy) * vsc;
					while( y <= y2 ) {
						cr->move_to( x1, y );
						cr->line_to( x2, y );
						y += gsy*vsc;
					}
				}
				// vertical lines
				if( gsx > 0 ) {
					double x = (floor((x1-ceil(0.5*maxt) - gox) / (gsx*hsc)) * gsx + gox) * hsc;
					while( x <= x2 ) {
						cr->move_to( x, y1 );
						cr->line_to( x, y2 );
						x += gsx*hsc;
					}
				}
				// stroke lines
				cr->stroke_preserve();
			}
			// always draw primary pen
			cr->set_line_width( lineWidth() );
			applyLineColor(cr);
			applyLineDash(cr);
			// stroke lines
			cr->stroke();
		}
	} else if( m_Type == GRID_SHADES ) {
		// shades are slow, start at 5 pixels
		if( sc > 4 ) {
			cr->set_line_width(1.0);
			// brightening lines top/left
			cr->set_source_rgba(1, 1, 1, lineAlpha());
			// draw horizontal bright shades
			if( gsy > 0 ) {
				double y = 0.5 + (floor((y1 - goy) / (gsy*vsc)) * gsy + goy) * vsc;
				while( y <= y2 ) {
					cr->move_to( x1, y );
					cr->line_to( x2, y );
					y += gsy*vsc;
					
				}
			}
			// draw vertical bright shades
			if( gsx > 0 ) {
				double x = 0.5 + (floor((x1 - gox) / (gsx*hsc)) * gsx + gox) * hsc;
				while( x <= x2 ) {
					cr->move_to( x, y1 );
					cr->line_to( x, y2 );
					x += gsx*hsc;
				}
			}
			cr->stroke();
			// darkening lines bottom/right
			cr->set_source_rgba(0, 0, 0, baseAlpha());
			// draw horizontal bright shades
			if( gsy > 0 ) {
				double y = -0.5 + (floor((y1-1 - goy) / (gsy*vsc)) * gsy + goy) * vsc;
				while( y <= y2 ) {
					cr->move_to( x1, y );
					cr->line_to( x2, y );
					y += gsy*vsc;
				}
			}
			// draw vertical bright shades
			if( gsx > 0 ) {
				double x = -0.5 + (floor((x1-1 - gox) / (gsx*hsc)) * gsx + gox) * hsc;
				while( x <= x2 ) {
					cr->move_to( x, y1 );
					cr->line_to( x, y2 );
					x += gsx*hsc;
				}
			}
			cr->stroke();
		}
	} else { /* GRID_DOTS */
		// minimum size for dots is 3
		if( sc > 2 ) {
			// set dot pattern
			std::vector<double> p({1.0, vsc*gsy-1.0});
			cr->set_dash( p, 0 );
			cr->set_line_width( baseWidth() );
			applyBaseColor(cr);
			// coloured dots in top left tile
			for( double x = gox*hsc+0.5; x < x2; x += gsx*hsc ) {
				cr->move_to(x, 0);
				cr->line_to(x, parent().height()*vsc);
			}
			cr->stroke();
			// black dots in bottom right tile
			cr->set_dash( p, 1.0 );
			cr->set_line_width( lineWidth() );
			applyLineColor(cr);
			for( double x = gox*hsc-0.5; x < x2; x += gsx*hsc ) {
				if( x < 0 ) continue;
				cr->move_to(x, 0);
				cr->line_to(x, parent().height()*vsc);
			}
			cr->stroke();
			// reset
			cr->unset_dash();
		}
	}
}




/*
 * LineShape object for ShapeDrawingArea
 */

LineShape::LineShape()
	: m_X(0), m_Y(0), m_W(1), m_H(1)
{
}

Cairo::RefPtr<LineShape> LineShape::create()
{
	return Cairo::RefPtr<LineShape>( new LineShape() );
}

void LineShape::requestUpdate()
{
	if( assigned() ) {
		// shortcuts
		int hsc = parent().hScale();
		int vsc = parent().hScale();

		// size can be negative!
		updateArea( (std::min( x()+width(), x() )) * hsc, 
					(std::min( y()+height(), y() )) * vsc,
					abs(width()) * hsc + hsc, abs(height()) * vsc + vsc);
	}
}

// Line drawing
void LineShape::drawShape( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// no antialiasing
	cr->set_antialias( Cairo::ANTIALIAS_NONE );

	// shortcuts
	int hsc = parent().hScale();
	int vsc = parent().hScale();

	// draw double line
	cr->move_to( (x()+0.5)*hsc, (y()+0.5)*vsc );
	cr->rel_line_to( width()*hsc, height()*vsc );
	if( baseWidth() > 0 ) {
		cr->set_line_width( baseWidth() );
		applyBaseColor(cr);
		applyBaseDash(cr);
		cr->stroke_preserve();
	}
	cr->set_line_width( lineWidth() );
	applyLineColor(cr);
	applyLineDash(cr);
	cr->stroke();
}



/*
 * BrushShape object for ShapeDrawingArea
 */

BrushShape::BrushShape( bool show_brush, bool show_outline, OutlineType type )
	: m_X(0), m_Y(0),
	  m_ShowBrush(show_brush), m_ShowOutline(show_outline), m_Outline(type),
	  m_pBrush(0)
{
}

Cairo::RefPtr<BrushShape> BrushShape::create( bool show_brush, bool show_outline, OutlineType type )
{
	return Cairo::RefPtr<BrushShape>( new BrushShape( show_brush, show_outline, type ) );
}
	
void BrushShape::setBrush( Brush& brush, const Palette& pal )
{
	// update current area
	requestUpdate();
	
	m_pBrush = &brush;
	m_pPalette = &pal;
	// generate outline
	const int *d = brush.data();
	m_OutlinePath.clear();
	// find all pixels with open edges
	int p = 0, w = brush.width(), h = brush.height();
	for( int y = 0; y < h; y++ ) {
		for( int x = 0; x < w; x++ ) {
			if( d[p] != -1 ) {
				// create lines
				// top
				if( y == 0 || d[p-w] == -1 ) {
					m_OutlinePath.push_back( {x,y} );
					m_OutlinePath.push_back( {x+1,y} );
				}
				// right
				if( x == w-1 || d[p+1] == -1 ) {
					m_OutlinePath.push_back( {x+1,y} );
					m_OutlinePath.push_back( {x+1,y+1} );
				}
				// bottom
				if( y == h-1 || d[p+w] == -1 ) {
					m_OutlinePath.push_back( {x+1,y+1} );
					m_OutlinePath.push_back( {x,y+1} );
				}
				// left
				if( x == 0 || d[p-1] == -1 ) {
					m_OutlinePath.push_back( {x,y+1} );
					m_OutlinePath.push_back( {x,y} );
				}
			}
			p++;
		}
	}
	// update new area
	requestUpdate();
}

void BrushShape::unsetBrush()
{
	m_pBrush = 0;
	m_pPalette = 0;
}

void BrushShape::setShowBrush( bool val )
{
	m_ShowBrush = val;
	requestUpdate();
}

void BrushShape::setShowOutline( bool val )
{
	m_ShowOutline = val;
	requestUpdate();
}

bool BrushShape::showBrush() const
{
	return m_ShowBrush;
}

bool BrushShape::showOutline() const
{
	return m_ShowOutline;
}

void BrushShape::setOutlineType( OutlineType type )
{
	m_Outline = type;
	requestUpdate();
}

void BrushShape::requestUpdate()
{
	if( m_pBrush && assigned() ) {
		int border = ceil(std::max( baseWidth(), lineWidth() ) * 0.5);
		updateArea( (x() - m_pBrush->offsetX()) * parent().hScale() - border,
		            (y() - m_pBrush->offsetY()) * parent().vScale() - border,
		            m_pBrush->width() * parent().hScale() + 2*border,
		            m_pBrush->height() * parent().vScale() + 2*border );
	}
}

void BrushShape::drawShape( const Cairo::RefPtr<Cairo::Context>& cr )
{
	if( m_pBrush ) {
		// transform to brush location
		int hscale = parent().hScale();
		int vscale = parent().vScale();
		cr->translate( (x()-m_pBrush->offsetX())*hscale, (y()-m_pBrush->offsetY())*vscale );
		// draw brush
		if( m_ShowBrush ) {
			cr->save();
			cr->scale(hscale, vscale);
			//cc->rectangle( 0, 0, m_pBrush->width(), m_pBrush->height() );
			Cairo::RefPtr<Cairo::SurfacePattern> sp = Cairo::SurfacePattern::create( m_pBrush->getImage(*m_pPalette) );
			sp->set_filter(Cairo::FILTER_FAST);
			cr->set_source(sp);
			//cc->mask(sp);
			cr->paint();
			cr->restore();
		}
		// draw outline
		if( m_ShowOutline ) {
			// set path
			switch( m_Outline ) {
				case OUTLINE_RECT:
					cr->rectangle( 0.5, 0.5,
					               double(m_pBrush->width() *hscale)-1.0,
					               double(m_pBrush->height()*vscale)-1.0 );
					break;
				case OUTLINE_SHAPED:
				{
					int x1, y1, x2 = -1, y2 = -1;
					for( unsigned int i = 0; i < m_OutlinePath.size(); i+=2 ) {
						x1 = m_OutlinePath[i].first; y1 = m_OutlinePath[i].second;
						bool rel = x1 == x2 && y1 == y2;
						x2 = m_OutlinePath[i+1].first; y2 = m_OutlinePath[i+1].second;
						if( !rel ) {
							double dsx = (x2>x1)||(y2<y1) ? +0.5:-0.5;
							double dsy = (x2>x1)||(y2>y1) ? +0.5:-0.5;
							cr->move_to( dsx+x1*hscale, dsy+y1*vscale );
						}
						double dex = (x2<x1)||(y2<y1) ? +0.5:-0.5;
						double dey = (x2>x1)||(y2<y1) ? +0.5:-0.5;
						cr->line_to( dex+x2*hscale, dey+y2*vscale );
					}
					break;
				}
			}
			// draw
			if( baseWidth() > 0 ) {
				cr->set_line_width( baseWidth() );
				applyBaseColor(cr);
				applyBaseDash(cr);
				cr->stroke_preserve();
			}
			cr->set_line_width( lineWidth() );
			applyLineColor(cr);
			applyLineDash(cr);
			cr->stroke();
		}
	}
}


} // namespace Polka 
