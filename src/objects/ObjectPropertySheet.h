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

#ifndef _POLKA_OBJECTPROPERTYSHEET_H_
#define _POLKA_OBJECTPROPERTYSHEET_H_

#include <gtkmm/box.h>

namespace Polka {

class Canvas;

class ObjectPropertySheet: public Gtk::VBox
{
public:
	ObjectPropertySheet( const Glib::ustring& _name );
	virtual ~ObjectPropertySheet();

	const Glib::ustring& pageName() const;

	// signals
	typedef sigc::signal<void, bool> SignalSetModified;
	SignalSetModified signalSetModified();

	// actions
	virtual void apply() const = 0;
	virtual void reset() const = 0;

protected:
	void setModified( bool value );

private:
	SignalSetModified m_SignalSetModified;
	Glib::ustring m_Name;
};

} // namespace Polka

#endif // _POLKA_OBJECTPROPERTYSHEET_H_
