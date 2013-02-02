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

#include "ToolBrushPanel.h"
#include <glibmm/i18n.h>

namespace Polka {

ToolBrushPanel::ToolBrushPanel()
{
	pack_start( m_BrushFrame );
	m_BrushFrame.add(m_BrushSelector);
	m_BrushFrame.set_shadow_type(Gtk::SHADOW_IN);
	m_BrushFrame.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);
	m_BrushFrame.set_min_content_height(100);
	
	show_all_children();
}

ToolBrushPanel::~ToolBrushPanel()
{
}

void ToolBrushPanel::setBrushVector( const std::vector<Brush*>& brushvec )
{
	m_BrushSelector.setBrushVector(brushvec);
}

BrushSelector::SignalBrushSelected ToolBrushPanel::signalBrushSelected()
{
	return m_BrushSelector.signalBrushSelected();
}

void ToolBrushPanel::selectBrush( int id )
{
	m_BrushSelector.selectBrush(id);
}

void ToolBrushPanel::regenerate()
{
	m_BrushSelector.regenerate();
}

} // namespace Polka 
