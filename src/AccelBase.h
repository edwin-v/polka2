#ifndef _POLKA_ACCELBASE_H_
#define _POLKA_ACCELBASE_H_

#include <gdkmm/types.h>
#include <glibmm/ustring.h>
#include <string>
#include <vector>

namespace Polka {

// convenience modifier
const Gdk::ModifierType MOD_SHIFT = Gdk::SHIFT_MASK;
const Gdk::ModifierType MOD_CTRL = Gdk::CONTROL_MASK;
const Gdk::ModifierType MOD_ALT = Gdk::MOD1_MASK;
const Gdk::ModifierType MOD_LWIN = Gdk::MOD4_MASK;
const Gdk::ModifierType MOD_RWIN = Gdk::MOD5_MASK;

class AccelBase
{
public:
	AccelBase( const std::string& _id );
	virtual ~AccelBase();


protected:
	// shortcut/accelerator/actions helpers
	
	// create accelerator
	void accAdd( guint id, const Glib::ustring& path, guint button, guint key = 0, Gdk::ModifierType mods = Gdk::ModifierType(0) );
	// retrieve accelerator definition
	const Glib::ustring& accPath( guint id );
	guint accButton( guint id );
	guint accKey( guint id );
	// check accelerator
	bool checkAccButton( guint id, guint button, guint mods = 0, bool exact = false );
	bool checkAccKey( guint id, guint key, guint mods = 0, bool exact = false );
	bool checkAccMods( guint id, guint mods, bool exact = false );
	
private:
	std::string m_Id;

	// accelerators
	struct Accel {
		Glib::ustring path;
		guint button, key;
		Gdk::ModifierType mods;
	};
	std::vector<Accel> m_Accels;
	
};

} // namespace Polka

#endif // _POLKA_ACCELBASE_H_
