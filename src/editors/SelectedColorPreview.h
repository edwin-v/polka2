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

#ifndef _SELECTEDCOLORPREVIEW_H_
#define _SELECTEDCOLORPREVIEW_H_

#include <gtkmm/drawingarea.h>

namespace Polka {


class SelectedColorPreview: public Gtk::DrawingArea 
{
public:
	SelectedColorPreview( int margin = 4 );
	~SelectedColorPreview();

	void setMargin( int margin );
	void setFGColor( double r, double g, double b );
	void setBGColor( double r, double g, double b );

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );

	// widget sizing overrides
	virtual Gtk::SizeRequestMode get_request_mode_vfunc() const;
	virtual void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const;
	virtual void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const;
	virtual void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const;
	virtual void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const;
	
private:
	int m_Margin;
	double m_FR, m_FG, m_FB;
	double m_BR, m_BG, m_BB;
};

} // namespace Polka

#endif // _SELECTEDCOLORPREVIEW_H_
