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

#include "Settings.h"
#include "Functions.h"
#include <gtkmm/window.h>
#include <glibmm/miscutils.h>
#include <iostream>

namespace Polka {

Settings::Settings()
{
	// store in system config dir
	std::string settings_file = Glib::get_user_config_dir();
	// with app name
	settings_file = Glib::build_filename(settings_file, "Polka2");
	// force create directory / error checking ????
	g_mkdir_with_parents( settings_file.c_str(), 0755 );
	// and filename
	settings_file = Glib::build_filename(settings_file, "polka.settings");
	
	m_Storage.setFilename( settings_file );
	
	int err = m_Storage.load();
	std::cout << "Settings load err = " << err << std::endl;
}

Settings::~Settings()
{
	int err = m_Storage.save();
	std::cout << "Settings save err = " << err << std::endl;
}

Settings& Settings::get()
{
	static Settings instance;
	return instance;
}

void Settings::setValue( const std::string& path, const std::string& name, bool value )
{
	Storage& s = selectPath(path);
	
	if( !s.findItem( name ) ) s.createItem( name, "I" );
	s.setField( 0, value?1:0 );
}

void Settings::setValue( const std::string& path, const std::string& name, int value )
{
	Storage& s = selectPath(path);
	
	if( !s.findItem( name ) ) s.createItem( name, "I" );
	s.setField( 0, value );
}

void Settings::setValue( const std::string& path, const std::string& name, double value )
{
	Storage& s = selectPath(path);

	if( !s.findItem( name ) ) s.createItem( name, "F" );
	s.setField( 0, value );
}

void Settings::setValue( const std::string& path, const std::string& name, const std::string& value )
{
	Storage& s = selectPath(path);

	if( !s.findItem( name ) ) s.createItem( name, "S" );
	s.setField( 0, value );
}

bool Settings::getBool( const std::string& path, const std::string& name, bool default_value )
{
	Storage& s = selectPath(path);

	if( !s.findItem(name) ) return default_value;
	if( !s.checkFormat("I") ) return default_value;
	
	return s.integerField(0)!=0;	 
}

int Settings::getInteger( const std::string& path, const std::string& name, int default_value )
{
	Storage& s = selectPath(path);

	if( !s.findItem(name) ) return default_value;
	if( !s.checkFormat("I") ) return default_value;
	
	return s.integerField(0);	 
}

double Settings::getFloat( const std::string& path, const std::string& name, double default_value )
{
	Storage& s = selectPath(path);

	if( !s.findItem(name) ) return default_value;
	if( !s.checkFormat("F") ) return default_value;
	
	return s.floatField(0);	 
}

std::string Settings::getString( const std::string& path, const std::string& name, const std::string& default_value )
{
	Storage& s = selectPath(path);

	if( !s.findItem(name) ) return default_value;
	if( !s.checkFormat("S") ) return default_value;
	
	return s.stringField(0);	 
}

Storage& Settings::selectPath( const std::string& path )
{
	std::vector<std::string> pathList = split(path, ':');
	Storage *s = &m_Storage;
	// check empty
	if( pathList.size() == 1 && trim(pathList[0]).empty() )
		return m_Storage;

	// create path
	for( unsigned int i = 0; i < pathList.size(); i++ ) {
		if( s->findObject( pathList[i] ) ) {
			// path item exists
			s = &s->object();
		} else {
			// create path item
			s = &s->createObject( pathList[i] );
		}
	}
	
	return *s;
}

void Settings::setWindowGeometry( const std::string& path, const std::string& name, const Gtk::Window& window )
{
	Storage& s = selectPath(path);

	if( !s.findItem( name ) ) s.createItem( name, "IIII" );
	
	int x, y;
	window.get_position( x, y );
	s.setField( 0, x );
	s.setField( 1, y );
	window.get_size( x, y );
	s.setField( 2, x );
	s.setField( 3, y );
}

void Settings::getWindowGeometry( const std::string& path, const std::string& name, Gtk::Window& window, int x, int y, int w, int h )
{
	Storage& s = selectPath(path);

	bool hasItem = s.findItem(name);
	if( hasItem ) hasItem = s.checkFormat("IIII");
	
	if( hasItem ) {
		window.move( s.integerField(0), s.integerField(1) );
		window.resize( s.integerField(2), s.integerField(3) );
	} else {
		if( x != -1 )
			window.move( x, y );
		if( w != -1 )
			window.resize( w, h );
	}
}

} // namespace ...
