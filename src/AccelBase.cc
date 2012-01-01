#include "AccelBase.h"
#include "ObjectManager.h"
#include <cassert>
#include <iostream>

namespace Polka {


AccelBase::AccelBase( const std::string& _id )
	: m_Id(_id), m_Accels( AccelManager::get().getAccelContext(_id) )
{
}

AccelBase::~AccelBase()
{
}



/**
 * Check if a button or key with modifier mask is a match for the 
 * action accelarator.
 * 
 * @param id accelerator id
 * @param button number
 * @param key key number
 * @param mods a modifier mask
 * @param exact set true if other allowed modifiers should not be pressed
 * @return true if button and modifiers match action
 */
bool AccelBase::isAccel( guint id, guint button, guint key, guint mods, bool exact )
{
	assert( id < m_Accels.size() );

	// update possible links
	if( !m_Accels[id].updated ) AccelManager::get().updateLink( m_Id, id );

	// check mods
	mods &= MOD_ALL;
	bool mods_ok = exact ?  m_Accels[id].modifiers == Gdk::ModifierType(mods) :
	                        (Gdk::ModifierType(mods) & m_Accels[id].modifiers ) == m_Accels[id].modifiers;
	
	return m_Accels[id].button == button && m_Accels[id].key == key && mods_ok;
}

/**
 * Check if a button or key a match for the action accelarator. This
 * function does not check the modifiers and is generally only used
 * to check button/key releases.
 * 
 * @param id accelerator id
 * @param button number
 * @param key key number
 * @return true if button and key match action
 */
bool AccelBase::isAccel( guint id, guint button, guint key )
{
	assert( id < m_Accels.size() );

	// update possible links
	if( !m_Accels[id].updated ) AccelManager::get().updateLink( m_Id, id );

	return m_Accels[id].button == button && m_Accels[id].key == key;
}

/**
 * Check if one of the buttons or keys with modifier mask is a match for
 * the action accelarator.
 * 
 * @param ids a vector of id numbers
 * @param button number
 * @param key key number
 * @param mods a modifier mask
 * @param exact set true if other allowed modifiers should not be pressed
 * @return the matching id or -1 if nothing matches.
 */
int AccelBase::isAccel( const std::vector<guint> ids, guint button, guint key, guint mods, bool exact )
{
	int first_match = -1;
	for( unsigned int i = 0; i < ids.size(); i++ )
		if( isAccel( ids[i], button, key, mods, exact ) ) {
			if( m_Accels[ids[i]].modifiers != MOD_NONE )
				return ids[i];
			else
				first_match = ids[i];
		}
	// return either first or no match
	return first_match;
}

/**
 * Check if one of the buttons or keys is a match for the action 
 * accelarator. This function does not check the modifiers and is 
 * generally only used to check button/key releases.
 * 
 * @param ids a vector of id numbers
 * @param button number
 * @param key key number
 * @return the matching id or -1 if nothing matches.
 */
int AccelBase::isAccel( const std::vector<guint> ids, guint button, guint key )
{
	int first_match = -1;
	for( unsigned int i = 0; i < ids.size(); i++ )
		if( isAccel( ids[i], button, key ) ) {
			if( m_Accels[ids[i]].modifiers != MOD_NONE )
				return ids[i];
			else
				first_match = ids[i];
		}
	// return either first or no match
	return first_match;
}

/**
 * Check if a modifier mask is valid for an action accelarator.
 * 
 * @param id accelerator id
 * @param mods a modifier mask
 * @param exact set true if other allowed modifiers should not be pressed
 * @return true if the modifiers match action
 */
bool AccelBase::isAccelMod( guint id, guint mods, bool exact )
{
	assert( id < m_Accels.size() );

	// update possible links
	if( !m_Accels[id].updated ) AccelManager::get().updateLink( m_Id, id );

	mods &= MOD_ALL;
	if( exact ) {
		return  m_Accels[id].modifiers == Gdk::ModifierType(mods);
	} else {
		return  (Gdk::ModifierType(mods) & m_Accels[id].modifiers ) == m_Accels[id].modifiers;
	}
}

/**
 * Helper function to update between two actions and a possible switch
 * modifier.
 * 
 * @param id1 first accelerator id
 * @param id2 second accelerator id
 * @param first id1 is currently active
 * @param mods a modifier mask
 * @param idflip id of the switch modifier accelerator (optional)
 * @return true if id1 is active
 */
bool AccelBase::updateAccel( guint id1, guint id2, bool first, guint mods, guint idflip )
{
	if( m_Accels[id1].button == m_Accels[id2].button && 
	    m_Accels[id1].key    == m_Accels[id2].key )
	{
		// with identical activators, mods determine function
		bool m1 = isAccelMod(id1, mods);
		bool m2 = isAccelMod(id2, mods);
		if( m1 != m2 )
			// choose the matching id
			first = m1 ? id1:id2;
		else if( m1 )
			// both match, choose the one with the most matching accels
			first = modCount(m_Accels[id1].modifiers) > modCount(m_Accels[id2].modifiers);
	} else if( idflip != guint(-1) ) {
		// different activators, use flip
		if( isAccelMod(idflip, mods) )
			first = !first;
	}
	return first;
}

/**
 * Helper function to guess the best match for two accelerators by
 * checking modifiers alone.
 * 
 * @param id1 first accelerator id
 * @param id2 second accelerator id
 * @param mods a modifier mask
 * @param idflip id of the switch modifier accelerator (optional)
 * @return true if id1 is active
 */
bool AccelBase::guessAccel( guint id1, guint id2, guint mods, guint idflip )
{
	if( m_Accels[id1].button == m_Accels[id2].button && 
	    m_Accels[id1].key    == m_Accels[id2].key )
	{
		// with identical activators, mods determine function
		bool m1 = isAccelMod(id1, mods);
		bool m2 = isAccelMod(id2, mods);
		if( m1 != m2 )
			return m1;
		else
			return modCount(m_Accels[id1].modifiers) >= modCount(m_Accels[id2].modifiers);
	} else if( idflip != guint(-1) ) {
		// different activators, use flip
		return !isAccelMod(idflip, mods);
	}
	return true;
}

/**
 * Helper to calculate the button from the event structure.
 * 
 * @param event the GdkEventButton structure
 * @return the button id
 */
guint AccelBase::accelEventButton( const GdkEventButton *event ) const
{
	return event->button + (event->type == GDK_2BUTTON_PRESS ? DBL_CLICK:0);
}

int AccelBase::modCount( guint mods )
{
	int c = 0;
	if( mods & MOD_SHIFT ) c++;
	if( mods & MOD_CTRL  ) c++;
	if( mods & MOD_ALT   ) c++;
	if( mods & MOD_LWIN  ) c++;
	if( mods & MOD_RWIN  ) c++;
	return c;
}

} // namespace Polka
