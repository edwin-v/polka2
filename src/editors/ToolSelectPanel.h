#ifndef _POLKA_TOOLSELECTPANEL_H_
#define _POLKA_TOOLSELECTPANEL_H_

#include <gtkmm/box.h>
#include <gtkmm/radiobutton.h>

namespace Polka {

class ToolSelectPanel: public Gtk::VBox
{
public:
	ToolSelectPanel();
	~ToolSelectPanel();

	Glib::SignalProxy0<void> floatModeChanged();
	Glib::SignalProxy0<void> toBrushClicked();

	void setHasFloating( bool value = true );

	bool copyMode() const;
	bool solidMode() const;

private:
	Gtk::RadioButton m_Copy, m_Cut;
	Gtk::RadioButton m_Solid, m_Transparent;
	Gtk::Button m_ToBrush;
};

} // namespace Polka

#endif // _POLKA_TOOLSELECTPANEL_H_
