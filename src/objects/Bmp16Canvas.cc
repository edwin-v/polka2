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

#include "Bmp16Canvas.h"
#include "CanvasData.h"
#include "BitmapCanvasPropertySheet.h"
#include <cairomm/surface.h>
#include <glibmm/i18n.h>


namespace Polka {

static const char *BMP16CANVAS_ID = "CANVAS/16/BMP";
static const char *DEP_PAL_TYPES = "PAL/16/";


Bmp16Canvas::Bmp16Canvas( Project& _prj )
	: Canvas( _prj, BMP16CANVAS_ID )
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
		                                BMP16CANVAS_ID, "CANVASEDIT",
		                                "object_sketch16" )
{
}

bool Bmp16CanvasFactory::canCreate( const Project& _prj ) const
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
