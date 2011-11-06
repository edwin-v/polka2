#include "ColorChooser.h"
#include "Palette.h"
#include "Project.h"
#include "PaletteEditor.h"
#include <gtkmm/frame.h>

const int CSIZE = 16;
const int CSIZE2 = 2*CSIZE;

namespace Polka {

ColorChooser::ColorChooser()
	: m_Selector( -1, 2, false ), m_pPalette(0)
{
	Gtk::Frame *f = manage( new Gtk::Frame );

	f->add(m_Selector);
	f->set_shadow_type( Gtk::SHADOW_IN );

	pack_start( m_Preview, Gtk::PACK_EXPAND_WIDGET );
	pack_start( *f, Gtk::PACK_EXPAND_WIDGET );

	// connect signals
	m_Selector.signalColorClicked().connect( sigc::mem_fun(*this, &ColorChooser::onColorClicked ) );
	m_Selector.signalColorDoubleClicked().connect( sigc::mem_fun(*this, &ColorChooser::onDoubleClick ) );

	show_all_children();
}

ColorChooser::~ColorChooser()
{
}

	// color selection
void ColorChooser::setFGColor( int col )
{
	if( col == m_FGColor ) return;
	m_FGColor = col;
	if( m_pPalette ) {
		m_Preview.setFGColor( m_pPalette->r( col ),
		                      m_pPalette->g( col ),
		                      m_pPalette->b( col ) );
		m_SignalFGChanged.emit(col);
	}
}

void ColorChooser::setBGColor( int col )
{
	if( col == m_BGColor ) return;
	m_BGColor = col;
	if( m_pPalette ) {
		m_Preview.setBGColor( m_pPalette->r( col ),
		                      m_pPalette->g( col ),
		                      m_pPalette->b( col ) );
		m_SignalBGChanged.emit(col);
	}
}

int ColorChooser::FGColor() const
{
	return m_FGColor;
}

int ColorChooser::BGColor() const
{
	return m_BGColor;
}

IntSignal ColorChooser::signalFGChanged()
{
	return m_SignalFGChanged;
}

IntSignal ColorChooser::signalBGChanged()
{
	return m_SignalBGChanged;
}


void ColorChooser::setPalette( const Palette *pal )
{
	m_pPalette = pal;
	m_Selector.setPalette( pal );
	if( pal ) {
		//m_ColorFrame.set( 0.5, 0.5, 0.5*ceil(double(pal->size())/2), false );
		setFGColor(m_FGColor);
		setBGColor(m_BGColor);
	} else {
		//m_ColorFrame.set( 0.5, 0.5, 1.0, false );
		m_Preview.setFGColor(1,1,1);
		m_Preview.setBGColor(0,0,0);
	}
}

void ColorChooser::onColorClicked( int b )
{
	if( b == 1 )
		setFGColor( m_Selector.primaryColor() );
	else if( b == 3 )
		setBGColor( m_Selector.secondaryColor() );
}

void ColorChooser::onDoubleClick()
{
	if( m_pPalette ) {
		Palette& p = *const_cast<Palette*>(m_pPalette);
		p.project().editObject( p.name() );
		PaletteEditor *pe = dynamic_cast<PaletteEditor*>(p.editor());
		if( pe ) {
			pe->selectColor( m_Selector.primaryColor() );
		}
	}
}


} // namespace Polka 
