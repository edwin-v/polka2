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
