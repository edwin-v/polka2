#ifndef _ACTIONGRABFRAME_H_
#define _ACTIONGRABFRAME_H_

#include <gtkmm/drawingarea.h>
#include "Defs.h"

namespace Polka {


class ActionGrabFrame: public Gtk::DrawingArea 
{
public:
	ActionGrabFrame();
	~ActionGrabFrame();

	IntIntSignal signalButtonGrabbed();
	IntIntSignal signalKeyGrabbed();
	
protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );

	virtual bool on_enter_notify_event(GdkEventCrossing* event);
	virtual bool on_button_press_event(GdkEventButton *event);
	virtual bool on_key_press_event (GdkEventKey* event);

private:
	IntIntSignal m_SignalButtonGrabbed, m_SignalKeyGrabbed;
};

} // namespace Polka

#endif // _ACTIONGRABFRAME_H_
