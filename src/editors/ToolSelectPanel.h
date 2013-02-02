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

#ifndef _POLKA_TOOLSELECTPANEL_H_
#define _POLKA_TOOLSELECTPANEL_H_

#include <gtkmm/box.h>
#include <gtkmm/radiobutton.h>

namespace Polka {

class ToolSelectPanel: public Gtk::VBox
{
public:
	ToolSelectPanel();
	~ToolSelectPanel();

	Glib::SignalProxy0<void> floatModeChanged();
	Glib::SignalProxy0<void> toBrushClicked();

	void setHasFloating( bool value = true );

	bool copyMode() const;
	bool solidMode() const;

private:
	Gtk::RadioButton m_Copy, m_Cut;
	Gtk::RadioButton m_Solid, m_Transparent;
	Gtk::Button m_ToBrush;
};

} // namespace Polka

#endif // _POLKA_TOOLSELECTPANEL_H_
