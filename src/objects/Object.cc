#include "Object.h"
#include "Editor.h"
#include "Project.h"
#include "Functions.h"
#include <assert.h>
#include <algorithm>
#include <iostream>


namespace Polka {
	
Object::Object( Project& _prj, const std::string& _id, bool delayed_update )
	: m_Id( _id ), m_Project( _prj ), m_Dirty(true), m_AllowUpdateDelay(delayed_update), m_pEditor(0)
{
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
bool Object::registerDependency( int id, const std::string& typespec )
{
	// check if dependency id exists
	auto it = m_Dependencies.find(id);
	if( it != m_Dependencies.end() )
		return false;
		
	// get dependency
	Object *obj = m_Project.findObjectOfTypes(typespec);
	if( !obj ) return false;
	
	m_Dependencies[id].type = typespec;
	m_Dependencies[id].object = obj;

	obj->setDependencyOf(this);
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
	
	// remove the dependecy
	m_Dependencies.erase(it);
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
{
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
	// save object data
	return store(s);
}

int Object::load( Storage& s )
{
	// name is already loaded, continue with others
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


} // namespace Polka
