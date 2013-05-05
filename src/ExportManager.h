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

#ifndef _POLKA_EXPORTMANAGER_H_
#define _POLKA_EXPORTMANAGER_H_

#include <gtkmm/window.h>
#include <gtkmm/filechooserdialog.h>
#include <glibmm/ustring.h>
#include <vector>

namespace Polka { 

class Exporter;
class Project;
class Object;

class ExportManager
{
public:
	static ExportManager& get();


	// object registration
	void registerExporter( Exporter *exp );

	// main interface
	bool canExport( const Object& obj );
	void executeFileExport( /*Gtk::Window& parent,*/ Object& obj );

private:
	ExportManager();
	~ExportManager();

	void exportOptions();
	
	Gtk::FileChooserDialog *m_pFileChooser;
	std::vector<Exporter*> m_Exporters;
	std::map<Glib::RefPtr<Gtk::FileFilter>, Exporter*> m_ActiveFilters;
	Glib::RefPtr<Gtk::FileFilter> m_CurrentFilter;
	Object *m_pCurrentObject;
};


} // namespace Polka 

#endif // _POLKA_IMPORTMANAGER_H_
