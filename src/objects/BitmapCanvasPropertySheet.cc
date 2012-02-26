#include "BitmapCanvasPropertySheet.h"
#include "Canvas.h"
#include "Project.h"
#include "Palette.h"
#include "HIGFrame.h"
#include "Functions.h"
#include "ResourceManager.h"
#include <gtkmm/label.h>
#include <gtkmm/table.h>
#include <gtkmm/alignment.h>
#include <glibmm/i18n.h>
#include <iostream>

namespace Polka {

struct Template {
	const char *name;
	int width;
	int height;
	int hpixscale;
	int vpixscale;
};

static const Template Templates[] = {
	{ "128x128 Screen 5", 128, 128, 1, 1 },
	{ "256x212 Screen 5", 256, 212, 1, 1 },
	{ "256x256 Screen 5", 256, 256, 1, 1 },
	{ "256x128 Screen 7", 256, 128, 1, 2 },
	{ "512x212 Screen 7", 512, 212, 1, 2 },
	{ "512x256 Screen 7", 512, 256, 1, 2 },
};
static const int NumTemplates = sizeof(Templates)/sizeof(Template);

BitmapCanvasPropertySheet::BitmapCanvasPropertySheet( Canvas& canvas )
	: m_Canvas( canvas ), m_Square( _("Square") ), m_High( _("High 1:2") ),
	  m_Wide( _("Wide 2:1") )
{
	set_border_width(12);
	set_spacing(12);
	// templates
	Gtk::Box *box = manage( new Gtk::HBox );
	Gtk::Label *l = manage( new Gtk::Label( _("Template:") ) );
	box->set_spacing(12);
	box->pack_start( *l, Gtk::PACK_SHRINK );
	box->pack_start( m_Templates );
	pack_start(*box, Gtk::PACK_SHRINK);
	// create size frame
	HIGFrame *f = manage( new HIGFrame( _("Canvas Size") ));
	pack_start(*f);
	// create size editor
	Gtk::Table *t = manage( new Gtk::Table(2, 2) );
	t->set_col_spacings(12);
	f->add(*t);
	
	l = manage( new Gtk::Label( _("Width:") ) );
	t->attach( *l, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 0, 4 );
	l = manage( new Gtk::Label( _("Height:") ) );
	t->attach( *l, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 4 );
	t->attach( m_Width,  1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 0, 4 );
	t->attach( m_Height, 1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 4 );

	// create aspect frame
	f = manage( new HIGFrame( _("Pixel ratio") ));
	pack_start(*f, Gtk::PACK_SHRINK);

	box  = manage( new Gtk::HBox );
	f->add(*box);
	box->pack_start(m_Square);
	box->pack_start(m_High);
	box->pack_start(m_Wide);

	// create palette frame
	f = manage( new HIGFrame( _("Palette:") ));
	pack_start(*f, Gtk::PACK_SHRINK);
	f->add( m_Palettes );
	
	m_Square.set_active();
	Gtk::RadioButtonGroup g = m_Square.get_group();
	m_High.set_group( g );
	m_Wide.set_group( g );
	m_Width.set_range( 64, 4096 );
	m_Width.set_increments( 1, 64 );
	m_Height.set_range( 64, 4096 );
	m_Height.set_increments( 1, 64 );
	
	fillTemplates();
	fillPalettes();
	initObject();
	
	m_Templates.signal_changed().connect( sigc::mem_fun(*this, &BitmapCanvasPropertySheet::useTemplate) );
	m_Width.signal_changed().connect( sigc::mem_fun(*this, &BitmapCanvasPropertySheet::checkChanges) );
	m_Height.signal_changed().connect( sigc::mem_fun(*this, &BitmapCanvasPropertySheet::checkChanges) );
	m_Square.signal_toggled().connect( sigc::mem_fun(*this, &BitmapCanvasPropertySheet::checkChanges) );
	m_High.signal_toggled().connect( sigc::mem_fun(*this, &BitmapCanvasPropertySheet::checkChanges) );
	m_Palettes.signal_changed().connect( sigc::mem_fun(*this, &BitmapCanvasPropertySheet::checkChanges) );
	
	show_all_children();
}

BitmapCanvasPropertySheet::~BitmapCanvasPropertySheet()
{
}

void BitmapCanvasPropertySheet::fillTemplates()
{
	for( int i = 0; i < NumTemplates; i++ ) {
		m_Templates.append( Templates[i].name );
		if( m_Canvas.width() == Templates[i].width &&
		    m_Canvas.height() == Templates[i].height &&
		    m_Canvas.pixelScaleHor() == Templates[i].hpixscale &&
		    m_Canvas.pixelScaleVer() == Templates[i].vpixscale ) m_Templates.set_active(i);
	}
}

void BitmapCanvasPropertySheet::fillPalettes()
{
	std::vector<std::string> types = split( m_Canvas.dependencyType(DEP_PAL), ',' );
	for(unsigned int i = 0; i < types.size(); i++ ) {
		std::vector<Polka::Object*> objs;
		m_Canvas.project().findAllObjectsOfType( types[i], objs );
		for(unsigned int j = 0; j < objs.size(); j++ ) {
			if( dynamic_cast<Palette*>(objs[j]) ) {
				m_Palettes.append( objs[j]->name() );
				if( m_Canvas.palette().name() == objs[j]->name() )
					m_Palettes.set_active( m_Palettes.get_model()->children().size()-1 );
			}
		}
	}
}

void BitmapCanvasPropertySheet::useTemplate()
{
	int i = m_Templates.get_active_row_number();
	if( i >= 0 ) {
		m_Width.set_value(Templates[i].width);
		m_Height.set_value(Templates[i].height);
		if( Templates[i].vpixscale > 1 )
			m_High.set_active();
		else if( Templates[i].hpixscale > 1 )
			m_Wide.set_active();
		else
			m_Square.set_active();
	}
}

void BitmapCanvasPropertySheet::initObject()
{
	m_Width.set_value( m_Canvas.width() );
	m_Height.set_value( m_Canvas.height() );
	if( m_Canvas.pixelScaleHor() > 1 )
		m_Wide.set_active();
	else if( m_Canvas.pixelScaleVer() > 1 )
		m_High.set_active();
	else
		m_Square.set_active();
}

void BitmapCanvasPropertySheet::checkChanges()
{
	bool changed = false;
	// check resolution
	changed |= (m_Width.get_value_as_int() != m_Canvas.width() );
	changed |= (m_Height.get_value_as_int() != m_Canvas.height() );
	// check aspect
	if( m_Square.get_active() ) {
		changed |= ( m_Canvas.pixelScaleHor() != m_Canvas.pixelScaleVer() );
	} else if( m_High.get_active() ) {
		changed |= ( m_Canvas.pixelScaleHor() >= m_Canvas.pixelScaleVer() );
	} else {
		changed |= ( m_Canvas.pixelScaleHor() <= m_Canvas.pixelScaleVer() );
	}
	// check palette
	changed |= (m_Canvas.palette().name() != m_Palettes.get_active_text() );
	
	setModified( changed );
	
	// find matching template
	for( int i = 0; i < NumTemplates; i++ ) { std::cout << "T" << i << "   " << m_Square.get_active() << "|" << m_High.get_active() << "|" << m_Wide.get_active() << "   " << Templates[i].hpixscale << "|" << Templates[i].vpixscale << std::endl;
		// check ratio first
		if( Templates[i].hpixscale == Templates[i].vpixscale ) {
			if( !m_Square.get_active() ) continue;
		} else if( Templates[i].hpixscale < Templates[i].vpixscale ) {
			if( !m_High.get_active() ) continue;
		} else {
			if( !m_Wide.get_active() ) continue;
		}
		// check resolution
		if( m_Width.get_value_as_int() == Templates[i].width &&
		    m_Height.get_value_as_int() == Templates[i].height )
		{ 
			m_Templates.set_active(i);
			return;
		}
	}
	// unselect if no match
	m_Templates.set_active(-1);
}

void BitmapCanvasPropertySheet::apply() const
{
	int xs = 1, ys = 1;
	if( m_High.get_active() ) 
		ys = 2; 
	else if( m_Wide.get_active() ) 
		xs = 2; 
	
	m_Canvas.resize( m_Width.get_value_as_int(), m_Height.get_value_as_int(), xs, ys, true );
	Palette *pal = dynamic_cast<Palette*>( m_Canvas.project().findObject( m_Palettes.get_active_text() ) );
	m_Canvas.setPalette(*pal);
	
}

void BitmapCanvasPropertySheet::reset() const
{
	// no action needed
}

} // namespace Polka 
