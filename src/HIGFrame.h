#ifndef _POLKA_HIGFRAME_H_
#define _POLKA_HIGFRAME_H_

#include <gtkmm/frame.h>
#include <gtkmm/alignment.h>

namespace Polka {

/*
 * Gnome HIG frame as created by Glade interface designer
 */

class HIGFrame : public Gtk::Frame
{
public:
	HIGFrame( const Glib::ustring& label );
	~HIGFrame();

	void set_bold( bool value = true );

	virtual void add(Widget& widget);
	void remove();

	Gtk::Widget *get_child();
	const Gtk::Widget *get_child() const;

protected:

private:
	Gtk::Alignment m_Contents;
};

} // namespace Polka 

#endif // _POLKA_HIGFRAME_H_
