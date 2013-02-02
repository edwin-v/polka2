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
	static guint accelEventButton( const GdkEventButton *event );
	static bool keyIsMod( guint key );
	static guint keyToMod( guint key );
	
private:
	std::string m_Id;
	const AccelManager::AssignmentMap& m_Accels;
	
	int modCount( guint mods );
	
};

} // namespace Polka

#endif // _POLKA_ACCELBASE_H_
