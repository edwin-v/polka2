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

#ifndef _POLKA_MSX2PALETTE_H_
#define _POLKA_MSX2PALETTE_H_

#include "ObjectManager.h"
#include "Palette.h"
#include <glibmm/i18n.h>


namespace Polka {

static const char *MSX2PAL_ID = "PAL/16/MSX2";

class MSX2Palette : public Palette 
{
public:
	MSX2Palette( Project& _prj );
	~MSX2Palette();
};


class MSX2PaletteFactory : public ObjectManager::ObjectFactory
{
public:
	MSX2PaletteFactory()
		: ObjectManager::ObjectFactory( _("MSX2 Palette"),
		                                _("Palettes"), "0010PAL",
		                                _("16 colour MSX2 palette"),
		                                MSX2PAL_ID, "PALEDIT",
		                                "object_palmsx2" ) {}

	Object *create( Project& _prj ) const { return new MSX2Palette(_prj); }
};


} // namespace Polka

#endif // _POLKA_MSX2PALETTE_H_
