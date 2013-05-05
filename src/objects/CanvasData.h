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
	virtual ~CanvasData();

	// dimensions
	void setSize( int w, int h );
	int depth() const;
	int width() const;
	int height() const;

	// palette data
	const Palette &palette() const;

	// data access
	int data( int x, int y ) const;

	// output
	virtual void writeImage( Cairo::RefPtr<Cairo::ImageSurface> image, const Gdk::Rectangle& rect );

	// modification
	virtual void draw( int x, int y, const Pen& pen );
	virtual bool changeColorDraw( int x, int y, const Pen& pen, int current );
	virtual void drawLine( int x1, int y1, int x2, int y2, const Pen& pen );
	virtual void drawRect( int x1, int y1, int x2, int y2, const Pen& lpen, const Pen& fpen );
	virtual Gdk::Rectangle bucketFill( int x, int y, const Pen& pen );
	virtual void flip( int x1, int y1, int x2, int y2, bool vertical = false );
	virtual void rotate( int x, int y, int sz, bool ccw = false );

	virtual void setData( int x, int y, const char *data, int w, int h );

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
