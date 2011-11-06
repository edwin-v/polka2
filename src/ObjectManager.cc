#include "ObjectManager.h"
#include "ResourceManager.h"
#include "Register.h"
#include "Functions.h"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>

const char *FOLDER_MAIN = "system_mainmap";
const char *FOLDER_SUB = "system_submap";

namespace Polka { 

/* Object Factory */

ObjectManager::ObjectFactory::ObjectFactory( const Glib::ustring& _name,
	const Glib::ustring& _location, const std::string& _locId, const Glib::ustring& _description,
	const std::string& _classId, const std::string& _editorId,
	const std::string& _iconObj, const std::string& _iconLoc, const std::string& _iconSubLoc )
	: m_Name( _name ), m_Location( _location ), m_Description( _description ),
	  m_LocationId( _locId ), m_ClassId( _classId),
	  m_EditorId( _editorId ),
	  m_LocationIcon( _iconLoc ), m_SubLocationIcon( _iconSubLoc ), m_ObjectIcon( _iconObj )
{
	if( m_LocationIcon.empty() ) m_LocationIcon = FOLDER_MAIN;
	if( m_SubLocationIcon.empty() ) m_SubLocationIcon = FOLDER_SUB;
}

ObjectManager::ObjectFactory::~ObjectFactory()
{
}

const Glib::ustring& ObjectManager::ObjectFactory::name() const
{
	return m_Name; 
}

const Glib::ustring& ObjectManager::ObjectFactory::location() const 
{
	return m_Location;
}

const std::string& ObjectManager::ObjectFactory::locationId() const 
{ 
	return m_LocationId; 
}

const std::string& ObjectManager::ObjectFactory::classId() const 
{ 
	return m_ClassId; 
}

const std::string& ObjectManager::ObjectFactory::editorId() const 
{ 
	return m_EditorId; 
}

const Glib::RefPtr<Gdk::Pixbuf> ObjectManager::ObjectFactory::locationIcon() const 
{ 
	return ResourceManager::get().getIcon( m_LocationIcon ); 
}

const Glib::RefPtr<Gdk::Pixbuf> ObjectManager::ObjectFactory::subLocationIcon() const 
{ 
	return ResourceManager::get().getIcon( m_SubLocationIcon ); 
}

const Glib::RefPtr<Gdk::Pixbuf> ObjectManager::ObjectFactory::objectIcon() const 
{ 
	return ResourceManager::get().getIcon( m_ObjectIcon ); 
}

bool ObjectManager::ObjectFactory::canCreate( const Project& /*project*/ ) const
{
	return true;
}

ObjectPropertySheet *ObjectManager::ObjectFactory::createPropertySheet( Polka::Object& /*_obj*/ ) const
{
	// default is no property sheet
	return 0;
}


/* Editor Factory */

ObjectManager::EditorFactory::EditorFactory( const std::string& _classId, const Glib::ustring& _name )
	: m_ClassId( _classId ), m_Name( _name )
{
}

ObjectManager::EditorFactory::~EditorFactory()
{
}

const std::string& ObjectManager::EditorFactory::classId() const
{
	return m_ClassId;
}

const Glib::ustring& ObjectManager::EditorFactory::name() const
{
	return m_Name;
}

/* Object Manager */

ObjectManager::ObjectManager()
{
	registerObjects( *this );
	initPlugIns();
}

ObjectManager::~ObjectManager()
{
}

ObjectManager& ObjectManager::instance()
{
	static ObjectManager om;
	
	return om;
}

void ObjectManager::initPlugIns()
{
}

void ObjectManager::registerObject( ObjectFactory *obj_factory )
{
	// add factory
	m_ObjectFactories.push_back(obj_factory);
	// find container location
	std::map<std::string,Glib::ustring>::iterator nit = m_ContainerNames.find( obj_factory->locationId() );
	if( nit == m_ContainerNames.end() ) {
		// new container, store
		m_ContainerNames[ obj_factory->locationId() ] = obj_factory->location();
	}
}

void ObjectManager::registerEditor( EditorFactory *edt_factory )
{
	m_EditorFactories[ edt_factory->classId() ] = edt_factory;
}

const std::map<std::string, Glib::ustring>& ObjectManager::containers() const
{
	return m_ContainerNames;
}

bool ObjectManager::checkContainerId( const std::string& locId ) const
{
	std::map<std::string,Glib::ustring>::const_iterator nit = m_ContainerNames.find( locId );
	return nit != m_ContainerNames.end();
}

unsigned int ObjectManager::numObjectTypes() const
{
	return m_ObjectFactories.size();
}

const std::string& ObjectManager::objectId( unsigned int nr ) const
{
	// assume correct nr
	assert( nr < m_ObjectFactories.size() );
	// return text id
	return m_ObjectFactories[nr]->classId();
}

const Glib::ustring& ObjectManager::objectName( unsigned int nr ) const
{
	// assume correct nr
	assert( nr < m_ObjectFactories.size() );
	// return text id
	return m_ObjectFactories[nr]->name();
}

const std::string& ObjectManager::objectLocationId( unsigned int nr ) const
{
	// assume correct nr
	assert( nr < m_ObjectFactories.size() );
	// return text id
	return m_ObjectFactories[nr]->locationId();
}

const std::string& ObjectManager::objectLocationId( const std::string& id ) const
{
	static const std::string EMPTY;
	const ObjectManager::ObjectFactory *of = objectFactoryFromId(id);
	if( of ) return of->locationId();
	return EMPTY;
}

const Glib::ustring& ObjectManager::objectLocation( const std::string& id ) const
{
	static const Glib::ustring EMPTY;
	const ObjectManager::ObjectFactory *of = objectFactoryFromId(id);
	if( of ) return of->location();
	return EMPTY;
}

const ObjectManager::ObjectFactory *ObjectManager::objectFactoryFromId( const std::string& id ) const
{
	std::vector<ObjectFactory*>::const_iterator it = m_ObjectFactories.begin();
	while( it != m_ObjectFactories.end() ) {
		if( (*it)->classId() == id ) return *it;
		it++;
	}
	return 0;
}

const Glib::ustring& ObjectManager::nameFromId( const std::string& id ) const
{
	const ObjectFactory *of = objectFactoryFromId(id);
	assert(of);
	return of->name();
}

const Glib::ustring& ObjectManager::editorNameFromId( const std::string& id ) const
{
	// find factory
	std::map<std::string,EditorFactory*>::const_iterator fit = m_EditorFactories.find(id);
	assert( fit != m_EditorFactories.end() );
	// create and store new editor
	return fit->second->name();
}

const Glib::RefPtr<Gdk::Pixbuf> ObjectManager::locationIcon( const std::string& locId ) const
{
	// search for location
	std::vector<ObjectFactory*>::const_iterator it = m_ObjectFactories.begin();
	while( it != m_ObjectFactories.end() ) {
		if( (*it)->locationId() == locId ) {
			if( (*it)->locationIcon() )
				return (*it)->locationIcon();
			else
				break;
		}
		it++;
	}
	return ResourceManager::get().getIcon("folder_main");
}

const Glib::RefPtr<Gdk::Pixbuf> ObjectManager::subLocationIcon( const std::string& locId ) const
{
	// search for location
	std::vector<ObjectFactory*>::const_iterator it = m_ObjectFactories.begin();
	while( it != m_ObjectFactories.end() ) {
		if( (*it)->locationId() == locId ) {
			if( (*it)->subLocationIcon() )
				return (*it)->subLocationIcon();
			else
				break;
		}
		it++;
	}
	return ResourceManager::get().getIcon("folder_sub");
}

const Glib::RefPtr<Gdk::Pixbuf> ObjectManager::iconFromId( const std::string& id ) const
{
	const ObjectFactory *of = objectFactoryFromId(id);
	assert(of);
	return of->objectIcon();
}

Object *ObjectManager::createObject( Project& _prj, const std::string& id )
{
	const ObjectFactory *of = objectFactoryFromId(id);
	assert(of);
	assert(of->canCreate(_prj));
	return of->create(_prj);
}

const std::string& ObjectManager::getObjectEditorId( const std::string& id ) const
{
	const ObjectFactory *of = objectFactoryFromId(id);
	assert(of);
	return of->editorId();
}

Editor *ObjectManager::getObjectEditor( const std::string& id )
{
	return getEditor( getObjectEditorId( id ) );
}

Editor *ObjectManager::getEditor( const std::string& id )
{
	if( id.empty() ) return 0;
	// find existing editor
	auto eit = m_Editors.find(id);
	if( eit != m_Editors.end() ) return eit->second;
	// find factory
	auto fit = m_EditorFactories.find(id);
	if( fit == m_EditorFactories.end() ) return 0;
	// create new editor
	Editor *newEditor = fit->second->create();
	// store and return
	m_Editors[id] = newEditor;
	return newEditor;
}

void ObjectManager::hideEditors()
{
	auto it = m_Editors.begin();
	while( it != m_Editors.end() ) {
		if( !it->second->isMainEditor() ) {
			it->second->hide();
			std::cout << "HIDEED\n";
		}
		++it;
	}
}

bool ObjectManager::canCreateObject( const std::string& id, const Project& prj ) const
{
	const ObjectFactory *of = objectFactoryFromId(id);
	assert(of);
	// ok if empty
	return of->canCreate(prj);
}

ObjectPropertySheet *ObjectManager::createObjectPropertySheet( Polka::Object& obj )
{
	const ObjectFactory *of = objectFactoryFromId( obj.id() );
	if( of )
		return of->createPropertySheet(obj);
	else
		return 0;
}


} // namespace Polka 
