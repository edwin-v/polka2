#include "MainWindow.h"
#include "Editor.h"
#include "ObjectPropertiesDialog.h"
#include "PreferencesDialog.h"
#include "Settings.h"
#include "ImportManager.h"
#include "ResourceManager.h"
#include <glibmm/i18n.h>
#include <glibmm/convert.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/aboutdialog.h>
#include <iostream>
#include <algorithm>
#include <limits>


namespace Polka {

MainWindow::MainWindow()
	: m_pProject(0), m_EditorMenuId(0)
{
	set_title("Polka 2");
	std::vector< Glib::RefPtr<Gdk::Pixbuf> > icons;
	icons.push_back( ResourceManager::get().getIcon("p2icon_48") );
	set_default_icon_list( icons );
	add(m_MainBox);
	
	// build the menus and toolbars
	buildActions();
	
	//Get the menubar and toolbar widgets, and add them to a container widget:
	Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
	if(pMenubar)
		m_MainBox.pack_start(*pMenubar, Gtk::PACK_SHRINK);

	Gtk::Widget* pToolbar = m_refUIManager->get_widget("/ToolBar") ;
	if(pToolbar)
		m_MainBox.pack_start(*pToolbar, Gtk::PACK_SHRINK);
	
	// add the main layout box
	m_MainBox.pack_start(m_LayoutPane);
	
	m_TreeFrame.set_shadow_type( Gtk::SHADOW_IN );
	m_TreeFrame.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	
	// prepare main area
	m_EditBox.pack_start(m_EditorTitle, Gtk::PACK_SHRINK);
	m_EditBox.pack_start(m_MainFrame);
	m_MainFrame.set_shadow_type( Gtk::SHADOW_NONE );
	
	m_LayoutPane.add1(m_TreeFrame);
	m_LayoutPane.add2(m_EditBox);
	m_LayoutPane.set_border_width(4);
	show_all_children();
	
	m_MainEditor = 0;

	// set editor title bold
	Pango::AttrList atts;
	Pango::AttrInt attr = Pango::Attribute::create_attr_weight(Pango::WEIGHT_BOLD);
	atts.insert(attr);
	m_EditorTitle.set_attributes(atts);


	// retrieve settings
	Settings::get().getWindowGeometry( "", "MainWindowGeometry", *this, -1, -1, 780, 600 );
	m_LayoutPane.set_position( Settings::get().getInteger( "", "MainWindowTreeSize", 200 ) );

	// init history
	m_HistoryWindow.setActions( m_refActionGroup->get_action("EditUndo"), m_refActionGroup->get_action("EditRedo") );
	if( Settings::get().getBool( "", "HistoryVisible", false ) ) onViewHistory();
}

MainWindow::~MainWindow()
{
}

void MainWindow::buildActions()
{
	// create group for menu and toolbar actions
	m_refActionGroup = Gtk::ActionGroup::create("MainWindow");

	// File menu
	m_refActionGroup->add(Gtk::Action::create("FileMenu", _("File")));
	m_refActionGroup->add(Gtk::Action::create("FileNew", Gtk::Stock::NEW),
	                      sigc::mem_fun(*this, &MainWindow::onFileNew));
	m_refActionGroup->add(Gtk::Action::create("FileOpen", Gtk::Stock::OPEN),
	                      sigc::mem_fun(*this, &MainWindow::onFileOpen));
	m_refActionGroup->add(Gtk::Action::create("FileSave", Gtk::Stock::SAVE),
	                      sigc::mem_fun(*this, &MainWindow::onFileSave));
	m_refActionGroup->add(Gtk::Action::create("FileSaveAs", Gtk::Stock::SAVE_AS),
	                      sigc::mem_fun(*this, &MainWindow::onFileSaveAs));
	m_refActionGroup->add(Gtk::Action::create("FileImport", _("_Import")),
	                      sigc::mem_fun(*this, &MainWindow::onFileImport));
	m_refActionGroup->add(Gtk::Action::create("FileClose", Gtk::Stock::CLOSE),
	                      sigc::mem_fun(*this, &MainWindow::onFileClose));
	m_refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
	                      sigc::mem_fun(*this, &MainWindow::onFileQuit));
	// create recent file list
	m_refRecent = Gtk::RecentAction::create("FileRecent", _("Open _Recent"));
	m_refRecent->signal_item_activated().connect( sigc::mem_fun(*this, &MainWindow::onFileRecent) );
	Glib::RefPtr<Gtk::RecentFilter> recentFilter = Gtk::RecentFilter::create();
	recentFilter->add_pattern("*.ppp");
	m_refRecent->add_filter(recentFilter);
	m_refRecent->set_sort_type(Gtk::RECENT_SORT_MRU);
	m_refActionGroup->add(m_refRecent);
	


	// Edit menu
	m_refActionGroup->add(Gtk::Action::create("EditMenu", _("Edit")));
	m_refActionGroup->add(Gtk::Action::create("EditUndo", Gtk::Stock::UNDO),
	                      sigc::mem_fun(m_HistoryWindow, &HistoryWindow::onUndoClick));
	m_refActionGroup->add(Gtk::Action::create("EditRedo", Gtk::Stock::REDO),
	                      sigc::mem_fun(m_HistoryWindow, &HistoryWindow::onRedoClick));
	m_refActionGroup->add(Gtk::Action::create("EditCut", Gtk::Stock::CUT),
	                      sigc::mem_fun(*this, &MainWindow::onEditCut));
	m_refActionGroup->add(Gtk::Action::create("EditCopy", Gtk::Stock::COPY),
	                      sigc::mem_fun(*this, &MainWindow::onEditCopy));
	m_refActionGroup->add(Gtk::Action::create("EditPaste", Gtk::Stock::PASTE),
	                      sigc::mem_fun(*this, &MainWindow::onEditPaste));
	m_refActionGroup->add(Gtk::Action::create("EditDelete", Gtk::Stock::DELETE),
	                      sigc::mem_fun(*this, &MainWindow::onEditDelete));
	m_refActionGroup->add(Gtk::Action::create("EditRename", _("Re_name")),
	                      Gtk::AccelKey("F2"),
	                      sigc::mem_fun(*this, &MainWindow::onEditRename));
	m_refActionGroup->add(Gtk::Action::create("EditProperties", Gtk::Stock::PROPERTIES),
	                      sigc::mem_fun(*this, &MainWindow::onEditProperties));
	m_refActionGroup->add(Gtk::Action::create("EditPreferences", Gtk::Stock::PREFERENCES),
	                      sigc::mem_fun(*this, &MainWindow::onEditPreferences));

	// View menu
	m_refActionGroup->add(Gtk::Action::create("ViewMenu", _("View")));
	m_refActionGroup->add(Gtk::Action::create("ViewHistory", _("_History")),
	                      sigc::mem_fun(*this, &MainWindow::onViewHistory));

	// Help menu
	m_refActionGroup->add(Gtk::Action::create("HelpMenu", _("Help")));
	m_refActionGroup->add(Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT),
	                      sigc::mem_fun(*this, &MainWindow::onHelpAbout));

	m_refUIManager = Gtk::UIManager::create();
	m_refUIManager->insert_action_group(m_refActionGroup);

	add_accel_group(m_refUIManager->get_accel_group());

	// init actions
	m_refActionGroup->get_action("FileClose")->set_sensitive(false);
	m_refActionGroup->get_action("FileSave")->set_sensitive(false);
	m_refActionGroup->get_action("FileSaveAs")->set_sensitive(false);
	m_refActionGroup->get_action("FileImport")->set_sensitive(false);
	m_refActionGroup->get_action("EditUndo")->set_sensitive(false);
	m_refActionGroup->get_action("EditRedo")->set_sensitive(false);

	// Create the menubar and toolbars
	Glib::ustring ui_info = 
		"<ui>"
		"  <menubar name='MenuBar'>"
		"    <menu action='FileMenu'>"
		"      <menuitem action='FileNew'/>"
		"      <menuitem action='FileOpen'/>"
		"      <menuitem action='FileRecent'/>"
		"      <separator/>"
		"      <menuitem action='FileSave'/>"
		"      <menuitem action='FileSaveAs'/>"
		"      <separator/>"
		"      <menuitem action='FileImport'/>"
		"      <separator/>"
		"      <menuitem action='FileClose'/>"
		"      <menuitem action='FileQuit'/>"
		"    </menu>"
		"    <menu action='EditMenu'>"
		"      <menuitem action='EditUndo'/>"
		"      <menuitem action='EditRedo'/>"
		"      <separator/>"
		"      <menuitem action='EditCut'/>"
		"      <menuitem action='EditCopy'/>"
		"      <menuitem action='EditPaste'/>"
		"      <menuitem action='EditDelete'/>"
		"      <separator/>"
		"      <menuitem action='EditRename'/>"
		"      <menuitem action='EditProperties'/>"
		"      <separator/>"
		"      <menuitem action='EditPreferences'/>"
		"    </menu>"
		"    <menu action='ViewMenu'>"
		"      <menuitem action='ViewHistory'/>"
		"    </menu>"
		"    <placeholder name='EditorMenu'/>"
		"    <menu action='HelpMenu'>"
		"      <menuitem action='HelpAbout'/>"
		"    </menu>"
		"  </menubar>"
		"  <toolbar  name='ToolBar'>"
		"    <toolitem action='FileNew'/>"
		"    <toolitem action='FileSave'/>"
		"    <separator/>"
		"    <toolitem action='EditUndo'/>"
		"    <toolitem action='EditRedo'/>"
		"    <separator/>"
		"    <toolitem action='EditCut'/>"
		"    <toolitem action='EditCopy'/>"
		"    <toolitem action='EditPaste'/>"
		"    <toolitem action='EditDelete'/>"
		"    <separator/>"
		"    <toolitem action='EditProperties'/>"
		"  </toolbar>"
		"</ui>";	

	m_refUIManager->add_ui_from_string(ui_info);
}

bool MainWindow::on_delete_event( GdkEventAny * )
{
	if( !m_pProject ) return false;

	if( m_ModifiedCounter != 0 ) {
		Gtk::MessageDialog msg( *this, _("Your current project has unsaved changes!"), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_NONE );
		msg.set_secondary_text( _("Do you want to save it before closing?") );
		msg.add_button( Gtk::Stock::CLOSE, 1002 );
		msg.add_button( Gtk::Stock::CANCEL, 0 );
		msg.add_button( Gtk::Stock::SAVE, 1001 );
		msg.set_default_response(1001);
		switch( msg.run() ) {
			case 1001: 
				onFileSave();
				break;
			case 1002:
				break;
			default:
				return true;
		}
	}

	// remove tree from layout
	m_TreeFrame.remove();
	// remove history
	m_HistoryWindow.setUndoHistory(0);
	// remove project
	delete m_pProject;
	m_pProject = 0;
	// unactivate close
	m_refActionGroup->get_action("FileClose")->set_sensitive(false);
	m_refActionGroup->get_action("FileSave")->set_sensitive(false);
	m_refActionGroup->get_action("FileSaveAs")->set_sensitive(false);
	m_refActionGroup->get_action("FileImport")->set_sensitive(false);
	return false;
}

void MainWindow::on_hide()
{
	// first hide editor windows (store settings)
	ObjectManager::get().hideEditors();
	// store history visibility
	Settings::get().setValue( "", "HistoryVisible", m_HistoryWindow.get_visible() );
	// then hide self
	Settings::get().setWindowGeometry( "", "MainWindowGeometry", *this );
	Settings::get().setValue( "", "MainWindowTreeSize", m_LayoutPane.get_position() );
	m_HistoryWindow.hide();
	Gtk::Window::on_hide();
}

void MainWindow::addRecentFile( const std::string& file )
{
	Glib::RefPtr<Gtk::RecentManager> recentMgr = Gtk::RecentManager::get_default();
	recentMgr->add_item( Glib::filename_to_uri(file) );
}

void MainWindow::onFileNew()
{
	if( m_pProject )
		if( on_delete_event() )
			return;

	// create a project
	m_pProject = new Project(m_refUIManager);
	m_pProject->signal_row_activated().connect(sigc::mem_fun(*this,
	                    &MainWindow::onTreeActivate) );	

	// link it to the layout
	m_TreeFrame.add( *m_pProject );
	m_pProject->show();
	
	// connect project signals
	m_pProject->signalEditObject().connect(sigc::mem_fun(*this,
			&MainWindow::activateEditor) );
	m_pProject->signalTreeUpdate().connect( sigc::mem_fun(*this,
			&MainWindow::treeUpdated) );

	// set history 
	m_ModifiedCounter = 0;
	m_pProject->undoHistory().signalHistoryChanged().connect( sigc::mem_fun(*this, &MainWindow::changeModifiedStatus ) );
	m_HistoryWindow.setUndoHistory( &m_pProject->undoHistory() );
	m_refActionGroup->get_action("FileClose")->set_sensitive(true);
	m_refActionGroup->get_action("FileSaveAs")->set_sensitive(true);
	m_refActionGroup->get_action("FileImport")->set_sensitive(true);
}

void MainWindow::onFileOpen()
{
	if( m_pProject )
		if( on_delete_event() )
			return;

	Gtk::FileChooserDialog dialog("Open Project File", Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);

	// Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	// Add project filter
	Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
	filter->set_name("Polka 2 project files");
	filter->add_mime_type("application/x-polka2-project");
	filter->add_pattern("*.ppp");
	dialog.add_filter(filter);

	// Show the dialog and wait for a user response:
	int result = dialog.run();

	if( result == Gtk::RESPONSE_OK ) {
		// check typed extension
		std::string fname = dialog.get_filename();
		// create an empty projet
		onFileNew();
		// load the project
		m_pProject->loadFromFile( fname );
		addRecentFile(fname);
	}
}

void MainWindow::onFileRecent()
{
	if( m_pProject )
		if( on_delete_event() )
			return;

	// check typed extension
	std::string fname = Glib::filename_from_uri( m_refRecent->get_current_uri() );
	// create an empty projet
	onFileNew();
	// load the project
	m_pProject->loadFromFile( fname );
}

void MainWindow::onFileSave()
{
	if( m_pProject->filename().empty() )
		onFileSaveAs();
	else {
		int res = m_pProject->save();
		// check result
		if( res == 0 ) { 
			m_ModifiedCounter = 0;
			addRecentFile(m_pProject->filename());
		}
	}
}

void MainWindow::onFileSaveAs()
{
	Gtk::FileChooserDialog dialog("Save Project File As", Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	// Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	// Add project filter
	Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
	filter->set_name("Polka 2 project files");
	filter->add_mime_type("application/x-polka2-project");
	filter->add_pattern("*.ppp");
	dialog.add_filter(filter);

	// Show the dialog and wait for a user response:
	int result = dialog.run();

	if( result == Gtk::RESPONSE_OK ) {
		// check typed extension
		std::string fname = dialog.get_filename();
		std::string ext = "    ";
		std::transform(fname.end()-4, fname.end(), ext.begin(), tolower);
		// add extension if not there
		if( ext != ".ppp" ) fname += ".ppp";
		// save to selected file
		if( m_pProject->saveToFile( fname ) == 0 ) {
			m_ModifiedCounter = 0;
			addRecentFile(m_pProject->filename());
		}
	}
}

void MainWindow::onFileImport()
{
	if( m_pProject )
		ImportManager::get().executeFileImport(*this, *m_pProject);
}

void MainWindow::onFileClose()
{
	on_delete_event(0);
}

void MainWindow::onFileQuit()
{
	if( !on_delete_event(0) ) {
		hide();
	}
}

void MainWindow::onEditCopy()
{
}

void MainWindow::onEditCut()
{
}

void MainWindow::onEditPaste()
{
}

void MainWindow::onEditDelete()
{
	if( m_pProject )
		m_pProject->deleteObject();
}

void MainWindow::onEditRename()
{
	if( m_pProject )
		m_pProject->startRename();
}

void MainWindow::onEditProperties()
{
	Polka::Object *obj = m_pProject->getObject();
	if( obj ) {
		ObjectPropertiesDialog objDlg( *obj );
		objDlg.run();
	}
}

void MainWindow::onEditPreferences()
{
	PreferencesDialog pd;
	pd.run();
}

void MainWindow::onViewHistory()
{
	m_HistoryWindow.present();
}

void MainWindow::onHelpAbout()
{
	Gtk::AboutDialog ad;
	ad.set_program_name("Polka2");
	ad.set_version("0.5.pre");
	ad.set_copyright("Copyright (c) 2010-1011 by Infinite");
	ad.set_comments(_("Polka2 is an environment for retro development."));
	ad.set_license(_("This software is currently licensed under the 'do as Edwin says' license."));
	ad.set_license_type(Gtk::LICENSE_CUSTOM);
	ad.set_website("http://www.infinitemsx.com");
	ad.set_website_label(_("Go to the Infinite website."));
	ad.set_authors({_("Edwin")});
	ad.run();
}

void MainWindow::onTreeActivate(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* /*column*/)
{
	if( !m_pProject ) return;
	
	Polka::Object *object = m_pProject->getObject( path );
	if( object ) {
		Editor *objEdit = ObjectManager::get().getObjectEditor( object->id() );
		if( objEdit ) {
			objEdit->setObject( object );
			activateEditor( objEdit );
		}
	}
}

void MainWindow::changeModifiedStatus( UndoHistory::ChangeType type )
{
	switch(type) {
		case UndoHistory::CHANGE_ADDUNDO:
			if( m_ModifiedCounter < 0 )
				m_ModifiedCounter = std::numeric_limits<int>::max()/2;
			else
				m_ModifiedCounter++;
			break;
		case UndoHistory::CHANGE_UNDOACTION:
			m_ModifiedCounter--;
			break;
		case UndoHistory::CHANGE_REDOACTION:
			m_ModifiedCounter++;
			break;
		case UndoHistory::CHANGE_NEWDISPLAY:
			break;
		default:
			if( m_ModifiedCounter != 0 )
				m_ModifiedCounter = std::numeric_limits<int>::max()/2;
			break;
	}
	m_refActionGroup->get_action("FileSave")->set_sensitive( m_ModifiedCounter );
}

void MainWindow::activateEditor( Editor *edt )
{
	// hook up signal
	m_pProject->signalTreeUpdate().connect( sigc::mem_fun(*edt, &Editor::treeUpdated) );

	// start main or windowed editor
	if( edt->isMainEditor() ) {
		// remove current editor
		Gtk::Widget *w = m_MainFrame.get_child();
		if( w ) {
			m_MainFrame.remove();
			dynamic_cast<Editor*>(w)->hide();
		}
		// remove focus menu
		if( m_EditorMenuId )
			m_refUIManager->remove_ui( m_EditorMenuId );
		// add to main window
		m_MainFrame.add(*edt);
		// add its menu (if any)
		const Glib::ustring& menu_string = edt->menuString();
		if( !menu_string.empty() )
			m_EditorMenuId = m_refUIManager->add_ui_from_string(menu_string);
		// show editor
		m_EditorTitle.set_text( edt->title() );
		m_MainFrame.show_all_children();
	} else {
		edt->activate();
	}
}

void MainWindow::treeUpdated()
{
	// update displayed names
	Gtk::Widget *w = m_MainFrame.get_child();
	if(w) {
		m_EditorTitle.set_text( dynamic_cast<Editor*>(w)->title() );
	}
}

} // namespace Polka

