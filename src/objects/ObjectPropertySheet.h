#ifndef _POLKA_OBJECTPROPERTYSHEET_H_
#define _POLKA_OBJECTPROPERTYSHEET_H_

#include <gtkmm/box.h>

namespace Polka {

class Canvas;

class ObjectPropertySheet: public Gtk::VBox
{
public:
	ObjectPropertySheet( const Glib::ustring& _name );
	virtual ~ObjectPropertySheet();

	const Glib::ustring& pageName() const;

	// signals
	typedef sigc::signal<void, bool> SignalSetModified;
	SignalSetModified signalSetModified();

	// actions
	virtual void apply() const = 0;
	virtual void reset() const = 0;

protected:
	void setModified( bool value );

private:
	SignalSetModified m_SignalSetModified;
	Glib::ustring m_Name;
};

} // namespace Polka

#endif // _POLKA_OBJECTPROPERTYSHEET_H_
