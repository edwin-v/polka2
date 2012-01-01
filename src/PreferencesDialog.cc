#include "PreferencesDialog.h"
#include "ResourceManager.h"
#include "Settings.h"
#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <gtkmm/scrolledwindow.h>


namespace Polka {

PreferencesDialog::PreferencesDialog()
	: Dialog(_("Preferences"), true)
{
	set_border_width(5);
	set_default_size(700, 500);
	
	add_button( Gtk::Stock::CLOSE, 1 );

	Gtk::ScrolledWindow *swin = manage(new Gtk::ScrolledWindow);
	swin->set_shadow_type( Gtk::SHADOW_IN );
	swin->add(m_CategoryList);
	swin->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

	m_MainBox.pack_start( *swin, Gtk::PACK_SHRINK );
	m_MainBox.pack_start( m_PrefsPages, Gtk::PACK_EXPAND_WIDGET );
	m_MainBox.set_spacing(5);

	get_content_area()->pack_start( m_MainBox, Gtk::PACK_EXPAND_WIDGET );

	m_refListModel = Gtk::ListStore::create(m_Columns);
	m_CategoryList.set_model(m_refListModel);
	m_CategoryList.append_column( "I", m_Columns.m_ColIcon );
	m_CategoryList.append_column( "N", m_Columns.m_ColName );
	m_CategoryList.set_headers_visible(false);

	m_PrefsPages.set_show_border(false);
	m_PrefsPages.set_show_tabs(false);

	ResourceManager& rm = ResourceManager::get();
	// add preferences pages
	addPrefsPage( rm.getIcon("preferences_accelerators"), _("Mouse/keyboard"), m_AcceleratorPrefs );

	// restore settings
	Settings::get().getWindowGeometry( "Preferences", "WindowGeometry", *this, -1, -1, 775, 550 );

	show_all_children();
}

PreferencesDialog::~PreferencesDialog()
{
	Settings::get().setWindowGeometry( "Preferences", "WindowGeometry", *this );
}

void PreferencesDialog::on_response( int id )
{
	// stub
}

void PreferencesDialog::addPrefsPage( Glib::RefPtr<Gdk::Pixbuf> icon, const Glib::ustring& name, Gtk::Widget& w )
{
	int id = m_PrefsPages.get_n_pages();
	m_PrefsPages.append_page(w, name);
	
	auto it = m_refListModel->append();
	Gtk::TreeModel::Row row = *it;
	row[m_Columns.m_ColIcon] = icon;
	row[m_Columns.m_ColName] = name;
	row[m_Columns.m_PageId] = id;
}

} // namespace ...

