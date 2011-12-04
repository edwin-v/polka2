#include "ImageImport.h"
#include "Palette.h"
#include "Bmp16Canvas.h"
#include "Project.h"
#include "Functions.h"
#include "Storage.h"
#include "StorageHelpers.h"
#include <gtkmm/table.h>
#include <cairomm/surface.h>
#include <iostream>
#include <cassert>

using namespace std;

namespace Polka {

const char PNG_HEADER[8] = { 137-256, 80, 78, 71, 13, 10, 26, 10 };

ImageImporter::ImageImporter()
	: Importer("PNGIMP"), m_Preview(*this)
{
}

ImageImporter::~ImageImporter()
{
}

Gtk::Widget& ImageImporter::getPreviewWidget()
{
	return m_Preview;
}

bool ImageImporter::initImport( const std::string& filename )
{
	cleanUp();

	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate );
	if( !file.is_open() ) return false;
	m_FileName = filename;	
	
	// get file size
	m_FileSize = file.tellg();
	file.seekg(0);

	// check PNG header
	if( m_FileSize < 8 ) return false;
	char header[8];
	file.read( header, 8 );
	file.close();
	for( int i = 0; i < 8; i++ )
		if( header[i] != PNG_HEADER[i] ) return false;

	if( !tryLoadPNG() ) return false;
	
	return true;
}

void ImageImporter::cancelImport()
{
	cleanUp();
}

void ImageImporter::cleanUp()
{
	m_Preview.reset();
}

bool ImageImporter::tryLoadPNG()
{
	// open file with cairo
	m_refImage = Cairo::ImageSurface::create_from_png(m_FileName);
	// check validity
	if( !m_refImage ) return false;
	if( m_refImage->get_width() <= 0 || m_refImage->get_height() <= 0 ) return false;
	
	Glib::ustring coltext;
	switch( m_refImage->get_format() ) {
		case Cairo::FORMAT_ARGB32:
		case Cairo::FORMAT_RGB24:
			coltext = _("true color");
			break;
		case Cairo::FORMAT_RGB16_565:
			coltext = _("65536 color");
			break;
		default:
			// unsuitable type
			return false;
	}
	// image ok, init preview
	m_Preview.setImage( _("PNG"), m_refImage->get_width(), m_refImage->get_height(), coltext );
	return true;
}

bool ImageImporter::importToProject( Project& project )
{
	// convenience vars
	int w = m_refImage->get_width(), h = m_refImage->get_height();

	// Create color reducer
	ColorReducer cr;
	cr.setRGBSource( m_refImage->get_data(), w, h, 4, m_refImage->get_stride() );

	// fetch settings from widget and init color reducer
	std::string palid = m_Preview.getPaletteType();
	unsigned char *data = new unsigned char[w*h];
	cr.setTarget( palid, data );
	// quantization
	if( cr.needQuantization(palid) ) {
		cr.setQuantizationMethod( m_Preview.getQuantMethod(), m_Preview.getErrorColorSpace() );
	}
	// dither
	cr.setDitherType( m_Preview.getDitherType(),
	                  m_Preview.getOrderedDitherWidth(),
	                  m_Preview.getOrderedDitherHeight(),
	                  m_Preview.getOrderedDitherOffsetX(),
	                  m_Preview.getOrderedDitherOffsetY() );

	// perform quantization and dither
	cr.quantizeColors();
	cr.generateImage();

	// create import storage for objects
	Glib::ustring palname, name = getNameFromFilename(m_FileName);
	Storage& impS = project.createImportObjects(name);

	// create a new palette for this
	Storage& s = impS.createObject(palid);
	palname = project.createUniqueName( name + _(" Palette") );
	storageSetObjectName( s, palname );
	s.createItem( "RGB", "[FFF]" );
	for( int c = 0; c < cr.palSize(); c++ ) {
		s.setField( c, 0, cr.palRed(c) );
		s.setField( c, 1, cr.palGreen(c) );
		s.setField( c, 2, cr.palBlue(c) );
	}
	
	// create canvas
	// calculate the number of lines
	Storage& s2 = impS.createObject( cr.getTargetCanvas(palid) );
	std::string scname = project.createUniqueName( name + _(" Canvas") );
	storageSetObjectName( s2, scname );

	// size
	Storage& sm = s2.createObject("DATA_MAIN");
	sm.createItem("DATA_SIZE", "II");
	sm.setField( 0, w );
	sm.setField( 1, h );

	// palette
	sm.createItem("PALETTE", "S");
	sm.setField( 0, palname );
	
	// data
	sm.createItem("DATA", "S");
	std::string& cdata = sm.setDataField(0);
	cdata.assign( (char*)data, w*h );
	
	delete [] data;
	
	project.finishImportObjects();
	return true;
}


/*************************
 * Import Preview Widget *
 *************************/


ImageImporter::ImagePreviewWidget::ImagePreviewWidget( ImageImporter& importer )
	: m_Importer(importer), 
	  m_PaletteLabel( _("Color palette:"), 0.0, 0.5 ),
	  m_QuantLabel( _("Quantization method:"), 0.0, 0.5 ),
	  m_DitherLabel( _("Dithering:"), 0.0, 0.5 ),
	  m_DitherErrorLabel( _("Method:"), 0.0, 0.5 ),
	  m_DitherOrderedLabel( _("Width and height:"), 0.5, 0.5 ),
	  m_DitherOrderedOffsetLabel( _("Offset:"), 0.5, 0.5 ),
	  m_ColorSpaceLabel( _("Calculate errors in:"), 0.0, 0.5 ),
	  m_OptionsFrame( _("Conversion options") ),
	  m_RGBRadio( _("RGB colorspace") ),
	  m_PerceptualRadio( _("Perceptual colorspace") )
{
	set_spacing(5);
	set_border_width(8);

	pack_start( m_ImportLabel, Gtk::PACK_SHRINK );
	pack_start( m_ImageLabel, Gtk::PACK_SHRINK );
	pack_start( m_OptionsFrame, Gtk::PACK_SHRINK );
	
	m_OptionsFrame.add( m_OptionsGrid );
	m_OptionsGrid.attach( m_PaletteLabel, 0, 0, 1, 1 );
	m_OptionsGrid.attach( m_PaletteCombo, 0, 1, 1, 1 );
	m_OptionsGrid.attach( m_QuantLabel, 0, 2, 1, 1 );
	m_OptionsGrid.attach( m_QuantCombo, 0, 3, 1, 1 );
	m_OptionsGrid.attach( m_ColorSpaceLabel, 0, 4, 1, 1 );
	m_OptionsGrid.attach( m_RGBRadio, 0, 5, 2, 1 );
	m_OptionsGrid.attach( m_PerceptualRadio, 0, 6, 1, 1 );
	m_OptionsGrid.attach( m_DitherLabel, 0, 7, 1, 1 );
	m_OptionsGrid.attach( m_DitherCombo, 0, 8, 1, 1 );
	m_OptionsGrid.attach( m_DitherMethodSelect, 0, 9, 1, 1 );

	m_ErrorDiffOptions.attach( m_DitherErrorLabel, 0, 0, 1, 1 );
	m_ErrorDiffOptions.attach( m_DitherErrorCombo, 0, 1, 1, 1 );
	m_OrderedOptions.attach( m_DitherOrderedLabel, 0, 0, 2, 1 );
	m_OrderedOptions.attach( m_OrderedWidthSpin, 0, 1, 1, 1 );
	m_OrderedOptions.attach( m_OrderedHeightSpin, 1, 1, 1, 1 );
	m_OrderedOptions.attach( m_DitherOrderedOffsetLabel, 0, 2, 2, 1 );
	m_OrderedOptions.attach( m_OrderedOffsetXSpin, 0, 3, 1, 1 );
	m_OrderedOptions.attach( m_OrderedOffsetYSpin, 1, 3, 1, 1 );

	m_DitherMethodSelect.append_page( *manage( new Gtk::VBox ) );
	m_DitherMethodSelect.append_page( m_ErrorDiffOptions );
	m_DitherMethodSelect.append_page( m_OrderedOptions );
	m_DitherMethodSelect.set_show_border(false);
	m_DitherMethodSelect.set_show_tabs(false);

	m_OrderedWidthSpin.set_name("width");
	m_OrderedWidthSpin.set_numeric();
	m_OrderedWidthSpin.set_editable(false);
	m_OrderedWidthSpin.set_range( 1, 5 );
	m_OrderedWidthSpin.set_increments( 1, 1 );

	m_OrderedHeightSpin.set_numeric();
	m_OrderedHeightSpin.set_editable(false);
	m_OrderedHeightSpin.set_range( 1, 5 );
	m_OrderedHeightSpin.set_increments( 1, 1 );

	m_OrderedOffsetXSpin.set_numeric();
	m_OrderedOffsetXSpin.set_editable(false);
	m_OrderedOffsetXSpin.set_increments( 1, 1 );

	m_OrderedOffsetYSpin.set_numeric();
	m_OrderedOffsetYSpin.set_editable(false);
	m_OrderedOffsetYSpin.set_increments( 1, 1 );

	// add some spacing
	m_PaletteLabel.set_margin_top(4);
	m_QuantLabel.set_margin_top(4);
	m_ColorSpaceLabel.set_margin_top(2);
	m_ColorSpaceLabel.set_margin_left(2);
	m_RGBRadio.set_margin_left(4);
	m_PerceptualRadio.set_margin_left(4);
	m_DitherLabel.set_margin_top(4);
	m_DitherMethodSelect.set_border_width(8);
	m_OrderedOptions.set_column_spacing(4);
	
	// init combos
	std::vector<std::string> vec;
	// palettes
	ObjectManager& om = ObjectManager::instance();
	ColorReducer c;//TEMP
	c.getTargetPalettes( vec );
	for( unsigned int i = 0; i < vec.size(); i++ )
		m_PaletteCombo.append( vec[i], om.nameFromId( vec[i] ) );
	// quant methods
	m_QuantCombo.append( Glib::ustring::compose("%1", ColorReducer::QUANT_OCTREE)          , _("Octree") );
	m_QuantCombo.append( Glib::ustring::compose("%1", ColorReducer::QUANT_ERRORELIMINATION), _("Error Elimination") );
	m_QuantCombo.append( Glib::ustring::compose("%1", ColorReducer::QUANT_ELIMINATION)     , _("Elimination") );
	m_QuantCombo.append( Glib::ustring::compose("%1", ColorReducer::QUANT_POPULARITY)      , _("Popularity") );
	m_QuantCombo.set_active(0);
	// dither groups
	m_DitherCombo.append( _("None") );
	m_DitherCombo.append( _("Error diffusion") );
	m_DitherCombo.append( _("Ordered") );
	// error diffusions
	m_DitherErrorCombo.append( Glib::ustring::compose("%1", ColorReducer::DITHER_FLOYDSTEINBERG)   , _("Floyd-Steinberg") );
	m_DitherErrorCombo.append( Glib::ustring::compose("%1", ColorReducer::DITHER_JARVISJUDICENINKE), _("Jarvis-Judice-Ninke") );
	m_DitherErrorCombo.append( Glib::ustring::compose("%1", ColorReducer::DITHER_STEVENSONARCE)    , _("Stevenson-Arce") );
	m_DitherErrorCombo.append( Glib::ustring::compose("%1", ColorReducer::DITHER_STUCKI)           , _("Stucki") );
	m_DitherErrorCombo.append( Glib::ustring::compose("%1", ColorReducer::DITHER_SIERRA3)          , _("Sierra 3") );
	m_DitherErrorCombo.append( Glib::ustring::compose("%1", ColorReducer::DITHER_SIERRA2)          , _("Sierra 2") );
	m_DitherErrorCombo.append( Glib::ustring::compose("%1", ColorReducer::DITHER_SIERRA2_4A)       , _("Sierra 2-4a") );
	m_DitherErrorCombo.append( Glib::ustring::compose("%1", ColorReducer::DITHER_BURKES)           , _("Burkes") );
	m_DitherErrorCombo.append( Glib::ustring::compose("%1", ColorReducer::DITHER_ATKINSON)         , _("Atkinson") );
	m_DitherErrorCombo.set_active(0);
	// colorspaces
	m_PerceptualRadio.join_group( m_RGBRadio );
	m_RGBRadio.set_active();
	
	reset();

	m_PaletteCombo.signal_changed().connect( sigc::mem_fun(*this, &ImageImporter::ImagePreviewWidget::paletteChanged) );
	m_DitherCombo.signal_changed().connect( sigc::mem_fun(*this, &ImageImporter::ImagePreviewWidget::ditherGroupChanged) );
	m_OrderedWidthSpin.signal_output().connect( sigc::bind<Gtk::SpinButton&>( sigc::mem_fun(*this, &ImageImporter::ImagePreviewWidget::orderedSizeChanged), m_OrderedWidthSpin ) );
	m_OrderedHeightSpin.signal_output().connect( sigc::bind<Gtk::SpinButton&>( sigc::mem_fun(*this, &ImageImporter::ImagePreviewWidget::orderedSizeChanged), m_OrderedHeightSpin ) );

	// update status (with signal)
	m_DitherCombo.set_active(0);
	m_OrderedWidthSpin.set_value(2);
	m_OrderedHeightSpin.set_value(2);

	show_all_children();

}

void ImageImporter::ImagePreviewWidget::reset()
{
	m_PaletteCombo.set_active(0);
}

void ImageImporter::ImagePreviewWidget::setImage( const Glib::ustring& type, int hres, int vres, const Glib::ustring& colors )
{
	m_ImportLabel.set_text( Glib::ustring::compose( _("<b>Import %1 Graphics</b>"), type ) );
	m_ImageLabel.set_text( Glib::ustring::compose( _("Source is a %3 image with\na resolution of %1x%2."), hres, vres, colors ) );
	m_ImportLabel.set_use_markup();
}

void ImageImporter::ImagePreviewWidget::paletteChanged()
{
	// quantization needed
	bool need_quant = ColorReducer::needQuantization( m_PaletteCombo.get_active_id().raw() );
	m_QuantLabel.set_sensitive( need_quant );
	m_QuantCombo.set_sensitive( need_quant );
	m_ColorSpaceLabel.set_sensitive( need_quant );
	m_RGBRadio.set_sensitive( need_quant );
	m_PerceptualRadio.set_sensitive( need_quant );
}

void ImageImporter::ImagePreviewWidget::ditherGroupChanged()
{
	m_DitherMethodSelect.set_current_page( m_DitherCombo.get_active_row_number() );
}

bool ImageImporter::ImagePreviewWidget::orderedSizeChanged( Gtk::SpinButton& sb )
{
	// convert exponent to readable value
	int v = 1 << sb.get_value_as_int();
	sb.set_text( Glib::ustring::compose("%1", v) );
	
	if( sb.get_name() == "width" )
		m_OrderedOffsetXSpin.set_range( 0, v-1 );
	else
		m_OrderedOffsetYSpin.set_range( 0, v-1 );
	return true;
}

/* access members */

std::string ImageImporter::ImagePreviewWidget::getPaletteType() const
{
	return m_PaletteCombo.get_active_id().raw();
}

ColorReducer::QuantizationMethod ImageImporter::ImagePreviewWidget::getQuantMethod() const
{
	std::stringstream ss;
	int res;
	ss << m_QuantCombo.get_active_id().raw();
	ss >> res;
	return ColorReducer::QuantizationMethod(res);
}

ColorReducer::ColorErrorMethod ImageImporter::ImagePreviewWidget::getErrorColorSpace() const
{
	if( m_PerceptualRadio.get_active() )
		return ColorReducer::COLORERROR_PERCEPTUAL;
	else
		return ColorReducer::COLORERROR_RGB;
}

ColorReducer::DitherType ImageImporter::ImagePreviewWidget::getDitherType() const
{
	
	switch( m_DitherCombo.get_active_row_number() )
	{
		case 1: // error diffusion
		{
			std::stringstream ss;
			int res;
			ss << m_DitherErrorCombo.get_active_id().raw();
			ss >> res;
			return ColorReducer::DitherType(res);
		}
		case 2: // ordered dither
			return ColorReducer::DITHER_ORDERED;
		default:
			return ColorReducer::DITHER_NONE;
	}
}

int ImageImporter::ImagePreviewWidget::getOrderedDitherWidth() const
{
	return m_OrderedWidthSpin.get_value_as_int();
}

int ImageImporter::ImagePreviewWidget::getOrderedDitherHeight() const
{
	return m_OrderedHeightSpin.get_value_as_int();
}

int ImageImporter::ImagePreviewWidget::getOrderedDitherOffsetX() const
{
	return m_OrderedOffsetXSpin.get_value_as_int();
}

int ImageImporter::ImagePreviewWidget::getOrderedDitherOffsetY() const
{
	return m_OrderedOffsetYSpin.get_value_as_int();
}


} // namespace Polka 
