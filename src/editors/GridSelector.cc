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

#include "GridSelector.h"
#include "ResourceManager.h"
#include <gtkmm/grid.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <glibmm/i18n.h>
#include <gtkmm/cssprovider.h>

#include <iostream>
namespace Polka {

GridSelector::GridSelector()
{
	Gtk::Grid *grid = manage( new Gtk::Grid );
	
	add( *grid );

	ResourceManager& rm = ResourceManager::get();

	// custom style for grid radiobuttons
	Glib::RefPtr<Gtk::CssProvider> rbStyle = Gtk::CssProvider::create();
	rbStyle->load_from_data("* { -GtkButton-inner-border: 0; -GtkButton-child-displacement-x: 0; -GtkButton-child-displacement-y: 0; }");
	// pixel grid
	Gtk::Label *lp = manage( new Gtk::Label );
	lp->set_markup( Glib::ustring::compose("<span size=\"smaller\">%1:</span>", _("Pixel grid") ) );
	grid->add(*lp);
	Gtk::Image *imgoff = manage( new Gtk::Image(rm.getIcon("grid_off")) );
	Gtk::Image *imgdot = manage( new Gtk::Image(rm.getIcon("grid_dot")) );
	Gtk::Image *imglin = manage( new Gtk::Image(rm.getIcon("grid_line")) );
	m_TBPixelNone.add( *imgoff );
	m_TBPixelNone.set_relief( Gtk::RELIEF_NONE );
	m_TBPixelNone.set_mode(false);
	m_TBPixelNone.get_style_context()->add_provider( rbStyle, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
	m_TBPixelDot.add( *imgdot );
	m_TBPixelDot.set_relief( Gtk::RELIEF_NONE );
	m_TBPixelDot.set_mode(false);
	m_TBPixelDot.join_group( m_TBPixelNone );
	m_TBPixelDot.get_style_context()->add_provider( rbStyle, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
	m_TBPixelShade.add( *imglin );
	m_TBPixelShade.set_relief( Gtk::RELIEF_NONE );
	m_TBPixelShade.set_mode(false);
	m_TBPixelShade.join_group( m_TBPixelNone );
	m_TBPixelShade.get_style_context()->add_provider( rbStyle, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
	Gtk::HBox *hbox = manage( new Gtk::HBox );
	hbox->pack_start(m_TBPixelNone,  Gtk::PACK_SHRINK, 0);
	hbox->pack_start(m_TBPixelDot,   Gtk::PACK_SHRINK, 0);
	hbox->pack_start(m_TBPixelShade, Gtk::PACK_SHRINK, 0);
	grid->attach_next_to(*hbox, *lp, Gtk::POS_RIGHT, 3, 1);

	// pixel grid
	Gtk::Label *lt = manage( new Gtk::Label );
	lt->set_markup( Glib::ustring::compose("<span size=\"smaller\">%1:</span>", _("Tile grid") ) );
	grid->attach_next_to(*lt, *lp, Gtk::POS_BOTTOM, 1, 1);
	imgoff = manage( new Gtk::Image(rm.getIcon("grid_off")) );
	imgdot = manage( new Gtk::Image(rm.getIcon("grid_dot")) );
	imglin = manage( new Gtk::Image(rm.getIcon("grid_line")) );
	m_TBTileNone.add( *imgoff );
	m_TBTileNone.set_relief( Gtk::RELIEF_NONE );
	m_TBTileNone.set_mode(false);
	m_TBTileNone.get_style_context()->add_provider( rbStyle, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
	m_TBTileDot.add( *imgdot );
	m_TBTileDot.set_relief( Gtk::RELIEF_NONE );
	m_TBTileDot.set_mode(false);
	m_TBTileDot.join_group( m_TBTileNone );
	m_TBTileDot.get_style_context()->add_provider( rbStyle, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
	m_TBTileShade.add( *imglin );
	m_TBTileShade.set_relief( Gtk::RELIEF_NONE );
	m_TBTileShade.set_mode(false);
	m_TBTileShade.join_group( m_TBTileNone );
	m_TBTileShade.get_style_context()->add_provider( rbStyle, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
	grid->attach_next_to(m_TBTileNone, *lt, Gtk::POS_RIGHT, 1, 1);
	grid->attach_next_to(m_TBTileDot, m_TBTileNone, Gtk::POS_RIGHT, 1, 1);
	grid->attach_next_to(m_TBTileShade, m_TBTileDot, Gtk::POS_RIGHT, 1, 1);

	// defaults
	m_TBPixelShade.set_active();
	m_TBTileShade.set_active();

	// signals
	m_TBPixelNone.signal_toggled().connect( sigc::mem_fun(*this, &GridSelector::onTogglePixelGrid) );
	m_TBPixelDot.signal_toggled().connect( sigc::mem_fun(*this, &GridSelector::onTogglePixelGrid) );
	m_TBPixelShade.signal_toggled().connect( sigc::mem_fun(*this, &GridSelector::onTogglePixelGrid) );
	                      
	m_TBTileNone.signal_toggled().connect( sigc::mem_fun(*this, &GridSelector::onToggleTileGrid) );
	m_TBTileDot.signal_toggled().connect( sigc::mem_fun(*this, &GridSelector::onToggleTileGrid) );
	m_TBTileShade.signal_toggled().connect( sigc::mem_fun(*this, &GridSelector::onToggleTileGrid) );

	show_all_children();
}

GridSelector::~GridSelector()
{
}

void GridSelector::setPixelGrid( Type grid )
{
	// set grid
	switch( grid ) {
		case GRID_DOT:
			if( !m_TBPixelDot.get_active() ) {
				m_TBPixelDot.set_active();
				m_SignalPixelGridChanged.emit();
			}
			break;
		case GRID_SHADE:
			if( !m_TBPixelShade.get_active() ) {
				m_TBPixelShade.set_active();
				m_SignalPixelGridChanged.emit();
			}
			break;
		default:
			if( !m_TBPixelNone.get_active() ) {
				m_TBPixelNone.set_active();
				m_SignalPixelGridChanged.emit();
			}
			break;
	}
}

void GridSelector::onTogglePixelGrid()
{
	m_SignalPixelGridChanged.emit();
}

void GridSelector::setTileGrid( Type grid )
{
}

void GridSelector::onToggleTileGrid()
{
	m_SignalTileGridChanged.emit();
}

GridSelector::Type GridSelector::pixelGrid() const
{
	if( m_TBPixelDot.get_active() ) {
		return GRID_DOT;
	} else if( m_TBPixelShade.get_active() ) {
		return GRID_SHADE;
	} else {
		return GRID_OFF;
	}
}

GridSelector::Type GridSelector::tileGrid() const
{
	if( m_TBTileDot.get_active() ) {
		return GRID_DOT;
	} else if( m_TBTileShade.get_active() ) {
		return GRID_SHADE;
	} else {
		return GRID_OFF;
	}
}

VoidSignal GridSelector::signalPixelGridChanged()
{
	return m_SignalPixelGridChanged;
}

VoidSignal GridSelector::signalTileGridChanged()
{
	return m_SignalTileGridChanged;
}

}

