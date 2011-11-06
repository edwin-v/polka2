#include "AccelBase.h"
#include "ObjectManager.h"
#include <cassert>
#include <iostream>

namespace Polka {

const Gdk::ModifierType ACC_ALLOWED = Gdk::SHIFT_MASK | 
                                      Gdk::CONTROL_MASK | 
                                      Gdk::MOD1_MASK /*ALT*/ |
                                      Gdk::MOD4_MASK /*Left Win*/ |
                                      Gdk::MOD5_MASK /*Right Win*/ ;

AccelBase::AccelBase( const std::string& _id )
	: m_Id(_id)
{
}

AccelBase::~AccelBase()
{
}


/*
 * Accelerator functions
 */

/**
 * Add accelerator keys, mouse button actions or action modifiers to the list.
 * 
 * @param id incremental id (asserts if not in correct order starting from zero!)
 * @param path this string is added to the system accelerator path prefix for this AccelBase
 * @param button mouse button for action (0 if not used)
 * @param key key for action (defaults to GDK_KEY_VoidSymbol for none)
 * @param mods modifier keys (defaults to none)
 */
void AccelBase::accAdd( guint id, const Glib::ustring& path, guint button, guint key, Gdk::ModifierType mods )
{
	// only allow incremental IDs
	assert( id == m_Accels.size() );
	
	// create new accel
	Accel a;
	a.path = Glib::ustring::compose("<Polka2>/Editors/%1/%2", ObjectManager::instance().editorNameFromId( m_Id ), path );
	a.button = button;
	a.key = key;
	a.mods = mods & ACC_ALLOWED;
	
	m_Accels.push_back(a);
}

/**
 * Full "accel_path" string for use in gtk actions.
 * 
 * @param id accelerator id
 * @return gtk accelerator path
 */
const Glib::ustring& AccelBase::accPath( guint id )
{
	assert( id < m_Accels.size() );

	return m_Accels[id].path;
}

/**
 * Return action button number.
 * 
 * @param id accelerator id
 * @return button number
 */
guint AccelBase::accButton( guint id )
{
	assert( id < m_Accels.size() );

	return  m_Accels[id].button;
}

/**
 * Return action key number.
 * 
 * @param id accelerator id
 * @return key number
 */
guint AccelBase::accKey( guint id )
{
	assert( id < m_Accels.size() );

	return  m_Accels[id].key;
}

/**
 * Check if a button with modifier mask is valid for an action accelarator.
 * 
 * @param id  id
 * @param button number
 * @param a modifier mask
 * @param set true if other allowed modifiers should not be pressed
 * @return true if button and modifiers match action
 */
bool AccelBase::checkAccButton( guint id, guint button, guint mods, bool exact )
{
	assert( id < m_Accels.size() );

	// check mods
	mods &= ACC_ALLOWED;
	bool mods_ok = exact ?  m_Accels[id].mods == Gdk::ModifierType(mods) :
	                        (Gdk::ModifierType(mods) & m_Accels[id].mods ) == m_Accels[id].mods;
	
	return m_Accels[id].button == button && mods_ok;
}

/**
 * Check if a key with modifier mask is valid for an action accelarator.
 * 
 * @param id accelerator id
 * @param key key number
 * @param a modifier mask
 * @param set true if other allowed modifiers should not be pressed
 * @return true if key and modifiers match action
 */
bool AccelBase::checkAccKey( guint id, guint key, guint mods, bool exact )
{
	assert( id < m_Accels.size() );

	// check mods
	mods &= ACC_ALLOWED;
	bool mods_ok = exact ?  m_Accels[id].mods == Gdk::ModifierType(mods) :
	                        (Gdk::ModifierType(mods) & m_Accels[id].mods ) == m_Accels[id].mods;
	
	return m_Accels[id].key == key && mods_ok;
}

/**
 * Check if a modifier mask is valid for an action accelarator.
 * 
 * @param id accelerator id
 * @param a modifier mask
 * @param set true if other allowed modifiers should not be pressed
 * @return true if the modifiers match action
 */
bool AccelBase::checkAccMods( guint id, guint mods, bool exact )
{
	assert( id < m_Accels.size() );

	mods &= ACC_ALLOWED;
	if( exact ) {
		return  m_Accels[id].mods == Gdk::ModifierType(mods);
	} else {
		return  (Gdk::ModifierType(mods) & m_Accels[id].mods ) == m_Accels[id].mods;
	}
}

} // namespace Polka
