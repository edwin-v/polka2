#include "SelectedColorPreview.h"


namespace Polka {


SelectedColorPreview::SelectedColorPreview( int margin )
	: m_Margin(margin)
{
	m_FR = m_FG = m_FB = 1.0;
	m_BR = m_BG = m_BB = 0.0;
}

SelectedColorPreview::~SelectedColorPreview()
{
}

void SelectedColorPreview::setMargin( int margin )
{
	m_Margin = margin;
	queue_draw();
}

void SelectedColorPreview::setFGColor( double r, double g, double b )
{
	m_FR = r;
	m_FG = g;
	m_FB = b;
	queue_draw();
}

void SelectedColorPreview::setBGColor( double r, double g, double b )
{
	m_BR = r;
	m_BG = g;
	m_BB = b;
	queue_draw();
}

bool SelectedColorPreview::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// calculate sizes
	const Gtk::Allocation& a = get_allocation();
	int w = (a.get_width()  - 2*m_Margin)*2/3;
	int h = (a.get_height() - 2*m_Margin)*2/3;

	// paint the areas
	cr->rectangle( m_Margin+w/2, m_Margin+h/2, w, h );
	cr->set_source_rgb(m_BR, m_BG, m_BB);
	cr->fill_preserve();
	// edge
	cr->set_source_rgba( 0, 0, 0, 0.1 );
	cr->set_line_width(1);
	cr->stroke();

	cr->rectangle( m_Margin, m_Margin, w, h );
	cr->set_source_rgb(m_FR, m_FG, m_FB);
	cr->fill_preserve();
	// light edge
	cr->set_source_rgba( 1, 1, 1, 0.5 );
	cr->stroke();

	return true;
}

Gtk::SizeRequestMode SelectedColorPreview::get_request_mode_vfunc() const
{
	return Gtk::SIZE_REQUEST_WIDTH_FOR_HEIGHT;
}

void SelectedColorPreview::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
	minimum_width = 2*m_Margin + 8;
	natural_width = 2*m_Margin + 16;
}

void SelectedColorPreview::get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const
{
	minimum_height = 2*m_Margin + 8;
	natural_height = width;
}

void SelectedColorPreview::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
	minimum_height = 2*m_Margin + 8;
	natural_height = 2*m_Margin + 16;
}

void SelectedColorPreview::get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const
{
	minimum_width = 2*m_Margin + 8;
	natural_width = height;
}


} // namespace Polka
