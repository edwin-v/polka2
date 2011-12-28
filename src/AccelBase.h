#ifndef _POLKA_ACCELBASE_H_
#define _POLKA_ACCELBASE_H_

#include <AccelManager.h>
#include <gdkmm/types.h>
#include <glibmm/ustring.h>
#include <string>
#include <vector>

namespace Polka {


class AccelBase
{
public:
	AccelBase( const std::string& _id );
	virtual ~AccelBase();


protected:
	// check accelerator
	bool isAccel( guint id, guint button, guint key, guint mods, bool exact = false );
	bool isAccel( guint id, guint button, guint key );
	int isAccel( const std::vector<guint> ids, guint button, guint key, guint mods, bool exact = false );
	int isAccel( const std::vector<guint> ids, guint button, guint key );
	bool isAccelMod( guint id, guint mods, bool exact = false );
	// helpers
	bool updateAccel( guint id1, guint id2, bool first, guint mods, guint idflip = -1 );
	bool guessAccel( guint id1, guint id2, guint mods, guint idflip = -1 );
	guint accelEventButton( const GdkEventButton *event ) const;
	
private:
	std::string m_Id;
	const AccelManager::AssignmentMap& m_Accels;
	
	int modCount( guint mods );
	
};

} // namespace Polka

#endif // _POLKA_ACCELBASE_H_
