#ifndef _POLKA_TILEEDITGRID_H_
#define _POLKA_TILEEDITGRID_H_

#include "TilePatternEditor.h"
#include "TileColorEditor.h"
#include <gtkmm/table.h>
#include <gtkmm/togglebutton.h>
#include <vector>

namespace Polka {

class Palette;

class TileEditGrid: public Gtk::Table
{
public:
	TileEditGrid();
	~TileEditGrid();

	void setGrid( int width, int height,
	              int x, int y, unsigned char *data, int stride );
	void setPalette( const Palette *palette );
	void setZoomSize( int zoom );
	void setSelectColor( int color );
	void reset();

	TilePatternEditor::SignalTileChanged signalTileChanged();
	TileColorEditor::SignalColorSelected signalColorSelected();
	
protected:

private:
	void tileChanged( int x, int y );
	void onColorToggle( int n );
	void drawPixelToAll( Gtk::Widget& w, int x, int y, bool fg );
	void colorSelected( int c );
	
	TilePatternEditor::SignalTileChanged m_SignalTileChanged;
	TileColorEditor::SignalColorSelected m_SignalColorSelected;
	
	int m_Width, m_Height, m_Zoom;
	Gtk::ToggleButton m_ColorToggles[6];
	const Palette *m_pPalette;
	std::vector<TilePatternEditor*> m_PatternEditors;
	std::vector<TileColorEditor*> m_ColorEditors;
};


} // namespace Polka

#endif // _POLKA_TILEEDITGRID_H_
