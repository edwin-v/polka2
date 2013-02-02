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

#ifndef _POLKA_TILECANVAS_H_
#define _POLKA_TILECANVAS_H_

#include "Object.h"
#include "ObjectManager.h"
#include "Project.h"
#include <glibmm/i18n.h>
#include <cairomm/surface.h>


namespace Polka {

class Palette;

static const int DEP_TCPAL = 0;
static const char *DEP_TCPAL_TYPES = "PAL2,PAL1";

class TileCanvas : public Object 
{
public:
	TileCanvas( Project& _prj );
	~TileCanvas();

	int width() const;
	int height() const;
	
	const Palette *getPalette() const;
	Cairo::RefPtr<Cairo::ImageSurface> getImage() const;
	void setPalette( const Palette& palette );

protected:
	void doUpdate();
	
private:
	Cairo::RefPtr<Cairo::ImageSurface> m_Image;
	const Palette *m_Palette;
};

class TileCanvasFactory : public ObjectManager::ObjectFactory
{
public:
	TileCanvasFactory()
		: ObjectManager::ObjectFactory( _("Tile Canvas"),
		                                _("Sketch canvasses"), "0020CANV",
		                                _("Canvas for sketching Screen 2 and Screen 4 graphics."),
		                                "TILECANVAS", "TILECANVASEDIT",
		                                "sketch" ) {}

	bool canCreate( Project& _prj ) const { return _prj.findObjectOfTypes(DEP_TCPAL_TYPES) != 0; }
	Object *create( Project& _prj ) const { return new TileCanvas(_prj); }
};


} // namespace Polka

#endif // _POLKA_TILECANVAS_H_
