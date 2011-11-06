#ifndef _POLKA_COLORSLIDER_H_
#define _POLKA_COLORSLIDER_H_

#include <gtkmm/drawingarea.h>
#include <gtkmm/adjustment.h>

namespace Polka {

class ColorSlider : public Gtk::DrawingArea
{
public:
	ColorSlider( Gtk::Orientation orient = Gtk::ORIENTATION_HORIZONTAL );
	~ColorSlider();

	Glib::RefPtr<Gtk::Adjustment> getAdjustment();

	// access
	int range() const;
	int value() const;

	// appearance control
	void setRange( int upper );
	void setLowColor( double r, double g, double b );
	void setHighColor( double r, double g, double b );

	// signals
	typedef sigc::signal<void> SignalChanged;
	typedef sigc::signal<void> SignalChanging;
	SignalChanged signalChanged();
	SignalChanging signalChanging();
	
protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event(GdkEventButton* event);
	virtual bool on_button_release_event(GdkEventButton* event);
	virtual bool on_motion_notify_event(GdkEventMotion *event);

private:
	void onChange();

	void changePosition( int x, int y );
	
	// signal objects
	SignalChanged m_SignalChanged;
	SignalChanging m_SignalChanging;
	// slider appearance
	Gtk::Orientation m_Orientation;
	Glib::RefPtr<Gtk::Adjustment> m_refAdj;
	double m_R0, m_G0, m_B0;
	double m_R1, m_G1, m_B1;
	int m_StartX, m_StartY, m_Delta;
	bool m_Changing, m_Changed;
	int m_ChangeStart;
};

} // namespace Polka

#endif // _POLKA_COLORSLIDER_H_
