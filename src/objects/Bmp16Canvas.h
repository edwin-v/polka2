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

	bool canCreate( Project& _prj ) const;
	Object *create( Project& _prj ) const;
	ObjectPropertySheet *createPropertySheet( Object& _obj, int nr ) const;
};

} // namespace Polka

#endif // _POLKA_BMP16CANVAS_H_
