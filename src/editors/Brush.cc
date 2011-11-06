#include "Brush.h"
#include <cstring>

namespace Polka {

/*
 * Brush -- Full color brush
 */

Brush::Brush( int width, int height, int offsetx, int offsety )
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

void Brush::setData( const int *data )
{
	memcpy( m_Data, data, sizeof(int)*m_Width*m_Height );
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
}


} // namespace Polka

