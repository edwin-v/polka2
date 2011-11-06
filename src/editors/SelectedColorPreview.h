#ifndef _SELECTEDCOLORPREVIEW_H_
#define _SELECTEDCOLORPREVIEW_H_

#include <gtkmm/drawingarea.h>

namespace Polka {


class SelectedColorPreview: public Gtk::DrawingArea 
{
public:
	SelectedColorPreview( int margin = 4 );
	~SelectedColorPreview();

	void setMargin( int margin );
	void setFGColor( double r, double g, double b );
	void setBGColor( double r, double g, double b );

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );

	// widget sizing overrides
	virtual Gtk::SizeRequestMode get_request_mode_vfunc() const;
	virtual void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const;
	virtual void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const;
	virtual void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const;
	virtual void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const;
	
private:
	int m_Margin;
	double m_FR, m_FG, m_FB;
	double m_BR, m_BG, m_BB;
};

} // namespace Polka

#endif // _SELECTEDCOLORPREVIEW_H_
