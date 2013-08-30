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

#ifndef _POLKA_COLORSELECTOR_H_
#define _POLKA_COLORSELECTOR_H_

#include "ShapeDrawingArea.h"
#include "ShapeDrawingObjects.h"
#include "Palette.h"
#include "Defs.h"

namespace Polka {
	
class Palette;

/*
 * ColorSelector widget
 * 
 * This widget displays the colors from a palette object in a grid.
 * If either width or height is smaller than one, then this value will
 * be calculated. If both values are smaller than one then a width and
 * height will be calculated that results in a color rectangle closest
 * to square.
 * 
 * Click signals are always sent, but the construction parameter can
 * be used to decide whether selections should be shown.
 */

class ColorSelector : public ShapeDrawingArea 
{
public:
	ColorSelector( int width = -1, int height = -1, bool display_selection = true );
	~ColorSelector();

	// list control
	void setSize( int width, int height );
	void setPalette( const Palette *palette );
	void reset();

	void setSelection( int pri, int sec );
	int primaryColor() const;
	int secondaryColor() const;
	int hoverColor() const;

	IntSignal signalColorClicked();
	VoidSignal signalColorDoubleClicked();

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event(GdkEventButton* event);
	virtual bool on_button_release_event(GdkEventButton *event);
	virtual bool on_motion_notify_event(GdkEventMotion* event);
	virtual bool on_leave_notify_event(GdkEventCrossing* event);

	virtual Gtk::SizeRequestMode get_request_mode_vfunc() const;
	virtual void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const;
	virtual void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const;
	virtual void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const;
	virtual void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const;
	
private:
	// selector appearance
	const Palette *m_pPalette;
	Cairo::RefPtr<RectangleShape> m_rCursor, m_rSecondaryMarker;
	int m_Width, m_Height, m_ReqWidth, m_ReqHeight;
	bool m_Selection;
	bool m_Dragging;
	int m_PriColor, m_SecColor, m_HovColor;

	IntSignal m_SignalColorClicked;
	VoidSignal m_SignalColorDoubleClicked;
	
	void calcLayout();
	int calcColor( int x, int y );
};


} // namespace Polka

#endif // _POLKA_COLORSELECTOR_H_
