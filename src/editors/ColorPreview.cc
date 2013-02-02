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

#include "ColorPreview.h"


namespace Polka {


ColorPreview::ColorPreview()
	: m_R(0.0), m_G(0.0), m_B(0.0)
{
	set_shadow_type(Gtk::SHADOW_OUT);
}

ColorPreview::~ColorPreview()
{
}

void ColorPreview::setColor( double r, double g, double b )
{
	m_R = r;
	m_G = g;
	m_B = b;
	queue_draw();
}


bool ColorPreview::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	const Gtk::Allocation& a = get_allocation();

	// calculate sizes
	auto border = get_style_context()->get_border();
	double wx = (double)a.get_width() - border.get_left() - border.get_right();
	double wy = (double)a.get_height() - border.get_top() - border.get_bottom();

	// paint the background
	cr->rectangle( border.get_left(), border.get_top(), wx, wy );
	cr->set_source_rgb(m_R, m_G, m_B);
	cr->fill();
	
	// draw the frame
	Frame::on_draw(cr);
	return true;
}

} // namespace Polka
