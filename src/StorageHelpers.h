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

#ifndef _POLKA_STORAGEHELPERS_H_
#define _POLKA_STORAGEHELPERS_H_

#include <glibmm/i18n.h>
#include <glibmm/ustring.h>
#include <gdkmm/rectangle.h>

namespace Polka {

class Storage;

// action names
extern const char *ACTION_RENAME;
extern const char *ACTION_COMMENTS;
extern const char *ACTION_CREATE;
extern const char *ACTION_OBJECTS;
extern const char *ACTION_CREATEFOLDER;
extern const char *ACTION_DELETE;
extern const char *ACTION_MULTIPLE;

void storageRename( Storage& s, const Glib::ustring& from, const Glib::ustring& to );
void storageRename( Storage& s, guint32 oid, const Glib::ustring& to );
void storageComments( Storage& s, guint32 oid, const Glib::ustring& comments );
void storageSetRect( Storage& s, const std::string& name, const Gdk::Rectangle& rect );
bool storageGetRect( Storage& s, const std::string& name, Gdk::Rectangle& rect );
void storageSetObjectName( Storage& s, const std::string& name );

} // namespace Polka

#endif // _POLKA_STORAGEHELPERS_H_

