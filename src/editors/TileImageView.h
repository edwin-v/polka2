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

#ifndef _POLKA_TILEIMAGEVIEW_H_
#define _POLKA_TILEIMAGEVIEW_H_

#include <gtkmm/drawingarea.h>
#include <cairomm/surface.h>


namespace Polka {

class Palette;

class TileImageView : public Gtk::DrawingArea 
{
public:
	TileImageView();
	~TileImageView();

	void setImage( Cairo::RefPtr<Cairo::ImageSurface> image );
	void tileUpdated( int x, int y );
	void setSelector( int width, int height = 0, int horgrid = 8, int vergrid = 0 );
	void setSelection( int x, int y );
	
	// signals
	typedef sigc::signal<void, int, int> SignalSelectionChanged;
	SignalSelectionChanged signalSelectionChanged();

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event(GdkEventButton* event);
	virtual bool on_motion_notify_event(GdkEventMotion* event);
	virtual bool on_leave_notify_event(GdkEventCrossing* event);
	
private:
	SignalSelectionChanged m_SignalSelectionChanged;
	Cairo::RefPtr<Cairo::ImageSurface> m_Image;
	int m_Scale;
	int m_SelectWidth, m_SelectHeight;
	int m_GridHor, m_GridVer;
	int m_HoverX, m_HoverY;
	int m_SelectX, m_SelectY;
	
	void drawSelectRect( const Cairo::RefPtr<Cairo::Context>& context, double x, double y, double w, double h );
	void drawHover();
	void drawSelect();
	
};

} // namespace Polka

#endif // _POLKA_TILEIMAGEVIEW_H_
