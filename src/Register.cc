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

	//manager.registerObject( new TileCanvasFactory() );
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
