#include "Canvas.h"
#include "CanvasData.h"
#include "Palette.h"
#include "Functions.h"
#include "Brush.h"
#include "UndoAction.h"
#include "Project.h"
#include <cstring>
#include <cassert>
#include <iostream>

namespace Polka {


Canvas::Canvas( Project& _prj, const std::string& _id )
	: Object(_prj, _id, true), m_pData(0), m_PixelHScale(1), m_PixelVScale(1)
{
	// create default grids
	m_TileGridWidth = 16;
	m_TileGridHeight = 16;
	m_TileGridOffsetH = 0;
	m_TileGridOffsetV = 0;
	
	// default view
	m_ViewScale = 2;
	m_ViewOffsetH = 0;
	m_ViewOffsetV = 0;
}

Canvas::~Canvas()
{
	// delete allocated data
	if( m_pData ) {
		delete m_pData;
	}		
}

int Canvas::width() const
{
	return m_pData->width();
}

int Canvas::height() const
{
	return m_pData->height();
}

int Canvas::pixelScaleHor() const
{
	return m_PixelHScale;
}

int Canvas::pixelScaleVer() const
{
	return m_PixelVScale;
}

void Canvas::setPixelScale( int hor, int ver )
{
	m_PixelHScale = hor;
	m_PixelVScale = ver;
}

int Canvas::tileGridWidth() const
{
	return m_TileGridWidth;
}

int Canvas::tileGridHeight() const
{
	return m_TileGridHeight;
}

int Canvas::tileGridHorOffset() const
{
	return m_TileGridOffsetH;
}

int Canvas::tileGridVerOffset() const
{
	return m_TileGridOffsetV;
}

void Canvas::setTileGrid( int width, int height, int hor_offset, int ver_offset )
{
	if( width >= 0 )
		m_TileGridWidth = width;
	if( height >= 0 )
		m_TileGridHeight = height;
	if( hor_offset >= 0 )
		m_TileGridOffsetH = hor_offset;
	if( ver_offset >= 0 )
		m_TileGridOffsetV = ver_offset;
	
	// correct offsets
	m_TileGridOffsetH %= m_TileGridWidth;
	m_TileGridOffsetV %= m_TileGridHeight;
}

int Canvas::viewScale() const
{
	return m_ViewScale;
}

int Canvas::viewHorOffset() const
{
	return m_ViewOffsetH;
}

int Canvas::viewVerOffset() const
{
	return m_ViewOffsetV;
}

void Canvas::setViewScale( int scale )
{
	m_ViewScale = scale;
}

void Canvas::setViewOffset( int hor, int ver )
{
	m_ViewOffsetH = hor;
	m_ViewOffsetV = ver;
}

void Canvas::setClipRectangle( int x, int y, int w, int h )
{
	if( !m_pData ) return;
	if( x == -1 ) {
		m_ClipX1 = 0;
		m_ClipY1 = 0;
		m_ClipX2 = m_pData->width()-1;
		m_ClipY2 = m_pData->height()-1;
	} else {
		m_ClipX1 = x;
		m_ClipY1 = y;
		m_ClipX2 = x+w-1;
		m_ClipY2 = y+h-1;
	}
}

void Canvas::clipRectangle( int& x1, int& y1, int& x2, int& y2 )
{
	// right orientation
	if( x2 < x1 ) swap(x1, x2);
	if( y2 < y1 ) swap(y1, y2);
std::cout << std::dec << "(" << m_ClipX1 << ", " << m_ClipY1 << ")-("<< m_ClipX2 << ", " << m_ClipY2 << ")\n";
	
	if( x1 < m_ClipX1 ) x1 = m_ClipX1;
	if( y1 < m_ClipY1 ) y1 = m_ClipY1;
	if( x2 > m_ClipX2 ) x2 = m_ClipX2;
	if( y2 > m_ClipY2 ) y2 = m_ClipY2;
}

int Canvas::clipLeft() const
{
	return m_ClipX1;
}

int Canvas::clipRight() const
{
	return m_ClipX2;
}

int Canvas::clipTop() const
{
	return m_ClipY1;
}

int Canvas::clipBottom() const
{
	return m_ClipY2;
}

bool Canvas::getClipped() const
{
	return m_ClipX1 != 0 && m_ClipY1 != 0 && m_ClipX2 != m_pData->width()-1 && m_ClipY2 != m_pData->height()-1;
}

void Canvas::resize( int w, int h, int horscale, int verscale )
{
	bool mod = false;
	if( m_pData->width() != w || m_pData->height() != h ) {
		bool clipped = getClipped();
		// modify data
		m_pData->setSize(w, h);
		m_Image.clear();
		mod = true;
		// modify clipping
		if( !clipped ) setClipRectangle();
	}
	if( horscale != m_PixelHScale || verscale != m_PixelVScale ) {
		m_PixelHScale = horscale;
		m_PixelVScale = verscale;
		mod = true;
	}
	if( mod ) update();
}

void Canvas::setPalette( Palette& pal )
{
	setDependency( DEP_PAL, &pal );
	update();
}

Cairo::RefPtr<Cairo::ImageSurface> Canvas::getImage()
{
	if( !m_Image ) {
		assert( m_pData );
		m_Image = Cairo::ImageSurface::create( Cairo::FORMAT_RGB24, m_pData->width(), m_pData->height() );
		update();
	}
	return m_Image;
}

int Canvas::data( int x, int y )
{
	return m_pData->data( x, y );
}

const Palette& Canvas::palette() const
{
	return *dynamic_cast<const Palette*>(dependency(DEP_PAL));
}

const Gdk::Rectangle& Canvas::lastUpdate() const
{
	return m_LastUpdateRect;
}

void Canvas::onUpdate( bool full )
{
	// ensure image object exists
	if( !m_Image ) {
		assert( m_pData );
		m_Image = Cairo::ImageSurface::create( Cairo::FORMAT_RGB24, m_pData->width(), m_pData->height() );
		full = true;
	}
	
	if( full ) {
		// update entire buffer
		m_pData->writeImage( m_Image, Gdk::Rectangle( 0, 0, m_pData->width(), m_pData->height() ) );
	} else {
		// only within data range
		bool intersects;
		m_UpdateRect.intersect( Gdk::Rectangle( 0, 0, m_pData->width(), m_pData->height() ), intersects );
		
		if( intersects ) {
			// update changed area
			m_pData->writeImage( m_Image, m_UpdateRect );
			m_LastUpdateRect = m_UpdateRect;
		} else {
			m_LastUpdateRect = Gdk::Rectangle(0, 0, 0, 0);
		}
	}
	m_UpdateRect = Gdk::Rectangle(0, 0, 0, 0);	
}

void Canvas::draw( int x, int y, const Pen& pen )
{
	// TODO: draw on each data layer
	m_pData->draw( x, y, pen );

	// update area
	if( addChangedRect( Gdk::Rectangle( x-pen.offsetX(), y-pen.offsetY(), pen.width(), pen.height() ) ) ) {
		// partial update self and dependencies
		update(false);
	}
}

void Canvas::changeColorDraw( int x, int y, const Pen& pen, int current )
{
	// TODO: draw on each data layer
	if( !m_pData->changeColorDraw( x, y, pen, current ) ) return;

	// update area
	if( addChangedRect( Gdk::Rectangle( x-pen.offsetX(), y-pen.offsetY(), pen.width(), pen.height() ) ) ) {
		// partial update self and dependencies
		update(false);
	}
}

void Canvas::drawLine( int x1, int y1, int x2, int y2, const Pen& pen )
{
	// TODO: draw on each data layer
	m_pData->drawLine( x1, y1, x2, y2, pen );
	// swap rect points
	if( x2 < x1 ) swap(x1, x2);
	if( y2 < y1 ) swap(y1, y2);
	
	// update area
	if( addChangedRect( Gdk::Rectangle( x1 - pen.offsetX(), y1 - pen.offsetY(), x2-x1 + pen.width(), y2-y1 + pen.height() ) ) ) {
		// partial update self and dependencies
		update(false);
	}
}

void Canvas::drawRect( int x1, int y1, int x2, int y2, const Pen& lpen, const Pen& fpen )
{
	if( x2 < x1 ) swap(x1, x2);
	if( y2 < y1 ) swap(y1, y2);

	m_pData->drawRect( x1, y1, x2, y2, lpen, fpen );

	// update area
	if( addChangedRect(Gdk::Rectangle( x1, y1, 1+x2-x1, 1+y2-y1 )) ) {
		// partial update self and dependencies
		update(false);
	}
}

void Canvas::bucketFill( int x, int y, const Pen& pen )
{
	// TODO: draw on each data layer
	Gdk::Rectangle curDrw = m_pData->bucketFill( x, y, pen );

	// update area
	if( addChangedRect(curDrw) ) {
		// partial update self and dependencies
		update(false);
	}
}

Brush *Canvas::createBrushFromRect( int x, int y, int w, int h, int bg )
{
	return m_pData->createBrushFromRect( x, y, w, h, bg );
}


// storage
int Canvas::store( Storage& s )
{
	// save pixel grid
	//s.createItem("PIXEL_GRID", "II");
	//s.setField( 0, m_PixelGridVisible );
	//s.setField( 1, m_PixelGridStyle );
	// save pixel grid
	s.createItem("TILE_GRID", "IIII");
	//s.setField( 0, m_TileGridVisible );
	s.setField( 0, m_TileGridWidth );
	s.setField( 1, m_TileGridHeight );
	s.setField( 2, m_TileGridOffsetH );
	s.setField( 3, m_TileGridOffsetV );
	//s.setField( 5, m_TileGridStyle );

	s.createItem("PIXEL_SCALE", "II");
	s.setField( 0, m_PixelHScale );
	s.setField( 1, m_PixelVScale );
	
	Storage& subS = s.createObject("DATA_MAIN");
	m_pData->save(subS);
	return 0;
}
	
int Canvas::restore( Storage& s )
{
	// read pixel grid
	/*if( s.findItem("PIXEL_GRID") ) {
		if( s.checkFormat("II") ) {
			m_PixelGridVisible = s.integerField(0);
			m_PixelGridStyle = (GridStyle)s.integerField(1);
		}
	}*/
	// read tile grid
	if( s.findItem("TILE_GRID") ) {
		int i = -1;
		if( s.checkFormat("IIIIII") )
			i = 1;
		else if( s.checkFormat("IIII") )
			i = 0;
		if( i >= 0 ) {
			//m_TileGridVisible = s.integerField(0);
			m_TileGridWidth   = s.integerField(i+0);
			m_TileGridHeight  = s.integerField(i+1);
			m_TileGridOffsetH = s.integerField(i+2);
			m_TileGridOffsetV = s.integerField(i+3);
			//m_TileGridStyle   = (GridStyle)s.integerField(5);
		}
	}

	if( s.findItem("PIXEL_SCALE") ) {
		if( s.checkFormat("II") ) {
			m_PixelHScale = s.integerField(0);
			m_PixelVScale = s.integerField(1);
		}
	}

	if( s.findObject("DATA_MAIN") ) {
		// read data
		int err = m_pData->load( s.object() );
		update();
		return err;
	} else
		return Storage::EMISSINGDATANONFATAL;
}

// display and undo area functions

bool Canvas::addChangedRect( const Gdk::Rectangle& rect )
{
	// create clip rectangle
	m_UpdateRect = Gdk::Rectangle( m_ClipX1, m_ClipY1, 1+m_ClipX2-m_ClipX1, 1+m_ClipY2-m_ClipY1 );
	bool has_int;
	m_UpdateRect.intersect( rect, has_int );

	// return if no change
	if( !has_int ) return false;
	
	// update undo rectangle
	if( m_ActionRect.has_zero_area() )
		m_ActionRect = m_UpdateRect;
	else
		m_ActionRect.join( m_UpdateRect );
	
	return true;
}

// data transfer and undo

void Canvas::undo( const std::string& id, Storage& s )
{
	if( id == "RECT" ) { 
		m_UpdateRect = m_pData->restoreRect( s );
		update(false);
	}
}

void Canvas::redo( const std::string& id, Storage& s )
{
	if( id == "RECT" )  {
		m_UpdateRect = m_pData->restoreRect( s );
		update(false);
	}
}

// undo functions

void Canvas::startAction( Glib::ustring text, Glib::RefPtr<Gdk::Pixbuf> icon )
{
	m_ActionRect = Gdk::Rectangle( 0, 0, 0, 0 );	
	m_ActionText = text;
	m_rpActionIcon = icon;
	// signal data layers to backup
	m_pData->backupState();
}

void Canvas::finishAction()
{
	if( !m_ActionRect.has_zero_area() ) {
		// clip rect to data area
		m_ActionRect.intersect( Gdk::Rectangle( 0, 0, m_pData->width(), m_pData->height() ) );
		// create undo action for rect
	std::cout << "UNDO  (" << m_ActionRect.get_x() << ", " << m_ActionRect.get_y() << ")-(" << m_ActionRect.get_width() << ", " << m_ActionRect.get_height() << ")\n";
		UndoAction& action = project().undoHistory().createAction( *this );
		action.setName( m_ActionText );
		action.setIcon( m_rpActionIcon );
		// create undo data block
		Storage& su = action.setUndoData("RECT");
		m_pData->storeBackupRect( su, m_ActionRect );
		// set redo action
		Storage& sr = action.setRedoData("RECT");
		m_pData->storeRect( sr, m_ActionRect );
	
		m_ActionText.clear();
		m_rpActionIcon.reset();
	}
}

} // namespace Polka
