#ifndef _POLKA_TILECOLOREDITOR_H_
#define _POLKA_TILECOLOREDITOR_H_

#include <gtkmm/drawingarea.h>
#include <Types.h>

namespace Polka {

class Palette;

class TileColorEditor: public Gtk::DrawingArea 
{
public:
	TileColorEditor();
	~TileColorEditor();

	void setColors( int x, int y, const Byte *colors );
	void setPalette( const Palette *palette );
	void setZoomSize( int zoom );
	void setSelectColor( int color );
	
	// signals
	typedef sigc::signal<void, int, int, Byte> SignalColorChanged;
	SignalColorChanged signalColorChanged();
	typedef sigc::signal<void, int> SignalColorSelected;
	SignalColorSelected signalColorSelected();

protected:
	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );
	virtual bool on_button_press_event(GdkEventButton* event);
	virtual bool on_motion_notify_event (GdkEventMotion* event);

private:
	void setColor( int y );

	// signal objects
	SignalColorChanged m_SignalColorChanged;
	SignalColorSelected m_SignalColorSelected;
	
	Byte m_Colors[8];
	int m_X, m_Y;
	const Palette *m_pPalette;
	int m_Size;
	int m_Left, m_Top;
	int m_Select;
	bool m_FgDown;
};

} // namespace Polka

#endif // _POLKA_TILECOLOREDITOR_H_
