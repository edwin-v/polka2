#include "ResourceManager.h"
#include "icons/icons.c"
#include "cursors/cursors.c"


namespace Polka {

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

ResourceManager& ResourceManager::get()
{
	static ResourceManager instance;
	return instance;
}

Glib::RefPtr<Gdk::Pixbuf> ResourceManager::getIcon( const std::string& name )
{
	IconMap::iterator it = m_Icons.find( name );
	// return existing
	if( it != m_Icons.end() ) return it->second;
	
	// find new icon
	int i = 0;
	while( Icons[i].pixbuf != 0 ) {
		if( Icons[i].name == name ) {
			Glib::RefPtr<Gdk::Pixbuf> newicon = Gdk::Pixbuf::create_from_inline( -1, Icons[i].pixbuf );
			m_Icons[name] = newicon;
			return newicon;
		}
		i++;
	}	
	
	// nothing found, return default icon
	Glib::RefPtr<Gdk::Pixbuf> noicon;
	return noicon;
}

Glib::RefPtr<Gdk::Cursor> ResourceManager::getCursor( const Glib::RefPtr< Gdk::Display >& display, const std::string& name )
{
	CursorMap::iterator it = m_Cursors.find( name );
	// return existing
	if( it != m_Cursors.end() ) return it->second;
	
	// find new icon
	int i = 0;
	while( Cursors[i].pixbuf != 0 ) {
		if( Cursors[i].name == name ) {
			Glib::RefPtr<Gdk::Pixbuf> newimg = Gdk::Pixbuf::create_from_inline( -1, Cursors[i].pixbuf );
			Glib::RefPtr<Gdk::Cursor> newcur = Gdk::Cursor::create( display, newimg, Cursors[i].x, Cursors[i].y );
			m_Cursors[name] = newcur;
			return newcur;
		}
		i++;
	}	
	
	// nothing found, return default icon
	Glib::RefPtr<Gdk::Cursor> defcur = Gdk::Cursor::create(display, Gdk::X_CURSOR);
	return defcur;
}

Glib::RefPtr<Gdk::Cursor> ResourceManager::getCursor( const Glib::RefPtr< Gdk::Window >& window, const std::string& name )
{
	return getCursor( window->get_display(), name );
}

} // namespace ...

