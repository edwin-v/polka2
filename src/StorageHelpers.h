#ifndef _POLKA_STORAGEHELPERS_H_
#define _POLKA_STORAGEHELPERS_H_

#include <glibmm/i18n.h>
#include <glibmm/ustring.h>
#include <gdkmm/rectangle.h>

namespace Polka {

class Storage;

// action names
extern const char *ACTION_RENAME;
extern const char *ACTION_CREATE;
extern const char *ACTION_OBJECTS;
extern const char *ACTION_CREATEFOLDER;
extern const char *ACTION_DELETE;
extern const char *ACTION_MULTIPLE;

void storageRename( Storage& s, const Glib::ustring& from, const Glib::ustring& to );
void storageSetRect( Storage& s, const std::string& name, const Gdk::Rectangle& rect );
bool storageGetRect( Storage& s, const std::string& name, Gdk::Rectangle& rect );
void storageSetObjectName( Storage& s, const std::string& name );

} // namespace Polka

#endif // _POLKA_STORAGEHELPERS_H_

