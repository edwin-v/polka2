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

#include "MSXBitmapExport.h"
#include "Palette.h"
#include "Bmp16Canvas.h"
#include "HIGFrame.h"
#include <gtkmm/grid.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/stock.h>
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;

namespace Polka {

class ExportOptionsDialog : public Gtk::Dialog
{
public:
	ExportOptionsDialog( Gtk::Window& parent );

	void setValues( const Canvas& canvas, int format, bool palette, int cropl, int cropt, int cropr, int cropb, int offsetx, int offsety );

	Gtk::ComboBoxText ComboSize;
	Gtk::CheckButton CheckPalette;
	Gtk::SpinButton SpinCropLeft, SpinCropRight, SpinCropTop, SpinCropBottom;
	Gtk::SpinButton SpinOffsetLeft, SpinOffsetTop;

private:
	int w, we, h, he;
};


MSXBitmapExporter::MSXBitmapExporter()
	: Exporter("MSXBMPEXP"), m_pOptionsDialog(0)
{
	// init file filters
	auto f = Gtk::FileFilter::create();
	// type 0
	f->set_name("MSX Screen 5 image");
	f->add_mime_type("image/msx-sc5");
	f->add_pattern("*.sc5");
	f->add_pattern("*.ge5");
	m_FileFilters.push_back(f);
	// type 1
	f = Gtk::FileFilter::create();
	f->set_name("MSX Screen 7 image");
	f->add_mime_type("image/msx-sc7");
	f->add_pattern("*.sc7");
	f->add_pattern("*.ge7");
	m_FileFilters.push_back(f);
}

MSXBitmapExporter::~MSXBitmapExporter()
{
}

std::vector< Glib::RefPtr<Gtk::FileFilter> > MSXBitmapExporter::fileFilters( const Object& obj ) const
{
	std::vector< Glib::RefPtr<Gtk::FileFilter> > res;

	// can export 16 color canvas objects with MSX2 palette
	if( obj.id() == "CANVAS/16/BMP" ) {
		const Canvas& c = dynamic_cast<const Canvas&>(obj);
		if( c.palette().id() == "PAL/16/MSX2" ) {
			if( c.pixelScaleHor() == 1 && c.pixelScaleVer() == 1 ) {
				// screen 5
				res.push_back(m_FileFilters[0]);
			} else if( c.pixelScaleHor() == 1 && c.pixelScaleVer() == 2 ) {
				// screen 7
				res.push_back(m_FileFilters[1]);
			}
		}
	}
	
	return res;
}	

void MSXBitmapExporter::setActiveFilter( Glib::RefPtr<Gtk::FileFilter> filter )
{
}

void MSXBitmapExporter::initObject()
{
	if( m_pObject ) {
		const Canvas& c = *dynamic_cast<const Canvas*>(m_pObject);

		// init format
		m_IncludePalette = true;
		if( c.height() <= 192 )
			m_Format = 0;
		else if( c.height() <= 212 )
			m_Format = 1;
		else
			m_Format = 2;

		// init crop
		m_CropLeft = m_CropTop = m_CropRight = m_CropBottom = 0;
		
		if( c.pixelScaleVer() == 2 ) {
			// screen 7
			if( c.width() > 512 ) m_CropRight = c.width() - 512;
		} else {
			// screen 5
			if( c.width() > 256 ) m_CropRight = c.width() - 256;
		}
		if( m_Format == 0 ) {
			if( c.height() > 192 ) m_CropBottom = c.height() - 192;
		} else if( m_Format == 1 ) {
			if( c.height() > 212 ) m_CropBottom = c.height() - 212;
		} else {
			if( c.height() > 256 ) m_CropBottom = c.height() - 256;
		}
		
		m_OffsetX = m_OffsetY = 0;
	}
}

bool MSXBitmapExporter::showExportOptions( Gtk::Window& parent )
{
	if( !m_pObject ) return false;

	const Canvas& c = *dynamic_cast<const Canvas*>(m_pObject);

	ExportOptionsDialog d(parent);

	d.setValues( c, m_Format, m_IncludePalette,
	                m_CropLeft, m_CropTop, m_CropRight, m_CropBottom,
	                m_OffsetX, m_OffsetY );
	if( d.run() == Gtk::RESPONSE_OK ) {

		m_Format = d.ComboSize.get_active_row_number();
		m_IncludePalette = d.CheckPalette.get_active();
		
		m_CropLeft = d.SpinCropLeft.get_value_as_int();
		m_CropRight = d.SpinCropRight.get_value_as_int();
		m_CropTop = d.SpinCropTop.get_value_as_int();
		m_CropBottom = d.SpinCropBottom.get_value_as_int();

		m_OffsetX = d.SpinOffsetLeft.get_value_as_int();
		m_OffsetY = d.SpinOffsetTop.get_value_as_int();
	}

	return true; // options shown
}

bool MSXBitmapExporter::exportObject( const std::string& filename )
{
	if( !m_pObject ) return false;
	const Canvas& canvas = *dynamic_cast<const Canvas*>(m_pObject);

	// open file
	std::ofstream file;
	file.open( filename, std::ofstream::binary );
	if( !file.is_open() ) return false;
	
	// calc sizes
	int w = canvas.pixelScaleVer() == 2 ? 512 : 256;
	int h = 192 + m_Format*(m_Format==1?20:32);
	int ew = canvas.width() - m_CropLeft - m_CropRight;
	int eh = canvas.height() - m_CropTop - m_CropBottom;
	int ox = m_CropLeft - m_OffsetX;
	int oy = m_CropTop - m_OffsetY;
	
	int size = w*h/2;
	if( m_Format < 2 && m_IncludePalette ) size = w==256 ? 0x7680+32 : 0xFA80+32;
	
	// write header
	file.put(0xFE);
	file.put(0);
	file.put(0);
	file.put((size-1) & 255);
	file.put((size-1) >> 8);
	file.put(0);
	file.put(0);
	
	// write data
	char c1, c;
	for( int y=0; y<h; y++ ) {
		for( int x=0; x<w; x++ ) {
			// get pixel color
			c = 0;
			if( y >= m_OffsetY && y < m_OffsetY+eh && 
			    x >= m_OffsetX && x < m_OffsetX+ew )
			{
				c = canvas.data( x + ox, y + oy );
			}
			// write or store pixel
			if( x&1 )
				file.put( 16*c1 + c );
			else
				c1 = c;
		}
	}
	
	// output palette if needed
	if( m_Format < 2 && m_IncludePalette ) {
		// pad with zeros
		for( int i = 0; i < size-32-w*h/2; i++ )
			file.put(0);
		// write palette
		const Palette& pal = canvas.palette();
		for( int c = 0; c < 16; c++ ) {
			file.put( round(pal.r(c)*7)*16 + round(pal.b(c)*7) );
			file.put( round(pal.g(c)*7) );
		}
	}
	file.close();
	
	return true;
}





ExportOptionsDialog::ExportOptionsDialog( Gtk::Window& parent )
	:  Dialog(_("Export options"), parent, true)
{
	set_border_width(12);
	get_vbox()->set_spacing(12);

	// create format frame
	HIGFrame *f = manage( new HIGFrame( _("Format") ));
	get_vbox()->pack_start(*f, Gtk::PACK_SHRINK);
	Gtk::VBox *b = manage( new Gtk::VBox );
	f->add(*b);

	ComboSize.append( _("192 lines") );
	ComboSize.append( _("212 lines") );
	ComboSize.append( _("Full page") );
	b->pack_start(ComboSize, Gtk::PACK_SHRINK);
	b->pack_start(CheckPalette, Gtk::PACK_SHRINK);
	
	CheckPalette.set_label( _("Include palette information") );
	CheckPalette.set_active();

	// create crop frame
	f = manage( new HIGFrame( _("Crop") ));
	get_vbox()->pack_start(*f, Gtk::PACK_SHRINK);

	Gtk::Grid *g = manage( new Gtk::Grid );
	g->set_column_spacing(12);
	f->add(*g);

	Gtk::Label *l = manage( new Gtk::Label( _("Left:"), 0.0, 0.5 ) );
	g->attach( *l, 0, 0, 1, 1 );
	g->attach( SpinCropLeft, 1, 0, 1, 1 );
	l = manage( new Gtk::Label( _("Top:"), 0.0, 0.5 ) );
	g->attach( *l, 2, 0, 1, 1 );
	g->attach( SpinCropTop, 3, 0, 1, 1 );
	l = manage( new Gtk::Label( _("Right:"), 0.0, 0.5 ) );
	g->attach( *l, 0, 1, 1, 1 );
	g->attach( SpinCropRight, 1, 1, 1, 1 );
	l = manage( new Gtk::Label( _("Bottom:"), 0.0, 0.5 ) );
	g->attach( *l, 2, 1, 1, 1 );
	g->attach( SpinCropBottom, 3, 1, 1, 1 );

	// create offset frame
	f = manage( new HIGFrame( _("Offset") ));
	get_vbox()->pack_start(*f, Gtk::PACK_SHRINK);

	g = manage( new Gtk::Grid );
	g->set_column_spacing(12);
	f->add(*g);

	l = manage( new Gtk::Label( _("Left:"), 0.0, 0.5 ) );
	g->attach( *l, 0, 0, 1, 1 );
	g->attach( SpinOffsetLeft, 1, 0, 1, 1 );
	l = manage( new Gtk::Label( _("Top:"), 0.0, 0.5 ) );
	g->attach( *l, 0, 1, 1, 1 );
	g->attach( SpinOffsetTop, 1, 1, 1, 1 );

	add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	add_button( Gtk::Stock::OK, Gtk::RESPONSE_OK );
	set_default_response( Gtk::RESPONSE_OK );

	show_all_children();
	
	// connect signals
	ComboSize.signal_changed().connect( [this]() {
		int s = ComboSize.get_active_row_number();
		CheckPalette.set_sensitive( s<2 );
		he = 192+s*(s==1?20:32);
		// correct for minimum crop
		if( SpinCropBottom.get_value_as_int() < h-he-SpinCropTop.get_value_as_int() )
			SpinCropBottom.set_value( h-he-SpinCropTop.get_value_as_int() );
	} );
	SpinCropLeft.signal_value_changed().connect( [this]() {
		// correct for maximum crop
		if( SpinCropRight.get_value_as_int() >= w-SpinCropLeft.get_value_as_int() )
			SpinCropRight.set_value( w-SpinCropLeft.get_value_as_int()-1 );
		// correct for minimum crop
		if( SpinCropRight.get_value_as_int() < w-we-SpinCropLeft.get_value_as_int() )
			SpinCropRight.set_value( w-we-SpinCropLeft.get_value_as_int() );
		// correct offset range
		SpinOffsetLeft.set_range( 0, we-(w-SpinCropLeft.get_value_as_int()-SpinCropRight.get_value_as_int()) );
	} );
	SpinCropRight.signal_value_changed().connect( [this]() {
		// correct for maximum crop
		if( SpinCropLeft.get_value_as_int() >= w-SpinCropRight.get_value_as_int() )
			SpinCropLeft.set_value( w-SpinCropRight.get_value_as_int()-1 );
		// correct for minimum crop
		if( SpinCropLeft.get_value_as_int() < w-we-SpinCropRight.get_value_as_int() )
			SpinCropLeft.set_value( w-we-SpinCropRight.get_value_as_int() );
		// correct offset range
		SpinOffsetLeft.set_range( 0, we-(w-SpinCropLeft.get_value_as_int()-SpinCropRight.get_value_as_int()) );
	} );
	SpinCropTop.signal_value_changed().connect( [this]() {
		// correct for maximum crop
		if( SpinCropBottom.get_value_as_int() >= h-SpinCropTop.get_value_as_int() )
			SpinCropBottom.set_value( h-SpinCropTop.get_value_as_int()-1 );
		// correct for minimum crop
		if( SpinCropBottom.get_value_as_int() < h-he-SpinCropTop.get_value_as_int() )
			SpinCropBottom.set_value( h-he-SpinCropTop.get_value_as_int() );
		// correct offset range
		SpinOffsetTop.set_range( 0, he-(h-SpinCropTop.get_value_as_int()-SpinCropBottom.get_value_as_int()) );
	} );
	SpinCropBottom.signal_value_changed().connect( [this]() {
		// correct for maximum crop
		if( SpinCropTop.get_value_as_int() >= h-SpinCropBottom.get_value_as_int() )
			SpinCropTop.set_value( h-SpinCropBottom.get_value_as_int()-1 );
		// correct for minimum crop
		if( SpinCropTop.get_value_as_int() < h-he-SpinCropBottom.get_value_as_int() )
			SpinCropTop.set_value( h-he-SpinCropBottom.get_value_as_int() );
		// correct offset range
		SpinOffsetTop.set_range( 0, he-(h-SpinCropTop.get_value_as_int()-SpinCropBottom.get_value_as_int()) );
	} );

}

void ExportOptionsDialog::setValues( const Canvas& canvas, int format, bool palette, int cropl, int cropt, int cropr, int cropb, int offsetx, int offsety )
{
	w = canvas.width();
	h = canvas.height();
	we = canvas.pixelScaleVer() == 2 ? 512 : 256;

	CheckPalette.set_active( palette );
	ComboSize.set_active( format );

	// set crop increments
	SpinCropLeft.set_increments( 1, canvas.tileGridWidth() );
	SpinCropRight.set_increments( 1, canvas.tileGridWidth() );
	SpinCropTop.set_increments( 1, canvas.tileGridHeight() );
	SpinCropBottom.set_increments( 1, canvas.tileGridHeight() );
	
	// set crop limits
	SpinCropLeft.set_range( 0, w-1 );
	SpinCropRight.set_range( 0, w-1 );
	SpinCropTop.set_range( 0, h-1 );
	SpinCropBottom.set_range( 0, h-1 );

	// set crop values
	SpinCropRight.set_value( cropr );
	SpinCropLeft.set_value( cropl );
	SpinCropBottom.set_value( cropb );
	SpinCropTop.set_value( cropt );
	
	// set offset increments
	SpinOffsetLeft.set_increments( 1, canvas.tileGridWidth() );
	SpinOffsetTop.set_increments( 1, canvas.tileGridHeight() );

	// set offset values
	SpinOffsetLeft.set_value( offsetx );
	SpinOffsetTop.set_value( offsety );
}

	

} // namespace Polka 
