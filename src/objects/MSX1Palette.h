#ifndef _POLKA_MSX1PALETTE_H_
#define _POLKA_MSX1PALETTE_H_

#include "ObjectManager.h"
#include "Palette.h"
#include <glibmm/i18n.h>


namespace Polka {


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
		                                "PAL1", "", "object_palmsx1" ) {}

	Object *create( Project& _prj ) const { return new MSX1Palette(_prj); }
};


} // namespace Polka

#endif // _POLKA_MSX1PALETTE_H_
