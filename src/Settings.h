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

#ifndef _POLKA_SETTINGS_H_
#define _POLKA_SETTINGS_H_

#include "Storage.h"

namespace Gtk {
	class Window;
}

namespace Polka {

class Settings
{
public:
	static Settings& get();

	// general setting handlers
	void setValue( const std::string& path, const std::string& name, bool value );
	void setValue( const std::string& path, const std::string& name, int value );
	void setValue( const std::string& path, const std::string& name, double value );
	void setValue( const std::string& path, const std::string& name, const std::string& value );
	// special purpose setting handlers
	void setWindowGeometry(  const std::string& path, const std::string& name, const Gtk::Window& window );
	
	// general value fetch
	bool getBool( const std::string& path, const std::string& name, bool default_value );
	int getInteger( const std::string& path, const std::string& name, int default_value );
	double getFloat( const std::string& path, const std::string& name, double default_value );
	std::string getString( const std::string& path, const std::string& name, const std::string& default_value );
	// special purpose fetch
	void getWindowGeometry(  const std::string& path, const std::string& name, Gtk::Window& window, int x, int y, int w, int h );

private:
	Settings();
	~Settings();
	
	Storage m_Storage;
	
	Storage& selectPath( const std::string& path );
};

} // namespace Polka

#endif // _POLKA_SETTINGS_H_

