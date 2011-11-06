#ifndef _POLKA_BRUSH_H_
#define _POLKA_BRUSH_H_

#include "Pen.h"

namespace Polka {

static const int NOOFFS = -100000;

class Brush : public Pen
{
public:
	Brush( int width, int height, int offsetx = NOOFFS, int offsety = NOOFFS );
	~Brush();
	
	virtual const int *data() const;
	virtual int *data();

	// set data
	virtual void setColor( int col );
	virtual void setData( const int *data );

protected:
	int *m_Data;

};

class Shape : public Brush
{
public:
	Shape( int width, int height, int offsetx = NOOFFS, int offsety = NOOFFS );
	~Shape();
	
	// recolor
	virtual void setColor( int col );
};


} // namespace Polka

#endif // _POLKA_BRUSH_H_
