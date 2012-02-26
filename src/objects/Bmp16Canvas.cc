#include "Bmp16Canvas.h"
#include "CanvasData.h"
#include "BitmapCanvasPropertySheet.h"
#include <cairomm/surface.h>
#include <glibmm/i18n.h>


namespace Polka {

static const char *DEP_PAL_TYPES = "PAL1,PAL2,PAL9";


Bmp16Canvas::Bmp16Canvas( Project& _prj )
	: Canvas( _prj, "BMP16CANVAS" )
{
	if( !registerDependency( DEP_PAL, DEP_PAL_TYPES ) )
		throw(1);
	
	m_pData = new CanvasData( *this, 256, 128, 4 );
	setClipRectangle();
	update();
}

Bmp16Canvas::~Bmp16Canvas()
{
}



/* Bmp16CanvasFactory */

Bmp16CanvasFactory::Bmp16CanvasFactory()
		: ObjectManager::ObjectFactory( _("16 Colour Canvas"),
		                                _("Sketch canvasses"), "0020CANV",
		                                _("Canvas for sketching 16 colour graphics using a palette. Usable for Screen 5 mode and V9990 P1 and B modes."),
		                                "BMP16CANVAS", "CANVASEDIT",
		                                "object_sketch16" )
{
}

bool Bmp16CanvasFactory::canCreate( Project& _prj ) const
{
	return _prj.findObjectOfTypes(DEP_PAL_TYPES) != 0;
}

Object *Bmp16CanvasFactory::create( Project& _prj ) const
{
	return new Bmp16Canvas(_prj);
}

ObjectPropertySheet *Bmp16CanvasFactory::createPropertySheet( Object& _obj, int nr ) const
{
	switch(nr) {
		case 0:
			return new BitmapCanvasPropertySheet( dynamic_cast<Canvas&>(_obj) );
		case 1:
			return new BitmapCanvasGridSheet( dynamic_cast<Canvas&>(_obj) );
		default:
			return 0;
	}
}


} // namespace Polka
