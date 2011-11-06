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

