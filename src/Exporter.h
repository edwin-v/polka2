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

#ifndef _POLKA_EXPORTER_H_
#define _POLKA_EXPORTER_H_

#include <string>
#include <vector>
#include <gtkmm/filefilter.h>

namespace Gtk {
	class Dialog;
	class Window;
}

namespace Polka {

class Project;
class Object;

class Exporter
{
public:
	Exporter( const std::string& _id );
	virtual ~Exporter();

	void setObject( const Object *obj );
	
	virtual std::vector< Glib::RefPtr<Gtk::FileFilter> > fileFilters( const Object& obj ) const = 0;
	virtual void setActiveFilter( Glib::RefPtr<Gtk::FileFilter> filter ) = 0;

	virtual bool showExportOptions( Gtk::Window& parent );

	virtual bool exportObject( const std::string& filename ) = 0;

protected:
	const Object *m_pObject;

	virtual void initObject();
	
private:
	std::string m_Id;
};

} // namespace Polka

#endif // _POLKA_IMPORTER_H_
