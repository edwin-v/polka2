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

#include "EditorWindow.h"
#include "Editor.h"
#include "Settings.h"
#include <gtkmm/widget.h>
#include <iostream>

namespace Polka {


EditorWindow::EditorWindow( const std::string& _id )
	: m_Id(_id), m_Editor(0)
{
	// main window options
	set_type_hint( Gdk::WINDOW_TYPE_HINT_UTILITY );
	set_skip_taskbar_hint();
	set_border_width(5);
	
	// add main layout
	add(m_MainBox);
}

EditorWindow::~EditorWindow()
{
	// remove widget
	setEditor(0);
}

bool EditorWindow::on_delete_event( GdkEventAny */*event*/ )
{
	// workaround for utter failure
	return !get_deletable();
}


void EditorWindow::setEditor( Editor *edt )
{
	if( m_Editor )
		m_MainBox.remove( *m_Editor );
	
	m_Editor = edt;
	
	if( m_Editor )
		m_MainBox.add( *m_Editor );

}

Editor *EditorWindow::editor()
{
	return m_Editor;
}

void EditorWindow::on_show()
{
	Settings::get().getWindowGeometry( m_Id, "Geometry", *this, -1, -1, -1, -1 );
	Gtk::Window::on_show();
}

void EditorWindow::on_hide()
{
	Settings::get().setWindowGeometry( m_Id, "Geometry", *this );
	Gtk::Window::on_hide();
}

} // namespace Polka 


