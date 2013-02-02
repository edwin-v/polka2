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

#include "StorageHelpers.h"
#include "Storage.h"

namespace Polka {
	
// action names
const char *ACTION_RENAME = "rename";
const char *ACTION_COMMENTS = "comments";
const char *ACTION_CREATE = "create";
const char *ACTION_OBJECTS = "objects";
const char *ACTION_CREATEFOLDER = "folder";
const char *ACTION_DELETE = "delete";
const char *ACTION_MULTIPLE = "multiple";



void storageRename( Storage& s, const Glib::ustring& from, const Glib::ustring& to )
{
	s.createItem("NAMES", "SS");
	s.setField( 0, from );
	s.setField( 1, to );
}

void storageRename( Storage& s, guint32 oid, const Glib::ustring& to )
{
	s.createItem("NAME_ID", "IS");
	s.setField( 0, int(oid) );
	s.setField( 1, to );
}

void storageComments( Storage& s, guint32 oid, const Glib::ustring& comments )
{
	s.createItem("COMMENTS", "IS");
	s.setField( 0, int(oid) );
	s.setField( 1, comments );
}

void storageSetRect( Storage& s, const std::string& name, const Gdk::Rectangle& rect )
{
	s.createItem( name, "IIII" );
	s.setField( 0, rect.get_x() );
	s.setField( 1, rect.get_y() );
	s.setField( 2, rect.get_width() );
	s.setField( 3, rect.get_height() );
}

bool storageGetRect( Storage& s, const std::string& name, Gdk::Rectangle& rect )
{
	if( s.findItem( name ) )
		if( s.checkFormat("IIII") ) {
			rect.set_x(      s.integerField(0) );
			rect.set_y(      s.integerField(1) );
			rect.set_width(  s.integerField(2) );
			rect.set_height( s.integerField(3) );
			return true;
		}
	return false;
}

void storageSetObjectName( Storage& s, const std::string& name )
{
	s.createItem("OBJECT_NAME", "S");
	s.setField( 0, name );
}


}
