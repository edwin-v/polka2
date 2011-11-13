#include "ToolButtonWindow.h"
#include "Palette.h"
#include <gtkmm/image.h>
#include <gtkmm/accelmap.h>
#include <gtkmm/uimanager.h>
#include <iostream>

using namespace std;

namespace Polka {

ToolButtonWindow::ToolButtonWindow( int width )
	: /*EditorWindow("Bmp16CanvasEditor_Toolbox"), */m_Width( width ), m_CurrentTool(-1)
{
//	set_title("Tools");
	
//	set_deletable( false );
//	set_resizable( false );
	//set_skip_taskbar_hint();
	//set_type_hint( Gdk::WINDOW_TYPE_HINT_UTILITY );
	//set_keep_above( true );
	set_border_width(4);

	set_shadow_type( Gtk::SHADOW_NONE );
	set_handle_position( Gtk::POS_TOP );
	set_snap_edge( Gtk::POS_RIGHT );

	// add widget layout
	remove();
	add( m_MainBox );

	m_MainBox.pack_start( m_ButtonGrid, Gtk::PACK_SHRINK );
	//m_MainBox.pack_start( m_Expand );
	m_MainBox.pack_start( m_Pages, Gtk::PACK_EXPAND_WIDGET );
	m_MainBox.set_spacing(4);
	//m_Expand.add( m_Pages );
	
	m_Pages.append_page( m_Empty );
	m_Pages.set_show_border(false);
	m_Pages.set_show_tabs(false);

	show_all_children();
}

ToolButtonWindow::~ToolButtonWindow()
{
}

void ToolButtonWindow::addTool(Glib::RefPtr<Gdk::Pixbuf> icon, Gtk::Widget *w )
{
	// create button from icon
	Gtk::ToggleButton *but = manage( new Gtk::ToggleButton );
	Gtk::Image *img = manage( new Gtk::Image(icon) );
	but->set_image( *img );
	but->set_relief( Gtk::RELIEF_NONE );
	but->signal_toggled().connect( sigc::bind<int>( sigc::mem_fun(*this, &ToolButtonWindow::toolButtonPressed), m_Buttons.size() ) );
	// calc location of button
	int x = m_Buttons.size() % m_Width;
	int y = m_Buttons.size() / m_Width;
	m_ButtonGrid.resize( m_Width, y+1 );
	
	// add button to grid
	m_ButtonGrid.attach( *but, x,x+1, y, y+1, Gtk::SHRINK, Gtk::SHRINK );
	but->show();
	
	// add page widget to pages
	if( w ) {
		if( m_Pages.page_num(*w) == -1 ) {
			m_Pages.append_page( *w );
			w->show();
		}
	}
	
		
	m_Buttons.push_back( but );
	m_PageWidgets.push_back( w );

}

void ToolButtonWindow::activateTool( int id )
{
	if( id >= 0 && id < int(m_Buttons.size()) )
		toolButtonPressed(id);
}

ToolButtonWindow::SignalSelectTool ToolButtonWindow::signalSelectTool()
{
	return m_SignalSelectTool;
}

void ToolButtonWindow::toolButtonPressed( int id )
{

	if( id == m_CurrentTool ) {
		m_CurrentTool = -1;
		m_Pages.set_current_page(0);
		m_SignalSelectTool.emit( -1 );
		return;
	}
	
	// unselect old tool button
	if( m_CurrentTool >= 0 ) m_Buttons[m_CurrentTool]->set_active(false);
	
	// select new page
	if( m_PageWidgets[id] ) {
		// find widget
		int n = m_Pages.page_num( *m_PageWidgets[id] );
		m_Pages.set_current_page(n);
	} else {
		// no extra widget, empty
		m_Pages.set_current_page(0);
	}
	
	// tool selected
	m_CurrentTool = id;
	m_SignalSelectTool.emit( id );
}

} // namespace Polka 
