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

	// default to unused colors
	m_FGColor = m_BGColor = -1;

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
		// correct colors
		int fg = m_FGColor, bg = m_BGColor;
		if( fg < 0 || fg >= pal->size() ) fg = pal->size()-1;
		if( bg < 0 || bg >= pal->size() ) bg = 0;
		// force signals
		m_FGColor = m_BGColor = -1;
		// set colors
		setFGColor(fg);
		setBGColor(bg);
	} else {
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
