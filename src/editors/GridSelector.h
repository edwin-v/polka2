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

#ifndef _POLKA_GRIDSELECTOR_H_
#define _POLKA_GRIDSELECTOR_H_

#include <gtkmm/handlebox.h>
#include <gtkmm/radiobutton.h>
#include "Defs.h"

namespace Polka {
	
class GridSelector : public Gtk::HandleBox
{
public:
	GridSelector();
	~GridSelector();

	enum Type { GRID_OFF, GRID_DOT, GRID_SHADE };

	// grid selection
	void setPixelGrid( Type grid );
	void setTileGrid( Type grid );
	Type pixelGrid() const;
	Type tileGrid() const;

	VoidSignal signalPixelGridChanged();
	VoidSignal signalTileGridChanged();

private:
	Gtk::RadioButton m_TBPixelNone, m_TBPixelDot, m_TBPixelShade,
	                  m_TBTileNone, m_TBTileDot, m_TBTileShade;

	VoidSignal m_SignalPixelGridChanged, m_SignalTileGridChanged;
	
	void onTogglePixelGrid();
	void onToggleTileGrid();
};

} // namespace Polka

#endif // _POLKA_GRIDSELECTOR_H_
