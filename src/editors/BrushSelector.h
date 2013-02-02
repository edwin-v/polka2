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

#ifndef _POLKA_BRUSHSELECTOR_H_
#define _POLKA_BRUSHSELECTOR_H_

#include <gtkmm/drawingarea.h>

namespace Polka {

class Brush;

class BrushSelector: public Gtk::DrawingArea
{
public:
	BrushSelector();
	~BrushSelector();

	void setBrushVector( const std::vector<Brush*>& brushvec );

	void selectBrush( int id );
	void regenerate();

	typedef sigc::signal<void, int> SignalBrushSelected;
	SignalBrushSelected signalBrushSelected();

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );

	virtual bool on_button_press_event(GdkEventButton *event);
	virtual bool on_button_release_event(GdkEventButton *event);
	virtual bool on_motion_notify_event(GdkEventMotion* event);

private:
	int m_SelectId, m_HoverId;

	const std::vector<Brush*> *m_pBrushes;
	std::vector<int> m_X;
	std::vector<int> m_Y;

	SignalBrushSelected m_SignalBrushSelected;
	Cairo::RefPtr<Cairo::ImageSurface> m_Image;
};

} // namespace Polka

#endif // _POLKA_BRUSHSELECTOR_H_
