#include "ObjectPropertiesDialog.h"
#include "ObjectPropertySheet.h"
#include "Object.h"
#include "Project.h"
#include "ObjectManager.h"
#include <glibmm/i18n.h>
#include <gtkmm/stock.h>
#include <gtkmm/scrolledwindow.h>


namespace Polka {


ObjectPropertiesDialog::ObjectPropertiesDialog( Polka::Object& obj_ )
	: Dialog( _("Properties for ") + obj_.name(), true ), m_Object( obj_ ),
	  m_NameLabel( _("Name:"), Gtk::ALIGN_START ),
	  m_CommentsLabel( _("Comments:"), Gtk::ALIGN_START ),
	  m_UsesLabel( _("This object uses:"), Gtk::ALIGN_START ), 
	  m_UsedByLabel( _("This is used by:"), Gtk::ALIGN_START )
{
	set_border_width(12);
	set_default_size( 300, 400 );

	// create main tab
	m_MainBox.set_spacing( 2 );
	m_MainBox.set_border_width(12);
	m_MainBox.pack_start( m_NameLabel, Gtk::PACK_SHRINK );
	m_NameEntry.set_text( m_Object.name() );
	m_NameEntry.signal_changed().connect( sigc::mem_fun(*this, &ObjectPropertiesDialog::setInfoChanged ) );
	m_MainBox.pack_start( m_NameEntry, Gtk::PACK_SHRINK, 4 );
	m_MainBox.pack_start( m_CommentsLabel, Gtk::PACK_SHRINK, 4 );
	Gtk::ScrolledWindow *s = manage( new Gtk::ScrolledWindow );
	s->add( m_CommentsText );
	s->set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
	s->set_shadow_type( Gtk::SHADOW_IN );
	m_MainBox.pack_start( *s );
	m_CommentsText.get_buffer()->set_text( m_Object.comments() );
	m_CommentsText.set_wrap_mode( Gtk::WRAP_WORD_CHAR );
	m_CommentsText.get_buffer()->signal_changed().connect( sigc::mem_fun(*this, &ObjectPropertiesDialog::setInfoChanged ) );
	
	// create object property tabs
	ObjectPropertySheet *props;
	while( (props = ObjectManager::get().createObjectPropertySheet( obj_, m_ObjectPropertyTabs.size() )) ) {
		manage(props);
		props->signalSetModified().connect(
			sigc::mem_fun(*this, &ObjectPropertiesDialog::setPropertiesChanged ) );
		m_ObjectPropertyTabs.push_back(props);
	}

	// links
	m_LinksBox.set_spacing( 3 );
	m_LinksBox.set_border_width( 10 );
	m_LinksBox.pack_start( m_UsesLabel, Gtk::PACK_SHRINK );

	ObjectManager& om = ObjectManager::get();
	// generate dependency text
	Glib::ustring depStr;
	DependencyMap::iterator it = m_Object.m_Dependencies.begin();
	while( it != m_Object.m_Dependencies.end() ) {
		// add newline
		if( !depStr.empty() ) depStr += '\n';
		// add name (type)
		depStr += it->second.object->name();
		depStr += " (";
		depStr += om.nameFromId( it->second.object->id() );
		depStr += ')';
		// next
		++it;
	}
	m_UsesText.get_buffer()->set_text( depStr);
	
	m_UsesText.set_editable( false );
	s = manage( new Gtk::ScrolledWindow );
	s->add( m_UsesText );
	s->set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
	s->set_shadow_type( Gtk::SHADOW_IN );
	m_LinksBox.pack_start( *s );
	m_LinksBox.pack_start( m_UsedByLabel, Gtk::PACK_SHRINK);
	// generate dependency_of text
	depStr.clear();
	ConstObjectList::iterator it2 = m_Object.m_UsedBy.begin();
	while( it2 != m_Object.m_UsedBy.end() ) {
		// add newline
		if( !depStr.empty() ) depStr += '\n';
		// add name (type)
		depStr += (*it2)->name();
		depStr += " (";
		depStr += om.nameFromId( (*it2)->id() );
		depStr += ')';
		// next
		++it2;
	}
	m_UsedByText.get_buffer()->set_text( depStr);
	
	m_UsedByText.set_editable( false );
	s = manage( new Gtk::ScrolledWindow );
	s->add( m_UsedByText );
	s->set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
	s->set_shadow_type( Gtk::SHADOW_IN );
	m_LinksBox.pack_start( *s );
	
	// fill notebook
	m_Tabs.append_page( m_MainBox, _("Info") );
	for( unsigned int i = 0; i < m_ObjectPropertyTabs.size(); i++ ) {
		ObjectPropertySheet *ptab = m_ObjectPropertyTabs[i];
		m_Tabs.append_page( *ptab, ptab->pageName() );
	}
	m_Tabs.append_page( m_LinksBox, _("Links") );
	
	// add buttons
	//add_button( Gtk::Stock::APPLY, Gtk::RESPONSE_NONE );
	add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	add_button( Gtk::Stock::OK, Gtk::RESPONSE_OK );
	set_response_sensitive( Gtk::RESPONSE_CANCEL, false );
	set_default_response( Gtk::RESPONSE_OK );
	
	m_InfoChanged = m_PropertiesChanged = false;

	// add notebook
	get_vbox()->pack_start( m_Tabs );
		
	show_all_children();
	
}

ObjectPropertiesDialog::~ObjectPropertiesDialog()
{
}

void ObjectPropertiesDialog::on_response( int id )
{
	if( !m_InfoChanged && !m_PropertiesChanged ) return;
	if( id == Gtk::RESPONSE_OK ) {
		// init undo group
		m_Object.project().undoHistory().openActionGroup( _("Change properties"), ObjectManager::get().iconFromId( m_Object.id() ) );

		// rename object through project (for undo)
		if( m_InfoChanged ) {
			m_Object.project().setObjectName( m_Object, m_NameEntry.get_text() );
			m_Object.project().setObjectComments( m_Object, m_CommentsText.get_buffer()->get_text() );
		}
		// apply settings through custom widget
		if( m_PropertiesChanged )
			for( unsigned int i = 0; i < m_ObjectPropertyTabs.size(); i++ )
				m_ObjectPropertyTabs[i]->apply();

		// close undo group
		m_Object.project().undoHistory().closeActionGroup();

	} else
		for( unsigned int i = 0; i < m_ObjectPropertyTabs.size(); i++ )
		m_ObjectPropertyTabs[i]->reset();
}

void ObjectPropertiesDialog::setInfoChanged()
{
	m_InfoChanged = true;
	updateSensitivity();
}

void ObjectPropertiesDialog::setPropertiesChanged( bool value )
{
	m_PropertiesChanged = value;
	updateSensitivity();
}

void ObjectPropertiesDialog::updateSensitivity()
{
	set_response_sensitive( Gtk::RESPONSE_CANCEL, 
	                        m_PropertiesChanged || m_InfoChanged );
}

} // namespace Polka

