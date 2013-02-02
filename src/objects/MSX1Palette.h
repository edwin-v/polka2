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

#ifndef _POLKA_MSX1PALETTE_H_
#define _POLKA_MSX1PALETTE_H_

#include "ObjectManager.h"
#include "Palette.h"
#include <glibmm/i18n.h>


namespace Polka {

static const char *MSX1PAL_ID = "PAL/16/MSX1";

class MSX1Palette : public Palette 
{
public:
	MSX1Palette( Project& _prj );
	~MSX1Palette();
};


class MSX1PaletteFactory : public ObjectManager::ObjectFactory
{
public:
	MSX1PaletteFactory()
		: ObjectManager::ObjectFactory( _("MSX1 Palette"),
		                                _("Palettes"), "0010PAL",
		                                _("Unchangable palette for displaying original MSX colours."),
		                                MSX1PAL_ID, "", "object_palmsx1" ) {}

	Object *create( Project& _prj ) const { return new MSX1Palette(_prj); }
};


} // namespace Polka

#endif // _POLKA_MSX1PALETTE_H_
