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

#ifndef _POLKA_ACCELMANAGER_H_
#define _POLKA_ACCELMANAGER_H_

#include "Storage.h"
#include <vector>
#include <map>
#include <string>
#include <glibmm/ustring.h>
#include <gdkmm/types.h>

namespace Polka {

// convenience definitions for modifiers
const Gdk::ModifierType MOD_NONE = Gdk::ModifierType(0);

const Gdk::ModifierType MOD_SHIFT = Gdk::SHIFT_MASK;
const Gdk::ModifierType MOD_CTRL = Gdk::CONTROL_MASK;
const Gdk::ModifierType MOD_ALT = Gdk::MOD1_MASK;
const Gdk::ModifierType MOD_LWIN = Gdk::MOD4_MASK;
const Gdk::ModifierType MOD_RWIN = Gdk::MOD5_MASK;

// note that the casts in the next constant are to prevent warnings when MOD_ALL is unused.
const Gdk::ModifierType MOD_ALL = Gdk::ModifierType( guint(MOD_SHIFT) | 
                                                     guint(MOD_CTRL)  | 
                                                     guint(MOD_ALT)   | 
                                                     guint(MOD_LWIN)  | 
                                                     guint(MOD_RWIN) );
const Gdk::ModifierType MOD_DISABLED = Gdk::RELEASE_MASK;

const guint DBL_CLICK = 1<<31;

enum DefinitionType { DEF_ACTION, DEF_KEY_ACTION, DEF_BUTTON_ACTION, DEF_MODIFIER };

class AccelManager
{
public:
	static AccelManager& get();

	// structure of an accelerator/action definition
	struct Definition {
		// identification
		guint nr;
		std::string id;
		std::vector<Glib::ustring> location;
		Glib::ustring description;
		// definition
		DefinitionType type;
		std::vector<std::string> allowed_links;
		// default
		std::string link;
		guint default_button, default_key;
		Gdk::ModifierType default_modifiers;
	};
	typedef std::vector<Definition> DefinitionMap;

	// current accelerator/action definition
	struct Assignment {
		std::string link;
		guint button, key;
		Gdk::ModifierType modifiers;
		bool updated;
	};
	typedef std::vector<Assignment> AssignmentMap;

	// register accelerators
	void addAccelMap( const std::string& context, const DefinitionMap& map );

	// access to assignments
	std::vector<std::string> getContexts() const;
	const DefinitionMap& getAccelDefinitons( const std::string& context ) const;
	const Definition& getAccelDefiniton( const std::string& context, const std::string& id ) const;
	const AssignmentMap& getAccelContext( const std::string& context ) const;
	Assignment& getAccelAssignment( const std::string& context, const std::string& id );
	void updateLink( const std::string& context, guint accel );
	Glib::ustring linkDisplayText( const std::string& context, const std::string& link );
	
	void changed();
	void save();

private:
	AccelManager();
	~AccelManager();
	
	int m_NextId;
	bool m_Changed;
	std::map<const std::string, const DefinitionMap *> m_Definitions;
	std::map<const std::string, AssignmentMap> m_Assignments;

	Storage m_Storage;
};

} // namespace Polka

#endif // _POLKA_ACCELMANAGER_H_

