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


