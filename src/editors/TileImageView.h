#ifndef _POLKA_TILEIMAGEVIEW_H_
#define _POLKA_TILEIMAGEVIEW_H_

#include <gtkmm/drawingarea.h>
#include <cairomm/surface.h>


namespace Polka {

class Palette;

class TileImageView : public Gtk::DrawingArea 
{
public:
	TileImageView();
	~TileImageView();

	void setImage( Cairo::RefPtr<Cairo::ImageSurface> image );
	void tileUpdated( int x, int y );
	void setSelector( int width, int height = 0, int horgrid = 8, int vergrid = 0 );
	void setSelection( int x, int y );
	
	// signals
	typedef sigc::signal<void, int, int> SignalSelectionChanged;
	SignalSelectionChanged signalSelectionChanged();

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event(GdkEventButton* event);
	virtual bool on_motion_notify_event(GdkEventMotion* event);
	virtual bool on_leave_notify_event(GdkEventCrossing* event);
	
private:
	SignalSelectionChanged m_SignalSelectionChanged;
	Cairo::RefPtr<Cairo::ImageSurface> m_Image;
	int m_Scale;
	int m_SelectWidth, m_SelectHeight;
	int m_GridHor, m_GridVer;
	int m_HoverX, m_HoverY;
	int m_SelectX, m_SelectY;
	
	void drawSelectRect( const Cairo::RefPtr<Cairo::Context>& context, double x, double y, double w, double h );
	void drawHover();
	void drawSelect();
	
};

} // namespace Polka

#endif // _POLKA_TILEIMAGEVIEW_H_
