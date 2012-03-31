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
