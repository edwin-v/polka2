#include "Brush.h"
#include "Palette.h"
#include "Functions.h"
#include <cstring>
#include "cairomm/context.h"


namespace Polka {

/*
 * Brush -- Full color brush
 */

Brush::Brush( int width, int height, int offsetx, int offsety )
	: m_TransparentColor(-1)
{
	m_Width = width;
	m_Height = height;
	m_DX = offsetx==NOOFFS?width/2:offsetx;
	m_DY = offsety==NOOFFS?height/2:offsety;
	
	m_Data = new int[m_Width*m_Height];
}

Brush::~Brush()
{
	delete [] m_Data;
}


const int *Brush::data() const
{
	return m_Data;
}

int *Brush::data()
{
	return m_Data;
}

void Brush::setColor( int col )
{
	// skip color data
}

void Brush::setData( const int *data, int trans_col )
{
	memcpy( m_Data, data, sizeof(int)*m_Width*m_Height );
	m_TransparentColor = trans_col;
	m_refImage.clear();
}

void Brush::setTransparentColor( int col )
{
	if( col != m_TransparentColor ) {
		unsigned int p = 0;
		for( int y = 0; y < m_Height; y++ ) {
			for( int x = 0; x < m_Width; x++ ) {
				if( m_Data[p] == -1 )
					m_Data[p] = m_TransparentColor;
				else if( m_Data[p] == col )
					m_Data[p] = -1;
				p++;
			}
		}
		m_TransparentColor = col;
		m_refImage.clear();
	}
}

Cairo::RefPtr<Cairo::ImageSurface> Brush::getImage( const Palette& pal )
{
	if( m_refImage && m_pRefPal == &pal ) return m_refImage;
	
	// create new image
	m_refImage = Cairo::ImageSurface::create( Cairo::FORMAT_ARGB32, m_Width, m_Height );
	// draw centre point with alpha
	Cairo::RefPtr<Cairo::Context> cc = Cairo::Context::create(m_refImage);
	cc->set_antialias(Cairo::ANTIALIAS_NONE);
	cc->move_to(m_DX,m_DY+0.5);
	cc->set_source_rgba(1.0, 1.0, 1.0, 0.25);
	cc->line_to(m_DX, m_DY+0.5);
	cc->stroke();
	// write data
	unsigned int p = 0, addr;
	unsigned char *imgData = m_refImage->get_data();
	// loop over all pixels
	for( int y = 0; y < m_Height; y++ ) {
		// start line
		addr = m_refImage->get_stride() * y;
		// write line
		for( int x = 0; x < m_Width; x++ ) {
			if( m_Data[p] != -1 ) {
				// write pixel data
				int pixel = m_Data[p] % pal.size();
				imgData[addr++] = int(255 * pal.b( pixel ));
				imgData[addr++] = int(255 * pal.g( pixel ));
				imgData[addr++] = int(255 * pal.r( pixel ));
				imgData[addr++] = 255;
			} else
				addr += 4;
			p++;
		}
	}
	m_pRefPal = &pal;
	return m_refImage;
}

/* convert to shape if only a single color */
Shape *Brush::convertToShape()
{
	// count colors
	int c = -1, p = 0;
	for( int y = 0; y < m_Height; y++ )
		for( int x = 0; x < m_Width; x++ ) {
			if( m_Data[p] != -1 ) {
				if( c == -1 )
					c = m_Data[p];
				else if( m_Data[p] != c )
					return 0;
			}
			p++;
		}
	Shape *s = new Shape(m_Width, m_Height);
	s->setData(m_Data);
	return s;
}

void Brush::flip( bool vertical )
{
	if( vertical ) {
		int ym = m_Height/2;
		for( int y = 0; y < ym; y++ ) {
			for( int x = 0; x < m_Width; x++ ) {
				int t = m_Data[y*m_Width+x];
				m_Data[y*m_Width+x] = m_Data[(m_Height-1-y)*m_Width+x];
				m_Data[(m_Height-1-y)*m_Width+x] = t;
			}
		}
	} else {
		int xm = m_Width/2;
		for( int x = 0; x < xm; x++ ) {
			for( int y = 0; y < m_Height; y++ ) {
				int t = m_Data[y*m_Width+x];
				m_Data[y*m_Width+x] = m_Data[y*m_Width+m_Width-1-x];
				m_Data[y*m_Width+m_Width-1-x] = t;
			}
		}
	}
	m_refImage.clear();
}

void Brush::rotate( bool ccw )
{
	// temp copy data
	int *dat = new int[m_Width*m_Height];
	memcpy(dat, m_Data, m_Width*m_Height*sizeof(int));

	int *src, dp, dl;
	if( ccw ) {
		src = &dat[m_Width-1];
		dp = m_Width;
		dl = -m_Width*m_Height-1;
	} else {
		src = &dat[m_Width*(m_Height-1)];
		dp = -m_Width;
		dl = m_Width*m_Height+1;
	}
	
	// rotate
	int *rotdat = m_Data;
	for( int y = 0; y < m_Width; y++ ) {
		for( int x = 0; x < m_Height; x++ ) {
			*rotdat = *src;
			src += dp;
			rotdat++;
		}
		src += dl;
	}

	delete [] dat;
	std::swap( m_Width, m_Height );
	std::swap( m_DX, m_DY );
	m_refImage.clear();
}

/*
 * Shape -- Single color brush
 */

Shape::Shape( int width, int height, int offsetx, int offsety )
	: Brush( width, height, offsetx, offsety )
{
}

Shape::~Shape()
{
}

void Shape::setColor( int col )
{
	// recolor shape
	for( int i = 0; i < m_Width*m_Height; i++ )
		if( m_Data[i] != -1 )
			m_Data[i] = col;
	m_refImage.clear();
}

void Shape::setTransparentColor( int col )
{
	// not for shape
}

} // namespace Polka

