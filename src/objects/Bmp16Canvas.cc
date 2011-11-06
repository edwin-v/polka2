#include "Bmp16Canvas.h"
#include "CanvasData.h"


namespace Polka {


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

} // namespace Polka
