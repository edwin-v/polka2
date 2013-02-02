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

#ifndef _POLKA_TOOLBRUSHPANEL_H_
#define _POLKA_TOOLBRUSHPANEL_H_

#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include "BrushSelector.h"

namespace Polka {

class ToolBrushPanel: public Gtk::VBox
{
public:
	ToolBrushPanel();
	~ToolBrushPanel();

	void setBrushVector( const std::vector<Brush*>& brushvec );
	BrushSelector::SignalBrushSelected signalBrushSelected();
	
	void selectBrush( int id );
	void regenerate();

private:
	Gtk::ScrolledWindow m_BrushFrame;
	BrushSelector m_BrushSelector;
};

} // namespace Polka

#endif // _POLKA_TOOLRECTPANEL_H_
