#include "RawImport.h"
#include "Palette.h"
#include "Bmp16Canvas.h"
#include "Project.h"
#include "Functions.h"
#include "Storage.h"
#include "StorageHelpers.h"
#include <gtkmm/table.h>
#include <iostream>
#include <cassert>

using namespace std;

namespace Polka {



RawImporter::RawImporter()
	: Importer("RAWIMP"), m_Preview(*this)
{
}

RawImporter::~RawImporter()
{
}

Gtk::Widget& RawImporter::getPreviewWidget()
{
	return m_Preview;
}

bool RawImporter::initImport( const std::string& filename )
{
	cleanUp();

	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate );
	if( !file.is_open() ) return false;
	m_FileName = filename;	
	
	// get file size
	m_FileSize = file.tellg();
	file.seekg(0);

	// don't assume anything
	m_Preview.addType("4 bit, high first", true);

	// always possible
	return true;
}

void RawImporter::cancelImport()
{
	cleanUp();
}

void RawImporter::cleanUp()
{
	m_Preview.reset();
}

void RawImporter::updateTarget( int id )
{
	// dummy
}

bool RawImporter::importToProject( Project& project )
{
	// open file
	ifstream file(m_FileName.c_str(), ios::in | ios::binary);
	if( !file.is_open() ) return false;

	file.seekg( m_Preview.offset() );

	// default import name
	Glib::ustring palname, name = m_FileName;
	
	// create import storage for objects
	Storage& impS = project.createImportObjects(name);

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
	
	// create canvas

	// calculate the number of lines
	int size = m_Preview.width()/2 * m_Preview.height();
	char *data = new char[size];
	memset( data, 0, size );
	file.read( data, size );

	Storage& s = impS.createObject("BMP16CANVAS");
	std::string scname = project.createUniqueName( name + _(" Canvas") );
	storageSetObjectName( s, scname );

	// size
	Storage& sm = s.createObject("DATA_MAIN");
	sm.createItem("DATA_SIZE", "II");
	sm.setField( 0, m_Preview.width() );
	sm.setField( 1, m_Preview.height() );

	// palette
	sm.createItem("PALETTE", "S");
	sm.setField( 0, palname );
	
	// data
	sm.createItem("DATA", "S");
	std::string& cdata = sm.setDataField(0);

	int addr = 0;
	while( addr < size ) {
		cdata += char( (data[addr] >> 4) & 15 );
		cdata += char(  data[addr++]     & 15 );
	}
	
	delete [] data;
	
	project.finishImportObjects();
	return true;
}


/*************************
 * Import Preview Widget *
 *************************/


RawImporter::RawPreviewWidget::RawPreviewWidget( RawImporter& importer )
	: m_Importer(importer), m_ImportLabel( _("<b>Import Raw Graphics</b>") ),
	  m_TypeLabel( _("Select image depth:"), 0.0, 0.5 ),
	  m_OffsetLabel( _("Offset:"), 0.0, 0.5 ),
	  m_WidthLabel( _("Width:"), 0.0, 0.5 ),
	  m_HeightLabel( _("Height:"), 0.0, 0.5 )
{
	set_spacing(5);
	set_border_width(8);
	
	m_ImportLabel.set_use_markup();
	
	pack_start( m_ImportLabel, Gtk::PACK_SHRINK );
	pack_start( m_TypeLabel, Gtk::PACK_SHRINK );
	pack_start(	m_ComboTypes, Gtk::PACK_SHRINK );
	Gtk::Table *table = manage( new Gtk::Table );
	table->attach( m_OffsetLabel, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK );
	table->attach( m_WidthLabel,  0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK );
	table->attach( m_HeightLabel, 0, 1, 2, 3, Gtk::SHRINK, Gtk::SHRINK );
	table->attach( m_OffsetSpin, 1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK );
	table->attach( m_WidthSpin,  1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK );
	table->attach( m_HeightSpin, 1, 2, 2, 3, Gtk::SHRINK, Gtk::SHRINK );
	pack_start(*table, Gtk::PACK_SHRINK);

	m_OffsetSpin.set_range(0, 1000000);
	m_OffsetSpin.set_increments(1, 16);
	m_WidthSpin.set_range(2, 4096);
	m_WidthSpin.set_increments(1, 8);
	m_HeightSpin.set_range(1, 4096);
	m_HeightSpin.set_increments(1, 8);

	show_all_children();
	
	m_ComboTypes.signal_changed().connect( sigc::mem_fun(*this, &RawImporter::RawPreviewWidget::changeTarget) );

}

void RawImporter::RawPreviewWidget::reset()
{
	m_ComboTypes.remove_all();
}

void RawImporter::RawPreviewWidget::addType( const Glib::ustring& text, bool active )
{
	m_ComboTypes.append(text);
	if( active ) m_ComboTypes.set_active( m_ComboTypes.get_model()->children().size()-1 );
}

void RawImporter::RawPreviewWidget::estimateSize( int size )
{
	
}

void RawImporter::RawPreviewWidget::changeTarget()
{
	m_Importer.updateTarget( m_ComboTypes.get_active_row_number() );
}

int RawImporter::RawPreviewWidget::targetId() const
{
	return m_ComboTypes.get_active_row_number();
}

int RawImporter::RawPreviewWidget::offset() const
{
	return m_OffsetSpin.get_value();
}

int RawImporter::RawPreviewWidget::width() const
{
	return m_WidthSpin.get_value();
}

int RawImporter::RawPreviewWidget::height() const
{
	return m_HeightSpin.get_value();
}

} // namespace Polka 
