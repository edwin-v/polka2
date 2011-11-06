#ifndef _POLKA_TOOLSELECTPANEL_H_
#define _POLKA_TOOLSELECTPANEL_H_

#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>

namespace Polka {

class ToolSelectPanel: public Gtk::VBox
{
public:
	ToolSelectPanel();
	~ToolSelectPanel();

	Glib::SignalProxy0< void > toBrushClicked();

private:
	//Gtk::CheckButton m_FillCheck;
	Gtk::Button m_ToBrush;
};

} // namespace Polka

#endif // _POLKA_TOOLSELECTPANEL_H_
