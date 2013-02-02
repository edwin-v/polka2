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

#ifndef _POLKA_HIGFRAME_H_
#define _POLKA_HIGFRAME_H_

#include <gtkmm/frame.h>
#include <gtkmm/alignment.h>

namespace Polka {

/*
 * Gnome HIG frame as created by Glade interface designer
 */

class HIGFrame : public Gtk::Frame
{
public:
	HIGFrame( const Glib::ustring& label );
	~HIGFrame();

	void set_bold( bool value = true );

	virtual void add(Widget& widget);
	void remove();

	Gtk::Widget *get_child();
	const Gtk::Widget *get_child() const;

protected:

private:
	Gtk::Alignment m_Contents;
};

} // namespace Polka 

#endif // _POLKA_HIGFRAME_H_
