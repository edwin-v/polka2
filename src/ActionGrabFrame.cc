#include "ActionGrabFrame.h"
#include "AccelManager.h"

namespace Polka {


ActionGrabFrame::ActionGrabFrame()
{
	set_can_focus();
	add_events(Gdk::ENTER_NOTIFY_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::KEY_PRESS_MASK);
}

ActionGrabFrame::~ActionGrabFrame()
{
}

IntIntSignal ActionGrabFrame::signalButtonGrabbed()
{
	return m_SignalButtonGrabbed;
}
	
IntIntSignal ActionGrabFrame::signalKeyGrabbed()
{
	return m_SignalKeyGrabbed;
}

bool ActionGrabFrame::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	Gtk::DrawingArea::on_draw(cr);
	
	// draw the frame
	const Gtk::Allocation& a = get_allocation();
	auto sc = get_style_context();
	sc->context_save();

	// draw frame or focus
	if( has_focus() ) {
		sc->render_focus( cr, 0, 0, a.get_width(), a.get_height() );
	} else {
		sc->add_class( GTK_STYLE_CLASS_FRAME );
		sc->render_frame( cr, 0, 0, a.get_width(), a.get_height() );
	}

	sc->context_restore();
	return true;
}

bool ActionGrabFrame::on_enter_notify_event(GdkEventCrossing* event)
{
	grab_focus();
	return true;
}

bool ActionGrabFrame::on_button_press_event(GdkEventButton *event)
{
	if( event->button < DBL_CLICK ) {
		int b = event->button + (event->type == GDK_2BUTTON_PRESS ? DBL_CLICK:0);
		m_SignalButtonGrabbed.emit( b, event->state & MOD_ALL );
	}
	return true;
}

bool ActionGrabFrame::on_key_press_event (GdkEventKey* event)
{
	if( !event->is_modifier ) {
		int k = gdk_keyval_to_upper(event->keyval);
		m_SignalKeyGrabbed.emit( k, event->state & MOD_ALL );
	}
	return true;
}

} // namespace Polka
