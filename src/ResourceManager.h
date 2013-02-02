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

#ifndef _POLKA_RESOURCEMANAGER_H_
#define _POLKA_RESOURCEMANAGER_H_

#include <gdkmm/pixbuf.h>
#include <gdkmm/cursor.h>
#include <gdkmm/display.h>
#include <gdkmm/window.h>
#include <map>

namespace Polka {

class ResourceManager
{
public:
	static ResourceManager& get();

	Glib::RefPtr<Gdk::Pixbuf> getIcon( const std::string& name );
	Glib::RefPtr<Gdk::Cursor> getCursor( const Glib::RefPtr< Gdk::Display >& display, const std::string& name );
	Glib::RefPtr<Gdk::Cursor> getCursor( const Glib::RefPtr< Gdk::Window >& window, const std::string& name );

private:
	ResourceManager();
	~ResourceManager();

	typedef std::map<std::string, Glib::RefPtr<Gdk::Pixbuf> > IconMap;
	IconMap m_Icons;
	typedef std::map<std::string, Glib::RefPtr<Gdk::Cursor> > CursorMap;
	CursorMap m_Cursors;
};

} // namespace Polka

#endif // _POLKA_RESOURCEMANAGER_H_

