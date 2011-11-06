#include "ColorPreview.h"


namespace Polka {


ColorPreview::ColorPreview()
	: m_R(0.0), m_G(0.0), m_B(0.0)
{
	set_shadow_type(Gtk::SHADOW_OUT);
}

ColorPreview::~ColorPreview()
{
}

void ColorPreview::setColor( double r, double g, double b )
{
	m_R = r;
	m_G = g;
	m_B = b;
	queue_draw();
}


bool ColorPreview::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	const Gtk::Allocation& a = get_allocation();

	// calculate sizes
	auto border = get_style_context()->get_border();
	double wx = (double)a.get_width() - border.get_left() - border.get_right();
	double wy = (double)a.get_height() - border.get_top() - border.get_bottom();

	// paint the background
	cr->rectangle( border.get_left(), border.get_top(), wx, wy );
	cr->set_source_rgb(m_R, m_G, m_B);
	cr->fill();
	
	// draw the frame
	Frame::on_draw(cr);
	return true;
}

} // namespace Polka
