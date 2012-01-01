#include "AcceleratorPrefs.h"
#include "AccelManager.h"
#include "ObjectManager.h"
#include "Settings.h"
#include "gtkmm/scrolledwindow.h"
#include <glibmm/i18n.h>
#include <iostream>
using namespace std;

namespace Polka {

AcceleratorPrefs::AcceleratorPrefs()
	: m_RadioDisable( _("Disable") ),
	  m_RadioLink( _("Link to:") ),
	  m_RadioSet( _("Set combination:") ),
	  m_NameLabel("", 0.0, 0.5),
	  m_TypeLabel( _("Action definition:"), 0.0, 0.5), m_AccelLabel("", 0.0, 0.5),
	  m_Updating(false)
{
	set_border_width(0);
	set_row_spacing(4);
	set_column_spacing(4);
	
	Gtk::VBox *listbox = manage(new Gtk::VBox);
	Gtk::Label *l = manage(new Gtk::Label(_("Action list:"), 0.0, 0.5));
	Gtk::ScrolledWindow *swin = manage(new Gtk::ScrolledWindow);
	swin->set_shadow_type( Gtk::SHADOW_IN );
	swin->add(m_AccTree);
	swin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	swin->set_hexpand();
	swin->set_vexpand();
	swin->set_size_request(100, 50);

	listbox->pack_start(*l, Gtk::PACK_SHRINK);
	listbox->pack_start(*swin, Gtk::PACK_EXPAND_WIDGET);

	Gtk::VBox *defbox = manage(new Gtk::VBox);
	swin = manage(new Gtk::ScrolledWindow);
	swin->set_shadow_type( Gtk::SHADOW_IN );
	swin->add(m_DescText);
	swin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	swin->set_hexpand();
	swin->set_vexpand();
	swin->set_size_request(100, 50);

	defbox->pack_start(m_NameLabel, Gtk::PACK_SHRINK);
	defbox->pack_start(*swin, Gtk::PACK_EXPAND_WIDGET);

	m_AccPane.pack1(*listbox, true, false);
	m_AccPane.pack2(*defbox, false, false);

	attach( m_AccPane, 0, 0, 3, 1 );
	attach( m_TypeLabel, 0, 1, 1, 1 );
	attach( m_RadioDisable, 0, 2, 1, 1 );
	attach( m_RadioLink, 0, 3, 1, 1 );
	attach( m_ComboLinks, 1, 3, 2, 1 );
	attach( m_RadioSet, 0, 4, 1, 1 );
	attach( m_AccelLabel, 1, 4, 1, 1);
	attach( m_GrabFrame, 2, 4, 1, 2);
	m_RadioLink.join_group( m_RadioDisable );
	m_RadioSet.join_group( m_RadioDisable );
	m_NameLabel.set_hexpand();
	m_NameLabel.set_vexpand(false);
	m_DescText.set_editable(false);
	m_DescText.set_wrap_mode(Gtk::WRAP_WORD);
	m_ComboLinks.set_hexpand();
	m_AccelLabel.set_size_request(250);
	m_GrabFrame.set_size_request( 75, 75 );
	m_GrabFrame.set_hexpand();
	
	// Create the Tree model
	m_refTreeModel = Gtk::TreeStore::create(m_Cols);
	m_AccTree.set_model(m_refTreeModel);
	m_AccTree.append_column( "N", m_Cols.m_ColName );
	m_AccTree.set_headers_visible(false);

	// set acc label bold
	Pango::AttrList atts;
	Pango::AttrInt attr = Pango::Attribute::create_attr_weight(Pango::WEIGHT_BOLD);
	atts.insert(attr);
	m_AccelLabel.set_attributes(atts);

	fillTree();
	reset();

	m_AccTree.signal_cursor_changed().connect(sigc::mem_fun(*this, &AcceleratorPrefs::onAccSelected) );
	m_RadioDisable.signal_toggled().connect(sigc::mem_fun(*this, &AcceleratorPrefs::onRadioChange) );
	m_RadioLink.signal_toggled().connect(sigc::mem_fun(*this, &AcceleratorPrefs::onRadioChange) );
	m_ComboLinks.signal_changed().connect( sigc::mem_fun(*this, &AcceleratorPrefs::onLinkChange) );
	m_RadioSet.signal_toggled().connect(sigc::mem_fun(*this, &AcceleratorPrefs::onRadioChange) );
	m_GrabFrame.signalButtonGrabbed().connect(sigc::mem_fun(*this, &AcceleratorPrefs::setButton) );
	m_GrabFrame.signalKeyGrabbed().connect(sigc::mem_fun(*this, &AcceleratorPrefs::setKey) );

	// restore saved settings
	m_AccPane.set_position( Settings::get().getInteger( "Preferences/MouseAndKeyboard", "ListSplitterSize", 300 ) );

	show_all_children();
}

AcceleratorPrefs::~AcceleratorPrefs()
{
	Settings::get().setValue( "Preferences/MouseAndKeyboard", "ListSplitterSize", m_AccPane.get_position() );
}

void AcceleratorPrefs::fillTree()
{
	ObjectManager& om = ObjectManager::get(); // leave this here to force initialisation before anything else
	AccelManager& am = AccelManager::get();
	// loop contexts
	std::vector<std::string> contexts = am.getContexts();
	auto cit = contexts.begin();
	while( cit != contexts.end() ) {

		// init context container
		Gtk::TreeModel::Row crow = *(m_refTreeModel->append());
		crow[m_Cols.m_ColName] = om.editorNameFromId(*cit);

		// get definition map
		const AccelManager::DefinitionMap& dm = am.getAccelDefinitons(*cit);
		
		// loop over all definitions
		auto dit = dm.begin();
		while( dit != dm.end() ) {
			const AccelManager::Definition& d = *dit;

			Gtk::TreeModel::Row row;
			auto children = crow.children();
			// create all children
			for( guint i = 0; i < d.location.size(); i++ ) {
				// find location
				auto it = children.begin();
				for(; it != children.end(); it++)
					if( Glib::ustring((*it)[m_Cols.m_ColName]) == d.location[i] )
						break; // found existing item

				// create new row if nothing
				if( it == children.end() ) {
					row = *(m_refTreeModel->append(children));
					row[m_Cols.m_ColName] = d.location[i];
				} else
					row = *it;
					
				children = row.children();
			}

			// fill last row
			row[m_Cols.m_Context] = *cit;
			row[m_Cols.m_Id] = d.id;

			++dit;
		}
		++cit;
	}

}

void AcceleratorPrefs::reset()
{
	m_NameLabel.set_text("");
	m_DescText.get_buffer()->set_text("");
	m_RadioDisable.set_sensitive(false);
	m_RadioLink.set_sensitive(false);
	m_ComboLinks.remove_all();
	m_ComboLinks.append(""); // append an empty line to prevent drawing small size combobox
	m_ComboLinks.set_active(0);
	m_ComboLinks.set_button_sensitivity(Gtk::SENSITIVITY_OFF);
	m_RadioSet.set_sensitive(false);
	m_GrabFrame.set_sensitive(false);
	m_AccelLabel.set_text("");
}

Glib::ustring AcceleratorPrefs::keyDisplayName( guint key, Gdk::ModifierType mods )
{
	Glib::ustring s;
	// add own modifier strings
	if( mods & MOD_CTRL ) s += "<CTRL>";
	if( mods & MOD_SHIFT ) s += "<SHIFT>";
	if( mods & MOD_ALT ) s += "<ALT>";
	if( mods & MOD_LWIN ) s += "<Left WIN>";
	if( mods & MOD_RWIN ) s += "<Right WIN>";
	// add gtk key name
	s += gtk_accelerator_name(key, GdkModifierType(0) );
	return s;
}

void AcceleratorPrefs::onAccSelected()
{
	auto it = m_AccTree.get_selection()->get_selected();
	if( it ) {
		Gtk::TreeModel::Row row = *it;
		std::string context = row[m_Cols.m_Context], id = row[m_Cols.m_Id];
		if( !context.empty() && !id.empty() ) {
		
			// get definition
			AccelManager& am = AccelManager::get();
			const AccelManager::Definition& d = am.getAccelDefiniton( context, id );
			const AccelManager::Assignment& a = am.getAccelAssignment( context, id );

			m_Updating = true;

			// fill record
			m_NameLabel.set_text( am.linkDisplayText( context, id ) );
			m_DescText.get_buffer()->set_text( d.description );
			m_GrabFrame.set_sensitive(false);
			
			// start disabled
			if( d.type == DEF_ACTION ) {
				if( a.button ) {
					// modifiers + button
					m_AccelLabel.set_text( Glib::ustring::compose(_("%1Button %2%3"), keyDisplayName(0, a.modifiers), a.button & (DBL_CLICK-1), a.button & DBL_CLICK ? " (2x)":"" ) );
				} else {
					// modifiers + key
					m_AccelLabel.set_text( keyDisplayName(a.key, a.modifiers) );
				}
				m_TypeLabel.set_text(_("Action definition:"));
			} else {
				// only modifiers
				m_AccelLabel.set_text( keyDisplayName(0, a.modifiers) );
				m_TypeLabel.set_text(_("Modifier definition:"));
			}
					
			if( a.modifiers & MOD_DISABLED )
				m_RadioDisable.set_active();
			else
				m_RadioSet.set_active();
	
			// set links
			m_ComboLinks.remove_all();
			m_ComboLinks.set_button_sensitivity(Gtk::SENSITIVITY_OFF);
			if( d.allowed_links.size() ) {
				for( unsigned int i = 0; i < d.allowed_links.size(); i++ ) {
					m_ComboLinks.append( d.allowed_links[i], am.linkDisplayText( context, d.allowed_links[i] ) );
					if( a.link == d.allowed_links[i] ) {
						m_ComboLinks.set_active( m_ComboLinks.get_model()->children().size()-1 );
					}
				}
				m_RadioLink.set_sensitive();
				if( !a.link.empty() ) {
					m_ComboLinks.set_button_sensitivity(Gtk::SENSITIVITY_ON);
					m_RadioLink.set_active();
				}
			} else {
				m_RadioLink.set_sensitive(false);
				m_ComboLinks.append("");
			}
			
			// enable
			m_RadioDisable.set_sensitive();
			m_RadioSet.set_sensitive();
			if( m_RadioSet.get_active() )
				m_GrabFrame.set_sensitive();

			m_Updating = false;
			return;
		}
	}
	reset();
}

void AcceleratorPrefs::onRadioChange()
{
	if( m_Updating ) return;
	auto it = m_AccTree.get_selection()->get_selected();
	if( it ) {
		Gtk::TreeModel::Row row = *it;
		std::string context = row[m_Cols.m_Context], id = row[m_Cols.m_Id];
		if( !context.empty() && !id.empty() ) {
		
			// get definition
			AccelManager& am = AccelManager::get();
			const AccelManager::Definition& d = am.getAccelDefiniton( context, id );
			AccelManager::Assignment& a = am.getAccelAssignment( context, id );
			
			if( m_RadioDisable.get_active() ) {
				a.modifiers |= MOD_DISABLED;
			} else if( m_RadioLink.get_active() ) {
				a.modifiers &= MOD_ALL;
				if( a.link.empty() )
					a.link = d.allowed_links[0];
			} else {
				a.modifiers &= MOD_ALL;
				a.link.clear();
			}
			AccelManager::get().changed();
			onAccSelected();
			
			if( m_RadioSet.get_active() ) m_GrabFrame.grab_focus();
		}
	}
}

void AcceleratorPrefs::onLinkChange()
{
	if( m_Updating ) return;
	auto it = m_AccTree.get_selection()->get_selected();
	if( it ) {
		Gtk::TreeModel::Row row = *it;
		std::string context = row[m_Cols.m_Context], id = row[m_Cols.m_Id];
		if( !context.empty() && !id.empty() ) {
		
			// get definition
			AccelManager& am = AccelManager::get();
			AccelManager::Assignment& a = am.getAccelAssignment( context, id );
			
			a.link = m_ComboLinks.get_active_id();

			AccelManager::get().changed();
			onAccSelected();
			
			if( m_RadioSet.get_active() ) m_GrabFrame.grab_focus();
		}
	}
}

void AcceleratorPrefs::setButton( int button, int modifiers )
{
	if( m_RadioSet.get_active() ) {
		auto it = m_AccTree.get_selection()->get_selected();
		if( it ) {
			Gtk::TreeModel::Row row = *it;
			std::string context = row[m_Cols.m_Context], id = row[m_Cols.m_Id];
			if( !context.empty() && !id.empty() ) {
			
				// get definition
				AccelManager& am = AccelManager::get();
				const AccelManager::Definition& d = am.getAccelDefiniton( context, id );
				AccelManager::Assignment& a = am.getAccelAssignment( context, id );
				
				if( d.type == DEF_ACTION || d.type == DEF_BUTTON_ACTION ) {
					a.key = 0;
					a.button = button;
					a.modifiers = Gdk::ModifierType(modifiers);
				} else if( d.type == DEF_MODIFIER && modifiers ) {
					a.key = 0;
					a.button = 0;
					a.modifiers = Gdk::ModifierType(modifiers);
				}
				
				AccelManager::get().changed();
				onAccSelected();
			}
		}
	}
}

void AcceleratorPrefs::setKey( int key, int modifiers )
{
	if( m_RadioSet.get_active() ) {
		auto it = m_AccTree.get_selection()->get_selected();
		if( it ) {
			Gtk::TreeModel::Row row = *it;
			std::string context = row[m_Cols.m_Context], id = row[m_Cols.m_Id];
			if( !context.empty() && !id.empty() ) {
			
				// get definition
				AccelManager& am = AccelManager::get();
				const AccelManager::Definition& d = am.getAccelDefiniton( context, id );
				AccelManager::Assignment& a = am.getAccelAssignment( context, id );
				
				if( d.type == DEF_ACTION || d.type == DEF_BUTTON_ACTION ) {
					a.key = key;
					a.button = 0;
					a.modifiers = Gdk::ModifierType(modifiers);
				} else if( d.type == DEF_MODIFIER && modifiers ) {
					a.key = 0;
					a.button = 0;
					a.modifiers = Gdk::ModifierType(modifiers);
				}
				
				AccelManager::get().changed();
				onAccSelected();
			}
		}
	}
}

} // namespace ...

