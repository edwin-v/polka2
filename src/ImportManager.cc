#include "ImportManager.h"
#include "Importer.h"
#include "Project.h"
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
	
	// Show the dialog and wait for a user response:
	int result = m_pFileChooser->run();

	if( result == Gtk::RESPONSE_OK ) {
		// check typed extension
		if( m_pCurrentImporter ) {
			m_pCurrentImporter->importToProject( project );
			m_pCurrentImporter = 0;
		}
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
