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

#ifndef _COLORPREVIEW_H_
#define _COLORPREVIEW_H_

#include <gtkmm/frame.h>

namespace Polka {


class ColorPreview: public Gtk::Frame 
{
public:
	ColorPreview();
	~ColorPreview();

	void setColor( double r, double g, double b );

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );

private:
	double m_R, m_G, m_B;

};

} // namespace Polka

#endif // _COLORPREVIEW_H_
