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

#ifndef _POLKA_TILEPATTERNEDITOR_H_
#define _POLKA_TILEPATTERNEDITOR_H_

#include <gtkmm/drawingarea.h>
#include <Types.h>

namespace Polka {

class Palette;

class TilePatternEditor: public Gtk::DrawingArea 
{
public:
	TilePatternEditor();
	~TilePatternEditor();

	void setData( int x, int y, unsigned int *data, int stride );
	void setPalette( const Palette *palette );
	void setZoomSize( int zoom );
	
	void updateColor( int x, int y, Byte color );
	void tryDrawPixel( Gtk::Widget& w, double x, double y, bool fg );
	
	// signals
	typedef sigc::signal<void, int, int> SignalTileChanged;
	SignalTileChanged signalTileChanged();
	typedef sigc::signal<void, Widget&, int, int, bool> SignalDrawPixel;
	SignalDrawPixel signalDrawPixel();

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event(GdkEventButton* event);
	virtual bool on_motion_notify_event (GdkEventMotion* event);

private:
	void updateLine( int line );
	void drawAction( int x, int y );
	
	// signal objects
	SignalTileChanged m_SignalTileChanged;
	SignalDrawPixel m_SignalDrawPixel;
	
	unsigned int *m_Data;
	int m_X, m_Y;
	const Palette *m_pPalette;
	int m_Size;
	int m_Stride;
	bool m_DrawFG;
};

} // namespace Polka

#endif // _POLKA_TILEPATTERNEDITOR_H_
