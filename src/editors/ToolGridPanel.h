#ifndef _POLKA_TOOLGRIDPANEL_H_
#define _POLKA_TOOLGRIDPANEL_H_

#include <gtkmm/box.h>
#include <gtkmm/togglebutton.h>

namespace Polka {

class ToolGridPanel: public Gtk::VBox
{
public:
	ToolGridPanel();
	~ToolGridPanel();

	// signals
	typedef sigc::signal<void, int> SignalGridTypeChanged;
	SignalGridTypeChanged signalGridTypeChanged();

private:
	Gtk::ToggleButton m_GridStyleNone, m_GridStyleLines, m_GridStyleDots;
	Gtk::HBox m_GridStyleBox;
	// signal objects
	SignalGridTypeChanged m_SignalGridTypeChanged;
	
	void gridHandler( int type );

};

} // namespace Polka

#endif // _POLKA_TOOLGRIDPANEL_H_
