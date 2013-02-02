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

#ifndef _POLKA_G9KPALETTE_H_
#define _POLKA_G9KPALETTE_H_

#include "ObjectManager.h"
#include "Palette.h"
#include <glibmm/i18n.h>


namespace Polka {

static const char *G9KPAL_ID = "PAL/16/G9K";

class G9KPalette : public Palette 
{
public:
	G9KPalette( Project& _prj );
	~G9KPalette();
};


class G9KPaletteFactory : public ObjectManager::ObjectFactory
{
public:
	G9KPaletteFactory()
		: ObjectManager::ObjectFactory( _("V9990 Palette"),
		                                _("Palettes"), "0010PAL",
		                                _("16 colour palette for the V9990/Graphics9000 video display processor."),
		                                G9KPAL_ID, "PALEDIT",
		                                "object_palg9k" ) {}

	Object *create( Project& _prj ) const { return new G9KPalette(_prj); }
};


} // namespace Polka

#endif // _POLKA_G9KPALETTE_H_
