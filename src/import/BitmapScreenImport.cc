#include "BitmapScreenImport.h"
#include "Palette.h"
#include "Bmp16Canvas.h"
#include "Project.h"
#include "Functions.h"
#include "Storage.h"
#include "StorageHelpers.h"
#include "ResourceManager.h"
#include <iostream>
#include <cassert>

using namespace std;

namespace Polka {



BitmapScreenImporter::BitmapScreenImporter()
	: Importer("BMPSCRIMP"), m_pData(0), m_DetectedType(-1), m_PaletteOffset(-1),
	  m_Preview(*this)
{
}

BitmapScreenImporter::~BitmapScreenImporter()
{
}

Gtk::Widget& BitmapScreenImporter::getPreviewWidget()
{
	return m_Preview;
}

bool BitmapScreenImporter::initImport( const std::string& filename )
{
	cleanUp();

	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate );
	if( !file.is_open() ) return false;
	m_FileName = filename;	
	
	// get file size
	m_FileSize = file.tellg();
	file.seekg(0);
	
	// possible pallette?
	if( m_FileSize == 32 )  
		return tryOpenPalette(file);
	else if( m_FileSize == 48 )  
		return tryOpenG9kPalette(file);
	
	// bsaved file check
	if( tryOpenBSaved(file) ) {
		// bsaved file can't be anything else
		for( unsigned int i = 0; i < m_PossibleTypes.size(); i++ )
			m_Preview.addType( Glib::ustring::compose( _("Screen %1 Graphics"), m_PossibleTypes[i]+5-SC5 ),
			                   m_DetectedType == int(i) );
		return true;
	} else {
		file.seekg(0, ios::beg);
	}

	// check Copy'd file
	return false;
}

void BitmapScreenImporter::cancelImport()
{
	cleanUp();
}

bool BitmapScreenImporter::tryOpenPalette( ifstream& stream )
{
	m_pData = new char[32];
	stream.read( m_pData, 32 );
	// check for valid data 
	if( !checkPalette( m_pData ) )  {
		cleanUp();
		return false;
	}
	// good palette data
	m_PaletteOffset = 0;
	m_PaletteG9k = false;
	m_Preview.addType( _("MSX2 Palette"), true );
	m_Preview.setTargetText( _("Creates a single MSX2\nPalette object.") );
	return true;
}

bool BitmapScreenImporter::tryOpenG9kPalette( ifstream& stream )
{
	m_pData = new char[48];
	stream.read( m_pData, 48 );
	// check for valid data 
	if( !checkG9kPalette( m_pData ) )  {
		cleanUp();
		return false;
	}
	// good palette data
	m_PaletteOffset = 0;
	m_PaletteG9k = true;
	m_Preview.addType( _("Gfx9000 Palette"), true );
	m_Preview.setTargetText( _("Creates a single Gfx9000\nPalette object.") );
	return true;
}

bool BitmapScreenImporter::tryOpenBSaved( ifstream& stream )
{
	unsigned char header[7];
	stream.read( (char *)header, 7 );

	// bsave header
	if( header[0] != 0xFE ) return false;

	// check bsave file size 
	m_StartAddress = (unsigned char)header[1] + 256*(unsigned char)header[2];
	m_EndAddress   = (unsigned char)header[3] + 256*(unsigned char)header[4];
	int size = 1 + m_EndAddress - m_StartAddress;
	// hack for end address inconsistency
	if( 7 + size == m_FileSize+1 ) { m_EndAddress--; size--; }
	if( m_FileSize != 7+size ) return false;

	// load data
	m_pData = new char[size];
	stream.read( m_pData, size );
	// check existence of palette
	if( m_EndAddress >= 0xFA80+31 ) {
		if( checkPalette( m_pData + 0xFA80 - m_StartAddress ) ) {
			// found palette at high address, possible sc7 or sc8
			m_PossibleTypes.push_back(SC7);
			m_PossibleTypes.push_back(SC8);
			// select type
			switch( getBitmapTypeByFileName() ) {
				case SC8:
					m_DetectedType = 1;
					break;
				default:
					m_DetectedType = 0;
			}
			// set palette
			m_PaletteOffset = 0xFA80-m_StartAddress;
			return true;
		}
	}
	if( m_EndAddress >= 0x7680+31 ) {
		if( checkPalette( m_pData + 0x7680 - m_StartAddress ) ) {
			// found palette at low address, possible sc5 or sc6
			m_PossibleTypes.push_back(SC5);
			m_PossibleTypes.push_back(SC6);
			// select type
			switch( getBitmapTypeByFileName() ) {
				case SC6:
					m_DetectedType = 1;
					break;
				default:
					m_DetectedType = 0;
			}
			// set palette
			m_PaletteOffset = 0x7680-m_StartAddress;
			return true;
		}
	}

	// no pallette everything is possible really
	m_PossibleTypes.push_back(SC5);
	m_PossibleTypes.push_back(SC6);
	m_PossibleTypes.push_back(SC7);
	m_PossibleTypes.push_back(SC8);
	
	// select type
	m_DetectedType = getBitmapTypeByFileName()-SC5;
	
	if( m_DetectedType < 0 ) {
		if( m_EndAddress < 32768 )
			m_DetectedType = 0;
		else
			m_DetectedType = 3;
	}
	
	return true;
}

void BitmapScreenImporter::cleanUp()
{
	delete [] m_pData;
	m_pData = 0;
	m_PossibleTypes.clear();
	m_DetectedType = -1;
	m_PaletteOffset = -1;
	m_Preview.reset();
}

bool BitmapScreenImporter::checkPalette( const char *data ) const
{
	// check if data is %01110111 %00000111 and not all zero
	bool hasdata = false;
	for( int i = 0; i<32 ; i+=2 )
		if( data[i] & 0x88 || data[i+1] & 0x08 )
			return false;
		else if( data[i] )
			hasdata = true;

	return hasdata;
}

bool BitmapScreenImporter::checkG9kPalette( const char *data ) const
{
	// check if data is all lower than 32 and not all zero
	for( int i = 0; i<48 ; i+=1 )
		if( data[i] >= 32 )
			return false;
	return true;
}

BitmapScreenImporter::BitmapType BitmapScreenImporter::getBitmapTypeByFileName()
{
	char last = m_FileName[m_FileName.size()-1];
	if( last == '5' )
		return SC5;
	else if( last == '6' )
		return SC6;
	else if( last == '7' )
		return SC7;
	else if( last == '8' )
		return SC8;
	
	return NONE;
}

void BitmapScreenImporter::updateTarget( int id )
{
	if( id<0 || id>=int(m_PossibleTypes.size()) ) return;
	
	Glib::ustring base;

	if( m_PaletteOffset >= 0 )
		base = "Create both a %1 color canvas\nobject and a palette object.";
	else if( m_PossibleTypes[id] == SC8 )
		base = "Create a 256 color canvas.";
	else
		base = "Creates a %1 color canvas object\nand a default MSX2 palette if\nno other palette exists.";
		
	int c = 16;
	if( m_PossibleTypes[id] == SC6 ) c = 4;

	m_Preview.setTargetText( Glib::ustring::compose( base, c ) );
}

bool BitmapScreenImporter::importToProject( Project& project )
{
	// default import name
	Glib::ustring palname, name = getNameFromFilename(m_FileName);
	Palette *pal = 0;
	
	// create import storage for objects
	project.undoHistory().createUndoPoint( _("Import of ") + name, ResourceManager::get().getIcon("import") );

	// first import palette
	if( m_PaletteOffset >= 0 ) {
		// create a new palette
		palname = project.createUniqueName( name + _(" Palette") );

		// space for palette for temporary storage
		double r[16], g[16], b[16];

		// create new palette object
		if( m_PaletteG9k ) {
			int addr = m_PaletteOffset;
			for( int c = 0; c < 16; c++ ) {
				r[c] = double(m_pData[addr++]) / 31.0;
				g[c] = double(m_pData[addr++]) / 31.0;
				b[c] = double(m_pData[addr++]) / 31.0;
			}
			pal = dynamic_cast<Palette*>( project.createNewObject("PAL/16/G9K") );
		} else {
			int addr = m_PaletteOffset;
			for( int c = 0; c < 16; c++ ) {
				r[c] = double( (m_pData[addr  ] >> 4) & 7 )/7.0;
				b[c] = double(  m_pData[addr++]       & 7 )/7.0;
				g[c] = double(  m_pData[addr++]       & 7 )/7.0;
			}
			pal = dynamic_cast<Palette*>( project.createNewObject("PAL/16/MSX2") );
		}
		
		pal->setColors( 0, 16, r, g, b );
		project.setObjectName( *pal, palname );
		
	} else if( m_PossibleTypes.size() ) {
		// no palette exists, create a default one if required
		switch( m_PossibleTypes[m_Preview.targetId()] ) {
			case SC5:
			{
				if( !project.checkObjectRequirements("CANVAS/16/BMP") ) {
					pal = dynamic_cast<Palette*>( project.createNewObject("PAL/16/MSX2") );
					palname = project.createUniqueName( name + _(" Palette") );
					project.setObjectName( *pal, palname );
				} else {
					// get name of existing palette
					pal = dynamic_cast<Palette*>( project.findObjectOfTypes("PAL/16/") );
					palname = pal->name();
				}
				break;
			}
			default:
				break;
		}
	}
	
	assert(pal); // should always exist at this point
	
	// create canvas
	if( m_PossibleTypes.size() ) {
		switch( m_PossibleTypes[m_Preview.targetId()] ) {
			case SC5:
			{
				// calculate the number of lines
				int startline = m_StartAddress >> 7, startpix = (m_StartAddress & 127)*2;
				if( m_PaletteOffset >= 0 ) {
					// including palette, last line is 212
					m_EndAddress = 128*212-1;
				}
				int vres = 1 + (m_EndAddress >> 7);

				Canvas *canvas = dynamic_cast<Canvas*>( project.createNewObject("CANVAS/16/BMP") );
				std::string scname = project.createUniqueName( name + _(" Canvas") );
				project.setObjectName( *canvas, scname );
				canvas->setPalette( *pal );

				// size
				canvas->resize( 256, vres, 1, 1, true );

				// data
				std::string data;
				data.resize( 256*startline + startpix, 0 );

				int addr = 0;
				while( addr <= m_EndAddress ) {
					data += char( (m_pData[addr] >> 4) & 15 );
					data += char(  m_pData[addr++]     & 15 );
				}
				
				data.resize( 256*vres, 0 );
				canvas->setData( 0, 0, data.c_str(), 256, vres );
				
				break;
			}
			case SC7:
			{
				// calculate the number of lines
				int startline = m_StartAddress >> 7, startpix = (m_StartAddress & 255)*2;
				if( m_PaletteOffset >= 0 ) {
					// including palette, last line is 212
					m_EndAddress = 256*212-1;
				}
				int vres = 1 + (m_EndAddress >> 7);

				Canvas *canvas = dynamic_cast<Canvas*>( project.createNewObject("CANVAS/16/BMP") );
				std::string scname = project.createUniqueName( name + _(" Canvas") );
				project.setObjectName( *canvas, scname );
				canvas->setPalette( *pal );

				// size
				canvas->resize( 512, vres, 1, 2, true );
				
				// data
				std::string data;
				data.resize( 512*startline + startpix, 0 );

				int addr = 0;
				while( addr <= m_EndAddress ) {
					data += char( (m_pData[addr] >> 4) & 15 );
					data += char(  m_pData[addr++]     & 15 );
				}
				
				data.resize( 512*vres, 0 );
				canvas->setData( 0, 0, data.c_str(), 512, vres );
				
				break;
			}
			default:
				break;
		}
	}
	
	return true;
}


/*************************
 * Import Preview Widget *
 *************************/


BitmapScreenImporter::BitmapPreviewWidget::BitmapPreviewWidget( BitmapScreenImporter& importer )
	: m_Importer(importer), m_ImportLabel( _("<b>Import MSX2 Graphics</b>") ),
	  m_TypeLabel( _("Select the image format:"), 0.0, 0.5 ),
	  m_TargetLabel( "", 0.0, 0.0 )
{
	set_spacing(5);
	set_border_width(8);
	
	m_ImportLabel.set_use_markup();
	
	pack_start( m_ImportLabel, Gtk::PACK_SHRINK );
	pack_start( m_TypeLabel, Gtk::PACK_SHRINK );
	pack_start(	m_ComboTypes, Gtk::PACK_SHRINK );
	pack_start(	m_TargetLabel );
	show_all_children();
	
	m_ComboTypes.signal_changed().connect( sigc::mem_fun(*this, &BitmapScreenImporter::BitmapPreviewWidget::changeTarget) );

}

void BitmapScreenImporter::BitmapPreviewWidget::reset()
{
	m_ComboTypes.remove_all();
	m_TargetLabel.set_text("");
}

void BitmapScreenImporter::BitmapPreviewWidget::addType( const Glib::ustring& text, bool active )
{
	m_ComboTypes.append(text);
	if( active ) m_ComboTypes.set_active( m_ComboTypes.get_model()->children().size()-1 );
}

void BitmapScreenImporter::BitmapPreviewWidget::setTargetText( const Glib::ustring& text )
{
	m_TargetLabel.set_text( text );
}

void BitmapScreenImporter::BitmapPreviewWidget::changeTarget()
{
	m_Importer.updateTarget( m_ComboTypes.get_active_row_number() );
}

int BitmapScreenImporter::BitmapPreviewWidget::targetId() const
{
	return m_ComboTypes.get_active_row_number();
}


} // namespace Polka 
