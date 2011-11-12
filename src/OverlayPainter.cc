#include "OverlayPainter.h"
#include <iostream>


namespace Polka {


OverlayPainter::OverlayPainter()
{
	// default coordspace
	m_DX = m_DY = 0;
	m_Width = m_Height = 16;
	m_HScale = m_VScale = 1;
}

OverlayPainter::~OverlayPainter()
{
	// delete all owned objects
	std::map<int,Shape*>::iterator it = m_Objects.begin();
	while( it != m_Objects.end() ) {
		delete it->second;
		++it;
	}
}

void OverlayPainter::setImageSize( int width, int height )
{
	m_Width = width;
	m_Height = height;
}

void OverlayPainter::setCoordSpace( int offset_x, int offset_y, int hscale, int vscale )
{
	m_DX = offset_x;
	m_DY = offset_y;
	m_HScale = hscale;
	m_VScale = vscale;
}

OverlayPainter::Shape& OverlayPainter::shape( int priority )
{
	return *m_Objects[priority];
}

void OverlayPainter::add( int priority, Shape *s )
{
	m_Objects[priority] = s;
}

bool OverlayPainter::remove( int priority )
{
	return m_Objects.erase(priority) > 0;
}

bool OverlayPainter::erase( int priority )
{
	std::map<int,Shape*>::iterator it = m_Objects.find( priority );
	if( it != m_Objects.end() ) {
		delete it->second;
		m_Objects.erase(it);
		return true;
	}
	return false;
}

void OverlayPainter::paint( Cairo::RefPtr<Cairo::Context> cc )
{
	// set coord space
	cc->save();
	cc->translate( -m_DX, -m_DY );
	
	std::map<int,Shape*>::iterator it = m_Objects.begin();
	while( it != m_Objects.end() ) {
		(*it).second->paint( cc, m_Width, m_Height, m_HScale, m_VScale );
		++it;
	}

	// restore transformation
	cc->restore();
}

/*
 * Shape implementation
 */

OverlayPainter::Shape::Shape()
{
	// default pen
	m_PriWidth = 1;
	m_SecWidth = 3;
	m_PriR = m_PriG = m_PriB = m_PriA = m_SecA = 1.0;
	m_SecR = m_SecG = m_SecB = 0.0;
	m_PriSub = 0.5;
	m_SecSub = 0.5;
	// default invisible
	m_Visible = false;
}
	
OverlayPainter::Shape::~Shape()
{
}
		
void OverlayPainter::Shape::paint( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hscale, int vscale )
{
	if( !m_Visible ) return;

	drawShape( cc, width, height, hscale, vscale );
}

void OverlayPainter::Shape::setVisible( bool value )
{
	m_Visible = value;
}

bool OverlayPainter::Shape::isVisible() const
{
	return m_Visible;
}

void OverlayPainter::Shape::setPrimaryPen( int width, double r, double g, double b, double a, double sub )
{
	m_PriWidth = width;
	m_PriR = r;
	m_PriG = g;
	m_PriB = b;
	m_PriA = a;
	if( sub < -10.0 ) 
		m_PriSub = 0.5*(m_PriWidth&1);
	else
		m_PriSub = sub;
}

void OverlayPainter::Shape::setSecondaryPen( int width, double r, double g, double b, double a, double sub )
{
	m_SecWidth = width;
	m_SecR = r;
	m_SecG = g;
	m_SecB = b;
	m_SecA = a;
	if( sub < -10.0 ) 
		m_SecSub = 0.5*(m_SecWidth&1);
	else
		m_SecSub = sub;
}

void OverlayPainter::Shape::setValue( const std::string& name, int value )
{
	// default to no action
}

void OverlayPainter::Shape::setValue( const std::string& name, double value )
{
	// default to no action
}

void OverlayPainter::Shape::setValue( const std::string& name, const std::string& value )
{
	// default to no action
}

/*
 * OverlayRectangle implementation
 */

OverlayRectangle::OverlayRectangle( int x, int y, int w, int h, bool around )
	: m_X(x), m_Y(y), m_W(w), m_H(h), m_Around(around), m_Pattern(0)
{
}

OverlayRectangle::~OverlayRectangle()
{
}

void OverlayRectangle::setSize( int w, int h )
{
	m_W = w;
	m_H = h;
}

void OverlayRectangle::setLocation( int x, int y )
{
	m_X = x;
	m_Y = y;
}

void OverlayRectangle::setRectAroundPixels( bool around )
{
	m_Around = around;
}

void OverlayRectangle::setPattern( int pat )
{
	m_Pattern = pat;
}

void OverlayRectangle::move( int x, int y )
{
	m_X += x;
	m_Y += y;
}

int OverlayRectangle::x()
{
	return m_X;
}

int OverlayRectangle::y()
{
	return m_Y;
}

int OverlayRectangle::width()
{
	return m_W;
}

int OverlayRectangle::height()
{
	return m_H;
}


// Rectangle drawing
void OverlayRectangle::drawShape( Cairo::RefPtr<Cairo::Context> cc, int, int, int hsc, int vsc )
{
	// no antialiasing
	cc->set_antialias( Cairo::ANTIALIAS_NONE );

	// pixel rect offsets
	double offx = m_Around?-m_PriSub:m_PriSub+double(hsc>>1);	
	double offy = m_Around?-m_PriSub:m_PriSub+double(vsc>>1);	
	// draw rectangle with edge
	cc->rectangle( double(m_X*hsc) + offx, m_Y*vsc + offy, m_W*hsc - 2*offx, m_H*vsc - 2*offy);
	if( m_SecWidth > 0 ) {
		cc->set_line_width( m_SecWidth );
		cc->set_source_rgba( m_SecR, m_SecG, m_SecB, m_SecA );
		cc->stroke_preserve();
	}
	cc->set_line_width( m_PriWidth );
	cc->set_source_rgba( m_PriR, m_PriG, m_PriB, m_PriA );
	if( m_Pattern > 0 ) {
		std::vector<double> p({double(m_Pattern)});
		cc->set_dash( p, 0 );
	}
	cc->stroke();
	if( m_Pattern > 0 ) cc->unset_dash();

	// restore antialiasing
	cc->set_antialias( Cairo::ANTIALIAS_DEFAULT );
}

/*
 * OverlayLine implementation
 */

OverlayLine::OverlayLine( int x, int y, int w, int h )
	: m_X(x), m_Y(y), m_W(w), m_H(h)
{
}

OverlayLine::~OverlayLine()
{
}

void OverlayLine::setSize( int w, int h )
{
	m_W = w;
	m_H = h;
}

void OverlayLine::setLocation( int x, int y )
{
	m_X = x;
	m_Y = y;
}

void OverlayLine::move( int x, int y )
{
	m_X += x;
	m_Y += y;
}

int OverlayLine::x()
{
	return m_X;
}

int OverlayLine::y()
{
	return m_Y;
}

int OverlayLine::width()
{
	return m_W;
}

int OverlayLine::height()
{
	return m_H;
}


// Line drawing
void OverlayLine::drawShape( Cairo::RefPtr<Cairo::Context> cc, int, int, int hsc, int vsc )
{
	// no antialiasing
	cc->set_antialias( Cairo::ANTIALIAS_NONE );

	// draw double line
	cc->move_to( (m_X+0.5)*hsc, (m_Y+0.5)*vsc );
	cc->rel_line_to( m_W*hsc, m_H*vsc );
	cc->set_line_width( m_SecWidth );
	cc->set_source_rgba( m_SecR, m_SecG, m_SecB, m_SecA );
	cc->stroke_preserve();
	cc->set_line_width( m_PriWidth );
	cc->set_source_rgba( m_PriR, m_PriG, m_PriB, m_PriA );
	if( m_PriWidth == m_SecWidth ) {
		std::vector<double> p({double(2*m_PriWidth), double(4*m_PriWidth)});
		cc->set_dash( p, 0 );
		cc->stroke();
		cc->unset_dash();
	} else
		cc->stroke();

	// restore antialiasing
	cc->set_antialias( Cairo::ANTIALIAS_DEFAULT );
}

/*
 * OverlayCross implementation
 */

OverlayCross::OverlayCross( int x, int y, bool left_of_pixel, bool top_of_pixel )
	: m_X(x), m_Y(y), m_LeftOfPixel(left_of_pixel), m_TopOfPixel(top_of_pixel)
{
}

OverlayCross::~OverlayCross()
{
}

void OverlayCross::setSize( int w, int h )
{
	// irrelevant
}

void OverlayCross::setLocation( int x, int y )
{
	m_X = x;
	m_Y = y;
}

void OverlayCross::move( int x, int y )
{
	m_X += x;
	m_Y += y;
}

int OverlayCross::x()
{
	return m_X;
}

int OverlayCross::y()
{
	return m_Y;
}

int OverlayCross::width()
{
	return -1;
}

int OverlayCross::height()
{
	return -1;
}

// Cross drawing
void OverlayCross::drawShape( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hsc, int vsc )
{
	// no antialiasing
	cc->set_antialias( Cairo::ANTIALIAS_NONE );

	// line offsets
	double px = hsc*m_X + (m_LeftOfPixel?-0.5:hsc+0.5);
	double py = vsc*m_Y + (m_TopOfPixel ?-0.5:vsc+0.5);

	// draw rectangle with edge
	cc->move_to( px, 0 );
	cc->line_to( px, height*vsc );
	cc->move_to( 0, py ); 
	cc->line_to( width*hsc, py ); 
	cc->set_line_width( m_SecWidth );
	cc->set_source_rgba( m_SecR, m_SecG, m_SecB, m_SecA );
	cc->stroke_preserve();
	cc->set_line_width( m_PriWidth );
	cc->set_source_rgba( m_PriR, m_PriG, m_PriB, m_PriA );
	cc->stroke();

	// restore antialiasing
	cc->set_antialias( Cairo::ANTIALIAS_DEFAULT );
}

/*
 * OverlayText implementation
 */

OverlayText::OverlayText( int x, int y, int size, Align halign, Align valign, int margin )
	: m_X(x), m_Y(y), m_Width(1), m_Height(1), m_HorAlign(halign), 
	  m_VerAlign(valign), m_Size(size), m_Margin(margin)
{
}

OverlayText::~OverlayText()
{
}

void OverlayText::setSize( int w, int h )
{
	m_Width = w;
	m_Height = h;
}

void OverlayText::setLocation( int x, int y )
{
	m_X = x;
	m_Y = y;
}

void OverlayText::move( int x, int y )
{
	m_X += x;
	m_Y += y;
}

void OverlayText::setValue( const std::string& name, const std::string& value )
{
	// set text for every name
	m_Text = value;
}

int OverlayText::x()
{
	return m_X;
}

int OverlayText::y()
{
	return m_Y;
}

int OverlayText::width()
{
	return m_Width;
}

int OverlayText::height()
{
	return m_Height;
}

// Text drawing
void OverlayText::drawShape( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hsc, int vsc )
{
	Cairo::TextExtents te;

	cc->select_font_face("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
	cc->set_font_size(m_Size);
	
	cc->get_text_extents(m_Text, te);

	double x = m_X*hsc - te.x_bearing, y = m_Y*vsc - te.y_bearing;	
	switch(m_HorAlign) {
		case CENTER:
			x += m_Width*hsc/2.0 - te.width/2.0;
			break;
		case RIGHT:
			x +=  m_Width*hsc - te.width - m_Margin;;
			break;
		default:
			x += m_Margin;
			break;
	}

	switch(m_VerAlign) {
		case CENTER:
			y += m_Height*vsc/2.0 - te.height/2.0;
			break;
		case BOTTOM:
			y += m_Height*vsc - te.height - m_Margin;
			break;
		default:
			y += m_Margin;
			break;
	}

	cc->move_to(x,y);
	cc->text_path(m_Text);
	
	if( m_SecWidth > 0 ) {
		cc->set_line_width( m_SecWidth );
		cc->set_source_rgba( m_SecR, m_SecG, m_SecB, m_SecA );
		cc->stroke_preserve();
	}
	cc->set_source_rgba( m_PriR, m_PriG, m_PriB, m_PriA );
	cc->fill();
}


/*
 * OverlayGrid implementation
 */

OverlayGrid::OverlayGrid( int hgrid , int vgrid , GridType type )
	: m_HorGridSize(hgrid), m_VerGridSize(vgrid), m_HorOffset(0), 
	  m_VerOffset(0), m_Type(type)
{
	setVisible();
}

OverlayGrid::~OverlayGrid()
{
}

void OverlayGrid::setSize( int w, int h )
{
	m_HorGridSize = w;
	m_VerGridSize = h;
}

void OverlayGrid::setLocation( int x, int y )
{
	if( m_HorGridSize )
		m_HorOffset = x % m_HorGridSize;
		
	if( m_VerGridSize )
		m_VerOffset = y % m_VerGridSize;
}

void OverlayGrid::move( int x, int y )
{
	setLocation( m_HorOffset + x, m_VerOffset + y );
}

int OverlayGrid::x()
{
	return m_HorOffset;
}

int OverlayGrid::y()
{
	return m_VerOffset;
}

int OverlayGrid::width()
{
	return m_HorGridSize;
}

int OverlayGrid::height()
{
	return m_VerGridSize;
}

void OverlayGrid::setType( GridType type )
{
	m_Type = type;
}

// Rectangle drawing
void OverlayGrid::drawShape( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hsc, int vsc )
{
	double x1, y1, x2, y2;

	// clip to content and get area
	cc->rectangle( 0, 0, hsc*width, vsc*height );
	cc->clip();
	cc->get_clip_extents(x1, y1, x2, y2);
	// no antialiasing
	cc->set_antialias( Cairo::ANTIALIAS_NONE );

	// calc grid size
	double gsph =  m_PriWidth/2.0 + m_PriSub;
	double gspl = -m_PriWidth/2.0 + m_PriSub;
	double gssh =  m_SecWidth/2.0 + m_SecSub;
	double gssl = -m_SecWidth/2.0 + m_SecSub;
	int gs = ceil((gsph>gssh?gsph:gssh)-(gspl<gssl?gspl:gssl));
	int sc = hsc<vsc?hsc:vsc;

	if( m_Type == GRID_LINES ) {
		// draw grid if greater than minimum size
		if( sc > ((m_HorGridSize>2 && m_VerGridSize>2)?1:3)*gs ) {
			// first draw secondary pen if needed
			if( m_SecWidth > 0 ) {
				cc->set_line_width( m_SecWidth );
				cc->set_source_rgba( m_SecR, m_SecG, m_SecB, m_SecA );
				// horizontal lines
				if( m_VerGridSize > 0 ) {
					int start = (y1+m_SecWidth/2.0-m_SecSub-m_VerOffset*vsc) / (m_VerGridSize*vsc);
					start = vsc*(m_VerOffset + start*m_VerGridSize);
					for( int c = start; c < y2+m_SecWidth; c += vsc*m_VerGridSize ) {
						cc->move_to( x1, c+m_SecSub );
						cc->line_to( x2, c+m_SecSub );
					}
				}
				// vertical lines
				if( m_HorGridSize > 0 ) {
					int start = (x1+m_SecWidth/2.0-m_SecSub-m_HorOffset*hsc) / (m_HorGridSize*hsc);
					start = hsc*(m_HorOffset + start*m_HorGridSize);
					for( int c = start; c <= x2+m_SecWidth; c += hsc*m_HorGridSize ) {
						cc->move_to( c+m_SecSub, y1 );
						cc->line_to( c+m_SecSub, y2 );
					}
				}
				// stroke lines
				cc->stroke();
			}
			// always draw primary pen
			cc->set_line_width( m_PriWidth );
			cc->set_source_rgba( m_PriR, m_PriG, m_PriB, m_PriA );
			// horizontal lines
			if( m_VerGridSize > 0 ) {
				int start = (y1+m_PriWidth/2.0-m_PriSub-m_VerOffset*vsc) / (m_VerGridSize*vsc);
				start = vsc*(m_VerOffset + start*m_VerGridSize);
				for( int c = start; c < y2+m_PriWidth; c += vsc*m_VerGridSize ) {
					cc->move_to( x1, c+m_PriSub );
					cc->line_to( x2, c+m_PriSub );
				}
			}
			// vertical lines
			if( m_HorGridSize > 0 ) {
				int start = (x1+m_PriWidth/2.0-m_PriSub-m_HorOffset*hsc) / (m_HorGridSize*hsc);
				start = hsc*(m_HorOffset + start*m_HorGridSize);
				for( int c = start; c <= x2+m_PriWidth; c += hsc*m_HorGridSize ) {
					cc->move_to( c+m_PriSub, y1 );
					cc->line_to( c+m_PriSub, y2 );
				}
			}
			// stroke lines
			cc->stroke();
		}

	} else { /* GRID_DOTS */
		// minimum size for dots is 3
		if( sc > 2 ) {
			// set dot pattern
			std::vector<double> p({1.0, vsc*m_VerGridSize-1.0});
			cc->set_dash( p, 0 );
			cc->set_line_width(1.0);
			cc->set_source_rgba(  m_SecR, m_SecG, m_SecB, m_SecA );
			// coloured dots in top left tile
			for( double x = m_HorOffset*hsc+0.5; x < width*hsc; x += m_HorGridSize*hsc ) {
				cc->move_to(x, 0);
				cc->line_to(x, height*vsc);
			}
			cc->stroke();
			// black dots in bottom right tile
			cc->set_dash( p, 1.0 );
			cc->set_source_rgba( m_PriR, m_PriG, m_PriB, m_PriA );
			for( double x = m_HorOffset*hsc-0.5; x < width*hsc; x += m_HorGridSize*hsc ) {
				if( x < 0 ) continue;
				cc->move_to(x, 0);
				cc->line_to(x, height*vsc);
			}
			cc->stroke();
			// reset
			cc->unset_dash();
		}
	}

	// restore antialiasing
	cc->set_antialias( Cairo::ANTIALIAS_DEFAULT );
	cc->reset_clip();
}



} // namespace Polka 


