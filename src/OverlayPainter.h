#ifndef _POLKA_OVERLAYPAINTER_H_
#define _POLKA_OVERLAYPAINTER_H_

#include <string>
#include <map>
#include <cairomm/context.h>

namespace Polka {


class OverlayPainter
{
public:
	OverlayPainter();
	~OverlayPainter();

	// base class for paint objects
	class Shape {
	public:
		Shape();
		~Shape();
		
		void setPrimaryPen( int width, double r, double g, double b, double a = 1.0, double sub = -100.0 );
		void setSecondaryPen( int width, double r, double g, double b, double a = 1.0, double sub = -100.0  );

		void setVisible( bool value = true );
		bool isVisible() const;

		// shape control default implementation
		virtual void setSize( int w, int h ) = 0;
		virtual void setLocation( int x, int y ) = 0;
		virtual void move( int x, int y ) = 0;

		virtual void setValue( const std::string& name, int value );
		virtual void setValue( const std::string& name, double value );
		virtual void setValue( const std::string& name, const std::string& value );

		virtual int x() = 0;
		virtual int y() = 0;
		virtual int width() = 0;
		virtual int height() = 0;

		virtual void paint( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hscale, int vscale );
	
	protected:
		int m_PriWidth, m_SecWidth;
		double m_PriR, m_PriG, m_PriB, m_PriA, m_PriSub;
		double m_SecR, m_SecG, m_SecB, m_SecA, m_SecSub;

		// drawing base
		virtual void drawShape( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hscale, int vscale ) = 0;
		
	private:
		bool m_Visible;

	};

	// set coordinate space
	void setCoordSpace( int offset_x, int offset_y, int hscale, int vscale );
	void setImageSize( int width, int height );

	// shape access
	void add( int priority, Shape *s );
	bool remove( int priority );
	bool erase( int priority );
	Shape& shape( int priority );

	void paint( Cairo::RefPtr<Cairo::Context> cc );

private:
	// ordered map of overlay objects
	std::map<int,Shape*> m_Objects;
	int m_DX, m_DY, m_Width, m_Height, m_HScale, m_VScale;
};

/*
 * Rectangle object for overlay painter
 * 
 *   Draws a rectangle around pixels
 */
class OverlayRectangle : public OverlayPainter::Shape
{
public:
	OverlayRectangle( int x = 0, int y = 0, int w = 16, int h = 16, bool around = true );
	~OverlayRectangle();
	
	virtual void setSize( int w, int h );
	virtual void setLocation( int x, int y );
	virtual void move( int x, int y );
	void setRectAroundPixels( bool around );
	void setPattern( int pat );
	
	virtual int x();
	virtual int y();
	virtual int width();
	virtual int height();

protected:
	void drawShape( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hscale, int vscale );

private:
	int	m_X, m_Y, m_W, m_H;
	bool m_Around;
	int m_Pattern;
};

/*
 * Line object for overlay painter
 * 
 *   Draws a line between pixels
 */
class OverlayLine : public OverlayPainter::Shape
{
public:
	OverlayLine( int x = 0, int y = 0, int w = 16, int h = 16 );
	~OverlayLine();
	
	virtual void setSize( int w, int h );
	virtual void setLocation( int x, int y );
	virtual void move( int x, int y );
	
	virtual int x();
	virtual int y();
	virtual int width();
	virtual int height();

protected:
	void drawShape( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hscale, int vscale );

private:
	int	m_X, m_Y, m_W, m_H;

};

/*
 * Rectangle object for overlay painter
 * 
 *   Draws a rectangle around pixels
 */
class OverlayCross : public OverlayPainter::Shape
{
public:
	OverlayCross( int x = 0, int y = 0, bool left_of_pixel = true, bool top_of_pixel = true);
	~OverlayCross();
	
	virtual void setSize( int w, int h );
	virtual void setLocation( int x, int y );
	virtual void move( int x, int y );
	
	virtual int x();
	virtual int y();
	virtual int width();
	virtual int height();

protected:
	void drawShape( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hscale, int vscale );

private:
	int	m_X, m_Y;
	bool m_LeftOfPixel, m_TopOfPixel;
};

/*
 * Rectangle object for overlay painter
 * 
 *   Draws a rectangle around pixels
 */
class OverlayText : public OverlayPainter::Shape
{
public:
	enum Align { LEFT, CENTER, RIGHT, TOP, BOTTOM };

	OverlayText( int x = 0, int y = 0, int size = 16, Align halign = CENTER, Align valign = CENTER, int margin = 0 );
	~OverlayText();
	
	virtual void setSize( int w, int h );
	virtual void setLocation( int x, int y );
	virtual void move( int x, int y );
	
	virtual int x();
	virtual int y();
	virtual int width();
	virtual int height();

	virtual void setValue( const std::string& name, const std::string& value );

protected:
	void drawShape( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hscale, int vscale );

private:
	int	m_X, m_Y;
	int m_Width, m_Height;
	Align m_HorAlign, m_VerAlign;
	int m_Size, m_Margin;
	std::string m_Text;
};

/*
 * Rectangle object for overlay painter
 * 
 *   Draws a rectangle around pixels
 */
class OverlayGrid : public OverlayPainter::Shape
{
public:
	enum GridType { GRID_LINES, GRID_DOTS };

	OverlayGrid( int hgrid = 16, int vgrid = 16, GridType type = GRID_LINES );
	~OverlayGrid();
	
	virtual void setSize( int w, int h );
	virtual void setLocation( int x, int y );
	virtual void move( int x, int y );
	
	virtual int x();
	virtual int y();
	virtual int width();
	virtual int height();

	void setType( GridType type );

protected:
	void drawShape( Cairo::RefPtr<Cairo::Context> cc, int width, int height, int hscale, int vscale );

private:
	int	m_HorGridSize, m_VerGridSize;
	int m_HorOffset, m_VerOffset;
	GridType m_Type;
};

} // namespace Polka 

#endif // _POLKA_OVERLAYPAINTER_H_

