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

#ifndef _POLKA_CANVASVIEW_H_
#define _POLKA_CANVASVIEW_H_

#include <gtkmm/drawingarea.h>
#include <cairomm/surface.h>
#include "OverlayPainter.h"
#include "GridSelector.h"
#include "AccelBase.h"

namespace Polka {

class Canvas;

class CanvasView : public Gtk::DrawingArea, public AccelBase
{
public:
	CanvasView( const std::string& _id );
	~CanvasView();

	bool hasCanvas() const;
	virtual void setCanvas( Canvas *canvas );
	Canvas& canvas();
	
	// notifies partial canvas changes
	void canvasChanged( const Gdk::Rectangle& r );
	
	// display properties
	int scale() const;
	int hscale() const;
	int vscale() const;
	int dx() const;
	int dy() const;
	
	//
	GridSelector& gridSelector();

	// action/accelerator IDs
	enum { ACC_PAN = 0, ACC_END };

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_scroll_event(GdkEventScroll* event);
	virtual bool on_button_press_event(GdkEventButton *event);
	virtual bool on_button_release_event(GdkEventButton *event);
	virtual bool on_motion_notify_event(GdkEventMotion* event);
	
	void lockView( int offsetx, int offsety, int scale );
	void unlockView();
	
	void setFastUpdate( bool fast = true );
	
	virtual void changeCursor( Glib::RefPtr<Gdk::Cursor> cursor = Glib::RefPtr<Gdk::Cursor>() );
	virtual void restoreCursor();
	
private:
	Canvas *m_pCanvas;
	bool m_Dragging;
	int m_DragFromX, m_DragFromY;
	bool m_ViewLocked;
	int m_UnlockDX, m_UnlockDY, m_UnlockScale;

	OverlayPainter m_Overlay;
	GridSelector m_GridSelect;
	
	void clipDeltas( int& ox, int& oy, bool adjust_drag = false );
	void changeGrid( int id );
};

} // namespace Polka

#endif // _POLKA_CANVASVIEW_H_
