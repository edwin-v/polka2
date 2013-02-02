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

