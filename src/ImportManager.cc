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

#include "ImportManager.h"
#include "Importer.h"
#include "Project.h"
#include "Settings.h"
#include "Register.h"
#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <iostream>

namespace Polka { 


/******************
 * Import Manager *
 ******************/

ImportManager::ImportManager()
	: m_pFileChooser(0), m_pCurrentImporter(0)
{
	registerImporters( *this );
	//initPlugIns();
}

ImportManager::~ImportManager()
{
}

ImportManager& ImportManager::get()
{
	static ImportManager im;
	
	return im;
}

void ImportManager::registerImporter( Importer *imp )
{
	m_Importers.push_back(imp);
}

void ImportManager::executeFileImport( Gtk::Window& parent, Project& project )
{
	m_pFileChooser = new Gtk::FileChooserDialog("Import File", Gtk::FILE_CHOOSER_ACTION_OPEN);
	m_pFileChooser->set_transient_for(parent);

	// Add response buttons the the dialog:
	m_pFileChooser->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	m_pFileChooser->add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	// add preview
	m_pFileChooser->signal_update_preview().connect( sigc::mem_fun(*this, &ImportManager::updatePreview ) );
	m_pFileChooser->set_use_preview_label(false);
	m_pFileChooser->get_widget_for_response(Gtk::RESPONSE_OK)->set_sensitive(false);
	
	// set initial path
	std::string folder = Settings::get().getString( "", "ImportPath", "" );
	if( !folder.empty() ) m_pFileChooser->set_current_folder( folder );

	// Show the dialog and wait for a user response:
	int result = m_pFileChooser->run();

	if( result == Gtk::RESPONSE_OK ) {
		// check typed extension
		if( m_pCurrentImporter ) {
			m_pCurrentImporter->importToProject( project );
			m_pCurrentImporter = 0;
		}

		// store import path
		Settings::get().setValue( "", "ImportPath", m_pFileChooser->get_current_folder() );
	}
	// remove import window
	m_pFileChooser->hide();
	delete m_pFileChooser;
	m_pFileChooser = 0;
}

void ImportManager::updatePreview()
{
	// find converter that accepts the file
	std::string filename = m_pFileChooser->get_preview_filename();
	for( unsigned int i = 0; i < m_Importers.size(); i++ ) {
		if( m_Importers[i]->initImport(filename) ) {
			// importer accepts file
			m_pCurrentImporter = m_Importers[i];
			m_pFileChooser->set_preview_widget( m_pCurrentImporter->getPreviewWidget() );
			m_pFileChooser->set_preview_widget_active();
			m_pFileChooser->get_widget_for_response(Gtk::RESPONSE_OK)->set_sensitive();
			return;
		}
	}
	m_pFileChooser->set_preview_widget_active(false);
	m_pFileChooser->get_widget_for_response(Gtk::RESPONSE_OK)->set_sensitive(false);
	m_pCurrentImporter = 0;
}


} // namespace Polka 
