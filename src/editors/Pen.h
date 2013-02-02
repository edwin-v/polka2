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

#ifndef _POLKA_PEN_H_
#define _POLKA_PEN_H_


namespace Polka {

class Pen
{
public:
	Pen();
	virtual ~Pen();
	
	// pen data access
	int width() const;
	int height() const;
	virtual const int *data() const;

	int offsetX() const;
	int offsetY() const;

	// set data
	virtual void setColor( int col );
	virtual void setData( const int *data );

protected:
	int m_Width, m_Height;
	int	m_DX, m_DY;
	
private:
	int m_Color;

};

} // namespace Polka

#endif // _POLKA_PEN_H_
