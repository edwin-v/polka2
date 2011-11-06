#include "BitmapScreenImport.h"
#include "Palette.h"
#include "Bmp16Canvas.h"
#include "Project.h"
#include "Functions.h"
#include "Storage.h"
#include "StorageHelpers.h"
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
	m_Preview.addType( _("MSX2 Palette"), true );
	m_Preview.setTargetText( _("Creates a single MSX2\nPalette object.") );
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
	
	// create import storage for objects
	Storage& impS = project.createImportObjects(name);

	// first import palette
	if( m_PaletteOffset >= 0 ) {

		// create new palette object
		Storage& s = impS.createObject("PAL2");
		palname = project.createUniqueName( name + _(" Palette") );
		storageSetObjectName( s, palname );
		s.createItem("DEPTH", "I");
		s.setField(0, 3);
		s.createItem( "RGB", "[FFF]" );
		int addr = m_PaletteOffset;
		for( int c = 0; c < 16; c++ ) {
			s.setField( c, 0, double( (m_pData[addr  ] >> 4) & 7 )/7.0 );
			s.setField( c, 2, double(  m_pData[addr++]       & 7 )/7.0 );
			s.setField( c, 1, double(  m_pData[addr++]       & 7 )/7.0 );
		}
		
	} else if( m_PossibleTypes.size() ) {
		// no palette exists, create a default one if required
		switch( m_PossibleTypes[m_Preview.targetId()] ) {
			case SC5:
			{
				if( !project.checkObjectRequirements("BMP16CANVAS") ) {
					Storage& s = impS.createObject("PAL2");
					palname = project.createUniqueName( name + _(" Palette") );
					storageSetObjectName( s, palname );
					s.createItem("DEPTH", "I");
					s.setField(0, 3);
					// create dummy color
					s.createItem("FIRST_COLOR", "I");
					s.setField(0, 0);
					s.createItem( "RGB", "[FFF]" );
					s.setField(0, 0.0);
					s.setField(1, 0.0);
					s.setField(2, 0.0);
				} else {
					// get name of existing palette
					Polka::Object *obj = project.findObjectOfTypes("PAL2,PAL1,PAL9");
					assert(obj);
					palname = obj->name();
				}
				break;
			}
			default:
				break;
		}
	}
	
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

				Storage& s = impS.createObject("BMP16CANVAS");
				std::string scname = project.createUniqueName( name + _(" Canvas") );
				storageSetObjectName( s, scname );
			
				// size
				Storage& sm = s.createObject("DATA_MAIN");
				sm.createItem("DATA_SIZE", "II");
				sm.setField( 0, 256 );
				sm.setField( 1, vres );
			
				// palette
				sm.createItem("PALETTE", "S");
				sm.setField( 0, palname );
				
				// data
				sm.createItem("DATA", "S");
				std::string& data = sm.setDataField(0);
				data.resize( 256*startline + startpix, 0 );

				int addr = 0;
				while( addr <= m_EndAddress ) {
					data += char( (m_pData[addr] >> 4) & 15 );
					data += char(  m_pData[addr++]     & 15 );
				}
				
				data.resize( 256*vres, 0 );
				
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

				Storage& s = impS.createObject("BMP16CANVAS");
				std::string scname = project.createUniqueName( name + _(" Canvas") );
				storageSetObjectName( s, scname );
				
				// sc7 scale
				s.createItem("PIXEL_SCALE", "II");
				s.setField( 0, 1 );
				s.setField( 1, 2 );
			
				// size
				Storage& sm = s.createObject("DATA_MAIN");
				sm.createItem("DATA_SIZE", "II");
				sm.setField( 0, 512 );
				sm.setField( 1, vres );
			
				// palette
				sm.createItem("PALETTE", "S");
				sm.setField( 0, palname );
				
				// data
				sm.createItem("DATA", "S");
				std::string& data = sm.setDataField(0);
				data.resize( 512*startline + startpix, 0 );

				int addr = 0;
				while( addr <= m_EndAddress ) {
					data += char( (m_pData[addr] >> 4) & 15 );
					data += char(  m_pData[addr++]     & 15 );
				}
				
				data.resize( 512*vres, 0 );
				
				break;
			}
			default:
				break;
		}
	}
	
	project.finishImportObjects();
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
