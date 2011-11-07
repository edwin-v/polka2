#ifndef _POLKA_CANVASDATA_H_
#define _POLKA_CANVASDATA_H_

#include <cairomm/surface.h>
#include <gdkmm/rectangle.h>

namespace Polka {

class Palette;
class Canvas;
class Pen;
class Storage;
class Brush;

class CanvasData
{
public:
	CanvasData( Canvas& canvas, int w, int h, int depth );
	~CanvasData();

	// dimensions
	void setSize( int w, int h );
	int depth() const;
	int width() const;
	int height() const;

	// palette data
	const Palette &palette() const;

	// data access
	int data( int x, int y );

	// output
	virtual void writeImage( Cairo::RefPtr<Cairo::ImageSurface> image, const Gdk::Rectangle& rect );

	// modification
	virtual void draw( int x, int y, const Pen& pen );
	virtual bool changeColorDraw( int x, int y, const Pen& pen, int current );
	virtual void drawLine( int x1, int y1, int x2, int y2, const Pen& pen );
	virtual void drawRect( int x1, int y1, int x2, int y2, const Pen& lpen, const Pen& fpen );
	virtual Gdk::Rectangle bucketFill( int x, int y, const Pen& pen );

	virtual void applyBrush( int x, int y, const Pen& pen );

	virtual Brush *createBrushFromRect( int x, int y, int w, int h, int bg );

	// storage
	virtual int save( Storage& s );
	virtual int load( Storage& s );

	void backupState();
	void storeBackupRect( Storage& s, const Gdk::Rectangle& rect );
	void storeRect( Storage& s, const Gdk::Rectangle& rect );
	const Gdk::Rectangle restoreRect( Storage& s );


protected:
	std::vector<char*> m_Data;
	
private:
	Canvas& m_Canvas;
	int m_Depth, m_PixSize;
	int m_Width, m_Height;
	char *m_pDataStore;
	
	bool fillLine( int x, int y, char fg[4], char bg[4], Gdk::Rectangle& r );
};


} // namespace Polka

#endif // _POLKA_CANVASDATA_H_
