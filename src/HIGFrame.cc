/*
	Copyright (C) 2013 Edwin Velds

    This file is part of Polka 2.

    Polka 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Polka 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Polka 2.  If not, see <http://www.gnu.org/licenses/>.
*/

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

