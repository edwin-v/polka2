#ifndef _POLKA_BMP16CANVAS_H_
#define _POLKA_BMP16CANVAS_H_

#include "Canvas.h"
#include "Project.h"
#include "ObjectManager.h"
#include "BitmapCanvasPropertySheet.h"
#include <glibmm/i18n.h>
#include <cairomm/surface.h>

namespace Polka {

class Palette;
class ObjectPropertySheet;

static const char *DEP_PAL_TYPES = "PAL1,PAL2,PAL9";

class Bmp16Canvas : public Polka::Canvas 
{
public:
	Bmp16Canvas( Project& _prj );
	~Bmp16Canvas();

private:
};

class Bmp16CanvasFactory : public ObjectManager::ObjectFactory
{
public:
	Bmp16CanvasFactory()
		: ObjectManager::ObjectFactory( _("16 Colour Canvas"),
		                                _("Sketch canvasses"), "0020CANV",
		                                _("Canvas for sketching 16 colour graphics using a palette. Usable for Screen 5 mode and V9990 P1 and B modes."),
		                                "BMP16CANVAS", "CANVASEDIT",
		                                "object_sketch16" ) {}

	bool canCreate( Project& _prj ) const { return _prj.findObjectOfTypes(DEP_PAL_TYPES) != 0; }
	Object *create( Project& _prj ) const { return new Bmp16Canvas(_prj); }
	ObjectPropertySheet *createPropertySheet( Object& _obj ) const { return new BitmapCanvasPropertySheet( dynamic_cast<Canvas&>(_obj) ); }
};

} // namespace Polka

#endif // _POLKA_BMP16CANVAS_H_
