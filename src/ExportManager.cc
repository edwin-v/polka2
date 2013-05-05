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

#include "ExportManager.h"
#include "Exporter.h"
#include "Project.h"
#include "Settings.h"
#include "Register.h"
#include <gtkmm/messagedialog.h>
#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <iostream>

namespace Polka { 


/******************
 * Export Manager *
 ******************/

ExportManager::ExportManager()
	: m_pFileChooser(0), m_pCurrentObject(0)
{
	registerExporters( *this );
	//initPlugIns();
}

ExportManager::~ExportManager()
{
}

ExportManager& ExportManager::get()
{
	static ExportManager em;
	
	return em;
}

void ExportManager::registerExporter( Exporter *exp )
{
	m_Exporters.push_back(exp);
}

bool ExportManager::canExport( const Object& obj )
{
	for( unsigned int i = 0; i < m_Exporters.size(); i++ )
		if( m_Exporters[i]->fileFilters(obj).size() > 0 )
			return true;
	return false;
}

void ExportManager::executeFileExport( /*Gtk::Window& parent,*/ Object& obj )
{
	m_pFileChooser = new Gtk::FileChooserDialog("Export File", Gtk::FILE_CHOOSER_ACTION_SAVE);

	// Add response buttons the the dialog:
	m_pFileChooser->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	m_pFileChooser->add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	// add options button to dialog
	Gtk::Button expbut( _("Export options ...") );
	expbut.signal_clicked().connect( sigc::mem_fun(*this, &ExportManager::exportOptions ) );
	Gtk::HBox box;
	box.pack_end(expbut, false, false);
	box.show_all_children();
	m_pFileChooser->set_extra_widget( box );
	// keep button centered (file dialog will not allow other alignment options)
	m_pFileChooser->signal_check_resize().connect( [this, &expbut]() { 
		m_pFileChooser->get_extra_widget()->set_size_request( (m_pFileChooser->get_content_area()->get_width() + expbut.get_width() )/2, -1); 
	} );

	// add filters
	m_ActiveFilters.clear();
	for( unsigned int i = 0; i < m_Exporters.size(); i++ ) {
		auto filters = m_Exporters[i]->fileFilters(obj);
		for( unsigned int j = 0; j < filters.size(); j++ ) {
			m_pFileChooser->add_filter( filters[j] );
			m_ActiveFilters[ filters[j] ] = m_Exporters[i];
		}
	}
	m_CurrentFilter.clear();

	// set initial path and name
	m_pCurrentObject = &obj;
	m_pFileChooser->set_current_name( obj.name() );
	std::string folder = Settings::get().getString( "", "ExportPath", "" );
	if( !folder.empty() ) m_pFileChooser->set_current_folder( folder );

	// run the save diallog
	int result = m_pFileChooser->run();

	if( result == Gtk::RESPONSE_OK ) {

		// get the selected exporter
		auto exporter = m_ActiveFilters[ m_pFileChooser->get_filter() ];

		// set object if not done so through export options
		if( m_pFileChooser->get_filter() != m_CurrentFilter ) {
			exporter->setActiveFilter( m_CurrentFilter );
			exporter->setObject( m_pCurrentObject );
		}

		// export object to file
		bool res = exporter->exportObject( m_pFileChooser->get_filename() );
		
		if( !res ) {
			Gtk::MessageDialog m(*m_pFileChooser, _("Could not export this object to the file."));
			m.run();
		}

		// store export path
		Settings::get().setValue( "", "ExportPath", m_pFileChooser->get_current_folder() );
	}

	// remove import window
	m_pFileChooser->hide();
	delete m_pFileChooser;
	m_pFileChooser = 0;
	m_pCurrentObject = 0;
}

void ExportManager::exportOptions()
{
	// find the current converter
	auto exporter = m_ActiveFilters[ m_pFileChooser->get_filter() ];

	// set current filter and reset options if changed
	if( m_CurrentFilter != m_pFileChooser->get_filter() ) {
		m_CurrentFilter = m_pFileChooser->get_filter();
		exporter->setActiveFilter( m_CurrentFilter );
		exporter->setObject( m_pCurrentObject );
	}

	// show options
	if( !exporter->showExportOptions(*m_pFileChooser) ) {
		
		Gtk::MessageDialog m(*m_pFileChooser, _("This exporter format has no options."));
		m.run();
		
	}
}


} // namespace Polka 
