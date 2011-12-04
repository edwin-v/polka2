#include "Register.h"
#include "ObjectManager.h"
#include "ImportManager.h"
#include "MSX1Palette.h"
#include "MSX2Palette.h"
#include "G9KPalette.h"
#include "G9KPalette64.h"
#include "PaletteEditor.h"
#include "TileCanvas.h"
#include "TileCanvasEditor.h"
#include "Bmp16Canvas.h"
#include "CanvasEditor.h"

#include "BitmapScreenImport.h"
#include "ImageImport.h"
#include "RawImport.h"

namespace Polka {

void registerObjects( ObjectManager& manager )
{
	manager.registerObject( new MSX1PaletteFactory() );
	manager.registerObject( new MSX2PaletteFactory() );
	manager.registerObject( new G9KPaletteFactory() );
	manager.registerObject( new G9KPalette64Factory() );
	manager.registerEditor( new PaletteEditorFactory() );

	manager.registerObject( new TileCanvasFactory() );
	manager.registerEditor( new TileCanvasEditorFactory() );
	manager.registerObject( new Bmp16CanvasFactory() );
	manager.registerEditor( new CanvasEditorFactory() );
}

void registerImporters( ImportManager& manager )
{
	manager.registerImporter( new BitmapScreenImporter() );
	manager.registerImporter( new ImageImporter() );
	manager.registerImporter( new RawImporter() );
}

}
