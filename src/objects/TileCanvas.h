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
