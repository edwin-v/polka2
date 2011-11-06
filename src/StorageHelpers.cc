#include "StorageHelpers.h"
#include "Storage.h"

namespace Polka {
	
// action names
const char *ACTION_RENAME = "rename";
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
