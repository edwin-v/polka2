#ifndef _POLKA_CANVAS_H_
#define _POLKA_CANVAS_H_

#include "Object.h"
#include "ObjectManager.h"
#include "Pen.h"
#include <glibmm/i18n.h>
#include <cairomm/surface.h>
#include <gdkmm/rectangle.h>

namespace Polka {

class CanvasData;
class Palette;
class Brush;

// dependency id for the main palette
static const int DEP_PAL = 0;

class Canvas : public Object 
{
public:
	Canvas( Project& _prj, const std::string& _id );
	virtual ~Canvas();

	// dimensions
	int width() const;
	int height() const;
	int pixelScaleHor() const;
	int pixelScaleVer() const;
	void setPixelScale( int hor, int ver );
	void resize( int w, int h, int horscale = -1, int verscale = -1);
	
	// grids
	int tileGridWidth() const;
	int tileGridHeight() const;
	int tileGridHorOffset() const;
	int tileGridVerOffset() const;

	void setTileGrid( int width, int height, int hor_offset, int ver_offset );
	
	// default view
	int viewScale() const;
	int viewHorOffset() const;
	int viewVerOffset() const;
	void setViewScale( int scale );
	void setViewOffset( int hor, int ver );
		
	Cairo::RefPtr<Cairo::ImageSurface> getImage();

	// data modification
	virtual void setPalette( Palette& pal );
	virtual void draw( int x, int y, const Pen& pen );
	virtual void changeColorDraw( int x, int y, const Pen& pen, int current );
	virtual void drawLine( int x1, int y1, int x2, int y2, const Pen& pen );
	virtual void drawRect( int x1, int y1, int x2, int y2, const Pen& lpen, const Pen& fpen );
	virtual void bucketFill( int x, int y, const Pen& pen );
	virtual void flip( int x, int y, int w, int h, bool vertical = false );
	virtual void rotate( int x, int y, int sz, bool ccw = false );

	// clipping
	void setClipRectangle( int x = -1, int y = -1, int w = -1, int h = -1 );
	void clipRectangle( int& x1, int& y1, int& x2, int& y2 );
	int clipLeft() const;
	int clipRight() const;
	int clipTop() const;
	int clipBottom() const;
	bool getClipped() const;
	
	// data access
	virtual int data( int x, int y );
	virtual const Palette& palette() const;

	virtual Brush *createBrushFromRect( int x, int y, int w, int h, int bg );

	// undo stuff
	virtual void undo( const std::string& id, Storage& s );
	virtual void redo( const std::string& id, Storage& s );

	const Gdk::Rectangle& lastUpdate() const;
	virtual void startAction( Glib::ustring text, Glib::RefPtr<Gdk::Pixbuf> icon );
	virtual void finishAction();

protected:
	virtual void onUpdate( bool full );

	CanvasData *m_pData;
	
	// storage
	virtual int store( Storage& s );
	virtual int restore( Storage& s );

private:
	Cairo::RefPtr<Cairo::ImageSurface> m_Image;
	int m_PixelHScale, m_PixelVScale;
	Gdk::Rectangle m_UpdateRect, m_LastUpdateRect;
	Gdk::Rectangle m_ActionRect;
	Glib::ustring m_ActionText;
	Glib::RefPtr<Gdk::Pixbuf> m_rpActionIcon;

	int m_TileGridWidth, m_TileGridHeight, m_TileGridOffsetH, m_TileGridOffsetV;

	int m_ViewScale, m_ViewOffsetH, m_ViewOffsetV;
	int m_ClipX1, m_ClipY1, m_ClipX2, m_ClipY2;

	friend class CanvasData;
	
	bool addChangedRect( const Gdk::Rectangle& rect );
};


} // namespace Polka

#endif // _POLKA_CANVAS_H_
