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
