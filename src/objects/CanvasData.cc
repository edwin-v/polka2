#include "CanvasData.h"
#include "Canvas.h"
#include "Palette.h"
#include "Project.h"
#include "Storage.h"
#include "StorageHelpers.h"
#include "Brush.h"
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>


namespace Polka {

CanvasData::CanvasData( Canvas& canvas, int w, int h, int depth )
	: m_Canvas( canvas ), m_Depth( depth ), m_Width(0), m_Height(0), m_pDataStore(0)
{
	// number of bytes per pixel
	m_PixSize = (m_Depth+7) / 8;
	// init data
	setSize( w, h );
}

CanvasData::~CanvasData()
{
	setSize( 0, 0 );
}

void CanvasData::setSize( int w, int h )
{
	// first adjust number of rows
	if( h < m_Height ) {
		while( m_Height > h ) {
			free( m_Data.back() );
			m_Data.pop_back();
			m_Height--;
		}
	}
	// adjust line sizes
	if( w != m_Width ) {
		std::vector<char*>::iterator it = m_Data.begin();
		while( it != m_Data.end() ) {
			*it = static_cast<char*>(realloc( *it, w * m_PixSize ));
			if( w > m_Width ) memset( *it + m_Width*m_PixSize, 0, (w-m_Width)*m_PixSize );
			++it;
		}
		m_Width = w;
	}
	// add more data
	for( int i = m_Height; i < h; i++ ) {
		m_Data.push_back( static_cast<char*>(malloc( w*m_PixSize )) );
		memset( m_Data.back(), 0, w*m_PixSize );
	}
		
	m_Height = h;
	
	// create backup space
	if( w && h ) {
		if( m_pDataStore )
			m_pDataStore = static_cast<char*>(realloc( m_pDataStore, w*h* m_PixSize ));
		else
			m_pDataStore = static_cast<char*>(malloc( w*h*m_PixSize ));
	} else {
		if( m_pDataStore ) {
			free( m_pDataStore );
			m_pDataStore = 0;
		}
	}
	
}


int CanvasData::depth() const
{
	return m_Depth;
}

int CanvasData::width() const
{
	return m_Width;
}

int CanvasData::height() const
{
	return m_Height;
}

const Palette& CanvasData::palette() const
{
	return m_Canvas.palette();
}

int CanvasData::data( int x, int y )
{
	int result = 0;
	for( int a = x*m_PixSize; a < x*m_PixSize+m_PixSize; a++ )
		result = (result << 8 ) | m_Data[y][a];
	return result;
}

void CanvasData::writeImage( Cairo::RefPtr<Cairo::ImageSurface> image, const Gdk::Rectangle& rect )
{
	std::cout << "draw(" << rect.get_x() << ", " << rect.get_y() << ")-(" << rect.get_width() << ", " << rect.get_height() << ")\n";
	// rewrite all image data
	int addr;
	unsigned char *imgData = image->get_data();
	// loop over all pixels
	for( int y = rect.get_y(); y < rect.get_y()+rect.get_height(); y++ ) {
		// start line
		addr = image->get_stride() * y + 4*rect.get_x();
		char *line = m_Data[y] + rect.get_x()*m_PixSize;
		
		// write line
		for( int x = 0; x < rect.get_width()*m_PixSize; x+=m_PixSize ) {
			// skip alpha
				
			// process data
			// basic palette data (never more than 1 byte)
			int pixel = line[x] % palette().size();
	//std::cout << pixel << std::endl;
			imgData[addr++] = int(255 * palette().b( pixel ));
			imgData[addr++] = int(255 * palette().g( pixel ));
			imgData[addr++] = int(255 * palette().r( pixel ));
			imgData[addr++] = 0;
		}
	}
}

void CanvasData::draw( int x, int y, const Pen& pen )
{
	applyBrush(x, y, pen);
}

void CanvasData::drawLine( int x1, int y1, int x2, int y2, const Pen& pen )
{
	// handle single point
	if( x1 == x2 && y1 == y2 ) {
		draw(x1, x2, pen);
		return;
	}

	// calc line stuff
	bool vert = false;
	if( abs(y2-y1) > abs(x2-x1) ) {
		// flip
		vert = true;
		int t = x1;	x1 = y1; y1 = t;
		t = x2;	x2 = y2; y2 = t;
	}
	
	// calc stepping
	int dx = x1<=x2?1:-1;
	double dy = double(y2-y1)/abs(x2-x1), yerr = 0.0;
	
	// draw line
	int y = y1;
	for( int x = x1; x != x2+dx; x += dx ) {
		if( vert )
			applyBrush(y, x, pen);
		else
			applyBrush(x, y, pen);

		yerr += dy;
		if( dy > 0.0 && yerr > 0.5 ) {
			yerr -= 1.0;
			y++;
		} else if( dy < 0.0 && yerr < -0.5 ) {
			yerr += 1.0;
			y--;
		}
	}

}

void CanvasData::drawRect( int x1, int y1, int x2, int y2, const Pen& lpen, const Pen& fpen )
{
	char *dat1 = m_Data[y1];
	char *dat2 = m_Data[y2];
	char lc[4], fc[4];
	for( int i = 0; i < m_PixSize; i++ ) {
		lc[i] = (lpen.data()[0] >> (i*8)) & 255;
		fc[i] = (fpen.data()[0] >> (i*8)) & 255;
	}
	// horizontal lines
	for( int x = x1*m_PixSize; x <= x2*m_PixSize; x+=m_PixSize )
		for( int i = 0; i < m_PixSize; i++ )
			dat1[x+i] = dat2[x+i] = lc[i];
	// vertical lines
	for( int y = y1+1; y < y2; y++ ) {
		dat1 = m_Data[y];
		for( int i = 0; i < m_PixSize; i++ )
			dat1[x1*m_PixSize] = dat1[x2*m_PixSize] = lc[i];
		// fill
		if( fpen.data()[0] != -1 ) {
			for( int x = (x1+1)*m_PixSize; x < x2*m_PixSize; x+=m_PixSize ) 
				for( int i = 0; i < m_PixSize; i++ )
					dat1[x+i] = fc[i];
		}		
	}
}

void CanvasData::applyBrush( int x, int y, const Pen& pen )
{
	int cols = x - pen.offsetX();
	int cole = cols + pen.width()-1;
	int rows = y - pen.offsetY();
	int rowe = rows + pen.height()-1;
	m_Canvas.clipRectangle( cols, rows, cole, rowe );
	int dx = cols - ( x-pen.offsetX() );
	int dy = rows - ( y-pen.offsetY() );

	// exit if nothing left
	if( dx >= pen.width() || dy >= pen.height() ) return;

	// loop pen
	for( int row = rows; row <= rowe; row++ ) {
		// data pointers
		const int *penData = pen.data() + (dy+row-rows)*pen.width() + dx;
		char *data = m_Data[row] + cols*m_PixSize;
		// copy data
		for( int col = cols; col <= cole; col++ ) {
			if( penData[0] != -1 )
				for( int i = 0; i < m_PixSize; i++ ) {
					data[i] = (penData[0] >> (i*8)) & 255;
				}
			data += m_PixSize;
			penData++;
		}
	}

}

Gdk::Rectangle CanvasData::bucketFill( int x, int y, const Pen& pen )
{
	Gdk::Rectangle r;
	char fg[4], bg[4];
	// start address
	char *data = m_Data[y] + x*m_PixSize;
	// calc background colour
	bool same = true;
	for( int i = 0; i < m_PixSize; i++ ) {
		fg[i] = (pen.data()[0] >> (i*8)) & 255;
		bg[i] = data[i];
		same &= fg[i] == bg[i];
	}
	// fill line
	if( !same ) fillLine( x, y, fg, bg, r );
	return r;	
}

bool CanvasData::fillLine( int x, int y, char fg[4], char bg[4], Gdk::Rectangle& r )
{
	int xb = x;
	char *dat = m_Data[y] + x*m_PixSize;
	// fill left
	while( xb >= m_Canvas.clipLeft() ) {
		bool same = true;
		for( int i = 0; i < m_PixSize; i++ )
			same &= bg[i] == dat[i];
		// break from loop if not same
		if( !same ) break;
		
		// fill pixel
		for( int i = 0; i < m_PixSize; i++ )
			dat[i] = fg[i];
		
		dat -= m_PixSize;
		xb--;
	}
	
	// return if no fill
	if( xb == x ) return false;
	
	// line start
	char *sdat = dat + m_PixSize;
	
	// fill right
	int xe = x+1;
	dat = m_Data[y] + (x+1)*m_PixSize;
	while( xe <= m_Canvas.clipRight() ) {
		bool same = true;
		for( int i = 0; i < m_PixSize; i++ )
			same &= bg[i] == dat[i];
		// break from loop if not same
		if( !same ) break;
		
		// fill pixel
		for( int i = 0; i < m_PixSize; i++ )
			dat[i] = fg[i];
		
		dat += m_PixSize;
		xe++;
	}

	// update rect
	if( r.get_x() > xb+1 ) r.set_x( xb+1 );
	if( r.get_x()+r.get_width() < xe ) r.set_width( xe - r.get_x() );
	if( r.get_y() > y ) 
		r.set_y(y);
	else if( y >= r.get_y()+r.get_height() )
		r.set_height( 1+y-r.get_y() );

	// bluntly loop over all positions above and below the fill
	for( int i = xb+1; i < xe; i++ ) {
		// fill above
		if( y>m_Canvas.clipTop() ) fillLine( i, y-1, fg, bg, r );
		// fill below
		if( y<m_Canvas.clipBottom() ) fillLine( i, y+1, fg, bg, r );

		sdat += m_PixSize;
	}
	return true;

}


Brush *CanvasData::createBrushFromRect( int x, int y, int w, int h, int bg )
{
	// count colours
	int c2 = -1;
	Brush *b = 0;
	for( int sy = y; sy < y+h; sy++ ) {
		char *data = m_Data[sy];
		for( int sx = x; sx < x+w; sx++ ) {
			if( data[sx] != -1 ) {
				if( c2 == -1 )
					c2 = data[sx];
				else {
					// two colours other than bg
					b = new Brush(w, h);
					sy = y+h;
					break;
				}
					
			}
		}
	}
	if(!b) b = new Shape(w,h);

	// copy brush data
	int *bdat = b->data();
	for( int sy = y; sy < y+h; sy++ ) {
		char *data = m_Data[sy];
		for( int sx = x; sx < x+w; sx++ ) {
			if( data[sx] == bg )
				*bdat = -1;
			else
				*bdat = data[sx];
			bdat++;
		}
	}

	return b;
}


// storage
int CanvasData::save( Storage& s )
{
	s.createItem("DATA_SIZE", "II");
	s.setField( 0, m_Width );
	s.setField( 1, m_Height );
	
	s.createItem("PALETTE", "S");
	s.setField( 0, palette().name() );
		
	s.createItem("DATA", "S");
	std::string& dat = s.setDataField(0);
	int lineSize = m_Width * m_PixSize;
	dat.reserve( lineSize * m_Height );
	for( unsigned int i = 0; i < m_Data.size(); i++ )
		dat.append( m_Data[i], lineSize );

	return 0;
}
	
int CanvasData::load( Storage& s )
{
	// get image size
	if( !s.findItem("DATA_SIZE") ) return Storage::EMISSINGDATAFATAL;
	if( !s.checkFormat("II") ) return Storage::EINCORRECTDATATYPE;
	int w = s.integerField(0);
	int h = s.integerField(1);
	setSize( w, h );
	
	// read palettes
	if( !s.findItem("PALETTE") ) return Storage::EMISSINGDATAFATAL;
	if( !s.checkFormat("S") ) return Storage::EINCORRECTDATATYPE;

	Glib::ustring palName( s.stringField(0) );
	Object *pal = m_Canvas.project().findObject( palName );
	if( !pal ) return Storage::EMISSINGDATAFATAL;
	m_Canvas.setPalette( *dynamic_cast<Palette*>(pal) );

	// read data
	if( !s.findItem("DATA") ) return Storage::EMISSINGDATAFATAL;
	if( !s.checkFormat("S") ) return Storage::EINCORRECTDATATYPE;
	const std::string& dat = s.dataField(0);

	if( int(dat.size()) < m_Width*m_Height*m_PixSize ) return Storage::EINCORRECTDATALENGTH;
	
	//std::cout << std::hex << int(dat[0]) << int(dat[1]) << int(dat[2]) << int(dat[3]) << int(dat[4]) << std::endl;
	
	const char *cdat = dat.c_str();
	for( int i = 0; i < m_Height; i++ ) {
		memcpy( m_Data[i], cdat, m_Width*m_PixSize );
		cdat += m_Width*m_PixSize;
	}

	return 0;
}

// create a backup of the image data
void CanvasData::backupState()
{
	assert( m_pDataStore );
	// store image data
	int lineSize = m_Width * m_PixSize;
	char *dat = m_pDataStore;
	for( unsigned int i = 0; i < m_Data.size(); i++ ) {
		memcpy( dat, m_Data[i], lineSize );
		dat += lineSize;
	}
}

// save a data rect to storage from the backup
void CanvasData::storeBackupRect( Storage& s, const Gdk::Rectangle& rect )
{
	storageSetRect( s, "DATA_RECT", rect );
	s.createItem("DATA", "S");
	std::string& dat = s.setDataField(0);
	int lineSize = m_Width*m_PixSize;
	dat.reserve( rect.get_y()*rect.get_x()*m_PixSize );
	// copy data
	char *src = m_pDataStore + rect.get_y()*lineSize + rect.get_x()*m_PixSize;
	for( int i = 0; i < rect.get_height(); i++ ) {
		dat.append( src, rect.get_width()*m_PixSize );
		src += lineSize;
	}
}

// save a data rect to storage
void CanvasData::storeRect( Storage& s, const Gdk::Rectangle& rect )
{
	storageSetRect( s, "DATA_RECT", rect );
	s.createItem("DATA", "S");
	std::string& dat = s.setDataField(0);
	dat.reserve( rect.get_y()*rect.get_x()*m_PixSize );
	// copy data
	for( int i = rect.get_y(); i < rect.get_y()+rect.get_height(); i++ ) {
		dat.append( m_Data[i] + rect.get_x()*m_PixSize, rect.get_width()*m_PixSize );
	}
}

const Gdk::Rectangle CanvasData::restoreRect( Storage& s )
{
	Gdk::Rectangle r;
	if( storageGetRect( s, "DATA_RECT", r ) ) {
		if( s.findItem("DATA") ) {
			const std::string& dat = s.dataField(0);
			const char *src = dat.c_str();
			// since only used internally, assume correct size
			for( int i = r.get_y(); i < r.get_y()+r.get_height(); i++ ) {
				memcpy( m_Data[i] + r.get_x()*m_PixSize, src, r.get_width()*m_PixSize );
				src += r.get_width()*m_PixSize;
			}
		}
	}
	return r;
}

} // namespace Polka
