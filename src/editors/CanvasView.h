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

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_scroll_event(GdkEventScroll* event);
	virtual bool on_button_press_event(GdkEventButton *event);
	virtual bool on_button_release_event(GdkEventButton *event);
	virtual bool on_motion_notify_event(GdkEventMotion* event);
	
	void lockView( int offsetx, int offsety, int scale );
	void unlockView();
	
	void setFastUpdate( bool fast = true );
	
	void changeCursor( Glib::RefPtr<Gdk::Cursor> cursor = Glib::RefPtr<Gdk::Cursor>() );
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
