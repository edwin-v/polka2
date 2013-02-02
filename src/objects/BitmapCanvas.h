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

#ifndef _POLKA_BITMAPCANVAS_H_
#define _POLKA_BITMAPCANVAS_H_

#include "Object.h"
#include "ObjectManager.h"
#include "icons/sketch.c"
#include <glibmm/i18n.h>
#include <cairomm/surface.h>


namespace Polka {
/*
class Palette;

class BitmapCanvas : public Polka::Object 
{
public:
	BitmapCanvas( int depth );
	~BitmapCanvas();

	enum Type { BP2, BP4, BP6, BRGB332, BRGB555 };

	int width() const;
	int height() const;
	
	const Palette *getPalette( unsigned int slot = 0 ) const;
	Cairo::RefPtr<Cairo::ImageSurface> getImage() const;
	void setPalette( const Palette& palette, unsigned int slot = 0 );

protected:
	virtual void doUpdate();
	
private:
	Cairo::RefPtr<Cairo::ImageSurface> m_Image;
	std::vector<char*> m_Data;
	std::vector<const Palette *> m_Palettes;
};

class Bitmap16CanvasFactory : public ObjectManager::ObjectFactory
{
public:
	BitmapCanvasFactory()
		: ObjectManager::ObjectFactory( _("16 Color Canvas"),
		                                _("Sketch canvasses"), 20,
		                                "BMP16CANVAS", "BITMAPCANVASEDIT",
		                                "PAL1,PAL2,PALG9K",
		                                sketch ) {}

	Object *create() const { return new BitmapCanvas( BitmapCanvas::BP4 ); }
};
*/

} // namespace Polka

#endif // _POLKA_BITMAPCANVAS_H_
