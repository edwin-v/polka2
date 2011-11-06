#include "HIGFrame.h"
#include <gtkmm/label.h>

namespace Polka {


HIGFrame::HIGFrame( const Glib::ustring& label )
	: Gtk::Frame(label)
{
	set_shadow_type( Gtk::SHADOW_NONE );
	set_bold();
	m_Contents.set_padding(0,0,12,0);
	Gtk::Frame::add( m_Contents );
}

HIGFrame::~HIGFrame()
{
}

void HIGFrame::set_bold( bool value )
{
	Pango::AttrList attrs;
	Pango::Attribute bold = Pango::Attribute::create_attr_weight( value?Pango::WEIGHT_BOLD:Pango::WEIGHT_NORMAL );
	attrs.insert(bold);
	Gtk::Label *l = dynamic_cast<Gtk::Label*>( get_label_widget() );
	if( l ) l->set_attributes( attrs );
}

void HIGFrame::add(Widget& widget)
{
	m_Contents.add(widget);
}

void HIGFrame::remove()
{
	m_Contents.remove();
}

Gtk::Widget *HIGFrame::get_child()
{
	return m_Contents.get_child();
}

const Gtk::Widget *HIGFrame::get_child() const
{
	return m_Contents.get_child();
}

} // namespace Polka

