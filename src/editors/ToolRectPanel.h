#ifndef _POLKA_TOOLRECTPANEL_H_
#define _POLKA_TOOLRECTPANEL_H_

#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>

namespace Polka {

class ToolRectPanel: public Gtk::VBox
{
public:
	ToolRectPanel();
	~ToolRectPanel();

	bool filledRectangle() const;

private:
	Gtk::CheckButton m_FillCheck;
};

} // namespace Polka

#endif // _POLKA_TOOLRECTPANEL_H_
