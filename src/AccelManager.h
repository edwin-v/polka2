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

const Gdk::ModifierType MOD_ALL = MOD_SHIFT | MOD_CTRL | MOD_ALT | MOD_LWIN | MOD_RWIN;
const Gdk::ModifierType MOD_DISABLED = Gdk::RELEASE_MASK;

const guint DBL_CLICK = 1<<31;

enum DefinitionType { DEF_ACTION, DEF_MODIFIER };

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
	const AssignmentMap& getAccelContext( const std::string& context );
	void updateLink( const std::string& context, guint accel );

private:
	AccelManager();
	~AccelManager();
	
	int m_NextId;
	std::map<const std::string, const DefinitionMap *> m_Definitions;
	std::map<const std::string, AssignmentMap> m_Assignments;

	Storage m_Storage;
};

} // namespace Polka

#endif // _POLKA_ACCELMANAGER_H_

