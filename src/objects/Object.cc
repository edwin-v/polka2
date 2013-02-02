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

#include "Object.h"
#include "Editor.h"
#include "Project.h"
#include "Functions.h"
#include "ResourceManager.h"
#include <glibmm/i18n.h>
#include <assert.h>
#include <algorithm>
#include <iostream>


namespace Polka {


const char* DEP_ID = "DEPENDENCY";
const char* DEP_CREATE_ITEM = "CREATE_DEPENDENCY";
const char* DEP_CHANGE_ITEM = "CHANGE_DEPENDENCY";
const char* DEP_REMOVE_ITEM = "REMOVE_DEPENDENCY";
const char* DEP_ITEM_TYPE = "IS";
const char* DEP_ITEMID_TYPE = "I";


Object::Object( Project& _prj, const std::string& _id, bool delayed_update )
	: m_Id( _id ), m_Project( _prj ), m_InitMode(true), m_Dirty(true),
	  m_AllowUpdateDelay(delayed_update), m_pEditor(0)
{
	m_FUNID = _prj.getNewFunid();
}

Object::~Object()
{
	// object should only be deleted if nothing depends on it
	assert(m_UsedBy.size() == 0 );
	// if editted, remove self from editor
	if( m_pEditor ) m_pEditor->setObject(0);
	// unregister all dependencies
	auto it = m_Dependencies.begin();
	while( it != m_Dependencies.end() ) {
		Object *obj = const_cast<Object*>(it->second.object);
		obj->unsetDependencyOf(this);
		it++;
	}
}

/**
 * 
 * Returns the object type ID.
 * 
 * @return the object type ID string
 */
const std::string& Object::id() const
{
	return m_Id;
}

/**
 * 
 * Returns the object fixed and unique numerical identifier.
 * 
 * @return the object type ID string
 */
guint32 Object::funid() const
{
	return m_FUNID;
}

/**
 * 
 * Returns the user assigned name of the object.
 * 
 * @return the name string
 */
const Glib::ustring& Object::name() const
{
	return m_Name;
}

/**
 * 
 * Assigns a new name.
 * 
 * @param name a string containing the new user assigned name
 */
void Object::setName( const Glib::ustring& name )
{
	m_Name = name;
}

/**
 * 
 * Returns the comment string.
 * 
 * @return the name string
 */
const Glib::ustring& Object::comments() const
{
	return m_Comments;
}

/**
 * 
 * Assigns a new comment string.
 * 
 * @param text a string containing the new comment text
 */
void Object::setComments( const Glib::ustring& text )
{
	m_Comments = text;
}

/**
 * 
 * Returns the project that contains this object.
 * 
 * @return the project class
 */
Project& Object::project()
{
	return m_Project;
}

Editor *Object::editor()
{
	return m_pEditor;
}

/**
 * 
 * Check whether an object can be removed at this time.
 * 
 * @return #TRUE if removal is possible
 */
bool Object::canRemove() const
{
	return m_UsedBy.size() == 0;
}

/**
 * Return the type ID string of a dependency.
 * 
 * @param id a unique identifier for this dependecy
 * @return the type ID string or empty if id was invalid
 */
const std::string& Object::dependencyType( int id ) const
{
	static const std::string NONE;
	
	// check if dependency id exists
	auto it = m_Dependencies.find(id);
	if( it == m_Dependencies.end() )
		return NONE;
		
	return it->second.type;
}

/**
 * Registers an object dependency. This function will immediately satisfy
 * the dependency by requesting an object from with the given type
 * specification from the project. The dependency can be changed with
 * setDependency(id, object).
 * 
 * Returns #FALSE if the registration fails. This can be either due to
 * id already existing as dependency or if the typespec is not satisfied
 * by the project.
 * 
 * @param id a unique identifier for this dependecy
 * @param a type specification string containing one or more comma separated
 *        object types that can satisfy this dependecy
 * @return #FALSE if id cannot be registered as dependecy
 */
bool Object::registerDependency( int id, const std::string& typespec, const Object *object )
{
	// check if dependency id exists
	auto it = m_Dependencies.find(id);
	if( it != m_Dependencies.end() )
		return false;
		
	// verify dependency
	if( object )
		if( !checkObjectType( object->id(), typespec ) )
			object = 0;

	// get dependency
	if( !object ) {
		object = m_Project.findObjectOfTypes(typespec);
		if( !object ) return false;
	}

	if( !m_InitMode ) {
		// create undo
		UndoAction& action = project().undoHistory().createAction( *this );
		action.setName( m_Name + _(" dependency created") );
		Storage& su = action.setUndoData( DEP_ID );
		su.createItem( DEP_REMOVE_ITEM, DEP_ITEMID_TYPE );
		su.setField( 0, id );
		// add redo data
		Storage &sr = action.setRedoData( DEP_ID );
		sr.createItem( DEP_CREATE_ITEM, DEP_ITEM_TYPE );
		sr.setField( 0, id );
		sr.setField( 1, typespec );
		sr.createItem( DEP_CHANGE_ITEM, DEP_ITEM_TYPE );
		sr.setField( 0, id );
		sr.setField( 1, object->name() );
	}
	
	m_Dependencies[id].type = typespec;
	m_Dependencies[id].object = object;

	const_cast<Object*>(object)->setDependencyOf(this);
	
	// notify editor
	if( m_pEditor ) m_pEditor->updateDependencies();

	return true;
}

/**
 * Unregister a dependency from the object. The current assigned dependency
 * will be released automatically.
 * 
 * Returns #FALSE if the dependency id is not registered.
 * 
 * @param id the identifier of the dependency to be removed
 * @return #FALSE if the dependency does not exist.
 */
bool Object::unregisterDependency( int id )
{
	// check if dependency id exists
	auto it = m_Dependencies.find(id);
	if( it == m_Dependencies.end() )
		return false;

	// unregister from this dependency
	Object& obj = *const_cast<Object*>(it->second.object);
	obj.unsetDependencyOf(this);

	if( !m_InitMode ) {
		// create undo
		UndoAction& action = project().undoHistory().createAction( *this );
		action.setName( m_Name + _(" dependency removed") );
		Storage& su = action.setUndoData( DEP_ID );
		su.createItem( DEP_CREATE_ITEM, DEP_ITEM_TYPE );
		su.setField( 0, id );
		su.setField( 1, it->second.type );
		su.createItem( DEP_CHANGE_ITEM, DEP_ITEM_TYPE );
		su.setField( 0, id );
		su.setField( 1, it->second.object->name() );
		// add redo data
		Storage &sr = action.setRedoData( DEP_ID );
		sr.createItem( DEP_REMOVE_ITEM, DEP_ITEMID_TYPE );
		sr.setField( 0, id );
	}
	
	// remove the dependecy
	m_Dependencies.erase(it);

	// notify editor
	if( m_pEditor ) m_pEditor->updateDependencies();

	return true;
}

/**
 * Change the object of a dependency. If the dependency id is
 * not registered or the newly supplied object does not satisfy the
 * dependency, #FALSE is returned.
 * 
 * @param id dependecy to be changed
 * @param new object
 * @return #FALSE if the change failed
 */
bool Object::setDependency( int id, const Object *object )
{ std::cout << "setdep init: " << m_InitMode << std::endl;
	// check if dependency id exists
	auto it = m_Dependencies.find(id);
	if( it == m_Dependencies.end() )
		assert(false); // EXCEPTION!

	// check object type validity
	if( !object )
		assert(false); //EXCEPTION
	if( !checkObjectType( object->id(), it->second.type ) )
		assert(false); //EXCEPTION

	// unregister from previous dependency
	Object& uobj = *const_cast<Object*>(it->second.object);
	uobj.unsetDependencyOf(this);
	
	// register new dependency
	Object& robj = *const_cast<Object*>(object);
	robj.setDependencyOf(this);
	it->second.object = object;

	if( !m_InitMode ) {
		// create undo
		UndoAction& action = project().undoHistory().createAction( *this );
		action.setName( m_Name + _(" dependency changed") );
		Storage& su = action.setUndoData( DEP_ID );
		su.createItem( DEP_CHANGE_ITEM, DEP_ITEM_TYPE );
		su.setField( 0, id );
		su.setField( 1, uobj.name() );
		// add redo data
		Storage &sr = action.setRedoData( DEP_ID );
		sr.createItem( DEP_CHANGE_ITEM, DEP_ITEM_TYPE );
		sr.setField( 0, id );
		sr.setField( 1, robj.name() );
	}
	
	// notify editor
	if( m_pEditor ) m_pEditor->updateDependencies();

	return true;
}

/**
 * Returns the object for a dependecy.
 * 
 * This function should be considered the primary access point for 
 * dependency objects. It is *not* recommended that derived classes
 * keep their own pointers to object. Instead access should be implemented
 * through functions such as:
 * 
 * @code
 * DepType& DerivedObj::getDep()
 * {
 *    return *const_cast<DepType*>(dependency(DEP_ID));
 * }
 * @endcode
 * 
 * @param id dependency to return the object for
 * @return the dependency object or 0 if the dependency does not exist
 */
const Object *Object::dependency( int id ) const
{
	// check if dependency id exists
	auto it = m_Dependencies.find(id);
	if( it == m_Dependencies.end() )
		return 0;
		
	return it->second.object;
}

/**
 * Returns the number of dependecies for this object.
 * 
 * @return the number of dependencies
 */
int Object::dependencyCount() const
{
	return m_Dependencies.size();
}

/**
 * Returns the dependency ID for the dependency that is the 'nr'th in
 * the sorted list.
 * 
 * This function can be used to loop through all dependencies.
 * 
 * @param nr the number of the dependency (asserts if out of range!)
 * @return the dependency ID
 */
int Object::dependencyId( int nr ) const
{
	assert( guint(nr) < m_Dependencies.size() );
	auto it = m_Dependencies.begin();
	while(nr) {
		++it;
		nr--;
	}
	return it->first;
}

/**
 * Signals that this object is being used as a dependency by the
 * supplied object.
 * 
 * @param object the object that depends on this one
 */
void Object::setDependencyOf( const Object *object )
{
	// don't check existence. double dependency would be perfectly ok
	m_UsedBy.push_back( object );
}

/**
 * Signals that this object no longer being used as a dependency
 * by the supplied object.
 * 
 * @param object the object that depends on this one
 * @return #TRUE is the object was found and removed
 */
bool Object::unsetDependencyOf( const Object *object )
{
	auto it = find( m_UsedBy.begin(), m_UsedBy.end(), object );
	if( it == m_UsedBy.end() )
		return false;
	
	m_UsedBy.erase(it);
	return true;
}

/**
 * Checks whether an update can be delayed.
 * This is allowed if the following three conditions are true:
 * 1. The object type allows it
 * 2. The object is not currently in an editor
 * 3. All objects depending on this one allow it * 
 * 
 * @return #TRUE if an update can be delayed
 */
bool Object::canDelayUpdate() const
{
	if( !m_AllowUpdateDelay ) return false;
	if( m_pEditor ) return false;
	// check with dependents
	auto it = m_UsedBy.begin();
	while( it != m_UsedBy.end() ) {
		if( !(*it)->canDelayUpdate() ) return false;
		it++;
	}
	
	return true;
}

/**
 * Signals that the object has changed.
 * 
 * This causes update calls to traverse through the tree
 * of dependent objects. However, in order to prevent 
 * unnecessary updating of invisible objects, updates are
 * delayed when conditions allow this.
 * 
 * @param full determines whether the object should be fully or 
 * partially updated. This can be used by the object and editor to
 * minimize the effort during interactive use.
 */
void Object::update( bool full )
{
	if( canDelayUpdate() ) {
		// set all dependents dirty
		setDirty();
	} else {
		// update self
		onUpdate(full);
		m_Dirty = false;
		// send changed signal to editor
		if( m_pEditor )
			m_pEditor->objectUpdated(full);

		// update dependents
		auto it = m_UsedBy.begin();
		while( it != m_UsedBy.end() ) {
			Object *obj = const_cast<Object*>(*it);
			obj->update();
			it++;
		}

	}
}

/**
 * Force an update of dirty objects. When an update is forced,
 * a forced update must also be performed on all dependencies
 * in order to guarantee that an object is indeed fully up
 * to date.
 */
void Object::forceUpdate()
{
	if( m_Dirty ) {
		// first update depdencies
		auto it = m_Dependencies.begin();
		while( it != m_Dependencies.end() ) {
			Object *obj = const_cast<Object*>(it->second.object);
			obj->forceUpdate();
			it++;
		}
		// update self fully
		onUpdate(true);
		m_Dirty = false;
	}
}

/**
 * Turns init mode on or off. When init mode is turned on, certain
 * automated responsed won't be executed. These include automated
 * generation of undo events.
 * 
 * @param val set true to turn init mode on and false to turn it off.
 */
void Object::setInitMode( bool val )
{
	m_InitMode = val;
}

void Object::setDirty()
{
	// set self dirty
	m_Dirty = true;
	// set dependents dirty
	auto it = m_UsedBy.begin();
	while( it != m_UsedBy.end() ) {
		Object *obj = const_cast<Object*>(*it);
		obj->setDirty();
		it++;
	}
}

void Object::onUpdate( bool /*full*/ )
{
	// implement in derived class if relevant
}


int Object::save( Storage& s )
{
	// store generic stuff
	// create name
	s.createItem("OBJECT_NAME", "S");
	s.setField( 0, m_Name );
	// create comments
	s.createItem("COMMENTS", "S");
	s.setField( 0, m_Comments );
	// store unique id
	s.createItem("UNIQUE_ID", "I");
	s.setField( 0, gint32(m_FUNID) );
	// save object data
	return store(s);
}

int Object::load( Storage& s )
{
	// name is already loaded, continue with others
	if( s.version() >= 2 ) {
		// require unique id
		if( !s.findItem("UNIQUE_ID") ) return Storage::EINVALIDDATA;
		if( !s.checkFormat("I") ) return Storage::EINVALIDDATA;
		m_FUNID = s.integerField(0);
		std::cout << "FUNID restored: " << m_FUNID << std::endl;
	} else {
		// generate unique id
		m_FUNID = m_Project.getNewFunid();
	}
	// required id availabe
	if( s.findItem("COMMENTS") ) {
		if( s.checkFormat("S") ) {
			m_Comments = s.stringField(0);
		}
	}
	// load object data
	return restore(s);
}

int Object::store( Storage& s )
{
	// implement in derived class if relevant
	return 0;
}

int Object::restore( Storage& s )
{
	// implement in derived class if relevant
	return 0;
}

void Object::attachEditor( Editor* editor )
{
	assert( m_pEditor == 0 );
	// attach editor
	m_pEditor = editor;
	// force an update for possible dirty objects
	forceUpdate();
}

void Object::detachEditor( Editor* editor )
{
	assert( m_pEditor == editor );
	m_pEditor = 0;
}

void Object::undo( const std::string& /*id*/, Storage& /*s*/ )
{
	// implement in derived class if relevant
}

void Object::redo( const std::string& /*id*/, Storage& /*s*/ )
{
	// implement in derived class if relevant
}

void Object::objectUndo( const std::string& id, Storage& s )
{
	if( id == DEP_ID ) 
		objectStorageAction( s );
	else
		undo( id, s );
}

void Object::objectRedo( const std::string& id, Storage& s )
{
	if( id == DEP_ID ) 
		objectStorageAction( s );
	else
		redo( id, s );
}

void Object::objectStorageAction( Storage& s )
{
	bool upd = false, init = m_InitMode;
	// no undo generation
	m_InitMode = true;

	// first try create item
	if( s.findItem( DEP_CREATE_ITEM ) ) {
		registerDependency( s.integerField(0), s.stringField(1) );
		upd = true;
	}
	// then try change item
	if( s.findItem( DEP_CHANGE_ITEM ) ) {
		Object *obj = m_Project.findObject( s.stringField(1) );
		if( obj ) {
			// change dependency
			setDependency( s.integerField(0), obj );
			upd = true;
		}
	}
	// and lastly delete
	if( s.findItem( DEP_REMOVE_ITEM ) ) {
		unregisterDependency( s.integerField(0) );
		upd = true;
	}
	if( upd ) update();
	
	m_InitMode = init;
}


} // namespace Polka
