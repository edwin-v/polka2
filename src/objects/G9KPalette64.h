#ifndef _POLKA_G9KPALETTE64_H_
#define _POLKA_G9KPALETTE64_H_

#include "ObjectManager.h"
#include "Palette.h"
#include <glibmm/i18n.h>


namespace Polka {

static const char *G9KPAL4_ID = "PAL/64/G9K";

class G9KPalette64 : public Palette 
{
public:
	G9KPalette64( Project& _prj );
	~G9KPalette64();
};


class G9KPalette64Factory : public ObjectManager::ObjectFactory
{
public:
	G9KPalette64Factory()
		: ObjectManager::ObjectFactory( _("64 colour V9990 Palette"),
		                                _("Palettes"), "0010PAL",
		                                _("64 colour palette for the V9990/Graphics9000 video display processor."),
		                                G9KPAL4_ID, "PALEDIT",
		                                "object_palg9k64" ) {}

	Object *create( Project& _prj ) const { return new G9KPalette64(_prj); }
};


} // namespace Polka

#endif // _POLKA_G9KPALETTE64_H_
