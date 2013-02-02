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

#ifndef _POLKA_BMP16CANVAS_H_
#define _POLKA_BMP16CANVAS_H_

#include "Canvas.h"
#include "Project.h"
#include "ObjectManager.h"

namespace Polka {

class Palette;
class ObjectPropertySheet;

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
	Bmp16CanvasFactory();

	bool canCreate( const Project& _prj ) const;
	Object *create( Project& _prj ) const;
	ObjectPropertySheet *createPropertySheet( Object& _obj, int nr ) const;
};

} // namespace Polka

#endif // _POLKA_BMP16CANVAS_H_
