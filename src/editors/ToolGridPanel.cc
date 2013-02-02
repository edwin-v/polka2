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

#include "ToolGridPanel.h"

namespace Polka {

ToolGridPanel::ToolGridPanel()
	: m_GridStyleNone("N"), m_GridStyleLines("L"), m_GridStyleDots("D")
{
	
	m_GridStyleBox.pack_start( m_GridStyleNone );
	m_GridStyleBox.pack_start( m_GridStyleLines );
	m_GridStyleBox.pack_start( m_GridStyleDots );

	m_GridStyleNone.signal_toggled().connect( sigc::bind<int>( sigc::mem_fun(*this, &ToolGridPanel::gridHandler), 0 ) );
	m_GridStyleLines.signal_toggled().connect( sigc::bind<int>( sigc::mem_fun(*this, &ToolGridPanel::gridHandler), 1 ) );
	m_GridStyleDots.signal_toggled().connect( sigc::bind<int>( sigc::mem_fun(*this, &ToolGridPanel::gridHandler), 2 ) );
	
	pack_start( m_GridStyleBox );

	show_all_children();
}

ToolGridPanel::~ToolGridPanel()
{
}

ToolGridPanel::SignalGridTypeChanged ToolGridPanel::signalGridTypeChanged()
{
	return m_SignalGridTypeChanged;
}


void ToolGridPanel::gridHandler( int type )
{
	if( type == 0 ) {
		m_GridStyleLines.set_active(false);
		m_GridStyleDots.set_active(false);
	} if( type == 1 ) {
		m_GridStyleNone.set_active(false);
		m_GridStyleDots.set_active(false);
	} if( type == 2 ) {
		m_GridStyleNone.set_active(false);
		m_GridStyleLines.set_active(false);
	}
	m_SignalGridTypeChanged.emit(type);
}

} // namespace Polka 
