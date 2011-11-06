#include "Pen.h"

namespace Polka {


Pen::Pen()
	: m_Width(1), m_Height(1), m_DX(0), m_DY(0), m_Color(0)
{
}

Pen::~Pen()
{
}

int Pen::width() const
{
	return m_Width;
}

int Pen::height() const
{
	return m_Height;
}

const int *Pen::data() const
{
	return &m_Color;
}

int Pen::offsetX() const
{
	return m_DX;
}

int Pen::offsetY() const
{
	return m_DY;
}


void Pen::setColor( int data )
{
	m_Color = data;
}

void Pen::setData( const int *data )
{
	m_Color = data[0];
}


} // namespace Polka

