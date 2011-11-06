#ifndef _COLORPREVIEW_H_
#define _COLORPREVIEW_H_

#include <gtkmm/frame.h>

namespace Polka {


class ColorPreview: public Gtk::Frame 
{
public:
	ColorPreview();
	~ColorPreview();

	void setColor( double r, double g, double b );

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );

private:
	double m_R, m_G, m_B;

};

} // namespace Polka

#endif // _COLORPREVIEW_H_
