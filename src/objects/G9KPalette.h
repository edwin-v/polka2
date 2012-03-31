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
