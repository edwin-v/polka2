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

#ifndef _POLKA_SHAPEDRAWINGOBJECTS_H_
#define _POLKA_SHAPEDRAWINGOBJECTS_H_

#include <ShapeDrawingArea.h>
#include <string>
#include <map>

namespace Polka {

/*
 * A simple image shape
 * 
 */
class ImageShape : public ShapeDrawingArea::Shape
{
public:
	static Cairo::RefPtr<ImageShape> create();
	
	void setImage( const Cairo::RefPtr<Cairo::ImageSurface>& image );
	Cairo::RefPtr<Cairo::ImageSurface> image() const;

	void setSmoothScaling( bool val = true );
	bool smoothScaling() const;

protected:
	virtual void requestUpdate();

private:
	ImageShape();

	void drawShape( const Cairo::RefPtr<Cairo::Context>& cr );
	
	Cairo::RefPtr<Cairo::ImageSurface> m_Image;
	bool m_Smooth;
};


/*
 * Base class for line objects
 * 
 *   Draws a rectangle around pixels
 */
class LineShapeBase : public ShapeDrawingArea::Shape
{
public:

	// base pen
	void setBaseWidth( int width );
	void setBaseColor( double r, double g, double b, double a = 1.0 );
	void setBaseDashSize( int dash );
	
	int baseWidth() const;
	double baseR() const;
	double baseG() const;
	double baseB() const;
	double baseAlpha() const;
	int baseDashSize() const;
	void applyBaseColor( const Cairo::RefPtr<Cairo::Context>& cr ) const;
	void applyBaseDash( const Cairo::RefPtr<Cairo::Context>& cr ) const;
	
	// line pen
	void setLineWidth( int width );
	void setLineColor( double r, double g, double b, double a = 1.0 );
	void setLineDashSize( int dash );

	int lineWidth() const;
	double lineR() const;
	double lineG() const;
	double lineB() const;
	double lineAlpha() const;
	int lineDashSize() const;
	void applyLineColor( const Cairo::RefPtr<Cairo::Context>& cr ) const;
	void applyLineDash( const Cairo::RefPtr<Cairo::Context>& cr ) const;
	
protected:
	LineShapeBase();

private:
	int m_BaseWidth, m_LineWidth;
	double m_BaseR, m_BaseG, m_BaseB, m_BaseA;
	double m_LineR, m_LineG, m_LineB, m_LineA;
	int m_BaseDash, m_LineDash;
};


/*
 * Rectangle object for ShapeDrawingArea
 * 
 *   Draws a rectangle over/around pixels
 */
class RectangleShape : public LineShapeBase
{
public:
	enum Type { NORMAL, LINE_INSIDE, LINE_OUTSIDE, FULL_INSIDE, FULL_OUTSIDE };

	static Cairo::RefPtr<RectangleShape> create( Type t = NORMAL );
	
	void setType( Type t );
	Type type() const;

protected:
	virtual void requestUpdate();
	
private:
	RectangleShape( Type t );

	void calcOffsets( double& ox, double& oy );

	void drawShape( const Cairo::RefPtr<Cairo::Context>& cr );

	Type m_Type;
};


/*
 * Grid object for ShapeDrawingArea
 * 
 *   Draws a line/dot grid over the main area. The following three
 *   types are supported:
 * 
 * 	 GRID_LINES: Line over boundaries
 *   GRID_SHADES: Bright/dark edges
 *   GRID_DOTS: Dots at top loft (line) and bottom right (base)
 */
class GridShape : public LineShapeBase
{
public:
	enum GridType { GRID_LINES, GRID_SHADES, GRID_DOTS };

	static Cairo::RefPtr<GridShape> create( GridType type = GRID_SHADES );

	void setType( GridType type );
	GridType type() const;

protected:
	virtual void requestUpdate();

private:
	GridShape( GridType type );

	void drawShape( const Cairo::RefPtr<Cairo::Context>& cr );

	GridType m_Type;
};



/*
 * Line object for overlay painter
 * 
 *   Draws a line between pixels
 */
class LineShape : public LineShapeBase
{
public:
	static Cairo::RefPtr<LineShape> create();

protected:
	virtual void requestUpdate();

private:
	LineShape();

	void drawShape( const Cairo::RefPtr<Cairo::Context>& cr );

	int	m_X, m_Y, m_W, m_H;
};

/*
 * Object for overlay of (outlined)brushes
 * 
 *   Draws an overlay of a brush and/or a bounding rectangle or shape.
 */
class Brush;
class Palette;

class BrushShape : public LineShapeBase
{
public:
	enum OutlineType { OUTLINE_RECT, OUTLINE_SHAPED };

	static Cairo::RefPtr<BrushShape> create( bool show_brush = true, bool show_outline = true, OutlineType type = OUTLINE_RECT );

	void setBrush( Brush& brush, const Palette& pal );
	void unsetBrush();

	void setShowBrush( bool val = true );
	void setShowOutline( bool val = true );
	bool showBrush() const;
	bool showOutline() const;
	void setOutlineType( OutlineType type );

protected:
	virtual void requestUpdate();

private:
	BrushShape( bool show_brush, bool show_outline, OutlineType type );
	
	void drawShape( const Cairo::RefPtr<Cairo::Context>& cr );

	int m_X, m_Y;
	bool m_ShowBrush, m_ShowOutline;
	OutlineType m_Outline;
	Brush *m_pBrush;
	const Palette *m_pPalette;
	std::vector< std::pair<int,int> > m_OutlinePath;
};


} // namespace Polka 

#endif // _POLKA_SHAPEDRAWINGOBJECTS_H_

