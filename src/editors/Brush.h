#ifndef _POLKA_BRUSH_H_
#define _POLKA_BRUSH_H_

#include "Pen.h"
#include <cairomm/surface.h>

namespace Polka {

static const int NOOFFS = -100000;

class Palette;
class Shape;

class Brush : public Pen
{
public:
	Brush( int width, int height, int offsetx = NOOFFS, int offsety = NOOFFS );
	~Brush();
	
	virtual const int *data() const;
	virtual int *data();

	// set data
	virtual void setColor( int col );
	virtual void setData( const int *data, int trans_col = -1 );
	virtual void setTransparentColor( int col );
	
	// create image surface
	Cairo::RefPtr<Cairo::ImageSurface> getImage( const Palette& pal );

	Shape *convertToShape();
	void flip( bool vertical = false );
	void rotate( bool ccw = false );

protected:
	int *m_Data;
	int m_TransparentColor;
	Cairo::RefPtr<Cairo::ImageSurface> m_refImage;
	const Palette *m_pRefPal;
};

class Shape : public Brush
{
public:
	Shape( int width, int height, int offsetx = NOOFFS, int offsety = NOOFFS );
	~Shape();
	
	// recolor
	virtual void setColor( int col );
	virtual void setTransparentColor( int col );
};


} // namespace Polka

#endif // _POLKA_BRUSH_H_
